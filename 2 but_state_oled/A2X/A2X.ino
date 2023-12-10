#include <esp_now.h>
#include <WiFi.h>

#define Button1 22
#define ledPin1 23
#define Button2 18
#define ledPin2 21
uint8_t broadcastAddress[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

int Buttonstate1 = 0, Buttonstate2 = 0;
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
  pinMode(Button1, INPUT_PULLUP);
  pinMode(Button2, INPUT_PULLUP);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);

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
  Buttonstate1 = digitalRead(Button1);
  Buttonstate2 = digitalRead(Button2);

  // check if the pushbutton is pressed. If it is, then the Buttonstate is HIGH:
  if (Buttonstate1 == LOW && prestate1 == 0) {
    PressButton1();
    prestate1 = 1;
  } else if (Buttonstate1 == HIGH) {
    prestate1 = 0;
  }
  if (Buttonstate2 == LOW && prestate2 == 0) {
    prestate2 = 1;
    PressButton2();
  } else if (Buttonstate2 == HIGH) {
    prestate2 = 0;
  }
}
void PressButton1() {
  count_value1++;
  onoff1 = count_value1 % 2;
  if (onoff1 == 1) {
    digitalWrite(ledPin1, HIGH);
  } else {
    digitalWrite(ledPin1, LOW);
  }
}
void PressButton2() {
  count_value2++;
  onoff2 = count_value2 % 2;
  if (onoff2 == 1) {
    digitalWrite(ledPin2, HIGH);
  } else {
    digitalWrite(ledPin2, LOW);
  }
}