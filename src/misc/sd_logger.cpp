#include "sd_logger.h"

#include <SPI.h>
#include <SD.h>

SPIClass sdSPI(FSPI); // Use FSPI for SD

static bool sdBeginWithFallbacks(uint8_t csPin, uint32_t preferredFreqHz) {
  const uint32_t freqs[] = { preferredFreqHz, 1000000UL, 400000UL };

  for (uint32_t freq : freqs) {
    Serial.printf("[SD] SD card initialization attempt at %lu Hz...\n", (unsigned long)freq);
    if (SD.begin(csPin, sdSPI, freq)) {
      Serial.printf("[SD] SD card initialization OK at %lu Hz.\n", (unsigned long)freq);
      return true;
    }
    Serial.printf("[SD] SD card initialization failed at %lu Hz.\n", (unsigned long)freq);
    delay(50);
  }

  return false;
}

bool SdLogger::begin(uint8_t csPin, int sckPin, int misoPin, int mosiPin, uint32_t spiFreqHz) {
  // Set the internal state based on the provided SPI pins and frequency
  _csPin = csPin;
  _sckPin = sckPin;
  _misoPin = misoPin;
  _mosiPin = mosiPin;
  _spiFreqHz = spiFreqHz;

  pinMode(_csPin, OUTPUT);
  digitalWrite(_csPin, HIGH);

  // Start SPI bus for SD. If your display already calls SPI.begin(), this is still safe.
  sdSPI.begin(_sckPin, _misoPin, _mosiPin, _csPin);
  delay(50);

  // Try init SD
  if (!sdBeginWithFallbacks(_csPin, _spiFreqHz)) {
    _ready = false;
    return false;
  }

  uint8_t cardType = SD.cardType();
  Serial.print("[SD] Card type: ");
  Serial.print(cardType);
  switch (cardType) {
    case CARD_NONE:
      Serial.println(" (No SD card attached)");
      break;
    case CARD_MMC:
      Serial.println(" (MMC)");
      break;
    case CARD_SD:
      Serial.println(" (SDSC)");
      break;
    case CARD_SDHC:
      Serial.println(" (SDHC)");
      break;
    default:
      Serial.println(" (Unknown)");
  }

  uint64_t cardSize = SD.cardSize() / (1024ULL * 1024ULL);
  Serial.print("[SD] Card size: ");
  Serial.print((uint32_t)cardSize);
  Serial.println(" MB");

  _ready = true;
  _headerWritten = false;
  if (!ensureFile()) {
    Serial.println("[SD] Failed to ensure log file.");
    return false;
  }
  return true;
}

bool SdLogger::ensureFile() {
  if (!_ready) return false;

  // Check if file exists, if not create it. This ensures the SD card is still present and writable before each append.
  if (!SD.exists(_filePath.c_str())) {
    Serial.println("[SD] Log file does not exist. Attempting to create it...");
    File f = SD.open(_filePath.c_str(), FILE_WRITE);
    if (!f) {
      Serial.println("[SD] Failed to create log file.");
      return false;
    }
    f.close();
  }

  writeHeaderIfNeeded();
  return true;
}

void SdLogger::writeHeaderIfNeeded() {
  if (_headerWritten || !_ready) return;

  File f = SD.open(_filePath.c_str(), FILE_READ);
  if (!f) {
    Serial.println("[SD] Failed to open log file for reading.");
    return;
  }

  bool empty = (f.size() == 0);
  f.close();

  if (!empty) {
    _headerWritten = true;
    return;
  }

  File fw = SD.open(_filePath.c_str(), FILE_WRITE);
  if (!fw) {
    Serial.println("[SD] Failed to open log file for writing.");
    return;
  }

  fw.println(
    "timestamp,ts_ms,wifi_ok,rtc_ok,veml_ok,sen55_ok,s88_ok,sd_ok,temp_c,rh_pct,lux,white,als,co2_ppm,pm1_0,pm2_5,pm4_0,pm10,voc_index,nox_index"
  );
  fw.flush();
  fw.close();

  _headerWritten = true;
}

bool SdLogger::appendSample(const Readings& r, bool wifiOk) {
  if (!_ready) return false;

  if (!ensureFile()){
      Serial.println("[SD] Failed to ensure log file before appending.");
      return false;
  } 

  File f = SD.open(_filePath.c_str(), FILE_APPEND);
  if (!f){
    Serial.println("[SD] Failed to open log file for appending.");
    return false;
  } 

  f.print(r.timestamp); f.print(',');
  f.print(r.ms); f.print(',');
  f.print(wifiOk ? 1 : 0); f.print(',');
  f.print(r.rtc_ok ? 1 : 0); f.print(',');
  f.print(r.veml_ok ? 1 : 0); f.print(',');
  f.print(r.sen55_ok ? 1 : 0); f.print(',');
  f.print(r.s88_ok ? 1 : 0); f.print(',');
  f.print(_ready ? 1 : 0); f.print(',');

  f.print(r.tC, 2); f.print(',');
  f.print(r.rh, 2); f.print(',');

  f.print(r.lux, 2); f.print(',');
  f.print(r.white, 2); f.print(',');
  f.print(r.als); f.print(',');

  f.print(r.co2_ppm); f.print(',');

  f.print(r.pm1_0, 1); f.print(',');
  f.print(r.pm2_5, 1); f.print(',');
  f.print(r.pm4_0, 1); f.print(',');
  f.print(r.pm10_0, 1);  f.print(',');
  f.print(r.voc_index, 1); f.print(',');
  f.print(r.nox_index, 1);

  f.println();
  f.flush();
  f.close();
  return true;
}

void SdLogger::flush() {
  // Each append flushes, so this is a no-op; left for API completeness.
}
