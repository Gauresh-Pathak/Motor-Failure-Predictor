#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu.initialize();

  Serial.println("Motor Monitor Started");
}

void loop() {
  delay(2000);

  // getting vibration from mpu6050
  int16_t ax, ay, az;
  int16_t gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // converting raw vibration to readable value
  float vibration = sqrt((float)(ax*ax) + (float)(ay*ay) + (float)(az*az)) / 16384.0;

  Serial.print("Vibration: ");
  Serial.print(vibration);
  Serial.println("g");
}
