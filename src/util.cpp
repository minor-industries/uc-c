#include "Adafruit_NeoPixel.h"
#include "ArduinoLowPower.h"


unsigned long startTime;


bool after(unsigned long timeInSeconds) {
    unsigned long timeInMillis = timeInSeconds * 1000;
    return (millis() - startTime) > timeInMillis;
}

bool before(unsigned long timeInSeconds) {
    return !after(timeInSeconds);
}

void sleep(bool allowDeep, int sleepMillis) {
    if (allowDeep && after(30)) {
        LowPower.sleep(sleepMillis);
    } else
        delay(sleepMillis);
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

