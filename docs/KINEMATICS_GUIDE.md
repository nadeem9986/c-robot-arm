# 📐 4-DOF Cantilever Arm Kinematics & Geometry Guide

This document describes the kinematic model, forward kinematics (FK), and inverse kinematics (IK) formulation for a **4-DOF Cantilever Type Robotic Arm**.

---

## 🏗️ Robot Arm Structure

The cantilever robot arm consists of 4 revolute joints:

1. **Joint 1 ($\theta_1$) - Base Yaw**: Rotates around the vertical Z-axis in the horizontal XY plane ($0^\circ \le \theta_1 \le 180^\circ$).
2. **Joint 2 ($\theta_2$) - Shoulder Pitch**: Elevates the main cantilever boom $L_1$ relative to the base height $L_0$.
3. **Joint 3 ($\theta_3$) - Elbow Pitch**: Controls the relative angle of the forearm cantilever link $L_2$ relative to link $L_1$.
4. **Joint 4 ($\theta_4$) - Wrist Pitch / Gripper**: Controls the end effector orientation and gripper tool extension $L_3$.

```
                 (Elbow)
                  (J3) *-------[ Forearm L2 ]-------* (J4 Wrist)
                      /                              \
                     /                                \ [ Gripper L3 ]
         [ Boom L1 ]/                                  v  (End Effector X,Y,Z)
                   /
                  /
           (J2)  * (Shoulder)
                 |
                 | [ Base L0 ]
                 |
     ===========[J1 Base]===========  (Ground / XY Plane)
```

---

## 🧮 Forward Kinematics (FK)

Given joint angles $(\theta_1, \theta_2, \theta_3, \theta_4)$, calculate the Cartesian position $(X, Y, Z)$ of the end effector tip:

$$R = L_1 \cos(\theta_2) + L_2 \cos(\theta_2 + \theta_3) + L_3 \cos(\theta_2 + \theta_3 + \theta_4)$$

$$X = R \cos(\theta_1)$$

$$Y = R \sin(\theta_1)$$

$$Z = L_0 + L_1 \sin(\theta_2) + L_2 \sin(\theta_2 + \theta_3) + L_3 \sin(\theta_2 + \theta_3 + \theta_4)$$

$$\phi = \theta_2 + \theta_3 + \theta_4 \quad (\text{End Effector Pitch Angle})$$

---

## 🎯 Inverse Kinematics (IK)

Given target end effector Cartesian position $(X, Y, Z)$ and desired pitch angle $\phi$:

### Step 1: Base Angle $\theta_1$
$$\theta_1 = \arctan2(Y, X)$$

$$\text{Horizontal Radial Distance: } R = \sqrt{X^2 + Y^2}$$

### Step 2: Wrist Center $(W_r, W_z)$
$$W_r = R - L_3 \cos(\phi)$$

$$W_z = Z - L_0 - L_3 \sin(\phi)$$

$$\text{Distance Shoulder-to-Wrist: } D = \sqrt{W_r^2 + W_z^2}$$

### Step 3: Check Reachability Bounds
$$|L_1 - L_2| \le D \le (L_1 + L_2)$$

### Step 4: Elbow Angle $\theta_3$
Using Law of Cosines on triangle formed by $L_1$, $L_2$, and $D$:

$$\cos(\alpha_3) = \frac{L_1^2 + L_2^2 - D^2}{2 L_1 L_2}$$

$$\theta_3 = -\left(\pi - \arccos(\cos(\alpha_3))\right) \quad (\text{Elbow-down configuration})$$

### Step 5: Shoulder Angle $\theta_2$
$$\beta = \arctan2(W_z, W_r)$$

$$\cos(\alpha_2) = \frac{L_1^2 + D^2 - L_2^2}{2 L_1 D}$$

$$\theta_2 = \beta + \arccos(\cos(\alpha_2))$$

### Step 6: Wrist Angle $\theta_4$
$$\theta_4 = \phi - (\theta_2 + \theta_3)$$
