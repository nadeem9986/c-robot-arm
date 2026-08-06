#include <Arduino.h>
#include "config.h"
#include "RobotArm.h"
#include "WebServerController.h"

// Instantiate Core Controllers
RobotArm robot;
WebServerController webServer(robot, WEB_SERVER_PORT);

void printBanner() {
    Serial.println("\n========================================================");
    Serial.println("   🤖 ESP32 CANTILEVER 4-DOF ROBOT ARM CONTROLLER v1.0   ");
    Serial.println("========================================================");
    Serial.println(" Target Hardware : ESP32 Dev Module (38 Pins)");
    Serial.println(" Servo Driver    : PCA9685 16-Channel 12-Bit PWM (I2C 0x40)");
    Serial.println(" Pinout Mapping  : SDA=GPIO21, SCL=GPIO22, OE=GPIO19");
    Serial.println(" Joint Channels  : J1=CH0, J2=CH1, J3=CH2, J4=CH3");
    Serial.println("========================================================\n");
}

void processSerialCommands() {
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim();
        if (input.length() > 0) {
            Serial.print("[COMMAND IN] ");
            Serial.println(input);
            String response = robot.executeCommand(input);
            Serial.print("[RESPONSE]   ");
            Serial.println(response);
        }
    }
}

void setup() {
    // 1. Initialize High-Speed Serial Monitor
    Serial.begin(115200);
    delay(1000);
    printBanner();

    // 2. Setup Status LED & E-Stop Pin
    pinMode(STATUS_LED_PIN, OUTPUT);
    digitalWrite(STATUS_LED_PIN, HIGH);
#ifdef EMERGENCY_STOP_PIN
    pinMode(EMERGENCY_STOP_PIN, INPUT_PULLUP);
#endif

    // 3. Initialize Robot Arm Engine (PCA9685 & Servos)
    if (!robot.begin()) {
        Serial.println("❌ FATAL: Robot Arm initialization failed! Check I2C wiring.");
        while (1) {
            digitalWrite(STATUS_LED_PIN, !digitalRead(STATUS_LED_PIN));
            delay(200);
        }
    }

    // 4. Initialize WiFi Access Point & Embedded Web Dashboard
    webServer.begin(WIFI_AP_SSID, WIFI_AP_PASS, true);

    Serial.println("✅ SYSTEM READY!");
    Serial.println("👉 Connect Wi-Fi to 'ESP32-RobotArm' (Password: robot1234)");
    Serial.println("👉 Open browser to http://192.168.4.1 for interactive Web Control.");
    Serial.println("👉 Or send serial commands (e.g. 'HOME', 'J 90 45 60 30', 'IK 120 0 100')\n");
}

void loop() {
    // 1. Update smooth trajectory interpolation & teach playback
    robot.update();

    // 2. Handle HTTP Web Dashboard Requests
    webServer.handleClient();

    // 3. Process Serial Commands from USB/UART
    processSerialCommands();

    // 4. E-Stop Button check
#ifdef EMERGENCY_STOP_PIN
    if (digitalRead(EMERGENCY_STOP_PIN) == LOW) {
        delay(20); // Debounce
        if (digitalRead(EMERGENCY_STOP_PIN) == LOW) {
            robot.getServoController().emergencyStop();
        }
    }
#endif

    // Yield control to ESP32 RTOS background tasks
    vTaskDelay(1 / portTICK_PERIOD_MS);
}
