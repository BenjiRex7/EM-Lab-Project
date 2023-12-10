#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeMono9pt7b.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

#define BUTTON_UP 5     // Button for moving up
#define BUTTON_DOWN 18  // Button for moving down
#define BUTTON_SEL 17   // Button for moving left

struct variable {
  int Pin = 0;
  int State = 0;
  int count = 0;
  int preState = 0;
  int onoff = 0;
};
variable UP;
variable DOWN;
variable SELECT;
int scroll = 1, HOMESCREEN = 1;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  UP.Pin = 5;
  DOWN.Pin = 18;
  SELECT.Pin = 19;
  pinMode(UP.Pin, INPUT_PULLUP);
  pinMode(DOWN.Pin, INPUT_PULLUP);
  pinMode(SELECT.Pin, INPUT_PULLUP);

  display.setFont(&FreeMono9pt7b);
  display.clearDisplay();
  display.fillRect(0, 0, 130, 22, WHITE);
  display.setTextColor(BLACK);
  display.setCursor(5, 15);
  display.print("Light 1         ");
  display.setTextColor(WHITE);
  display.setCursor(5, 50);
  display.print("Light 2        ");
  display.display();
}

void loop() {
  UP.State = digitalRead(UP.Pin);
  DOWN.State = digitalRead(DOWN.Pin);
  SELECT.State = digitalRead(SELECT.Pin);

  if (UP.State == LOW && UP.preState == 0) {
    UP.count++;
    UP.preState = 1;
    scroll = (UP.count % 2) + 1;
  } else if (UP.State == HIGH) {
    UP.preState = 0;
  }
  if (DOWN.State == LOW && DOWN.preState == 0) {
    DOWN.preState = 1;
    DOWN.count++;
    scroll = (DOWN.count % 2) + 1;
  } else if (DOWN.State == HIGH) {
    DOWN.preState = 0;
  }
  if (SELECT.State == LOW && SELECT.preState == 0) {
    SELECT.preState = 1;
    if (HOMESCREEN == 1) {
      if (scroll == 1) {
        LED1SELECT();
        HOMESCREEN = 0;
      } else if (scroll == 2) {
        LED1SELECT();
        HOMESCREEN = 0;
      } 
    }
  } else if (SELECT.State == HIGH) {
    SELECT.preState = 0;
  }
  if (HOMESCREEN == 1) {
    if (scroll == 1) {
      LED1ACTIVE();
    } else if (scroll == 2) {
      LED2ACTIVE();
    }
  }
  Serial.println(scroll);
}

void LED1ACTIVE() {
  display.clearDisplay();
  display.fillRect(0, 0, 130, 22, WHITE);
  display.setTextColor(BLACK);
  display.setCursor(5, 15);
  display.print("Light 1        ");
  display.setTextColor(WHITE);
  display.setCursor(5, 50);
  display.print("Light 2        ");
  display.display();
}
void LED2ACTIVE() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(5, 15);
  display.print("Light 1        ");
  display.fillRect(0, 33, 130, 22, WHITE);
  display.setTextColor(BLACK);
  display.setCursor(5, 50);
  display.print("Light 2        ");
  display.display();
}
void LED1SELECT() {
  display.setFont();
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("STATE...      ");
  display.setTextColor(BLACK, WHITE);
  // display.setCursor(0, 20);
  display.print("ON         ");
  display.display();
  display.setTextColor(WHITE);  // 'inverted' text
  // display.setCursor(0, 30);
  display.print("OFF         ");
  display.display();
  display.setTextColor(WHITE);  // 'inverted' text
  // display.setCursor(0, 40);
  display.print("BACK        ");
  display.display();
}
