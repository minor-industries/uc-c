#if defined(MI_CUSTOM_BOARD)

#include <Arduino.h>

#define LED_PIN 7

void setupLED() {
    g_APinDescription;
    pinMode(LED_PIN, OUTPUT);
}

void on() {
    digitalWrite(LED_PIN, HIGH);
}

void off() {
    digitalWrite(LED_PIN, LOW);
}

#endif