#include "RepDisplay.h"

#include <Fonts/FreeSansBold12pt7b.h>
#include <GxEPD2_BW.h>
#include <SPI.h>
#include <U8g2_for_Adafruit_GFX.h>

#include "config.h"

// Portrait layout, 122 x 250 — two stacked sections, everything centered:
//
//    (o) REPS
//       12
//   -----------
//    (/) TIME
//      1:24

static GxEPD2_BW<GxEPD2_213_B74, GxEPD2_213_B74::HEIGHT> epd(
    GxEPD2_213_B74(cfg::PIN_EPD_CS, cfg::PIN_EPD_DC, cfg::PIN_EPD_RST, cfg::PIN_EPD_BUSY));

// Adafruit GFX fonts top out at 24pt; U8g2's Logisoso goes much bigger.
static U8G2_FOR_ADAFRUIT_GFX bigFont;

namespace {

constexpr int16_t kSectionHeight = 125;  // REPS on top, TIME below
constexpr int16_t kIconSize = 16;
constexpr int16_t kIconGap = 6;          // between icon and label text
constexpr int16_t kLabelBaseline = 26;   // relative to section top
constexpr int16_t kValueTop = 34;        // values are centered between here...
constexpr int16_t kValueBottom = 118;    // ...and here (relative to section top)
constexpr int16_t kSidePad = 6;

// Largest first; the biggest one where both values fit is used for both.
// "_tn" = digits and ':' only.
const uint8_t* const kValueFonts[] = {
    u8g2_font_logisoso58_tn, u8g2_font_logisoso50_tn, u8g2_font_logisoso46_tn,
    u8g2_font_logisoso42_tn, u8g2_font_logisoso38_tn, u8g2_font_logisoso32_tn,
};

void formatTime(char* buf, size_t n, uint32_t sec) {
  uint32_t h = sec / 3600, m = (sec / 60) % 60, s = sec % 60;
  if (h > 0) {
    snprintf(buf, n, "%lu:%02lu:%02lu", (unsigned long)h, (unsigned long)m, (unsigned long)s);
  } else {
    snprintf(buf, n, "%lu:%02lu", (unsigned long)m, (unsigned long)s);
  }
}

// 16x16 icons, (x, y) = top-left.
void drawPersonIcon(int16_t x, int16_t y) {
  epd.fillCircle(x + 8, y + 4, 4, GxEPD_BLACK);
  epd.fillRoundRect(x + 1, y + 10, 14, 6, 3, GxEPD_BLACK);
}

void drawClockIcon(int16_t x, int16_t y) {
  int16_t cx = x + 8, cy = y + 8;
  epd.drawCircle(cx, cy, 8, GxEPD_BLACK);
  epd.drawCircle(cx, cy, 7, GxEPD_BLACK);
  epd.drawLine(cx, cy, cx, cy - 5, GxEPD_BLACK);
  epd.drawLine(cx, cy, cx + 4, cy, GxEPD_BLACK);
}

enum class Icon { Person, Clock };

// Icon + label, centered as one unit across the screen.
void drawLabel(int16_t sectionTop, Icon icon, const char* text) {
  epd.setFont(&FreeSansBold12pt7b);
  int16_t x1, y1;
  uint16_t w, h;
  epd.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

  int16_t total = kIconSize + kIconGap + w;
  int16_t left = (epd.width() - total) / 2;
  int16_t baseline = sectionTop + kLabelBaseline;
  int16_t iconTop = baseline - kIconSize - 1;  // sit the icon on the text baseline

  if (icon == Icon::Person) {
    drawPersonIcon(left, iconTop);
  } else {
    drawClockIcon(left, iconTop);
  }
  epd.setCursor(left + kIconSize + kIconGap - x1, baseline);
  epd.print(text);
}

void drawValue(int16_t sectionTop, const char* text) {
  int16_t w = bigFont.getUTF8Width(text);
  int16_t baseline = sectionTop + (kValueTop + kValueBottom) / 2 + bigFont.getFontAscent() / 2;
  bigFont.setCursor((epd.width() - w) / 2, baseline);
  bigFont.print(text);
}

}  // namespace

void RepDisplay::begin() {
  pinMode(cfg::PIN_EPD_PWR, OUTPUT);
  digitalWrite(cfg::PIN_EPD_PWR, HIGH);
  delay(10);

  // Explicit pins: the ESP32-S3 SPI defaults don't match this board.
  SPI.begin(cfg::PIN_EPD_SCK, -1, cfg::PIN_EPD_MOSI, -1);
  epd.init(0);
  epd.setRotation(cfg::DISPLAY_ROTATION);
  epd.setTextColor(GxEPD_BLACK);
  epd.setTextWrap(false);

  bigFont.begin(epd);
  bigFont.setFontMode(1);  // transparent background
  bigFont.setForegroundColor(GxEPD_BLACK);
  bigFont.setBackgroundColor(GxEPD_WHITE);
}

void RepDisplay::show(const Screen& s, bool full) {
  if (full) {
    epd.setFullWindow();
  } else {
    epd.setPartialWindow(0, 0, epd.width(), epd.height());
  }
  epd.firstPage();
  do {
    draw(s);
  } while (epd.nextPage());
}

void RepDisplay::draw(const Screen& s) {
  char reps[8];
  char time[12];
  snprintf(reps, sizeof(reps), "%u", s.reps);
  formatTime(time, sizeof(time), s.timerSec);

  epd.fillScreen(GxEPD_WHITE);
  epd.drawFastHLine(8, kSectionHeight - 1, epd.width() - 16, GxEPD_BLACK);
  epd.drawFastHLine(8, kSectionHeight, epd.width() - 16, GxEPD_BLACK);

  drawLabel(0, Icon::Person, "REPS");
  drawLabel(kSectionHeight, Icon::Clock, s.phase == Phase::Resting ? "REST" : "TIME");

  // Same font size for both values: the biggest one where both fit.
  const int16_t maxWidth = epd.width() - 2 * kSidePad;
  for (const uint8_t* font : kValueFonts) {
    bigFont.setFont(font);
    if (bigFont.getUTF8Width(reps) <= maxWidth && bigFont.getUTF8Width(time) <= maxWidth) break;
  }
  drawValue(0, reps);
  drawValue(kSectionHeight, time);

  if (s.simulated) {
    // Small marker so fake reps are never mistaken for real ones.
    // (Built-in font: the cursor y is the top of the text, not a baseline.)
    epd.setFont(nullptr);
    epd.setCursor((epd.width() - 18) / 2, epd.height() - 9);
    epd.print("SIM");
  }
}
