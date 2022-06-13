#include "sensors.h"

#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <Adafruit_VEML6070.h>

#define RAINPIN 32
#define WINDSPEEDPIN 33
#define BME280ADDRESS 0x76
const int WINDDIRECTIONPINS[8] = {39, 4, 5, 32, 16, 17, 21, 36};

const char *compass[] = {"N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE", "S", "SSW", "SW", "WSW", "", "WNW", "NW", "NNW", "Error"};

#define TIMERDELAY 400

unsigned long rainFall = 0;
unsigned long windSpeed = 0;
unsigned long rainFallTimer = 0;
unsigned long windSpeedTimer = 0;

Adafruit_BME280 bme;
Adafruit_VEML6070 uv = Adafruit_VEML6070();

void init()
{
	bme.begin(BME280ADDRESS);
	uv.begin(VEML6070_2_T);

	pinMode(RAINPIN, INPUT_PULLUP);
	attachInterrupt(RAINPIN, rainTick, FALLING);

	pinMode(WINDSPEEDPIN, INPUT_PULLUP);
	attachInterrupt(WINDSPEEDPIN, windTick, FALLING);

	pinMode(WINDDIRECTIONPINS[0], INPUT_PULLUP);
	pinMode(WINDDIRECTIONPINS[1], INPUT_PULLUP);
	pinMode(WINDDIRECTIONPINS[2], INPUT_PULLUP);
	pinMode(WINDDIRECTIONPINS[3], INPUT_PULLUP);
	pinMode(WINDDIRECTIONPINS[4], INPUT_PULLUP);
	pinMode(WINDDIRECTIONPINS[5], INPUT_PULLUP);
	pinMode(WINDDIRECTIONPINS[6], INPUT_PULLUP);
	pinMode(WINDDIRECTIONPINS[7], INPUT_PULLUP);
}

void IRAM_ATTR rainTick()
{
	if (millis() - rainFallTimer > TIMERDELAY)
	{
		if (digitalRead(RAINPIN))
		{
			rainFall++;
			Serial.print("Rain clapper tilted: ");
			Serial.println(rainFall);

			rainFallTimer = millis();
		}
	}
}

void IRAM_ATTR windTick()
{
	if (millis() - windSpeedTimer > TIMERDELAY / 2)
	{
		if (digitalRead(WINDSPEEDPIN))
		{
			windSpeed++;
			Serial.print("Wind magnet passed: ");
			Serial.println(windSpeed);

			windSpeedTimer = millis();
		}
	}
}

float getTemp()
{
	if (isnan(bme.readTemperature()))
	/*sensors_event_t event;
	dht.temperature().getEvent(&event);
	if (isnan(event.temperature))*/
	{
		Serial.println(F("Error reading temperature!"));
		return -1;
	}
	else
	{
		int a = (int)bme.readTemperature() * 10;
		// int a = (int)event.temperature * 10;
		return (a / 10.0);
	}
}

float getHumid()
{
	if (isnan(bme.readHumidity()))
	/*sensors_event_t event;
	dht.humidity().getEvent(&event);
	if (isnan(event.relative_humidity))*/
	{
		Serial.println(F("Error reading humidity!"));
		return -1;
	}
	else
	{
		int a = (int)bme.readHumidity() * 10;
		// int a = (int)event.relative_humidity * 10;
		return (a / 10.0);
	}
}

int getPressure()
{
	if (isnan(bme.readPressure()))
	{
		Serial.println(F("Error reading pressure!"));
		return -1;
	}
	else
	{
		int a = (int)(bme.readPressure() / 100.0) * 10;
		return (a / 10.0);
	}
}

int getAltitude()
{
	if (isnan(bme.readAltitude(1013.25)))
	{
		Serial.println(F("Error reading pressure!"));
		return -1;
	}
	else
	{
		int a = (int)(bme.readAltitude(1013.25)) * 10;
		return (a / 10.0);
	}
}

int getUV()
{
	return map(uv.readUV(), 0, 2530, 1, 11);
}

float getWindSpeed()
{
	windSpeed = 0;
	delay(5000);
	return (int)((windSpeed * 3.1414 * 2 * 0.065 * 12 * 60 / 1000) * 10) / 10.0;
}

String getWindDirection()
{
	int pointer = -1;
	int direct = 0;

	for (int i = 0; i < 8; i++)
	{
		direct += pow(digitalRead(WINDDIRECTIONPINS[i]), (2 * i));
	}

	switch (direct)
	{
	case 1:
		pointer = 0;
		break;
	case 3:
		pointer = 1;
		break;
	case 2:
		pointer = 2;
		break;
	case 6:
		pointer = 3;
		break;
	case 4:
		pointer = 4;
		break;
	case 12:
		pointer = 5;
		break;
	case 8:
		pointer = 6;
		break;
	case 24:
		pointer = 7;
		break;
	case 16:
		pointer = 8;
		break;
	case 48:
		pointer = 9;
		break;
	case 32:
		pointer = 10;
		break;
	case 96:
		pointer = 11;
		break;
	case 64:
		pointer = 12;
		break;
	case 192:
		pointer = 13;
		break;
	case 128:
		pointer = 14;
		break;
	case 129:
		pointer = 15;
		break;
	default:
		pointer = 16;
		break;
	}

	return compass[pointer];
}

unsigned int getRain()
{
	// TODO convert to correct unit
	return rainFall * 0.3527;
}

void resetValues()
{
	rainFall = 0;
}