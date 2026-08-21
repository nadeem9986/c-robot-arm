#ifndef WEB_SERVER_CONTROLLER_H
#define WEB_SERVER_CONTROLLER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "RobotArm.h"
#include "config.h"

class WebServerController {
private:
    WebServer server;
    RobotArm& robot;
    bool apMode;

    void setupRoutes();
    void handleRoot();
    void handleModelPage();
    void handleApiStatus();
    void handleApiMoveJoints();
    void handleApiMoveIK();
    void handleApiTeach();
    void handleApiSetLimits();
    void handleApiSetSpeed();

public:
    WebServerController(RobotArm& robotArm, uint16_t port = WEB_SERVER_PORT);

    void begin(bool useStationMode = WIFI_MODE_STATION,
               const char* staSSID = WIFI_STA_SSID,
               const char* staPass = WIFI_STA_PASS,
               const char* apSSID = WIFI_AP_SSID,
               const char* apPass = WIFI_AP_PASS);
    
    void handleClient();
};

#endif // WEB_SERVER_CONTROLLER_H
