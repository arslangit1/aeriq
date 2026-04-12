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

#include <SPI.h>
#include <SD.h>

#define sdSpi SPI

static SdLogger sdLogger;
static SPISettings sdProbeSettings(400000, MSBFIRST, SPI_MODE0);

void setup() {
  Serial.begin(115200);
  delay(2000); // wait for 2 seconds for serial monitor

  Serial.println("\nAerIQ - Indoor Air Quality Monitor");

  // // Initialize SD card logger
  // bool sdOk = sdLogger.begin(SD_CS_PIN, SD_SCK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_SPI_FREQ);
  // if (!sdOk) {
  //   Serial.println("[SD] SD Card initialization failed (card missing, wiring, or CS conflict). Logging disabled.");
  // } else {
  //   Serial.println("[SD] SD Card Initialized successfully.");
  //   Serial.print("[SD] logging to ");
  //   Serial.println(sdLogger.filePath());
  // }

  // Initialize I2C
  Wire.begin();
  Wire.setClock(100000);
  

  // Define LCD pins and initialize display
  LcdPins lcdPins;
  lcdPins.cs  = LCD_CS_PIN;
  lcdPins.dc  = LCD_DC_PIN;
  lcdPins.rst = LCD_RST_PIN;
  lcdPins.bl  = LCD_BL_PIN;

  display_init(lcdPins);

  I2CUtils::scan(Serial);

  Indicators::begin();
  Sensors::begin(g);

  WifiMgr::begin();
  WebServerMgr::begin();
  MdnsMgr::begin();

  Serial.println("Open a browser to: http://iaqm.local/  (or use the IP printed above)");

    // Initialize SD card logger
  bool sdOk = sdLogger.begin(SD_CS_PIN, SD_SCK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_SPI_FREQ);
  if (!sdOk) {
    Serial.println("[SD] SD Card initialization failed (card missing, wiring, or CS conflict). Logging disabled.");
  } else {
    Serial.println("[SD] SD Card Initialized successfully.");
    Serial.print("[SD] logging to ");
    Serial.println(sdLogger.filePath());
  }
}

void loop() {
  // Handle networking
  WebServerMgr::tick();
  WifiMgr::tick();

  // Periodic sensor read
  static uint32_t lastReadMs = 0;
  if (millis() - lastReadMs >= SENSOR_READ_PERIOD_MS) {
    Sensors::read(g, WifiMgr::isConnected());
    lastReadMs = millis();
  }

  // Update indicators
  Indicators::tick(g);

  // Update LCD display
  display_update(g, WifiMgr::isConnected(), WEB_HOST_NAME);

  // Log to SD card
  if (sdLogger.isReady()) {
    if (!sdLogger.appendSample(g)) {
      Serial.println("[SD] append failed (card removed? FS issue?).");
    }
  }

}

