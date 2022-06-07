#ifndef SENSORS_H
#define SENSORS_H
#include "Arduino.h"

void init();
float getTemp();
float getHumid();
int getUV();
unsigned long getRain();
float getWindSpeed();
String getWindDirection();
int getPressure();
int getAltitude();
void rainTick();
void windTick();
void resetValues();

#endif