#include "RobotArm.h"

RobotArm::RobotArm()
    : teachCount(0),
      isPlayingSequence(false),
      currentPlaybackIndex(0),
      playbackNextStepMs(0),
      loopPlayback(false) {}

bool RobotArm::begin() {
    Serial.println("[ROBOT] Initializing Robot Arm Engine...");
    if (!servoCtrl.begin()) {
        Serial.println("[ROBOT ERROR] Servo controller failed to initialize!");
        return false;
    }
#if ENABLE_AUTO_HOME_ON_BOOT
    goHome();
#else
    Serial.println("[ROBOT] Ready in passive mode. Waiting for Web / Serial movement commands.");
#endif
    return true;
}

void RobotArm::update() {
    // 1. Update servo smooth interpolation
    servoCtrl.update();

    // 2. Playback teach sequence if active
    if (isPlayingSequence) {
        if (!servoCtrl.isMoving() && millis() >= playbackNextStepMs) {
            if (currentPlaybackIndex < teachCount) {
                PoseKeyframe kf = teachSequence[currentPlaybackIndex];
                Serial.printf("[TEACH PLAYBACK] Step %d/%d -> J1:%.1f, J2:%.1f, J3:%.1f, J4:%.1f\n", 
                              currentPlaybackIndex + 1, teachCount, kf.j1, kf.j2, kf.j3, kf.j4);
                
                moveJoints(kf.j1, kf.j2, kf.j3, kf.j4);
                playbackNextStepMs = millis() + kf.delayMs;
                currentPlaybackIndex++;
            } else {
                if (loopPlayback) {
                    currentPlaybackIndex = 0; // Restart sequence
                } else {
                    isPlayingSequence = false;
                    Serial.println("[TEACH PLAYBACK] Sequence execution completed.");
                }
            }
        }
    }
}

void RobotArm::moveJoints(float j1, float j2, float j3, float j4, bool immediate) {
    if (immediate) {
        servoCtrl.setAllDirect(j1, j2, j3, j4);
    } else {
        servoCtrl.setAllTargets(j1, j2, j3, j4);
    }
}

void RobotArm::moveJoint(uint8_t jointIdx, float angleDeg) {
    servoCtrl.setJointTargetAngle(jointIdx, angleDeg);
}

bool RobotArm::moveCartesian(float x, float y, float z, float pitchDeg) {
    JointAngles solution = solver.inverseKinematics(x, y, z, pitchDeg);
    if (solution.isValid) {
        moveJoints(solution.j1, solution.j2, solution.j3, solution.j4);
        return true;
    } else {
        Serial.printf("[IK ERROR] Target position unreachable: (X:%.1f, Y:%.1f, Z:%.1f, Pitch:%.1f)\n", x, y, z, pitchDeg);
        return false;
    }
}

void RobotArm::goHome() {
    Serial.println("[ROBOT] Moving to HOME position...");
    moveJoints(J1_DEFAULT_DEG, J2_DEFAULT_DEG, J3_DEFAULT_DEG, J4_DEFAULT_DEG);
}

void RobotArm::goRest() {
    Serial.println("[ROBOT] Moving to REST position...");
    moveJoints(90.0f, 30.0f, 30.0f, 0.0f);
}

void RobotArm::openGripper() {
    servoCtrl.setJointTargetAngle(3, J4_MIN_DEG);
}

void RobotArm::closeGripper() {
    servoCtrl.setJointTargetAngle(3, J4_MAX_DEG);
}

bool RobotArm::recordCurrentPose(uint16_t dwellTimeMs) {
    if (teachCount >= MAX_TEACH_POSES) {
        Serial.println("[TEACH ERROR] Pose buffer full!");
        return false;
    }

    teachSequence[teachCount] = {
        servoCtrl.getJointTargetAngle(0),
        servoCtrl.getJointTargetAngle(1),
        servoCtrl.getJointTargetAngle(2),
        servoCtrl.getJointTargetAngle(3),
        dwellTimeMs
    };
    teachCount++;
    Serial.printf("[TEACH] Recorded pose #%d\n", teachCount);
    return true;
}

void RobotArm::clearTeachSequence() {
    teachCount = 0;
    isPlayingSequence = false;
    currentPlaybackIndex = 0;
    Serial.println("[TEACH] Cleared sequence buffer.");
}

void RobotArm::startPlayback(bool loop) {
    if (teachCount == 0) {
        Serial.println("[TEACH ERROR] No recorded poses to play.");
        return;
    }
    loopPlayback = loop;
    currentPlaybackIndex = 0;
    playbackNextStepMs = millis();
    isPlayingSequence = true;
    Serial.println("[TEACH] Starting playback...");
}

void RobotArm::stopPlayback() {
    isPlayingSequence = false;
    Serial.println("[TEACH] Stopped playback.");
}

JointAngles RobotArm::getCurrentJoints() const {
    return {
        servoCtrl.getJointCurrentAngle(0),
        servoCtrl.getJointCurrentAngle(1),
        servoCtrl.getJointCurrentAngle(2),
        servoCtrl.getJointCurrentAngle(3),
        true
    };
}

Vector3D RobotArm::getCurrentCartesian() const {
    float pitch;
    return solver.forwardKinematics(getCurrentJoints(), pitch);
}

String RobotArm::executeCommand(const String& cmd) {
    String trimmed = cmd;
    trimmed.trim();
    if (trimmed.length() == 0) return "ERR: EMPTY";

    if (trimmed == "HOME") {
        goHome();
        return "OK: HOME";
    } else if (trimmed == "REST") {
        goRest();
        return "OK: REST";
    } else if (trimmed == "ESTOP") {
        servoCtrl.emergencyStop();
        return "OK: ESTOPPED";
    } else if (trimmed == "RESET_ESTOP") {
        servoCtrl.resetEmergencyStop();
        return "OK: ESTOP_RESET";
    } else if (trimmed.startsWith("J ")) {
        // Syntax: J <j1> <j2> <j3> <j4>
        float j1, j2, j3, j4;
        if (sscanf(trimmed.c_str(), "J %f %f %f %f", &j1, &j2, &j3, &j4) == 4) {
            moveJoints(j1, j2, j3, j4);
            return "OK: JOINT_MOVED";
        }
    } else if (trimmed.startsWith("IK ")) {
        // Syntax: IK <x> <y> <z> [pitch]
        float x, y, z, pitch = 0.0f;
        int parsed = sscanf(trimmed.c_str(), "IK %f %f %f %f", &x, &y, &z, &pitch);
        if (parsed >= 3) {
            if (moveCartesian(x, y, z, pitch)) {
                return "OK: IK_MOVED";
            } else {
                return "ERR: UNREACHABLE";
            }
        }
    } else if (trimmed == "REC") {
        recordCurrentPose();
        return "OK: RECORDED";
    } else if (trimmed == "PLAY") {
        startPlayback(false);
        return "OK: PLAYING";
    } else if (trimmed == "LOOP") {
        startPlayback(true);
        return "OK: LOOPING";
    } else if (trimmed == "CLEAR") {
        clearTeachSequence();
        return "OK: CLEARED";
    }

    return "ERR: UNKNOWN_CMD";
}
