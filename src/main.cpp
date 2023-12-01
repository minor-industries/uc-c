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


void loop() {
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);

    blink(1, 25, 10);

    uint8_t buf[9];
    buf[0] = 0x01;

    packFloat(temp.temperature, buf + 1);
    packFloat(humidity.relative_humidity, buf + 5);

    radio->send(
            0x02,
            buf,
            sizeof(buf));

    radio->setMode(RF69_MODE_SLEEP);

    sleep(5000);
}

