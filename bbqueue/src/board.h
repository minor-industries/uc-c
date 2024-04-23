#ifndef UNTITLED_BOARD_H
#define UNTITLED_BOARD_H

#include <Arduino.h>
#include <SPI.h>

struct Board {

public:
    uint8_t radioCsPin;
    uint8_t radioInterruptPin;

    void setup();

    void ledOn();

    void ledOff();

    void resetRadio();

    SPIClass *spi();

    void blink(int repeat, int high, int low) {
        for (int i = 0; i < repeat; ++i) {
            ledOn();
            delay(high);
            ledOff();
            delay(low);
        }
    }

    void blinkForever(int high, int low) {
        while (true) {
            ledOn();
            delay(high);
            ledOff();
            delay(low);
        }
    }
};

#endif //UNTITLED_BOARD_H
