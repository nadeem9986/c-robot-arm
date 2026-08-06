# 🤖 ESP32 4-DOF Cantilever Robot Arm Control Software

[![ESP32 Hardware Verified](https://img.shields.io/badge/ESP32-Detected%20on%20COM4-brightgreen.svg)](file:///c:/Users/nad/Desktop/c-robot-arm/README.md)
[![Driver](https://img.shields.io/badge/PCA9685-16--Channel%2012--Bit%20PWM-blue.svg)](file:///c:/Users/nad/Desktop/c-robot-arm/include/config.h)
[![Framework](https://img.shields.io/badge/Platform-PlatformIO%20%7C%20Arduino-orange.svg)](file:///c:/Users/nad/Desktop/c-robot-arm/platformio.ini)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](file:///c:/Users/nad/Desktop/c-robot-arm/LICENSE)

An industrial-grade, open-source robotics control software developed for **4-DOF Cantilever Type Robot Arms**. Powered by an **ESP32 Dev Module (38-Pin)** and the **PCA9685 16-Channel 12-Bit PWM I2C Servo Controller**, this project provides real-time smooth motion generation, geometric Inverse Kinematics (IK), Teach-and-Repeat pose memory, an embedded touch-friendly Web Dashboard, and high-speed Serial CLI control.

---

## 🔍 ESP32 Hardware Detection Status

The target hardware has been scanned and verified on your system:

| Parameter | Detected Hardware Value |
| :--- | :--- |
| **Connection Status** | 🟢 **DETECTED & CONNECTED** |
| **Serial Port** | `COM4` |
| **USB-to-UART Bridge** | `Silicon Labs CP210x USB to UART Bridge` |
| **Chip Model** | `ESP32-D0WD-V3 (Revision v3.1)` |
| **CPU / Clock** | `Dual-Core LX6 @ 240MHz` |
| **Crystal Frequency** | `40 MHz` |
| **MAC Address** | `84:1f:e8:68:b3:94` |
| **Features** | `Wi-Fi, Bluetooth Classic / BLE, Dual Core, LP Core` |

---

## ✨ Key Features

- **🕹️ 4-DOF Cantilever Joint Control**: Precise angle control for Base Yaw ($J_1$), Shoulder Pitch ($J_2$), Elbow Pitch ($J_3$), and Wrist/Gripper ($J_4$).
- **⚡ PCA9685 12-Bit Resolution**: Offloads PWM generation from the ESP32 CPU via I2C (`0x40`) at 400kHz fast mode.
- **📐 Real-Time Inverse Kinematics (IK)**: Analytic geometric solver for Cartesian coordinates $(X, Y, Z)$ and end-effector pitch angle.
- **🌊 Smooth Acceleration & Velocity Trajectories**: Prevents physical cantilever shaking, servo gear stripping, and current spikes.
- **📱 Standalone Glassmorphic Web Dashboard**: Embedded HTML5/CSS3/JavaScript interface hosted directly on the ESP32 via Wi-Fi AP (`http://192.168.4.1`). Includes a live 2D/3D kinematic canvas visualizer.
- **🎬 Teach & Repeat Memory Engine**: Record keyframe poses into onboard memory with custom dwell times and loop playback.
- **🚨 Multi-Layer Safety & E-Stop**: Software emergency stop API, hardware pin E-Stop, and soft angle boundary constraints.
- **💻 Dual Protocol Support**: High-speed USB Serial ASCII parser and JSON REST API.

---

## 📂 Project Folder Structure

```text
c-robot-arm/
├── platformio.ini                   # PlatformIO build configuration & libraries
├── README.md                        # Primary project documentation
├── LICENSE                          # MIT Open-Source License
├── .gitignore                       # Git exclusion rules
│
├── include/                         # C++ Header Files
│   ├── config.h                     # Hardware pins, PCA9685 address, servo calibration limits
│   ├── ServoController.h            # PCA9685 low-level PWM driver & smooth interpolation
│   ├── Kinematics.h                 # Forward & Inverse Kinematics solver engine
│   ├── RobotArm.h                   # High-level motion coordinator & Teach-and-Repeat engine
│   └── WebServerController.h        # WiFi AP manager & embedded HTTP REST server
│
├── src/                             # C++ Source Code
│   ├── main.cpp                     # Core application entry point (setup & loop)
│   ├── ServoController.cpp          # Smooth PWM joint driver implementation
│   ├── Kinematics.cpp               # 4-DOF trigonometric IK & reachability algorithm
│   ├── RobotArm.cpp                 # Motion planning & command dispatcher
│   └── WebServerController.cpp      # REST endpoints & embedded Web Dashboard UI
│
├── docs/                            # Comprehensive Technical Guides
│   ├── HARDWARE_WIRING.md           # Pinout maps, power distribution & electrical safety
│   ├── KINEMATICS_GUIDE.md          # Trigonometric equations & DH transformation models
│   └── SERIAL_PROTOCOL.md           # ASCII Serial CLI & REST JSON API specifications
│
└── examples/                        # Standalone Diagnostic & Utility Sketches
    ├── 01_pca9685_i2c_scanner/      # I2C scanner to verify PCA9685 address (0x40)
    ├── 02_servo_calibration/        # PWM pulse width calibration tool (0° - 180°)
    └── 03_arm_sweep_test/           # Demonstration sweep routine for all 4 joints
```

---

## 🔌 Hardware Pinout & Wiring Table

> ⚠️ **CRITICAL POWER WARNING**: Do **NOT** power servos directly from the ESP32 board pins! Use an external **5V - 6V DC 5A Power Supply** connected to the green screw terminal on the PCA9685 board. Connect ESP32 GND and Servo Power GND together.

### 1. ESP32 38-Pin to PCA9685 Board Wiring

| ESP32 Pin (38-Pin Module) | PCA9685 Pin | Cable Function | Notes |
| :---: | :---: | :--- | :--- |
| **3V3** | **VCC** | Red | Logic power (3.3V) |
| **GND** | **GND** | Black | Common Ground |
| **GPIO 21** | **SDA** | Blue | I2C Data Line |
| **GPIO 22** | **SCL** | Yellow | I2C Clock Line |
| **GPIO 19** | **OE** | Green | Output Enable (Active LOW) |
| **GPIO 15** | **E-STOP** | White | Hardware Emergency Stop Button (Optional) |

### 2. PCA9685 Servo Channel Assignment

| PCA9685 Channel | Arm Joint | Joint Motion | Default Range |
| :---: | :--- | :--- | :---: |
| **Channel 0** | **Joint 1 ($J_1$)** | Base Yaw Rotation | $0^\circ \text{ to } 180^\circ$ |
| **Channel 1** | **Joint 2 ($J_2$)** | Shoulder Cantilever Boom | $15^\circ \text{ to } 165^\circ$ |
| **Channel 2** | **Joint 3 ($J_3$)** | Elbow Forearm Extension | $10^\circ \text{ to } 170^\circ$ |
| **Channel 3** | **Joint 4 ($J_4$)** | Wrist Pitch / End-Effector Gripper | $0^\circ \text{ to } 120^\circ$ |

---

## 🚀 Quick Start Guide

### Option A: Using PlatformIO (Recommended)

1. Open VSCode and install the **PlatformIO IDE** extension.
2. Clone or open the project folder `c-robot-arm`.
3. Connect your ESP32 board via USB (Verified on `COM4`).
4. Click **PlatformIO: Build** or run:
   ```bash
   pio run
   ```
5. Flash firmware to ESP32:
   ```bash
   pio run --target upload --upload-port COM4
   ```
6. Open Serial Monitor at **115200 baud**:
   ```bash
   pio device monitor -b 115200 -p COM4
   ```

### Option B: Using Arduino IDE

1. Open **Arduino IDE** (v2.x or later).
2. Go to **File -> Preferences** and add the ESP32 board manager URL:
   `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
3. Install **ESP32** board package via Board Manager.
4. Install required libraries via Library Manager:
   - `Adafruit PWM Servo Driver Library` (v3.0.1+)
   - `ArduinoJson` (v7.0.4+)
5. Select Board: **ESP32 Dev Module**, Port: **COM4**.
6. Open `src/main.cpp` (or rename to `.ino`), compile, and upload!

---

## 🌐 Web Dashboard User Interface

Once powered on, the ESP32 broadcasts its own Wi-Fi Access Point:

- **SSID**: `ESP32-RobotArm`
- **Password**: `robot1234`
- **Dashboard URL**: `http://192.168.4.1`

### Features of the Web Dashboard:
1. **Direct Joint Sliders**: Real-time slider control for $J_1, J_2, J_3, J_4$ with live degree readouts.
2. **Inverse Kinematics Input**: Enter target $X, Y, Z$ positions in millimeters and click **Move to Coordinate**.
3. **Kinematic Canvas Visualizer**: Live 2D side-view rendering of the arm joints and links.
4. **Teach & Repeat Panel**: Click **Record Pose** to capture current keyframe, then click **Play Once** or **Loop Play**.
5. **Global Emergency Stop**: Single-tap red E-STOP button to instantly halt all servo PWM pulses.

---

## 💬 Serial CLI Commands

Connect via Serial Monitor (`115200 Baud`, Newline `\n` line ending):

| Command | Description | Example |
| :--- | :--- | :--- |
| `J <j1> <j2> <j3> <j4>` | Move joints directly to angles in degrees | `J 90 45 60 30` |
| `IK <x> <y> <z> [pitch]` | Move end effector to Cartesian coordinate (mm) | `IK 120 0 100 0` |
| `HOME` | Move arm to default safe home position | `HOME` |
| `REST` | Park arm in compact rest position | `REST` |
| `REC` | Save current joint angles into teach keyframe memory | `REC` |
| `PLAY` | Execute recorded pose sequence once | `PLAY` |
| `LOOP` | Execute recorded pose sequence continuously | `LOOP` |
| `CLEAR` | Erase teach memory buffer | `CLEAR` |
| `ESTOP` | Emergency stop all servos | `ESTOP` |
| `RESET_ESTOP` | Clear emergency stop state | `RESET_ESTOP` |

---

## 🔧 Servo Calibration Procedure

Servos vary in internal pulse width calibration. To ensure exact 0° to 180° mapping:

1. Flash the calibration sketch located at `examples/02_servo_calibration/02_servo_calibration.ino`.
2. Open Serial Monitor at `115200 baud`.
3. Send test tick commands:
   - `C 0 T 125` -> Moves Channel 0 to approx 0°
   - `C 0 T 575` -> Moves Channel 0 to approx 180°
4. Adjust `SERVOMIN` and `SERVOMAX` values in [include/config.h](file:///c:/Users/nad/Desktop/c-robot-arm/include/config.h) based on your hardware measurements.

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](file:///c:/Users/nad/Desktop/c-robot-arm/LICENSE) file for details.
