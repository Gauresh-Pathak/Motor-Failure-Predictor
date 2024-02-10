#include <Wire.h>
#include <MPU6050.h>
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT22

MPU6050 mpu;
DHT dht(DHTPIN, DHTTYPE);

int calibrationReadings = 10;
int readingCount = 0;
float totalVibration = 0;
float avgVibration = 0;
bool calibrated = false;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu.initialize();
  dht.begin();

  Serial.println("Calibrating normal behavior...");
}

void loop() {
  delay(2000);

  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  float vibration = sqrt((float)(ax*ax) + (float)(ay*ay) + (float)(az*az)) / 16384.0;
  float temp = dht.readTemperature();

  // calibration phase - learning normal vibration
  if (!calibrated) {
    totalVibration += vibration;
    readingCount++;
    Serial.println("Calibrating... reading " + String(readingCount) + "/" + String(calibrationReadings));
    if (readingCount >= calibrationReadings) {
      avgVibration = totalVibration / calibrationReadings;
      calibrated = true;
      Serial.println("Calibration done! Normal vibration: " + String(avgVibration) + "g");
    }
    return;
  }

  // check if abnormal
  String status = "NORMAL";
  if (temp > 60.0) status = "ALERT: OVERHEATING";
  if (vibration > avgVibration * 1.5) status = "ALERT: ABNORMAL VIBRATION";

  Serial.println("Temp: " + String(temp) + "C | Vibration: " + String(vibration) + "g | " + status);
}
