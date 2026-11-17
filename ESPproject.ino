#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

// 1. Your Wi-Fi Credentials (Must be 2.4 GHz)
const char* ssid = "Exactly What You're Looking For";
const char* password = "amitkumar";

// 2. Your Google Sheets Web App URL
const char* scriptURL = "https://script.google.com/macros/s/AKfycbx0wC6g9CLTV_l5Vdg9NL5eGk0qobzygSmua690R2T3nji-JOkwu9EiljFkHgwJ6z4AkA/exec";

// 3. DHT11 Sensor on GPIO 4
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
      Serial.println("Check DHT11 wiring on pin 4!");
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
      Serial.println(http.getString()); // Should print "Data added successfully"
    } else {
      Serial.println("Error: " + String(code));
    }
    http.end();
  }
  delay(5000);
}