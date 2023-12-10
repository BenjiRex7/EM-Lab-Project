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
  display.drawRect(0, 13, 117, 15, WHITE);
  display.setCursor(2, 25);
  display.print("ON         ");
  display.setCursor(2, 43);
  display.print("OFF         ");
  display.setCursor(2, 60);
  display.print("BACK        ");
  display.display();
  delay(1000);

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(2, 10);
  display.print("LIGHT1: ON");
  display.setCursor(2, 25);
  display.print("ON         ");
  display.drawRect(0, 31, 117, 15, WHITE);
  display.setCursor(2, 43);
  display.print("OFF         ");
  display.setCursor(2, 60);
  display.print("BACK        ");
  display.display();
  delay(1000);

  display.clearDisplay();
  display.setCursor(2, 10);
  display.print("LIGHT1: ON");
  display.setCursor(2, 25);
  display.print("ON         ");
  display.setCursor(2, 43);
  display.print("OFF         ");
  display.drawRect(0, 48, 117, 15, WHITE);
  display.setCursor(2, 60);
  display.print("BACK        ");
  display.display();
  delay(1000);
}

void boxclose() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(2, 10);
  display.print("LIGHT1: OFF");
  display.fillRect(0, 13, 117, 15, WHITE);
  display.setCursor(2, 25);
  display.setTextColor(BLACK);
  display.print("ON         ");
  display.setCursor(2, 43);
  display.setTextColor(WHITE);
  display.print("OFF         ");
  display.setCursor(2, 60);
  display.print("BACK        ");
  display.display();
  delay(1000);

  display.clearDisplay();
  display.setCursor(2, 10);
  display.print("LIGHT1: ON");
  display.setCursor(2, 25);
  display.setTextColor(WHITE);
  display.print("ON         ");
  display.setCursor(2, 43);
  display.fillRect(0, 31, 117, 15, WHITE);
  display.setTextColor(BLACK);
  display.print("OFF         ");
  display.setCursor(2, 60);
  display.setTextColor(WHITE);
  display.print("BACK        ");
  display.display();
  delay(1000);

  display.clearDisplay();
  display.setCursor(2, 10);
  display.print("LIGHT1: ON");
  display.setCursor(2, 25);
  display.setTextColor(WHITE);
  display.print("ON         ");
  display.setCursor(2, 43);
  display.print("OFF         ");
  display.fillRect(0, 48, 117, 15, WHITE);
  display.setTextColor(BLACK);
  display.setCursor(2, 60);
  display.print("BACK        ");
  display.display();
  delay(1000);
}

void Arrow() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(2, 10);
  display.print("LIGHT1: ON");
  display.setCursor(2, 25);
  display.print(">ON<         ");
  display.setCursor(0, 43);
  display.print(" OFF         ");
  display.setCursor(0, 60);
  display.print(" BACK        ");
  display.display();
  delay(1000);

  display.clearDisplay();
  display.setCursor(2, 10);
  display.print("LIGHT1: ON");
  display.setCursor(2, 25);
  display.print(" ON         ");
  display.setCursor(0, 43);
  display.print(">OFF<         ");
  display.setCursor(0, 60);
  display.print(" BACK        ");
  display.display();
  delay(1000);

  display.clearDisplay();
  display.setCursor(2, 10);
  display.print("LIGHT1: ON");
  display.setCursor(2, 25);
  display.print(" ON         ");
  display.setCursor(0, 43);
  display.print(" OFF         ");
  display.setCursor(0, 60);
  display.print(">BACK<        ");
  display.display();
  delay(1000);
}
