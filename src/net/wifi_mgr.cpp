#include "wifi_mgr.h"
#include "../app_config.h"

#include <WiFi.h>

namespace WifiMgr {

void begin() {
  Serial.printf("\nConnecting to Wi-Fi SSID: %s\n", WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(false);
  WiFi.setSleep(false);

  if (WIFI_PASS[0] == '\0') {
    WiFi.begin(WIFI_SSID);
  } else {
    WiFi.begin(WIFI_SSID, WIFI_PASS);
  }

  const uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
    if (millis() - start > 15000) {
      Serial.println("\nWi-Fi connect timed out. Check SSID/password and signal.");
      return;
    }
  }

  Serial.println("\nWi-Fi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void tick() {
  static uint32_t lastWifiCheckMs = 0;
  if (millis() - lastWifiCheckMs < WIFI_CHECK_PERIOD_MS) return;
  lastWifiCheckMs = millis();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi down; attempting reconnect...");
    WiFi.disconnect();
    WiFi.reconnect();
  }
}

bool isConnected() {
  return WiFi.status() == WL_CONNECTED;
}

} // namespace WifiMgr
