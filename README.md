# 🌡️ ThermoSync IoT
**Real-Time Low-Cost Wireless Environmental Data Logger Using ESP32 & Google Sheets**

ThermoSync IoT is a lightweight, reliable edge-to-cloud environmental monitoring system built with an **ESP32** microcontroller and a **DHT11** temperature and humidity sensor. It streams live ambient telemetry over Wi-Fi directly into **Google Sheets** via Google Apps Script, transforming raw sensor logs into an auto-updating visual dashboard with live KPIs, anomaly detection, and trend forecasting—without requiring paid cloud platforms or complex backend servers.

---

## 👥 Team Members

| Name | Enrollment / ID |
| :--- | :--- |
| **Suryansh Srivastava** | `202410101150085` |
| **Ruchi Singh** | `202410101150061` |
| **Jayesh Srivastava** | `202410101150117` |
| **Aditya Dwivedi** | `202410101150087` |

---

## ✨ Key Features
* **Zero-Cost Cloud Logging:** Streams temperature (°C) and relative humidity (%) directly to Google Sheets every few seconds using HTTPS `GET` requests.
* **Crash-Free Firmware:** Optimized Wi-Fi-only ESP32 implementation avoiding heap fragmentation and dual-radio collisions.
* **Live Google Sheets Dashboard:**
  * **Real-Time KPI Summary Cards:** Tracks latest readings, last updated timestamp, session peak temperature, and rolling median baseline.
  * **Automated Anomaly Detection:** Color-coded status badge (`✅ NORMAL AMBIENT` vs. `🔥 HEAT SPIKE DETECTED`) using dynamic baseline thresholds.
  * **5-Minute Trend Forecast:** Built-in linear regression (`FORECAST`) predicting near-term temperature shifts.
  * **Auto-Scrolling Live Window:** Dynamic `QUERY` table feeding a live dual-series line chart of the latest 30 samples.

---

## 🛠️ Hardware Components & Wiring

### Components Required
* **ESP32 Dev Module:** Dual-core 2.4 GHz Wi-Fi microcontroller acting as the edge telemetry node.
* **DHT11 Sensor Module:** Digital capacitive humidity and NTC thermistor sensor (`0–50 °C`, `20–90% RH`).
* **Breadboard & Jumper Wires**

### Circuit Connection Table

| DHT11 Pin | ESP32 Pin | Function |
| :--- | :--- | :--- |
| **VCC** | **3V3** | Power (`3.3V`) |
| **DATA** | **D4 (GPIO 4)** | Digital Signal |
| **GND** | **GND** | Common Ground |

---

## 💻 Setup & Installation

### 1. Google Sheets & Apps Script Setup
1. Create a new **Google Sheet** and name the first tab `Sheet1` with headers in Row 1: `TIME STAMP`, `Temperature`, `Humidity`.
2. Open **Extensions** → **Apps Script**, paste the following code, and replace `YOUR_SHEET_ID` with your spreadsheet's ID:

```javascript
function doGet(e) {
  var sheet = SpreadsheetApp.openById("YOUR_SHEET_ID").getActiveSheet();

  if (sheet.getLastRow() === 0) {
    sheet.appendRow(["TIME STAMP", "Temperature", "Humidity"]);
  }

  var temp = e.parameter.temp;
  var hum = e.parameter.hum;

  sheet.appendRow([new Date(), temp, hum]);

  return ContentService.createTextOutput("Data added successfully");
}
```

3. Click **Deploy** → **New deployment** → select **Web app**:
   * **Execute as:** `Me`
   * **Who has access:** `Anyone`
4. Authorize permissions and copy the generated **`/exec` Web App URL**.

---

### 2. ESP32 Firmware (Arduino IDE)
1. Install the **ESP32 Board Package** and **DHT sensor library** (by Adafruit) in Arduino IDE.
2. Update your Wi-Fi credentials and Web App URL in the sketch below, then upload to your ESP32 (`115200` baud):

```cpp
#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

// 1. Wi-Fi Credentials (2.4 GHz)
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// 2. Google Apps Script Web App URL
const char* scriptURL = "YOUR_GOOGLE_APPS_SCRIPT_EXEC_URL";

// 3. DHT11 Sensor Configuration
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected! IP: " + WiFi.localIP().toString());
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT11 sensor!");
      delay(2000);
      return;
    }

    HTTPClient http;
    String url = String(scriptURL) + "?temp=" + String(t, 1) + "&hum=" + String(h, 0);
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    http.begin(url);

    int code = http.GET();
    Serial.print("Temp: " + String(t, 1) + "C | Hum: " + String(h, 0) + "% -> ");
    if (code > 0) {
      Serial.println(http.getString());
    } else {
      Serial.println("Error: " + String(code));
    }
    http.end();
  }
  delay(5000);
}
```

---

## 📊 Live Dashboard Formulas (Google Sheets)

Add these formulas alongside your incoming columns (`A:C`) to power the live analytics panel:

* **Latest Temperature:** `=INDEX(B:B, COUNTA(B:B))`
* **Latest Humidity:** `=INDEX(C:C, COUNTA(C:C))`
* **Last Updated Time:** `=INDEX(A:A, COUNTA(A:A))`
* **Session Peak Temp:** `=MAX(B2:B)`
* **Session Baseline (Median):** `=MEDIAN(B2:B)`
* **Live Status Badge:**
  ```excel
  =IF(INDEX(B:B, COUNTA(B:B)) - MEDIAN(B2:B) > 1.2, "🔥 HEAT SPIKE DETECTED", IF(INDEX(C:C, COUNTA(C:C)) - MEDIAN(C2:C) > 4, "💧 HUMIDITY SPIKE DETECTED", "✅ NORMAL AMBIENT"))
  ```
* **Auto-Scrolling Live Window (Latest 30 Readings):**
  ```excel
  =QUERY(A2:C, "SELECT A, B, C WHERE A IS NOT NULL ORDER BY A DESC LIMIT 30", 0)
  ```

---

## 📄 License
This project is open-source and available for educational and academic use.
