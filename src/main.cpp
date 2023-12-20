#include <Arduino.h>
#include "Adafruit_AHTX0.h"
#include "RFM69.h"
#include "Adafruit_INA219.h"
#include "Adafruit_MCP9600.h"
#include "Adafruit_ADS1X15.h"
#include "board.h"
#include "util.h"
#include "adc.h"

#define ALLOW_DEEP_SLEEP false
#define ENABLE_I2C true

#define INA219_ADDR 0x41
#define MCP9600_ADDR 0x60

#define RADIO_SRC_ADDR 0xC4
#define RADIO_DST_ADDR 0x02

Adafruit_AHTX0 *aht;
Adafruit_INA219 *ina219;
Adafruit_MCP9600 *mcp;
Adafruit_ADS1115 *ads1115;

RFM69 *radio;
extern unsigned long startTime;
extern Board board;

void setupDevices() {
    aht = new Adafruit_AHTX0();
    if (!aht->begin()) {
        aht = null;
    } else {
        board.blink(3, 250, 250);
    }

    ina219 = new Adafruit_INA219(INA219_ADDR);
    if (!ina219->begin()) {
        ina219 = null;
    } else {
        board.blink(4, 50, 250);
    }

    mcp = new Adafruit_MCP9600();
    if (!mcp->begin(MCP9600_ADDR)) {
        mcp = null;
    } else {
        board.blink(5, 50, 250);
    }

    ads1115 = new Adafruit_ADS1115();
    if (ads1115->begin()) {
        ads1115->setGain(GAIN_ONE);
        board.blink(6, 50, 250);
    } else {
        mcp = null;
    }
}


void setup() {
    g_APinDescription;
    Serial.begin(9600);

    startTime = millis();

    board.setup();

    board.blink(25, 25, 175);

#if ENABLE_I2C
    setupDevices();
#endif

    board.resetRadio();

    radio = new RFM69(
            board.radioCsPin,
            board.radioInterruptPin,
            true,
            board.spi());

    if (!radio->initialize(RF69_433MHZ, RADIO_SRC_ADDR, 100)) {
        board.blinkForever(500, 100);
    }

    radio->setPowerLevel(23);
}


void readTemp() {
    sensors_event_t humidity, temp;
    aht->getEvent(&humidity, &temp);


    uint8_t buf[9];
    buf[0] = 0x01;

    packFloat(temp.temperature, buf + 1);
    packFloat(humidity.relative_humidity, buf + 5);

    radio->send(RADIO_DST_ADDR, buf, sizeof(buf));
}

void readThermocouple() {
    float t = mcp->readThermocouple();

    uint8_t buf[1 + 4 + 16];
    memset(buf, 0, sizeof(buf));

    uint8_t *pos = buf;

    pos[0] = 0x02;
    pos += 1;

    packFloat(t, pos);
    pos += 4;

    const char *description = "bbq01-bbq";
    memcpy(pos, description, strlen(description));

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


int blinkCount = 10;


void loop() {
    if (blinkCount > 0) {
        board.blink(1, 25, 10);
        blinkCount--;
    }

    if (aht != null) {
        readTemp();
    }

    if (ina219 != null) {
        readPower();
    }

    if (mcp != null) {
        readThermocouple();
    }

    if (ads1115 != null) {
        Datum D;
        memset(&D, 0, sizeof(D));
        readADC(ads1115, &D);
    }

    radio->setMode(RF69_MODE_SLEEP);

    sleep(ALLOW_DEEP_SLEEP, 5000);
}




