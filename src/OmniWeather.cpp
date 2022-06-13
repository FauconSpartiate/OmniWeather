#include <ArduinoJson.h>
#include <Arduino.h>
#include "SPIFFS.h"
#include "IPv6Address.h"
#include <WiFi.h>
#include <time.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <errno.h>
#include <sys/stat.h>
#include <sys/reent.h>
#include "tcpip_adapter.h"
#include <esp_event_legacy.h>
#include "driver/uart.h"
#include "esp_vfs_dev.h"

#include "OmniWeather.h"
#include "sensors.h"
#include "ssh.h"

struct _reent reent_data_esp32;
struct _reent *_impure_ptr = &reent_data_esp32;

// Timing and timeout configuration.
#define WIFI_TIMEOUT_S 10
#define NET_WAIT_MS 100

bool wifiConnected;
bool gotIpAddr;

unsigned long getUNIX()
{
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return (0);
  }
  time(&now);
  return now;
}

String getTime(bool fileName)
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println(F("Failed to obtain time"));
    return "Failed to obtain time";
  }

  String month = String(timeinfo.tm_mon + 1);
  String day = String(timeinfo.tm_mday);
  String hours = String(timeinfo.tm_hour);
  String mins = String(timeinfo.tm_min);

  if (timeinfo.tm_mon + 1 < 10)
    month = "0" + month;
  if (timeinfo.tm_mday < 10)
    day = "0" + day;
  if (timeinfo.tm_hour < 10)
    hours = "0" + hours;
  if (timeinfo.tm_min < 10)
    mins = "0" + mins;

  if (!fileName)
    return (String(timeinfo.tm_year + 1900) + "-" + month + "-" + day + " " + hours + ":" + mins);
  else
    return (String(timeinfo.tm_year + 1900) + "-" + month + "-" + day + "-" + hours + "-" + mins);
}

#define delayTime 60000
#define TICK 100

void controlTask(void *pvParameter)
{
  _REENT_INIT_PTR((&reent_data_esp32));

  init();
  Serial.println("% Sensors set up");

  // Mount the file system.
  boolean fsGood = SPIFFS.begin();
  if (!fsGood)
  {
    printf("%% No formatted SPIFFS filesystem found to mount.\n");
    printf("%% Formatting SPIFFS\n");
    fsGood = SPIFFS.format();
    if (fsGood)
      SPIFFS.begin();
  }
  if (!fsGood)
  {
    printf("%% Aborting now.\n");
    while (true)
      vTaskDelay(TICK);
  }
  printf("%% Mounted SPIFFS used=%d total=%d\r\n", SPIFFS.usedBytes(), SPIFFS.totalBytes());

  connectWiFI();

  configTime(3600, 3600, NTPSERVER);

  vTaskDelay(2000);

  disconnectWiFI();

  int lastHour = -1;

  while (true)
  {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
      Serial.println(F("Failed to obtain time"));
      break;
    }

    if (timeinfo.tm_sec == 0)
    // if (timeinfo.tm_min == 0 && timeinfo.tm_hour != lastHour)
    {
      lastHour = timeinfo.tm_hour;

      DynamicJsonDocument doc(2048);

      doc["time"] = getTime(false);
      doc["UNIX"] = getUNIX();                    // UNIX
      doc["temperature"] = getTemp();             // °
      doc["humidity"] = getHumid();               // %
      doc["UV"] = getUV();                        // UV Index
      doc["rain"] = getRain();                    // mm
      doc["wind_speed"] = getWindSpeed();         // km/h
      doc["wind_direction"] = getWindDirection(); // °
      doc["barometric_pressure"] = getPressure(); // hPA
      doc["altitude"] = getAltitude();            // m

      resetValues();

      connectWiFI();

      libssh_begin();

      String fileNameS = (String("/") + getTime(true) + String(".json"));
      char fileName[fileNameS.length() + 1];
      fileNameS.toCharArray(fileName, fileNameS.length() + 1);

      String fileName2S = "/spiffs" + fileNameS;
      char fileName2[fileName2S.length() + 1];
      fileName2S.toCharArray(fileName2, fileName2S.length() + 1);

      char *fileCommand[] = {"libssh_scp", fileName2, SSHSERVER, NULL};

      File file = SPIFFS.open(fileName, FILE_WRITE);

      Serial.println("Writing JSON:");
      serializeJsonPretty(doc, file);
      serializeJsonPretty(doc, Serial);
      Serial.println("");

      file.close();

      int ex_argc = sizeof fileCommand / sizeof fileCommand[0] - 1;
      printf("%% Execution in progress:");
      short a;
      for (a = 0; a < ex_argc; a++)
        printf(" %s", fileCommand[a]);
      printf("\n");
      int ex_rc = ex_main(ex_argc, fileCommand);
      printf("%% Execution completed with return code: %d\n", ex_rc);

      // SPIFFS.format();
      SPIFFS.remove(fileNameS);

      disconnectWiFI();
    }

    vTaskDelay(TICK);
  }
}

void setup()
{
  uart_driver_install((uart_port_t)CONFIG_CONSOLE_UART_NUM, 256, 0, 0, NULL, 0);
  esp_vfs_dev_uart_use_driver(CONFIG_CONSOLE_UART_NUM);
  Serial.begin(115200);

  tcpip_adapter_init();
  esp_event_loop_init(event_cb, NULL);

  // Stack size needs to be larger, so continue in a new task.
  xTaskCreatePinnedToCore(controlTask, "ctl", 40960, NULL, (tskIDLE_PRIORITY + 3), NULL, portNUM_PROCESSORS - 1);
}

void loop()
{
  // Nothing to do here since controlTask has taken over.
  vTaskDelay(TICK);
  // Serial.println(getUV());
}

void connectWiFI()
{
  disconnectWiFI();
  WiFi.mode(WIFI_MODE_STA);
  WiFi.setHostname("OmniWeather");
  WiFi.begin(WIFISSID, WIFIPASSWORD);

  unsigned long lastTime = millis();

  while (!gotIpAddr)
  {
    vTaskDelay(TICK);

    if (!wifiConnected && millis() - lastTime > 10000)
      connectWiFI();
  }
}

void disconnectWiFI()
{
  gotIpAddr = false;
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

// WiFi
esp_err_t event_cb(void *ctx, system_event_t *event)
{
  switch (event->event_id)
  {
  case SYSTEM_EVENT_STA_START:
    Serial.print("% WiFi Enabled - SSID: ");
    Serial.println(WIFISSID);
    break;
  case SYSTEM_EVENT_STA_CONNECTED:
    wifiConnected = true;
    Serial.print("% WiFi Connected - SSID: ");
    Serial.println(WIFISSID);
    break;
  case SYSTEM_EVENT_STA_GOT_IP:
    gotIpAddr = true;
    Serial.print("% WIFi - IPv4 Address: ");
    Serial.println(IPAddress(event->event_info.got_ip.ip_info.ip.addr));
    break;
  case SYSTEM_EVENT_STA_LOST_IP:
  case SYSTEM_EVENT_STA_DISCONNECTED:
    wifiConnected = false;
    gotIpAddr = false;
    Serial.print("% WIFi Disconnected - SSID: ");
    Serial.println(WIFISSID);
    // connectWiFI();
    break;
  default:
    break;
  }
  return ESP_OK;
}