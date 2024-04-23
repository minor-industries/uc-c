#include <Arduino.h>
#include "RFM69.h"
#include "Adafruit_ADS1X15.h"
#include <SpritzCipher.h>

#include "board.h"
#include "util.h"
#include "adc.h"

#define ALLOW_DEEP_SLEEP false
#define ENABLE_I2C true

#define RADIO_SRC_ADDR 0xC6
#define RADIO_DST_ADDR 0x02

Adafruit_ADS1115 *ads1115;
spritz_ctx the_ctx;
RFM69 *radio;
extern unsigned long startTime;
extern Board board;


void setupI2CDevices() {
    ads1115 = new Adafruit_ADS1115();
    if (ads1115->begin()) {
        ads1115->setGain(GAIN_ONE);
        board.blink(6, 50, 250);
    } else {
        ads1115 = null;
    }
}


void setup() {
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


int blinkCount = 10;


void sendToRadio() {
    Serial.println("sendToRadio()");
    if (blinkCount > 0) {
        board.blink(1, 25, 10);
        blinkCount--;
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

    radio->setMode(RF69_MODE_SLEEP);

    uint32_t sleepMillis = spritz_random32_uniform(&the_ctx, 1000) + 4500;
    sleep(ALLOW_DEEP_SLEEP, (int32_t) sleepMillis);
}

void loop() {
    sendToRadio();
}

