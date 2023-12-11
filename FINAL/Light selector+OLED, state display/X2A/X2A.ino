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
  int count = 0;
};
variable UP;
variable DOWN;
variable SELECT;
variable LEFT;

// Variable to store if sending data was successful
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

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
int scroll = 1, count = 0;

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
  // Init Serial Monitor
  Serial.begin(115200);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  UP.Pin = 5;
  DOWN.Pin = 18;
  SELECT.Pin = 19;
  LEFT.Pin = 17;
  pinMode(UP.Pin, INPUT_PULLUP);
  pinMode(DOWN.Pin, INPUT_PULLUP);
  pinMode(SELECT.Pin, INPUT_PULLUP);
  pinMode(LEFT.Pin, INPUT_PULLUP);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
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
  display.setTextColor(WHITE);
  display.clearDisplay();
  display.display();
}

void loop() {
  getReadings();
  delay(100);
}

void getReadings() {
  UP.State = digitalRead(UP.Pin);
  DOWN.State = digitalRead(DOWN.Pin);
  SELECT.State = digitalRead(SELECT.Pin);

  if (UP.State == LOW && UP.preState == 0) {
    count--;
    UP.preState = 1;
    scroll = (abs(count) % 2) + 1;
    Serial.print("scroll: ");
    Serial.println(scroll);
  } else if (UP.State == HIGH) {
    UP.preState = 0;
  }

  if (DOWN.State == LOW && DOWN.preState == 0) {
    DOWN.preState = 1;
    count++;
    scroll = (count % 2) + 1;
    Serial.print("scroll: ");
    Serial.println(scroll);
  } else if (DOWN.State == HIGH) {
    DOWN.preState = 0;
  }
  if (SELECT.State == LOW && SELECT.preState == 0) {
    SELECT.preState = 1;
    if (scroll == 1) {
      Send.Stateask1 = 1;
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&Send, sizeof(Send));
      if (result == ESP_OK) {
        Serial.println("Sent with success");
      } else {
        Serial.println("Error sending the data");
      }
    }
    if (scroll == 2) {
      Send.Stateask2 = 1;
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&Send, sizeof(Send));
      if (result == ESP_OK) {
        Serial.println("Sent with success");
      } else {
        Serial.println("Error sending the data");
      }
    }
  } else if (SELECT.State == HIGH) {
    SELECT.preState = 0;
  }

  if (scroll == 1) {
    TV_LEDON();
  } else if (scroll == 2) {
    TV_LEDOFF();
  }
  if (Send.Stateask1 == 1) {
    GettingState();
    if (incomingReadings.onoff1 == 1) {
      display.setCursor(0, 32);
      display.clearDisplay();
      display.print("State1: ON");
      display.display();
      delay(1500);
    } else {
      display.setCursor(0, 32);
      display.clearDisplay();
      display.print("State1: OFF");
      display.display();
      delay(1500);
    }
    Send.Stateask1 = 0;
  }
  if (Send.Stateask2 == 1) {
    GettingState();
    if (incomingReadings.onoff2 == 1) {
      display.setCursor(0, 32);
      display.clearDisplay();
      display.print("State2: ON");
      display.display();
      delay(1500);

    } else {
      display.setCursor(0, 32);
      display.clearDisplay();
      display.print("State2: OFF");
      display.display();
      delay(1500);
    }
    Send.Stateask2 = 0;
  }
}

void TV_LEDON() {
  display.setFont(&FreeMonoBoldOblique9pt7b);
  display.clearDisplay();
  display.fillRect(0, 0, 130, 22, WHITE);
  display.setTextColor(BLACK);
  display.setCursor(5, 15);
  display.print("LED 1 STATUS         ");
  display.setTextColor(WHITE);
  display.setCursor(5, 50);
  display.print("LED 2 STATUS         ");
  display.display();
}
void TV_LEDOFF() {
  display.setFont(&FreeMonoBoldOblique9pt7b);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(5, 15);
  display.print("LED 1 STATUS         ");
  display.fillRect(0, 33, 130, 22, WHITE);
  display.setTextColor(BLACK);
  display.setCursor(5, 50);
  display.print("LED 2 STATUS         ");
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
