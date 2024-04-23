#include <string>
#include "stdint.h"
#include "Adafruit_ADS1X15.h"
#include "RFM69.h"

#ifndef UNTITLED_ADC_H
#define UNTITLED_ADC_H

typedef struct {
    bool present;
    int16_t counts;
    float V;
    float R;
    float T;
} Channel;


typedef struct {
    Channel channels[4];
} Datum;

void readADC(Adafruit_ADS1115 *, Datum *);

void printADC(Datum *);

void sendADC(RFM69 *radio, uint16_t toAddr, Datum *D, const String &description);

#endif //UNTITLED_ADC_H
