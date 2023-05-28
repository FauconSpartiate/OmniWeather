#ifndef SENSORS_H
#define SENSORS_H
#include "Arduino.h"

void init();
float getTemperature();
float getHumidity();
int getUV();
unsigned int getRain();
float getWindSpeed();
String getWindDirection();
int getPressure();
int getAltitude();
void rainTick();
void windTick();
void resetValues();

#endif