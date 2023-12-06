#include "Adafruit_NeoPixel.h"


unsigned long startTime;

void on();

void off();


bool after(unsigned long timeInSeconds) {
    unsigned long timeInMillis = timeInSeconds * 1000;
    return (millis() - startTime) > timeInMillis;
}

bool before(unsigned long timeInSeconds) {
    return !after(timeInSeconds);
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

void packFloat(float floatVal, uint8_t *buf) {
    union {
        float float_val;
        uint8_t bytes[4];
    } value{
            .float_val =  floatVal
    };

    buf[0] = value.bytes[0];
    buf[1] = value.bytes[1];
    buf[2] = value.bytes[2];
    buf[3] = value.bytes[3];
}

