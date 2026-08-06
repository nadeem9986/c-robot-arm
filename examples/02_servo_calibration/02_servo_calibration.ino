/*
  Servo Calibration Utility
  Use Serial Monitor at 115200 baud to send target tick counts (100 to 600)
  to calibrate 0 degree and 180 degree pulse limits for your servos.
*/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

uint8_t channel = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);
  pwm.begin();
  pwm.setPWMFreq(50);
  
  Serial.println("--- PCA9685 Servo Calibration Utility ---");
  Serial.println("Send command format: C <channel> T <ticks>");
  Serial.println("Example: C 0 T 150  (Sets Channel 0 to 150 ticks)");
  Serial.println("Example: C 1 T 500  (Sets Channel 1 to 500 ticks)");
}

void loop() {
  if (Serial.available()) {
    String str = Serial.readStringUntil('\n');
    str.trim();
    int ch, ticks;
    if (sscanf(str.c_str(), "C %d T %d", &ch, &ticks) == 2) {
      if (ch >= 0 && ch < 16 && ticks >= 80 && ticks <= 650) {
        pwm.setPWM(ch, 0, ticks);
        Serial.printf("Set Channel %d to Pulse Ticks: %d\n", ch, ticks);
      } else {
        Serial.println("Error: Channel must be 0-15, Ticks must be 80-650.");
      }
    }
  }
}
