#include "Adafruit_PM25AQI.h"
#include <FastLED.h>

struct AirQualitySensor : Service::AirQualitySensor {
  CRGB *statusLED;
  //Adafruit_PM25AQI *sensor;
  Adafruit_PM25AQI sensor;
  SpanCharacteristic *airQuality;
  SpanCharacteristic *pm10Density;
  SpanCharacteristic *pm25Density;
  int sensorFailureCount = 0;

  AirQualitySensor(CRGB &led) : Service::AirQualitySensor() {
    statusLED = &led;
    sensor = Adafruit_PM25AQI();
    airQuality = new Characteristic::AirQuality(0);
    pm10Density = new Characteristic::PM10Density(0.0);
    pm25Density = new Characteristic::PM25Density(0.0);

    Serial.print("Configuring Air Quality Sensor");
    Serial.print("\n");

    if (!sensor.begin_I2C()) {
      Serial.println("Could not find PM 2.5 sensor!");
      while(1) delay(10);
    } else {
      Serial.println("Found PM 2.5 Sensor");
    }
  }

  void loop() {
    if (airQuality->timeVal() < 5000) {
      return;
    }

    PM25_AQI_Data data;

    if (!sensor.read(&data)) {
      statusLED->setRGB(255, 255, 255);
      Serial.println("Failed to read sensor");
      sensorFailureCount += 1;
      if (sensorFailureCount > 10) {
        while(1) delay(10);
      }
      return;
    }

    Serial.print(F("PM 1.0: ")); Serial.print(data.pm10_standard);
    Serial.print(F("\t\tPM 2.5: ")); Serial.print(data.pm25_standard);
    Serial.print(F("\t\tPM 10: ")); Serial.println(data.pm100_standard);

    double pm100 = double(data.particles_100um);
    double pm25 = double(data.particles_25um);

    int homeKitQuality = 0;

    if (pm25 <= 12) {
      homeKitQuality = 1; // Excellent
      statusLED->setRGB(0, 255, 0); // Green
      Serial.println("Excellent");
    } else if (pm25 <= 35) {
      homeKitQuality = 2; // Good
      statusLED->setRGB(25, 225, 50); // YellowGreen
      Serial.println("Good");
    } else if (pm25 <= 55) {
      homeKitQuality = 3; // Fair
      statusLED->setRGB(255, 255, 0); // Yellow
      Serial.println("Fair");
    } else if (pm25 <= 150) {
      homeKitQuality = 4; // Inferior
      statusLED->setRGB(255, 0, 0); // Red
      Serial.println("Inferior");
    } else {
      homeKitQuality = 5; // Poor
      statusLED->setRGB(128, 0, 128); // Purple
      Serial.println("Poor");
    }

    airQuality->setVal(homeKitQuality);
    pm10Density->setVal(pm100);
    pm25Density->setVal(pm25);

  }

};
