# 🤖 ESP32 4-DOF MeArm Parallel Cantilever Robot Arm Control Software

[![ESP32 Hardware Verified](https://img.shields.io/badge/ESP32-Detected%20on%20COM4-brightgreen.svg)](README.md)
[![Driver](https://img.shields.io/badge/PCA9685-16--Channel%2012--Bit%20PWM-blue.svg)](include/config.h)
[![Framework](https://img.shields.io/badge/Platform-PlatformIO%20%7C%20Arduino-orange.svg)](platformio.ini)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

An industrial-grade, open-source robotics control software developed for **4-DOF Cantilever / MeArm Parallel Type Robot Arms**. Powered by an **ESP32 Dev Module (38-Pin)** and the **PCA9685 16-Channel 12-Bit PWM I2C Servo Controller**, this project features **FreeRTOS Dual-Core Multitasking**, real-time smooth motion generation, geometric Inverse Kinematics (IK), Teach-and-Repeat pose memory, an embedded touch-friendly **3D WebGL Dashboard**, and high-speed Serial CLI control.

---

## 🔍 ESP32 Hardware Detection Status

The target hardware has been scanned and verified on your system:

| Parameter | Detected Hardware Value |
| :--- | :--- |
| **Connection Status** | 🟢 **DETECTED & CONNECTED** |
| **Serial Port** | `COM4` |
| **USB-to-UART Bridge** | `Silicon Labs CP210x USB to UART Bridge` |
| **Chip Model** | `ESP32-D0WD-V3 (Revision v3.1)` |
| **CPU / Clock** | `Dual-Core Xtensa LX6 @ 240MHz` |
| **Dual-Core Tasking** | **Core 0** (Wi-Fi / Web Server) \| **Core 1** (100Hz Motion Task) |
| **Crystal Frequency** | `40 MHz` |
| **MAC Address** | `84:1f:e8:68:b3:94` |

---

## ✨ Key Features

- **🧠 FreeRTOS Dual-Core Architecture**:
  - **Core 0**: Handles Wi-Fi connection, HTTP requests, and WebSocket/JSON APIs.
  - **Core 1**: Dedicated 100Hz real-time servo trajectory engine (zero network latency jitter).
- **🕹️ 4-DOF Parallel Cantilever Joint Control**: Precise angle control for Base Yaw ($J_1$), Shoulder Pitch ($J_2$), Elbow Pitch ($J_3$), and Wrist/Gripper ($J_4$).
- **🔒 Servo Degree Safety Limits**: Custom boundaries per joint (including $0^\circ - 17^\circ$ safety limit for $J_4$ gripper).
- **⚡ PCA9685 12-Bit Resolution**: Offloads PWM generation from the ESP32 CPU via I2C (`0x40`) at 400kHz fast mode.
- **📐 Ground-Referenced MeArm Kinematics**: Decoupled parallel-linkage Forward and Inverse Kinematics solver.
- **📱 Real-Time 3D WebGL Dashboard**: Embedded HTML5/Three.js interface with 360° interactive 3D model, orbit controls, and live joint synchronization.
- **🎬 Teach & Repeat Memory Engine**: Record keyframe poses into onboard memory with custom dwell times and loop playback.
- **🚨 Multi-Layer Safety & E-Stop**: Software emergency stop API, hardware pin E-Stop, and soft angle boundary constraints.

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
│   ├── Kinematics.h                 # Ground-referenced MeArm Kinematics solver engine
│   ├── RobotArm.h                   # High-level motion coordinator & Teach-and-Repeat engine
│   └── WebServerController.h        # WiFi AP manager & embedded HTTP REST server
│
├── src/                             # C++ Source Code
│   ├── main.cpp                     # Dual-core FreeRTOS task manager (setup & loop)
│   ├── ServoController.cpp          # Smooth PWM joint driver implementation
│   ├── Kinematics.cpp               # MeArm parallel linkage 4-DOF trigonometric solver
│   ├── RobotArm.cpp                 # Motion planning & command dispatcher
│   └── WebServerController.cpp      # REST endpoints & embedded 3D WebGL Dashboard UI
│
├── docs/                            # Comprehensive Technical Guides
│   ├── HARDWARE_WIRING.md           # Pinout maps, power distribution & electrical safety
│   ├── KINEMATICS_GUIDE.md          # MeArm 4-bar parallel linkage mathematical equations
│   └── SERIAL_PROTOCOL.md           # ASCII Serial CLI & REST JSON API specifications
│
└── acrylic-robot-4-dof-robot-mechanical-arm-1.snapshot.3/ # Inventor 3D CAD Files & Assemblies
```

---

## 🔌 Hardware Pinout & Wiring Table

> ⚠️ **CRITICAL POWER WARNING**: Do **NOT** power servos directly from the ESP32 board pins! Use an external **5V DC 3A - 5A Power Supply** connected to the green screw terminal on the PCA9685 board. Connect ESP32 GND and Servo Power GND together.

### 1. ESP32 38-Pin to PCA9685 Board Wiring

| ESP32 Pin (38-Pin Module) | PCA9685 Pin | Cable Function | Notes |
| :---: | :---: | :--- | :--- |
| **3V3** | **VCC** | Red | Logic power (3.3V) |
| **GND** | **GND** | Black | Common Ground |
| **GPIO 21** | **SDA** | Blue | I2C Data Line |
| **GPIO 22** | **SCL** | Yellow | I2C Clock Line |
| **OE Pin** | *Disconnected* | - | OE pin disconnected (`-1`): onboard pull-down enables outputs |

### 2. PCA9685 Servo Channel Assignment & Calibrated Limits

| PCA9685 Channel | Arm Joint | Joint Motion | Safe Range |
| :---: | :--- | :--- | :---: |
| **Channel 0** | **Joint 1 ($J_1$)** | Base Yaw Rotation | $0^\circ \text{ to } 180^\circ$ |
| **Channel 1** | **Joint 2 ($J_2$)** | Shoulder Cantilever Boom | $15^\circ \text{ to } 165^\circ$ |
| **Channel 2** | **Joint 3 ($J_3$)** | Elbow Forearm Extension | $10^\circ \text{ to } 170^\circ$ |
| **Channel 3** | **Joint 4 ($J_4$)** | Wrist / Gripper Claw | **$0^\circ \text{ to } 17^\circ$** |

---

## 📥 Comprehensive Step-by-Step Upload & Flashing Guide

Follow these instructions to compile and flash the firmware to your ESP32 board.

### Step 1: Putting ESP32 into Bootloader Mode (If Required)
Most ESP32 Dev Modules have auto-reset circuitry (RTS/DTR pins). However, if flashing fails with `Could not open COM4` or `Failed to connect to ESP32`:
1. Connect the ESP32 to your PC via a micro-USB cable.
2. Hold down the **BOOT (IO0)** button on the ESP32 board.
3. Press and release the **EN / RST** button once while holding BOOT.
4. Release the **BOOT** button. The ESP32 is now in flashing mode (`waiting for download`).

---

### Step 2: Flashing Firmware via PlatformIO CLI (Recommended)

1. Open a Terminal / PowerShell window in your project root folder (`c-robot-arm`).
2. Verify your ESP32 COM port (e.g., `COM4`):
   ```bash
   python -m platformio device list
   ```
3. Compile the source code:
   ```bash
   python -m platformio run
   ```
4. Upload firmware to the ESP32:
   ```bash
   python -m platformio run --target upload --upload-port COM4
   ```
5. Open Serial Monitor at **115200 Baud**:
   ```bash
   python -m platformio device monitor -b 115200 -p COM4
   ```

---

### Step 3: Flashing Firmware via VS Code GUI

1. Install the **PlatformIO IDE** extension in VS Code.
2. Open the project workspace folder `c-robot-arm`.
3. In the left activity bar, click the **PlatformIO Alien icon** (🛸).
4. Under **Project Tasks -> esp32dev**:
   - Click **Build** to compile the project.
   - Click **Upload** to flash firmware to `COM4`.
   - Click **Monitor** to open the 115200 baud serial console.

---

## 🌐 Web Dashboard User Interface

Connect to your Wi-Fi network (or station AP):
- **Dashboard URL**: `http://192.168.31.174` (or ESP32 local IP printed over Serial)
- **Fallback AP SSID**: `ESP32-RobotArm` (Pass: `robot1234`)

### Features of the Web Console:
1. **Interactive 3D Robot Visualizer**: Three.js WebGL rendering of the MeArm parallel linkage with 360° camera orbit controls.
2. **Direct Joint Sliders**: Real-time slider control for $J_1, J_2, J_3, J_4$ ($J_4$ capped at $17^\circ$).
3. **Cartesian Inverse Kinematics**: Enter target $X, Y, Z$ positions in millimeters and execute move.
4. **Teach & Repeat Panel**: Click **Record Pose** to capture keyframes, then run single or loop playback.
5. **Global Emergency Stop**: Instant hardware PWM lockout button.

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
