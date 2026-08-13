# 🤖 ESP32 4-DOF MeArm Parallel Cantilever Robot Arm Control Software

> **🤖 AI AGENT QUICK-CONTEXT & ARCHITECTURE SUMMARY**  
> *(Token-efficient cheatsheet for AI Pair-Programmers & Subagents)*  
> - **Hardware**: ESP32-D0WD-V3 (38-Pin Dev Module) on `COM4` + PCA9685 16-Ch 12-Bit PWM Driver (`0x40` I2C).
> - **Pinout**: `SDA=GPIO21`, `SCL=GPIO22`, `OE=-1` (Disconnected, hardware pull-down enabled).
> - **Joint Servo Channels & Limits**:
>   - $J_1$ (Base Yaw): `CH 0` | $0^\circ - 180^\circ$ (Def: $90^\circ$)
>   - $J_2$ (Shoulder Boom): `CH 1` | $15^\circ - 165^\circ$ (Def: $90^\circ$)
>   - $J_3$ (Elbow Linkage): `CH 2` | $10^\circ - 170^\circ$ (Def: $90^\circ$)
>   - $J_4$ (Gripper Claw): `CH 3` | **$0^\circ - 17^\circ$** (Def: $10^\circ$)
> - **FreeRTOS Dual-Core Architecture**:
>   - **Core 0**: Wi-Fi Stack, Web Server & REST/JSON API (`webServerTask`, Prio 1).
>   - **Core 1**: 100Hz Real-Time Trajectory Engine & PWM Hardware Driver (`motionControlTask`, Prio 2).
> - **Kinematic Model**: MeArm 4-Bar Parallel Linkage ($L_0=50\text{mm}, L_1=110\text{mm}, L_2=100\text{mm}, L_3=50\text{mm}$). $J_3$ angle is ground-referenced. Top parallel rod keeps wrist platform level.
> - **Wi-Fi Config**: Station Mode (`SSID: "AMANAT"`, `Pass: "amanath@123"`). AP Fallback (`ESP32-RobotArm` / `robot1234`).
> - **Boot Mode**: `ENABLE_AUTO_HOME_ON_BOOT false` (Passive mode: zero movement on startup until commanded).
> - **Web UI**: Embedded HTML5/Three.js 3D WebGL Dashboard with 360° orbit view and real-time joint articulation.
> - **Build & Flash**: `python -m platformio run --target upload --upload-port COM4`

---

## 🔍 Hardware Telemetry & Connection Status

| Parameter | Value |
| :--- | :--- |
| **Status** | 🟢 **DETECTED & CONNECTED** |
| **Port** | `COM4` (`Silicon Labs CP210x USB to UART Bridge`) |
| **Target Microcontroller** | `ESP32-D0WD-V3` (Revision 3.1) @ 240MHz |
| **Dual-Core Distribution** | Core 0: Wi-Fi & Web UI \| Core 1: 100Hz Motion Trajectory |
| **I2C Bus** | GPIO 21 (SDA), GPIO 22 (SCL) @ 400kHz Fast Mode |
| **PWM Controller** | PCA9685 at I2C Address `0x40` (50Hz Servo Frequency) |

---

## 📂 Codebase Map & Responsibilities

```text
c-robot-arm/
├── platformio.ini                   # Build settings, platform = espressif32, board = esp32dev
├── README.md                        # AI-optimized context & comprehensive project guide
├── include/
│   ├── config.h                     # Pinout definitions, joint limits, Wi-Fi credentials
│   ├── ServoController.h            # PCA9685 driver, pulse tick mapping (125-575), limits
│   ├── Kinematics.h                 # MeArm parallel 4-bar FK & IK trigonometric equations
│   ├── RobotArm.h                   # High-level coordinator, Teach & Repeat memory buffer
│   └── WebServerController.h        # Embedded HTTP Web Server & JSON API dispatcher
├── src/
│   ├── main.cpp                     # FreeRTOS Dual-Core task spawner (setup & dual loops)
│   ├── ServoController.cpp          # 100Hz trajectory interpolation engine
│   ├── Kinematics.cpp               # Analytic IK solver for (X, Y, Z) Cartesian coordinates
│   ├── RobotArm.cpp                 # Command dispatcher & pose keyframe storage
│   └── WebServerController.cpp      # PROGMEM index.html with Three.js 3D visualizer
└── docs/                            # KINEMATICS_GUIDE.md, HARDWARE_WIRING.md, SERIAL_PROTOCOL.md
```

---

## 📥 Flash & Upload Guide

### Option 1: PlatformIO CLI (Recommended)
1. Verify device connection:
   ```bash
   python -m platformio device list
   ```
2. Build and upload to ESP32:
   ```bash
   python -m platformio run --target upload --upload-port COM4
   ```
3. Open Serial Monitor (115200 Baud):
   ```bash
   python -m platformio device monitor -b 115200 -p COM4
   ```

### Option 2: ESP32 Manual Bootloader Mode (If Upload Errors Occur)
If `Could not open COM4` or connection timeout occurs:
1. Hold down the **BOOT (IO0)** button on the ESP32 module.
2. Press and release the **EN / RST** button once.
3. Release the **BOOT** button. (ESP32 is now in flashing mode).
4. Run the upload command.

---

## 💬 Serial & Web API Reference

| Endpoint / Command | Description | Example |
| :--- | :--- | :--- |
| `GET /api/status` | Returns JSON joint angles, XYZ, limits, and E-Stop state | - |
| `GET /api/move?j1=..&j2=..&j3=..&j4=..` | Move joints to target angles | `/api/move?j1=90&j2=90&j3=90&j4=10` |
| `GET /api/ik?x=..&y=..&z=..` | Move to Cartesian position (mm) | `/api/ik?x=120&y=0&z=100` |
| `GET /api/limits?j1_min=..` | Update dynamic servo angle bounds | `/api/limits?j4_min=0&j4_max=17` |
| Serial CLI: `J <j1> <j2> <j3> <j4>` | Direct joint move over Serial | `J 90 90 90 10` |
| Serial CLI: `HOME` / `REST` | Move arm to preset positions | `HOME` |

---

## 📄 License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
