
const int playPauseButton = 9;
const int nextButton = 3;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(playPauseButton, INPUT_PULLUP);
  pinMode(nextButton, INPUT_PULLUP);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(playPauseButton) == LOW) {
    Serial.println("PLAY_PAUSE");
    delay(500);
  }
  else if (digitalRead(nextButton) == LOW) {
    Serial.println("NEXT_TRACK");
    delay(500);
  }
}
