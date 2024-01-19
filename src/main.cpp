#include <Arduino.h>
#include "Adafruit_AHTX0.h"
#include "RFM69.h"
#include "Adafruit_INA219.h"
#include "Adafruit_MCP9600.h"
#include "Adafruit_ADS1X15.h"
#include "Adafruit_MAX31856.h"
#include "Adafruit_SHT4x.h"
#include <SpritzCipher.h>

#include "board.h"
#include "util.h"
#include "adc.h"

#define ALLOW_DEEP_SLEEP true
#define ENABLE_I2C true

#define INA219_ADDR 0x41
#define MCP9600_ADDR 0x60

#define RADIO_SRC_ADDR 0xC6
#define RADIO_DST_ADDR 0x02

Adafruit_AHTX0 *aht;
Adafruit_INA219 *ina219;
Adafruit_MCP9600 *mcp;
Adafruit_ADS1115 *ads1115;
Adafruit_MAX31856 *max31856;
Adafruit_SHT4x *sht41;
spritz_ctx the_ctx;


RFM69 *radio;
extern unsigned long startTime;
extern Board board;

void setupSPIDevices() {
    max31856 = new Adafruit_MAX31856(0);
    if (max31856->begin()) {
        board.blink(50, 25, 25);
    } else {
        max31856 = null;
    }
}

void setupI2CDevices() {
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
        ads1115 = null;
    }

    sht41 = new Adafruit_SHT4x();
    if (sht41->begin()) {
        Serial.println("found sht");
        board.blink(7, 50, 250);
    } else {
        Serial.println("didn't find sht");
        sht41 = null;
    }
}


void setup() {
    g_APinDescription;
    Serial.begin(9600);

    // this doesn't have to be secure, just don't want to collide with other radios
    const uint8_t key[] = {RADIO_SRC_ADDR};
    spritz_setup(&the_ctx, key, sizeof(key));

    startTime = millis();

    board.setup();

    board.blink(25, 25, 175);

#if ENABLE_I2C
    setupI2CDevices();
#endif
    setupSPIDevices();

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


void monitorThermocouple();

void readSht41(Adafruit_SHT4x *pX);

void sendToRadio() {
    Serial.println("sendToRadio()");
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
        Serial.println("read ADC");
        Datum D;
        memset(&D, 0, sizeof(D));
        readADC(ads1115, &D);
        printADC(&D);
        if (radio) {
            sendADC(radio, RADIO_DST_ADDR, &D);
        }
    }

    if (sht41 != null) {
        readSht41(sht41);
    }

    radio->setMode(RF69_MODE_SLEEP);

    uint32_t sleepMillis = spritz_random32_uniform(&the_ctx, 1000) + 4500;
    Serial.println(String("delay ") + String((int32_t) sleepMillis));
    sleep(ALLOW_DEEP_SLEEP, (int32_t) sleepMillis);
}

void readSht41(Adafruit_SHT4x *sht) {
    Serial.println("readSht41()");
    sensors_event_t humidity, temp;

    uint32_t timestamp = millis();
    sht->getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
    timestamp = millis() - timestamp;

    Serial.println(String("temp: ") + String(temp.temperature));
}

void monitorThermocouple() {
    uint8_t fault = max31856->readFault();
    if (fault) {
        Serial.println(String("FAULT ") + fault);
        return;
    }

    float temp = max31856->readThermocoupleTemperature();
    Serial.println(String("TEMP ") + String(temp));

    sleep(false, 250);
}

void loop() {
    sendToRadio();
//    monitorThermocouple();
}

