#include <Arduino.h>
#include "Adafruit_NeoPixel.h"
#include "ArduinoLowPower.h"
#include "Adafruit_AHTX0.h"
#include "RFM69.h"
#include <SPI.h>

#define LED_PIN 0
#define NEO_POWER 12
#define NEO 11

Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, NEO, NEO_GRB + NEO_KHZ800);
Adafruit_AHTX0 aht;

RFM69 *radio;

//SPIClass SPI1(PERIPH_SPI);


unsigned long startTime;

void on() {
    strip.setPixelColor(0, 0xFF0000);
    strip.show();
}

void off() {
    strip.setPixelColor(0, 0x000000);
    strip.show();
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

void sleep(int sleepMillis) {
    bool upAndRunning = millis() - startTime > 30000;
    if (upAndRunning) {
        LowPower.sleep(sleepMillis);
    } else {
        delay(sleepMillis);
    }
}

void setup() {
    startTime = millis();

    g_APinDescription;


    pinMode(LED_PIN, OUTPUT);
    pinMode(NEO_POWER, OUTPUT);
    digitalWrite(NEO_POWER, HIGH);
    pinMode(NEO, OUTPUT);

    strip.begin();
    strip.setBrightness(50);
    strip.show();

    blink(25, 25, 175);

    if (!aht.begin()) {
        blinkForever(1000, 1000);
    } else {
        blink(3, 250, 250);
    }

    radio = new RFM69(0, 1, true, &SPI);

    if (!radio->initialize(RF69_433MHZ, 0xC0, 100)) {
        blinkForever(500, 100);
    }

    radio->setPowerLevel(23);
}

void loop() {
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);

    int tInt = int(temp.temperature);
    blink(tInt, 25, 75);

    uint8_t buf[] = {0xAA, 0xBB, 0xCC, 0xDD};

    radio->send(
            0x02,
            buf,
            sizeof(buf));

    sleep(5000);
}

