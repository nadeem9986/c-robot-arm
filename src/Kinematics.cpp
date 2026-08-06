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

Vector3D Kinematics::forwardKinematics(const JointAngles& angles, float& outPitchDeg) {
    Vector3D pos = {0.0f, 0.0f, 0.0f};

    float th1 = angles.j1 * DEG_TO_RAD;
    float th2 = angles.j2 * DEG_TO_RAD;
    float th3 = angles.j3 * DEG_TO_RAD;
    float th4 = angles.j4 * DEG_TO_RAD;

    // Planar projection reach in XY plane
    float r = l1 * cos(th2) + l2 * cos(th2 + th3) + l3 * cos(th2 + th3 + th4);
    
    pos.x = r * cos(th1);
    pos.y = r * sin(th1);
    pos.z = l0 + l1 * sin(th2) + l2 * sin(th2 + th3) + l3 * sin(th2 + th3 + th4);

    outPitchDeg = (th2 + th3 + th4) * RAD_TO_DEG;
    return pos;
}

JointAngles Kinematics::inverseKinematics(float x, float y, float z, float pitchDeg) {
    JointAngles result = {0, 0, 0, 0, false};

    // 1. Calculate Base Angle J1
    float theta1 = atan2(y, x);
    float r = sqrt(x * x + y * y);

    // 2. Wrist Center Position (w_r, w_z) factoring in pitch angle
    float pitchRad = pitchDeg * DEG_TO_RAD;
    float wr = r - l3 * cos(pitchRad);
    float wz = z - l0 - l3 * sin(pitchRad);

    // 3. Distance from shoulder joint to wrist center
    float d2 = wr * wr + wz * wz;
    float d = sqrt(d2);

    // Check reachability triangle
    if (d > (l1 + l2) || d < fabs(l1 - l2) || d == 0) {
        result.isValid = false;
        return result;
    }

    // 4. Calculate Elbow Angle J3 using Law of Cosines
    float cosAlpha3 = (l1 * l1 + l2 * l2 - d2) / (2.0f * l1 * l2);
    cosAlpha3 = constrain(cosAlpha3, -1.0f, 1.0f);
    float alpha3 = acos(cosAlpha3); // Internal angle
    float theta3 = -(M_PI - alpha3); // Elbow down configuration

    // 5. Calculate Shoulder Angle J2
    float beta = atan2(wz, wr);
    float cosAlpha2 = (l1 * l1 + d2 - l2 * l2) / (2.0f * l1 * d);
    cosAlpha2 = constrain(cosAlpha2, -1.0f, 1.0f);
    float alpha2 = acos(cosAlpha2);
    float theta2 = beta + alpha2;

    // 6. Calculate Wrist Pitch Angle J4
    float theta4 = pitchRad - (theta2 + theta3);

    // Convert Radians to Degrees
    result.j1 = theta1 * RAD_TO_DEG;
    result.j2 = theta2 * RAD_TO_DEG;
    result.j3 = theta3 * RAD_TO_DEG;
    result.j4 = theta4 * RAD_TO_DEG;

    // Enforce Joint Limits
    if (result.j1 < J1_MIN_DEG || result.j1 > J1_MAX_DEG ||
        result.j2 < J2_MIN_DEG || result.j2 > J2_MAX_DEG ||
        result.j3 < J3_MIN_DEG || result.j3 > J3_MAX_DEG ||
        result.j4 < J4_MIN_DEG || result.j4 > J4_MAX_DEG) {
        result.isValid = false;
    } else {
        result.isValid = true;
    }

    return result;
}

bool Kinematics::isReachable(float x, float y, float z, float pitchDeg) {
    JointAngles res = inverseKinematics(x, y, z, pitchDeg);
    return res.isValid;
}
