#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

enum AnimationMode { RAINBOW, COMET, SPARKLE, SQUARE, RECTANGLE, CIRCLE };


// Forward declarations
uint16_t xy(uint8_t x, uint8_t y);
void setXY(int8_t x, int8_t y, uint32_t color);
void drawRectangle(int8_t left, int8_t top, int8_t right, int8_t bottom, uint32_t color);
void showRainbow();
void showComet();
void showSparkle();
void showSquare();
void showRectangle();
void showCircle();
void nextMode();

constexpr uint8_t MATRIX_PIN = 18;
constexpr uint8_t WIDTH = 8;
constexpr uint8_t HEIGHT = 8;
constexpr uint16_t PIXEL_COUNT = WIDTH * HEIGHT;
constexpr uint8_t BRIGHTNESS = 85;
constexpr uint32_t FRAME_INTERVAL_MS = 50;
constexpr uint32_t MODE_DURATION_MS = 10000;

Adafruit_NeoPixel matrix(PIXEL_COUNT, MATRIX_PIN, NEO_GRB + NEO_KHZ800);
AnimationMode mode = RAINBOW;
uint32_t lastFrameMs = 0;
uint32_t modeStartedMs = 0;
uint16_t frameNumber = 0;

uint16_t xy(uint8_t x, uint8_t y) {
  return (y % 2 == 0) ? (y * WIDTH + x) : (y * WIDTH + (WIDTH - 1 - x));
}

void setXY(int8_t x, int8_t y, uint32_t color) {
  if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) matrix.setPixelColor(xy(x, y), color);
}

void drawRectangle(int8_t left, int8_t top, int8_t right, int8_t bottom, uint32_t color) {
  for (int8_t x = left; x <= right; ++x) {
    setXY(x, top, color);
    setXY(x, bottom, color);
  }
  for (int8_t y = top + 1; y < bottom; ++y) {
    setXY(left, y, color);
    setXY(right, y, color);
  }
}

void showRainbow() {
  for (uint16_t i = 0; i < PIXEL_COUNT; ++i) {
    uint16_t hue = (i * 65536UL / PIXEL_COUNT) + frameNumber * 700UL;
    matrix.setPixelColor(i, matrix.gamma32(matrix.ColorHSV(hue)));
  }
}

void showComet() {
  matrix.clear();
  uint8_t head = frameNumber % PIXEL_COUNT;
  for (uint8_t tail = 0; tail < 12; ++tail) {
    int16_t pixel = head - tail;
    if (pixel < 0) pixel += PIXEL_COUNT;
    uint8_t intensity = 255 - tail * 20;
    matrix.setPixelColor(pixel, matrix.Color(intensity, intensity / 6, 0));
  }
}

void showSparkle() {
  for (uint8_t y = 0; y < HEIGHT; ++y) {
    for (uint8_t x = 0; x < WIDTH; ++x) {
      uint8_t phase = (frameNumber + x * 19 + y * 31) % 64;
      uint8_t value = (phase < 12) ? (255 - phase * 14) : 0;
      matrix.setPixelColor(xy(x, y), matrix.Color(0, value / 4, value));
    }
  }
}

void showSquare() {
  matrix.clear();
  uint8_t inset = (frameNumber / 10) % 4;
  uint16_t hue = frameNumber * 900UL;
  drawRectangle(inset, inset, 7 - inset, 7 - inset, matrix.gamma32(matrix.ColorHSV(hue)));
}

void showRectangle() {
  matrix.clear();
  uint8_t phase = (frameNumber / 8) % 6;
  int8_t top = (phase < 3) ? phase : 5 - phase;
  int8_t bottom = 7 - top;
  uint16_t hue = 21845UL + frameNumber * 700UL;
  drawRectangle(0, top, 7, bottom, matrix.gamma32(matrix.ColorHSV(hue)));
}

void showCircle() {
  matrix.clear();
  uint8_t phase = (frameNumber / 10) % 4;
  uint32_t color = matrix.gamma32(matrix.ColorHSV(43690UL + frameNumber * 800UL));
  if (phase == 0) {
    setXY(3, 3, color); setXY(4, 3, color); setXY(3, 4, color); setXY(4, 4, color);
  } else if (phase == 1) {
    const int8_t points[][2] = {{3,2},{4,2},{2,3},{5,3},{2,4},{5,4},{3,5},{4,5}};
    for (const auto &p : points) setXY(p[0], p[1], color);
  } else if (phase == 2) {
    const int8_t points[][2] = {{2,1},{3,1},{4,1},{5,1},{1,2},{6,2},{1,3},{6,3},{1,4},{6,4},{1,5},{6,5},{2,6},{3,6},{4,6},{5,6}};
    for (const auto &p : points) setXY(p[0], p[1], color);
  } else {
    const int8_t points[][2] = {{2,0},{3,0},{4,0},{5,0},{1,1},{6,1},{0,2},{7,2},{0,3},{7,3},{0,4},{7,4},{0,5},{7,5},{1,6},{6,6},{2,7},{3,7},{4,7},{5,7}};
    for (const auto &p : points) setXY(p[0], p[1], color);
  }
}

void nextMode() {
  mode = static_cast<AnimationMode>((mode + 1) % 6);
  modeStartedMs = millis();
  frameNumber = 0;
  matrix.clear();
}

void setup() {
  matrix.begin();
  matrix.setBrightness(BRIGHTNESS);
  matrix.clear();
  matrix.show();
  modeStartedMs = millis();
}

void loop() {
  uint32_t now = millis();
  if (now - modeStartedMs >= MODE_DURATION_MS) nextMode();
  if (now - lastFrameMs < FRAME_INTERVAL_MS) return;
  lastFrameMs = now;

  switch (mode) {
    case RAINBOW: showRainbow(); break;
    case COMET: showComet(); break;
    case SPARKLE: showSparkle(); break;
    case SQUARE: showSquare(); break;
    case RECTANGLE: showRectangle(); break;
    case CIRCLE: showCircle(); break;
  }
  matrix.show();
  ++frameNumber;
}