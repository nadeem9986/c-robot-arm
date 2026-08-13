#include <Arduino.h>
#include "config.h"
#include "RobotArm.h"
#include "WebServerController.h"

// Instantiate Core Controllers
RobotArm robot;
WebServerController webServer(robot, WEB_SERVER_PORT);

// FreeRTOS Task Handles for ESP32 Dual Core Multitasking
TaskHandle_t WebTaskHandle = NULL;
TaskHandle_t MotionTaskHandle = NULL;

void printBanner() {
    Serial.println("\n========================================================");
    Serial.println("   🤖 ESP32 CANTILEVER 4-DOF ROBOT ARM CONTROLLER v2.0   ");
    Serial.println("========================================================");
    Serial.println(" Target Hardware : ESP32 Dev Module Dual-Core (38 Pins)");
    Serial.println(" Dual Core Engine: CORE 0 = Wi-Fi/Web | CORE 1 = Motion");
    Serial.println(" Servo Driver    : PCA9685 16-Channel 12-Bit PWM (I2C 0x40)");
    Serial.println(" Pinout Mapping  : SDA=GPIO21, SCL=GPIO22");
    Serial.println(" Joint Channels  : J1=CH0, J2=CH1, J3=CH2, J4=CH3");
    Serial.println(" Soft Boot Mode  : Passive (No auto-movement on boot)");
    Serial.println("========================================================\n");
}

// -------------------------------------------------------------------------
// 🧠 CORE 0 TASK: Wi-Fi, HTTP Server & Network Telemetry
// -------------------------------------------------------------------------
void webServerTask(void *pvParameters) {
    Serial.printf("[FREE-RTOS] Web Server Task pinned to CORE %d\n", xPortGetCoreID());
    for (;;) {
        webServer.handleClient();
        vTaskDelay(5 / portTICK_PERIOD_MS); // Yield 5ms to Prevent WDT Reset
    }
}

// -------------------------------------------------------------------------
// ⚡ CORE 1 TASK: High-Precision 50Hz Real-Time Servo Trajectory Control
// -------------------------------------------------------------------------
void motionControlTask(void *pvParameters) {
    Serial.printf("[FREE-RTOS] Real-Time Motion Task pinned to CORE %d\n", xPortGetCoreID());
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(10); // 100Hz (10ms) High-precision tick

    for (;;) {
        robot.update();

#ifdef EMERGENCY_STOP_PIN
        if (digitalRead(EMERGENCY_STOP_PIN) == LOW) {
            vTaskDelay(10 / portTICK_PERIOD_MS);
            if (digitalRead(EMERGENCY_STOP_PIN) == LOW) {
                robot.getServoController().emergencyStop();
            }
        }
#endif

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
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

    // 4. Initialize Wi-Fi (Station Mode with AP Fallback) & Web Server
    webServer.begin(WIFI_MODE_STATION, WIFI_STA_SSID, WIFI_STA_PASS, WIFI_AP_SSID, WIFI_AP_PASS);

    // 5. Spawn FreeRTOS Dual-Core Tasks
#if SYSTEM_RUN_DUAL_CORE
    Serial.println("[SYSTEM] Spawning FreeRTOS Dual-Core Architecture...");

    // Task 1: Web Server on Core 0
    xTaskCreatePinnedToCore(
        webServerTask,
        "WebServerTask",
        8192,
        NULL,
        1,              // Priority 1
        &WebTaskHandle,
        0               // CORE 0
    );

    // Task 2: Real-time Servo Motion Control on Core 1
    xTaskCreatePinnedToCore(
        motionControlTask,
        "MotionTask",
        8192,
        NULL,
        2,              // Higher Priority 2 for deterministic timing
        &MotionTaskHandle,
        1               // CORE 1
    );

    Serial.println("✅ DUAL-CORE MULTITASKING INITIALIZED SUCCESSFULLY!");
#endif

    Serial.println("=================================================");
    Serial.println("  👉 SYSTEM READY & OPERATING ON BOTH CPU CORES!");
    Serial.println("=================================================\n");
}

void loop() {
    // If Dual-Core is active, loop() only needs to handle Serial & WDT
    processSerialCommands();
    vTaskDelay(100 / portTICK_PERIOD_MS);
}
