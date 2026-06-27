#include <Arduino.h>
#include <Wire.h>

#include "app_config.h"
#include "app_state.h"

#include "sensors/i2c_utils.h"
#include "sensors/sensors.h"

#include "net/wifi_mgr.h"
#include "net/web_server.h"
#include "net/mdns_mgr.h"

#include "ui/indicators.h"
#include "ui/display_lcd.h"

#include "misc/sd_logger.h"
#include "misc/rtc_clock.h"

static SdLogger sdLogger;

static void updateRtcState() {
  RtcDateTime dt;
  g.rtc_ok = RtcClock::read(dt);
  if (g.rtc_ok) {
    String stamp = RtcClock::format(dt);
    stamp.toCharArray(g.timestamp, sizeof(g.timestamp));
  } else {
    strlcpy(g.timestamp, "-", sizeof(g.timestamp));
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("AerIQ - Indoor Air Quality Monitor");

  pinMode(I2C_POWER_PIN, OUTPUT);
  digitalWrite(I2C_POWER_PIN, HIGH);
  delay(50);

  Wire.begin();
  Wire.setClock(100000);
  I2CUtils::scan(Serial);

  RtcClock::begin(Wire, RTC_I2C_ADDR);
  updateRtcState();

  LcdPins lcdPins;
  lcdPins.cs = LCD_CS_PIN;
  lcdPins.dc = LCD_DC_PIN;
  lcdPins.rst = LCD_RST_PIN;
  lcdPins.bl = LCD_BL_PIN;
  lcdPins.mosi = LCD_MOSI_PIN;
  lcdPins.sck = LCD_SCK_PIN;
  display_init(lcdPins);

  Indicators::begin();
  Sensors::begin(g);

  WifiMgr::begin();
  WebServerMgr::begin();
  MdnsMgr::begin();

  bool sdOk = sdLogger.begin(SD_CS_PIN, SD_SCK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_SPI_FREQ);
  Serial.println(sdOk ? "[SD] Logger ready" : "[SD] Logger unavailable");

  Serial.println("Open a browser to: http://iaqm.local/");
}

void loop() {
  WebServerMgr::tick();
  WifiMgr::tick();
  updateRtcState();

  static uint32_t lastReadMs = 0;
  if (millis() - lastReadMs >= SENSOR_READ_PERIOD_MS) {
    Sensors::read(g, WifiMgr::isConnected());
    lastReadMs = millis();
  }

  const bool wifiOk = WifiMgr::isConnected();
  const bool sdOk = sdLogger.isReady();

  Indicators::tick(g, wifiOk, sdOk);
  display_update(g, wifiOk, sdOk);

  static uint32_t lastLogMs = 0;
  if (sdLogger.isReady() && millis() - lastLogMs >= SD_LOG_PERIOD_MS) {
    if (!sdLogger.appendSample(g, wifiOk)) {
      Serial.println("[SD] append failed.");
    }
    lastLogMs = millis();
  }
}
