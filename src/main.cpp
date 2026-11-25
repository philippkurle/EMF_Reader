#include <Arduino.h>

const uint8_t PIN_ANTENNA = A0;
const uint8_t PIN_LED_LEVEL_1 = 30;    // LED green 1
const uint8_t PIN_LED_LEVEL_2 = 31;    // LED green 2
const uint8_t PIN_LED_LEVEL_3 = 32;    // LED yellow
const uint8_t PIN_LED_LEVEL_4 = 33;    // LED red 1
const uint8_t PIN_LED_LEVEL_5 = 34;    // LED red 2
const uint8_t PIN_BUZZER = 40;         // IN1 driver
const uint8_t PIN_BUTTON = 44;

bool buzzerEnabled = false;
bool buzzerPinState = false;
uint32_t lastBuzzerToggle_ms = 0;

bool lastRawButtonState = HIGH;
bool lastButtonState = HIGH;           // HIGH = not pressed, since INPUT_PULLUP
uint32_t lastDebounceTime_ms = 0;
const uint8_t DEBOUNCE_DELAY_ms = 50;

const uint8_t ON_TIME_ms = 30;
const uint16_t OFF_TIME_ms[] = {400, 100, 80, 30};  // index 0..3 = levels 2..5

uint8_t currentLevel = 1;

const uint32_t SAMPLE_WINDOW_ms = 100;  // 5 periods of 50 Hz mains per window

// temporary values
const uint16_t LEVEL2_THRESHOLD = 50;
const uint16_t LEVEL3_THRESHOLD = 183;
const uint16_t LEVEL4_THRESHOLD = 316;
const uint16_t LEVEL5_THRESHOLD = 450;
const uint16_t HYSTERESIS_GAP = 20;

const uint8_t LED_PINS[] = {PIN_LED_LEVEL_1, PIN_LED_LEVEL_2, PIN_LED_LEVEL_3, PIN_LED_LEVEL_4, PIN_LED_LEVEL_5};
const uint8_t NUM_LEDS = 5;

// function declarations:
void beginAntenna();
void beginLED();
void beginBuzzer();

void readButton(uint32_t now_ms);

void updateAntenna(uint32_t now_ms);
void updateLED(void);
void updateBuzzer(uint32_t now_ms);

uint16_t levelUpThreshold(uint8_t level);
uint16_t levelDownThreshold(uint8_t level);

void setup() {
  Serial.begin(115200);

  pinMode(PIN_BUTTON, INPUT_PULLUP);

  beginAntenna();
  beginLED();
  beginBuzzer();
}

void loop() {
  uint32_t now_ms = millis();

  readButton(now_ms);

  updateAntenna(now_ms);
  updateLED();
  updateBuzzer(now_ms);
}

// function definitions:
void beginAntenna() {
  pinMode(PIN_ANTENNA, INPUT);
}

void beginLED() {
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);
  } 
}

void beginBuzzer() {
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);
}

void readButton(uint32_t now_ms) {
  bool rawState = digitalRead(PIN_BUTTON);

  if (rawState != lastRawButtonState) {
    lastDebounceTime_ms = now_ms; // reading changed, restart the debounce window
  }
  lastRawButtonState = rawState;  // raw tracking grouped together

  // stable for the full window AND different from last confirmed state
  if ((now_ms - lastDebounceTime_ms) >= DEBOUNCE_DELAY_ms && rawState != lastButtonState) {
    lastButtonState = rawState;
    if (rawState == LOW) {
      buzzerEnabled = !buzzerEnabled;
    }
  }
}

void updateAntenna(uint32_t now_ms) {
  static uint16_t minValue = 1023;  // start at opposite extremes
  static uint16_t maxValue = 0;
  static uint32_t windowStart_ms = 0;

  uint16_t currentValue = analogRead(PIN_ANTENNA);

  if (currentValue < minValue) {  // two separate ifs: one sample can set both
    minValue = currentValue;
  }
  if (currentValue > maxValue) {
    maxValue = currentValue;
  }

  if ((now_ms - windowStart_ms) >= SAMPLE_WINDOW_ms) {  // evaluate once per window
    uint16_t fieldStrength = maxValue - minValue;       // peak-to-peak amplitude

    // hysteresis
    if (currentLevel < 5 && fieldStrength >= levelUpThreshold(currentLevel + 1)) {
      currentLevel += 1;
    } else if (currentLevel > 1 && fieldStrength <= levelDownThreshold(currentLevel)) {
      currentLevel -= 1;
    }

    Serial.print(F("fieldStrength: "));   // print once per window, not every loop
    Serial.print(fieldStrength);
    Serial.print(F("  level: "));
    Serial.println(currentLevel);

    minValue = 1023;
    maxValue = 0;
    windowStart_ms = now_ms;
  }
}

void updateLED(void) {
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    if (i < currentLevel) {
      digitalWrite(LED_PINS[i], HIGH);
    } else {
      digitalWrite(LED_PINS[i], LOW);
    }
  } 
}

void updateBuzzer(uint32_t now_ms) {
  if (!buzzerEnabled || currentLevel <= 1) {
    digitalWrite(PIN_BUZZER, LOW); // LOW -> off
    buzzerPinState = false;        // keep state variable in sync with the actual pin
    return;
  }

  // level 2..5 maps to index 0..3
  uint16_t phase_ms = buzzerPinState ? ON_TIME_ms : OFF_TIME_ms[currentLevel - 2];

  if ((now_ms - lastBuzzerToggle_ms) >= phase_ms) {
    buzzerPinState = !buzzerPinState;
    digitalWrite(PIN_BUZZER, buzzerPinState);  // true -> HIGH -> on
    lastBuzzerToggle_ms = now_ms;
  }
}

uint16_t levelUpThreshold(uint8_t level) {
  switch (level) {
    case 2:
      return LEVEL2_THRESHOLD;
    case 3:
      return LEVEL3_THRESHOLD;
    case 4:
      return LEVEL4_THRESHOLD;
    case 5:
      return LEVEL5_THRESHOLD;
  }
  return 0;
}

uint16_t levelDownThreshold(uint8_t level) {
  switch (level) {
    case 5:
      return LEVEL5_THRESHOLD - HYSTERESIS_GAP;
    case 4:
      return LEVEL4_THRESHOLD - HYSTERESIS_GAP;
    case 3:
      return LEVEL3_THRESHOLD - HYSTERESIS_GAP;
    case 2:
      return LEVEL2_THRESHOLD - HYSTERESIS_GAP;
  }
  return 0;
}