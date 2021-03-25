#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "secrets.h"

#define DHT_PIN D2

DHT dht = DHT(DHT_PIN, DHT22);

void setup() {
    Serial.begin(9600);

    dht.begin();
    WiFi.begin(WIFI_SSID, WIFI_PASSWD);

    int tries = 0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.printf("Connecting... %d\t", ++tries);
    }

    Serial.printf("Connected to %s with IP Address: %s", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
}

#define MEASURE_INTERVAL (1000 * 60 * 5);
//const int MEASURE_INTERVAL = 5000;

void loop() {
    if (millis() % MEASURE_INTERVAL != 0) {
        return;
    }

    // Read the humidity and temperature in %:
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    float heat_index = dht.computeHeatIndex(temperature, humidity, false);

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println("%");
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println("°C");
    Serial.print("Feels like: ");
    Serial.print(heat_index);
    Serial.println("°C");
    Serial.println("Read finished...");

    delay(1);
}