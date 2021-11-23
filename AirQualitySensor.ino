// HomeKit
#include "HomeSpan.h"
#include "AQSensor.h"
#include <FastLED.h>

// Thing Plus C Status Indicator
#define LED_PIN     2
#define COLOR_ORDER GRB
#define CHIPSET     WS2812
#define NUM_LEDS    1
#define BRIGHTNESS  128

// Watchdog
#include <esp_task_wdt.h>

CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");

  esp_task_wdt_init(3, true);
  esp_task_wdt_add(NULL);

  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );
  FastLED.clear();
  FastLED.show();

  homeSpan.enableOTA();
  homeSpan.begin(Category::Sensors, "Air Quality Sensor");
  new SpanAccessory();
  new Service::AccessoryInformation();
  new Characteristic::Name("Air Quality Sensor");
  new Characteristic::Manufacturer("Josh Billions");
  new Characteristic::SerialNumber("123-ABC");
  new Characteristic::Model("Air Quality Sensor");
  new Characteristic::FirmwareRevision("0.91");
  new Characteristic::Identify();

  new Service::HAPProtocolInformation();
  new Characteristic::Version("1.1.0");
  
  new AirQualitySensor(leds[0]); 
}

void loop() {
  esp_task_wdt_reset();
  homeSpan.poll();
  FastLED.show();
}
