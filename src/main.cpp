#include <Arduino.h>

const uint8_t PIN_ANTENNA = 22;
const uint8_t PIN_LED_LEVEL_1 = 30;    // LED green 1
const uint8_t PIN_LED_LEVEL_2 = 31;    // LED green 2
const uint8_t PIN_LED_LEVEL_3 = 32;    // LED yellow
const uint8_t PIN_LED_LEVEL_4 = 33;    // LED red 1
const uint8_t PIN_LED_LEVEL_5 = 34;    // LED red 2
const uint8_t PIN_BUZZER = 40;         // IN1 driver
const uint8_t PIN_BUTTON = 44;

bool buzzerEnabled = false;
bool lastRawButtonState = HIGH;
bool lastButtonState = HIGH;           // HIGH = not pressed, since INPUT_PULLUP

uint32_t lastDebounceTime_ms = 0;
const uint8_t DEBOUNCE_DELAY_ms = 50;

bool buzzerPinState = false;
uint32_t lastBuzzerToggle_ms = 0;

const uint8_t ON_TIME = 30;
const uint16_t OFF_TIME_LEVEL_2 = 400;
const uint8_t OFF_TIME_LEVEL_3 = 180;
const uint8_t OFF_TIME_LEVEL_4 = 80;
const uint8_t OFF_TIME_LEVEL_5 = 30;

uint8_t currentLevel = 0;

bool lastAntennaState = LOW;
uint16_t pulseCount = 0;
uint32_t windowStart_ms = 0;
const uint32_t PULSE_WINDOW_ms = 200;

const uint8_t LEVEL2_THRESHOLD = 10;
const uint8_t LEVEL3_THRESHOLD = 20;
const uint8_t LEVEL4_THRESHOLD = 30;
const uint8_t LEVEL5_THRESHOLD = 40;
const uint8_t HYSTERESIS_GAP = 3;

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
  pinMode(PIN_ANTENNA, INPUT);
}

void beginLED() {
  pinMode(PIN_LED_LEVEL_1, OUTPUT);
  digitalWrite(PIN_LED_LEVEL_1, LOW);
  pinMode(PIN_LED_LEVEL_2, OUTPUT);
  digitalWrite(PIN_LED_LEVEL_2, LOW);
  pinMode(PIN_LED_LEVEL_3, OUTPUT);
  digitalWrite(PIN_LED_LEVEL_3, LOW);
  pinMode(PIN_LED_LEVEL_4, OUTPUT);
  digitalWrite(PIN_LED_LEVEL_4, LOW);
  pinMode(PIN_LED_LEVEL_5, OUTPUT);
  digitalWrite(PIN_LED_LEVEL_5, LOW);
}

void beginBuzzer() {
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, HIGH);
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
  bool antennaState = digitalRead(PIN_ANTENNA);
  if (antennaState && !lastAntennaState) {
    pulseCount += 1;
    lastAntennaState = antennaState;
  }

  if ((now_ms - windowStart_ms) >= PULSE_WINDOW_ms) {
    // hysteresis
    if (pulseCount >= LEVEL2_THRESHOLD) {
      currentLevel = 2;
    } else if (pulseCount >= LEVEL3_THRESHOLD) {
      currentLevel = 3;
    } else if (pulseCount >= LEVEL4_THRESHOLD) {
      currentLevel = 4;
    } else if (pulseCount >= LEVEL5_THRESHOLD) {
      currentLevel = 5;
    } else if (pulseCount <= LEVEL5_THRESHOLD - HYSTERESIS_GAP) {
      
    }

    pulseCount = 0;
    windowStart_ms = now_ms;
  }
}

void updateLED(uint32_t now_ms) {

}

void updateBuzzer(uint32_t now_ms) {
  if (!buzzerEnabled || currentLevel <= 1) {
    digitalWrite(PIN_BUZZER, HIGH);
    return;
  }

  uint8_t onTime = ON_TIME;
  uint16_t offTime;

  if (currentLevel == 2) {
    offTime = OFF_TIME_LEVEL_2;
  } else if (currentLevel == 3) {
    offTime = OFF_TIME_LEVEL_3;
  } else if (currentLevel == 4) {
    offTime = OFF_TIME_LEVEL_4;
  } else if (currentLevel == 5) {
    offTime = OFF_TIME_LEVEL_5;
  } else {
    offTime = OFF_TIME_LEVEL_2;
  }

  uint16_t threshold = buzzerPinState ? onTime : offTime;

  if ((now_ms - lastBuzzerToggle_ms) >= threshold) {
    buzzerPinState = !buzzerPinState;
    digitalWrite(PIN_BUZZER, buzzerPinState ? LOW : HIGH);
    lastBuzzerToggle_ms = now_ms;
  }
}