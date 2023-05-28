#include <ArduinoJson.h>
#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <base64.h>

#include "OmniWeather.h"
#include "sensors.h"

#define NTP_SERVER "europe.pool.ntp.org"

#define NET_WAIT_MS 500
#define WIFI_TIMEOUT_S 10
#define TICK 1000

const char *GITHUB_REPOSITORY_OWNER = "FauconSpartiate";
const char *GITHUB_REPOSITORY_NAME = "OmniWeather-Reports";

bool wifiConnected = false;
bool gotIpAddr = false;

unsigned long getCurrentUnixTime()
{
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("% Failed to obtain time");
    return 0;
  }
  time(&now);
  return now;
}

String getCurrentTime(bool fileName)
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("% Failed to obtain time");
    return "Failed to obtain time";
  }

  String format;

  if (fileName)
  {
    format = "%04d-%02d-%02d-%02d-%02d";
  }
  else
  {
    format = "%04d-%02d-%02d %02d:%02d";
  }

  char buffer[20];
  snprintf(buffer, sizeof(buffer), format.c_str(),
           timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
           timeinfo.tm_hour, timeinfo.tm_min);

  return String(buffer);
}

void disconnectWiFi()
{
  gotIpAddr = false;
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  wifiConnected = false;
  Serial.println("% WiFi Disconnected");
}

void connectWiFi()
{
  if (WiFi.isConnected())
  {
    return;
  }

  WiFi.mode(WIFI_MODE_STA);
  WiFi.setHostname("OmniWeather");

  unsigned long startTime = millis();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (!gotIpAddr)
  {
    delay(NET_WAIT_MS);

    if (!wifiConnected && millis() - startTime > WIFI_TIMEOUT_S * 1000)
    {
      Serial.println("% Failed to connect to WiFi. Timeout exceeded.");
      break;
    }
  }
}

void commitFileToGitHub(const String &fileName, const String &content)
{
  // GitHub API endpoint
  String url = "/repos/" + String(GITHUB_REPOSITORY_OWNER) + "/" + String(GITHUB_REPOSITORY_NAME) + "/contents/reports/" + fileName;

  // Create the JSON payload
  DynamicJsonDocument jsonPayload(500);
  jsonPayload["message"] = getCurrentTime(false);
  jsonPayload["content"] = base64::encode(content);
  jsonPayload["branch"] = "main";

  // Encode the JSON payload
  String payload;
  serializeJson(jsonPayload, payload);
  Serial.println("% Payload: ");
  Serial.println(payload);
  Serial.println();

  // Create the HTTP client object
  WiFiClientSecure wifi;
  wifi.setInsecure();

  HTTPClient http;

  String authHeader = "token " + String(GITHUB_TOKEN);

  // Send the PUT request to GitHub API
  http.begin(wifi, "api.github.com", 443, url, true);

  http.addHeader("Authorization", authHeader);
  http.addHeader("Content-Type", "application/json");

  int httpCode = http.PUT(payload);

  // Read the response data
  String response = http.getString();

  // Print the response data
  Serial.print("Status code: ");
  Serial.println(httpCode);
  Serial.println("Response: ");
  Serial.println(response);
  Serial.println();

  // Clean up
  http.end();
}

void controlTask()
{
  init();
  Serial.println("% Sensors set up");

  connectWiFi();
  configTime(3600, 3600, NTP_SERVER);
  Serial.print("% Time set up. Current time: ");
  Serial.println(getCurrentTime(false));
  disconnectWiFi();

  int lastHour = -1;

  while (true)
  {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
      Serial.println("% Failed to obtain time");
    }

    if (timeinfo.tm_min == 0 && timeinfo.tm_hour != lastHour)
    {
      lastHour = timeinfo.tm_hour;

      DynamicJsonDocument jsonContent(350);

      jsonContent["time"] = getCurrentTime(false);
      jsonContent["UNIX"] = getCurrentUnixTime();
      jsonContent["temperature"] = getTemperature();
      jsonContent["humidity"] = getHumidity();
      jsonContent["UV"] = getUV();
      jsonContent["rain"] = getRain();
      jsonContent["wind_speed"] = getWindSpeed();
      jsonContent["wind_direction"] = getWindDirection();
      jsonContent["barometric_pressure"] = getPressure();
      jsonContent["altitude"] = getAltitude();

      String content;
      serializeJsonPretty(jsonContent, content);
      Serial.println("% Content: ");
      Serial.println(content);
      Serial.println();

      resetValues();

      connectWiFi();

      String fileName = getCurrentTime(true) + ".json";
      commitFileToGitHub(fileName, content);

      disconnectWiFi();
    }

    delay(TICK);
  }
}

void configWiFi()
{
  WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info)
               {
    switch (event)
    {
      case SYSTEM_EVENT_STA_START:
        Serial.print("% WiFi Enabled - SSID: ");
        Serial.println(WIFI_SSID);
        break;
      case SYSTEM_EVENT_STA_CONNECTED:
        wifiConnected = true;
        Serial.print("% WiFi Connected - SSID: ");
        Serial.println(WIFI_SSID);
        break;
      case SYSTEM_EVENT_STA_GOT_IP:
        gotIpAddr = true;
        Serial.print("% WiFi - IPv4 Address: ");
        Serial.println(IPAddress(info.got_ip.ip_info.ip.addr));
        break;
      case SYSTEM_EVENT_STA_LOST_IP:
      case SYSTEM_EVENT_STA_DISCONNECTED:
        wifiConnected = false;
        gotIpAddr = false;
        Serial.print("% WiFi Disconnected - SSID: ");
        Serial.println(WIFI_SSID);
        connectWiFi(); // Retry connection
        break;
      default:
        break;
    } });
}

void setup()
{
  Serial.begin(115200);
  Serial.println("% OmniWeather started");

  configWiFi();
  controlTask();
}

void loop()
{
  delay(2000);

  // Sensor test
  /*Serial.print(digitalRead(39));
  Serial.print(digitalRead(4));
  Serial.print(digitalRead(5));
  Serial.print(digitalRead(65));
  Serial.print(digitalRead(16));
  Serial.print(digitalRead(17));
  Serial.print(digitalRead(21));
  Serial.println(digitalRead(36));*/
}
