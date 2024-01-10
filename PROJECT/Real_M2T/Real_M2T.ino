#include <esp_now.h>
#include <WiFi.h>
#define ledPin1 1
#define ledPin2 2
uint8_t broadcastAddress[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
struct variable {
  int Pin = 0;
  int State = 0;
  int preState = 0;
  int onoff = 0;
  int count = 0;
};
variable Button1;
variable Button2;
int incoming_count;
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
  //X asking for state Of LIGHT 1 and A returning state
  {
    if (incomingReadings.Stateask1 == 1) {
      Send.state1 = Send.onoff1;
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&Send, sizeof(Send));
      if (result == ESP_OK) {
        Serial.println("Sent with success");
      } else {
        Serial.println("Error sending the data");
      }
    }
  }
  //X asking for state Of LIGHT 1 and A returning state
  {
    if (incomingReadings.Stateask2 == 1) {
      Send.state2 = Send.onoff2;
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&Send, sizeof(Send));
      if (result == ESP_OK) {
        Serial.println("Sent with success");
      } else {
        Serial.println("Error sending the data");
      }
    }
  }
  //control light 1 from X
  {
    if (incomingReadings.onoff1 == 1) {
      Send.onoff1 = 1;
      Button1.count++;
    } else if (incomingReadings.onoff1 == 0) {
      digitalWrite(ledPin1, HIGH);
    }
  }
  //control light 2 from X
  {
    if (incomingReadings.onoff2 == 1) {
      Send.onoff2 = 1;
      Button2.count++;
    } else if (incomingReadings.onoff2 == 0) {
      digitalWrite(ledPin2, HIGH);
    }
  }
}
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  Button1.Pin = 13;
  Button2.Pin = 21;
  pinMode(Button1.Pin, INPUT_PULLUP);
  pinMode(Button2.Pin, INPUT_PULLUP);
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
  updateLIGHTS();
  delay(100);
}
void getReadings() {
  Button1.State = digitalRead(Button1.Pin);
  Button2.State = digitalRead(Button2.Pin);
  //Button 1
  {
    if (Button1.State == LOW && Button1.preState == 0) {
      if (incomingReadings.onoff1 == 0) {
        digitalWrite(ledPin1, HIGH);
        Send.onoff1 = 0;
      } else {
        Button1.count++;
        Send.onoff1 = Button1.count % 2;
        Button1.preState = 1;
      }
    } else if (Button1.State == HIGH) {
      Button1.preState = 0;
    }
  }
  //Button 2
  {
    if (Button2.State == LOW && Button2.preState == 0) {
      Button2.preState = 1;
      Button2.count++;
      Send.onoff2 = Button2.count % 2;
    } else if (Button2.State == HIGH) {
      Button2.preState = 0;
    }
  }
}
void updateLIGHTS() {
  //Light 1
  {
    if (incomingReadings.onoff1 != 0) {
      if (Send.onoff1 == 1) {
        digitalWrite(ledPin1, LOW);
      } else {
        digitalWrite(ledPin1, HIGH);
      }
    } else if (incomingReadings.onoff1 == 0) {
      Send.onoff1 = 0;
    }
  }
  //Light 1
  {
    if (incomingReadings.onoff2 != 0) {
      if (Send.onoff2 == 1) {
        digitalWrite(ledPin2, LOW);
      } else {
        digitalWrite(ledPin2, HIGH);
      }
    } else if (incomingReadings.onoff2 == 0) {
      Send.onoff2 = 0;
    }
  }
}