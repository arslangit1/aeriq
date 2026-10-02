#include "display_lcd.h"

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>

static constexpr int TFT_W = 240;
static constexpr int TFT_H = 280;
static constexpr int SAFE_MARGIN = 12;
static constexpr int CONTENT_W = TFT_W - (SAFE_MARGIN * 2);
static constexpr uint16_t UI_GREY = 0x7BEF;
static constexpr uint16_t UI_PANEL = 0x10A2;
static constexpr uint16_t UI_PANEL_DARK = 0x0841;
static constexpr uint16_t UI_ACCENT = 0x3666;

static Adafruit_ST7789* tft = nullptr;
static LcdPins g_pins { -1, -1, -1, -1, -1, -1 };
static bool g_ok = false;
static bool g_layoutDrawn = false;
static uint32_t g_lastDrawMs = 0;
static bool g_dynamicStateValid = false;
static String g_dateComponents[3];
static String g_timeComponents[3];
static String g_meridiem;
static bool g_clockValid = false;
static String g_tileValues[6];
static String g_temperatureText;
static String g_humidityText;
static bool g_wifiState = false;
static bool g_sensorStates[4] = {};

static uint16_t okColor(bool ok) {
  return ok ? ST77XX_GREEN : ST77XX_RED;
}

static String valueOrDash(float value, uint8_t decimals) {
  if (isnan(value)) return "-";
  return String(value, static_cast<unsigned int>(decimals));
}

static String timestampPart(const char* timestamp, bool datePart) {
  String value(timestamp);
  const int separator = value.indexOf(' ');
  if (separator < 0) return value;
  return datePart ? value.substring(0, separator) : value.substring(separator + 1);
}

static String timestampComponent(const char* timestamp, uint8_t start,
                                 uint8_t length) {
  String value(timestamp);
  if (start + length > value.length()) return "";
  return value.substring(start, start + length);
}

static void useRegularFont() {
  tft->setFont(&FreeSans9pt7b);
  tft->setTextSize(1);
}

static void useCompactFont() {
  tft->setFont();
  tft->setTextSize(1);
}

static void useSensorFont() {
  tft->setFont(&FreeSansBold12pt7b);
  tft->setTextSize(1);
}

static void printAt(int x, int y, const String& text,
                    uint16_t color = ST77XX_WHITE) {
  tft->setTextColor(color);
  tft->setCursor(x, y);
  tft->print(text);
}

static void drawWifiIcon(int x, int y, uint16_t color) {
  tft->drawCircleHelper(x, y, 9, 1, color);
  tft->drawCircleHelper(x, y, 6, 1, color);
  tft->drawCircleHelper(x, y, 3, 1, color);
  tft->fillCircle(x, y + 2, 2, color);
}

static void drawThermometer(int x, int y, uint16_t color) {
  tft->drawRoundRect(x + 4, y, 5, 13, 2, color);
  tft->fillCircle(x + 6, y + 14, 4, color);
  tft->drawFastVLine(x + 6, y + 4, 8, color);
}

static void drawDroplet(int x, int y, uint16_t color) {
  tft->fillTriangle(x + 6, y, x + 1, y + 9, x + 6, y + 16, color);
  tft->fillTriangle(x + 6, y, x + 11, y + 9, x + 6, y + 16, color);
}

static void drawTile(int x, int y, const char* label, const String& value,
                     const char* unit) {
  tft->fillRoundRect(x, y, 103, 42, 5, UI_PANEL);
  tft->fillRoundRect(x + 5, y + 9, 3, 22, 2, UI_ACCENT);
  useCompactFont();
  printAt(x + 14, y + 27, unit, UI_GREY);
  printAt(x + 66, y + 14, label, UI_GREY);
  useSensorFont();
  printAt(x + 14, y + 21, value, ST77XX_WHITE);
}

static void drawTileValue(int x, int y, const String& value) {
  tft->fillRect(x + 12, y + 7, 86, 17, UI_PANEL);
  useSensorFont();
  printAt(x + 14, y + 21, value, ST77XX_WHITE);
}

static void updateTileValue(uint8_t index, int x, int y, const String& value) {
  if (g_dynamicStateValid && g_tileValues[index] == value) return;
  g_tileValues[index] = value;
  drawTileValue(x, y, value);
}

static void drawSensorStatus(int x, const char* label, bool ok) {
  tft->fillCircle(x, 272, 3, okColor(ok));
  useCompactFont();
  printAt(x + 6, 265, label, UI_GREY);
}

static void drawSensorStatusIndicator(int x, bool ok) {
  tft->fillCircle(x, 272, 3, okColor(ok));
}

static void updateSensorStatusIndicator(uint8_t index, int x, bool ok) {
  if (g_dynamicStateValid && g_sensorStates[index] == ok) return;
  g_sensorStates[index] = ok;
  drawSensorStatusIndicator(x, ok);
}

static void drawStaticLayout() {
  tft->fillScreen(ST77XX_BLACK);
  tft->setTextWrap(false);
  useRegularFont();

  drawWifiIcon(21, 15, ST77XX_GREEN);
  printAt(96, 20, "IAQM", UI_GREY);
  printAt(186, 18, "AQ", UI_GREY);
  tft->setFont(&FreeSansBold24pt7b);
  tft->setTextSize(1);
  printAt(176, 60, "00", ST77XX_WHITE);
  useRegularFont();
  printAt(46, 40, "-", ST77XX_WHITE);
  printAt(66, 40, "-", ST77XX_WHITE);
  printAt(41, 60, ":", ST77XX_WHITE);
  printAt(64, 60, ":", ST77XX_WHITE);

  drawTile(12, 72, "PM 2.5", "-", "ug/m3");
  drawTile(125, 72, "PM 10", "-", "ug/m3");
  drawTile(12, 116, "CO2", "-", "ppm");
  drawTile(125, 116, "NOx", "-", "index");
  drawTile(12, 160, "VOC", "-", "index");
  drawTile(125, 160, "LUX", "-", "lux");

  drawThermometer(25, 220, ST77XX_ORANGE);
  drawDroplet(145, 219, ST77XX_CYAN);
  tft->drawFastHLine(SAFE_MARGIN, 265, CONTENT_W, UI_PANEL_DARK);

  drawSensorStatus(27, "VEM", true);
  drawSensorStatus(82, "SEN", true);
  drawSensorStatus(137, "S88", true);
  drawSensorStatus(192, "SD", true);
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
  g_dynamicStateValid = false;
  g_clockValid = false;
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

  if (!g_layoutDrawn) drawStaticLayout();

  if (r.rtc_ok) {
    const String dateComponents[] = {
      timestampComponent(r.timestamp, 0, 4),
      timestampComponent(r.timestamp, 5, 2),
      timestampComponent(r.timestamp, 8, 2)
    };
    const String timeComponents[] = {
      timestampComponent(r.timestamp, 11, 2),
      timestampComponent(r.timestamp, 14, 2),
      timestampComponent(r.timestamp, 17, 2)
    };
    const String meridiem = timestampComponent(r.timestamp, 20, 2);

    useRegularFont();
    const int dateX[] = { 18, 53, 73 };
    const int timeX[] = { 18, 47, 70 };
    const int dateWidth[] = { 27, 13, 13 };
    const int timeWidth[] = { 19, 17, 17 };

    for (uint8_t i = 0; i < 3; ++i) {
      if (!g_dynamicStateValid || !g_clockValid ||
          g_dateComponents[i] != dateComponents[i]) {
        tft->fillRect(dateX[i], 25, dateWidth[i], 20, ST77XX_BLACK);
        printAt(dateX[i], 40, dateComponents[i], ST77XX_WHITE);
        g_dateComponents[i] = dateComponents[i];
      }
      if (!g_dynamicStateValid || !g_clockValid ||
          g_timeComponents[i] != timeComponents[i]) {
        tft->fillRect(timeX[i], 45, timeWidth[i], 20, ST77XX_BLACK);
        printAt(timeX[i], 60, timeComponents[i], ST77XX_WHITE);
        g_timeComponents[i] = timeComponents[i];
      }
    }
    if (!g_dynamicStateValid || !g_clockValid || g_meridiem != meridiem) {
      tft->fillRect(88, 45, 24, 20, ST77XX_BLACK);
      printAt(88, 60, meridiem, ST77XX_WHITE);
      g_meridiem = meridiem;
    }
    g_clockValid = true;
  } else if (!g_dynamicStateValid || g_clockValid) {
    useRegularFont();
    tft->fillRect(18, 25, 27, 20, ST77XX_BLACK);
    tft->fillRect(53, 25, 13, 20, ST77XX_BLACK);
    tft->fillRect(73, 25, 13, 20, ST77XX_BLACK);
    tft->fillRect(18, 45, 19, 20, ST77XX_BLACK);
    tft->fillRect(47, 45, 17, 20, ST77XX_BLACK);
    tft->fillRect(70, 45, 17, 20, ST77XX_BLACK);
    tft->fillRect(88, 45, 24, 20, ST77XX_BLACK);
    printAt(18, 40, "RTC N/A", ST77XX_RED);
    g_clockValid = false;
  }

  updateTileValue(0, 12, 72, valueOrDash(r.pm2_5, 1));
  updateTileValue(1, 125, 72, valueOrDash(r.pm10_0, 1));
  updateTileValue(2, 12, 116, r.s88_ok ? String(r.co2_ppm) : String("-"));
  updateTileValue(3, 125, 116, valueOrDash(r.nox_index, 0));
  updateTileValue(4, 12, 160, valueOrDash(r.voc_index, 0));
  updateTileValue(5, 125, 160, valueOrDash(r.lux, 0));

  const String temperatureText =
    valueOrDash(r.tC, 1) + String("\xB0") + "C";
  const String humidityText = valueOrDash(r.rh, 1) + "%";
  if (!g_dynamicStateValid || g_temperatureText != temperatureText ||
      g_humidityText != humidityText) {
    tft->fillRect(35, 214, 91, 30, ST77XX_BLACK);
    tft->fillRect(155, 214, 73, 30, ST77XX_BLACK);
    useSensorFont();
    printAt(39, 236, temperatureText, ST77XX_ORANGE);
    printAt(159, 236, humidityText, ST77XX_CYAN);
    g_temperatureText = temperatureText;
    g_humidityText = humidityText;
  }

  updateSensorStatusIndicator(0, 27, r.veml_ok);
  updateSensorStatusIndicator(1, 82, r.sen55_ok);
  updateSensorStatusIndicator(2, 137, r.s88_ok);
  updateSensorStatusIndicator(3, 192, sdOk);

  if (!g_dynamicStateValid || g_wifiState != wifiOk) {
    tft->fillRect(10, 8, 23, 18, ST77XX_BLACK);
    drawWifiIcon(21, 15, wifiOk ? ST77XX_GREEN : ST77XX_RED);
    g_wifiState = wifiOk;
  }

  g_dynamicStateValid = true;
}
