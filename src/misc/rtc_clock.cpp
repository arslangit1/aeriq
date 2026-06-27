#include "rtc_clock.h"

static TwoWire* rtcWire = nullptr;
static uint8_t rtcAddr = 0x68;
static bool present = false;
static bool ready = false;

static uint8_t bcdToDec(uint8_t value) {
  return ((value >> 4) * 10) + (value & 0x0F);
}

static uint8_t decToBcd(uint8_t value) {
  return ((value / 10) << 4) | (value % 10);
}

static bool readBytes(uint8_t startReg, uint8_t* data, size_t len) {
  rtcWire->beginTransmission(rtcAddr);
  rtcWire->write(startReg);
  if (rtcWire->endTransmission(false) != 0) return false;
  if (rtcWire->requestFrom(rtcAddr, len) != len) return false;
  for (size_t i = 0; i < len; i++) data[i] = rtcWire->read();
  return true;
}

static bool writeBytes(uint8_t startReg, const uint8_t* data, size_t len) {
  rtcWire->beginTransmission(rtcAddr);
  rtcWire->write(startReg);
  for (size_t i = 0; i < len; i++) rtcWire->write(data[i]);
  return rtcWire->endTransmission() == 0;
}

static bool valid(const RtcDateTime& dt) {
  return dt.year >= 2020 && dt.year <= 2099 &&
         dt.month >= 1 && dt.month <= 12 &&
         dt.day >= 1 && dt.day <= 31 &&
         dt.hour <= 23 && dt.minute <= 59 && dt.second <= 59;
}

static uint8_t monthFromBuildDate(const char* date) {
  if (strncmp(date, "Jan", 3) == 0) return 1;
  if (strncmp(date, "Feb", 3) == 0) return 2;
  if (strncmp(date, "Mar", 3) == 0) return 3;
  if (strncmp(date, "Apr", 3) == 0) return 4;
  if (strncmp(date, "May", 3) == 0) return 5;
  if (strncmp(date, "Jun", 3) == 0) return 6;
  if (strncmp(date, "Jul", 3) == 0) return 7;
  if (strncmp(date, "Aug", 3) == 0) return 8;
  if (strncmp(date, "Sep", 3) == 0) return 9;
  if (strncmp(date, "Oct", 3) == 0) return 10;
  if (strncmp(date, "Nov", 3) == 0) return 11;
  if (strncmp(date, "Dec", 3) == 0) return 12;
  return 1;
}

static RtcDateTime buildTime() {
  RtcDateTime dt;
  const char* date = __DATE__;
  const char* time = __TIME__;
  dt.month = monthFromBuildDate(date);
  dt.day = atoi(date + 4);
  dt.year = atoi(date + 7);
  dt.hour = atoi(time);
  dt.minute = atoi(time + 3);
  dt.second = atoi(time + 6);
  return dt;
}

static bool setPcf8523(const RtcDateTime& dt) {
  uint8_t data[7] = {
    decToBcd(dt.second),
    decToBcd(dt.minute),
    decToBcd(dt.hour),
    decToBcd(dt.day),
    0,
    decToBcd(dt.month),
    decToBcd(dt.year % 100),
  };
  if (!writeBytes(0x03, data, sizeof(data))) return false;
  uint8_t control1 = 0x00;
  return writeBytes(0x00, &control1, 1);
}

namespace RtcClock {

bool begin(TwoWire& wire, uint8_t address) {
  rtcWire = &wire;
  rtcAddr = address;

  rtcWire->beginTransmission(rtcAddr);
  present = (rtcWire->endTransmission() == 0);
  if (!present) {
    ready = false;
    Serial.println("[RTC] Not found");
    return false;
  }

  RtcDateTime dt;
  if (read(dt)) {
    ready = true;
    Serial.printf("[RTC] Time: %s\n", format(dt).c_str());
    return true;
  }

  Serial.println("[RTC] Present but unset; setting from firmware build time");
  if (setPcf8523(buildTime()) && read(dt)) {
    ready = true;
    Serial.printf("[RTC] Set/read OK: %s\n", format(dt).c_str());
    return true;
  }

  ready = false;
  Serial.println("[RTC] Present but not readable");
  return false;
}

bool read(RtcDateTime& dt) {
  if (!present || rtcWire == nullptr) return false;

  uint8_t data[7] = {};
  if (!readBytes(0x03, data, sizeof(data))) return false;

  dt.second = bcdToDec(data[0] & 0x7F);
  dt.minute = bcdToDec(data[1] & 0x7F);
  dt.hour = bcdToDec(data[2] & 0x3F);
  dt.day = bcdToDec(data[3] & 0x3F);
  dt.month = bcdToDec(data[5] & 0x1F);
  dt.year = 2000 + bcdToDec(data[6]);

  ready = valid(dt);
  return ready;
}

bool isPresent() {
  return present;
}

bool isReady() {
  return ready;
}

String format(const RtcDateTime& dt) {
  char buf[24];
  snprintf(buf, sizeof(buf), "%04u-%02u-%02u %02u:%02u:%02u",
           dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);
  return String(buf);
}

}
