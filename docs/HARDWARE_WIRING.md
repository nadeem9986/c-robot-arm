# 🔌 Hardware Wiring & Pinout Guide

This document details the exact hardware wiring for controlling a **4-DOF Cantilever Robot Arm** using an **ESP32 Dev Module (38-Pin)** and a **PCA9685 16-Channel 12-Bit PWM I2C Driver Board**.

---

## 📌 Components Required

| Component | Quantity | Notes / Specifications |
| :--- | :---: | :--- |
| **ESP32 Dev Module** | 1 | 38-Pin NodeMCU ESP-WROOM-32 |
| **PCA9685 Module** | 1 | 16-Channel PWM Servo Controller (I2C) |
| **Servos** | 4 | MG996R / MG995 / SG90 (4.8V - 6.0V) |
| **External Power Supply** | 1 | 5V - 6V DC @ 3A to 5A (For servos) |
| **Logic Power** | 1 | 5V USB (For ESP32 logic) |
| **Capacitor** | 1 | 1000µF - 4700µF (Across PCA9685 power terminal) |
| **Jumper Wires** | Several | Male-to-Female & Male-to-Male |

---

## ⚡ ESP32 (38-Pin) to PCA9685 Signal Wiring

> ⚠️ **CRITICAL SAFETY NOTE**: Do **NOT** power high-torque servos directly from the ESP32 3.3V or 5V VIN pins. High current draw will brownout or permanently damage the ESP32. Power the servos through the screw terminal block of the PCA9685 board.

```
       ESP32 (38-Pin)                    PCA9685 Servo Driver
  +-----------------------+            +-----------------------+
  |                   3V3 |----------->| VCC (Logic Power 3.3V)|
  |                   GND |-----+----->| GND (Common Ground)   |
  |             GPIO 21   |----/------>| SDA (I2C Data)        |
  |             GPIO 22   |----------->| SCL (I2C Clock)       |
  |             GPIO 19   |----------->| OE  (Output Enable)   |
  +-----------------------+            +-----------------------+
                                                   |
   External 5V/6V Power Supply                      |
  +-----------------------+                        |
  |               V+ (5V) |------------------------+ (Power Terminal V+)
  |               GND     |------------------------+ (Power Terminal GND)
  +-----------------------+
```

| ESP32 Pin (38-Pin) | PCA9685 Pin | Wire Color / Function | Description |
| :---: | :---: | :--- | :--- |
| **3V3** | **VCC** | Red (Logic) | 3.3V Logic Supply for PCA9685 chip |
| **GND** | **GND** | Black (GND) | Common ground reference |
| **GPIO 21** | **SDA** | Blue (I2C Data) | Primary ESP32 Hardware I2C Data |
| **GPIO 22** | **SCL** | Yellow (I2C Clock)| Primary ESP32 Hardware I2C Clock |
| **GPIO 19** | **OE** | Green (Optional) | Output Enable (Active LOW). Pull LOW to enable servos |

---

## 🤖 PCA9685 Servo Channel Mapping

Connect the 3-pin servo headers (GND=Black/Brown, V+=Red, Signal=Yellow/Orange) to the PCA9685 header channels:

```
  PCA9685 Channel      Target Joint                  DOF Function
  ---------------+-----------------------+----------------------------------
     Channel 0    |  Joint 1: Base        | Yaw Rotation (0° to 180°)
     Channel 1    |  Joint 2: Shoulder    | Main Cantilever Boom Elevation
     Channel 2    |  Joint 3: Elbow       | Forearm Extension / Flexion
     Channel 3    |  Joint 4: Wrist/Claw  | End Effector Gripper Open/Close
```

---

## 🔋 Power Distribution & Capacitor Setup

1. **Servo Power Supply**:
   - Servos generate high current spikes (up to 2.5A peak per MG996R servo during initial motion).
   - Use a **5V - 6V DC 5A Bench Power Supply** or high-discharge LiPo with a 5V/5A SBEC connected to the PCA9685 green screw terminal.
2. **Bulk Electrolytic Capacitor**:
   - Solder/screw a **1000µF to 4700µF 10V/16V capacitor** directly across the PCA9685 power terminal pins (Observe polarity: Long pin = `+`, Short pin with line = `-`).
3. **Common Ground**:
   - Ensure the ESP32 GND and External Servo Power GND are connected together at the PCA9685 logic ground pin.
