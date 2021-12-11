#ifndef WIFI_H_
#define WIFI_H_

#include "main.h"
#include <WiFi.h>
#include "ESPAsyncWebServer.h"
#include "ESPAsyncWiFiManager.h"

void TaskWifi(void *arg);
wl_status_t WifiGetStatus();

#endif