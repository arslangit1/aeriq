#pragma once
#include <Arduino.h>
#include <Wire.h>

struct RtcDateTime {
  uint16_t year = 0;
  uint8_t month = 0;
  uint8_t day = 0;
  uint8_t hour = 0;
  uint8_t minute = 0;
  uint8_t second = 0;
};

namespace RtcClock {
  bool begin(TwoWire& wire, uint8_t address);
  bool read(RtcDateTime& dt);
  bool isPresent();
  bool isReady();
  String format(const RtcDateTime& dt);
}
