#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// =========================================================================
// 🌐 1. WI-FI CONFIGURATION (ENTER YOUR WI-FI CREDENTIALS HERE)
// =========================================================================
// Set WIFI_MODE_STATION to true to connect to your Home/Mobile Hotspot Wi-Fi.
// If set to false or if connection fails, it will start Access Point (AP) mode.
#define WIFI_MODE_STATION    true

// ✏️ EDIT YOUR WI-FI DETAILS HERE:
#define WIFI_STA_SSID        "AMANAT"        // Replace with your Wi-Fi Name / Hotspot
#define WIFI_STA_PASS        "amanath@123"   // Replace with your Wi-Fi Password

// Fallback Access Point (AP) Details:
#define WIFI_AP_SSID         "ESP32-RobotArm"
#define WIFI_AP_PASS         "robot1234"
#define WEB_SERVER_PORT      80

// =========================================================================
// ⚙️ 2. BOOT BEHAVIOR & SAFETY (NO AUTOMATIC MOVEMENT ON STARTUP)
// =========================================================================
// Set to false so ESP32 DOES NOT force sudden servo movements on boot!
// Servos will only move when commanded via the Phone Web Interface or Serial.
#define ENABLE_AUTO_HOME_ON_BOOT  false

// =========================================================================
// 📌 3. ESP32 38-PIN DEV MODULE PIN DEFINITIONS
// =========================================================================
#define I2C_SDA_PIN          21    // ESP32 I2C Data Pin (Connect to PCA9685 SDA)
#define I2C_SCL_PIN          22    // ESP32 I2C Clock Pin (Connect to PCA9685 SCL)
#define PCA9685_OE_PIN       -1    // OE Pin Disconnected (-1): PCA9685 defaults to Enabled via onboard pull-down
#define STATUS_LED_PIN        2    // Onboard ESP32 Status LED
#define EMERGENCY_STOP_PIN   15    // Optional Hardware E-Stop Button Pin

// =========================================================================
// ⚡ 4. PCA9685 HARDWARE CONFIGURATION
// =========================================================================
#define PCA9685_I2C_ADDR     0x40  // Default I2C address for PCA9685 board
#define SERVO_FREQ_HZ        50    // Standard PWM frequency for servos (50Hz = 20ms period)
#define I2C_CLOCK_SPEED      400000 // I2C Fast Mode (400 kHz)

// Pulse Width Calibration Ticks for 50Hz (12-bit PCA9685: 0 to 4095 ticks)
#define SERVOMIN             125   // Min pulse tick count (0 degrees ~ 600us)
#define SERVOMAX             575   // Max pulse tick count (180 degrees ~ 2400us)

// =========================================================================
// 🤖 5. 4-DOF CANTILEVER ARM SERVO CHANNELS & LIMITS
// =========================================================================
#define JOINT1_BASE_CH       0     // Joint 1: Base Yaw (PCA9685 Channel 0)
#define JOINT2_SHOULDER_CH   1     // Joint 2: Shoulder Pitch (PCA9685 Channel 1)
#define JOINT3_ELBOW_CH      2     // Joint 3: Elbow Pitch (PCA9685 Channel 2)
#define JOINT4_GRIPPER_CH    3     // Joint 4: Wrist / Gripper (PCA9685 Channel 3)

// Joint Angle Limits (Degrees)
#define J1_MIN_DEG           0.0f
#define J1_MAX_DEG           180.0f
#define J1_DEFAULT_DEG       90.0f

#define J2_MIN_DEG           15.0f
#define J2_MAX_DEG           165.0f
#define J2_DEFAULT_DEG       90.0f

#define J3_MIN_DEG           10.0f
#define J3_MAX_DEG           170.0f
#define J3_DEFAULT_DEG       90.0f

#define J4_MIN_DEG           0.0f
#define J4_MAX_DEG           17.0f
#define J4_DEFAULT_DEG       10.0f

// Calibration Zero-Offset (Degrees)
#define J1_OFFSET_DEG        0.0f
#define J2_OFFSET_DEG        0.0f
#define J3_OFFSET_DEG        0.0f
#define J4_OFFSET_DEG        0.0f

// =========================================================================
// 📐 6. CANTILEVER ROBOT GEOMETRY (KINEMATICS)
// =========================================================================
#define LINK_BASE_HEIGHT_L0  85.0f   // Height from ground to Shoulder joint (mm)
#define LINK_SHOULDER_L1     120.0f  // Upper arm cantilever boom length (mm)
#define LINK_FOREARM_L2      110.0f  // Forearm cantilever link length (mm)
#define LINK_GRIPPER_L3       65.0f  // Gripper / tool length (mm)

// =========================================================================
// 🚀 7. MOTION PROFILE & DUAL-CORE RTOS CONFIG
// =========================================================================
#define DEFAULT_MAX_SPEED_DPS 120.0f // Increased speed limit for responsive cantilever torque
#define MAX_TEACH_POSES       50     // Max keyframe positions for Teach & Repeat
#define SYSTEM_RUN_DUAL_CORE  true   // Enable FreeRTOS Core 0 (Wi-Fi/Web) + Core 1 (Motion)

#endif // CONFIG_H
