#include "display_lcd.h"

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

static constexpr int TFT_W = 240;
static constexpr int TFT_H = 280;
static constexpr int VALUE_X = 76;
static constexpr int VALUE_W = TFT_W - VALUE_X - 4;
static constexpr int LINE_H = 16;

static Adafruit_ST7789* tft = nullptr;
static LcdPins g_pins { -1, -1, -1, -1, -1, -1 };
static bool g_ok = false;
static bool g_layoutDrawn = false;
static uint32_t g_lastDrawMs = 0;

static uint16_t okColor(bool ok) {
  return ok ? ST77XX_GREEN : ST77XX_RED;
}

static String valueOrDash(float value, uint8_t decimals) {
  if (isnan(value)) return "-";
  return String(value, static_cast<unsigned int>(decimals));
}

static void printAt(int x, int y, const String& text, uint16_t color = ST77XX_WHITE) {
  tft->setTextColor(color);
  tft->setCursor(x, y);
  tft->print(text);
}

static void drawLabel(int y, const char* label) {
  printAt(8, y, label, ST77XX_WHITE);
}

static void drawValue(int y, const String& text, uint16_t color = ST77XX_CYAN) {
  tft->fillRect(VALUE_X, y - 1, VALUE_W, 11, ST77XX_BLACK);
  printAt(VALUE_X, y, text, color);
}

static void drawStatusTag(int x, int y, const char* label, bool ok) {
  tft->fillRect(x, y - 1, 34, 11, ST77XX_BLACK);
  printAt(x, y, label, okColor(ok));
}

static void drawStaticLayout() {
  tft->fillScreen(ST77XX_BLACK);
  tft->setTextWrap(false);

  tft->setTextSize(2);
  printAt(8, 6, "IAQM", ST77XX_CYAN);

  tft->setTextSize(1);
  drawLabel(70, "Temp:");
  drawLabel(86, "RH:");
  drawLabel(102, "CO2:");
  drawLabel(118, "Lux:");
  drawLabel(134, "ALS:");
  drawLabel(150, "PM1:");
  drawLabel(166, "PM2.5:");
  drawLabel(182, "PM4:");
  drawLabel(198, "PM10:");
  drawLabel(214, "VOC:");
  drawLabel(230, "NOx:");

  printAt(8, 258, "Log every 10s", ST77XX_YELLOW);
  g_layoutDrawn = true;
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
  g_layoutDrawn = false;
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

  if (!g_layoutDrawn) {
    drawStaticLayout();
  }

  tft->setTextSize(1);

  tft->fillRect(8, 30, 224, 11, ST77XX_BLACK);
  printAt(8, 30, r.rtc_ok ? String(r.timestamp) : String("RTC not ready"),
          r.rtc_ok ? ST77XX_WHITE : ST77XX_YELLOW);

  drawStatusTag(8, 48, "WiFi", wifiOk);
  drawStatusTag(56, 48, "SD", sdOk);
  drawStatusTag(92, 48, "RTC", r.rtc_ok);
  drawStatusTag(132, 48, "VEML", r.veml_ok);
  drawStatusTag(180, 48, "SEN", r.sen55_ok);
  drawStatusTag(216, 48, "S88", r.s88_ok);

  drawValue(70, valueOrDash(r.tC, 1) + " C");
  drawValue(86, valueOrDash(r.rh, 1) + " %");
  drawValue(102, String(r.co2_ppm) + " ppm");
  drawValue(118, valueOrDash(r.lux, 1));
  drawValue(134, String(r.als));
  drawValue(150, valueOrDash(r.pm1_0, 1));
  drawValue(166, valueOrDash(r.pm2_5, 1));
  drawValue(182, valueOrDash(r.pm4_0, 1));
  drawValue(198, valueOrDash(r.pm10_0, 1));
  drawValue(214, valueOrDash(r.voc_index, 1));
  drawValue(230, valueOrDash(r.nox_index, 1));
}
