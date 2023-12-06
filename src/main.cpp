#include <Arduino.h>
#include "ArduinoLowPower.h"
#include "Adafruit_AHTX0.h"
#include "RFM69.h"
#include <SPI.h>
#include "Adafruit_INA219.h"

#include "util.h"


#define INA219_ADDR 0x41

#define RADIO_SRC_ADDR 0xC1
#define RADIO_DST_ADDR 0x02

#if defined(ADAFRUIT_QTPY_M0)

#elif defined(ADAFRUIT_TRINKET_M0)

#endif


Adafruit_AHTX0 *aht;
Adafruit_INA219 *ina219;
RFM69 *radio;
extern unsigned long startTime;

void setupLED();


void sleep(int sleepMillis) {
    if (after(30)) {
        LowPower.sleep(sleepMillis);
    } else
        delay(sleepMillis);
}

void setup() {
    g_APinDescription;

    startTime = millis();

    setupLED();

    blink(25, 25, 175);

    aht = new Adafruit_AHTX0();
    ina219 = new Adafruit_INA219(INA219_ADDR);

    if (!aht->begin()) {
        aht = null;
    } else {
        blink(3, 250, 250);
    }

    if (!ina219->begin()) {
        ina219 = null;
    } else {
        blink(4, 50, 250);
    }

    radio = new RFM69(0, 1, true, &SPI);

    if (!radio->initialize(RF69_433MHZ, RADIO_SRC_ADDR, 100)) {
        blinkForever(500, 100);
    }

    radio->setPowerLevel(23);
}

int blinkCount = 10;

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

    radio->send(RADIO_DST_ADDR, buf, sizeof(buf));
}

void readPower() {
    float v = ina219->getBusVoltage_V();
    float i = ina219->getCurrent_mA();
    float p = ina219->getCurrent_mA();

    uint8_t buf[1 + 4 + 4 + 4 + 1 + 16];
    memset(buf, 0, sizeof(buf));

    uint8_t *pos = buf;
    pos[0] = 0x03;
    pos += 1;

    packFloat(v, pos);
    pos += 4;

    packFloat(i, pos);
    pos += 4;

    packFloat(p, pos);
    pos += 4;

    pos[0] = INA219_ADDR;
    pos += 1;

    const char *description = "test-11";
    memcpy(pos, description, strlen(description));

    radio->send(RADIO_DST_ADDR, buf, sizeof(buf));
}


void loop() {
    if (aht != null) {
        readTemp();
    }

    if (ina219 != null) {
        readPower();
    }

    radio->setMode(RF69_MODE_SLEEP);

    sleep(5000);
}


