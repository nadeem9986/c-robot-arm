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
    float j1; // Base Yaw (degrees: 0 to 180)
    float j2; // Shoulder Boom Angle relative to ground (degrees: 15 to 165)
    float j3; // Forearm Linkage Angle relative to ground (degrees: 10 to 170)
    float j4; // Gripper Claw Opening Width (degrees: 0 = Closed, 120 = Open)
    bool isValid;
};

class Kinematics {
private:
    float l0; // Base height to shoulder axis (mm)
    float l1; // Primary lower boom link length (mm)
    float l2; // Upper forearm link length (mm)
    float l3; // Gripper / claw extension length (mm)

public:
    Kinematics(float baseH = LINK_BASE_HEIGHT_L0, 
               float shoulderL = LINK_SHOULDER_L1, 
               float forearmL = LINK_FOREARM_L2, 
               float gripperL = LINK_GRIPPER_L3);

    // Forward Kinematics for MeArm Parallel Linkage Architecture
    Vector3D forwardKinematics(const JointAngles& angles, float& outPitchDeg) const;

    // Inverse Kinematics for MeArm Parallel Linkage Architecture
    JointAngles inverseKinematics(float x, float y, float z, float pitchDeg = 0.0f) const;

    // Workspace reachability check
    bool isReachable(float x, float y, float z, float pitchDeg = 0.0f) const;
    
    // Get max theoretical reach in mm
    float getMaxReach() const { return l1 + l2 + l3; }
};

#endif // KINEMATICS_H
