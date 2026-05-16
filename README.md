
# ParalytiCare 🏥
 
**IoT-Based Automated Paralysis Patient Healthcare Monitoring System**
 
> A low-cost, open-source IoT platform that monitors vitals and enables gesture-based communication for paralyzed patients — bridging the gap between patients and caregivers in real time.
 
---
 
## 📋 Table of Contents
 
- [Overview](#overview)
- [Features](#features)
- [System Architecture](#system-architecture)
- [Hardware Components](#hardware-components)
- [Software & Libraries](#software--libraries)
- [Circuit Design](#circuit-design)
- [How It Works](#how-it-works)
- [Results](#results)
- [Applications](#applications)
- [Future Scope](#future-scope)
- [Team](#team)
---
 
## Overview
 
Paralysis resulting from stroke, spinal cord injury, or neurodegenerative disorders severely limits a patient's ability to move, communicate needs, or self-monitor health. Conventional medical-grade monitoring systems are expensive, non-portable, and poorly suited for home-care or rural environments.
 
**ParalytiCare** addresses this gap by combining:
- Real-time **vital sign monitoring** (heart rate & temperature)
- **Gesture-based communication** via a wearable flex-sensor glove
- **Wireless IoT connectivity** for remote caregiver access
- **Automated Telegram alerts** for instant caregiver notifications
Built on open-source hardware (Arduino UNO + NodeMCU ESP8266), the system is affordable, portable, and easily replicable in resource-limited settings.
 
---
 
## Features
 
- 💓 **Heart Rate Monitoring** — Continuous BPM tracking via MAX30100 heartbeat sensor
- 🌡️ **Temperature Monitoring** — Real-time body temperature readings with DHT11
- 🤙 **Gesture-Based Communication** — Three flex sensors on a wearable glove map finger movements to predefined requests (Water / Medicine / Help)
- 📟 **LCD Display** — Real-time local display of vitals and gesture outputs on a 16×2 LCD
- 🌐 **Local Web Server** — Live patient data accessible from any device on the network via NodeMCU (ESP8266)
- 📲 **Telegram Alerts** — Automated instant notifications to caregivers on gesture detection or vital anomalies
- ⚡ **Low Cost & Portable** — USB-powered; deployable in home-care, rural hospitals, or disaster relief settings
---
 
## System Architecture
 
```
┌─────────────────────────────────────────────────────────┐
│                    SENSING LAYER                        │
│  Heartbeat Sensor │ DHT11 Sensor │ Flex Sensors (×3)   │
└───────────────────────────┬─────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────┐
│               PROCESSING LAYER (Arduino UNO)            │
│  Peak detection │ Gesture classification │ LCD output   │
└───────────────────────────┬─────────────────────────────┘
                            │ UART
                            ▼
┌─────────────────────────────────────────────────────────┐
│           COMMUNICATION LAYER (NodeMCU ESP8266)         │
│      Local Web Server  │  Telegram Bot API Alerts       │
└───────────────────────────┬─────────────────────────────┘
                            │ Wi-Fi
                            ▼
┌─────────────────────────────────────────────────────────┐
│                  CAREGIVER / REMOTE ACCESS              │
│       Smartphone Browser  │  Telegram App               │
└─────────────────────────────────────────────────────────┘
```
 
**Data Flow:**
1. Sensors acquire physiological data and gesture inputs
2. Arduino UNO processes raw data (BPM calculation, gesture classification)
3. Results are shown on the 16×2 LCD in real time
4. Arduino transmits structured data to NodeMCU via UART
5. NodeMCU hosts a local web page and sends Telegram alerts as needed
---
 
## Hardware Components
 
| Component | Model | Voltage | Interface | Function |
|-----------|-------|---------|-----------|----------|
| Heartbeat Sensor | MAX30100 | 5V | Analog | Pulse / Heart Rate Detection |
| Temperature Sensor | DHT11 | 3.3V – 5V | Digital (1-Wire) | Body Temperature Measurement |
| Flex Sensor (×3) | 2.2" Flex | 3.3V – 5V | Analog | Gesture / Bend Detection |
| LCD Display | 16×2 LCD | 5V | 4-bit Parallel | Local Data Display |
| Microcontroller | Arduino UNO R3 | 5V | USB / I2C / UART | Main Processing Unit |
| Wi-Fi Module | NodeMCU ESP8266 | 3.3V | UART / Wi-Fi | Web Server & Telegram Alerts |
 
**Pin Mapping (Arduino UNO):**
- `A0` — Heartbeat Sensor (KY-039)
- `A1`, `A2`, `A3` — Flex Sensors (index, middle, ring finger) via 10kΩ pull-down resistors
- `D2` — DHT11 Temperature Sensor
- `D10/D11` — NodeMCU Software Serial (TX/RX)
- `D4–D9` — 16×2 LCD (4-bit mode)
> ⚠️ **Note:** NodeMCU operates at 3.3V logic. Use a voltage divider on the Arduino TX line to protect the NodeMCU RX pin from 5V signals.
 
---
 
## Software & Libraries
 
**Development Environment:** Arduino IDE
 
| Library | Purpose |
|---------|---------|
| `DHT.h` | Reading temperature from DHT11 sensor |
| `LiquidCrystal.h` | Controlling 16×2 LCD in 4-bit mode |
| `SoftwareSerial.h` | UART communication between Arduino UNO and NodeMCU |
| `ESP8266WiFi.h` | Wi-Fi connectivity on NodeMCU |
| `ESP8266WebServer.h` | Hosting local web server on NodeMCU |
 
**Key Code Logic:**
1. Initialize all sensors, LCD, and UART communication
2. Continuously read heartbeat, temperature, and flex sensor values
3. Classify gestures based on empirically calibrated threshold values
4. Display vitals and gesture outputs on LCD
5. Transmit structured data to NodeMCU for web updates and Telegram alerts
---
 
## Circuit Design
 
The Arduino UNO acts as the central controller:
 
- **MAX30100** connected via I2C (SDA/SCL pins) for heart rate monitoring
- **DHT11** connected to a digital pin for single-wire temperature communication
- **Flex Sensors** connected to analog pins via pull-down resistors; resistance changes with finger bend angle
- **ESP8266** connected via Software Serial (UART) for IoT communication
- All component grounds tied to Arduino GND for a common reference
---
 
## How It Works
 
1. **Gesture Input** — The patient wears a glove embedded with three flex sensors on the index, middle, and ring fingers. Bending a finger changes the sensor resistance, producing an analog signal.
2. **Gesture Classification** — Arduino reads analog values and compares them against calibrated thresholds to identify predefined requests:
   - Finger combination 1 → **"Water"**
   - Finger combination 2 → **"Medicine"**
   - Finger combination 3 → **"Help / Emergency"**
3. **Vital Monitoring** — BPM is calculated using peak-detection logic on the heartbeat sensor signal. Temperature is read from DHT11 via 1-wire protocol.
4. **Local Display** — Vitals and gesture outputs are shown in real time on the 16×2 LCD.
5. **Remote Access** — NodeMCU hosts a web page updated every ~2 seconds with current patient data, accessible from any smartphone or browser on the same network.
6. **Caregiver Alerts** — When an emergency gesture or anomalous vital is detected, the NodeMCU sends an instant Telegram message to the caregiver's smartphone via the Telegram Bot API.
---
 
## Results
 
| Parameter | Result |
|-----------|--------|
| Heart Rate Range | 68–96 BPM (avg. 78 BPM at rest) |
| Temperature Range | 35.2°C – 36.8°C (palm-proximity sensing) |
| Gesture Recognition Accuracy | 94% across test sessions |
| Web Server Update Interval | ~2 seconds |
| Telegram Alert Delivery | 1–3 seconds after gesture detection |
| Wi-Fi Range | Stable up to 10 m from router |
 
**Key Observations:**
- Heartbeat sensor is sensitive to ambient light; shielding the fingertip improves signal stability
- Flex sensor thresholds require per-user calibration due to differences in finger size and glove fit
- DHT11 exhibits a 1–2 second response lag after sudden temperature changes
- NodeMCU Wi-Fi was stable with no dropouts during testing sessions
---
 
## Applications
 
- 🏠 **Home-Care for Paralysis Patients** — Enables independent communication, reducing reliance on constant caregiver presence
- 👴 **Elderly & Assisted Living Monitoring** — Early warning for fever spikes or irregular pulse; family members can monitor remotely
- 🩺 **Telemedicine** — Physicians can review live vitals during remote consultations without a physical visit
- 🎓 **Medical Research & Education** — Open-source and modular; ideal for students and researchers in embedded systems and biomedical IoT
- 🚨 **Emergency & Critical Care** — Gesture-triggered distress alerts enable rapid response in low-resource or disaster settings
---
 
## Future Scope
 
- ☁️ **Cloud Integration** — ThingSpeak, AWS IoT, or Firebase for longitudinal health trend tracking and physician-accessible records
- 📱 **Dedicated Mobile App** — Custom dashboard with graphical trends and customizable alert thresholds
- 🎯 **Enhanced Sensor Accuracy** — Replace DHT11 with DS18B20; add MAX30100 for simultaneous SpO₂ + heart rate monitoring
- 🤖 **Machine Learning** — Lightweight anomaly detection models for predictive alerts before conditions become critical
- ⌚ **Wearable Form Factor** — Miniaturize into a wrist-worn or embedded garment device for daily comfort
- 🔊 **Voice Output Module** — Verbal announcement of gesture requests on the caregiver's side to reduce response time
---
 
## Team
 
**Mini Project | Dayananda Sagar College of Engineering, Bengaluru**
*Department of Electrical & Electronics Engineering, 2025–26*
 
| Name | USN |
|------|-----|
| Aarya Sharma | 1DS23EE001 |
| Atharv Verma | 1DS23EE015 |
| Harsh Sahu | 1DS23EE032 |
| Khushi Ghosh | 1DS23EE045 |
 
**Guide:** Dr. Sridevi H R, Assistant Professor, Dept. of E&E Engg., DSCE, Bengaluru
 
---
 
 
