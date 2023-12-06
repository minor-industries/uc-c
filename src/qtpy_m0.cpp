#if defined(ADAFRUIT_QTPY_M0)

#include "Adafruit_NeoPixel.h

#define NEO 11
#define NEO_POWER 12

Adafruit_NeoPixel *strip;

void setupLED() {
    strip = new Adafruit_NeoPixel(1, NEO, NEO_GRB + NEO_KHZ800);

    pinMode(strip->getPin(), OUTPUT);
    digitalWrite(NEO_POWER, HIGH);
    pinMode(NEO, OUTPUT);

    strip->begin();
    strip->setBrightness(50);
    strip->show();
}

void on() {
    strip->setPixelColor(0, 0xFF0000);
    strip->show();
}

void off() {
    strip->setPixelColor(0, 0x000000);
    strip->show();
}

#endif