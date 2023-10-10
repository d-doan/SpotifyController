#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_ADDR   0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

const int playPauseButton = 9;
const int nextButton = 3;

const int SCROLL_SPEED = 1; // Number of pixels to shift the text to the left each frame
int scrollPosition; // Don't initialize here; will be set within the function

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

  // Serial.print("Text Measured: '");
  // Serial.print(text);
  // Serial.println("'");

  // Serial.print("x1: "); Serial.println(x1);
  // Serial.print("y1: "); Serial.println(y1);
  // Serial.print("w: "); Serial.println(w);
  // Serial.print("h: "); Serial.println(h);
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

    // Serial.print("title: '"); 
    // Serial.print(combined);
    // Serial.println("'");
    
    int titleWidth = getTextWidth(title, 2);
    // Serial.print("Title Width: ");
    // Serial.println(titleWidth);

    // TODO replace if condition with titleWidth > SCREEN_WIDTH

    // titleWidth truncates at screen size 128, char size is 12px, so start scrolling
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
    display.setCursor(artistX, 32);  // Adjust the y-coordinate as needed to position the artist's name correctly.
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

  // String testTitle = "Test Song'";
  // int width = getTextWidth(testTitle, 2);
  // Serial.print("Width of '");
  // Serial.print(testTitle);
  // Serial.print("' is: ");
  // Serial.println(width);

  // display.clearDisplay();
  // display.setTextSize(2);
  // display.setTextColor(SSD1306_WHITE);
  // display.setCursor(0,0);
  // display.print(testTitle);
  // display.display();
}

void loop() {
  // Read the incoming data if available and update the current song title
  if (Serial.available()) {
    String received = Serial.readStringUntil('\n');
    received.trim();  // Trim any whitespace or newlines
    
    // Serial.print("Received raw data: '");
    // Serial.print(received);
    // Serial.println("'");

    if (received == "HEARTBEAT") {
        lastHeartbeat = millis();
    } else if (received != "PLAY_PAUSE" && received != "NEXT_TRACK") {
        currentSongTitle = received;  // Update song title if not a command
        isDisplayActive = true;
    }

    // Serial.print("Received Song Data: '");
    // Serial.print(currentSongTitle);
    // Serial.println("'");
  }

  // Check if the heartbeat has timed out, and if so, clear the display
  if (millis() - lastHeartbeat > HEARTBEAT_TIMEOUT) {
      display.clearDisplay();
      display.display();
      currentSongTitle = "";  // Clear the current song title as well
      isDisplayActive = false;
  }
  if (isDisplayActive) {
    writeSongTitle(currentSongTitle);
  }

  // if (isDisplayActive && currentSongTitle != "") {
  //   writeSongTitle(currentSongTitle);
  // }

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
