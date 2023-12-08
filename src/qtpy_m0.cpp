#if defined(ADAFRUIT_QTPY_M0) && !defined(MI_CUSTOM_BOARD)

#include "Adafruit_NeoPixel.h"
#include "board.h"

#define NEO 11
#define NEO_POWER 12

Adafruit_NeoPixel *strip;

void Board::setup() {
    strip = new Adafruit_NeoPixel(1, NEO, NEO_GRB + NEO_KHZ800);

    pinMode(strip->getPin(), OUTPUT);
    digitalWrite(NEO_POWER, HIGH);
    pinMode(NEO, OUTPUT);

    strip->begin();
    strip->setBrightness(50);
    strip->show();
}

void Board::ledOn() {
    strip->setPixelColor(0, 0xFF0000);
    strip->show();
}

void Board::ledOff() {
    strip->setPixelColor(0, 0x000000);
    strip->show();
}

void Board::resetRadio() {
    // note: we expect the reset pin to be grounded in the circuit
}

SPIClass *Board::spi() {
    return &SPI;
}

Board board = Board{
        .radioCsPin = 0,
        .radioInterruptPin = 1,
};


#endif