#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "secrets.h"

#define DHT_PIN D2

DHT dht = DHT(DHT_PIN, DHT22);
HTTPClient http;
WiFiClientSecure wifiClient;

struct measurement_t {
    float humidity;
    float temperature;
    float carbon_dioxide;

public:
    String json() const {
        return "{\"humidity\":" + String(this->humidity) + ","
               + "\"carbon_dioxide\":" + String(this->carbon_dioxide) + ","
               + "\"temperature\":" + String(this->temperature) + "}";
    }
};

void setup() {
    Serial.begin(9600);

    dht.begin();
    WiFi.begin(WIFI_SSID, WIFI_PASSWD);
    wifiClient.setInsecure();

    int tries = 0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.printf("Connecting... %d\t", ++tries);
    }

    Serial.printf("Connected to %s with IP Address: %s", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
}

const int MEASURE_INTERVAL = (1000 * 60 * 5);

measurement_t get_latest_measurement() {
    measurement_t measurement{};
    measurement.humidity = dht.readHumidity();
    measurement.temperature = dht.readTemperature();

    return measurement;
}

int submit_measurement(const measurement_t measurement) {
    http.begin(wifiClient, String(APP_BASE_URL) + "/api/measurements");
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + String(APP_DEVICE_TOKEN));

    int responseCode = http.POST(measurement.json());
    http.end();

    return responseCode;
}

void loop() {
    if (millis() % MEASURE_INTERVAL != 0) {
        return;
    }

    const measurement_t measurement = get_latest_measurement();
    const int response = submit_measurement(measurement);

    if (response >= 400) {
        Serial.print(response);
        Serial.println(' ' + measurement.json());
    }

    delay(1);
}