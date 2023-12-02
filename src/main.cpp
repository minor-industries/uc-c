#include <Arduino.h>
#include "ArduinoLowPower.h"
#include "Adafruit_AHTX0.h"
#include "RFM69.h"
#include <SPI.h>
#include "Adafruit_INA219.h"
#include "Adafruit_NeoPixel.h"
#include "util.h"


#define NEO 11
#define NEO_POWER 12


Adafruit_AHTX0 *aht;
Adafruit_INA219 *ina219;
RFM69 *radio;
extern unsigned long startTime;
extern Adafruit_NeoPixel *strip;


void sleep(int sleepMillis) {
    if (after(30)) {
        LowPower.sleep(sleepMillis);
    } else
        delay(sleepMillis);
}

void setup() {
    startTime = millis();

    strip = new Adafruit_NeoPixel(1, NEO, NEO_GRB + NEO_KHZ800);

    pinMode(strip->getPin(), OUTPUT);
    digitalWrite(NEO_POWER, HIGH);
    pinMode(NEO, OUTPUT);

    strip->begin();
    strip->setBrightness(50);
    strip->show();

    blink(25, 25, 175);

    aht = new Adafruit_AHTX0();
    ina219 = new Adafruit_INA219(0x41);

    if (!aht->begin()) {
        aht = null;
    } else {
        blink(3, 250, 250);
    }

    if (!ina219->begin()) {
        aht = null;
    } else {
        blink(4, 50, 250);
    }

    radio = new RFM69(0, 1, true, &SPI);

    if (!radio->initialize(RF69_433MHZ, 0xC0, 100)) {
        blinkForever(500, 100);
    }

    radio->setPowerLevel(23);
}

int blinkCount = 10;

void readCurrent();

void readTemp() {
    sensors_event_t humidity, temp;
    aht->getEvent(&humidity, &temp);

    if (blinkCount > 0) {
        blink(1, 25, 10);
        blinkCount--;
    }

    uint8_t buf[9];
    buf[0] = 0x01;

    packFloat(temp.temperature, buf + 1);
    packFloat(humidity.relative_humidity, buf + 5);

    radio->send(
            0x02,
            buf,
            sizeof(buf));
}

void readCurrent() {
    float v = ina219->getBusVoltage_V();
    float i = ina219->getCurrent_mA();
    float p = ina219->getCurrent_mA();

    int vInt = int(v);
    blink(vInt, 25, 475);
}


void loop() {
    if (aht != null) {
        readTemp();
    }

    if (ina219 != null) {
        readCurrent();
    }

    radio->setMode(RF69_MODE_SLEEP);

    sleep(5000);
}


