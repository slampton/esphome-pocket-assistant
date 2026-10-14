#pragma once

// ==============================================================================
// 🧭 Pocket Assistant Reusable C++ UI, Geometry & Touch Toolkit (ui_toolkit.h)
// Target: 466x466 Circular AMOLED Display (Waveshare ESP32-S3-Touch-AMOLED-1.75C)
// Zero-exception (-fno-exceptions), zero-heap-churn embedded graphics & hit-testing
// ==============================================================================

#include "esphome.h"
#include <cmath>
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

// ------------------------------------------------------------------------------
// 3. Text, Time, SemVer & Synodic Astronomy Helpers
// ------------------------------------------------------------------------------
template<typename FontT>
inline std::string fit_text_to_width_font(const std::string &str, int max_w, FontT *font) {
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

}  // namespace pocket_ui

using namespace pocket_ui;
