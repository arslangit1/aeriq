# AerIQ / IAQM Bringup

Indoor Air Quality Monitor firmware for an Adafruit Feather ESP32-S3. The project reads multiple air-quality and environmental sensors, presents live values on an ST7789 LCD, exposes a browser dashboard over Wi-Fi, advertises itself through mDNS at `http://iaqm.local/`, and logs samples to an SD card as CSV.

## What The Firmware Does

- Connects to Wi-Fi using credentials from `src/secrets.h`
- Starts an HTTP server on port `80`
- Advertises `http://iaqm.local/` over mDNS
- Polls environmental sensors every `5 s`
- Updates a local LCD once per second
- Logs readings to `/iaq_log.csv` on the SD card
- Exposes machine-readable data at:
  - `/json`
  - `/metrics`

## Target Board

- PlatformIO environment: `adafruit_feather_esp32s3`
- Board: Adafruit Feather ESP32-S3
- Board defaults used by this project:
  - I2C SDA: `GPIO3`
  - I2C SCL: `GPIO4`
  - Hardware SPI MOSI: `GPIO35`
  - Hardware SPI MISO: `GPIO37`
  - Hardware SPI SCK: `GPIO36`
  - UART1 RX: `GPIO38`
  - UART1 TX: `GPIO39`
  - Built-in LED: `GPIO13`
  - Onboard NeoPixel data: `GPIO33`
  - Onboard NeoPixel power: `GPIO21`

The board defaults above come from the Adafruit Feather ESP32-S3 Arduino variant file used by this build.

## Sensors And Peripherals Used

### Implemented In Firmware

- Sensirion SHT45 on I2C
  - Temperature
  - Relative humidity
- Adafruit VEML7700 on I2C
  - Ambient light
  - ALS / white channel values
- Senseair S88 on UART/Modbus
  - CO2 in ppm
- Sensirion SEN55 on I2C
  - PM1.0
  - PM2.5
  - PM4.0
  - PM10
  - Internal temperature / humidity
  - VOC index
  - NOx index
- ST7789 SPI LCD
  - Local display for live readings
- SD card over SPI
  - CSV logging to `/iaq_log.csv`

### Referenced But Not Actually Used

- RTC at I2C address `0x68` is defined in configuration, but there is no RTC driver or RTC initialization in the project.

## Wiring / Pin Map

### Shared I2C Bus

These devices share the same I2C bus:

- Feather `GPIO3 (SDA)` -> SHT45 `SDA`
- Feather `GPIO4 (SCL)` -> SHT45 `SCL`
- Feather `3V` -> SHT45 `VCC`
- Feather `GND` -> SHT45 `GND`

- Feather `GPIO3 (SDA)` -> VEML7700 `SDA`
- Feather `GPIO4 (SCL)` -> VEML7700 `SCL`
- Feather `3V` -> VEML7700 `VIN`
- Feather `GND` -> VEML7700 `GND`

- Feather `GPIO3 (SDA)` -> SEN55 `SDA`
- Feather `GPIO4 (SCL)` -> SEN55 `SCL`
- Feather `3V/5V as required by your SEN55 carrier` -> SEN55 `VIN`
- Feather `GND` -> SEN55 `GND`
- SEN55 interface selection must be set for `I2C` mode

Detected / expected addresses from the code:

- SHT45: `0x44`
- VEML7700: `0x10`
- SEN55: `0x69`
- RTC placeholder: `0x68`

### Senseair S88 UART

- Feather `GPIO38 (RX1)` <- S88 `TXD`
- Feather `GPIO39 (TX1)` -> S88 `RXD`
- Feather `GND` -> S88 `GND`
- Feather supply -> S88 supply as required by the sensor/carrier

UART settings:

- `9600 baud`
- `8N1`
- Modbus request/response framing in software

### LCD Display (ST7789)

The display uses hardware SPI plus dedicated control pins:

- Feather `GPIO35 (MOSI)` -> LCD `MOSI / SDA`
- Feather `GPIO36 (SCK)` -> LCD `SCK / CLK`
- Feather `GPIO9` -> LCD `CS`
- Feather `GPIO12` -> LCD `DC`
- Feather `GPIO11` -> LCD `RST`
- Feather `GPIO10` -> LCD `BL`
- Feather `3V` -> LCD `VCC`
- Feather `GND` -> LCD `GND`

Notes:

- The ST7789 display does not use MISO in this project.
- The code initializes the display as a `240x280` panel.

### SD Card

The SD card also uses the Feather hardware SPI bus:

- Feather `GPIO35 (MOSI)` -> SD `MOSI`
- Feather `GPIO37 (MISO)` -> SD `MISO`
- Feather `GPIO36 (SCK)` -> SD `SCK`
- Feather `GPIO42` -> SD `CS`
- Feather `3V` -> SD `VCC`
- Feather `GND` -> SD `GND`

Notes:

- Logging path: `/iaq_log.csv`
- SPI frequency configured in code: `4 MHz`
- The display and SD card are intended to share the same SPI bus with different chip-select pins

### Onboard Indicators

- Built-in LED on `GPIO13`
  - Used as a Wi-Fi status blinker
- Onboard NeoPixel on `GPIO33`
  - Used to blink the count of healthy sensors every 10 seconds
- NeoPixel power enable on `GPIO21`

## Software Layout

- `src/main.cpp`
  - System bring-up, networking, display, logging, periodic loop
- `src/sensors/`
  - Sensor drivers and aggregation
- `src/ui/`
  - LCD and indicator behavior
- `src/net/`
  - Wi-Fi, mDNS, and web server
- `src/misc/sd_logger.*`
  - CSV logging support
- `src/app_config.h`
  - Project-wide pin map, addresses, and timing constants

## Build And Setup

1. Copy `src/secrets_example.h` to `src/secrets.h`
2. Fill in:
   - `WIFI_SSID`
   - `WIFI_PASS`
3. Build and upload with PlatformIO using the `adafruit_feather_esp32s3` environment

Current serial and upload settings from `platformio.ini`:

- Monitor speed: `115200`
- Upload protocol: `esptool`
- Upload speed: `921600`
- Upload port: `COM5`
- Monitor port: `COM6`

## Logged Data

The CSV header written by the firmware is:

`ts_ms,temp_c,rh_pct,lux,white,als,co2_ppm,pm1_0,pm2_5,pm4_0,pm10,sen55_tC,sen55_rh,voc_index,nox_index`

## Discrepancies Found

### 1. LCD backlight comment does not match implementation

- `main.cpp` says the backlight is "tied to 3V for now (no PWM)"
- The code actually assigns `LCD_BL_PIN = GPIO10` and drives it with `analogWrite()`

Impact:

- The comment is misleading for wiring and debugging

### 2. SEN55 health is not included in the NeoPixel "healthy sensor count"

- The project reads and reports SEN55 data
- `Sensors::countHealthy()` only counts `SHT45`, `VEML7700`, and `S88`
- `SEN55` is omitted from that health count

Impact:

- The onboard NeoPixel under-reports sensor health

### 3. `SEN55_I2C_ADDR` is defined but not used consistently

- `app_config.h` defines `SEN55_I2C_ADDR`
- `sensors.cpp` calls `sen55.begin(Wire, 0x69)` directly instead of using the constant

Impact:

- Harmless now, but it creates avoidable configuration drift

### 4. RTC address is defined, but there is no RTC implementation

- `RTC_I2C_ADDR = 0x68` exists in configuration
- No RTC code uses it

Impact:

- The README and hardware expectations should treat RTC support as planned, not implemented

### 5. SD logging is active, but the web UI still says export is "coming soon"

- SD logging is already initialized in firmware and samples are appended to `/iaq_log.csv`
- The web page still says logging/export "will appear once enabled" and `/download` is only planned

Impact:

- The web UI understates what is already working locally

## Practical Summary

This project is already a solid IAQ bring-up firmware for the Adafruit Feather ESP32-S3 with:

- SHT45
- VEML7700
- Senseair S88
- SEN55
- ST7789 LCD
- SD CSV logging
- Wi-Fi dashboard and mDNS
