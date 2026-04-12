#pragma once
#include <stdint.h>

// =====================
// Secrets (Wi-Fi, etc.)
// =====================
#include "secrets.h"
#ifndef WIFI_SSID
  #error "WIFI_SSID not defined. Create src/secrets.h from secrets_example.h"
#endif

#ifndef WIFI_PASS
  #error "WIFI_PASS not defined. Create src/secrets.h from secrets_example.h"
#endif

// =====================
// mDNS
// =====================
static const char* MDNS_NAME = "iaqm";   // http://iaqm.local/
static const char* WEB_HOST_NAME = "http://iaqm.local/";

// =====================
// I2C addresses
// =====================
static constexpr uint8_t SHT4X_I2C_ADDR = 0x44;     // SHT45 (SHT4x family)
static constexpr uint8_t VEML7700_I2C_ADDR = 0x10;  // VEML7700 ALS/PS
static constexpr uint8_t SEN55_I2C_ADDR = 0x69;     // Sensirion SEN55 (PM, VOC, NOx)
static constexpr uint8_t RTC_I2C_ADDR = 0x68;       // PCF8523 RTC

// =====================
// Senseair S88 (UART Modbus)
// =====================
static constexpr int S88_UART_RX_PIN = 38;   // ESP32 RX  <- S88 TXD
static constexpr int S88_UART_TX_PIN = 39;   // ESP32 TX  -> S88 RXD
static constexpr uint32_t S88_UART_BAUD = 9600;

// =====================
// SD Card SPI
// =====================
// static constexpr int SD_CS_PIN   = 5;    // SD Card Chip Select (moved off GPIO13 to avoid LED conflict)
// static constexpr int SD_MOSI_PIN = 35;   // SD Card MOSI
// static constexpr int SD_MISO_PIN = 37;   // SD Card MISO
// static constexpr int SD_SCK_PIN  = 36;   // SD Card SCK

static constexpr int SD_CS_PIN   = 6;    // SD Card Chip Select
static constexpr int SD_MOSI_PIN = 11;   // SD Card MOSI
static constexpr int SD_MISO_PIN = 13;   // SD Card MISO
static constexpr int SD_SCK_PIN  = 12;   // SD Card SCK

static constexpr uint32_t SD_SPI_FREQ  = 4000000;   // SD Card SPI frequency

// =====================
// LCD Display
// =====================
// static constexpr int LCD_CS_PIN  = 9;    // LCD Chip Select
// static constexpr int LCD_DC_PIN  = 12;    // LCD Data/Command
// static constexpr int LCD_RST_PIN = 11;    // LCD Reset
// static constexpr int LCD_BL_PIN  = 10;    // LCD Backlight (set to -1 if tied to 3V)

static constexpr int LCD_CS_PIN  = 9;    // LCD Chip Select
static constexpr int LCD_DC_PIN  = 5;    // LCD Data/Command
static constexpr int LCD_RST_PIN = 16;    // LCD Reset
static constexpr int LCD_BL_PIN  = 10;    // LCD Backlight (set to -1 if tied to 3V)


// =====================
// Scheduler periods
// =====================
static constexpr uint32_t SENSOR_READ_PERIOD_MS   = 5000;     // read sensors every 5s
static constexpr uint32_t WIFI_CHECK_PERIOD_MS    = 5000;     // check Wi-Fi connection every 5s
static constexpr uint32_t SERIAL_STATUS_PERIOD_MS = 60000;    // print status line to Serial every 60s

// =====================
// Indicator timing parameters
// =====================
static constexpr uint32_t WIFI_BLINK_PERIOD_MS = 2000;   // once every 2s
static constexpr uint32_t WIFI_BLINK_ON_MS     = 100;

static constexpr uint32_t NP_CYCLE_PERIOD_MS   = 10000;  // every 10s
static constexpr uint32_t NP_BLINK_ON_MS       = 150;
static constexpr uint32_t NP_BLINK_OFF_MS      = 150;
