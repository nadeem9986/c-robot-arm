#include "ServoController.h"

ServoController::ServoController()
    : pwm(Adafruit_PWMServoDriver(PCA9685_I2C_ADDR)),
      initialized(false),
      isEmergencyStopped(false),
      speedDPS(DEFAULT_MAX_SPEED_DPS),
      lastUpdateMs(0) {
    
    // Initialize joint limits and channels
    joints[0] = { J1_DEFAULT_DEG, J1_DEFAULT_DEG, J1_MIN_DEG, J1_MAX_DEG, J1_OFFSET_DEG, JOINT1_BASE_CH, true };
    joints[1] = { J2_DEFAULT_DEG, J2_DEFAULT_DEG, J2_MIN_DEG, J2_MAX_DEG, J2_OFFSET_DEG, JOINT2_SHOULDER_CH, true };
    joints[2] = { J3_DEFAULT_DEG, J3_DEFAULT_DEG, J3_MIN_DEG, J3_MAX_DEG, J3_OFFSET_DEG, JOINT3_ELBOW_CH, true };
    joints[3] = { J4_DEFAULT_DEG, J4_DEFAULT_DEG, J4_MIN_DEG, J4_MAX_DEG, J4_OFFSET_DEG, JOINT4_GRIPPER_CH, true };
}

bool ServoController::begin(uint8_t sdaPin, uint8_t sclPin, uint32_t i2cSpeed) {
    Serial.printf("[SERVO] Initializing I2C Bus (SDA: %d, SCL: %d, Speed: %d Hz)...\n", sdaPin, sclPin, i2cSpeed);
    Wire.begin(sdaPin, sclPin, i2cSpeed);

#if defined(PCA9685_OE_PIN) && PCA9685_OE_PIN >= 0
    pinMode(PCA9685_OE_PIN, OUTPUT);
    digitalWrite(PCA9685_OE_PIN, LOW); // Active Low OE
#endif

    pwm.begin();
    pwm.setPWMFreq(SERVO_FREQ_HZ);
    delay(20);

    initialized = true;
    Serial.println("[SERVO] PCA9685 Servo Driver Initialized successfully.");

    // Move to default initial position safely
    setAllDirect(J1_DEFAULT_DEG, J2_DEFAULT_DEG, J3_DEFAULT_DEG, J4_DEFAULT_DEG);
    lastUpdateMs = millis();
    return true;
}

uint16_t ServoController::angleToTicks(float angleDeg, uint8_t jointIndex) {
    if (jointIndex >= 4) return SERVOMIN;

    // Apply calibration offset
    float calibratedAngle = angleDeg + joints[jointIndex].offsetAngle;

    // Constrain to physical limits
    calibratedAngle = constrain(calibratedAngle, joints[jointIndex].minAngle, joints[jointIndex].maxAngle);

    // Map 0.0 - 180.0 degrees to SERVOMIN - SERVOMAX
    float ticks = SERVOMIN + (calibratedAngle / 180.0f) * (SERVOMAX - SERVOMIN);
    return (uint16_t)constrain(ticks, SERVOMIN, SERVOMAX);
}

void ServoController::setJointAngleDirect(uint8_t jointIndex, float angleDeg) {
    if (jointIndex >= 4 || isEmergencyStopped || !initialized) return;

    angleDeg = constrain(angleDeg, joints[jointIndex].minAngle, joints[jointIndex].maxAngle);
    joints[jointIndex].currentAngle = angleDeg;
    joints[jointIndex].targetAngle = angleDeg;

    uint16_t ticks = angleToTicks(angleDeg, jointIndex);
    pwm.setPWM(joints[jointIndex].channel, 0, ticks);
}

void ServoController::setJointTargetAngle(uint8_t jointIndex, float angleDeg) {
    if (jointIndex >= 4 || isEmergencyStopped) return;
    angleDeg = constrain(angleDeg, joints[jointIndex].minAngle, joints[jointIndex].maxAngle);
    joints[jointIndex].targetAngle = angleDeg;
}

void ServoController::setAllTargets(float j1, float j2, float j3, float j4) {
    setJointTargetAngle(0, j1);
    setJointTargetAngle(1, j2);
    setJointTargetAngle(2, j3);
    setJointTargetAngle(3, j4);
}

void ServoController::setAllDirect(float j1, float j2, float j3, float j4) {
    setJointAngleDirect(0, j1);
    setJointAngleDirect(1, j2);
    setJointAngleDirect(2, j3);
    setJointAngleDirect(3, j4);
}

void ServoController::update() {
    if (isEmergencyStopped || !initialized) return;

    unsigned long now = millis();
    float dt = (now - lastUpdateMs) / 1000.0f; // dt in seconds
    if (dt <= 0.0f) return;
    lastUpdateMs = now;

    float maxDelta = speedDPS * dt;

    for (uint8_t i = 0; i < 4; i++) {
        if (!joints[i].enabled) continue;

        float diff = joints[i].targetAngle - joints[i].currentAngle;
        if (fabs(diff) > 0.01f) {
            if (fabs(diff) <= maxDelta) {
                joints[i].currentAngle = joints[i].targetAngle;
            } else {
                if (diff > 0) {
                    joints[i].currentAngle += maxDelta;
                } else {
                    joints[i].currentAngle -= maxDelta;
                }
            }
            uint16_t ticks = angleToTicks(joints[i].currentAngle, i);
            pwm.setPWM(joints[i].channel, 0, ticks);
        }
    }
}

float ServoController::getJointCurrentAngle(uint8_t jointIndex) const {
    if (jointIndex >= 4) return 0.0f;
    return joints[jointIndex].currentAngle;
}

float ServoController::getJointTargetAngle(uint8_t jointIndex) const {
    if (jointIndex >= 4) return 0.0f;
    return joints[jointIndex].targetAngle;
}

bool ServoController::isMoving() const {
    for (uint8_t i = 0; i < 4; i++) {
        if (fabs(joints[i].targetAngle - joints[i].currentAngle) > 0.1f) {
            return true;
        }
    }
    return false;
}

void ServoController::setSpeed(float speedDegPerSec) {
    if (speedDegPerSec > 0.5f && speedDegPerSec <= 300.0f) {
        speedDPS = speedDegPerSec;
    }
}

void ServoController::emergencyStop() {
    isEmergencyStopped = true;
    Serial.println("[EMERGENCY STOP] All Servos Halted!");
    enableOutputs(false);
}

void ServoController::resetEmergencyStop() {
    isEmergencyStopped = false;
    enableOutputs(true);
    // Sync targets with current angles to prevent sudden jump
    for (uint8_t i = 0; i < 4; i++) {
        joints[i].targetAngle = joints[i].currentAngle;
    }
    Serial.println("[SAFETY] Emergency stop reset. Servo control re-enabled.");
}

void ServoController::enableOutputs(bool enable) {
#if defined(PCA9685_OE_PIN) && PCA9685_OE_PIN >= 0
    digitalWrite(PCA9685_OE_PIN, enable ? LOW : HIGH);
#endif
    if (!enable) {
        for (uint8_t i = 0; i < 4; i++) {
            pwm.setPWM(joints[i].channel, 0, 0); // Turn off PWM pulses
        }
    }
}
