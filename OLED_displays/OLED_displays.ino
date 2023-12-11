#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeMonoBoldOblique9pt7b.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
int x = 0;
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  // delay(2000);
  display.setFont(&FreeMonoBoldOblique9pt7b);
  display.setTextSize(1);
}

void loop() {
  boxopen();
  boxclose();
  Arrow();
}

void boxopen() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(2, 10);
  display.print("LIGHT1: ON");
  display.drawRect(1, 13, 127, 15, WHITE);
  display.setCursor(52, 25);
  display.print("ON");
  display.setCursor(47, 43);
  display.print("OFF         ");
  display.setCursor(41, 60);
  display.print("BACK        ");
  display.display();
  delay(1000);

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(2, 10);
  display.print("LIGHT1: ON");
  display.setCursor(52, 25);
  display.print("ON         ");
  display.drawRect(1, 31, 127, 15, WHITE);
  display.setCursor(47, 43);
  display.print("OFF         ");
  display.setCursor(41, 60);
  display.print("BACK        ");
  display.display();
  delay(1000);

  display.clearDisplay();
  display.setCursor(2, 10);
  display.print("LIGHT1: ON");
  display.setCursor(52, 25);
  display.print("ON         ");
  display.setCursor(47, 43);
  display.print("OFF         ");
  display.drawRect(1, 48, 127, 15, WHITE);
  display.setCursor(41, 60);
  display.print("BACK        ");
  display.display();
  delay(1000);
}

void boxclose() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(2, 10); //9,10
  display.print("LIGHT1: OFF");
  display.fillRect(0, 13, 127, 15, WHITE);
  display.setTextColor(BLACK);
  display.setCursor(52, 25);
  display.print("ON");
  display.setTextColor(WHITE);
  display.setCursor(47, 43);
  display.print("OFF");
  display.setCursor(41, 60);
  display.print("BACK");
  display.display();
  delay(1000);

  display.clearDisplay();
  display.setCursor(2, 10);
  display.print("LIGHT1: ON");
  display.setTextColor(WHITE);
  display.setCursor(52, 25);
  display.print("ON");
  display.fillRect(0, 31, 127, 15, WHITE);
  display.setTextColor(BLACK);
  display.setCursor(47, 43);
  display.print("OFF");
  display.setTextColor(WHITE);
  display.setCursor(41, 60);
  display.print("BACK");
  display.display();
  delay(1000);

  display.clearDisplay();
  display.setCursor(2, 10);
  display.setTextColor(WHITE);
  display.print("LIGHT1: ON");
  display.setTextColor(WHITE);
  display.setCursor(52, 25);
  display.print("ON");
  display.setCursor(47, 43);
  display.print("OFF");
  display.fillRect(0, 48, 127, 15, WHITE);
  display.setTextColor(BLACK);
  display.setCursor(41, 60);
  display.print("BACK");
  display.display();
  delay(1000);
}

void Arrow() {
  display.setTextColor(WHITE);
  display.clearDisplay();
  display.setCursor(9, 10);
  display.print("LIGHT1: ON");
  display.setCursor(42, 25);
  display.print(">ON<");
  display.setCursor(48, 43);
  display.print("OFF");
  display.setCursor(42, 60);
  display.print("BACK");
  display.display();
  delay(1000);
  display.clearDisplay();
  display.setCursor(9, 10);
  display.print("LIGHT1: ON");
  display.setCursor(53, 25);
  display.print("ON");
  display.setCursor(37, 43);
  display.print(">OFF<");
  display.setCursor(42, 60);
  display.print("BACK");
  display.display();
  delay(1000);
  display.clearDisplay();
  display.setCursor(9, 10);
  display.print("LIGHT1: ON");
  display.setCursor(53, 25);
  display.print("ON");
  display.setCursor(48, 43);
  display.print("OFF");
  display.setCursor(31, 60);
  display.print(">BACK<");
  display.display();
  delay(1000);
}
