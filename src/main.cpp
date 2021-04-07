#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <MQUnifiedsensor.h>
#include "secrets.h"

#define DHT_PIN D2
#define MQ_PIN A0

DHT dht = DHT(DHT_PIN, DHT22);
MQUnifiedsensor MQ135("Arduino NANO", 5, 10, MQ_PIN, "MQ-135");

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
    MQ135.setRegressionMethod(1);
    MQ135.init();
    MQ135.setA(110.47);
    MQ135.setB(-2.862); // Configurate the ecuation values to get CO2 concentration

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
    MQ135.update();

    measurement_t measurement{};
    measurement.humidity = dht.readHumidity();
    measurement.temperature = dht.readTemperature();
    measurement.carbon_dioxide = MQ135.readSensor();

    return measurement;
}

int submit_measurement(const measurement_t measurement) {
    http.begin(wifiClient, String(APP_BASE_URL) + "/api/measurements");
    http.addHeader("Content-Type", "application/json");

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