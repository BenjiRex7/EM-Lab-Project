#include <esp_now.h>
#include <WiFi.h>
#include <Keypad.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeMonoBoldOblique9pt7b.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

#define Password_Length 4
char input[Password_Length];
char pass1[Password_Length] = "310";
char pass2[Password_Length] = "198";

const byte ROW_NUM = 4;
const byte COLUMN_NUM = 3;

char keys[ROW_NUM][COLUMN_NUM] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { 'S', '0', 'E' }
};

byte row_pins[ROW_NUM] = { 13, 12, 14, 27 };
byte col_pins[COLUMN_NUM] = { 26, 25, 33 };


Keypad customKeypad = Keypad(makeKeymap(keys), row_pins, col_pins, ROW_NUM, COLUMN_NUM);


uint8_t broadcastAddress[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

struct variable {
  int Pin = 0;
  int State = 0;
  int preState = 0;
  int onoff = 0;
};
variable UP;
variable DOWN;
variable SELECT;

int scroll = 0, count = 69;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

String success;

//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
  int onoff1 = 8;
  int onoff2 = 8;
  int state1 = 0;
  int state2 = 0;
  int Stateask1 = 0;
  int Stateask2 = 0;
};

struct_message Send;
struct_message incomingReadings;

esp_now_peer_info_t peerInfo;
int centerX = (SCREEN_WIDTH - 3) / 2;  // 3 accounts for the width of each underscore

// Function Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status == 0) {
    success = "Delivery Success :)";
  } else {
    success = "Delivery Fail :(";
  }
}

// Function Callback when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  Serial.print("Bytes received: ");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  // Pin declaration
  {
    UP.Pin = 19;
    DOWN.Pin = 5;
    SELECT.Pin = 18;
    pinMode(UP.Pin, INPUT_PULLUP);
    pinMode(DOWN.Pin, INPUT_PULLUP);
    pinMode(SELECT.Pin, INPUT_PULLUP);
  }

  // ESP-NOW DEFAULT
  {
    // Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
      Serial.println("Error initializing ESP-NOW");
      return;
    }
    esp_now_register_send_cb(OnDataSent);

    // Register peer
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    // Add peer
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
      Serial.println("Failed to add peer");
      return;
    }
    // Register for a callback function that will be called when data is received
    esp_now_register_recv_cb(OnDataRecv);
  }

  // Starting Screen
  {
    display.setFont(&FreeMonoBoldOblique9pt7b);
    display.clearDisplay();
    display.display();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(23, 23);
    display.println("Welcome to the");
    display.setCursor(5, 32);
    display.print("Fortress Of Solitude");
    display.display();
    delay(1000);
    display.clearDisplay();
    display.display();
    display.setTextSize(1);
    scroll = 0;
    Serial.print("scroll: ");
    Serial.println(scroll);
    delay(250);
  }
}

void loop() {

  char key = customKeypad.getKey();
  //Password Entering
  {
    if (key) {
      if (key == '0') {
        TV_HOME();
        scroll = 1;
      }
      if (key == 'S') {
        clearData();
      } else if (strlen(input) < Password_Length - 1) {
        input[strlen(input)] = key;
        if (strlen(input) == 1) {
          onestar();
        } else if (strlen(input) == 2) {
          twostar();
        } else if (strlen(input) == 3) {
          threestar();
        }
        display.setTextSize(1);
      }
    }
  }

  //If not Password screen
  {
    if (scroll != 0) {
      UP.State = digitalRead(UP.Pin);
      DOWN.State = digitalRead(DOWN.Pin);
      SELECT.State = digitalRead(SELECT.Pin);
      //UP
      {
        if (UP.State == LOW && UP.preState == 0) {
          count--;
          UP.preState = 1;
          scroll = (count % 3) + 1;
          Serial.print("scroll: ");
          Serial.println(scroll);

        } else if (UP.State == HIGH) {
          UP.preState = 0;
        }
      }
      //DOWN Button
      {
        if (DOWN.State == LOW && DOWN.preState == 0) {
          DOWN.preState = 1;
          count++;
          scroll = (count % 3) + 1;
          Serial.print("scroll: ");
          Serial.println(scroll);

        } else if (DOWN.State == HIGH) {
          DOWN.preState = 0;
        }
      }
      //SELECT Button
      {
        if (SELECT.State == LOW && SELECT.preState == 0) {
          SELECT.preState = 1;
          if (scroll == 1) {
            if (Send.Stateask1 == 8) {
              Send.onoff1 = 1;
              Send.Stateask1 = 0;
            }
            if (Send.Stateask2 == 8) {
              Send.onoff2 = 1;
              Send.Stateask2 = 0;
            }
            esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&Send, sizeof(Send));
            if (result == ESP_OK) {
              Serial.println("Sent with success");
            } else {
              Serial.println("Error sending the data");
            }
            scroll = 0;
            Serial.print("scroll: ");
            Serial.println(scroll);
            display.clearDisplay();
            display.display();
          } else if (scroll == 2) {
            if (Send.Stateask1 == 8) {
              Send.onoff1 = 0;
              Send.Stateask1 = 0;
            }
            if (Send.Stateask2 == 8) {
              Send.onoff2 = 0;
              Send.Stateask2 = 0;
            }
            esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&Send, sizeof(Send));
            if (result == ESP_OK) {
              Serial.println("Sent with success");
            } else {
              Serial.println("Error sending the data");
            }
            scroll = 0;
            display.clearDisplay();
            display.display();
          } else if (scroll == 3) {
            scroll = 0;
            count = 69;
            Serial.print("scroll: ");
            Serial.println(scroll);
            display.clearDisplay();
            display.display();
          }
        } else if (SELECT.State == HIGH) {
          SELECT.preState = 0;
        }
      }
    }

    //Password Compare
    {
      if (strlen(input) == Password_Length - 1) {
        if (strcmp(pass1, input) == 0) {
          Serial.println("Light 1 Control");
          Light1Cont();
          Send.Stateask1 = 1;
          Send.onoff1 = 8;
          esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&Send, sizeof(Send));
          if (result == ESP_OK) {
            Serial.println("Sent with success");
          } else {
            Serial.println("Error sending the data");
          }
          scroll = 1;
          count = 69;
          Serial.print("scroll: ");
          Serial.println(scroll);
        } else if (strcmp(pass2, input) == 0) {
          Serial.println("Light 2 Control");
          Light2Cont();
          Send.Stateask2 = 1;
          Send.onoff2 = 8;
          esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&Send, sizeof(Send));
          if (result == ESP_OK) {
            Serial.println("Sent with success");
          } else {
            Serial.println("Error sending the data");
          }
          scroll = 1;
          count = 69;
          Serial.print("scroll: ");
          Serial.println(scroll);
        } else {
          wrongpass();
        }
        clearData();
      }
    }

    //TV SCROLL
    if ((Send.Stateask1 != 1) && (Send.Stateask2 != 1)) {
      {
        if (scroll == 1) {
          TV_LEDON();
        } else if (scroll == 2) {
          TV_LEDOFF();
        } else if (scroll == 3) {
          TV_BACK();
        } else if (scroll == 0) {
          TV_HOME();
        }
      }
    }

    //LIGHT1 State display
    {
      if (Send.Stateask1 == 1) {
        GettingState();
        if (incomingReadings.onoff1 == 1) {
          display.clearDisplay();
          display.setFont(&FreeMonoBoldOblique9pt7b);
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(7, 35);  //9,10
          display.print("LIGHT1: ON");
          display.display();
          delay(1500);
          display.clearDisplay();
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(2, 10);  //9,10
          display.print("LIGHT1: ON");
          display.display();
        } else {
          display.clearDisplay();
          display.setFont(&FreeMonoBoldOblique9pt7b);
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(2, 35);  //9,10
          display.print("LIGHT1: OFF");
          display.display();
          delay(1500);
          display.clearDisplay();
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(2, 10);  //9,10
          display.print("LIGHT1: OFF");
          display.display();
        }
        Send.Stateask1 = 8;
      }
    }

    //LIGHT2 State display
    {
      if (Send.Stateask2 == 1) {
        GettingState();
        if (incomingReadings.onoff2 == 1) {
          display.clearDisplay();
          display.setFont(&FreeMonoBoldOblique9pt7b);
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(7, 35);  //9,10
          display.print("LIGHT2: ON");
          display.display();
          delay(1500);
          display.clearDisplay();
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(2, 10);  //9,10
          display.print("LIGHT2: ON");
          display.display();
        } else {
          display.clearDisplay();
          display.setFont(&FreeMonoBoldOblique9pt7b);
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(2, 35);  //9,10
          display.print("LIGHT2: OFF");
          display.display();
          delay(1500);
          display.clearDisplay();
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(2, 10);  //9,10
          display.print("LIGHT2: OFF");
          display.display();
        }
        Send.Stateask2 = 8;
      }
    }
  }
}

void TV_LEDON() {
  display.setFont(&FreeMonoBoldOblique9pt7b);
  display.fillRect(0, 13, 127, 15, WHITE);
  display.setTextColor(BLACK);
  display.setCursor(52, 25);
  display.print("ON");
  display.fillRect(0, 31, 127, 15, BLACK);
  display.setTextColor(WHITE);
  display.setCursor(47, 43);
  display.print("OFF");
  display.fillRect(0, 48, 127, 15, BLACK);
  display.setTextColor(WHITE);
  display.setCursor(41, 60);
  display.print("BACK");
  display.display();
}
void TV_LEDOFF() {
  display.setFont(&FreeMonoBoldOblique9pt7b);
  display.fillRect(0, 13, 127, 15, BLACK);
  display.setTextColor(WHITE);
  display.setCursor(52, 25);
  display.print("ON");
  display.fillRect(0, 31, 127, 15, WHITE);
  display.setTextColor(BLACK);
  display.setCursor(47, 43);
  display.print("OFF");
  display.fillRect(0, 48, 127, 15, BLACK);
  display.setTextColor(WHITE);
  display.setCursor(41, 60);
  display.print("BACK");
  display.display();
}
void TV_BACK() {
  display.setFont(&FreeMonoBoldOblique9pt7b);
  display.fillRect(0, 13, 127, 15, BLACK);
  display.setTextColor(WHITE);
  display.setCursor(52, 25);
  display.print("ON");
  display.fillRect(0, 31, 127, 15, BLACK);
  display.setTextColor(WHITE);
  display.setCursor(47, 43);
  display.print("OFF");
  display.fillRect(0, 48, 127, 15, WHITE);
  display.setTextColor(BLACK);
  display.setCursor(41, 60);
  display.print("BACK");
  display.display();
}
void TV_HOME() {
  display.setFont();
  display.setTextColor(WHITE);
  display.display();
  display.setCursor(22, 12);
  display.setTextSize(1);
  display.print("Enter Password");
  display.setTextSize(2);
  display.setCursor(centerX - 30, 40);
  display.println("_ ");  // print char at said cursor and show *
  display.setCursor(centerX, 40);
  display.println("_ ");  // print char at said cursor and show *
  display.setCursor(centerX + 30, 40);
  display.println("_ ");  // print char at said cursor and show *
}
void GettingState() {
  display.setFont();
  display.setTextSize(1);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 32);
  display.print("Getting State.");
  display.display();
  delay(250);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 32);
  display.print("Getting State..");
  display.display();
  delay(250);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 32);
  display.print("Getting State...");
  display.display();
  delay(250);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 32);
  display.print("Getting State....");
  display.display();
  delay(250);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(20, 32);
  display.print("State Received");
  display.display();
  delay(350);
}
void wrongpass() {
  display.setFont();
  display.setTextSize(1);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(14, 1);
  display.println("Incorrect");
  display.setCursor(20, 22);
  display.println("Password");
  display.setCursor(15, 44);
  display.println("TRY AGAIN");
  display.display();
  delay(3000);
}
void clearData() {
  memset(input, 0, sizeof(input));
  display.clearDisplay();
  display.display();
}
void onestar() {
  display.setTextSize(3);
  display.setCursor(centerX - 30, 40);
  display.println("* ");  // print char at said cursor and show *
  display.display();
}
void twostar() {
  display.setTextSize(3);
  display.setCursor(centerX, 40);
  display.println("* ");  // print char at said cursor and show *
  display.display();
}
void threestar() {
  display.setTextSize(3);
  display.setCursor(centerX + 30, 40);
  display.println("* ");  // print char at said cursor and show *
  display.display();
}
void Light1Cont() {
  display.setFont(&FreeMonoBoldOblique9pt7b);
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(5, 13);
  display.println("WELCOME TO");
  display.setCursor(26, 35);
  display.println("LIGHT 1");
  display.setCursor(0, 57);
  display.println("CONTROL BOX");
  display.display();
  delay(3000);
}
void Light2Cont() {
  display.setFont(&FreeMonoBoldOblique9pt7b);
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(5, 13);
  display.println("WELCOME TO");
  display.setCursor(26, 35);
  display.println("LIGHT 2");
  display.setCursor(0, 57);
  display.println("CONTROL BOX");
  display.display();
  delay(3000);
}
