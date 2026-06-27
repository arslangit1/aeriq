#include "indicators.h"
#include "../app_config.h"

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#ifndef LED_BUILTIN
  #define LED_BUILTIN 13
#endif

static Adafruit_NeoPixel pixel(1, NEOPIXEL_PIN, NEO_GRBW + NEO_KHZ800);

static uint32_t cycleStartMs = 0;
static bool pulseOn = false;

static bool isAbnormal(const Readings& r) {
  if (!isnan(r.tC) && (r.tC < 0.0f || r.tC > 40.0f)) return true;
  if (!isnan(r.rh) && (r.rh < 20.0f || r.rh > 80.0f)) return true;
  if (r.co2_ppm > 1000) return true;
  if (!isnan(r.pm2_5) && r.pm2_5 > 35.0f) return true;
  if (!isnan(r.pm10_0) && r.pm10_0 > 150.0f) return true;
  if (!isnan(r.voc_index) && r.voc_index > 250.0f) return true;
  if (!isnan(r.nox_index) && r.nox_index > 20.0f) return true;
  return false;
}

static bool devicesOk(const Readings& r, bool wifiOk, bool sdOk) {
  return wifiOk && sdOk && r.veml_ok && r.sen55_ok && r.s88_ok;
}

static void setPixel(uint32_t color) {
  pixel.setPixelColor(0, color);
  pixel.show();
}

namespace Indicators {

void begin() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  #ifdef PIN_NEOPIXEL
    pinMode(PIN_NEOPIXEL, OUTPUT);
    digitalWrite(PIN_NEOPIXEL, LOW);
  #endif

  #ifdef NEOPIXEL_POWER
    pinMode(NEOPIXEL_POWER, OUTPUT);
    digitalWrite(NEOPIXEL_POWER, LOW);
  #endif

  pixel.begin();
  pixel.setBrightness(255);
  pixel.clear();
  pixel.show();

  cycleStartMs = millis();
}

void tick(const Readings& state, bool wifiOk, bool sdOk) {
  digitalWrite(LED_BUILTIN, LOW);

  const uint32_t now = millis();
  const uint32_t elapsed = now - cycleStartMs;

  if (elapsed >= NP_CYCLE_PERIOD_MS) {
    cycleStartMs = now;
    pulseOn = true;

    if (!devicesOk(state, wifiOk, sdOk)) {
      setPixel(pixel.Color(50, 0, 0, 0));
    } else if (isAbnormal(state)) {
      setPixel(pixel.Color(0, 0, 50, 0));
    } else {
      setPixel(pixel.Color(0, 50, 0, 0));
    }
  }

  if (pulseOn && (now - cycleStartMs >= NP_BLINK_ON_MS)) {
    pulseOn = false;
    setPixel(0);
  }
}

}
