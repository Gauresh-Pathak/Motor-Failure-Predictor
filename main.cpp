#include <Wire.h>
#include <MPU6050.h>
#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define DHTPIN 4
#define DHTTYPE DHT22

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
const char* telegramUsername = "@YOUR_TELEGRAM_USERNAME";
const char* apiKey = "YOUR_CALLMEBOT_API_KEY";

MPU6050 mpu;
DHT dht(DHTPIN, DHTTYPE);

int calibrationReadings = 10;
int readingCount = 0;
float totalVibration = 0;
float avgVibration = 0;
bool calibrated = false;

void sendAlert(String message) {
  // sending telegram alert
  HTTPClient http;
  String url = "https://api.callmebot.com/text.php?user=" + String(telegramUsername) + "&apikey=" + String(apiKey) + "&text=" + message;
  http.begin(url);
  http.GET();
  http.end();
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu.initialize();
  dht.begin();

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected!");
  Serial.println("Calibrating normal behavior...");
}

void loop() {
  delay(2000);

  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  float vibration = sqrt((float)(ax*ax) + (float)(ay*ay) + (float)(az*az)) / 16384.0;
  float temp = dht.readTemperature();

  // calibration phase
  if (!calibrated) {
    totalVibration += vibration;
    readingCount++;
    Serial.println("Calibrating... " + String(readingCount) + "/" + String(calibrationReadings));
    if (readingCount >= calibrationReadings) {
      avgVibration = totalVibration / calibrationReadings;
      calibrated = true;
      Serial.println("Calibration done! Normal: " + String(avgVibration) + "g");
    }
    return;
  }

  String status = "NORMAL";
  if (temp > 60.0) status = "ALERT: OVERHEATING";
  if (vibration > avgVibration * 1.5) status = "ALERT: ABNORMAL VIBRATION";

  if (status != "NORMAL") {
    sendAlert("MOTOR ALERT: " + status + " | Temp: " + String(temp) + "C | Vibration: " + String(vibration) + "g");
  }

  Serial.println("Temp: " + String(temp) + "C | Vibration: " + String(vibration) + "g | " + status);
}
