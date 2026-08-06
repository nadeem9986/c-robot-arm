# 💬 Serial & REST API Protocol Specification

This document details the command protocol accepted over **USB Serial (115200 Baud)** and the **HTTP REST API** on the ESP32 Cantilever Robot Arm.

---

## 📟 USB Serial Command Protocol

Send ASCII strings terminated with `\n` (newline) over Serial (115200 Baud).

### 1. Joint Control Command (`J`)
Move joints directly by specifying angles in degrees.
* **Syntax**: `J <j1> <j2> <j3> <j4>`
* **Example**: `J 90.0 45.0 60.0 30.0`
* **Response**: `OK: JOINT_MOVED`

### 2. Inverse Kinematics Command (`IK`)
Move end effector to target 3D Cartesian coordinates (in mm).
* **Syntax**: `IK <x> <y> <z> [pitch]`
* **Example**: `IK 120.0 0.0 100.0 0.0`
* **Response**: `OK: IK_MOVED` or `ERR: UNREACHABLE`

### 3. Preset Commands
* `HOME` -> Moves all joints to safe home position (`J1=90°, J2=90°, J3=90°, J4=60°`).
* `REST` -> Moves arm to compact rest position (`J1=90°, J2=30°, J3=30°, J4=0°`).
* `ESTOP` -> Immediately halts all PWM outputs and disables servos.
* `RESET_ESTOP` -> Resets safety lockout and re-enables servos.

### 4. Teach & Repeat Commands
* `REC` -> Records current pose into keyframe memory.
* `PLAY` -> Plays recorded pose sequence once.
* `LOOP` -> Continuously loops recorded pose sequence.
* `CLEAR` -> Flushes recorded pose sequence buffer.

---

## 🌐 HTTP REST API Endpoints

When connected to `ESP32-RobotArm` Wi-Fi (IP `192.168.4.1`):

| Endpoint | Method | Parameters | Description |
| :--- | :---: | :--- | :--- |
| `/` | `GET` | None | Returns embedded interactive HTML Dashboard UI |
| `/api/status` | `GET` | None | Returns JSON status (current joint angles, XYZ, E-Stop state) |
| `/api/move` | `GET` | `j1`, `j2`, `j3`, `j4` | Move joints to target angles |
| `/api/ik` | `GET` | `x`, `y`, `z`, `pitch` | Move to target Cartesian position using IK |
| `/api/teach` | `GET` | `action` (`REC`/`PLAY`/`LOOP`/`CLEAR`) | Control Teach & Repeat sequence buffer |
| `/api/cmd` | `GET` | `c` (`HOME`/`REST`/`ESTOP`/`RESET_ESTOP`) | Execute generic ASCII command |

### Example `/api/status` JSON Response:
```json
{
  "j1": 90.0,
  "j2": 45.0,
  "j3": 60.0,
  "j4": 30.0,
  "x": 120.5,
  "y": 0.0,
  "z": 105.2,
  "isMoving": false,
  "isEStopped": false,
  "teachCount": 4
}
```
