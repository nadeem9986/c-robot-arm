# Robot Arm Structure, Dimensions, and Motion Limits

This file summarizes the physical structure of the robot arm, the measured model dimensions, and the firmware movement limits used by the ESP32 controller.

## 1. Robot Type

The robot is a 4-DOF MeArm-style parallel linkage robot arm.

It uses:

- 1 base yaw servo for left/right rotation.
- 1 shoulder servo for lifting the main boom.
- 1 elbow/linkage servo for forearm height and reach.
- 1 gripper servo for opening and closing the claw.

The arm is not a simple serial-chain robot. It uses a parallel four-bar linkage. This means the shoulder and forearm links work together mechanically, and the wrist platform stays mostly level because of the parallel rods.

## 2. Overall CAD Model Size

These values were extracted from the Autodesk Viewer model `Assembly1.iam`.

Units: millimeters.

| Dimension Axis | Size |
| --- | ---: |
| X width | 95.000 mm |
| Y height | 174.923 mm |
| Z depth / reach in current pose | 254.342 mm |

Important: this is the bounding box of the model in the pose shown in the Autodesk viewer. It is not the maximum possible reach envelope.

## 3. Main Physical Structure

### Base Assembly

The base is a flat rectangular plate with rounded corners. A rotating platform sits above it on four standoffs. The bottom servo rotates the upper arm assembly.

Measured base plate:

| Part | Size |
| --- | ---: |
| Base plate / `Part7` | 95.0 x 2.8 x 144.0 mm |

The model uses mostly 2.8 mm thick flat plates, likely acrylic or laser-cut sheet material.

### Rotating Upper Base

The upper base contains two vertical side cheek plates. These plates hold the shoulder and linkage servos and provide the main pivot supports.

Measured parts:

| Part | Size |
| --- | ---: |
| Side frame / `Part11` | 2.8 x 54.495 x 60.0 mm |
| Side frame / `Part9` | 2.8 x 45.0 x 60.0 mm |
| Servo mount / `Part8` | 50.0 x 2.8 x 50.0 mm |
| Servo mount / `Part12` | 50.0 x 2.8 x 45.0 mm |
| Horizontal rails / `Part13` | 50.0 x 19.0 x 2.8 mm |

### Arm Linkage

The arm has a main boom, rear linkage, and upper parallel links. These form the four-bar structure that keeps the end platform controlled.

Measured major links:

| Part | Size |
| --- | ---: |
| Rear vertical link / `Part26` | 2.8 x 76.53 x 109.91 mm |
| Side boom / `Part5` | 2.8 x 91.824 x 36.391 mm |
| Side boom / `Part6` | 2.8 x 91.824 x 36.391 mm |
| Upper link / `Part3` | 2.8 x 90.001 x 34.704 mm |
| Parallel link / `Part4` | 2.8 x 87.88 x 24.92 mm |
| Wrist link / `Part4` | 2.8 x 56.583 x 76.961 mm |
| Wrist link / `Part4` | 2.8 x 54.313 x 78.352 mm |

### Gripper Assembly

The front gripper is a small claw mechanism driven by an SG90 servo. It contains two opposing fingers with geared/linked motion.

Measured gripper assembly:

| Part | Size |
| --- | ---: |
| Gripper assembly / `Assembly4` | 40.661 x 51.155 x 74.609 mm |

## 4. Servo Layout

| Joint | Function | PCA9685 Channel | Servo Role |
| --- | --- | ---: | --- |
| J1 | Base yaw | CH0 | Rotates the full arm left/right |
| J2 | Shoulder boom | CH1 | Raises/lowers the main arm |
| J3 | Elbow/parallel linkage | CH2 | Controls forearm height/reach |
| J4 | Gripper claw | CH3 | Opens/closes the claw |

Measured SG90 bounding boxes from the model:

| Servo Position | Size |
| --- | ---: |
| Base SG90 | 12.2 x 30.27 x 36.4 mm |
| Side SG90 | 30.27 x 12.2 x 36.4 mm |
| Side SG90 | 30.27 x 12.2 x 36.4 mm |
| Gripper SG90 | 12.2 x 41.193 x 42.085 mm |

## 5. Firmware Motion Limits

These are the active joint limits in `include/config.h`.

| Joint | Minimum | Maximum | Default |
| --- | ---: | ---: | ---: |
| J1 base yaw | 0 deg | 180 deg | 90 deg |
| J2 shoulder | 15 deg | 165 deg | 90 deg |
| J3 elbow/linkage | 50 deg | 170 deg | 90 deg |
| J4 gripper | 0 deg | 17 deg | 10 deg |

## 6. Coupled Safety Limit

There is an extra protection rule for the shoulder and elbow/linkage.

When J2 is near its minimum angle, J3 must stay above a safer minimum angle:

| Condition | Limit |
| --- | ---: |
| If J2 is about 15 deg | J3 minimum becomes 85 deg |

This is used to prevent mechanical collision in the cantilever linkage.

## 7. Kinematic Link Lengths Used by Firmware

These values are used by the inverse kinematics solver.

| Parameter | Meaning | Length |
| --- | --- | ---: |
| L0 | Ground/base to shoulder joint height | 85 mm |
| L1 | Shoulder/main boom length | 120 mm |
| L2 | Forearm/linkage length | 110 mm |
| L3 | Gripper/tool extension | 65 mm |

Theoretical maximum reach from the shoulder model:

```text
L1 + L2 + L3 = 295 mm
```

This is a theoretical kinematic value. The real usable reach is smaller because of joint limits, plate collisions, servo horn limits, and the gripper geometry.

## 8. Joint Meaning

### J1 Base Yaw

J1 rotates the full upper arm assembly around the vertical base axis.

Firmware range:

```text
0 deg to 180 deg
```

Default center:

```text
90 deg
```

The firmware has `REVERSE_J1_BASE_YAW` enabled, so the logical angle is reversed before sending PWM to the actual servo.

### J2 Shoulder

J2 controls the main cantilever boom angle.

Firmware range:

```text
15 deg to 165 deg
```

Default:

```text
90 deg
```

Lower values bring the arm down/forward. Higher values lift the arm upward/backward depending on physical mounting.

### J3 Elbow / Parallel Linkage

J3 controls the secondary linkage angle. In this robot, J3 is treated as ground-referenced, not simply relative to J2.

Firmware range:

```text
50 deg to 170 deg
```

Default:

```text
90 deg
```

Extra safety:

```text
When J2 is close to 15 deg, J3 is forced to at least 85 deg.
```

### J4 Gripper

J4 controls claw open/close.

Firmware range:

```text
0 deg to 17 deg
```

Default:

```text
10 deg
```

The project code maps:

```text
openGripper()  -> J4 minimum, 0 deg
closeGripper() -> J4 maximum, 17 deg
```

## 9. Preset Positions

### Home

```text
J1 = 90 deg
J2 = 90 deg
J3 = 90 deg
J4 = 10 deg
```

### Rest

Commanded rest position:

```text
J1 = 90 deg
J2 = 30 deg
J3 = 30 deg
J4 = 0 deg
```

Because J3 has a firmware minimum of 50 deg, the actual J3 rest target will be constrained to:

```text
J3 = 50 deg
```

## 10. Motion Speed

Default motion speed:

```text
35 deg/sec
```

Allowed speed range in code:

```text
greater than 0.5 deg/sec and up to 300 deg/sec
```

The web UI speed slider currently uses:

```text
10 deg/sec to 120 deg/sec
```

## 11. Safety Features

### Passive Boot

The firmware is configured for passive boot:

```text
ENABLE_AUTO_HOME_ON_BOOT = false
```

This means the servos do not automatically move when the ESP32 starts.

### Emergency Stop

Emergency stop disables servo PWM output.

Commands:

```text
ESTOP
RESET_ESTOP
```

Optional hardware pin:

```text
GPIO15
```

### Anti-Jitter Protection

Anti-jitter protection is enabled.

Current limit:

```text
8 rapid direction reversals per 1000 ms
```

If this threshold is hit, the controller triggers emergency stop to protect the servos from overheating.

## 12. Notes for 3D Rebuilding

Use these as the most important values when rebuilding the robot arm:

```text
Plate thickness: 2.8 mm
Base plate: 95 x 144 mm
Base yaw: 0 to 180 deg
Shoulder: 15 to 165 deg
Elbow/linkage: 50 to 170 deg
Gripper: 0 to 17 deg
Firmware link model: L0=85, L1=120, L2=110, L3=65 mm
```

For exact hole centers, slot shapes, and contour geometry, use the Inventor part files in `cad/models/` or Autodesk Viewer measurement mode.
