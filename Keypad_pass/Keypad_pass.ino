
#include <Wire.h>
#include <Keypad.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define Password_Length 4
char input[Password_Length];
char pass1[Password_Length] = "131";
char pass2[Password_Length] = "147";

const byte ROW_NUM = 4;
const byte COLUMN_NUM = 3;

char keys[ROW_NUM][COLUMN_NUM] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { 'S', '0', 'E' }
};

byte row_pins[ROW_NUM] = { 32, 33, 25, 26 };
byte col_pins[COLUMN_NUM] = { 27, 14, 12 };

Keypad customKeypad = Keypad(makeKeymap(keys), row_pins, col_pins, ROW_NUM, COLUMN_NUM);

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
int centerX = (SCREEN_WIDTH - 3) / 2;  // 3 accounts for the width of each underscore

#define G 23
#define W 13

void setup() {
  Serial.begin(115200);
  SPI.begin();
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  pinMode(G, OUTPUT);
  pinMode(W, OUTPUT);
  digitalWrite(G, HIGH);
  digitalWrite(W, HIGH);
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
  delay(250);
}

void loop() {
  digitalWrite(G, LOW);
  digitalWrite(W, LOW);
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
  // display.setCursor(89, 40);
  // display.println("_ ");  // print char at said cursor and show *
  display.display();
  char key = customKeypad.getKey();
  if (key) {
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

  if (strlen(input) == Password_Length - 1) {
    if (strcmp(pass1, input) == 0) {
      digitalWrite(G, HIGH);
      digitalWrite(W, LOW);
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(23, 30);
      display.print("PLAYER 1");
      display.display();
      delay(2500);
      digitalWrite(G, LOW);
      clearData();
    } else if (strcmp(pass2, input) == 0) {
      digitalWrite(G, LOW);
      digitalWrite(W, HIGH);
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(23, 30);
      display.print("PLAYER 2");
      display.display();
      delay(2500);
      digitalWrite(W, LOW);
      clearData();
    } else {
      digitalWrite(G, HIGH);
      digitalWrite(W, HIGH);
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(23, 30);
      display.print("Access Denied!");
      display.display();
      delay(2500);
      digitalWrite(W, LOW);
      clearData();
    }
  }
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