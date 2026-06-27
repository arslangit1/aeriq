#include "sensors.h"
#include "s88.h"
#include "sen55.h"

#include "../app_config.h"

#include <Wire.h>
#include <Adafruit_VEML7700.h>
#include <WiFi.h>  // only used for optional serial status line text

// Sensor instances are private to this module
static Adafruit_VEML7700 veml;
static Sen55 sen55;

namespace Sensors {

void begin(Readings& state) {
  state.veml_ok = veml.begin(&Wire);
  Serial.println(state.veml_ok ? "VEML7700 found!" : "VEML7700 not found (expected 0x10).");

  S88::begin(S88_UART_RX_PIN, S88_UART_TX_PIN, S88_UART_BAUD);
  Serial.printf("S88 UART initialized on RX=%d, TX=%d @ %lu baud\n",
                S88_UART_RX_PIN, S88_UART_TX_PIN, (unsigned long)S88_UART_BAUD);

  state.sen55_ok = sen55.begin(Wire, SEN55_I2C_ADDR);
  if (!state.sen55_ok) {
    Serial.println("SEN55 not started (check wiring, power, SEL=I2C).");
  }
}

void read(Readings& state, bool wifiConnected) {
  state.ms = millis();

  if (state.veml_ok) {
    state.lux   = veml.readLux();
    state.white = veml.readWhite();
    state.als   = veml.readALS();
  } else {
    state.lux   = NAN;
    state.white = NAN;
    state.als   = 0;
  }

  uint16_t ppm = 0;
  if (S88::readCO2ppm(ppm, 300)) {
    state.co2_ppm = ppm;
    state.s88_ok = true;
  } else {
    state.s88_ok = false;
    Serial.printf("[%10lu ms] S88 read error (timeout/CRC/header)\n", state.ms);
  }

  if (sen55.isOk()) {
    float pm1, pm25, pm4, pm10, tC, rh, voc, nox;
    bool ok = sen55.read(pm1, pm25, pm4, pm10, tC, rh, voc, nox);
    state.sen55_ok = ok;

    if (ok) {
      state.pm1_0 = pm1;
      state.pm2_5 = pm25;
      state.pm4_0 = pm4;
      state.pm10_0 = pm10;
      state.sen55_tC = tC;
      state.sen55_rh = rh;
      state.tC = tC;
      state.rh = rh;
      state.voc_index = voc;
      state.nox_index = nox;
    } else {
      state.pm1_0 = state.pm2_5 = state.pm4_0 = state.pm10_0 = NAN;
      state.sen55_tC = state.sen55_rh = NAN;
      state.tC = state.rh = NAN;
      state.voc_index = state.nox_index = NAN;
    }
  } else {
    state.sen55_ok = false;
  }

  static uint32_t lastStatusPrintMs = 0;
  if (millis() - lastStatusPrintMs >= SERIAL_STATUS_PERIOD_MS) {
    lastStatusPrintMs = millis();

    Serial.printf(
      "[%10lu ms] T=%.2f C, RH=%.2f %% | Lux=%.2f | ALS=%u | CO2=%u ppm | WiFi=%s | OK(sen55/veml/s88)=%d/%d/%d\n",
      state.ms,
      state.tC, state.rh,
      state.lux, state.als,
      state.co2_ppm,
      (wifiConnected ? "OK" : "DOWN"),
      state.sen55_ok ? 1 : 0,
      state.veml_ok ? 1 : 0,
      state.s88_ok ? 1 : 0
    );
  }

}

int countHealthy(const Readings& state) {
  int n = 0;
  if (state.sen55_ok) n++;
  if (state.veml_ok) n++;
  if (state.s88_ok) n++;
  return n;
}

} // namespace Sensors
