#include <esp_now.h>
#include <WiFi.h>

#define buttonS 22
#define buttonL 23
#define ledPin1 5
uint8_t broadcastAddress[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

int buttonState1 = 0, buttonState2 = 0;
int count_value1 = 0, count_value2 = 0;
int prestate1 = 0, prestate2 = 0;
int onoff1 = 0, onoff2 = 0;
int state = 0;

int incoming_count;

// Variable to store if sending data was successful
String success;

//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
  int count, state, Stateask = 0,StateOther = 0;
  ;
} struct_message;

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
  if (incomingReadings.Stateask == 1) {
    Send.StateOther = onoff2;
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&Send, sizeof(Send));

    if (result == ESP_OK) {
      Serial.println("Sent with success");
    } else {
      Serial.println("Error sending the data");
    }
  } else {
    state = incomingReadings.StateOther;
    incoming_count = incomingReadings.count;
  }
}

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  pinMode(buttonS, INPUT_PULLUP);
  pinMode(buttonL, INPUT_PULLUP);
  pinMode(ledPin1, OUTPUT);

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

void loop() {
  getReadings();
  // Send message via ESP-NOW
  delay(100);
}

void getReadings() {
  buttonState1 = digitalRead(buttonS);
  buttonState2 = digitalRead(buttonL);

  // check if the pushbutton is pressed. If it is, then the buttonState is HIGH:
  if (buttonState1 == LOW && prestate1 == 0) {
    PressButtonS();
    prestate1 = 1;
  } else if (buttonState1 == HIGH) {
    prestate1 = 0;
  }
  if (buttonState2 == LOW && prestate2 == 0) {
    prestate2 = 1;
    PressButtonL();
  } else if (buttonState2 == HIGH) {
    prestate2 = 0;
  }
}
void PressButtonS() {
  Send.Stateask = 1;
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&Send, sizeof(Send));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }
  stateprinting();
}
void PressButtonL() {
  count_value2++;
  onoff2 = count_value2 % 2;
  if (onoff2 == 1) {
    digitalWrite(ledPin1, HIGH);
  } else {
    digitalWrite(ledPin1, LOW);
  }
}
void stateprinting() {
  Serial.print("Searching for state");
  for (int i = 0; i < 5; i++) {
    Serial.print("~ ");
    delay(350);
  }
  Serial.println("~ ");
  delay(1000);
  Send.Stateask = 0;
  Serial.print("State: ");
  Serial.println(state);
  if (state == 1) {
    Serial.println("LED is ON");
  } else if (state == 0) {
    Serial.println("LED is OFF");
  }
}