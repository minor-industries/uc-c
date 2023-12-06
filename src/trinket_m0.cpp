#include <Arduino.h>

#if defined(ADAFRUIT_TRINKET_M0)

#define LED_PIN 3


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