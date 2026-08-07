#ifndef SERVO_CONTROLLER_H
#define SERVO_CONTROLLER_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "config.h"

struct JointState {
    float currentAngle;
    float targetAngle;
    float minAngle;
    float maxAngle;
    float offsetAngle;
    uint8_t channel;
    bool enabled;
};

class ServoController {
private:
    Adafruit_PWMServoDriver pwm;
    JointState joints[4];
    bool initialized;
    bool isEmergencyStopped;
    float speedDPS; // Speed in degrees per second
    unsigned long lastUpdateMs;

    uint16_t angleToTicks(float angleDeg, uint8_t jointIndex);

public:
    ServoController();

    bool begin(uint8_t sdaPin = I2C_SDA_PIN, uint8_t sclPin = I2C_SCL_PIN, uint32_t i2cSpeed = I2C_CLOCK_SPEED);
    
    // Direct joint angle setting (Immediate)
    void setJointAngleDirect(uint8_t jointIndex, float angleDeg);
    
    // Smooth target angle setting (Interpolated)
    void setJointTargetAngle(uint8_t jointIndex, float angleDeg);
    
    // Set all 4 joint target angles at once
    void setAllTargets(float j1, float j2, float j3, float j4);
    
    // Set all 4 joint target angles immediately
    void setAllDirect(float j1, float j2, float j3, float j4);

    // Call regularly in main loop for smooth trajectory interpolation
    void update();

    // Getters & Setters for Joint Limits & Offsets
    JointState getJointState(uint8_t jointIndex) const;
    bool setJointLimits(uint8_t jointIndex, float minDeg, float maxDeg, float offsetDeg = 0.0f);

    float getJointCurrentAngle(uint8_t jointIndex) const;
    float getJointTargetAngle(uint8_t jointIndex) const;
    bool isMoving() const;

    // Motion parameters
    void setSpeed(float speedDegPerSec);
    float getSpeed() const { return speedDPS; }

    // Safety and Power
    void emergencyStop();
    void resetEmergencyStop();
    void enableOutputs(bool enable);
    bool isEStopped() const { return isEmergencyStopped; }
};

#endif // SERVO_CONTROLLER_H
