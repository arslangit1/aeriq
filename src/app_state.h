#pragma once
#include <Arduino.h>

// Shared application state (single source of truth)
struct Readings {
  uint32_t ms = 0;

  float tC = NAN;
  float rh = NAN;
  float lux = NAN;
  float white = NAN;
  uint16_t als = 0;

  uint16_t co2_ppm = 0;   // CO2 (Senseair S88)

  // SEN55
  float pm1_0 = NAN;
  float pm2_5 = NAN;
  float pm4_0 = NAN;
  float pm10_0 = NAN;
  float sen55_tC = NAN;
  float sen55_rh = NAN;
  float voc_index = NAN;
  float nox_index = NAN;

  bool veml_ok = false;  // VEML7700 initialized successfully
  bool s88_ok = false;   // last Senseair S88 read succeeded
  bool sen55_ok = false; // SEN55 initialized successfully
  bool rtc_ok = false;   // RTC present and time decoded

  char timestamp[24] = "-"; // YYYY-MM-DD HH:MM:SS or "-"
};

extern Readings g;
