#include <esp_now.h>
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeMonoBoldOblique9pt7b.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

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
variable LEFT;
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

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status == 0) {
    success = "Delivery Success :)";
  } else {
    success = "Delivery Fail :(";
  }
}

// Callback when data is received
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
  UP.Pin = 5;
  DOWN.Pin = 18;
  SELECT.Pin = 19;
  LEFT.Pin = 17;
  pinMode(UP.Pin, INPUT_PULLUP);
  pinMode(DOWN.Pin, INPUT_PULLUP);
  pinMode(SELECT.Pin, INPUT_PULLUP);
  pinMode(LEFT.Pin, INPUT_PULLUP);

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

  display.setFont(&FreeMonoBoldOblique9pt7b);
  display.setTextSize(1);
  TV_HOME();
  scroll = 0;
  Serial.print("scroll: ");
  Serial.println(scroll);
}

void loop() {
  //ADD KEYPAD PASSWORD REGOCNIZATION AND THEN SET A VARIABLE FOR EACH BULD ACCESS
  SELECT.State = digitalRead(SELECT.Pin);
  LEFT.State = digitalRead(LEFT.Pin);
  {
    if (scroll != 0) {
      UP.State = digitalRead(UP.Pin);
      DOWN.State = digitalRead(DOWN.Pin);

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

      //DOWN
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
    }

    //SELECT
    {
      if (SELECT.State == LOW && SELECT.preState == 0) {
        SELECT.preState = 1;
        if (scroll == 0) {
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
        } else if (scroll == 1) {
          Send.onoff1 = 1;
          esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&Send, sizeof(Send));
          if (result == ESP_OK) {
            Serial.println("Sent with success");
          } else {
            Serial.println("Error sending the data");
          }
          scroll = 0;
          Serial.print("scroll: ");
          Serial.println(scroll);
        } else if (scroll == 2) {
          Send.onoff1 = 0;
          esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&Send, sizeof(Send));
          if (result == ESP_OK) {
            Serial.println("Sent with success");
          } else {
            Serial.println("Error sending the data");
          }
          scroll = 0;
        } else if (scroll == 3) {
          scroll = 0;
          count = 69;
          Serial.print("scroll: ");
          Serial.println(scroll);
        }
      } else if (SELECT.State == HIGH) {
        SELECT.preState = 0;
      }
    }

    //LEFT
    {
      if (LEFT.State == LOW && LEFT.preState == 0) {
        LEFT.preState = 1;
        if (scroll == 0) {
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
        } else if (scroll == 1) {
          Send.onoff2 = 1;
          esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&Send, sizeof(Send));
          if (result == ESP_OK) {
            Serial.println("Sent with success");
          } else {
            Serial.println("Error sending the data");
          }
          scroll = 0;
          Serial.print("scroll: ");
          Serial.println(scroll);
        } else if (scroll == 2) {
          Send.onoff2 = 0;
          esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&Send, sizeof(Send));
          if (result == ESP_OK) {
            Serial.println("Sent with success");
          } else {
            Serial.println("Error sending the data");
          }
          scroll = 0;
        } else if (scroll == 3) {
          scroll = 0;
          count = 69;
          Serial.print("scroll: ");
          Serial.println(scroll);
        }
      } else if (LEFT.State == HIGH) {
        LEFT.preState = 0;
      }
    }

    //TV SCROLL
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

    //LIGHT1
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
        Send.Stateask1 = 0;
      }
    }

    //LIGHT2
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
        Send.Stateask2 = 0;
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
  display.clearDisplay();
  display.setFont(&FreeMonoBoldOblique9pt7b);
  display.setTextColor(WHITE);
  display.setCursor(10, 43);
  display.print("Hello World!");
  display.display();
}
void GettingState() {
  display.setFont();
  display.setTextSize(1);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 32);
  display.print("Getting State.");
  display.display();
  delay(500);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 32);
  display.print("Getting State..");
  display.display();
  delay(500);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 32);
  display.print("Getting State...");
  display.display();
  delay(500);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 32);
  display.print("Getting State....");
  display.display();
  delay(500);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(20, 32);
  display.print("State Received");
  display.display();
  delay(500);
}