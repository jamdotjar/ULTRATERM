#include <Arduino.h>
#include <SPI.h>
#include <FS.h>
#include <SD.h>
#include <TFT_eSPI.h>

static TFT_eSPI tft;
static const int PIN_BTN1 = 15, PIN_BTN2 = 17;
static const int SD_CS = 26; // adjust if different
static const char* IMG1 = "/img1.bmp";
static const char* IMG2 = "/img2.bmp";
static bool show_alt = false;

static uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}
static uint16_t read16(const uint8_t* p) { return p[0] | (p[1] << 8); }
static uint32_t read32(const uint8_t* p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }

static bool drawBMP(const char* path) {
  File f = SD.open(path, FILE_READ);
  if (!f) { Serial.printf("Failed to open %s\n", path); return false; }

  uint8_t header[54];
  if (f.read(header, 54) != 54 || header[0] != 'B' || header[1] != 'M') {
    Serial.println("Not a BMP"); f.close(); return false;
  }

  uint32_t dataOffset = read32(&header[10]);
  int32_t w = (int32_t)read32(&header[18]);
  int32_t h = (int32_t)read32(&header[22]);
  uint16_t bpp = read16(&header[28]);
  uint32_t compression = read32(&header[30]);

  if (!(bpp == 24 || bpp == 16) || compression != 0 || w <= 0 || h == 0) {
    Serial.println("Unsupported BMP format"); f.close(); return false;
  }

  bool flip = h > 0;
  uint32_t rowSize = ((bpp * (uint32_t)w + 31) / 32) * 4;
  uint32_t absH = (uint32_t)(flip ? h : -h);

  // Prepare buffers
  std::unique_ptr<uint8_t[]> row(new uint8_t[rowSize]);
  std::unique_ptr<uint16_t[]> line(new uint16_t[w]);

  tft.startWrite();
  tft.setSwapBytes(true); // send RGB565 MSB-first

  for (uint32_t rowIdx = 0; rowIdx < absH; ++rowIdx) {
    uint32_t fileRow = flip ? (absH - 1 - rowIdx) : rowIdx;
    uint32_t pos = dataOffset + fileRow * rowSize;
    f.seek(pos);
    if (f.read(row.get(), rowSize) != (int)rowSize) break;

    if (bpp == 24) {
      for (int x = 0; x < w; ++x) {
        uint8_t b = row[x * 3 + 0];
        uint8_t g = row[x * 3 + 1];
        uint8_t r = row[x * 3 + 2];
        line[x] = rgb565(r, g, b);
      }
    } else { // 16-bit RGB565
      for (int x = 0; x < w; ++x) {
        uint8_t lo = row[x * 2 + 0];
        uint8_t hi = row[x * 2 + 1];
        line[x] = (uint16_t)lo | ((uint16_t)hi << 8);
      }
    }

    if (rowIdx < (uint32_t)tft.height()) {
      tft.pushImage(0, rowIdx, min(w, (int)tft.width()), 1, line.get());
    }
  }

  tft.endWrite();
  f.close();
  return true;
}

static void draw_current() {
  const char* path = show_alt ? IMG2 : IMG1;
  Serial.printf("Drawing %s\n", path);
  drawBMP(path);
}

void setup() {
  Serial.begin(115200);

  pinMode(PIN_BTN1, INPUT_PULLUP);
  pinMode(PIN_BTN2, INPUT_PULLUP);

  SPI.begin(18, 19, 22, SD_CS);
  if (!SD.begin(SD_CS)) {
    Serial.println("SD init failed");
  }

  tft.init();
  tft.setRotation(1); // adjust to match your panel
  tft.fillScreen(TFT_BLACK);

  draw_current();
}

void loop() {
  static bool wasPressed = false;
  bool pressed = digitalRead(PIN_BTN1) == LOW || digitalRead(PIN_BTN2) == LOW;

  if (pressed && !wasPressed) {
    show_alt = !show_alt;
    draw_current();
  }
  wasPressed = pressed;
  delay(10);
}