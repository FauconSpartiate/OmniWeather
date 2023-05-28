<div align='center'>
  <a href='https://omniweather-beta.netlify.app'>
    <img src="https://user-images.githubusercontent.com/49079695/172239606-01d727ac-17bc-4d49-90ee-4558da8448fe.svg"  height="200"/>
  </a>

# OmniWeather

A simple weather station, powered by an Adafruit ESP32 Feather. The measured results are uploaded every hour to a GitHub repository via its API.

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

- Periodic data upload on a GitHub repository (default is 1h)
- WI-FI connection / reconnection on signal loss

## Usage/Examples

The OmniWeather project also consists of a website that displays the data in a nice and user friendly way. The website is built with Vue.js and displays the latest report available on the server. You can check out [it's repository here](https://github.com/FauconSpartiate/OmniWeather-Website), and the deployed website on [omniweather-beta.netlify.app](https://omniweather-beta.netlify.app).

You can also take a look at the hourly reports the microcontroller sends out to the server [on this repo](https://github.com/FauconSpartiate/OmniWeather-Reports). It "should" still be running, and new reports are added every hour.

## Tech Stack

PlatformIO with Arduino C++

## Deployment

You'll first need a microcontroller to run everything. We used an Adafruit ESP32 Feather.

You will then need some sensors :

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
