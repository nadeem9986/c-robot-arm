#ifndef ROBOT_ARM_H
#define ROBOT_ARM_H

#include <Arduino.h>
#include "ServoController.h"
#include "Kinematics.h"

struct PoseKeyframe {
    float j1, j2, j3, j4;
    uint16_t delayMs;
};

class RobotArm {
private:
    ServoController servoCtrl;
    Kinematics solver;

    PoseKeyframe teachSequence[MAX_TEACH_POSES];
    uint8_t teachCount;
    bool isPlayingSequence;
    uint8_t currentPlaybackIndex;
    unsigned long playbackNextStepMs;
    bool loopPlayback;

public:
    RobotArm();

    bool begin();
    void update();

    // Joint level motion
    void moveJoints(float j1, float j2, float j3, float j4, bool immediate = false);
    void moveJoint(uint8_t jointIdx, float angleDeg);

    // Cartesian level motion (Inverse Kinematics)
    bool moveCartesian(float x, float y, float z, float pitchDeg = 0.0f);

    // Preset positions
    void goHome();
    void goRest();
    void openGripper();
    void closeGripper();

    // Teach & Repeat controls
    bool recordCurrentPose(uint16_t dwellTimeMs = 1000);
    void clearTeachSequence();
    void startPlayback(bool loop = false);
    void stopPlayback();
    uint8_t getTeachCount() const { return teachCount; }
    bool isPlaying() const { return isPlayingSequence; }

    // Direct Accessors
    JointAngles getCurrentJoints() const;
    Vector3D getCurrentCartesian() const;
    ServoController& getServoController() { return servoCtrl; }
    Kinematics& getKinematics() { return solver; }

    // Command parser
    String executeCommand(const String& cmd);
};

#endif // ROBOT_ARM_H
