#include "Adafruit_NeoPixel.h"


Adafruit_NeoPixel *strip;
unsigned long startTime;


bool after(unsigned long timeInSeconds) {
    unsigned long timeInMillis = timeInSeconds * 1000;
    return (millis() - startTime) > timeInMillis;
}

bool before(unsigned long timeInSeconds) {
    return !after(timeInSeconds);
}

void on() {
    strip->setPixelColor(0, 0xFF0000);
    strip->show();
}

void off() {
    strip->setPixelColor(0, 0x000000);
    strip->show();
}

void blink(int repeat, int high, int low) {
    for (int i = 0; i < repeat; ++i) {
        on();
        delay(high);
        off();
        delay(low);
    }
}

void blinkForever(int high, int low) {
    while (true) {
        on();
        delay(high);
        off();
        delay(low);
    }
}
