#pragma once

// ==============================================================================
// 🧭 Pocket Assistant Reusable C++ UI, Geometry & Touch Toolkit (ui_toolkit.h)
// Target: 466x466 Circular AMOLED Display (Waveshare ESP32-S3-Touch-AMOLED-1.75C)
// Zero-exception (-fno-exceptions), zero-heap-churn embedded graphics & hit-testing
// ==============================================================================

#include "esphome.h"
#include <cmath>
#include <ctime>
#include <cstdio>
#include <cstring>
#include <initializer_list>
#include <string>

namespace pocket_ui {

// ------------------------------------------------------------------------------
// 1. Mathematical, Trigonometric & Circular Clipping Lookup Tables (.rodata Flash)
// ------------------------------------------------------------------------------
static constexpr float PI_F = 3.14159265358979323846f;

// Universal Precomputed 60-Tick Sine/Cosine LUTs (Clock, Stopwatch, Dial Gauges)
static constexpr float DIAL_SIN[60] = {
  0.000000f, 0.104528f, 0.207912f, 0.309017f, 0.406737f, 0.500000f,
  0.587785f, 0.669131f, 0.743145f, 0.809017f, 0.866025f, 0.913545f,
  0.951057f, 0.978148f, 0.994522f, 1.000000f, 0.994522f, 0.978148f,
  0.951057f, 0.913545f, 0.866025f, 0.809017f, 0.743145f, 0.669131f,
  0.587785f, 0.500000f, 0.406737f, 0.309017f, 0.207912f, 0.104528f,
  0.000000f, -0.104528f, -0.207912f, -0.309017f, -0.406737f, -0.500000f,
  -0.587785f, -0.669131f, -0.743145f, -0.809017f, -0.866025f, -0.913545f,
  -0.951057f, -0.978148f, -0.994522f, -1.000000f, -0.994522f, -0.978148f,
  -0.951057f, -0.913545f, -0.866025f, -0.809017f, -0.743145f, -0.669131f,
  -0.587785f, -0.500000f, -0.406737f, -0.309017f, -0.207912f, -0.104528f
};

static constexpr float DIAL_COS[60] = {
  1.000000f, 0.994522f, 0.978148f, 0.951057f, 0.913545f, 0.866025f,
  0.809017f, 0.743145f, 0.669131f, 0.587785f, 0.500000f, 0.406737f,
  0.309017f, 0.207912f, 0.104528f, 0.000000f, -0.104528f, -0.207912f,
  -0.309017f, -0.406737f, -0.500000f, -0.587785f, -0.669131f, -0.743145f,
  -0.809017f, -0.866025f, -0.913545f, -0.951057f, -0.978148f, -0.994522f,
  -1.000000f, -0.994522f, -0.978148f, -0.951057f, -0.913545f, -0.866025f,
  -0.809017f, -0.743145f, -0.669131f, -0.587785f, -0.500000f, -0.406737f,
  -0.309017f, -0.207912f, -0.104528f, 0.000000f, 0.104528f, 0.207912f,
  0.309017f, 0.406737f, 0.500000f, 0.587785f, 0.669131f, 0.743145f,
  0.809017f, 0.866025f, 0.913545f, 0.951057f, 0.978148f, 0.994522f
};

// Precomputed 466-element circular half-width table in Flash (.rodata, saves 932 B SRAM)
static constexpr int16_t CIRCLE_HW[466] = {
    0,  21,  30,  37,  42,  48,  52,  56,  60,  64,  67,  70,
   73,  76,  79,  82,  84,  87,  89,  92,  94,  96,  98, 100,
  102, 105, 106, 108, 110, 112, 114, 116, 117, 119, 121, 122,
  124, 125, 127, 129, 130, 132, 133, 134, 136, 137, 138, 140,
  141, 142, 144, 145, 146, 147, 149, 150, 151, 152, 153, 154,
  156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167,
  168, 169, 170, 171, 172, 173, 173, 174, 175, 176, 177, 178,
  179, 179, 180, 181, 182, 183, 183, 184, 185, 186, 186, 187,
  188, 189, 189, 190, 191, 192, 192, 193, 194, 194, 195, 195,
  196, 197, 197, 198, 199, 199, 200, 200, 201, 202, 202, 203,
  203, 204, 204, 205, 205, 206, 206, 207, 208, 208, 208, 209,
  209, 210, 210, 211, 211, 212, 212, 213, 213, 214, 214, 214,
  215, 215, 216, 216, 216, 217, 217, 218, 218, 218, 219, 219,
  219, 220, 220, 220, 221, 221, 221, 222, 222, 222, 223, 223,
  223, 224, 224, 224, 224, 225, 225, 225, 225, 226, 226, 226,
  226, 227, 227, 227, 227, 228, 228, 228, 228, 228, 228, 229,
  229, 229, 229, 229, 230, 230, 230, 230, 230, 230, 230, 231,
  231, 231, 231, 231, 231, 231, 231, 231, 232, 232, 232, 232,
  232, 232, 232, 232, 232, 232, 232, 232, 232, 232, 232, 232,
  232, 232, 232, 232, 232, 233, 232, 232, 232, 232, 232, 232,
  232, 232, 232, 232, 232, 232, 232, 232, 232, 232, 232, 232,
  232, 232, 232, 231, 231, 231, 231, 231, 231, 231, 231, 231,
  230, 230, 230, 230, 230, 230, 230, 229, 229, 229, 229, 229,
  228, 228, 228, 228, 228, 228, 227, 227, 227, 227, 226, 226,
  226, 226, 225, 225, 225, 225, 224, 224, 224, 224, 223, 223,
  223, 222, 222, 222, 221, 221, 221, 220, 220, 220, 219, 219,
  219, 218, 218, 218, 217, 217, 216, 216, 216, 215, 215, 214,
  214, 214, 213, 213, 212, 212, 211, 211, 210, 210, 209, 209,
  208, 208, 208, 207, 206, 206, 205, 205, 204, 204, 203, 203,
  202, 202, 201, 200, 200, 199, 199, 198, 197, 197, 196, 195,
  195, 194, 194, 193, 192, 192, 191, 190, 189, 189, 188, 187,
  186, 186, 185, 184, 183, 183, 182, 181, 180, 179, 179, 178,
  177, 176, 175, 174, 173, 173, 172, 171, 170, 169, 168, 167,
  166, 165, 164, 163, 162, 161, 160, 159, 158, 157, 156, 154,
  153, 152, 151, 150, 149, 147, 146, 145, 144, 142, 141, 140,
  138, 137, 136, 134, 133, 132, 130, 129, 127, 125, 124, 122,
  121, 119, 117, 116, 114, 112, 110, 108, 106, 105, 102, 100,
   98,  96,  94,  92,  89,  87,  84,  82,  79,  76,  73,  70,
   67,  64,  60,  56,  52,  48,  42,  37,  30,  21
};

// Standardized Dynamic Scroll Easing Constants (5-Step Cubic Eased Curve)
static constexpr int16_t SHARED_SCROLL_EASE_116[5] = {0, 25, 63, 99, 116}; // 2-Pill 116px shift
static constexpr int16_t SHARED_SCROLL_EASE_174[5] = {0, 38, 95, 148, 174}; // 3-Pill 174px shift
static constexpr int16_t SHARED_SCROLL_EASE_232[5] = {0, 50, 126, 198, 232}; // 4-Pill 232px shift

// Matrix Cyber Rain Screensaver Pre-Scattered Tables (31 streams across 19 columns)
static constexpr int16_t MATRIX_X[31] = {
   17,  41,  65,  89,  89, 113, 113, 137, 137, 161,
  161, 185, 185, 209, 209, 233, 233, 257, 257, 281,
  281, 305, 305, 329, 329, 353, 353, 377, 401, 425,
  449
};
static constexpr int16_t MATRIX_Y[31] = {
  340, 610, 150, 420,  20, 260, 680,  90, 510, 370,
  790, 180, 590,  40, 460, 290, 710, 120, 540, 390,
  810, 210, 630,  70, 480, 320, 740, 160, 580, 240,
  470
};
static constexpr uint8_t MATRIX_SPD[31] = {
  132, 129, 124, 137, 108, 109, 126, 108, 123, 111,
  105, 123, 118, 125, 110, 120, 108, 122, 114, 102,
  134, 116, 112, 119, 122, 120, 137,  95, 108, 101,
   96
};
static constexpr uint8_t MATRIX_LEN[31] = {
    9,  10,   8,  12,  12,  10,  13,  10,   8,   9,
    8,  12,   9,  10,  13,  11,   8,  11,   9,  13,
   13,  13,  11,   9,   8,  13,  11,  11,  12,  10,
   10
};

// Weather Motion Wallpaper Particle Tables (Starfield, Rain, Snow)
static constexpr int16_t WEATHER_STARS_X[64] = {
  66, 113, 160, 207, 254, 301, 395,  89, 136, 183,
  230, 277, 324, 371, 418,  69, 116, 351, 398,  92,
  139, 186, 233, 280, 327, 374,  72, 119, 166, 213,
  260, 307, 354, 401,  95, 142, 189, 236, 283, 330,
  377,  75, 122, 169, 216, 263, 310, 357, 404,  98,
  145, 192, 239, 286, 333, 380,  78, 125, 172, 219,
  266, 313, 360, 407
};
static constexpr int16_t WEATHER_STARS_Y[64] = {
   60,  97, 134, 171, 208, 245, 319, 393,  73, 110,
  147, 184, 221, 258, 295, 369, 406,  86, 123, 160,
  197, 234, 271, 308, 345, 382,  62,  99, 136, 173,
  210, 247, 284, 321, 395,  75, 112, 149, 186, 223,
  260, 334, 371, 408,  88, 125, 162, 199, 236, 273,
  310, 347, 384,  64, 101, 138, 212, 249, 286, 323,
  360, 397,  77, 114
};
static constexpr int16_t RAIN_X[40] = {
  247, 229, 298, 115, 48, 127, 402, 66, 198, 379, 199, 211, 260, 120, 281, 407, 276, 88, 313, 221,
  230, 308, 39, 254, 394, 146, 118, 38, 283, 226, 307, 48, 273, 200, 87, 350, 161, 377, 385, 328
};
static constexpr int16_t RAIN_Y[40] = {
  82, 179, 267, 94, 79, 187, 136, 155, 101, 197, 392, 322, 55, 327, 157, 208, 132, 332, 230, 380,
  58, 125, 395, 320, 433, 231, 425, 273, 425, 352, 294, 108, 81, 204, 119, 23, 370, 110, 51, 206
};
static constexpr uint8_t RAIN_SPD[40] = {
  143, 175, 143, 200, 204, 155, 169, 183, 206, 190, 179, 202, 201, 161, 150, 195, 203, 197, 168, 167,
  179, 152, 143, 166, 194, 185, 194, 188, 168, 177, 159, 146, 166, 158, 198, 176, 185, 157, 159, 152
};
static constexpr uint8_t RAIN_LEN[40] = {
  16, 12, 17, 11, 12, 12, 17, 10, 15, 16, 12, 14, 14, 14, 11, 17, 13, 16, 13, 18,
  10, 16, 18, 14, 17, 13, 18, 11, 17, 11, 10, 18, 13, 15, 17, 14, 11, 12, 11, 10
};
static constexpr int16_t SNOW_X[36] = {
  361, 419, 370, 245, 100, 303, 251,  54, 132, 354, 325, 340, 362, 145, 336, 215,  68, 154,
  139, 405, 312, 225, 176, 132, 321, 223, 265, 369, 225, 412, 358, 409, 258, 367, 143, 206
};
static constexpr int16_t SNOW_Y[36] = {
  288,  99, 419, 371, 232, 372, 425, 195, 422, 249,  49, 411,  42,  68, 175, 236, 412, 328,
  411, 392, 264, 391, 149, 176, 144, 325, 162, 251, 351, 124, 234, 185,  96, 179, 343, 414
};
static constexpr uint8_t SNOW_SPD[36] = {
   50,  52,  39,  34,  54,  49,  51,  47,  55,  47,  48,  29,  49,  37,  28,  46,  45,  33,
   49,  46,  45,  45,  30,  54,  29,  50,  51,  39,  49,  34,  45,  40,  50,  29,  41,  40
};
static constexpr float SNOW_PHASE[36] = {
  2.30f, 5.18f, 5.79f, 4.78f, 3.99f, 2.19f, 5.02f, 0.74f, 1.97f, 1.66f, 3.30f, 0.45f, 0.11f, 1.23f, 5.00f, 4.96f, 4.56f, 3.32f,
  0.32f, 2.86f, 4.72f, 5.01f, 4.45f, 0.49f, 3.48f, 5.53f, 6.10f, 2.39f, 4.04f, 0.85f, 1.59f, 3.92f, 2.95f, 0.19f, 1.93f, 5.49f
};
static constexpr uint8_t SNOW_TYPE[36] = {
   1, 1, 1, 1, 0, 1, 2, 0, 2, 0, 1, 2, 0, 2, 1, 1, 1, 1,
   1, 0, 2, 2, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 2, 1, 1, 0
};

// ------------------------------------------------------------------------------
// 2. Reusable Geometric & Vector UI Drawing Primitives
// ------------------------------------------------------------------------------
template<typename DisplayT, typename FontT>
inline void render_pill_button(DisplayT &it, int x1, int x2, int y_mid, int r,
                               esphome::Color border_c, esphome::Color bg_c,
                               const char *label, esphome::Color text_c, FontT *font) {
  it.filled_circle(x1, y_mid, r, border_c);
  it.filled_circle(x2, y_mid, r, border_c);
  it.filled_rectangle(x1, y_mid - r, x2 - x1, 2 * r + 1, border_c);

  it.filled_circle(x1, y_mid, r - 2, bg_c);
  it.filled_circle(x2, y_mid, r - 2, bg_c);
  it.filled_rectangle(x1, y_mid - (r - 2), x2 - x1, 2 * (r - 2) + 1, bg_c);

  if (label != nullptr && label[0] != '\0' && font != nullptr) {
    int btn_cx = (x1 + x2) / 2;
    it.print(btn_cx, y_mid, font, text_c, esphome::display::TextAlign::CENTER, label);
  }
}

template<typename DisplayT>
inline void render_scroll_chevron(DisplayT &it, int x, int y, bool is_up,
                                  esphome::Color c, esphome::Color shadow_c, bool shadow = false) {
  if (shadow) {
    if (is_up) {
      it.line(x - 7, y + 5, x, y - 4, shadow_c);
      it.line(x - 7, y + 6, x, y - 3, shadow_c);
      it.line(x + 7, y + 5, x, y - 4, shadow_c);
      it.line(x + 7, y + 6, x, y - 3, shadow_c);
    } else {
      it.line(x - 7, y - 3, x, y + 6, shadow_c);
      it.line(x - 7, y - 2, x, y + 7, shadow_c);
      it.line(x + 7, y - 3, x, y + 6, shadow_c);
      it.line(x + 7, y - 2, x, y + 7, shadow_c);
    }
  }
  if (is_up) {
    it.line(x - 7, y + 4, x, y - 5, c);
    it.line(x - 7, y + 5, x, y - 4, c);
    it.line(x + 7, y + 4, x, y - 5, c);
    it.line(x + 7, y + 5, x, y - 4, c);
  } else {
    it.line(x - 7, y - 4, x, y + 5, c);
    it.line(x - 7, y - 5, x, y + 4, c);
    it.line(x + 7, y - 4, x, y + 5, c);
    it.line(x + 7, y - 5, x, y + 4, c);
  }
}

template<typename DisplayT>
inline void render_deck_chevrons(DisplayT &it, esphome::Color chevron_col,
                                 esphome::Color shadow_c, bool with_shadow = true) {
  if (with_shadow) {
    it.line(165 + 4, 424 - 7 + 1, 165 - 4, 424 + 1, shadow_c);
    it.line(165 + 5, 424 - 7 + 1, 165 - 3, 424 + 1, shadow_c);
    it.line(165 + 4, 424 + 7 + 1, 165 - 4, 424 + 1, shadow_c);
    it.line(165 + 5, 424 + 7 + 1, 165 - 3, 424 + 1, shadow_c);

    it.line(301 - 4, 424 - 7 + 1, 301 + 4, 424 + 1, shadow_c);
    it.line(301 - 5, 424 - 7 + 1, 301 + 3, 424 + 1, shadow_c);
    it.line(301 - 4, 424 + 7 + 1, 301 + 4, 424 + 1, shadow_c);
    it.line(301 - 5, 424 + 7 + 1, 301 + 3, 424 + 1, shadow_c);
  }
  it.line(165 + 4, 424 - 7, 165 - 4, 424, chevron_col);
  it.line(165 + 5, 424 - 7, 165 - 3, 424, chevron_col);
  it.line(165 + 4, 424 + 7, 165 - 4, 424, chevron_col);
  it.line(165 + 5, 424 + 7, 165 - 3, 424, chevron_col);

  it.line(301 - 4, 424 - 7, 301 + 4, 424, chevron_col);
  it.line(301 - 5, 424 - 7, 301 + 3, 424, chevron_col);
  it.line(301 - 4, 424 + 7, 301 + 4, 424, chevron_col);
  it.line(301 - 5, 424 + 7, 301 + 3, 424, chevron_col);
}

template<typename DisplayT>
inline void render_lunar_disc(DisplayT &it, int moon_cx, int moon_cy, int moon_r,
                              double cycle, bool with_tycho = true) {
  auto d2 = [](int x1, int y1, int x2, int y2) { return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2); };
  it.circle(moon_cx, moon_cy, moon_r + 1, esphome::Color(35, 45, 60));
  float scale = (float)moon_r / 44.0f;

  for (int my = -moon_r; my <= moon_r; my++) {
    int w_line = (int)sqrtf((float)(moon_r * moon_r - my * my));
    float x_term = ((cycle <= 0.5) ? 1.0f : -1.0f) * (float)w_line * cosf((float)(cycle * 2.0 * 3.1415926535));

    for (int mx = -w_line; mx <= w_line; mx++) {
      float r_dist = sqrtf((float)(mx * mx + my * my));
      float sphere_dim = sqrtf(fmaxf(0.0f, 1.0f - (r_dist * r_dist) / (float)((moon_r + 2) * (moon_r + 2))));
      float base_lum = 205.0f + 25.0f * sphere_dim;

      if (d2(mx, my, (int)(-14 * scale), (int)(-14 * scale)) < (int)(144 * scale * scale)) base_lum -= 58.0f;
      else if (d2(mx, my, (int)(-22 * scale), (int)(2 * scale)) < (int)(225 * scale * scale)) base_lum -= 52.0f;
      else if (d2(mx, my, (int)(8 * scale), (int)(-13 * scale)) < (int)(100 * scale * scale)) base_lum -= 54.0f;
      else if (d2(mx, my, (int)(16 * scale), (int)(-2 * scale)) < (int)(121 * scale * scale)) base_lum -= 56.0f;
      else if (d2(mx, my, (int)(25 * scale), (int)(-10 * scale)) < (int)(49 * scale * scale)) base_lum -= 60.0f;
      else if (d2(mx, my, (int)(18 * scale), (int)(12 * scale)) < (int)(121 * scale * scale)) base_lum -= 50.0f;
      else if (d2(mx, my, (int)(-10 * scale), (int)(16 * scale)) < (int)(121 * scale * scale)) base_lum -= 48.0f;

      if (with_tycho) {
        if (d2(mx, my, (int)(-4 * scale), (int)(28 * scale)) < (int)(16 * scale * scale)) base_lum = 245.0f;
        else if (d2(mx, my, (int)(-4 * scale), (int)(28 * scale)) < (int)(144 * scale * scale) &&
                 abs((my - (int)(28 * scale)) - 1.5f * (mx + (int)(4 * scale))) < (2.0f * scale)) base_lum += 24.0f;
      }

      float dist_to_term = (cycle <= 0.5) ? ((float)mx - x_term) : (x_term - (float)mx);
      float penumbra = 6.0f * scale;
      float shadow;
      if (dist_to_term > penumbra) shadow = 1.0f;
      else if (dist_to_term < -penumbra) shadow = 0.0f;
      else {
        float t = (dist_to_term + penumbra) / (2.0f * penumbra);
        shadow = t * t * (3.0f - 2.0f * t);
      }

      float final_lum = base_lum * shadow + 12.0f * (1.0f - shadow);
      uint8_t c = (uint8_t)fminf(255.0f, final_lum);
      esphome::Color p_col = (shadow < 0.05f)
                                 ? esphome::Color(12, 16, 24)
                                 : esphome::Color(c, (uint8_t)(c * 0.98f), (uint8_t)(c * 0.94f));
      it.draw_pixel_at(moon_cx + mx, moon_cy + my, p_col);
    }
  }
  it.circle(moon_cx, moon_cy, moon_r, esphome::Color(45, 55, 70));
}

template<typename DisplayT>
inline void render_cumulus_cloud(DisplayT &it, int bx, int by, float scale, bool overcast_mode, bool is_night) {
  int max_extent = (int)(95.0f * scale);
  if (bx + max_extent < 0 || bx - max_extent >= 466 ||
      by + max_extent < 0 || by - max_extent >= 466) return;

  static constexpr int8_t lobes[8][3] = {
    {-45,  10, 24}, {-25,  -5, 32}, {  5, -18, 40}, { 35,  -8, 30},
    { 55,  10, 22}, {-10,  15, 26}, { 20,  15, 26}, {  0,   8, 28}
  };

  esphome::Color col_shadow = is_night ? esphome::Color(12, 16, 22) : (overcast_mode ? esphome::Color(24, 38, 56) : esphome::Color(34, 52, 74));
  esphome::Color col_body   = is_night ? esphome::Color(20, 28, 38) : (overcast_mode ? esphome::Color(38, 56, 78) : esphome::Color(54, 78, 106));
  esphome::Color col_high   = is_night ? esphome::Color(36, 48, 64) : (overcast_mode ? esphome::Color(54, 76, 102) : esphome::Color(78, 108, 142));
  esphome::Color col_crest  = is_night ? esphome::Color(58, 74, 98) : (overcast_mode ? esphome::Color(75, 102, 132) : esphome::Color(110, 145, 185));

  int16_t sc_x[8], sc_y[8], sc_r[8];
  for (int i = 0; i < 8; i++) {
    sc_x[i] = (int16_t)(lobes[i][0] * scale);
    sc_y[i] = (int16_t)(lobes[i][1] * scale);
    sc_r[i] = (int16_t)(lobes[i][2] * scale);
  }
  int16_t off_y_sh = (int16_t)(5.0f * scale);
  int16_t off_r_bd = (int16_t)(2.0f * scale);
  int16_t off_x_hi = (int16_t)(3.0f * scale);
  int16_t off_y_hi = (int16_t)(3.0f * scale);
  int16_t off_r_hi = (int16_t)(5.0f * scale);
  int16_t off_x_cr = (int16_t)(4.0f * scale);
  int16_t off_y_cr = (int16_t)(6.0f * scale);
  int16_t off_r_cr = (int16_t)(9.0f * scale);

  static constexpr uint8_t shadow_lobes[5] = {0, 4, 5, 6, 7};
  for (int i = 0; i < 5; i++) {
    uint8_t idx = shadow_lobes[i];
    int sx = bx + sc_x[idx], sy = by + sc_y[idx] + off_y_sh, sr = sc_r[idx];
    if (sx + sr < 0 || sx - sr >= 466 || sy + sr < 0 || sy - sr >= 466) continue;
    it.filled_circle(sx, sy, sr, col_shadow);
  }
  for (int i = 0; i < 5; i++) {
    uint8_t idx = shadow_lobes[i];
    int sx = bx + sc_x[idx], sy = by + sc_y[idx], sr = sc_r[idx] - off_r_bd;
    if (sr <= 0 || sx + sr < 0 || sx - sr >= 466 || sy + sr < 0 || sy - sr >= 466) continue;
    it.filled_circle(sx, sy, sr, col_body);
  }
  for (int i = 0; i < 5; i++) {
    int sx = bx + sc_x[i] + off_x_hi, sy = by + sc_y[i] - off_y_hi, sr = sc_r[i] - off_r_hi;
    if (sr <= 0 || sx + sr < 0 || sx - sr >= 466 || sy + sr < 0 || sy - sr >= 466) continue;
    it.filled_circle(sx, sy, sr, col_high);
  }
  static constexpr uint8_t crest_idx[3] = {1, 2, 3};
  for (int i = 0; i < 3; i++) {
    uint8_t idx = crest_idx[i];
    int sx = bx + sc_x[idx] + off_x_cr, sy = by + sc_y[idx] - off_y_cr, sr = sc_r[idx] - off_r_cr;
    if (sr <= 0 || sx + sr < 0 || sx - sr >= 466 || sy + sr < 0 || sy - sr >= 466) continue;
    it.filled_circle(sx, sy, sr, col_crest);
  }
}

// Zero-overdraw album art blitter with fused 2x-stride checkerboard scrim
// and black-pixel skip (leverages mipi_spi's pre-cleared black framebuffer).
template<typename DisplayT, typename ImageT>
inline bool render_album_art_canvas(DisplayT &it, ImageT *target_art, int cx,
                                    bool is_standard, bool is_letterbox,
                                    bool apply_scrim, bool show_metadata) {
  if (target_art == nullptr) return false;
  const uint16_t *src_data = reinterpret_cast<const uint16_t *>(target_art->get_data_start());
  int src_w = target_art->get_width();
  int src_h = target_art->get_height();
  if (src_data == nullptr || src_w <= 0 || src_h <= 0) return false;

  if (is_standard) {
    int step_x = (src_w << 12) / 328;
    int step_y = (src_h << 12) / 328;
    int curr_y = 0;
    for (int y = 0; y < 328; y++) {
      int sy = curr_y >> 12;
      curr_y += step_y;
      const uint16_t *src_row = src_data + sy * src_w;
      int py = 69 + y;
      int curr_x = 0;
      for (int x = 0; x < 328; x++) {
        if (!apply_scrim || ((x + py) & 1) != 0) {
          uint16_t p = src_row[curr_x >> 12];
          if (p != 0) {
            uint8_t r = (p & 0xF800) >> 8;
            uint8_t g = (p & 0x07E0) >> 3;
            uint8_t b = (p & 0x001F) << 3;
            it.draw_pixel_at(69 + x, py, esphome::Color(r, g, b));
          }
        }
        curr_x += step_x;
      }
    }
    return true;
  }

  int start_y = is_letterbox ? 60 : 0;
  int end_y = is_letterbox ? 385 : 466;
  int scrim_end = is_letterbox ? 398 : 405;

  if (src_w == 466 && src_h == 466) {
    for (int y = start_y; y < end_y; y++) {
      int hw = CIRCLE_HW[y];
      int start_x = cx - hw;
      int end_x = cx + hw;
      const uint16_t *src_row = src_data + y * 466;
      bool row_scrim = apply_scrim || (show_metadata && y >= 340 && y < scrim_end);
      if (row_scrim) {
        int x0 = start_x + (((start_x + y) & 1) == 0 ? 1 : 0);
        for (int x = x0; x <= end_x; x += 2) {
          uint16_t p = src_row[x];
          if (p != 0) {
            uint8_t r = (p & 0xF800) >> 8;
            uint8_t g = (p & 0x07E0) >> 3;
            uint8_t b = (p & 0x001F) << 3;
            it.draw_pixel_at(x, y, esphome::Color(r, g, b));
          }
        }
      } else {
        for (int x = start_x; x <= end_x; x++) {
          uint16_t p = src_row[x];
          if (p != 0) {
            uint8_t r = (p & 0xF800) >> 8;
            uint8_t g = (p & 0x07E0) >> 3;
            uint8_t b = (p & 0x001F) << 3;
            it.draw_pixel_at(x, y, esphome::Color(r, g, b));
          }
        }
      }
    }
  } else {
    int step_x = (src_w << 12) / 466;
    int step_y = (src_h << 12) / 466;
    int curr_y = start_y * step_y;
    for (int y = start_y; y < end_y; y++) {
      int sy = curr_y >> 12;
      curr_y += step_y;
      const uint16_t *src_row = src_data + sy * src_w;
      int hw = CIRCLE_HW[y];
      int start_x = cx - hw;
      int end_x = cx + hw;
      bool row_scrim = apply_scrim || (show_metadata && y >= 340 && y < scrim_end);
      if (row_scrim) {
        int x0 = start_x + (((start_x + y) & 1) == 0 ? 1 : 0);
        for (int x = x0; x <= end_x; x += 2) {
          uint16_t p = src_row[(x * step_x) >> 12];
          if (p != 0) {
            uint8_t r = (p & 0xF800) >> 8;
            uint8_t g = (p & 0x07E0) >> 3;
            uint8_t b = (p & 0x001F) << 3;
            it.draw_pixel_at(x, y, esphome::Color(r, g, b));
          }
        }
      } else {
        for (int x = start_x; x <= end_x; x++) {
          uint16_t p = src_row[(x * step_x) >> 12];
          if (p != 0) {
            uint8_t r = (p & 0xF800) >> 8;
            uint8_t g = (p & 0x07E0) >> 3;
            uint8_t b = (p & 0x001F) << 3;
            it.draw_pixel_at(x, y, esphome::Color(r, g, b));
          }
        }
      }
    }
  }
  return true;
}

// ------------------------------------------------------------------------------
// 3. Text, Time, SemVer & Synodic Astronomy Helpers
// ------------------------------------------------------------------------------
// UTF-8 sanitizer & normalizer for Roboto / Orbitron embedded bitmap fonts.
// Preserves ASCII (0x20..0x7E), full Latin-1 Supplement (U+00A1..U+00FF, e.g. ¡, ¿, ñ, é, ü, ß),
// Latin Extended-A (U+0100..U+017F), and core typographic symbols (•—–…‘’“”€™),
// while normalizing exotic Unicode spaces/dashes/quotes and stripping unsupported emojis
// or malformed UTF-8 sequences so ESPHome's font renderer never logs missing-glyph errors.
inline void append_utf8_cp(std::string &out, uint32_t cp) {
  if (cp <= 0x7F) {
    out.push_back(static_cast<char>(cp));
  } else if (cp <= 0x7FF) {
    out.push_back(static_cast<char>(0xC0 | ((cp >> 6) & 0x1F)));
    out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
  } else if (cp <= 0xFFFF) {
    out.push_back(static_cast<char>(0xE0 | ((cp >> 12) & 0x0F)));
    out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
    out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
  }
}

inline bool is_supported_font_cp(uint32_t cp) {
  if (cp >= 0x20 && cp <= 0x7E) return true;
  if (cp >= 0xA1 && cp <= 0xFF && cp != 0xAD) return true;  // Full Latin-1 Supplement (¡, ¿, À..ÿ)
  switch (cp) {
    // Supported Latin Extended-A & B (European languages in GF_Latin_Core)
    case 0x0100: case 0x0101: case 0x0102: case 0x0103: case 0x0104: case 0x0105:
    case 0x0106: case 0x0107: case 0x010C: case 0x010D: case 0x010E: case 0x010F:
    case 0x0110: case 0x0111: case 0x0112: case 0x0113: case 0x0116: case 0x0117:
    case 0x0118: case 0x0119: case 0x011A: case 0x011B: case 0x011E: case 0x011F:
    case 0x012A: case 0x012B: case 0x012E: case 0x012F: case 0x0130: case 0x0131:
    case 0x0136: case 0x0137: case 0x0139: case 0x013A: case 0x013B: case 0x013C:
    case 0x013D: case 0x013E: case 0x0141: case 0x0142: case 0x0143: case 0x0144:
    case 0x0145: case 0x0146: case 0x0147: case 0x0148: case 0x014C: case 0x014D:
    case 0x0150: case 0x0151: case 0x0152: case 0x0153: case 0x0154: case 0x0155:
    case 0x0158: case 0x0159: case 0x015A: case 0x015B: case 0x015E: case 0x015F:
    case 0x0160: case 0x0161: case 0x0162: case 0x0163: case 0x0164: case 0x0165:
    case 0x016A: case 0x016B: case 0x016E: case 0x016F: case 0x0170: case 0x0171:
    case 0x0172: case 0x0173: case 0x0178: case 0x0179: case 0x017A: case 0x017B:
    case 0x017C: case 0x017D: case 0x017E:
    // Typographic punctuation & symbols compiled into font_micro / font_small / font_date / font_btn_bold
    case 0x2013: case 0x2014: case 0x2018: case 0x2019: case 0x201C: case 0x201D:
    case 0x2022: case 0x2026: case 0x20AC: case 0x2122:
      return true;
    default:
      return false;
  }
}

inline std::string sanitize_utf8_for_font(const std::string &str) {
  if (str.empty()) return str;
  std::string out;
  out.reserve(str.size());
  const size_t n = str.size();
  size_t i = 0;
  while (i < n) {
    unsigned char b0 = static_cast<unsigned char>(str[i]);
    if (b0 < 0x80) {
      if (b0 >= 0x20 && b0 <= 0x7E) out.push_back(static_cast<char>(b0));
      i++;
      continue;
    }
    uint32_t cp = 0;
    size_t len = 0;
    if ((b0 & 0xE0) == 0xC0 && i + 1 < n) {
      unsigned char b1 = static_cast<unsigned char>(str[i + 1]);
      if ((b1 & 0xC0) == 0x80) {
        cp = (static_cast<uint32_t>(b0 & 0x1F) << 6) | static_cast<uint32_t>(b1 & 0x3F);
        if (cp >= 0x80) len = 2;
      }
    } else if ((b0 & 0xF0) == 0xE0 && i + 2 < n) {
      unsigned char b1 = static_cast<unsigned char>(str[i + 1]);
      unsigned char b2 = static_cast<unsigned char>(str[i + 2]);
      if ((b1 & 0xC0) == 0x80 && (b2 & 0xC0) == 0x80) {
        cp = (static_cast<uint32_t>(b0 & 0x0F) << 12) | (static_cast<uint32_t>(b1 & 0x3F) << 6) | static_cast<uint32_t>(b2 & 0x3F);
        if (cp >= 0x800) len = 3;
      }
    } else if ((b0 & 0xF8) == 0xF0 && i + 3 < n) {
      unsigned char b1 = static_cast<unsigned char>(str[i + 1]);
      unsigned char b2 = static_cast<unsigned char>(str[i + 2]);
      unsigned char b3 = static_cast<unsigned char>(str[i + 3]);
      if ((b1 & 0xC0) == 0x80 && (b2 & 0xC0) == 0x80 && (b3 & 0xC0) == 0x80) {
        cp = (static_cast<uint32_t>(b0 & 0x07) << 18) | (static_cast<uint32_t>(b1 & 0x3F) << 12) |
             (static_cast<uint32_t>(b2 & 0x3F) << 6) | static_cast<uint32_t>(b3 & 0x3F);
        if (cp >= 0x10000) len = 4;
      }
    }
    if (len == 0) {
      i++;
      continue;
    }
    i += len;

    // Normalize common Unicode whitespace, dashes, quotes, and zero-width characters
    if (cp == 0x00A0 || (cp >= 0x2000 && cp <= 0x200A) || cp == 0x202F || cp == 0x205F || cp == 0x3000) {
      out.push_back(' ');
      continue;
    }
    if (cp == 0x00AD || (cp >= 0x200B && cp <= 0x200F) || (cp >= 0x202A && cp <= 0x202E) ||
        (cp >= 0xFE00 && cp <= 0xFE0F) || cp == 0xFEFF) {
      continue;
    }
    if (cp == 0x2010 || cp == 0x2011 || cp == 0x2012 || cp == 0x2212 || cp == 0xFF0D) {
      out.push_back('-');
      continue;
    }
    if (cp == 0x2015) {
      append_utf8_cp(out, 0x2014);  // Horizontal bar -> em-dash
      continue;
    }
    if (cp == 0x02B9 || cp == 0x02BB || cp == 0x02BC || cp == 0x201A || cp == 0x2032 || cp == 0x2039 || cp == 0x203A || cp == 0xFF07) {
      out.push_back('\'');
      continue;
    }
    if (cp == 0x201E || cp == 0x2033 || cp == 0xFF02) {
      out.push_back('"');
      continue;
    }
    if (cp == 0xFF01) { out.push_back('!'); continue; }
    if (cp == 0xFF1F) { out.push_back('?'); continue; }

    if (is_supported_font_cp(cp)) {
      append_utf8_cp(out, cp);
    }
  }
  return out;
}

template<typename FontT>
inline std::string fit_text_to_width_font(const std::string &raw_str, int max_w, FontT *font) {
  std::string str = sanitize_utf8_for_font(raw_str);
  if (str.empty() || font == nullptr) return "";
  int w = 0, x_off = 0, base = 0, text_h = 0;
  font->measure(str.c_str(), &w, &x_off, &base, &text_h);
  if (w <= max_w) return str;

  std::string cur = str;
  if (cur.length() > 32) {
    while (cur.length() > 30) {
      while (!cur.empty() && (static_cast<unsigned char>(cur.back()) & 0xC0) == 0x80) cur.pop_back();
      if (!cur.empty()) cur.pop_back();
    }
  }
  while (!cur.empty()) {
    std::string test_str = cur + "..";
    font->measure(test_str.c_str(), &w, &x_off, &base, &text_h);
    if (w <= max_w) return test_str;
    while (!cur.empty() && (static_cast<unsigned char>(cur.back()) & 0xC0) == 0x80) cur.pop_back();
    if (!cur.empty()) cur.pop_back();
  }
  return "..";
}

inline bool is_newer_semver(const std::string &lat_str, const std::string &cur_str) {
  if (lat_str.empty() || lat_str == "unavailable" || lat_str == "unknown") return false;
  std::string l = lat_str;
  if (!l.empty() && (l[0] == 'v' || l[0] == 'V')) l = l.substr(1);
  std::string c = cur_str;
  if (!c.empty() && (c[0] == 'v' || c[0] == 'V')) c = c.substr(1);
  if (l.rfind("202", 0) == 0) return false;
  int l_maj = 0, l_min = 0, l_pat = 0;
  int c_maj = 0, c_min = 0, c_pat = 0;
  if (sscanf(l.c_str(), "%d.%d.%d", &l_maj, &l_min, &l_pat) < 2) return false;
  if (sscanf(c.c_str(), "%d.%d.%d", &c_maj, &c_min, &c_pat) < 2) return false;
  if (l_maj > c_maj) return true;
  if (l_maj < c_maj) return false;
  if (l_min > c_min) return true;
  if (l_min < c_min) return false;
  return l_pat > c_pat;
}

inline void format_time_hhmm(int hour, int minute, bool is_24h, char *out_buf,
                             size_t out_sz, char *ap_buf = nullptr, bool pad_zero = false) {
  if (is_24h) {
    snprintf(out_buf, out_sz, "%02d:%02d", hour, minute);
    if (ap_buf) ap_buf[0] = '\0';
  } else {
    int h12 = hour % 12;
    if (h12 == 0) h12 = 12;
    if (pad_zero) {
      snprintf(out_buf, out_sz, "%02d:%02d", h12, minute);
    } else {
      snprintf(out_buf, out_sz, "%d:%02d", h12, minute);
    }
    if (ap_buf) {
      strcpy(ap_buf, (hour >= 12) ? "PM" : "AM");
    }
  }
}

inline void compute_synodic_moon_phase(bool time_valid, int64_t epoch_sec, const std::string &fallback_phase,
                                       double &out_cycle, int *out_illum = nullptr, std::string *out_label = nullptr) {
  double cycle = 0.5;
  int illum_pct = 50;
  std::string mp_label = "Moon";

  if (time_valid) {
    double diff = (double)(epoch_sec - 947182440LL);
    cycle = fmod(diff, 2551442.86) / 2551442.86;
    if (cycle < 0.0) cycle += 1.0;
    illum_pct = (int)round((1.0 - cos(2.0 * 3.1415926535 * cycle)) / 2.0 * 100.0);

    if (cycle < 0.03 || cycle > 0.97) mp_label = "New Moon";
    else if (cycle < 0.22) mp_label = "Waxing Crescent";
    else if (cycle < 0.28) mp_label = "First Quarter";
    else if (cycle < 0.47) mp_label = "Waxing Gibbous";
    else if (cycle < 0.53) mp_label = "Full Moon";
    else if (cycle < 0.72) mp_label = "Waning Gibbous";
    else if (cycle < 0.78) mp_label = "Last Quarter";
    else mp_label = "Waning Crescent";
  } else {
    if (fallback_phase == "new_moon") { cycle = 0.0; illum_pct = 0; mp_label = "New Moon"; }
    else if (fallback_phase == "waxing_crescent") { cycle = 0.15; illum_pct = 25; mp_label = "Waxing Crescent"; }
    else if (fallback_phase == "first_quarter") { cycle = 0.25; illum_pct = 50; mp_label = "First Quarter"; }
    else if (fallback_phase == "waxing_gibbous") { cycle = 0.38; illum_pct = 75; mp_label = "Waxing Gibbous"; }
    else if (fallback_phase == "full_moon") { cycle = 0.50; illum_pct = 100; mp_label = "Full Moon"; }
    else if (fallback_phase == "waning_gibbous") { cycle = 0.68; illum_pct = 68; mp_label = "Waning Gibbous"; }
    else if (fallback_phase == "last_quarter" || fallback_phase == "third_quarter") { cycle = 0.75; illum_pct = 50; mp_label = "Last Quarter"; }
    else if (fallback_phase == "waning_crescent") { cycle = 0.88; illum_pct = 22; mp_label = "Waning Crescent"; }
  }

  out_cycle = cycle;
  if (out_illum != nullptr) *out_illum = illum_pct;
  if (out_label != nullptr) *out_label = mp_label;
}

// ------------------------------------------------------------------------------
// 4. Standardized Touch Timing Constants, Hit-Testing & Value Steppers (30 Hz)
// ------------------------------------------------------------------------------
static constexpr uint32_t TOUCH_DEBOUNCE_RAPID   = 100; // ms: Rapid steppers
static constexpr uint32_t TOUCH_DEBOUNCE_BUTTON  = 150; // ms: Standard buttons & pills
static constexpr uint32_t TOUCH_DEBOUNCE_WAKE    = 180; // ms: Post-wake settling lockout
static constexpr uint32_t TOUCH_DEBOUNCE_DISMISS = 200; // ms: Overlay/HUD dismissal
static constexpr uint32_t TOUCH_DEBOUNCE_SCROLL  = 250; // ms: Page transitions & chevrons
static constexpr uint32_t TOUCH_GUARD_DISMISS    = 250; // ms: Minimum post-wake delay

template<typename TouchT>
inline bool is_back_button(const TouchT &t) {
  return (t.y >= 385 && t.y <= 465 && t.x >= 50 && t.x <= 416);
}

template<typename TouchT>
inline int get_5slot_index(const TouchT &t, int x_min, int x_max) {
  if (t.x < x_min || t.x > x_max) return -1;
  if (t.y >= 75  && t.y <= 133) return 0;
  if (t.y >= 134 && t.y <= 191) return 1;
  if (t.y >= 192 && t.y <= 249) return 2;
  if (t.y >= 250 && t.y <= 307) return 3;
  if (t.y >= 308 && t.y <= 374) return 4;
  return -1;
}

template<typename ValT, typename DeltaT, typename MinT, typename MaxT>
inline auto step_clamp(ValT val, DeltaT delta, MinT min_val, MaxT max_val) {
  auto res = val + delta;
  if (res < min_val) return static_cast<decltype(res)>(min_val);
  if (res > max_val) return static_cast<decltype(res)>(max_val);
  return res;
}

inline int step_wrap(int val, int delta, int count) {
  if (count <= 0) return 0;
  int res = (val + delta) % count;
  if (res < 0) res += count;
  return res;
}

inline int step_wrap_1based(int val, int delta, int count) {
  if (count <= 1) return 1;
  int zero_based = (val - 1 + delta) % count;
  if (zero_based < 0) zero_based += count;
  return zero_based + 1;
}

template<typename ValT, typename DeltaT, typename MinT, typename MaxT>
inline auto step_wrap_range(ValT val, DeltaT delta, MinT min_val, MaxT max_val) {
  auto next = val + delta;
  if (next > max_val + 0.001f) return static_cast<decltype(next)>(min_val);
  if (next < min_val - 0.001f) return static_cast<decltype(next)>(max_val);
  return next;
}

template<typename CurT>
inline const char *cycle_option(const CurT &cur, std::initializer_list<const char *> opts) {
  for (auto it = opts.begin(); it != opts.end(); ++it) {
    if (cur == *it) {
      ++it;
      return (it != opts.end()) ? *it : *opts.begin();
    }
  }
  return (opts.size() > 0) ? *opts.begin() : "";
}


// ------------------------------------------------------------------------------
// 5. Docked Screensaver & Voice Assistant Spectral Engines
// ------------------------------------------------------------------------------
template<typename DisplayT>
inline void render_screensaver_aurora(DisplayT &it, int cx, int cy, uint32_t now_ms) {
  float t = now_ms * 0.0010f;
  for (int x = 8; x <= 458; x += 2) {
    int dx = x - cx;
    int dx2 = dx * dx;
    if (dx2 >= 51984) continue;
    int h_half = (int)sqrtf(51984.0f - (float)dx2);
    int y_min = cy - h_half;
    int y_max = cy + h_half;

    int y1 = cy - 35 + (int)(sinf(x * 0.011f + t * 0.6f) * 55.0f + cosf(x * 0.006f - t * 0.35f) * 25.0f);
    int len1 = 55 + (int)(sinf(x * 0.024f + t * 1.2f) * 25.0f);
    for (int py = y1; py >= y1 - len1; py -= 2) {
      if (py >= y_min && py <= y_max) {
        float k = (float)(y1 - py) / (float)len1;
        it.draw_pixel_at(x, py, esphome::Color((uint8_t)(180.0f - 80.0f * k), (uint8_t)(30.0f - 15.0f * k), (uint8_t)(190.0f - 60.0f * k)));
      }
    }

    int y2 = cy + 15 + (int)(sinf(x * 0.014f - t * 0.8f + 1.6f) * 60.0f + sinf(x * 0.008f + t * 0.5f) * 30.0f);
    int len2 = 65 + (int)(cosf(x * 0.028f - t * 1.5f) * 30.0f);
    for (int py = y2; py >= y2 - len2; py -= 2) {
      if (py >= y_min && py <= y_max) {
        float k = (float)(y2 - py) / (float)len2;
        it.draw_pixel_at(x, py, esphome::Color((uint8_t)(15.0f * (1.0f - k)), (uint8_t)(230.0f - 110.0f * k), (uint8_t)(240.0f - 70.0f * k)));
      }
    }

    int y3 = cy + 65 + (int)(sinf(x * 0.017f + t * 1.0f + 3.1f) * 65.0f + cosf(x * 0.010f - t * 0.7f) * 35.0f);
    int len3 = 80 + (int)(sinf(x * 0.032f + t * 1.8f) * 35.0f);
    for (int py = y3; py >= y3 - len3; py -= 2) {
      if (py >= y_min && py <= y_max) {
        float k = (float)(y3 - py) / (float)len3;
        it.draw_pixel_at(x, py, esphome::Color((uint8_t)(25.0f * (1.0f - k)), (uint8_t)(235.0f - 120.0f * k), (uint8_t)(85.0f - 40.0f * k)));
      }
    }
  }
}

template<typename DisplayT>
inline void render_screensaver_starfield(DisplayT &it, int cx, int cy) {
  struct Star3D { float x, y, z; };
  static Star3D stars[90];
  static bool stars_init = false;
  static uint32_t star_seed = 123456789;

  auto star_rand = [&]() -> float {
    star_seed = star_seed * 1664525u + 1013904223u;
    return (float)(star_seed & 0xFFFF) / 65535.0f;
  };
  auto respawn_star = [&](int i, bool init_z) {
    float ang = star_rand() * 2.0f * PI_F;
    float dist = 40.0f + star_rand() * 650.0f;
    stars[i].x = cosf(ang) * dist;
    stars[i].y = sinf(ang) * dist;
    stars[i].z = init_z ? (30.0f + star_rand() * 950.0f) : (850.0f + star_rand() * 150.0f);
  };
  if (!stars_init) {
    for (int i = 0; i < 90; i++) respawn_star(i, true);
    stars_init = true;
  }
  float speed = 25.0f;
  for (int i = 0; i < 90; i++) {
    stars[i].z -= speed;
    float k = 230.0f / stars[i].z;
    int sx = cx + (int)(stars[i].x * k);
    int sy = cy + (int)(stars[i].y * k);
    int dx = sx - cx, dy = sy - cy;
    if (stars[i].z <= 25.0f || (dx * dx + dy * dy) >= 51529) {
      respawn_star(i, false);
      continue;
    }
    float prev_k = 230.0f / (stars[i].z + speed * 1.5f);
    int px = cx + (int)(stars[i].x * prev_k);
    int py = cy + (int)(stars[i].y * prev_k);
    if (stars[i].z > 650.0f) {
      it.draw_pixel_at(sx, sy, esphome::Color(80, 100, 130));
    } else if (stars[i].z > 350.0f) {
      it.line(px, py, sx, sy, esphome::Color(100, 180, 230));
      it.draw_pixel_at(sx, sy, esphome::Color(180, 220, 255));
    } else {
      it.line(px, py, sx, sy, esphome::Color(40, 180, 255));
      it.filled_circle(sx, sy, 2, esphome::Color(255, 255, 255));
    }
  }
}

template<typename DisplayT>
inline void render_screensaver_matrix(DisplayT &it, int cx, int cy, uint32_t now_ms) {
  for (int i = 0; i < 31; i++) {
    int lead_y = (MATRIX_Y[i] + (int)((uint64_t)now_ms * (uint32_t)MATRIX_SPD[i] / 1000ULL)) % 820 - 120;
    int sx = MATRIX_X[i];
    int slen = MATRIX_LEN[i];
    for (int j = 0; j < slen; j++) {
      int gy = lead_y - j * 16;
      int dx = sx - cx, dy = gy - cy;
      if (dx * dx + dy * dy < 51529 && gy > 10 && gy < 456) {
        if (j == 0) {
          it.filled_rectangle(sx - 3, gy - 5, 7, 11, esphome::Color(140, 255, 140));
          it.filled_rectangle(sx - 2, gy - 4, 5, 9, esphome::Color(255, 255, 255));
        } else if (j <= 2) {
          it.filled_rectangle(sx - 3, gy - 5, 6, 10, esphome::Color(40, 255, 80));
          it.filled_rectangle(sx - 2, gy - 4, 4, 8, esphome::Color(160, 255, 180));
        } else if (j <= 5) {
          it.filled_rectangle(sx - 2, gy - 4, 5, 8, esphome::Color(20, 200, 50));
        } else if (j <= 8) {
          it.filled_rectangle(sx - 2, gy - 3, 4, 6, esphome::Color(10, 130, 30));
        } else {
          it.filled_rectangle(sx - 1, gy - 2, 3, 4, esphome::Color(5, 65, 15));
        }
      }
    }
  }
}

template<typename DisplayT>
inline void render_screensaver_solar_flare(DisplayT &it, int cx, int cy, uint32_t now_ms) {
  float t = now_ms * 0.0015f;
  int scx = cx + (int)(sinf(t * 0.20f) * 6.0f);
  int scy = cy + (int)(cosf(t * 0.16f) * 6.0f);
  float r0 = 62.0f + 6.0f * sinf(t * 1.5f);

  for (int w = 0; w < 3; w++) {
    float phase = fmodf((float)now_ms + (float)w * 950.0f, 2850.0f) / 2850.0f;
    int cme_r = (int)((r0 + 10.0f) + phase * (220.0f - (r0 + 10.0f)));
    float fade = 1.0f - phase;
    uint8_t c_red = (uint8_t)(220.0f * fade);
    uint8_t c_grn = (uint8_t)(90.0f * fade * fade);
    if (cme_r > (int)r0 + 10) {
      it.circle(scx, scy, cme_r, esphome::Color(c_red < 20 ? 20 : c_red, c_grn < 5 ? 5 : c_grn, 5));
      it.circle(scx, scy, cme_r - 2, esphome::Color((uint8_t)(c_red * 0.7f), (uint8_t)(c_grn * 0.7f), 2));
    }
  }

  for (int i = 0; i < 36; i++) {
    float theta = i * (2.0f * PI_F / 36.0f);
    float sa = sinf(theta), ca = cosf(theta);
    float L = 35.0f + 25.0f * sinf(theta * 4.0f + t * 2.4f) + 18.0f * cosf(theta * 7.0f - t * 1.8f) + 12.0f * sinf(theta * 11.0f + t * 3.2f);
    int in_x = scx + (int)((r0 + 4.0f) * ca), in_y = scy + (int)((r0 + 4.0f) * sa);
    int mid_x = scx + (int)((r0 + 4.0f + L * 0.6f) * ca), mid_y = scy + (int)((r0 + 4.0f + L * 0.6f) * sa);
    int out_x = scx + (int)((r0 + 4.0f + L) * ca), out_y = scy + (int)((r0 + 4.0f + L) * sa);
    it.line(in_x, in_y, mid_x, mid_y, esphome::Color(225, 75, 10));
    it.line(mid_x, mid_y, out_x, out_y, esphome::Color(255, 185, 25));
    if (L > 65.0f) {
      it.draw_pixel_at(out_x, out_y, esphome::Color(255, 220, 45));
      it.draw_pixel_at(out_x + 1, out_y, esphome::Color(255, 180, 20));
    }
  }

  auto draw_prominence_loop = [&](float center_ang, float peak_r, float arc_span) {
    float prev_lx = 0, prev_ly = 0;
    for (int step = 0; step <= 8; step++) {
      float prog = (float)step / 8.0f;
      float cur_ang = center_ang - arc_span * 0.5f + arc_span * prog;
      float h_factor = 4.0f * prog * (1.0f - prog);
      float cur_r = (r0 + 6.0f) + (peak_r - (r0 + 6.0f)) * h_factor;
      float lx = scx + cur_r * cosf(cur_ang), ly = scy + cur_r * sinf(cur_ang);
      if (step > 0) {
        it.line((int)prev_lx, (int)prev_ly, (int)lx, (int)ly, esphome::Color(255, 175, 25));
        it.line((int)prev_lx, (int)prev_ly + 1, (int)lx, (int)ly + 1, esphome::Color(220, 60, 5));
      }
      prev_lx = lx; prev_ly = ly;
    }
  };
  draw_prominence_loop(t * 0.5f, 155.0f + 20.0f * sinf(t * 1.4f), 0.55f);
  draw_prominence_loop(t * 0.5f + 2.2f, 175.0f + 25.0f * cosf(t * 1.1f), 0.70f);
  draw_prominence_loop(t * 0.5f + 4.3f, 145.0f + 18.0f * sinf(t * 1.9f), 0.50f);

  it.filled_circle(scx, scy, (int)(r0 + 14.0f), esphome::Color(160, 25, 10));
  it.filled_circle(scx, scy, (int)(r0 + 4.0f),  esphome::Color(235, 80, 10));
  it.filled_circle(scx, scy, (int)(r0 - 6.0f),  esphome::Color(255, 155, 20));

  for (int cp = 0; cp < 2; cp++) {
    float cp_phase = fmodf(t * 0.9f + (float)cp * 0.5f, 1.0f);
    int cp_r = (int)(4.0f + cp_phase * (r0 - 12.0f));
    uint8_t cr = (uint8_t)(255.0f - 30.0f * cp_phase);
    uint8_t cg = (uint8_t)(210.0f * (1.0f - 0.55f * cp_phase));
    uint8_t cb = (uint8_t)(35.0f * (1.0f - cp_phase));
    it.filled_circle(scx, scy, cp_r, esphome::Color(cr, cg, cb));
  }
  for (int k = 0; k < 4; k++) {
    float c_ang = t * 0.8f + k * (PI_F / 2.0f);
    float c_rad = 18.0f + 8.0f * sinf(t * 1.8f + (float)k);
    int vx = scx + (int)(cosf(c_ang) * c_rad), vy = scy + (int)(sinf(c_ang) * c_rad);
    uint8_t gran_g = (uint8_t)(160.0f + 50.0f * sinf(t * 2.0f + (float)k));
    it.filled_circle(vx, vy, 8, esphome::Color(255, gran_g, 20));
  }
  int eye_r = (int)(7.0f + 3.0f * sinf(t * 2.6f));
  uint8_t eye_g = (uint8_t)(120.0f + 70.0f * sinf(t * 2.2f));
  it.filled_circle(scx, scy, eye_r, esphome::Color(255, eye_g, 15));
}

// Linear RGB color interpolation helper (k in [0.0f .. 1.0f])
inline esphome::Color lerp_color(esphome::Color a, esphome::Color b, float k) {
  if (k <= 0.0f) return a;
  if (k >= 1.0f) return b;
  return esphome::Color(
      static_cast<uint8_t>(a.r + (b.r - a.r) * k),
      static_cast<uint8_t>(a.g + (b.g - a.g) * k),
      static_cast<uint8_t>(a.b + (b.b - a.b) * k));
}

inline esphome::Color scale_color(esphome::Color c, float scale) {
  if (scale <= 0.0f) return esphome::Color(0, 0, 0);
  if (scale >= 1.0f) return c;
  return esphome::Color(
      static_cast<uint8_t>(c.r * scale),
      static_cast<uint8_t>(c.g * scale),
      static_cast<uint8_t>(c.b * scale));
}

inline esphome::Color get_va_radiation_color(bool is_solaris, float t) {
  t = t - floorf(t);
  if (is_solaris) {
    if (t < 0.333f) {
      float k = t / 0.333f;
      return esphome::Color((uint8_t)(35.0f * (1.0f - k) + 255.0f * k),
                            (uint8_t)(210.0f * (1.0f - k) + 220.0f * k),
                            (uint8_t)(110.0f * (1.0f - k) + 30.0f * k));
    } else if (t < 0.667f) {
      float k = (t - 0.333f) / 0.334f;
      return esphome::Color((uint8_t)(255.0f * (1.0f - k) + 250.0f * k),
                            (uint8_t)(220.0f * (1.0f - k) + 100.0f * k),
                            (uint8_t)(30.0f * (1.0f - k) + 15.0f * k));
    } else {
      float k = (t - 0.667f) / 0.333f;
      return esphome::Color((uint8_t)(250.0f * (1.0f - k) + 35.0f * k),
                            (uint8_t)(100.0f * (1.0f - k) + 210.0f * k),
                            (uint8_t)(15.0f * (1.0f - k) + 110.0f * k));
    }
  } else {
    if (t < 0.333f) {
      float k = t / 0.333f;
      return esphome::Color((uint8_t)(138.0f * k), (uint8_t)(229.0f * (1.0f - k) + 43.0f * k), (uint8_t)(255.0f * (1.0f - k) + 226.0f * k));
    } else if (t < 0.667f) {
      float k = (t - 0.333f) / 0.334f;
      return esphome::Color((uint8_t)(138.0f + 86.0f * k), (uint8_t)(43.0f + 21.0f * k), (uint8_t)(226.0f + 25.0f * k));
    } else {
      float k = (t - 0.667f) / 0.333f;
      return esphome::Color((uint8_t)(224.0f * (1.0f - k)), (uint8_t)(64.0f * (1.0f - k) + 229.0f * k), 255);
    }
  }
}

// Complete Voice Assistant Visual Engine supporting 4 profiles:
//   - "Nebula": Flagship 3D Multi-Ribbon Silk Wave Orb (Siri / Gemini style, LUT-driven)
//   - "Solaris": Flagship Radial Acoustic Corona & Solar Astrolabe Spectrometer
//   - "Classic Nebula": Original Nebula visual design with trig & loop optimizations
//   - "Classic Solaris": Original Solaris visual design with trig & loop optimizations
template<typename DisplayT, typename FontT, typename StrRefT>
inline void render_voice_assistant_overlay(
    DisplayT &it, int cx, int cy, uint32_t now_ms,
    bool is_speaking, bool is_thinking, bool va_vad_active,
    const StrRefT &profile_opt, const std::string &va_status_text,
    FontT *font_small,
    esphome::Color col_white, esphome::Color col_dark_gray,
    esphome::Color col_cyan, esphome::Color col_light_blue,
    esphome::Color col_purple, esphome::Color col_magenta,
    esphome::Color col_solaris_gold) {

  const bool is_classic_nebula = (profile_opt == "Classic Nebula");
  const bool is_classic_solaris = (profile_opt == "Classic Solaris");
  const bool is_classic = (is_classic_nebula || is_classic_solaris);
  const bool is_solaris = (profile_opt == "Solaris" || is_classic_solaris);

  // Smooth exponential VAD envelope (0.0f -> 1.0f) for fluid organic transitions
  static float smooth_vad = 0.0f;
  const float vad_target = va_vad_active ? 1.0f : 0.0f;
  smooth_vad += (vad_target - smooth_vad) * 0.35f;

  // ============================================================================
  // A. CLASSIC NEBULA & CLASSIC SOLARIS (Preserved Visuals, C++ Trig Optimized)
  // ============================================================================
  if (is_classic) {
    if (is_speaking) {
      for (int w = 0; w < 5; w++) {
        uint32_t wave_t = (now_ms + static_cast<uint32_t>(w) * 400U) % 2000U;
        float progress = static_cast<float>(wave_t) * 0.0005f;
        int r_center = 36 + static_cast<int>(progress * 180.0f);

        float width_factor = sinf(progress * PI_F);
        int base_w = (w % 2 == 0) ? 7 : 4;
        int band_w = base_w + static_cast<int>(5.0f * width_factor);
        if (band_w < 3) band_w = 3;
        int half_w = band_w >> 1;

        float wave_color_phase = static_cast<float>(w) * 0.20f + progress * 0.30f +
                                 static_cast<float>(now_ms % 4000U) * 0.00025f;
        float fade = 1.0f - (progress * 0.45f);
        float inv_half = 1.0f / static_cast<float>(half_w + 1);
        float white_boost = 76.5f * fade;  // 255 * 0.30 * fade
        float body_scale = 0.70f * fade;

        for (int d = -half_w; d <= half_w; d++) {
          int r_curr = r_center + d;
          if (r_curr < 34 || r_curr > 226) continue;

          float edge_dist = static_cast<float>(d < 0 ? -d : d) * inv_half;
          esphome::Color base_col = get_va_radiation_color(is_solaris, wave_color_phase + static_cast<float>(d) * 0.02f);

          esphome::Color stroke_col = (edge_dist < 0.35f)
              ? esphome::Color(
                    static_cast<uint8_t>(base_col.r * body_scale + white_boost),
                    static_cast<uint8_t>(base_col.g * body_scale + white_boost),
                    static_cast<uint8_t>(base_col.b * body_scale + white_boost))
              : scale_color(base_col, fade);
          it.circle(cx, cy, r_curr, stroke_col);
        }
      }

      float core_pulse = sinf(static_cast<float>(now_ms % 800U) * (2.0f * PI_F / 800.0f));
      int core_r = 22 + static_cast<int>(4.0f * core_pulse);

      float color_t = static_cast<float>(now_ms % 3000U) * (1.0f / 3000.0f);
      esphome::Color aura_col = is_solaris ? col_solaris_gold : get_va_radiation_color(false, color_t);
      esphome::Color body_col = is_solaris ? esphome::Color(250, 100, 15) : get_va_radiation_color(false, color_t + 0.33f);

      it.filled_circle(cx, cy + 3, core_r + 4, col_dark_gray);
      it.circle(cx, cy, core_r + 4, aura_col);
      it.circle(cx, cy, core_r + 3, aura_col);
      it.circle(cx, cy, core_r + 2, aura_col);
      it.filled_circle(cx, cy, core_r, body_col);

      if (core_r > 6) {
        esphome::Color inner_tone = is_solaris ? col_solaris_gold : esphome::Color(
            static_cast<uint8_t>(body_col.r * 0.82f + 35),
            static_cast<uint8_t>(body_col.g * 0.82f + 35),
            static_cast<uint8_t>(body_col.b * 0.82f + 35));
        it.circle(cx, cy - 1, core_r - 3, inner_tone);
      }
      return;
    }

    if (is_thinking) {
      it.circle(cx, cy, 68, col_dark_gray);
      it.circle(cx, cy, 114, col_dark_gray);

      float spin_cw = static_cast<float>(now_ms % 1100U) * (2.0f * PI_F / 1100.0f);
      float s0 = sinf(spin_cw), c0 = cosf(spin_cw);

      esphome::Color inner_star_col = is_solaris ? col_solaris_gold : col_cyan;
      esphome::Color outer_star_col = is_solaris ? esphome::Color(35, 210, 110) : col_purple;
      esphome::Color outer_star_body = is_solaris ? esphome::Color(35, 210, 110) : col_magenta;
      esphome::Color outer_star_aura = is_solaris ? col_solaris_gold : col_purple;
      esphome::Color text_col = is_solaris ? col_solaris_gold : col_cyan;

      // 4 orthogonal points via 90-degree rotation of (c0, s0) -> only 1 sinf/cosf call!
      const float rot_c[4] = {c0, -s0, -c0, s0};
      const float rot_s[4] = {s0,  c0, -s0, -c0};
      for (int i = 0; i < 4; i++) {
        int nx = cx + static_cast<int>(68.0f * rot_c[i]);
        int ny = cy + static_cast<int>(68.0f * rot_s[i]);
        it.line(nx - 4, ny, nx + 4, ny, inner_star_col);
        it.line(nx, ny - 4, nx, ny + 4, inner_star_col);
        it.filled_circle(nx, ny, 3, inner_star_col);
        it.filled_circle(nx, ny, 1, col_white);
      }

      // Outer CCW orbit offset by +pi/4: cos(-a + pi/4) = (c0 + s0)*0.70710678f
      const float oc0 = (c0 + s0) * 0.70710678f;
      const float os0 = (c0 - s0) * 0.70710678f;
      const float orot_c[4] = {oc0, -os0, -oc0, os0};
      const float orot_s[4] = {os0,  oc0, -os0, -oc0};
      for (int i = 0; i < 4; i++) {
        int nx = cx + static_cast<int>(114.0f * orot_c[i]);
        int ny = cy + static_cast<int>(114.0f * orot_s[i]);
        it.line(nx - 5, ny, nx + 5, ny, outer_star_col);
        it.line(nx, ny - 5, nx, ny + 5, outer_star_col);
        it.filled_circle(nx, ny, 4, outer_star_body);
        it.filled_circle(nx, ny, 2, outer_star_aura);
        it.filled_circle(nx, ny, 1, col_white);
      }

      float pulse = sinf(static_cast<float>(now_ms % 600U) * (2.0f * PI_F / 600.0f));
      int core_r = 24 + static_cast<int>(5.0f * pulse);
      esphome::Color core_ring_col = is_solaris ? col_solaris_gold : col_purple;
      esphome::Color core_fill_col = is_solaris ? esphome::Color(250, 100, 15) : col_cyan;

      it.filled_circle(cx, cy + 3, core_r + 4, col_dark_gray);
      it.circle(cx, cy, core_r + 5, core_ring_col);
      it.circle(cx, cy, core_r + 4, core_ring_col);
      it.circle(cx, cy, core_r + 3, core_ring_col);
      it.filled_circle(cx, cy, core_r, core_fill_col);
      if (core_r > 6) {
        esphome::Color inner_tone = is_solaris ? col_solaris_gold : esphome::Color(30, 200, 230);
        it.circle(cx, cy - 1, core_r - 3, inner_tone);
      }
      it.print(cx, 401, font_small, text_col, esphome::display::TextAlign::CENTER, "Thinking...");
      return;
    }

    // Classic Listening
    float vad_boost = va_vad_active ? 6.0f : 0.0f;
    float pulse = sinf(static_cast<float>(now_ms % 1600U) * (2.0f * PI_F / 1600.0f));

    esphome::Color outer_halo_col = is_solaris ? esphome::Color(35, 210, 110) : col_purple;
    esphome::Color mid_ring_col = is_solaris ? col_solaris_gold : col_light_blue;
    esphome::Color mid_ring_inner = is_solaris ? col_solaris_gold : col_cyan;
    esphome::Color sat_col = is_solaris ? col_solaris_gold : col_cyan;
    esphome::Color sat_aura = is_solaris ? col_solaris_gold : col_light_blue;
    esphome::Color core_aura_col = is_solaris ? col_solaris_gold : col_purple;
    esphome::Color core_ring_col = is_solaris ? col_solaris_gold : col_light_blue;
    esphome::Color core_fill_col = is_solaris ? esphome::Color(250, 100, 15) : col_cyan;
    esphome::Color text_col = is_solaris ? col_solaris_gold : col_cyan;

    int outer_halo_r = 118 + static_cast<int>(8.0f * pulse + vad_boost);
    it.circle(cx, cy, outer_halo_r + 1, col_dark_gray);
    it.circle(cx, cy, outer_halo_r, outer_halo_col);
    it.circle(cx, cy, outer_halo_r - 1, outer_halo_col);

    int mid_ring_r = 74 + static_cast<int>(5.0f * pulse + vad_boost * 0.5f);
    it.circle(cx, cy, mid_ring_r + 1, col_dark_gray);
    it.circle(cx, cy, mid_ring_r, mid_ring_col);
    it.circle(cx, cy, mid_ring_r - 1, mid_ring_inner);

    float orbit_ang = static_cast<float>(now_ms % 3200U) * (2.0f * PI_F / 3200.0f);
    float s0 = sinf(orbit_ang), c0 = cosf(orbit_ang);
    const float rot_c[4] = {c0, -s0, -c0, s0};
    const float rot_s[4] = {s0,  c0, -s0, -c0};
    for (int i = 0; i < 4; i++) {
      int nx = cx + static_cast<int>(static_cast<float>(mid_ring_r) * rot_c[i]);
      int ny = cy + static_cast<int>(static_cast<float>(mid_ring_r) * rot_s[i]);
      it.line(nx - 3, ny, nx + 3, ny, sat_aura);
      it.line(nx, ny - 3, nx, ny + 3, sat_aura);
      it.filled_circle(nx, ny, 4, sat_col);
      it.filled_circle(nx, ny, 2, col_white);
    }

    int core_r = 26 + static_cast<int>(5.0f * pulse + vad_boost);
    int aura_r = 44 + static_cast<int>(8.0f * pulse + vad_boost * 1.2f);
    it.circle(cx, cy, aura_r + 2, col_dark_gray);
    it.circle(cx, cy, aura_r + 1, core_aura_col);
    it.circle(cx, cy, aura_r, core_aura_col);
    it.circle(cx, cy, aura_r - 1, core_aura_col);

    it.filled_circle(cx, cy + 3, core_r + 4, col_dark_gray);
    it.circle(cx, cy, core_r + 4, core_ring_col);
    it.circle(cx, cy, core_r + 3, core_ring_col);
    it.circle(cx, cy, core_r + 2, core_ring_col);
    it.filled_circle(cx, cy, core_r, core_fill_col);
    if (core_r > 6) {
      esphome::Color inner_tone = is_solaris ? col_solaris_gold : esphome::Color(30, 200, 230);
      it.circle(cx, cy - 1, core_r - 3, inner_tone);
    }
    const char *status_str = va_status_text.empty() ? "Listening..." : va_status_text.c_str();
    it.print(cx, 401, font_small, text_col, esphome::display::TextAlign::CENTER, status_str);
    return;
  }

  // ============================================================================
  // B. NEW FLAGSHIP "NEBULA" — 3D MULTI-RIBBON SILK WAVE ORB (Siri / Gemini Style)
  //    Uses 60-entry .rodata DIAL_SIN / DIAL_COS LUT for zero-sinf ribbon meshes!
  // ============================================================================
  if (!is_solaris) {
    const esphome::Color c_elec_cyan(0, 229, 255);
    const esphome::Color c_royal_indigo(115, 65, 255);
    const esphome::Color c_neon_magenta(255, 45, 145);
    const esphome::Color c_deep_azure(20, 95, 195);
    const esphome::Color c_ice_white(215, 248, 255);

    // Helper: Draws a 60-segment closed harmonic silk ribbon using DIAL_SIN/DIAL_COS LUT
    auto draw_silk_ribbon = [&](float base_r, float a2, float a3, float a5,
                                int ph2, int ph3, int ph5,
                                esphome::Color col_main, esphome::Color col_glow,
                                bool double_stroke) {
      int prev_x = 0, prev_y = 0, first_x = 0, first_y = 0;
      int prev_gx = 0, prev_gy = 0, first_gx = 0, first_gy = 0;
      for (int i = 0; i < 60; i++) {
        float w2 = DIAL_SIN[(i * 2 + ph2) % 60];
        float w3 = DIAL_COS[(i * 3 + ph3) % 60];
        float w5 = DIAL_SIN[(i * 5 + ph5) % 60];
        float r = base_r + a2 * w2 + a3 * w3 + a5 * w5;

        float ux = DIAL_SIN[i];
        float uy = -DIAL_COS[i];
        int x = cx + static_cast<int>(r * ux);
        int y = cy + static_cast<int>(r * uy);
        int gx = cx + static_cast<int>((r + 2.5f) * ux);
        int gy = cy + static_cast<int>((r + 2.5f) * uy);

        if (i == 0) {
          first_x = x; first_y = y;
          first_gx = gx; first_gy = gy;
        } else {
          if (double_stroke) {
            it.line(prev_gx, prev_gy, gx, gy, col_glow);
          }
          it.line(prev_x, prev_y, x, y, col_main);
        }
        prev_x = x; prev_y = y;
        prev_gx = gx; prev_gy = gy;
      }
      if (double_stroke) {
        it.line(prev_gx, prev_gy, first_gx, first_gy, col_glow);
      }
      it.line(prev_x, prev_y, first_x, first_y, col_main);
    };

    if (is_speaking) {
      // Natural syllabic speech-cadence envelope (combines 3 incommensurate frequencies)
      float t_sec = static_cast<float>(now_ms % 60000U) * 0.001f;
      float syl_env = 0.55f + 0.25f * sinf(t_sec * 11.5f) + 0.20f * cosf(t_sec * 19.3f);
      if (syl_env < 0.15f) syl_env = 0.15f;

      int tick = static_cast<int>(now_ms / 35U);

      // 3 Expanding Organic Acoustic Wave Contours (moiré-free 60-chord silk pressure rings)
      for (int w = 0; w < 3; w++) {
        uint32_t wave_t = (now_ms + static_cast<uint32_t>(w) * 600U) % 1800U;
        float prog = static_cast<float>(wave_t) * (1.0f / 1800.0f);
        float fade = (1.0f - prog) * (0.45f + 0.45f * syl_env);
        float r_wave = 68.0f + prog * 142.0f;
        float amp = (1.0f - prog * 0.5f) * (6.0f + 8.0f * syl_env);

        esphome::Color w_col = scale_color(get_va_radiation_color(false, prog + static_cast<float>(w) * 0.33f + t_sec * 0.25f), fade);
        esphome::Color w_glow = scale_color(w_col, 0.45f);
        draw_silk_ribbon(r_wave, amp, amp * 0.6f, amp * 0.3f,
                         (tick + w * 19) % 60, (tick * 2 + w * 11) % 60, (tick + w * 7) % 60,
                         w_col, w_glow, true);
      }

      // 4 Interweaving Inner Silk Voice Ribbons (Siri / Gemini multi-lobe acoustic membrane)
      float base_r = 52.0f + 12.0f * syl_env;
      float a2 = 7.0f + 11.0f * syl_env;
      float a3 = 5.0f + 8.0f * syl_env;
      float a5 = 2.5f + 4.5f * syl_env;

      draw_silk_ribbon(base_r + 4.0f, a2, a3, a5, (tick) % 60, (tick * 2 + 15) % 60, (tick + 30) % 60,
                       c_royal_indigo, scale_color(c_royal_indigo, 0.4f), true);
      draw_silk_ribbon(base_r, -a2 * 0.9f, a3 * 1.1f, -a5, (tick + 20) % 60, (tick + 40) % 60, (tick * 2) % 60,
                       c_neon_magenta, scale_color(c_neon_magenta, 0.4f), true);
      draw_silk_ribbon(base_r - 2.0f, a2 * 0.8f, -a3, a5 * 1.2f, (tick * 2 + 7) % 60, (tick + 25) % 60, (tick + 12) % 60,
                       c_elec_cyan, c_deep_azure, true);
      draw_silk_ribbon(base_r - 5.0f, a2 * 0.5f, a3 * 0.5f, 0.0f, (tick + 35) % 60, (tick + 10) % 60, 0,
                       c_ice_white, c_elec_cyan, false);

      // Luminous Layered Glass Core
      int core_r = 22 + static_cast<int>(7.0f * syl_env);
      it.filled_circle(cx, cy, core_r + 5, scale_color(c_royal_indigo, 0.45f));
      it.filled_circle(cx, cy, core_r, c_deep_azure);
      it.circle(cx, cy, core_r + 1, c_elec_cyan);
      it.circle(cx, cy, core_r, c_ice_white);
      it.filled_circle(cx - 4, cy - 4, core_r / 3, c_ice_white);
      it.print(cx, 401, font_small, c_elec_cyan, esphome::display::TextAlign::CENTER, "Responding...");
      return;
    }

    if (is_thinking) {
      int tick = static_cast<int>(now_ms / 25U);
      float pulse = sinf(static_cast<float>(now_ms % 700U) * (2.0f * PI_F / 700.0f));

      // Tightly wound high-velocity inner silk vortex
      draw_silk_ribbon(50.0f + 3.0f * pulse, 6.0f, 5.0f, 3.0f,
                       (tick * 2) % 60, (tick * 3) % 60, (tick) % 60,
                       c_royal_indigo, scale_color(c_royal_indigo, 0.4f), true);
      draw_silk_ribbon(48.0f - 2.0f * pulse, -5.0f, 6.0f, -2.5f,
                       (tick * 2 + 30) % 60, (tick + 15) % 60, (tick * 2) % 60,
                       c_neon_magenta, scale_color(c_neon_magenta, 0.4f), false);
      draw_silk_ribbon(46.0f, 4.0f, -4.0f, 2.0f,
                       (tick + 45) % 60, (tick * 2 + 20) % 60, (tick * 3) % 60,
                       c_elec_cyan, c_deep_azure, true);

      // 6 Relativistic Orbital Neural Nodes with 5-Step Fading Comet Trails (LUT-driven)
      const int head_idx_cw = static_cast<int>((now_ms / 18U) % 60U);
      const int head_idx_ccw = (60 - static_cast<int>((now_ms / 22U) % 60U)) % 60;

      for (int n = 0; n < 3; n++) {
        int base_cw = (head_idx_cw + n * 20) % 60;
        for (int tail = 4; tail >= 0; tail--) {
          int idx = (base_cw - tail * 2 + 60) % 60;
          int nx = cx + static_cast<int>(84.0f * DIAL_SIN[idx]);
          int ny = cy - static_cast<int>(84.0f * DIAL_COS[idx]);
          float k = static_cast<float>(5 - tail) * 0.20f;
          it.filled_circle(nx, ny, (tail == 0) ? 4 : (tail <= 2 ? 2 : 1),
                           (tail == 0) ? c_ice_white : scale_color(c_elec_cyan, k));
        }

        int base_ccw = (head_idx_ccw + n * 20 + 10) % 60;
        for (int tail = 4; tail >= 0; tail--) {
          int idx = (base_ccw + tail * 2) % 60;
          int nx = cx + static_cast<int>(114.0f * DIAL_SIN[idx]);
          int ny = cy - static_cast<int>(114.0f * DIAL_COS[idx]);
          float k = static_cast<float>(5 - tail) * 0.20f;
          it.filled_circle(nx, ny, (tail == 0) ? 4 : (tail <= 2 ? 2 : 1),
                           (tail == 0) ? c_ice_white : scale_color(c_neon_magenta, k));
        }
      }

      // Cognitive Core
      int core_r = 22 + static_cast<int>(4.0f * pulse);
      it.filled_circle(cx, cy, core_r + 4, scale_color(c_neon_magenta, 0.35f));
      it.filled_circle(cx, cy, core_r, c_royal_indigo);
      it.circle(cx, cy, core_r + 1, c_elec_cyan);
      it.filled_circle(cx, cy, core_r / 2, c_ice_white);
      it.print(cx, 401, font_small, c_elec_cyan, esphome::display::TextAlign::CENTER, "Thinking...");
      return;
    }

    // New Nebula Listening: Fluid Breathing Silk Aura + Smooth VAD Bloom
    float breath = sinf(static_cast<float>(now_ms % 2000U) * (2.0f * PI_F / 2000.0f));
    int tick = static_cast<int>(now_ms / 45U);

    float base_r = 76.0f + 6.0f * breath + 22.0f * smooth_vad;
    float a2 = 5.0f + 10.0f * smooth_vad;
    float a3 = 4.0f + 8.0f * smooth_vad;
    float a5 = 2.0f + 5.0f * smooth_vad;

    // Subtle outer attentive containment halo
    int outer_r = 122 + static_cast<int>(5.0f * breath + 12.0f * smooth_vad);
    it.circle(cx, cy, outer_r, scale_color(c_royal_indigo, 0.45f + 0.35f * smooth_vad));
    it.circle(cx, cy, outer_r - 1, scale_color(c_elec_cyan, 0.25f + 0.35f * smooth_vad));

    // 4 Fluid Silk Ribbons
    draw_silk_ribbon(base_r + 5.0f, a2, a3, a5, (tick) % 60, (tick + 20) % 60, (tick + 40) % 60,
                     c_royal_indigo, scale_color(c_royal_indigo, 0.35f), true);
    draw_silk_ribbon(base_r, -a2, a3 * 0.9f, -a5, (tick + 15) % 60, (tick * 2) % 60, (tick + 10) % 60,
                     c_neon_magenta, scale_color(c_neon_magenta, 0.35f), true);
    draw_silk_ribbon(base_r - 4.0f, a2 * 0.8f, -a3, a5, (tick + 35) % 60, (tick + 12) % 60, (tick * 2) % 60,
                     c_elec_cyan, c_deep_azure, true);
    draw_silk_ribbon(base_r - 8.0f, a2 * 0.4f, a3 * 0.4f, 0.0f, (tick + 50) % 60, (tick + 30) % 60, 0,
                     c_ice_white, c_elec_cyan, false);

    // Living Glass Core
    int core_r = 25 + static_cast<int>(4.0f * breath + 8.0f * smooth_vad);
    it.filled_circle(cx, cy, core_r + 5, scale_color(c_royal_indigo, 0.40f));
    it.filled_circle(cx, cy, core_r, lerp_color(c_deep_azure, c_elec_cyan, smooth_vad * 0.5f));
    it.circle(cx, cy, core_r + 1, c_elec_cyan);
    it.circle(cx, cy, core_r, c_ice_white);
    it.filled_circle(cx - 4, cy - 4, core_r / 3, c_ice_white);

    const char *status_str = va_status_text.empty() ? "Listening..." : va_status_text.c_str();
    it.print(cx, 401, font_small, c_elec_cyan, esphome::display::TextAlign::CENTER, status_str);
    return;
  }

  // ============================================================================
  // C. NEW FLAGSHIP "SOLARIS" — RADIAL ACOUSTIC CORONA & SOLAR ASTROLABE
  //    60-Ray Circular Spectrometer + Golden/Emerald Coronal Waves (LUT-driven)
  // ============================================================================
  const esphome::Color c_gold = col_solaris_gold;                 // #FFDC1E
  const esphome::Color c_amber(255, 105, 15);                     // Molten Solar Amber
  const esphome::Color c_crimson_ember(185, 35, 10);              // Deep Corona Crimson
  const esphome::Color c_emerald(35, 215, 115);                   // Solar Aurora Emerald
  const esphome::Color c_warm_white(255, 248, 215);               // Photosphere White-Gold

  if (is_speaking) {
    float t_sec = static_cast<float>(now_ms % 60000U) * 0.001f;
    float syl_env = 0.55f + 0.25f * sinf(t_sec * 11.5f) + 0.20f * cosf(t_sec * 19.3f);
    if (syl_env < 0.15f) syl_env = 0.15f;
    int tick = static_cast<int>(now_ms / 35U);

    // 3 Expanding Coronal Shockwave Rings (moiré-free 2-pass glowing rings)
    for (int w = 0; w < 3; w++) {
      uint32_t wave_t = (now_ms + static_cast<uint32_t>(w) * 600U) % 1800U;
      float prog = static_cast<float>(wave_t) * (1.0f / 1800.0f);
      float fade = (1.0f - prog) * (0.45f + 0.45f * syl_env);
      int r_wave = 82 + static_cast<int>(prog * 132.0f);
      esphome::Color ring_c = scale_color(get_va_radiation_color(true, prog + static_cast<float>(w) * 0.33f), fade);
      esphome::Color glow_c = scale_color(ring_c, 0.45f);
      it.circle(cx, cy, r_wave - 1, glow_c);
      it.circle(cx, cy, r_wave, ring_c);
      it.circle(cx, cy, r_wave + 1, glow_c);
    }

    // 60-Ray Circular Acoustic Spectrometer around the Solar Core
    const float r_inner = 42.0f;
    for (int i = 0; i < 60; i++) {
      float h1 = DIAL_SIN[(i * 3 + tick * 2) % 60];
      float h2 = DIAL_COS[(i * 5 + tick * 3) % 60];
      float bar_len = (14.0f + 28.0f * syl_env) * (0.55f + 0.28f * h1 + 0.17f * h2);
      float r_outer = r_inner + bar_len;

      float ux = DIAL_SIN[i];
      float uy = -DIAL_COS[i];
      int x0 = cx + static_cast<int>(r_inner * ux);
      int y0 = cy + static_cast<int>(r_inner * uy);
      int xm = cx + static_cast<int>((r_inner + bar_len * 0.65f) * ux);
      int ym = cy + static_cast<int>((r_inner + bar_len * 0.65f) * uy);
      int x1 = cx + static_cast<int>(r_outer * ux);
      int y1 = cy + static_cast<int>(r_outer * uy);

      it.line(x0, y0, xm, ym, c_amber);
      it.line(xm, ym, x1, y1, (i % 2 == 0) ? c_gold : c_emerald);
    }

    // Pulsing Photosphere Core
    int core_r = 26 + static_cast<int>(6.0f * syl_env);
    it.filled_circle(cx, cy, core_r + 6, c_crimson_ember);
    it.filled_circle(cx, cy, core_r + 2, c_amber);
    it.filled_circle(cx, cy, core_r - 3, c_gold);
    it.circle(cx, cy, core_r - 5, c_warm_white);
    it.print(cx, 401, font_small, c_gold, esphome::display::TextAlign::CENTER, "Responding...");
    return;
  }

  if (is_thinking) {
    // 3-Ring Counter-Rotating Solar Astrolabe with Comet Trails
    const int head_1 = static_cast<int>((now_ms / 18U) % 60U);
    const int head_2 = (60 - static_cast<int>((now_ms / 22U) % 60U)) % 60;
    const int head_3 = static_cast<int>((now_ms / 28U) % 60U);

    // Subtle astrolabe meridian rings
    it.circle(cx, cy, 62, scale_color(c_amber, 0.30f));
    it.circle(cx, cy, 90, scale_color(c_gold, 0.25f));
    it.circle(cx, cy, 118, scale_color(c_emerald, 0.25f));

    for (int n = 0; n < 3; n++) {
      // Inner Ring (r = 62, CW, Gold)
      int b1 = (head_1 + n * 20) % 60;
      for (int tail = 5; tail >= 0; tail--) {
        int idx = (b1 - tail * 2 + 60) % 60;
        int nx = cx + static_cast<int>(62.0f * DIAL_SIN[idx]);
        int ny = cy - static_cast<int>(62.0f * DIAL_COS[idx]);
        it.filled_circle(nx, ny, (tail == 0) ? 4 : (tail <= 2 ? 2 : 1),
                         (tail == 0) ? c_warm_white : scale_color(c_gold, (6 - tail) * 0.16f));
      }
      // Middle Ring (r = 90, CCW, Emerald)
      int b2 = (head_2 + n * 20 + 10) % 60;
      for (int tail = 5; tail >= 0; tail--) {
        int idx = (b2 + tail * 2) % 60;
        int nx = cx + static_cast<int>(90.0f * DIAL_SIN[idx]);
        int ny = cy - static_cast<int>(90.0f * DIAL_COS[idx]);
        it.filled_circle(nx, ny, (tail == 0) ? 4 : (tail <= 2 ? 2 : 1),
                         (tail == 0) ? c_warm_white : scale_color(c_emerald, (6 - tail) * 0.16f));
      }
      // Outer Ring (r = 118, CW, Amber)
      int b3 = (head_3 + n * 20 + 5) % 60;
      for (int tail = 4; tail >= 0; tail--) {
        int idx = (b3 - tail * 2 + 60) % 60;
        int nx = cx + static_cast<int>(118.0f * DIAL_SIN[idx]);
        int ny = cy - static_cast<int>(118.0f * DIAL_COS[idx]);
        it.filled_circle(nx, ny, (tail == 0) ? 3 : 2,
                         (tail == 0) ? c_gold : scale_color(c_amber, (5 - tail) * 0.18f));
      }
    }

    float pulse = sinf(static_cast<float>(now_ms % 600U) * (2.0f * PI_F / 600.0f));
    int core_r = 25 + static_cast<int>(4.0f * pulse);
    it.filled_circle(cx, cy, core_r + 5, c_crimson_ember);
    it.filled_circle(cx, cy, core_r + 1, c_amber);
    it.filled_circle(cx, cy, core_r - 3, c_gold);
    it.circle(cx, cy, core_r - 5, c_warm_white);
    it.print(cx, 401, font_small, c_gold, esphome::display::TextAlign::CENTER, "Thinking...");
    return;
  }

  // New Solaris Listening: Breathing 60-Ray Solar Corona + Emerald-Gold VAD Bloom
  float breath = sinf(static_cast<float>(now_ms % 1800U) * (2.0f * PI_F / 1800.0f));
  int tick = static_cast<int>(now_ms / 45U);

  int outer_r = 120 + static_cast<int>(6.0f * breath + 12.0f * smooth_vad);
  it.circle(cx, cy, outer_r, scale_color(c_emerald, 0.45f + 0.45f * smooth_vad));
  it.circle(cx, cy, outer_r - 1, scale_color(c_gold, 0.30f + 0.40f * smooth_vad));

  const float r_inner = 42.0f + 3.0f * breath;
  for (int i = 0; i < 60; i++) {
    float wave = DIAL_SIN[(i * 3 + tick) % 60] * 0.6f + DIAL_COS[(i * 2 + tick) % 60] * 0.4f;
    float bar_len = (16.0f + 6.0f * breath + 24.0f * smooth_vad) + (6.0f + 10.0f * smooth_vad) * wave;
    if (bar_len < 6.0f) bar_len = 6.0f;

    float ux = DIAL_SIN[i];
    float uy = -DIAL_COS[i];
    int x0 = cx + static_cast<int>(r_inner * ux);
    int y0 = cy + static_cast<int>(r_inner * uy);
    int xm = cx + static_cast<int>((r_inner + bar_len * 0.65f) * ux);
    int ym = cy + static_cast<int>((r_inner + bar_len * 0.65f) * uy);
    int x1 = cx + static_cast<int>((r_inner + bar_len) * ux);
    int y1 = cy + static_cast<int>((r_inner + bar_len) * uy);

    it.line(x0, y0, xm, ym, c_amber);
    it.line(xm, ym, x1, y1, (smooth_vad > 0.25f && (i % 2 == 0)) ? c_emerald : c_gold);
  }

  int core_r = 27 + static_cast<int>(4.0f * breath + 7.0f * smooth_vad);
  it.filled_circle(cx, cy, core_r + 5, c_crimson_ember);
  it.filled_circle(cx, cy, core_r + 1, c_amber);
  it.filled_circle(cx, cy, core_r - 3, c_gold);
  it.circle(cx, cy, core_r - 5, c_warm_white);

  const char *status_str = va_status_text.empty() ? "Listening..." : va_status_text.c_str();
  it.print(cx, 401, font_small, c_gold, esphome::display::TextAlign::CENTER, status_str);
}

// ------------------------------------------------------------------------------
// 6. Weather Suite Formatters, Simulation Override & Forecast Glyph Engine
// ------------------------------------------------------------------------------
inline std::string format_iso_time(const std::string &iso_str, bool is_24h) {
  if (iso_str.empty() || iso_str == "unknown" || iso_str == "unavailable") return "--:--";
  int yr = 0, mo = 0, dy = 0, hr = 0, mn = 0, sc = 0;
  if (sscanf(iso_str.c_str(), "%d-%d-%dT%d:%d:%d", &yr, &mo, &dy, &hr, &mn, &sc) < 5) return "--:--";
  int y = yr - (mo <= 2 ? 1 : 0);
  int era = (y >= 0 ? y : y - 399) / 400;
  int yoe = y - era * 400;
  int doy = (153 * (mo > 2 ? mo - 3 : mo + 9) + 2) / 5 + dy - 1;
  int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
  long long days = (long long)era * 146097 + doe - 719468;
  time_t t = (time_t)(days * 86400LL + hr * 3600LL + mn * 60LL + sc);
  struct tm tm_loc = {};
  localtime_r(&t, &tm_loc);
  char buf[16];
  if (is_24h) {
    snprintf(buf, sizeof(buf), "%02d:%02d", tm_loc.tm_hour, tm_loc.tm_min);
  } else {
    int h12 = tm_loc.tm_hour % 12;
    if (h12 == 0) h12 = 12;
    snprintf(buf, sizeof(buf), "%d:%02d %s", h12, tm_loc.tm_min, tm_loc.tm_hour >= 12 ? "PM" : "AM");
  }
  return std::string(buf);
}

inline const char *get_cardinal(float deg) {
  if (std::isnan(deg)) return "";
  deg = fmodf(deg, 360.0f);
  if (deg < 0) deg += 360.0f;
  if (deg >= 337.5f || deg < 22.5f) return "N";
  if (deg < 67.5f) return "NE";
  if (deg < 112.5f) return "E";
  if (deg < 157.5f) return "SE";
  if (deg < 202.5f) return "S";
  if (deg < 247.5f) return "SW";
  if (deg < 292.5f) return "W";
  return "NW";
}

template<typename CurT>
inline void apply_weather_sim_override(const CurT &sim_opt, std::string &cond, bool &is_night) {
  if (sim_opt.empty() || sim_opt == "Live (Home Assistant)") return;
  if (sim_opt == "Clear Night") { cond = "clear-night"; is_night = true; }
  else if (sim_opt == "Sunny / Clear Day") { cond = "sunny"; is_night = false; }
  else if (sim_opt == "Partly Cloudy (Day)") { cond = "partlycloudy"; is_night = false; }
  else if (sim_opt == "Partly Cloudy (Night)") { cond = "partlycloudy"; is_night = true; }
  else if (sim_opt == "Cloudy / Overcast (Day)") { cond = "cloudy"; is_night = false; }
  else if (sim_opt == "Cloudy / Overcast (Night)") { cond = "cloudy"; is_night = true; }
  else if (sim_opt == "Rainy (Day)") { cond = "rainy"; is_night = false; }
  else if (sim_opt == "Rainy (Night)") { cond = "rainy"; is_night = true; }
  else if (sim_opt == "Pouring / Heavy Rain") { cond = "pouring"; is_night = false; }
  else if (sim_opt == "Snow (Day)") { cond = "snowy"; is_night = false; }
  else if (sim_opt == "Snow (Night)") { cond = "snowy"; is_night = true; }
  else if (sim_opt == "Thunderstorm (Day)") { cond = "lightning-rainy"; is_night = false; }
  else if (sim_opt == "Thunderstorm (Night)") { cond = "lightning-rainy"; is_night = true; }
  else if (sim_opt == "Windy") { cond = "windy"; is_night = false; }
}

inline std::string format_weather_title(const std::string &c_in) {
  if (c_in.empty() || c_in == "unknown" || c_in == "unavailable") return "Unavailable";
  if (c_in == "partlycloudy" || c_in == "partly-cloudy") return "PARTLY CLOUDY";
  if (c_in == "mostlycloudy" || c_in == "mostly-cloudy") return "MOSTLY CLOUDY";
  if (c_in == "mostlysunny" || c_in == "mostly-sunny") return "MOSTLY SUNNY";
  if (c_in == "partlysunny" || c_in == "partly-sunny") return "PARTLY SUNNY";
  if (c_in == "clear-night" || c_in == "clearnight") return "CLEAR NIGHT";
  if (c_in == "clear-day" || c_in == "clearday" || c_in == "sunny") return "SUNNY";
  if (c_in == "lightning-rainy" || c_in == "thunderstorm") return "THUNDERSTORM";
  if (c_in == "lightning") return "LIGHTNING";
  if (c_in == "snowy-rainy" || c_in == "sleet") return "SLEET / MIX";
  if (c_in == "windy-variant" || c_in == "windy" || c_in == "breezy") return "WINDY";
  if (c_in == "pouring" || c_in == "heavy-rain") return "HEAVY RAIN";
  if (c_in == "exceptional") return "SEVERE ALERT";
  std::string s = c_in;
  for (char &c : s) {
    if (c == '-' || c == '_') c = ' ';
    else c = toupper(c);
  }
  return s;
}

inline std::string format_forecast_short_name(const std::string &c) {
  if (c == "partlycloudy" || c == "partly-cloudy") return "P.Cloudy";
  if (c == "mostlycloudy" || c == "mostly-cloudy") return "M.Cloudy";
  if (c == "mostlysunny" || c == "mostly-sunny") return "M.Sunny";
  if (c == "partlysunny" || c == "partly-sunny") return "P.Sunny";
  if (c == "clear-night" || c == "clearnight") return "Clear";
  if (c == "sunny" || c == "clear-day" || c == "clearday" || c == "clear") return "Sunny";
  if (c == "rainy" || c == "pouring" || c == "heavy-rain") return "Rain";
  if (c == "cloudy") return "Cloudy";
  if (c == "snowy") return "Snow";
  if (c == "lightning" || c == "lightning-rainy" || c == "thunderstorm") return "Storm";
  if (c == "snowy-rainy" || c == "sleet") return "Sleet";
  if (c == "windy" || c == "windy-variant" || c == "breezy") return "Windy";
  if (c == "fog") return "Fog";
  if (c == "hail") return "Hail";
  if (c.empty()) return "Fair";
  std::string s = c;
  for (char &ch : s) {
    if (ch == '-' || ch == '_') ch = ' ';
  }
  s[0] = toupper(s[0]);
  return s;
}

template<typename DisplayT>
inline void render_forecast_glyph(DisplayT &it, int gx, int gy, const std::string &c_name) {
  if (c_name == "sunny" || c_name == "clear-day" || c_name == "clear") {
    it.filled_circle(gx, gy, 8, esphome::Color(250, 185, 30));
    it.circle(gx, gy, 11, esphome::Color(255, 215, 80));
  } else if (c_name == "clear-night") {
    it.filled_circle(gx + 2, gy, 8, esphome::Color(225, 235, 250));
    it.filled_circle(gx, gy, 7, esphome::Color(0, 0, 0));
  } else if (c_name == "partlycloudy" || c_name == "partly-cloudy" || c_name == "mostlycloudy" || c_name == "mostlysunny" || c_name == "partlysunny") {
    it.filled_circle(gx - 4, gy - 4, 6, esphome::Color(245, 175, 30));
    it.filled_circle(gx + 2, gy + 1, 6, esphome::Color(175, 190, 210));
    it.filled_circle(gx - 3, gy + 2, 5, esphome::Color(155, 170, 190));
    it.filled_circle(gx + 7, gy + 3, 4, esphome::Color(145, 160, 180));
  } else if (c_name == "rainy" || c_name == "pouring") {
    it.filled_circle(gx, gy - 2, 6, esphome::Color(130, 145, 170));
    it.filled_circle(gx - 5, gy - 1, 5, esphome::Color(115, 130, 155));
    it.filled_circle(gx + 5, gy, 4, esphome::Color(115, 130, 155));
    it.line(gx - 4, gy + 6, gx - 6, gy + 10, esphome::Color(70, 170, 245));
    it.line(gx + 2, gy + 6, gx, gy + 10, esphome::Color(70, 170, 245));
  } else if (c_name == "snowy" || c_name == "snowy-rainy") {
    it.filled_circle(gx, gy - 2, 6, esphome::Color(160, 175, 195));
    it.draw_pixel_at(gx - 3, gy + 7, esphome::Color(255, 255, 255));
    it.draw_pixel_at(gx + 3, gy + 7, esphome::Color(255, 255, 255));
    it.draw_pixel_at(gx, gy + 9, esphome::Color(255, 255, 255));
  } else if (c_name == "lightning" || c_name == "lightning-rainy") {
    it.filled_circle(gx, gy - 3, 6, esphome::Color(110, 120, 140));
    it.line(gx - 1, gy + 3, gx - 3, gy + 7, esphome::Color(255, 220, 40));
    it.line(gx - 3, gy + 7, gx + 1, gy + 7, esphome::Color(255, 220, 40));
    it.line(gx + 1, gy + 7, gx - 1, gy + 11, esphome::Color(255, 220, 40));
  } else if (c_name == "windy" || c_name == "windy-variant") {
    it.line(gx - 8, gy - 2, gx + 6, gy - 2, esphome::Color(160, 185, 210));
    it.line(gx - 6, gy + 2, gx + 8, gy + 2, esphome::Color(160, 185, 210));
    it.line(gx - 4, gy + 6, gx + 4, gy + 6, esphome::Color(140, 165, 190));
  } else {
    it.filled_circle(gx, gy - 1, 7, esphome::Color(165, 180, 200));
    it.filled_circle(gx - 5, gy + 1, 5, esphome::Color(145, 160, 180));
    it.filled_circle(gx + 5, gy + 1, 5, esphome::Color(145, 160, 180));
  }
}

}  // namespace pocket_ui

using namespace pocket_ui;
