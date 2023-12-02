#ifndef __UTIL_H
#define __UTIL_H

bool after(unsigned long timeInSeconds);

bool before(unsigned long timeInSeconds);

void on();

void off();

void blink(int repeat, int high, int low);

void blinkForever(int high, int low);

#endif