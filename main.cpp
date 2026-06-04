#include <DHT.h>
#include <Wire.h>
#include <MPU6050.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <time.h>

#define DHTPIN 4
#define DHTTYPE DHT22

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
const char* telegramUsername = "@YOUR_TELEGRAM_USERNAME";
const char* apiKey = "YOUR_CALLMEBOT_API_KEY";

float maxTemp = 60.0;

int calibrationReadings = 10;
int readingCount = 0;
float totalVibration = 0;
float avgVibration = 0;
bool calibrated = false;

DHT dht(DHTPIN, DHTTYPE);
MPU6050 mpu;
WebServer server(80);
String dataLog = "";

void reconnectWifi() {
  // retry wifi if disconnected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi lost! Reconnecting...");
    WiFi.begin(ssid, password);
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      delay(500);
      Serial.print(".");
      attempts++;
    }
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Reconnected!");
    } else {
      Serial.println("Reconnection failed.");
    }
  }
}

void sendAlert(String message) {
  // sending telegram alert
  HTTPClient http;
  String url = "https://api.callmebot.com/text.php?user=" + String(telegramUsername) + "&apikey=" + String(apiKey) + "&text=" + message;
  http.begin(url);
  http.GET();
  http.end();
}

String getTime() {
  time_t now = time(nullptr);
  struct tm* t = localtime(&now);
  char buf[30];
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", t);
  return String(buf);
}

String getStatus(float temp, float vibration) {
  if (!calibrated) return "CALIBRATING";
  if (temp > maxTemp) return "ALERT: OVERHEATING";
  if (vibration > avgVibration * 1.5) return "ALERT: ABNORMAL VIBRATION";
  return "NORMAL";
}

void handleRoot() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  float vibration = sqrt((float)(ax*ax) + (float)(ay*ay) + (float)(az*az)) / 16384.0;

  String status = getStatus(temp, vibration);
  String statusColor = (status == "NORMAL") ? "safe" : "alert";

  String html = "<html><head><title>Motor Failure Predictor</title>";
  html += "<meta http-equiv='refresh' content='5'>";
  html += "<style>body{font-family:Arial;background:#0a0a0a;color:white;padding:20px}";
  html += "h1{color:#21ff7b}.card{background:#1a1a1a;border-radius:10px;padding:20px;margin:10px 0}";
  html += ".alert{color:#ff4444;font-weight:bold}.safe{color:#21ff7b}.calibrating{color:#ffd700}";
  html += "table{width:100%;border-collapse:collapse}";
  html += "th,td{padding:10px;border:1px solid #333;text-align:left}";
  html += "th{background:#21ff7b22;color:#21ff7b}</style></head><body>";
  html += "<h1>Motor Failure Predictor</h1>";
  html += "<div class='card'><h2>Live Reading</h2>";
  html += "<p>Temperature: <b>" + String(temp) + " C</b></p>";
  html += "<p>Vibration: <b>" + String(vibration) + " g</b></p>";
  html += "<p>Avg Normal Vibration: <b>" + String(avgVibration) + " g</b></p>";
  html += "<p>Time: " + getTime() + "</p>";
  html += "<p class='" + statusColor + "'>" + status + "</p></div>";
  html += "<div class='card'><h2>Log</h2><table>";
  html += "<tr><th>Time</th><th>Temp</th><th>Vibration</th><th>Status</th></tr>";
  html += dataLog + "</table></div></body></html>";

  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  dht.begin();
  mpu.initialize();

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected! Open: http://" + WiFi.localIP().toString());
  configTime(19800, 0, "pool.ntp.org");
  delay(2000);

  server.on("/", handleRoot);
  server.begin();

  Serial.println("Calibrating normal behavior...");
}

void loop() {
  reconnectWifi();
  server.handleClient();

  static unsigned long lastRead = 0;
  if (millis() - lastRead >= 3000) {
    lastRead = millis();

    float temp = dht.readTemperature();
    float hum = dht.readHumidity();

    int16_t ax, ay, az, gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    float vibration = sqrt((float)(ax*ax) + (float)(ay*ay) + (float)(az*az)) / 16384.0;

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

    String status = getStatus(temp, vibration);

    if (status != "NORMAL") {
      sendAlert("MOTOR ALERT: " + status + " | Temp: " + String(temp) + "C | Vibration: " + String(vibration) + "g at " + getTime());
    }

    dataLog += "<tr><td>" + getTime() + "</td><td>" + String(temp) + "C</td><td>" + String(vibration) + "g</td><td>" + status + "</td></tr>";
    Serial.println(getTime() + " | Temp: " + String(temp) + " | Vibration: " + String(vibration) + " | " + status);
  }
}
