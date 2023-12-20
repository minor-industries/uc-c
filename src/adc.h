#include "stdint.h"
#include "Adafruit_ADS1X15.h"

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

#endif //UNTITLED_ADC_H
