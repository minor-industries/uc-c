#ifndef __UTIL_H
#define __UTIL_H

bool after(unsigned long timeInSeconds);

bool before(unsigned long timeInSeconds);

void sleep(bool allowDeep, int sleepMillis);

void on();

void off();

void blink(int repeat, int high, int low);

void blinkForever(int high, int low);

void packFloat(float floatVal, uint8_t *buf);

#endif