<div align='center'>
<img src="https://user-images.githubusercontent.com/49079695/172239606-01d727ac-17bc-4d49-90ee-4558da8448fe.svg"  height="200"/>

# OmniWeather

A simple weather station, powered by an Adafruit ESP32 Feather. The measured results are uploaded every hour to a SSH server.

</div>

## Measured Data

- Temperature
- Humidity
- Barometric pressure
- Wind speed
- Wind direction
- Rainfall
- UV index

## Features

- Periodic data upload on a SSH server (default is 1h)
- WI-FI connection / reconnection on signal loss

## Usage/Examples

[Here's a page](https://foxi.ltam.lu/2CI/pirda350/public/OmniWeather/Weather_Reports/) that shows the output of this code. It should still be running, and new reports are added every hour.

We also created [an UI in Vue.js](https://foxi.ltam.lu/2CI/collo939/public/OmniWeather/) that goes through the latest hourly report and shows it in an user friendly way. You may take a look at it, it should also be updating every hour.

## Tech Stack

**Microcontroller:** PlatformIO

**Server:** Simple SSH storage server

## Deployment

You'll first need some sensors :

- BME280 sensor for temperature, humidity and pressure
- VEML6070 sensor for UV light

The wind speed, wind direction and rain sensors were 3D printed from this awesome project :
[LTB Weather Station by RobWLakes](https://www.thingiverse.com/thing:2849562). He also explains how to mount the whole thing, and I also took some code from there. Massive thanks to him for that!

## Optimizations

Everything was programmed with my ass, the code is kinda fucked, but it works and that's the only thing that matters. Every optimization is welcome!

## Authors

- [@FauconSpartiate](https://github.com/FauconSpartiate)
- [@xavion-lux](https://github.com/xavion-lux)
- [@AigleSpartiate](https://github.com/AigleSpartiate)

## License

[GNU General Public License v3.0](https://choosealicense.com/licenses/gpl-3.0/)
