#pragma once
#include <stdint.h>

// =====================
// Wi-Fi
// =====================
#if __has_include("secrets.h")
  #include "secrets.h"
#endif

#ifndef WIFI_SSID
  #define WIFI_SSID "IASTATE-Guest"
#endif

#ifndef WIFI_PASS
  #define WIFI_PASS ""
#endif

// =====================
// mDNS
// =====================
static const char* MDNS_NAME = "iaqm";   // http://iaqm.local/
static const char* WEB_HOST_NAME = "http://iaqm.local/";

// =====================
// I2C addresses
// =====================
static constexpr uint8_t VEML7700_I2C_ADDR = 0x10;  // VEML7700 ALS/PS
static constexpr uint8_t SEN55_I2C_ADDR = 0x69;     // Sensirion SEN55 (PM, VOC, NOx)
static constexpr uint8_t RTC_I2C_ADDR = 0x68;       // PCF8523 RTC
static constexpr int I2C_POWER_PIN = 7;             // Feather I2C power enable

// =====================
// Senseair S88 (UART Modbus)
// =====================
static constexpr int S88_UART_RX_PIN = 38;   // ESP32 RX  <- S88 TXD
static constexpr int S88_UART_TX_PIN = 39;   // ESP32 TX  -> S88 RXD
static constexpr uint32_t S88_UART_BAUD = 9600;

// =====================
// SD Card SPI
// =====================
static constexpr int SD_CS_PIN   = 6;    // SD Card Chip Select
static constexpr int SD_MOSI_PIN = 11;   // SD Card MOSI
static constexpr int SD_MISO_PIN = 13;   // SD Card MISO
static constexpr int SD_SCK_PIN  = 12;   // SD Card SCK

static constexpr uint32_t SD_SPI_FREQ  = 4000000;   // SD Card SPI frequency

// =====================
// LCD Display SPI (ST7789V2 240x280)
// =====================
static constexpr int LCD_CS_PIN  = 14;    // LCD Chip Select
static constexpr int LCD_DC_PIN  = 15;    // LCD Data/Command
static constexpr int LCD_RST_PIN = 16;    // LCD Reset
static constexpr int LCD_BL_PIN  = 10;    // LCD Backlight (set to -1 if tied to 3V)
static constexpr int LCD_MOSI_PIN = 35;   // LCD MOSI / SDA
static constexpr int LCD_SCK_PIN  = 36;   // LCD SCK / CLK

// =====================
// RGBW NeoPixel (WS2812B) indicator
// =====================
static constexpr int NEOPIXEL_PIN = 5;   // NeoPixel data pin

// =====================
// Scheduler periods
// =====================
static constexpr uint32_t SENSOR_READ_PERIOD_MS   = 5000;     // read sensors every 5s
static constexpr uint32_t SD_LOG_PERIOD_MS        = 10000;    // log one row every 10s
static constexpr uint32_t WIFI_CHECK_PERIOD_MS    = 5000;     // check Wi-Fi connection every 5s
static constexpr uint32_t SERIAL_STATUS_PERIOD_MS = 60000;    // print status line to Serial every 60s

// =====================
// Indicator timing parameters
// =====================
static constexpr uint32_t NP_CYCLE_PERIOD_MS   = 10000;  // every 10s
static constexpr uint32_t NP_BLINK_ON_MS       = 150;    // one short status pulse
