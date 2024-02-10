#include <Wire.h>
#include <MPU6050.h>
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT22

MPU6050 mpu;
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu.initialize();
  dht.begin();

  Serial.println("Motor Monitor Started");
}

void loop() {
  delay(2000);

  // getting vibration from mpu6050
  int16_t ax, ay, az;
  int16_t gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  float vibration = sqrt((float)(ax*ax) + (float)(ay*ay) + (float)(az*az)) / 16384.0;

  // getting temp from dht22
  float temp = dht.readTemperature();

  Serial.print("Vibration: ");
  Serial.print(vibration);
  Serial.print("g | Temp: ");
  Serial.print(temp);
  Serial.println("C");
}
