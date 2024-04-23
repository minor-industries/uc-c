#if defined(MI_CUSTOM_BOARD)

#include <Arduino.h>
#include "board.h"

#define LED_PIN 13

void Board::setup() {
    pinMode(LED_PIN, OUTPUT);
}

void Board::ledOn() {
    digitalWrite(LED_PIN, HIGH);
}

void Board::ledOff() {
    digitalWrite(LED_PIN, LOW);
}

void Board::resetRadio() {
    pinMode(22, OUTPUT);
    digitalWrite(22, LOW);
}

SPIClass *Board::spi() {
    return &SPI;
}

Board board = Board{
        .radioCsPin = 21,
        .radioInterruptPin = 23,
};


#endif