#pragma once
#include "../app_state.h"

namespace Indicators {
  void begin();
  void tick(const Readings& state, bool wifiOk, bool sdOk);
}
