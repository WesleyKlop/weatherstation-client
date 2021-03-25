#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#define DHT_PIN D2


DHT dht = DHT(DHT_PIN, DHT22);

void setup() {
    Serial.begin(9600);

    dht.begin();
}

const int MEASURE_INTERVAL = 1000 * 60 * 5;
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