#include <Arduino.h>
#include "Adafruit_NeoPixel.h"

#define LED_PIN 0 // Change this to the pin you connected your LED to
#define NEO_POWER 12
#define NEO 11

Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, NEO, NEO_GRB + NEO_KHZ800);


void setup() {
    pinMode(LED_PIN, OUTPUT);

    pinMode(NEO_POWER, OUTPUT);
    digitalWrite(NEO_POWER, HIGH);

    pinMode(NEO, OUTPUT);

    strip.begin();
    strip.setBrightness(50);
    strip.show();

    strip.setPixelColor(0, 0xFF0000);
    strip.show();
}

void on() {
    strip.setPixelColor(0, 0xFF0000);
    strip.show();
}

void off() {
    strip.setPixelColor(0, 0x000000);
    strip.show();
}

void loop() {
    on();
    delay(1000); // Wait for 1 second
    off();
    delay(1000);
}