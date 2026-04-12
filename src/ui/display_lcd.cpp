#include "display_lcd.h"

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

// 240x280 module
static constexpr int TFT_W = 240;
static constexpr int TFT_H = 280;

// static Adafruit_ST7789 tft;     // will be constructed after pins are known
static Adafruit_ST7789* tft = nullptr;
static bool g_ok = false;

static LcdPins g_pins { -1, -1, -1, -1 };
static uint32_t g_lastDrawMs = 0;

static bool g_layoutDrawn = false;

// Coordinates for “value fields” you will update
struct Field {
  int x, y, w, h;
};
static Field F_TEMP  { 20,  68, 200, 24 };
static Field F_RH    { 20,  92, 200, 24 };
static Field F_CO2   { 140, 78,  90, 24 };

static Field F_PM25  { 20,  162, 200, 24 };
static Field F_PM10  { 140, 170,  90, 24 };

static Field F_VOC   { 20,  252, 200, 24 };
static Field F_NOX   { 140, 260,  90, 24 };


static void drawHeader(bool wifiOk, bool sdOk, const char* hostName) {
  tft->fillRect(0, 0, TFT_W, 34, wifiOk ? ST77XX_GREEN : ST77XX_RED);
  tft->setTextColor(ST77XX_BLACK);
  tft->setTextSize(2);
  tft->setCursor(8, 8);
  tft->print("IAQM");
  tft->setTextSize(1);
  tft->setCursor(120, 10);
  tft->print(wifiOk ? "WiFi OK" : "WiFi DOWN");
  tft->setCursor(120, 24);
  tft->print(sdOk ? "SD OK" : "SD FAIL");

  tft->setCursor(8, 24);
  tft->print(hostName);
}

static void drawCard(int x, int y, int w, int h, const char* title) {
  tft->drawRoundRect(x, y, w, h, 8, ST77XX_WHITE);
  tft->setTextColor(ST77XX_WHITE);
  tft->setTextSize(1);
  tft->setCursor(x + 8, y + 6);
  tft->print(title);
}

static void printValue(int x, int y, const String& s, uint16_t color = ST77XX_CYAN) {
  tft->setTextColor(color);
  tft->setTextSize(2);
  tft->setCursor(x, y);
  tft->print(s);
}

bool display_init(const LcdPins& pins) {
  g_pins = pins;

  // Backlight pin optional
  if (g_pins.bl >= 0) {
    pinMode(g_pins.bl, OUTPUT);
    analogWrite(g_pins.bl, 255); // full brightness
  }

  // Construct the ST7789 with CS/DC/RST pins
    tft = new Adafruit_ST7789(g_pins.cs, g_pins.dc, g_pins.rst);


  // Use hardware SPI (SCK/MOSI pins on the Feather)
  tft->init(TFT_W, TFT_H);      // 240x280
  tft->setRotation(0);          // adjust later if you want landscape

  tft->fillScreen(ST77XX_BLACK);
  tft->setTextWrap(false);

  g_ok = true;
  g_lastDrawMs = 0;

  return true;
}

void display_set_backlight(uint8_t duty_0_255) {
  if (!g_ok) return;
  if (g_pins.bl < 0) return; // BL tied to 3V
  analogWrite(g_pins.bl, duty_0_255);
}

static void clearField(const Field& f) {
  tft->fillRect(f.x, f.y, f.w, f.h, ST77XX_BLACK);
}

static void drawStaticLayout() {
  tft->fillScreen(ST77XX_BLACK);

  // Header bar
  tft->fillRect(0, 0, TFT_W, 34, ST77XX_BLUE);
  tft->setTextColor(ST77XX_WHITE);
  tft->setTextSize(2);
  tft->setCursor(8, 8);
  tft->print("IAQM");

  // Cards + titles (unchanging)
  
  // Layout: 2 columns x 3 rows of cards
  const int margin = 10;
  const int cardW = (TFT_W - 3 * margin) / 2;
  const int cardH = 70;

  int x1 = margin;
  int x2 = margin * 2 + cardW;
  int y1 = 44;
  int y2 = y1 + cardH + margin;
  int y3 = y2 + cardH + margin;

  // Card 1: Temp/RH (SHT45 ambient)
  drawCard(x1, y1, cardW, cardH, "Temp / RH (SHT45)");
//   {
//     String line1 = String(r.tC, 1) + " C";
//     String line2 = String(r.rh, 1) + " %";
//     printValue(x1 + 10, y1 + 24, line1);
//     tft->setTextSize(2);
//     tft->setTextColor(ST77XX_YELLOW);
//     tft->setCursor(x1 + 10, y1 + 48);
//     tft->print(line2);
//   }

  // Card 2: CO2
  drawCard(x2, y1, cardW, cardH, "CO2 (S88)");
//   {
//     String v = (r.co2_ppm > 0) ? (String(r.co2_ppm) + " ppm") : String("-");
//     printValue(x2 + 10, y1 + 32, v);
//   }

  // Card 3: PM2.5
  drawCard(x1, y2, cardW, cardH, "PM2.5 (SEN55)");
//   {
//     String v = String(r.pm2_5, 1) + " ug/m3";
//     printValue(x1 + 10, y2 + 32, v);
//   }

  // Card 4: PM10
  drawCard(x2, y2, cardW, cardH, "PM10 (SEN55)");
//   {
//     String v = String(r.pm10_0, 1) + " ug/m3";
//     printValue(x2 + 10, y2 + 32, v);
//   }

  // Card 5: VOC Index
  drawCard(x1, y3, cardW, cardH, "VOC Index");
//   {
//     String v = String(r.voc_index);
//     printValue(x1 + 10, y3 + 32, v);
//   }

  // Card 6: NOx Index
  drawCard(x2, y3, cardW, cardH, "NOx Index");
//   {
//     String v = String(r.nox_index);
//     printValue(x2 + 10, y3 + 32, v);
//   }

  // Small footer
  tft->setTextColor(ST77XX_WHITE);
  tft->setTextSize(1);
  tft->setCursor(8, TFT_H - 10);
  tft->print("Uptime: ");
//   tft->print(r.ms / 1000);
  tft->print("s");

  g_layoutDrawn = true;
}

void display_update(const Readings& r, bool wifiOk, bool sdOk, const char* hostName) {
  if (!g_ok || !tft) return;

  const uint32_t now = millis();
  if (now - g_lastDrawMs < 1000) return;
  g_lastDrawMs = now;

  if (!g_layoutDrawn) {
    drawStaticLayout();
  }

  // Update only the header “WiFi status” small region
  tft->fillRect(120, 0, 120, 34, wifiOk ? ST77XX_GREEN : ST77XX_RED);
  tft->setTextColor(ST77XX_BLACK);
  tft->setTextSize(1);
  tft->setCursor(128, 10);
  tft->print(wifiOk ? "WiFi OK " : "WiFi DOWN");
  tft->setCursor(128, 24);
  tft->print(sdOk ? "SD OK " : "SD FAIL");

  // Optional: hostName changes rarely; draw once if you want
  // (or clear just that area and redraw if needed)

  // ---- Update fields (clear small regions then redraw numbers) ----
  clearField(F_TEMP);
  tft->setTextColor(ST77XX_CYAN);
  tft->setTextSize(2);
  tft->setCursor(F_TEMP.x, F_TEMP.y);
  tft->print(String(r.tC, 1));
  tft->print(" C");

  clearField(F_RH);
  tft->setTextColor(ST77XX_YELLOW);
  tft->setCursor(F_RH.x, F_RH.y);
  tft->print(String(r.rh, 1));
  tft->print(" %");

  clearField(F_CO2);
  tft->setTextColor(ST77XX_CYAN);
  tft->setCursor(F_CO2.x, F_CO2.y);
  if (r.co2_ppm > 0) {
    tft->print(r.co2_ppm);
    tft->print(" ppm");
  } else {
    tft->print("-");
  }

  clearField(F_PM25);
  tft->setCursor(F_PM25.x, F_PM25.y);
  tft->print(String(r.pm2_5, 1));
  tft->print(" ug/m3");

  clearField(F_PM10);
  tft->setCursor(F_PM10.x, F_PM10.y);
  tft->print(String(r.pm10_0, 1));
  tft->print(" ug/m3");

  clearField(F_VOC);
  tft->setCursor(F_VOC.x, F_VOC.y);
  tft->print(r.voc_index);

  clearField(F_NOX);
  tft->setCursor(F_NOX.x, F_NOX.y);
  tft->print(r.nox_index);
}

