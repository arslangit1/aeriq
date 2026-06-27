#pragma once
#include <Arduino.h>
#include "../app_state.h"

class SdLogger {
public:
  bool begin(uint8_t csPin, int sckPin = 18, int misoPin = 19, int mosiPin = 23,
             uint32_t spiFreqHz = 20000000);

  bool appendSample(const Readings& r, bool wifiOk);

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

  String _filePath = "/iaq_log_simple.csv";
};
