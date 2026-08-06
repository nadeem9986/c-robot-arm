/*
  ESP32 I2C Scanner for PCA9685 Detection
  Wire Pins: SDA -> GPIO 21, SCL -> GPIO 22
*/

#include <Wire.h>

#define SDA_PIN 21
#define SCL_PIN 22

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("\n--- ESP32 I2C Scanner for PCA9685 ---");
  Wire.begin(SDA_PIN, SCL_PIN);
}

void loop() {
  byte error, address;
  int nDevices = 0;

  Serial.println("Scanning I2C bus...");

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.printf("I2C device found at address 0x%02X", address);
      if (address == 0x40) {
        Serial.print("  <-- Default PCA9685 Servo Driver Address!");
      }
      Serial.println();
      nDevices++;
    } else if (error == 4) {
      Serial.printf("Unknown error at address 0x%02X\n", address);
    }
  }

  if (nDevices == 0) {
    Serial.println("No I2C devices found. Check SDA(GPIO21), SCL(GPIO22), VCC and GND connections.\n");
  } else {
    Serial.printf("Scan complete. Found %d device(s).\n\n", nDevices);
  }

  delay(5000);
}
