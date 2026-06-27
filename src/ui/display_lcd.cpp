#include "display_lcd.h"

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

static constexpr int TFT_W = 240;
static constexpr int TFT_H = 280;

static Adafruit_ST7789* tft = nullptr;
static LcdPins g_pins { -1, -1, -1, -1, -1, -1 };
static bool g_ok = false;
static uint32_t g_lastDrawMs = 0;

static uint16_t okColor(bool ok) {
  return ok ? ST77XX_GREEN : ST77XX_RED;
}

static void printAt(int x, int y, const String& text, uint16_t color = ST77XX_WHITE) {
  tft->setTextColor(color);
  tft->setCursor(x, y);
  tft->print(text);
}

static String valueOrDash(float value, uint8_t decimals) {
  if (isnan(value)) return "-";
  return String(value, static_cast<unsigned int>(decimals));
}

bool display_init(const LcdPins& pins) {
  g_pins = pins;

  if (g_pins.bl >= 0) {
    pinMode(g_pins.bl, OUTPUT);
    analogWrite(g_pins.bl, 255);
  }

  Serial.printf("[LCD] CS=%d DC=%d RST=%d BL=%d MOSI=%d SCK=%d\n",
                g_pins.cs, g_pins.dc, g_pins.rst, g_pins.bl,
                g_pins.mosi, g_pins.sck);

  tft = new Adafruit_ST7789(g_pins.cs, g_pins.dc, g_pins.mosi, g_pins.sck,
                            g_pins.rst);
  tft->init(TFT_W, TFT_H);
  tft->setRotation(0);
  tft->setTextWrap(false);
  tft->fillScreen(ST77XX_BLACK);

  g_ok = true;
  return true;
}

void display_set_backlight(uint8_t duty_0_255) {
  if (!g_ok || g_pins.bl < 0) return;
  analogWrite(g_pins.bl, duty_0_255);
}

void display_update(const Readings& r, bool wifiOk, bool sdOk) {
  if (!g_ok || !tft) return;

  const uint32_t now = millis();
  if (now - g_lastDrawMs < 1000) return;
  g_lastDrawMs = now;

  tft->fillScreen(ST77XX_BLACK);
  tft->setTextSize(2);
  printAt(8, 6, "IAQM", ST77XX_CYAN);

  tft->setTextSize(1);
  printAt(8, 30, r.rtc_ok ? String(r.timestamp) : String("RTC not ready"),
          r.rtc_ok ? ST77XX_WHITE : ST77XX_YELLOW);

  printAt(8, 48, "WiFi", okColor(wifiOk));
  printAt(56, 48, "SD", okColor(sdOk));
  printAt(92, 48, "RTC", okColor(r.rtc_ok));
  printAt(132, 48, "VEML", okColor(r.veml_ok));
  printAt(180, 48, "SEN", okColor(r.sen55_ok));
  printAt(216, 48, "S88", okColor(r.s88_ok));

  int y = 70;
  printAt(8, y, "Temp: " + valueOrDash(r.tC, 1) + " C"); y += 16;
  printAt(8, y, "RH:   " + valueOrDash(r.rh, 1) + " %"); y += 16;
  printAt(8, y, "CO2:  " + String(r.co2_ppm) + " ppm"); y += 16;
  printAt(8, y, "Lux:  " + valueOrDash(r.lux, 1)); y += 16;
  printAt(8, y, "ALS:  " + String(r.als)); y += 16;
  printAt(8, y, "PM1:  " + valueOrDash(r.pm1_0, 1)); y += 16;
  printAt(8, y, "PM2.5:" + valueOrDash(r.pm2_5, 1)); y += 16;
  printAt(8, y, "PM4:  " + valueOrDash(r.pm4_0, 1)); y += 16;
  printAt(8, y, "PM10: " + valueOrDash(r.pm10_0, 1)); y += 16;
  printAt(8, y, "VOC:  " + valueOrDash(r.voc_index, 1)); y += 16;
  printAt(8, y, "NOx:  " + valueOrDash(r.nox_index, 1)); y += 16;

  printAt(8, 258, "Log every 10s", ST77XX_YELLOW);
}
