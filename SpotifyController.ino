#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
// Pin locations for wires
#define PLAY_PAUSE_BUTTON 9
#define NEXT_BUTTON 3

#define OLED_ADDR   0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

// Number of pixels to shift the text to the left each frame
const int SCROLL_SPEED = 1; 
int scrollPosition; 
const int CHAR_WIDTH = 12;
const int MAX_CHAR_DISPLAY_LENGTH = 120;          // Last legal char for a given row given display width is 128

String currentSongTitle = "";
unsigned long lastButtonPressTime = 0;    
const unsigned long BUTTON_DEBOUNCE_DELAY = 500;  // Time in milliseconds to debounce button presses
const unsigned long HEARTBEAT_TIMEOUT = 600000;   // Time before screen turns off without receiving data
unsigned long lastHeartbeat = 0;
bool isDisplayActive = true;

// Calculates display center given song title to write to
int getCenteredXCoord(String text) {
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  return (display.width() - w) / 2;
}

// Calculate text width and set text size
int getTextWidth(String text, uint8_t textSize) {
  display.setTextSize(textSize);
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

  return w;
}

// Writes to display and implements marquee scrolling
void writeSongTitle(String combined) {
    int separatorIndex = combined.indexOf('|');
    String title = combined.substring(0, separatorIndex);
    String artist = combined.substring(separatorIndex + 1);

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);

    int titleWidth = getTextWidth(title, 2);

    if (titleWidth == MAX_CHAR_DISPLAY_LENGTH || titleWidth > SCREEN_WIDTH) {
        // Reset the scroll position if necessary
        if (scrollPosition <= -titleWidth) {
            scrollPosition = SCREEN_WIDTH - CHAR_WIDTH;
        }

        // Determine which characters are currently visible on the screen
        int startIndex = max(0, -(scrollPosition / CHAR_WIDTH));
        int endIndex = min(title.length(), startIndex + (SCREEN_WIDTH / CHAR_WIDTH) + 1);
        
        int x = scrollPosition;
        for (int i = startIndex; i < endIndex; i++) {
            if (x + CHAR_WIDTH <= SCREEN_WIDTH) {  // Make sure the character fits on the display
                display.setCursor(x, 0);
                display.print(title[i]);
                x += CHAR_WIDTH; // Advance for the next character
            }
        }
        scrollPosition -= SCROLL_SPEED;
    } else {
        // If the title fits on the screen, simply center it
        int titleX = getCenteredXCoord(title);
        display.setCursor(titleX, 0);
        display.print(title);
    }

    // Handle the artist as before
    display.setTextSize(1);
    int artistX = getCenteredXCoord(artist);
    display.setCursor(artistX, 32);
    display.println(artist);

    display.display();
}

void setup() {
  Serial.begin(9600);
  pinMode(PLAY_PAUSE_BUTTON, INPUT_PULLUP);
  pinMode(NEXT_BUTTON, INPUT_PULLUP);

  // Initialize OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("SSD1306 allocation failed");
    for(;;);
  }
  display.clearDisplay();

  scrollPosition = SCREEN_WIDTH - CHAR_WIDTH;
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
          scrollPosition = SCREEN_WIDTH - CHAR_WIDTH;
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
  
  if (digitalRead(PLAY_PAUSE_BUTTON) == LOW && currentMillis - lastButtonPressTime > BUTTON_DEBOUNCE_DELAY) {
    Serial.println("PLAY_PAUSE");
    lastButtonPressTime = currentMillis;
  } 
  else if (digitalRead(NEXT_BUTTON) == LOW && currentMillis - lastButtonPressTime > BUTTON_DEBOUNCE_DELAY) {
    Serial.println("NEXT_TRACK");
    lastButtonPressTime = currentMillis;
  }

  // Regulate display refresh rate
  delay(50);
}
