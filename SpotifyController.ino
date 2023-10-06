#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_ADDR   0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

const int playPauseButton = 9;
const int nextButton = 3;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(playPauseButton, INPUT_PULLUP);
  pinMode(nextButton, INPUT_PULLUP);

  // Initialize OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("SSD1306 allocation failed");
    for(;;);
  }
  display.clearDisplay();

}

void writeSongTitle(String title) {
  display.clearDisplay();
  display.setTextSize(1);  // Set text size. Increase for larger text.
  display.setTextColor(SSD1306_WHITE);  // Set text color
  display.setCursor(0,0);  // Start at top-left corner
  display.println(title);
  display.display();
}

void loop() {
  // put your main code here, to run repeatedly:

  if (Serial.available()) {
    String songTitle = Serial.readStringUntil('\n');  // Read the incoming data until a newline is received
    writeSongTitle(songTitle);
  }

  if (digitalRead(playPauseButton) == LOW) {
    Serial.println("PLAY_PAUSE");
    delay(500);
  }
  else if (digitalRead(nextButton) == LOW) {
    Serial.println("NEXT_TRACK");
    delay(500);
  }
}
