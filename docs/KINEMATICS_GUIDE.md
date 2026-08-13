# 📐 MeArm 4-DOF Parallel Linkage Kinematics & CAD Analysis

This document details the mechanical architecture and kinematic formulation for the **Acrylic MeArm 4-DOF Mechanical Robot Arm**.

---

## 🏗️ CAD Linkage Analysis

Unlike standard serial-chain industrial manipulators, this robot arm utilizes a **Parallel Four-Bar Linkage Mechanism**:

1. **Base Turntable ($J_1$ / Servo 1 - Base Yaw)**:
   - Rotates the main acrylic chassis horizontally ($0^\circ \le \theta_0 \le 180^\circ$).

2. **Shoulder Boom ($J_2$ / Servo 2 - Lower Boom)**:
   - Mounted on the left chassis side.
   - Actuates the main lower boom arm (Length $L_1 = 120\text{mm}$).
   - $\theta_s$ represents the elevation angle of $L_1$ relative to the horizontal ground plane.

3. **Parallel Forearm Linkage ($J_3$ / Servo 3 - Upper Linkage Bar)**:
   - Mounted on the right chassis side (statically at the base frame!).
   - Drives a rear vertical linkage bar connected to an upper triangular pivot plate.
   - **Parallel Kinematics Principle**:
     - The rear vertical bar forms a parallelogram with lower boom $L_1$.
     - Consequently, Servo 3 angle $\theta_e$ controls the elevation angle of the forearm link ($L_2 = 110\text{mm}$) **directly relative to the horizontal ground plane**, completely decoupled from the shoulder angle $\theta_s$!

4. **Parallel Leveling Wrist & Gripper ($J_4$ / Servo 4 - Claw)**:
   - A top horizontal rod connects the upper triangle plate to the wrist platform.
   - This secondary parallel bar passively keeps the end-effector platform **always level/parallel to the ground plane**.
   - Servo 4 drives the dual-gear rack-and-pinion claw open and closed ($0^\circ = \text{Closed}, 120^\circ = \text{Fully Open}$).

---

## 🧮 Mathematical Kinematic Formulation

### 1. Forward Kinematics (FK)

Given ground-referenced angles $(\theta_0, \theta_s, \theta_e)$ and claw width $G$:

$$R_{\text{wrist}} = L_1 \cos(\theta_s) + L_2 \cos(\theta_e) + L_3$$

$$X = R_{\text{wrist}} \cos(\theta_0)$$

$$Y = R_{\text{wrist}} \sin(\theta_0)$$

$$Z = L_0 + L_1 \sin(\theta_s) + L_2 \sin(\theta_e)$$

Where:
- $L_0 = 85\text{mm}$ (Base height from ground to shoulder pivot)
- $L_1 = 120\text{mm}$ (Lower boom link length)
- $L_2 = 110\text{mm}$ (Forearm link length)
- $L_3 = 65\text{mm}$ (Gripper platform extension)

---

### 2. Inverse Kinematics (IK)

Given target position $(X, Y, Z)$:

1. **Base Yaw Angle $\theta_0$**:
   $$\theta_0 = \arctan2(Y, X)$$
   $$R = \sqrt{X^2 + Y^2}$$

2. **Planar Projection $(W_r, W_z)$**:
   $$W_r = R - L_3$$
   $$W_z = Z - L_0$$
   $$D = \sqrt{W_r^2 + W_z^2} \quad (\text{Distance from shoulder axis to wrist axis})$$

3. **Check Reachability Bounds**:
   $$|L_1 - L_2| \le D \le (L_1 + L_2)$$

4. **Shoulder Boom Angle $\theta_s$ & Forearm Angle $\theta_e$**:
   $$\gamma = \arctan2(W_z, W_r)$$

   $$\cos(\alpha_s) = \frac{L_1^2 + D^2 - L_2^2}{2 L_1 D} \implies \theta_s = \gamma + \arccos(\cos(\alpha_s))$$

   $$\cos(\alpha_e) = \frac{L_2^2 + D^2 - L_1^2}{2 L_2 D} \implies \theta_e = \gamma - \arccos(\cos(\alpha_e))$$
