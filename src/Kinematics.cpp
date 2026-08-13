#include "Kinematics.h"
#include <math.h>

#ifndef RAD_TO_DEG
#define RAD_TO_DEG (180.0f / M_PI)
#endif

#ifndef DEG_TO_RAD
#define DEG_TO_RAD (M_PI / 180.0f)
#endif

Kinematics::Kinematics(float baseH, float shoulderL, float forearmL, float gripperL)
    : l0(baseH), l1(shoulderL), l2(forearmL), l3(gripperL) {}

Vector3D Kinematics::forwardKinematics(const JointAngles& angles, float& outPitchDeg) const {
    Vector3D pos = {0.0f, 0.0f, 0.0f};

    float th0 = angles.j1 * DEG_TO_RAD; // Base Yaw
    float th_s = angles.j2 * DEG_TO_RAD; // Shoulder boom angle relative to horizontal
    float th_e = angles.j3 * DEG_TO_RAD; // Forearm angle relative to horizontal via parallel linkage

    // In parallel linkage MeArm design:
    // Radial distance R in XY plane:
    float r = l1 * cos(th_s) + l2 * cos(th_e) + l3; 
    
    pos.x = r * cos(th0);
    pos.y = r * sin(th0);
    pos.z = l0 + l1 * sin(th_s) + l2 * sin(th_e);

    outPitchDeg = 0.0f; // Passively level claw platform
    return pos;
}

JointAngles Kinematics::inverseKinematics(float x, float y, float z, float pitchDeg) const {
    JointAngles result = {0, 0, 0, 0, false};

    // 1. Calculate Base Yaw J1
    float theta0 = atan2(y, x);
    float r = sqrt(x * x + y * y);

    // 2. Adjust for gripper horizontal offset l3 (claw is kept horizontal by top parallel bar)
    float wr = r - l3;
    float wz = z - l0;

    // 3. Distance from shoulder pivot to wrist pivot
    float d2 = wr * wr + wz * wz;
    float d = sqrt(d2);

    // Reachability check
    if (d > (l1 + l2) || d < fabs(l1 - l2) || d == 0) {
        result.isValid = false;
        return result;
    }

    // 4. Calculate Shoulder Boom Angle (j2) and Forearm Linkage Angle (j3)
    // Using Law of Cosines on triangle (L1, L2, D)
    float gamma = atan2(wz, wr); // Base angle to wrist target

    float cosAlphaS = (l1 * l1 + d2 - l2 * l2) / (2.0f * l1 * d);
    cosAlphaS = constrain(cosAlphaS, -1.0f, 1.0f);
    float alphaS = acos(cosAlphaS);
    float thetaS = gamma + alphaS; // Shoulder boom angle relative to horizontal

    float cosAlphaE = (l2 * l2 + d2 - l1 * l1) / (2.0f * l2 * d);
    cosAlphaE = constrain(cosAlphaE, -1.0f, 1.0f);
    float alphaE = acos(cosAlphaE);
    float thetaE = gamma - alphaE; // Forearm linkage angle relative to horizontal

    // Convert Radians to Degrees
    result.j1 = theta0 * RAD_TO_DEG;
    result.j2 = thetaS * RAD_TO_DEG;
    result.j3 = thetaE * RAD_TO_DEG;
    result.j4 = J4_DEFAULT_DEG; // Keep existing gripper setting

    // Enforce Joint Physical Limits
    if (result.j1 < J1_MIN_DEG || result.j1 > J1_MAX_DEG ||
        result.j2 < J2_MIN_DEG || result.j2 > J2_MAX_DEG ||
        result.j3 < J3_MIN_DEG || result.j3 > J3_MAX_DEG) {
        result.isValid = false;
    } else {
        result.isValid = true;
    }

    return result;
}

bool Kinematics::isReachable(float x, float y, float z, float pitchDeg) const {
    JointAngles res = inverseKinematics(x, y, z, pitchDeg);
    return res.isValid;
}
