#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_ADDR   0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

const int playPauseButton = 9;
const int nextButton = 3;

// Number of pixels to shift the text to the left each frame
const int SCROLL_SPEED = 1; 
int scrollPosition; 

String currentSongTitle = "";
unsigned long lastButtonPressTime = 0;
const unsigned long BUTTON_DEBOUNCE_DELAY = 500; // Time in milliseconds to debounce button presses
const unsigned long HEARTBEAT_TIMEOUT = 600000;
unsigned long lastHeartbeat = 0;
bool isDisplayActive = true;

// calculates display center given song title to write to
int getCenteredXCoord(String text) {
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  return (display.width() - w) / 2;
}

// calculate text width and set text size
int getTextWidth(String text, uint8_t textSize) {
  display.setTextSize(textSize);
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

  return w;
}

// writes given title to display
void writeSongTitle(String combined) {
    int separatorIndex = combined.indexOf('|');
    String title = combined.substring(0, separatorIndex);
    String artist = combined.substring(separatorIndex + 1);

    display.clearDisplay();

    // Set text size to 2 for the title
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);

    int titleWidth = getTextWidth(title, 2);
    // titleWidth truncates at screen size 128, char size is 12px
    // therefore need to check for titleWidth being 120 too
    if (titleWidth > SCREEN_WIDTH || titleWidth == 120) {
      // If we've scrolled off the end, reset scrollPosition
      if (scrollPosition + titleWidth <= 0) {
          scrollPosition = SCREEN_WIDTH;
      }
      display.setCursor(scrollPosition, 0);
      display.print(title);
      scrollPosition -= SCROLL_SPEED;
    } else {
        // If the title fits on the screen, center it
        int titleX = getCenteredXCoord(title);
        display.setCursor(titleX, 0);
        display.println(title);
    }

    // Set text size to 1 for the artist 
    display.setTextSize(1);
    int artistX = getCenteredXCoord(artist);
    display.setCursor(artistX, 32);
    display.println(artist);

    display.display();
}

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

  scrollPosition = SCREEN_WIDTH;
}

void loop() {
  // Read the incoming data if available and update the current song title
  if (Serial.available()) {
    String received = Serial.readStringUntil('\n');
    received.trim();

    if (received == "HEARTBEAT") {
        lastHeartbeat = millis();
    } else if (received != "PLAY_PAUSE" && received != "NEXT_TRACK") {
        if (currentSongTitle != received) {
          scrollPosition = SCREEN_WIDTH - 12;
        }
        // Update song title if not a command
        currentSongTitle = received;
        isDisplayActive = true;
    }
  }

  // Check if the heartbeat has timed out, and if so, clear the display
  if (millis() - lastHeartbeat > HEARTBEAT_TIMEOUT) {
      display.clearDisplay();
      display.display();
      currentSongTitle = "";
      isDisplayActive = false;
  }

  if (isDisplayActive) {
    writeSongTitle(currentSongTitle);
  }

  // Non-blocking check for button presses using debounce technique
  unsigned long currentMillis = millis();
  
  if (digitalRead(playPauseButton) == LOW && currentMillis - lastButtonPressTime > BUTTON_DEBOUNCE_DELAY) {
    Serial.println("PLAY_PAUSE");
    lastButtonPressTime = currentMillis;
  } 
  else if (digitalRead(nextButton) == LOW && currentMillis - lastButtonPressTime > BUTTON_DEBOUNCE_DELAY) {
    Serial.println("NEXT_TRACK");
    lastButtonPressTime = currentMillis;
  }

  // regulates display refresh rate
  delay(50);
}
