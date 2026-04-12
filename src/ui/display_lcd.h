#pragma once
#include <Arduino.h>
#include "../app_state.h"

// Configure your chosen GPIOs here (Arduino pin numbers: D5, D6, D9, etc.)
struct LcdPins {
  int8_t cs;
  int8_t dc;
  int8_t rst;
  int8_t bl;   // optional; set to -1 if tied to 3V
};

bool display_init(const LcdPins& pins);
void display_set_backlight(uint8_t duty_0_255);   // safe no-op if BL not wired
void display_update(const Readings& r, bool wifiOk, bool sdOk, const char* hostName);
