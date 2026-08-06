#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ==========================================
// 1. ESP32 38-PIN DEV MODULE PIN DEFINITIONS
// ==========================================
#define I2C_SDA_PIN          21    // Default ESP32 I2C Data Pin
#define I2C_SCL_PIN          22    // Default ESP32 I2C Clock Pin
#define PCA9685_OE_PIN       19    // Output Enable (Active LOW, Optional)
#define STATUS_LED_PIN        2    // Onboard ESP32 LED (GPIO 2)
#define EMERGENCY_STOP_PIN   15    // Hardware E-Stop Button (Pull-up)

// ==========================================
// 2. PCA9685 HARDWARE CONFIGURATION
// ==========================================
#define PCA9685_I2C_ADDR     0x40  // Default I2C address for PCA9685 board
#define SERVO_FREQ_HZ        50    // Standard analog/digital servo PWM frequency (50Hz = 20ms period)
#define I2C_CLOCK_SPEED      400000 // I2C Fast Mode (400 kHz)

// PCA9685 12-bit Resolution (0 - 4095 ticks)
// 50Hz period = 20,000 microseconds (20ms)
// 1 tick = 20,000us / 4096 = 4.8828 microseconds
#define SERVO_PULSE_MIN_US   600   // 0 degrees pulse width in microseconds (approx 123 ticks)
#define SERVO_PULSE_MAX_US   2400  // 180 degrees pulse width in microseconds (approx 491 ticks)

// Default Tick values for 50Hz (Calibrated for Standard SG90 / MG996R / MG995 servos)
#define SERVOMIN             125   // Min pulse tick count (0 degrees)
#define SERVOMAX             575   // Max pulse tick count (180 degrees)

// ==========================================
// 3. 4-DOF CANTILEVER ARM SERVO CHANNEL MAP
// ==========================================
#define JOINT1_BASE_CH       0     // Joint 1: Base Rotation (Yaw - Z axis)
#define JOINT2_SHOULDER_CH   1     // Joint 2: Shoulder Pitch (Cantilever Main Arm)
#define JOINT3_ELBOW_CH      2     // Joint 3: Elbow Pitch (Cantilever Forearm)
#define JOINT4_GRIPPER_CH    3     // Joint 4: Wrist / End-Effector Gripper

// Joint Angular Range Limits (Degrees)
#define J1_MIN_DEG           0.0f
#define J1_MAX_DEG           180.0f
#define J1_DEFAULT_DEG       90.0f

#define J2_MIN_DEG           15.0f
#define J2_MAX_DEG           165.0f
#define J2_DEFAULT_DEG       90.0f

#define J3_MIN_DEG           10.0f
#define J3_MAX_DEG           170.0f
#define J3_DEFAULT_DEG       90.0f

#define J4_MIN_DEG           0.0f   // Fully Closed / Open depending on mechanical link
#define J4_MAX_DEG           120.0f
#define J4_DEFAULT_DEG       60.0f

// Joint Zero-Offset Calibration (Degrees)
#define J1_OFFSET_DEG        0.0f
#define J2_OFFSET_DEG        0.0f
#define J3_OFFSET_DEG        0.0f
#define J4_OFFSET_DEG        0.0f

// ==========================================
// 4. CANTILEVER ROBOT GEOMETRY (KINEMATICS)
// ==========================================
// Link Lengths in Millimeters (mm) - Customise to your specific arm dimensions!
#define LINK_BASE_HEIGHT_L0  85.0f   // Height from ground/base to Shoulder joint center
#define LINK_SHOULDER_L1     120.0f  // Length of main upper cantilever link
#define LINK_FOREARM_L2      110.0f  // Length of forearm link to wrist
#define LINK_GRIPPER_L3       65.0f  // Length from wrist joint center to tip of gripper

// ==========================================
// 5. MOTION PROFILE & INTERPOLATION CONTROL
// ==========================================
#define DEFAULT_MAX_SPEED_DPS 60.0f  // Maximum speed (Degrees per second)
#define DEFAULT_ACCEL_DPSS    120.0f // Acceleration (Degrees per second^2)
#define INTERPOLATION_STEP_MS 15     // Trajectory update loop interval in ms (~66 Hz update rate)

// MAX Keyframes for Teach & Repeat
#define MAX_TEACH_POSES       50

// ==========================================
// 6. NETWORK & WEB CONTROL
// ==========================================
#define WIFI_AP_SSID         "ESP32-RobotArm"
#define WIFI_AP_PASS         "robot1234"
#define WEB_SERVER_PORT      80

#endif // CONFIG_H
