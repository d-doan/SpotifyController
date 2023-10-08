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

// calculates display center given song title to write to
int getCenteredXCoord(String text) {
  int16_t x1, y1;
  uint16_t w, h;
  // get the boundary of the text
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h); 

  return (display.width() - w) / 2;
}

// writes given title to display
void writeSongTitle(String combinedSong) {
  int separatorIndex = combinedSong.indexOf('|');
  String title = combinedSong.substring(0, separatorIndex);
  String artist = combinedSong.substring(separatorIndex + 1);

  display.clearDisplay();
  display.setTextSize(2);  
  display.setTextColor(SSD1306_WHITE);

  int titleX = getCenteredXCoord(title);
  display.setCursor(titleX, 0);  // 10 is an arbitrary y-coordinate, adjust as needed
  display.println(title);

  display.setTextSize(1); 
  int artistX = getCenteredXCoord(artist);
  display.setCursor(artistX, 32);  // 30 is an arbitrary y-coordinate, adjust based on title's display height
  display.println(artist);

  display.display();
}

void loop() {
  if (Serial.available()) {
    // Read the incoming data until a newline is received
    String songTitle = Serial.readStringUntil('\n');
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
