#include <Arduino.h>

bool buzzerEnabled = false;
bool lastRawButtonState = HIGH;
bool lastButtonState = HIGH;    // HIGH = not pressed, since INPUT_PULLUP

uint32_t lastDebounceTime_ms = 0;
const uint8_t DEBOUNCE_DELAY_ms = 50;

const uint8_t PIN_ANTENNA = 22;
const uint8_t PIN_LED1 = 30;    // LED green 1
const uint8_t PIN_LED2 = 31;    // LED green 2
const uint8_t PIN_LED3 = 32;    // LED yellow
const uint8_t PIN_LED4 = 33;    // LED red 1
const uint8_t PIN_LED5 = 34;    // LED red 2
const uint8_t PIN_BUZZER = 40;  // IN1 driver
const uint8_t PIN_BUTTON = 44;

// function declarations:

void beginAntenna();
void beginLED();
void beginBuzzer();

bool readButton(uint32_t now_ms);

void updateAntenna(uint32_t now_ms);
void updateLED(uint32_t now_ms);
void updateBuzzer(uint32_t now_ms);


void setup() {

  pinMode(PIN_BUTTON, INPUT_PULLUP);

  beginAntenna();
  beginLED();
  beginBuzzer();
}

void loop() {
  uint32_t now_ms = millis();

  readButton(now_ms);

  updateAntenna(now_ms);
  updateLED(now_ms);
  updateBuzzer(now_ms);
}

// function definitions:
void beginAntenna() {

}

void beginLED() {

}

void beginBuzzer() {

}

bool readButton(uint32_t now_ms) {
  bool rawState = digitalRead(PIN_BUTTON);

  if (rawState!= lastRawButtonState) {
    lastDebounceTime_ms = now_ms; // reading changed, restart the debounce window
  }

  if ((now_ms - lastDebounceTime_ms) >= DEBOUNCE_DELAY_ms) {
    if (rawState != lastButtonState) {  // stable AND different from last confirmed state
      lastButtonState = rawState;
      if (lastButtonState == LOW) {
        buzzerEnabled = !buzzerEnabled;
      }
    }
  }

  lastRawButtonState = rawState;  // update for next comparison
  return buzzerEnabled;
}

void updateAntenna(uint32_t now_ms) {

}

void updateLED(uint32_t now_ms) {

}

void updateBuzzer(uint32_t now_ms) {

}