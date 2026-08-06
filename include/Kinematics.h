#ifndef KINEMATICS_H
#define KINEMATICS_H

#include <Arduino.h>
#include "config.h"

struct Vector3D {
    float x;
    float y;
    float z;
};

struct JointAngles {
    float j1; // Base Yaw (degrees)
    float j2; // Shoulder Pitch (degrees)
    float j3; // Elbow Pitch (degrees)
    float j4; // Wrist Pitch / Gripper (degrees)
    bool isValid;
};

class Kinematics {
private:
    float l0; // Base height
    float l1; // Shoulder link length
    float l2; // Forearm link length
    float l3; // Gripper/Tool length

public:
    Kinematics(float baseH = LINK_BASE_HEIGHT_L0, 
               float shoulderL = LINK_SHOULDER_L1, 
               float forearmL = LINK_FOREARM_L2, 
               float gripperL = LINK_GRIPPER_L3);

    // Forward Kinematics: Joint Angles (deg) -> Cartesian Position (X, Y, Z, Pitch)
    Vector3D forwardKinematics(const JointAngles& angles, float& outPitchDeg);

    // Inverse Kinematics: Target (X, Y, Z) and Pitch Angle (deg) -> Joint Angles (deg)
    JointAngles inverseKinematics(float x, float y, float z, float pitchDeg = 0.0f);

    // Workspace check
    bool isReachable(float x, float y, float z, float pitchDeg = 0.0f);
    
    // Get max theoretical reach
    float getMaxReach() const { return l1 + l2 + l3; }
};

#endif // KINEMATICS_H
