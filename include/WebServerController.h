#ifndef WEB_SERVER_CONTROLLER_H
#define WEB_SERVER_CONTROLLER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "RobotArm.h"

class WebServerController {
private:
    WebServer server;
    RobotArm& robot;
    bool apMode;

    void setupRoutes();
    void handleRoot();
    void handleApiStatus();
    void handleApiMoveJoints();
    void handleApiMoveIK();
    void handleApiTeach();
    void handleApiEStop();

public:
    WebServerController(RobotArm& robotArm, uint16_t port = WEB_SERVER_PORT);

    void begin(const char* ssid = WIFI_AP_SSID, const char* password = WIFI_AP_PASS, bool startAP = true);
    void handleClient();
};

#endif // WEB_SERVER_CONTROLLER_H
