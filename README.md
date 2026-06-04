# Motor Failure Predictor

## What does it do?
Monitors a motor's temperature and vibration in real time using ESP32, DHT22, and MPU6050 sensors.
Learns what normal behavior looks like by calibrating itself on startup, then flags any abnormal readings as potential motor failure before it actually happens.

## Why I built this
Motors failing unexpectedly cause huge losses in factories and industrial setups.
I built this as a low cost predictive maintenance solution that gives early warning before damage occurs.

## Hardware used
- ESP32
- DHT22 Temperature Sensor
- MPU6050 Accelerometer/Gyroscope (for vibration detection)

## Features
- Self calibrates on startup to learn normal vibration
- Detects abnormal vibration and overheating
- Sends instant Telegram alert on anomaly detection
- Live web dashboard with full data log and timestamps

## How to run
1. Open main.cpp in Arduino IDE
2. Enter your WiFi name and password
3. Enter your Telegram username and CallMeBot API key
4. Upload to ESP32
5. Open Serial Monitor to get IP address
6. Open that IP in your browser

## Future Improvements
- Add RPM sensor for more accurate failure prediction
- Store data on SD card for offline logging
- Mobile app dashboard
- Support for multiple motors simultaneously
- Email alerts in addition to Telegram

## Built by
Gauresh Pathak — Electronics and Telecommunication Engineering Student, Mumbai

