#include <Arduino.h>

// function declarations:
bool buzzerEnabled = false;

uint8_t PIN_ANTENNA = 22;
uint8_t PIN_LED1 = 30;    // LED green 1
uint8_t PIN_LED2 = 31;    // LED green 2
uint8_t PIN_LED3 = 32;    // LED yellow
uint8_t PIN_LED4 = 33;    // LED red 1
uint8_t PIN_LED5 = 34;    // LED red 2
uint8_t PIN_BUZZER = 40;  // IN1 driver
uint8_t PIN_BUTTON = 44;

void readButton();
void updateAntenna(uint32_t now_ms);
void updateLED(uint32_t now_ms);
void updateBuzzer(uint32_t now_ms);


void setup() {
  uint32_t now_ms = millis();

  pinMode(PIN_BUTTON, INPUT_PULLUP);
}

void loop() {
  uint32_t now_ms = millis();

  updateAntenna(now_ms);
  updateLED(now_ms);
  updateBuzzer(now_ms);
}

// function definitions:
void readButton(){

};
void updateAntenna(uint32_t now_ms){

};
void updateLED(uint32_t now_ms){

};
void updateBuzzer(uint32_t now_ms){

};