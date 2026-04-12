#pragma once
#include <Arduino.h>
#include "../app_state.h"

// struct IaqSample {
//   // Populate these fields from your existing sensor readouts
//   float temp_c = NAN;
//   float rh_pct = NAN;

//   float pm1_0 = NAN;
//   float pm2_5 = NAN;
//   float pm4_0 = NAN;
//   float pm10  = NAN;

//   float voc_index = NAN;
//   float nox_index = NAN;

//   uint32_t co2_ppm = 0;        // if you have it, else leave 0
//   uint32_t ts_ms = 0;          // timestamp in ms (or epoch seconds if you prefer)
// };

class SdLogger {
public:
  // If you already have an SPI bus used by TFT, pass the same pins here and a unique SD CS pin.
  bool begin(uint8_t csPin, int sckPin = 18, int misoPin = 19, int mosiPin = 23,
             uint32_t spiFreqHz = 20000000);

  // Appends one CSV row. Creates file and header automatically if needed.
  bool appendSample(const Readings& r);

  // Optional: call to force sync (good before deep sleep / power cut).
  void flush();

  bool isReady() const { return _ready; }
  const String& filePath() const { return _filePath; }

private:
  bool ensureFile();
  void writeHeaderIfNeeded();

private:
  bool _ready = false;
  bool _headerWritten = false;

  uint8_t _csPin = 5;
  int _sckPin = 18, _misoPin = 19, _mosiPin = 23;
  uint32_t _spiFreqHz = 20000000;

  String _filePath = "/iaq_log.csv";
};
