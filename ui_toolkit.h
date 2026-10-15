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

// ------------------------------------------------------------------------------
// 1B. Universal Fast-Math, Circular Clipping, 3D Projection & Shader Primitives
// ------------------------------------------------------------------------------

// O(1) Circular AMOLED Bezel Half-Span Lookup (returns half-chord width/height at row/col `c`)
inline int circle_half_span(int c, int inset = 5) {
  if (c < 0 || c >= 466) return 0;
  int span = static_cast<int>(CIRCLE_HW[c]) - inset;
  return (span > 0) ? span : 0;
}

inline bool is_inside_bezel(int x, int y, int cy = 233, int inset = 5) {
  int h = circle_half_span(x, inset);
  return (h > 0) && (y >= cy - h) && (y <= cy + h);
}

// Fast interpolated sine/cosine backed by .rodata DIAL_SIN[60] (0 hardware FPU trig stalls)
inline float fast_sin(float rad) {
  float idx_f = rad * (60.0f / (2.0f * PI_F));
  int idx_i = static_cast<int>(idx_f);
  if (idx_f < 0.0f && static_cast<float>(idx_i) != idx_f) idx_i--;
  float frac = idx_f - static_cast<float>(idx_i);
  int i0 = idx_i % 60;
  if (i0 < 0) i0 += 60;
  int i1 = (i0 + 1 == 60) ? 0 : (i0 + 1);
  return DIAL_SIN[i0] + (DIAL_SIN[i1] - DIAL_SIN[i0]) * frac;
}

inline float fast_cos(float rad) {
  return fast_sin(rad + (PI_F * 0.5f));
}

inline void fast_sincos(float rad, float &out_s, float &out_c) {
  out_s = fast_sin(rad);
  out_c = fast_cos(rad);
}

// Lightweight 3D Vector, Euler Rotation & Perspective Projection Primitives
struct Vec3 {
  float x, y, z;
};

struct ProjectedPoint {
  int x, y;
  float scale;
  bool visible;
};

inline Vec3 rotate_3d_xyz(Vec3 v, float rx, float ry, float rz) {
  float sx, cx_r, sy, cy_r, sz, cz_r;
  fast_sincos(rx, sx, cx_r);
  fast_sincos(ry, sy, cy_r);
  fast_sincos(rz, sz, cz_r);

  // Rotate around X
  float y1 = v.y * cx_r - v.z * sx;
  float z1 = v.y * sx + v.z * cx_r;
  // Rotate around Y
  float x2 = v.x * cy_r + z1 * sy;
  float z2 = -v.x * sy + z1 * cy_r;
  // Rotate around Z
  float x3 = x2 * cz_r - y1 * sz;
  float y3 = x2 * sz + y1 * cz_r;
  return {x3, y3, z2};
}

inline ProjectedPoint project_3d(Vec3 v, int cx, int cy, float fov = 240.0f, float viewer_dist = 320.0f) {
  float depth = v.z + viewer_dist;
  if (depth <= 15.0f) return {cx, cy, 0.0f, false};
  float k = fov / depth;
  int sx = cx + static_cast<int>(v.x * k);
  int sy = cy + static_cast<int>(v.y * k);
  return {sx, sy, k, is_inside_bezel(sx, sy, cy, 4)};
}

// Deterministic Zero-RAM 2D Hash & Smooth Value Noise (0.0f .. 1.0f) for procedural shaders
inline float hash2d_01(int x, int y) {
  uint32_t n = static_cast<uint32_t>(x) * 374761393U + static_cast<uint32_t>(y) * 668265263U;
  n = (n ^ (n >> 13)) * 1274126177U;
  return static_cast<float>((n ^ (n >> 16)) & 0xFFFFU) * (1.0f / 65535.0f);
}

inline float value_noise_2d(float x, float y) {
  int xi = static_cast<int>(x);
  if (x < 0.0f && static_cast<float>(xi) != x) xi--;
  int yi = static_cast<int>(y);
  if (y < 0.0f && static_cast<float>(yi) != y) yi--;
  float xf = x - static_cast<float>(xi);
  float yf = y - static_cast<float>(yi);
  float u = xf * xf * (3.0f - 2.0f * xf);
  float v = yf * yf * (3.0f - 2.0f * yf);
  float a = hash2d_01(xi, yi);
  float b = hash2d_01(xi + 1, yi);
  float c = hash2d_01(xi, yi + 1);
  float d = hash2d_01(xi + 1, yi + 1);
  return (a + (b - a) * u) + ((c - a) + (a - b - c + d) * u) * v;
}

// Fast HSV-to-RGB (h in [0..1], s in [0..1], v in [0..1]) & Saturating Additive Blend
inline esphome::Color hsv_to_rgb(float h, float s, float v) {
  h = h - static_cast<float>(static_cast<int>(h));
  if (h < 0.0f) h += 1.0f;
  float r = v, g = v, b = v;
  if (s > 0.0f) {
    float h6 = h * 6.0f;
    int sector = static_cast<int>(h6);
    float f = h6 - static_cast<float>(sector);
    float p = v * (1.0f - s);
    float q = v * (1.0f - s * f);
    float t = v * (1.0f - s * (1.0f - f));
    switch (sector % 6) {
      case 0: r = v; g = t; b = p; break;
      case 1: r = q; g = v; b = p; break;
      case 2: r = p; g = v; b = t; break;
      case 3: r = p; g = q; b = v; break;
      case 4: r = t; g = p; b = v; break;
      default: r = v; g = p; b = q; break;
    }
  }
  return esphome::Color(
      static_cast<uint8_t>(r * 255.0f),
      static_cast<uint8_t>(g * 255.0f),
      static_cast<uint8_t>(b * 255.0f));
}

inline esphome::Color blend_additive(esphome::Color a, esphome::Color b) {
  int r = static_cast<int>(a.r) + static_cast<int>(b.r);
  int g = static_cast<int>(a.g) + static_cast<int>(b.g);
  int bl = static_cast<int>(a.b) + static_cast<int>(b.b);
  return esphome::Color(
      static_cast<uint8_t>(r > 255 ? 255 : r),
      static_cast<uint8_t>(g > 255 ? 255 : g),
      static_cast<uint8_t>(bl > 255 ? 255 : bl));
}

template<typename DisplayT>
inline void draw_glow_line(DisplayT &it, int x0, int y0, int x1, int y1,
                           esphome::Color core_col, esphome::Color halo_col) {
  int dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
  int dy = (y1 > y0) ? (y1 - y0) : (y0 - y1);
  if (dx >= dy) {
    it.line(x0, y0 - 1, x1, y1 - 1, halo_col);
    it.line(x0, y0 + 1, x1, y1 + 1, halo_col);
  } else {
    it.line(x0 - 1, y0, x1 - 1, y1, halo_col);
    it.line(x0 + 1, y0, x1 + 1, y1, halo_col);
  }
  it.line(x0, y0, x1, y1, core_col);
}

// Universal Audio-Aware Ambient Animation Pacer (20 FPS Idle / ~13.3 FPS Audio-Protected):
// Shared across all Docked Screensavers, Sky & Weather motion wallpapers, and future Home Page
// live backgrounds. Unlocks full 20 FPS (50ms interval, 42ms coalesce guard against disp1's 1s timer)
// whenever local audio/alerts are idle, and automatically paces at 75ms (~13.3 FPS) during active
// local Sendspin FLAC playback so audio DMA and Octal PSRAM never contend.
inline uint32_t &last_ambient_anim_ms() {
  static uint32_t ts = 0;
  return ts;
}

inline uint32_t &last_screensaver_draw_ms() {
  return last_ambient_anim_ms();
}

inline void record_ambient_frame_ms(uint32_t now_ms) {
  last_ambient_anim_ms() = now_ms;
}

inline bool should_step_ambient_anim(uint32_t now_ms, bool local_audio_busy,
                                     bool va_busy = false, bool alert_busy = false) {
  if (va_busy) return false;
  const uint32_t min_gap = alert_busy ? 120U : (local_audio_busy ? 75U : 42U);
  return (now_ms - last_ambient_anim_ms() >= min_gap);
}

// Standardized Dynamic Scroll Easing Constants (5-Step Cubic Eased Curve)
static constexpr int16_t SHARED_SCROLL_EASE_116[5] = {0, 25, 63, 99, 116}; // 2-Pill 116px shift
static constexpr int16_t SHARED_SCROLL_EASE_174[5] = {0, 38, 95, 148, 174}; // 3-Pill 174px shift
static constexpr int16_t SHARED_SCROLL_EASE_232[5] = {0, 50, 126, 198, 232}; // 4-Pill 232px shift


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
// 64-entry vertical plume intensity LUT in .rodata Flash (0..256 fixed-point scale):
// Full-bodied Chunk 72 physical band thickness paired with Chunk 73 hue-preserving overlap blending:
// - Indices 0..7   (bottom 12%): soft lower bloom (115 -> 254)
// - Indices 8..39  (core 50%):   full-bodied luminous ribbon plateau (256 -> 190) for 3x thicker visual body
// - Indices 40..63 (upper 38%):  hazy atmospheric tail fade into night sky (182 -> 4)
static constexpr uint16_t AURORA_PLUME_LUT[64] = {
  115, 138, 162, 186, 208, 228, 244, 254,
  256, 256, 256, 255, 254, 252, 250, 248,
  246, 243, 240, 237, 234, 231, 228, 225,
  222, 219, 216, 213, 210, 207, 204, 202,
  200, 198, 196, 194, 192, 190, 186, 182,
  176, 170, 162, 154, 145, 136, 126, 116,
  106,  96,  86,  76,  66,  57,  48,  40,
   33,  26,  20,  15,  11,   8,   6,   4
};

template<typename DisplayT>
inline void render_screensaver_aurora(DisplayT &it, int cx, int cy, uint32_t now_ms) {
  (void)cx;
  float t = now_ms * 0.0010f;

  // Slow organic breathing pulse per veil (0.90 .. 1.04) with deep, saturated base hues
  float b1 = 0.92f + 0.10f * fast_sin(t * 0.9f);
  int p1_r = static_cast<int>(192.0f * b1);
  int p1_g = static_cast<int>(24.0f  * b1);
  int p1_b = static_cast<int>(205.0f * b1);

  float b2 = 0.92f + 0.10f * fast_cos(t * 1.1f + 1.2f);
  int p2_r = static_cast<int>(12.0f  * b2);
  int p2_g = static_cast<int>(228.0f * b2);
  int p2_b = static_cast<int>(242.0f * b2);

  float b3 = 0.92f + 0.10f * fast_sin(t * 0.8f + 2.5f);
  int p3_r = static_cast<int>(20.0f  * b3);
  int p3_g = static_cast<int>(235.0f * b3);
  int p3_b = static_cast<int>(82.0f  * b3);

  for (int x = 8; x <= 458; x += 2) {
    int h_half = circle_half_span(x, 5);
    if (h_half <= 0) continue;
    int y_min = cy - h_half;
    int y_max = cy + h_half;
    float xf = static_cast<float>(x);

    // Full-bodied Chunk 72 physical veil geometry (120..150px heights & wide organic overlap)
    int y1 = cy - 10 + static_cast<int>(fast_sin(xf * 0.011f + t * 0.6f) * 58.0f + fast_cos(xf * 0.006f - t * 0.35f) * 28.0f);
    int len1 = 120 + static_cast<int>(fast_sin(xf * 0.024f + t * 1.2f) * 35.0f);
    int top1 = y1 - len1;
    int step1_fp = (63 << 10) / (len1 > 1 ? len1 : 1);

    int y2 = cy + 32 + static_cast<int>(fast_sin(xf * 0.014f - t * 0.8f + 1.6f) * 62.0f + fast_sin(xf * 0.008f + t * 0.5f) * 32.0f);
    int len2 = 135 + static_cast<int>(fast_cos(xf * 0.028f - t * 1.5f) * 40.0f);
    int top2 = y2 - len2;
    int step2_fp = (63 << 10) / (len2 > 1 ? len2 : 1);

    int y3 = cy + 78 + static_cast<int>(fast_sin(xf * 0.017f + t * 1.0f + 3.1f) * 66.0f + fast_cos(xf * 0.010f - t * 0.7f) * 36.0f);
    int len3 = 150 + static_cast<int>(fast_sin(xf * 0.032f + t * 1.8f) * 44.0f);
    int top3 = y3 - len3;
    int step3_fp = (63 << 10) / (len3 > 1 ? len3 : 1);

    int col_top = top1;
    if (top2 < col_top) col_top = top2;
    if (top3 < col_top) col_top = top3;
    if (col_top < y_min) col_top = y_min;

    int col_bot = y1;
    if (y2 > col_bot) col_bot = y2;
    if (y3 > col_bot) col_bot = y3;
    if (col_bot > y_max) col_bot = y_max;

    // Hue-Preserving Soft-Add Blending:
    // When two or three curtains overlap, we weight their RGB mix by (w_max + 28% of secondary weights) / w_sum
    // so intersections glow slightly brighter while blending into deep indigo, royal blue, and seafoam
    // instead of saturating all three RGB channels into white!
    for (int py = col_bot; py >= col_top; py -= 2) {
      int r = 0, g = 0, b = 0;
      uint32_t w_sum = 0;
      uint32_t w_max = 0;

      if (py <= y1 && py >= top1) {
        int idx1 = ((y1 - py) * step1_fp) >> 10;
        uint32_t w1 = AURORA_PLUME_LUT[idx1 & 63];
        r += (p1_r * static_cast<int>(w1)) >> 8;
        g += (p1_g * static_cast<int>(w1)) >> 8;
        b += (p1_b * static_cast<int>(w1)) >> 8;
        w_sum += w1;
        w_max = w1;
      }
      if (py <= y2 && py >= top2) {
        int idx2 = ((y2 - py) * step2_fp) >> 10;
        uint32_t w2 = AURORA_PLUME_LUT[idx2 & 63];
        r += (p2_r * static_cast<int>(w2)) >> 8;
        g += (p2_g * static_cast<int>(w2)) >> 8;
        b += (p2_b * static_cast<int>(w2)) >> 8;
        w_sum += w2;
        if (w2 > w_max) w_max = w2;
      }
      if (py <= y3 && py >= top3) {
        int idx3 = ((y3 - py) * step3_fp) >> 10;
        uint32_t w3 = AURORA_PLUME_LUT[idx3 & 63];
        r += (p3_r * static_cast<int>(w3)) >> 8;
        g += (p3_g * static_cast<int>(w3)) >> 8;
        b += (p3_b * static_cast<int>(w3)) >> 8;
        w_sum += w3;
        if (w3 > w_max) w_max = w3;
      }

      if (w_sum > w_max && w_sum > 0) {
        // Normalize overlapping curtains so hue mixes cleanly with a +25% intersection glow
        // rather than summing R+G+B to 255 (white)
        uint32_t w_target = w_max + ((w_sum - w_max) >> 2);
        r = static_cast<int>((static_cast<uint32_t>(r) * w_target) / w_sum);
        g = static_cast<int>((static_cast<uint32_t>(g) * w_target) / w_sum);
        b = static_cast<int>((static_cast<uint32_t>(b) * w_target) / w_sum);
      }

      if ((r | g | b) > 5) {
        uint8_t cr = static_cast<uint8_t>(r > 255 ? 255 : r);
        uint8_t cg = static_cast<uint8_t>(g > 255 ? 255 : g);
        uint8_t cb = static_cast<uint8_t>(b > 255 ? 255 : b);
        it.draw_pixel_at(x, py, esphome::Color(cr, cg, cb));
        // Subtle 38% companion pixel at py - 1: splits the brightness difference between Chunk 71 and Chunk 72
        if (py - 1 >= y_min) {
          it.draw_pixel_at(x, py - 1, esphome::Color(
              static_cast<uint8_t>((cr * 96U) >> 8),
              static_cast<uint8_t>((cg * 96U) >> 8),
              static_cast<uint8_t>((cb * 96U) >> 8)));
        }
      }
    }
  }
}

template<typename DisplayT>
inline void render_screensaver_starfield(DisplayT &it, int cx, int cy, uint32_t now_ms = 0) {
  struct Star3D { float x, y, z; };
  static Star3D stars[90];
  static bool stars_init = false;
  static uint32_t star_seed = 123456789;
  static uint32_t last_star_ms = 0;

  auto star_rand = [&]() -> float {
    star_seed = star_seed * 1664525u + 1013904223u;
    return static_cast<float>(star_seed & 0xFFFFU) * (1.0f / 65535.0f);
  };
  auto respawn_star = [&](int i, bool init_z) {
    float ang = star_rand() * (2.0f * PI_F);
    float dist = 40.0f + star_rand() * 650.0f;
    stars[i].x = fast_cos(ang) * dist;
    stars[i].y = fast_sin(ang) * dist;
    stars[i].z = init_z ? (30.0f + star_rand() * 950.0f) : (850.0f + star_rand() * 150.0f);
  };
  if (!stars_init) {
    for (int i = 0; i < 90; i++) respawn_star(i, true);
    stars_init = true;
    last_star_ms = now_ms;
  }

  // Time-delta driven progression (250 units/sec = 25 units per 100ms frame)
  float speed = 25.0f;
  if (now_ms > 0) {
    uint32_t dt = now_ms - last_star_ms;
    last_star_ms = now_ms;
    if (dt >= 20U && dt <= 300U) {
      speed = static_cast<float>(dt) * 0.25f;
    }
  }

  for (int i = 0; i < 90; i++) {
    stars[i].z -= speed;
    if (stars[i].z <= 25.0f) {
      respawn_star(i, false);
      continue;
    }
    float k = 230.0f / stars[i].z;
    int sx = cx + static_cast<int>(stars[i].x * k);
    int sy = cy + static_cast<int>(stars[i].y * k);
    if (!is_inside_bezel(sx, sy, cy, 6)) {
      respawn_star(i, false);
      continue;
    }
    float prev_k = 230.0f / (stars[i].z + speed * 1.5f);
    int px = cx + static_cast<int>(stars[i].x * prev_k);
    int py = cy + static_cast<int>(stars[i].y * prev_k);
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

// 32-entry procedural Matrix Katakana & mirrored cipher stroke bitmask LUT in .rodata Flash:
// Bits 0..9 encode 10 canonical 7x9 cell strokes (top/mid/bot bars, UL/LL/UR/LR stems, center spine,
// sweeping Katakana diagonal, and dakuten/cross accents) so every grid cell renders a sharp,
// authentic Japanese half-width Katakana or mirrored cipher character rather than a solid rectangle.
static constexpr uint16_t MATRIX_GLYPH_MASK[32] = {
  0x121, // 0: ア (top bar + UR stem + sweeping Katakana diagonal)
  0x181, // 1: イ / テ (top bar + center spine + sweeping diagonal)
  0x069, // 2: ウ (top bar + UL stem + UR/LR right vertical)
  0x085, // 3: エ (top bar + center spine + bottom bar)
  0x183, // 4: オ (top + mid bar + center spine + diagonal sweep)
  0x06B, // 5: カ (top + mid bar + UL stem + UR/LR hook)
  0x083, // 6: キ / 十 (top + mid bar + center spine)
  0x129, // 7: ク (top bar + UL stem + UR stem + diagonal sweep)
  0x0A9, // 8: ケ (top bar + UL stem + UR stem + center spine)
  0x065, // 9: コ (top bar + UR/LR stem + bottom bar)
  0x0A3, // 10: サ (top + mid bar + UL + UR + center spine)
  0x105, // 11: ス / Z (top bar + diagonal sweep + bottom bar)
  0x214, // 12: ソ / レ (LL stem + bottom bar + dakuten accent)
  0x12B, // 13: タ (top + mid bar + UL/UR + diagonal sweep)
  0x182, // 14: ナ (mid bar + center spine + diagonal sweep)
  0x007, // 15: ニ / 三 (top + mid + bottom horizontal bars)
  0x301, // 16: ヌ (top bar + sweeping diagonal + cross accent)
  0x381, // 17: ネ (top bar + center spine + diagonal + cross)
  0x108, // 18: ノ / ハ (UL stem + sweeping Katakana diagonal)
  0x056, // 19: ヒ (mid + bottom bar + LL + LR stems)
  0x141, // 20: フ (top bar + LR hook + sweeping diagonal)
  0x187, // 21: ホ (top + mid + bot bar + spine + diagonal)
  0x223, // 22: マ (top + mid bar + UR stem + accent)
  0x207, // 23: ミ (three horizontal bars + diagonal accent)
  0x114, // 24: ム (LL stem + bottom bar + sweeping diagonal)
  0x300, // 25: メ (crossing Katakana diagonals)
  0x097, // 26: モ (top + mid + bot bar + LL stem + center spine)
  0x067, // 27: ヨ (top + mid + bot bar + UR/LR right stem)
  0x107, // 28: ラ (top + mid + bot bar + sweeping diagonal)
  0x028, // 29: リ (UL short stem + UR/LR long stem)
  0x07D, // 30: ロ (full Katakana box)
  0x204  // 31: ン (bottom bar + dakuten diagonal accent)
};

template<typename DisplayT>
inline void draw_matrix_katakana_glyph(DisplayT &it, int sx, int gy, uint16_t mask,
                                       esphome::Color col, bool slim_tail) {
  if (slim_tail) {
    // Dimmest tail tip: render only primary vertical/diagonal spine + 1 bar so phosphor dissolves cleanly
    if (mask & 0x080) it.line(sx, gy - 3, sx, gy + 3, col);
    else if (mask & 0x100) it.line(sx + 2, gy - 2, sx - 2, gy + 3, col);
    else it.line(sx - 2, gy - 3, sx + 2, gy - 3, col);
    if (mask & 0x002) it.line(sx - 2, gy, sx + 2, gy, col);
    return;
  }
  if (mask & 0x001) it.line(sx - 3, gy - 4, sx + 3, gy - 4, col); // Top bar
  if (mask & 0x002) it.line(sx - 3, gy,     sx + 3, gy,     col); // Mid bar
  if (mask & 0x004) it.line(sx - 3, gy + 4, sx + 3, gy + 4, col); // Bottom bar
  if (mask & 0x008) it.line(sx - 3, gy - 4, sx - 3, gy,     col); // UL stem
  if (mask & 0x010) it.line(sx - 3, gy,     sx - 3, gy + 4, col); // LL stem
  if (mask & 0x020) it.line(sx + 3, gy - 4, sx + 3, gy,     col); // UR stem
  if (mask & 0x040) it.line(sx + 3, gy,     sx + 3, gy + 4, col); // LR stem
  if (mask & 0x080) it.line(sx,     gy - 4, sx,     gy + 4, col); // Center spine
  if (mask & 0x100) it.line(sx + 3, gy - 2, sx - 3, gy + 4, col); // Katakana sweep diagonal
  if (mask & 0x200) it.line(sx - 2, gy - 2, sx + 2, gy + 2, col); // Cross / dakuten accent
}

template<typename DisplayT>
inline void render_screensaver_matrix(DisplayT &it, int cx, int cy, uint32_t now_ms) {
  (void)cx;
  // 22 independent single-stream columns (19px pitch across x = 33..432) instead of 31 double-stacked streams:
  // Eliminates same-column stream collisions and leaves ~35% of columns in a dark rest gap at any moment
  // so the rain feels authentically sparse, airy, and un-clumped.
  static constexpr int NUM_COLS = 22;
  static constexpr int CELL_H = 14; // Stationary 14px vertical Katakana grid

  // Slow anti-burn-in horizontal & vertical grid drift (shifts ±4px X, ±3px Y over a 90s cycle)
  float t_drift = now_ms * 0.00007f;
  int drift_x = static_cast<int>(fast_sin(t_drift) * 4.2f);
  int drift_y = static_cast<int>(fast_cos(t_drift * 1.3f) * 3.2f);
  float t_sec = now_ms * 0.001f;

  for (int i = 0; i < NUM_COLS; i++) {
    int base_x = 34 + i * 19;
    int sx = base_x + drift_x;
    int max_dy = circle_half_span(sx, 7);
    if (max_dy <= 0) continue;
    int y_min = cy - max_dy;
    int y_max = cy + max_dy;

    // Convert circular column chord into discrete 14px grid row indices
    int min_row = (y_min - drift_y) / CELL_H;
    int max_row = (y_max - drift_y) / CELL_H;
    int visible_rows = max_row - min_row + 1;
    if (visible_rows <= 2) continue;

    // Wide 2.3x per-column base velocity spread (5.4 .. 12.4 rows/sec) using low-discrepancy hashing
    // so adjacent columns never march down the screen in synchronized horizontal waves ("clumps")
    float col_h1 = hash2d_01(i * 37 + 11, i * 19 + 5);
    float col_h2 = hash2d_01(i * 53 + 23, i * 29 + 7);
    float rows_per_sec = 5.4f + col_h1 * 7.0f;

    // Total cycle length per pass = visible rows + max tail (16) + generous dark rest gap (14..32 rows)
    // so each column rests in pure black for 1.5..3.5s between drops!
    float cycle_rows_f = static_cast<float>(visible_rows) + 30.0f + col_h2 * 18.0f;

    // Golden-ratio + hash phase scatter (0.0 .. cycle_rows_f):
    // Guarantees that at t = 0 (and at every moment thereafter), all 22 columns are uniformly
    // scattered across their lifecycle (some mid-fall, some entering, ~35% in their dark pause gap)
    float init_phase_frac = fmodf(static_cast<float>(i) * 0.6180339887f + col_h1 * 0.37f, 1.0f);
    float total_travel = t_sec * rows_per_sec + init_phase_frac * cycle_rows_f;

    uint32_t pass_idx = static_cast<uint32_t>(total_travel / cycle_rows_f);
    float local_pos = total_travel - static_cast<float>(pass_idx) * cycle_rows_f;

    // Deterministic per-pass randomization via hash2d_01:
    // Every pass of column `i` rolls a fresh tail length, extra spawn delay, and 3D depth tier
    int pass_seed = static_cast<int>(pass_idx * 43U + static_cast<uint32_t>(i) * 109U);
    float r_len   = hash2d_01(pass_seed, 13);
    float r_tier  = hash2d_01(pass_seed, 29);
    float r_delay = hash2d_01(pass_seed, 47);

    // Slightly shorter, crisper tail lengths (5..14 glyphs) for a cleaner, sparser look
    int slen = 5 + static_cast<int>(r_len * 9.9f);
    float slen_f = static_cast<float>(slen);

    // ~30% of passes run as dimmer background-depth code streams for 3D depth
    bool is_bg_stream = (r_tier < 0.30f);
    float bright_scale = is_bg_stream ? 0.54f : 1.0f;

    // Continuous floating-point cursor position (head_row_f) over stationary integer grid rows:
    // Glyphs stay locked to `row * CELL_H`, while `dist = head_row_f - row` interpolates brightness
    // continuously on every 20 FPS frame so the rain glides down smoothly with zero stepping!
    float start_row_f = static_cast<float>(min_row) - 1.0f - r_delay * 8.0f;
    float head_row_f = start_row_f + local_pos;
    int head_row_top = static_cast<int>(floorf(head_row_f));

    // Sub-pixel phosphor bloom box tracking the continuous cursor head on foreground streams
    if (!is_bg_stream) {
      int exact_hy = static_cast<int>(head_row_f * static_cast<float>(CELL_H)) + drift_y;
      if (exact_hy >= y_min && exact_hy <= y_max) {
        it.filled_rectangle(sx - 4, exact_hy - 5, 9, 11, esphome::Color(0, 62, 18));
      }
    }

    for (int j = 0; j <= slen; j++) {
      int row = head_row_top - j;
      if (row < min_row || row > max_row) continue;
      int gy = row * CELL_H + drift_y;
      if (gy < y_min || gy > y_max) continue;

      float dist = head_row_f - static_cast<float>(row);
      if (dist < 0.0f || dist > slen_f) continue;

      // Stationary cell glyph hash keyed by (column i, stationary grid row, pass_idx):
      // - Lead cursor (dist < 1.1) scrambles every 85ms
      // - Stationary trail cells hold their printed Katakana glyph, while ~12% of cells
      //   asynchronously "glitch-mutate" in place at independent intervals (190..445ms)
      uint32_t cell_hash = static_cast<uint32_t>(i * 73856093) ^
                           static_cast<uint32_t>(row * 19349663) ^
                           (pass_idx * 83492791U);
      uint32_t glitch_tick = (dist < 1.1f)
          ? (now_ms / 85U)
          : (((cell_hash & 7U) == 0U) ? (now_ms / (190U + (cell_hash & 255U))) : 0U);
      uint16_t mask = MATRIX_GLYPH_MASK[(cell_hash + glitch_tick * 19U) & 31U];

      esphome::Color g_col;
      if (dist < 0.95f) {
        // Smooth sub-row attack & incandescent white-mint peak as cursor sweeps across the cell
        float lead_env = (dist < 0.30f) ? (0.45f + (dist / 0.30f) * 0.55f) : 1.0f;
        if (!is_bg_stream) {
          g_col = esphome::Color(
              static_cast<uint8_t>(225.0f * lead_env),
              static_cast<uint8_t>(255.0f * lead_env),
              static_cast<uint8_t>(235.0f * lead_env));
        } else {
          g_col = esphome::Color(
              static_cast<uint8_t>(110.0f * lead_env),
              static_cast<uint8_t>(225.0f * lead_env),
              static_cast<uint8_t>(138.0f * lead_env));
        }
      } else if (dist < 2.2f) {
        // Continuous sub-pixel cooling from white-mint (dist=0.95) to bright neon emerald (dist=2.2)
        float u = (dist - 0.95f) * (1.0f / 1.25f);
        float r_c = (115.0f * (1.0f - u) + 24.0f * u) * bright_scale;
        float g_c = (255.0f * (1.0f - u) + 225.0f * u) * bright_scale;
        float b_c = (140.0f * (1.0f - u) + 58.0f * u) * bright_scale;
        g_col = esphome::Color(
            static_cast<uint8_t>(r_c),
            static_cast<uint8_t>(g_c),
            static_cast<uint8_t>(b_c));
      } else {
        // Smooth continuous exponential-like phosphor trail decay from dist=2.2 to slen_f
        float tail_u = (dist - 2.2f) / (slen_f > 2.4f ? (slen_f - 2.2f) : 1.0f);
        if (tail_u > 1.0f) tail_u = 1.0f;
        float inv_u = 1.0f - tail_u;
        float fade = (16.0f + 198.0f * inv_u * (0.65f + 0.35f * inv_u)) * bright_scale;

        // Asynchronous phosphor glitch-pulse when a stationary trail glyph mutates
        bool glitch_flash = (dist < 5.0f) && ((cell_hash & 7U) == 0U) &&
                            (((now_ms / 160U) + static_cast<uint32_t>(row)) % 6U == 0U);
        if (glitch_flash) {
          g_col = esphome::Color(
              static_cast<uint8_t>(90.0f * bright_scale),
              static_cast<uint8_t>(255.0f * bright_scale),
              static_cast<uint8_t>(118.0f * bright_scale));
        } else {
          uint8_t fg = static_cast<uint8_t>(fade);
          g_col = esphome::Color(0, fg, static_cast<uint8_t>(fg >> 2));
        }
      }

      bool slim_tail = (dist > slen_f - 1.6f && slen > 6);
      draw_matrix_katakana_glyph(it, sx, gy, mask, g_col, slim_tail);
    }
  }
}

template<typename DisplayT>
inline void render_screensaver_fireworks(DisplayT &it, int cx, int cy, uint32_t now_ms) {
  // 3 staggered pyrotechnic shells (3600ms lifecycle each, staggered by 1200ms)
  // Zero static pixels: every launch & skyburst occurs at a new pseudo-random coordinate
  // across the circular sky and fades completely to pure AMOLED black (0,0,0).
  static constexpr uint32_t SHELL_CYCLE_MS = 3600U;
  static constexpr uint32_t SHELL_STAGGER_MS = 1200U;

  for (int s = 0; s < 3; s++) {
    uint32_t t_offset = now_ms + static_cast<uint32_t>(s) * SHELL_STAGGER_MS;
    uint32_t cycle_idx = t_offset / SHELL_CYCLE_MS;
    uint32_t phase_ms = t_offset % SHELL_CYCLE_MS;
    float phase = static_cast<float>(phase_ms) * (1.0f / static_cast<float>(SHELL_CYCLE_MS));

    // Deterministic per-shell pseudo-random seed (unique per shell slot and cycle)
    int seed_base = static_cast<int>(cycle_idx * 17U + static_cast<uint32_t>(s) * 53U);
    float rx = hash2d_01(seed_base, 11);
    float ry = hash2d_01(seed_base, 29);
    float rtype = hash2d_01(seed_base, 47);
    float rhue = hash2d_01(seed_base, 73);

    // Burst target coordinates distributed across the upper/mid circular sky
    int bx = cx + static_cast<int>((rx - 0.5f) * 230.0f);
    int by = (cy - 30) + static_cast<int>((ry - 0.5f) * 165.0f);
    // Keep burst center comfortably inside bezel so 3D spark sphere has room to bloom
    int dx_b = bx - cx, dy_b = by - cy;
    if (dx_b * dx_b + dy_b * dy_b > 15625) { // R_burst_center <= 125px
      bx = cx + (dx_b * 110) / 160;
      by = cy + (dy_b * 110) / 160;
    }

    // Launch origin near the bottom bezel arc
    int launch_x = cx + static_cast<int>((hash2d_01(seed_base, 91) - 0.5f) * 120.0f);
    int launch_h = circle_half_span(launch_x, 10);
    int launch_y = cy + (launch_h > 40 ? launch_h : 180);

    // -------------------------------------------------------------------------
    // STAGE 1: ASCENDING MORTAR ROCKET WITH SPARKING COMET TAIL (phase 0..0.22)
    // -------------------------------------------------------------------------
    if (phase < 0.22f) {
      float u = phase * (1.0f / 0.22f); // 0.0 -> 1.0
      // Ease-out upward ballistic arc
      float ease = 1.0f - (1.0f - u) * (1.0f - u);
      float prev_u = (u > 0.14f) ? (u - 0.14f) : 0.0f;
      float prev_ease = 1.0f - (1.0f - prev_u) * (1.0f - prev_u);

      float wob_x = 8.0f * fast_sin(u * 10.0f + static_cast<float>(s) * 2.1f) * (1.0f - u);
      int hx = launch_x + static_cast<int>((bx - launch_x) * ease + wob_x);
      int hy = launch_y + static_cast<int>((by - launch_y) * ease);
      int tx = launch_x + static_cast<int>((bx - launch_x) * prev_ease);
      int ty = launch_y + static_cast<int>((by - launch_y) * prev_ease);
      int mx = (hx + tx) >> 1;
      int my = (hy + ty) >> 1;

      if (is_inside_bezel(hx, hy, cy, 5) && is_inside_bezel(tx, ty, cy, 5)) {
        it.line(tx, ty, mx, my, esphome::Color(160, 65, 10));
        draw_glow_line(it, mx, my, hx, hy, esphome::Color(255, 235, 150), esphome::Color(220, 110, 20));
        it.filled_circle(hx, hy, 2, esphome::Color(255, 255, 220));
      }
      continue;
    }

    // -------------------------------------------------------------------------
    // STAGE 2: 3D SPHERICAL PYROTECHNIC SKYBURST & GRAVITY SPARK TRAILS
    // -------------------------------------------------------------------------
    float bp = (phase - 0.22f) * (1.0f / 0.78f); // 0.0 -> 1.0 across explosion lifetime
    // Exponential drag expansion: fast outward pop that decelerates in air
    float expand = 1.0f - (1.0f - bp) * (1.0f - bp) * (1.0f - bp);
    // Tail position slightly earlier on the expansion curve
    float bp_tail = (bp > 0.14f) ? (bp - 0.14f) : (bp * 0.35f);
    float expand_tail = 1.0f - (1.0f - bp_tail) * (1.0f - bp_tail) * (1.0f - bp_tail);

    // Parabolic downward gravity droop (y += g * bp^2)
    float grav_head = 56.0f * bp * bp;
    float grav_tail = 56.0f * bp_tail * bp_tail;
    float fade = (1.0f - bp);
    float bright = (bp < 0.12f) ? 1.0f : (fade * (1.12f - 0.12f * bp));
    if (bright > 1.0f) bright = 1.0f;

    // Initial detonation core flash (first 16% of burst)
    if (bp < 0.16f) {
      float fl = 1.0f - (bp * 6.25f);
      int flash_r = static_cast<int>(4.0f + bp * 95.0f);
      esphome::Color fl_col(
          static_cast<uint8_t>(255.0f * fl),
          static_cast<uint8_t>(240.0f * fl),
          static_cast<uint8_t>(185.0f * fl));
      it.circle(bx, by, flash_r, fl_col);
    }

    // 4 Pyrotechnic Shell Styles:
    // 0: Golden Kamuro Willow (weeping gold-amber trails)
    // 1: Dual-Ring Peony + Pistil (Neon Magenta/Orchid outer + Electric Cyan inner core)
    // 2: Chrysanthemum Prism (360-degree rainbow spectrum burst)
    // 3: Emerald & Violet Crossette with Strobe Crackle Glitter
    int shell_style = static_cast<int>(rtype * 4.0f) & 3;
    float max_radius = 96.0f + 34.0f * hash2d_01(seed_base, 61);

    // 36 sparks distributed across 3 concentric 3D spherical latitude shells
    for (int i = 0; i < 60; i++) {
      // Select 36 rays out of the 60 LUT angles (3 out of every 5 ticks)
      if ((i % 5) == 1 || (i % 5) == 3) continue;

      // 3D spherical depth layer (outer equator=1.0, mid latitude=0.68, inner pole=0.38)
      int ring_id = i % 3;
      float ring_scale = (ring_id == 0) ? 1.00f : ((ring_id == 1) ? 0.68f : 0.40f);
      float jitter = 0.86f + 0.28f * hash2d_01(seed_base + i, i * 7);
      float spark_rmax = max_radius * ring_scale * jitter;

      float r_head = spark_rmax * expand;
      float r_tail = spark_rmax * expand_tail;

      float ca = DIAL_COS[i];
      float sa = DIAL_SIN[i];

      int hx = bx + static_cast<int>(r_head * ca);
      int hy = by + static_cast<int>(r_head * sa + grav_head * (0.75f + 0.25f * ring_scale));
      int tx = bx + static_cast<int>(r_tail * ca);
      int ty = by + static_cast<int>(r_tail * sa + grav_tail * (0.75f + 0.25f * ring_scale));

      if (!is_inside_bezel(hx, hy, cy, 5) || !is_inside_bezel(tx, ty, cy, 5)) continue;

      esphome::Color spark_col;
      if (shell_style == 0) {
        // Golden Kamuro Willow: white-gold cooling to warm amber and deep ember red
        uint8_t cr = static_cast<uint8_t>(255.0f * bright);
        uint8_t cg = static_cast<uint8_t>((215.0f * fade + 35.0f) * bright);
        uint8_t cb = static_cast<uint8_t>(110.0f * fade * fade * bright);
        spark_col = esphome::Color(cr, cg, cb);
      } else if (shell_style == 1) {
        // Dual-Ring Peony + Pistil: outer neon magenta/violet, inner electric cyan
        if (ring_id == 2) {
          spark_col = esphome::Color(
              static_cast<uint8_t>(40.0f * bright),
              static_cast<uint8_t>(245.0f * bright),
              static_cast<uint8_t>(255.0f * bright));
        } else {
          spark_col = esphome::Color(
              static_cast<uint8_t>(255.0f * bright),
              static_cast<uint8_t>((45.0f + 80.0f * fade) * bright),
              static_cast<uint8_t>(220.0f * bright));
        }
      } else if (shell_style == 2) {
        // Chrysanthemum Prism: smooth azimuthal hue wheel
        float h = rhue + static_cast<float>(i) * (1.0f / 60.0f);
        spark_col = hsv_to_rgb(h, 0.82f, bright);
      } else {
        // Emerald & Royal Amethyst Crossette
        if (ring_id == 1) {
          spark_col = esphome::Color(
              static_cast<uint8_t>(185.0f * bright),
              static_cast<uint8_t>(75.0f * bright),
              static_cast<uint8_t>(255.0f * bright));
        } else {
          spark_col = esphome::Color(
              static_cast<uint8_t>(35.0f * bright),
              static_cast<uint8_t>(255.0f * bright),
              static_cast<uint8_t>(120.0f * bright));
        }
      }

      // Draw two-stage tapering comet trail (faded tail -> bright mid-to-head spine)
      int mx = (hx + tx) >> 1;
      int my = (hy + ty) >> 1;
      esphome::Color tail_col(
          static_cast<uint8_t>((spark_col.r * 95U) >> 8),
          static_cast<uint8_t>((spark_col.g * 95U) >> 8),
          static_cast<uint8_t>((spark_col.b * 95U) >> 8));
      it.line(tx, ty, mx, my, tail_col);
      it.line(mx, my, hx, hy, spark_col);

      // Glowing spark tip during first 55% of burst, or crackling strobe glitter at the end
      if (bp < 0.55f) {
        esphome::Color tip_col(
            static_cast<uint8_t>(spark_col.r + ((255 - spark_col.r) >> 1)),
            static_cast<uint8_t>(spark_col.g + ((255 - spark_col.g) >> 1)),
            static_cast<uint8_t>(spark_col.b + ((255 - spark_col.b) >> 1)));
        it.draw_pixel_at(hx, hy, tip_col);
        if (bp < 0.28f && ring_id == 0) {
          it.filled_circle(hx, hy, 1, tip_col);
        }
      } else if (bp > 0.62f && ((i + (phase_ms / 90U)) % 4 == 0)) {
        // Secondary crackle glitter twinkle as embers dissolve into the night sky
        uint8_t gl = static_cast<uint8_t>(255.0f * fade);
        it.draw_pixel_at(hx, hy, esphome::Color(gl, gl, static_cast<uint8_t>(gl * 0.8f)));
      }
    }
  }
}

// Unified Docked Screensaver Dispatcher (Ambient Clock + 4 Procedural Engines)
template<typename DisplayT, typename TimeT, typename StrT, typename FontT>
inline void render_docked_screensaver(DisplayT &it, int cx, int cy, uint32_t now_ms,
                                      const TimeT &now, bool is_24h, const StrT &ss_style,
                                      FontT *font_time, FontT *font_small,
                                      esphome::Color col_slate, esphome::Color col_dark_gray) {
  if (ss_style == "Ambient Clock") {
    // Calm 1-minute OLED burn-in drift (shifts gently once per minute instead of twitching every second)
    int min_val = now.is_valid() ? now.minute : 0;
    int drift_x = (min_val % 10) * 4 - 18;
    int drift_y = ((min_val / 10) % 6) * 4 - 10;

    char amb_time[16];
    if (now.is_valid()) {
      format_time_hhmm(now.hour, now.minute, is_24h, amb_time, sizeof(amb_time));
      it.print(cx + drift_x, cy - 20 + drift_y, font_time, col_slate, esphome::display::TextAlign::CENTER, amb_time);
      it.strftime(cx + drift_x, cy + 45 + drift_y, font_small, col_dark_gray, esphome::display::TextAlign::CENTER, "%a, %b %d", now);
    } else {
      it.print(cx + drift_x, cy - 20 + drift_y, font_time, col_slate, esphome::display::TextAlign::CENTER, "--:--");
    }
    return;
  }

  last_screensaver_draw_ms() = now_ms;
  if (ss_style == "Starfield") {
    render_screensaver_starfield(it, cx, cy, now_ms);
  } else if (ss_style == "Matrix Rain") {
    render_screensaver_matrix(it, cx, cy, now_ms);
  } else if (ss_style == "Fireworks" || ss_style == "Solar Flare") {
    render_screensaver_fireworks(it, cx, cy, now_ms);
  } else {
    render_screensaver_aurora(it, cx, cy, now_ms);
  }
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

// Tracks the last timestamp (millis) when Page 2 (Now Playing) completed a redraw
// so throttled_disp_update never stacks redundant 466x466 PSRAM album-art blits
// on top of va_tts_drain_and_finish, cancel_voice_assistant, or disp1's 1s timer.
inline uint32_t &last_page2_draw_ms() {
  static uint32_t ts = 0;
  return ts;
}

// Zero-trig LUT-driven 2px perimeter track progress arc (r = 224..225) for Page 2 Now Playing
// Replaces 180-step (360 sincosf + 360 line) loop with 60-segment DIAL_SIN/DIAL_COS LUT + fractional tip
template<typename DisplayT>
inline void render_progress_arc_r224(DisplayT &it, int cx, int cy, float prog, esphome::Color col) {
  if (prog <= 0.005f) return;
  if (prog > 1.0f) prog = 1.0f;
  float total_segs = prog * 60.0f;
  int full_segs = static_cast<int>(total_segs);
  if (full_segs > 60) full_segs = 60;
  float frac = total_segs - static_cast<float>(full_segs);

  for (int i = 0; i < full_segs; i++) {
    int nxt = (i + 1) % 60;
    float s1 = DIAL_SIN[i], c1 = DIAL_COS[i];
    float s2 = DIAL_SIN[nxt], c2 = DIAL_COS[nxt];
    it.line(cx + static_cast<int>(224.0f * s1), cy - static_cast<int>(224.0f * c1),
            cx + static_cast<int>(224.0f * s2), cy - static_cast<int>(224.0f * c2), col);
    it.line(cx + static_cast<int>(225.0f * s1), cy - static_cast<int>(225.0f * c1),
            cx + static_cast<int>(225.0f * s2), cy - static_cast<int>(225.0f * c2), col);
  }
  if (full_segs < 60 && frac > 0.05f) {
    int nxt = (full_segs + 1) % 60;
    float s1 = DIAL_SIN[full_segs], c1 = DIAL_COS[full_segs];
    float s2 = s1 + (DIAL_SIN[nxt] - s1) * frac;
    float c2 = c1 + (DIAL_COS[nxt] - c1) * frac;
    it.line(cx + static_cast<int>(224.0f * s1), cy - static_cast<int>(224.0f * c1),
            cx + static_cast<int>(224.0f * s2), cy - static_cast<int>(224.0f * c2), col);
    it.line(cx + static_cast<int>(225.0f * s1), cy - static_cast<int>(225.0f * c1),
            cx + static_cast<int>(225.0f * s2), cy - static_cast<int>(225.0f * c2), col);
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

  // Smooth exponential envelopes (0.0f -> 1.0f) for fluid organic transitions
  // across Listening -> Thinking -> Speaking and progressive wave launch from the core
  static float smooth_vad = 0.0f;
  static float smooth_think = 0.0f;
  static float smooth_speak = 0.0f;
  static bool prev_speaking = false;
  static uint32_t speak_start_ms = 0;

  if (is_speaking && !prev_speaking) {
    speak_start_ms = now_ms;
  }
  prev_speaking = is_speaking;

  const float vad_target = va_vad_active ? 1.0f : 0.0f;
  const float think_target = is_thinking ? 1.0f : 0.0f;
  const float speak_target = is_speaking ? 1.0f : 0.0f;
  smooth_vad += (vad_target - smooth_vad) * 0.35f;
  smooth_think += (think_target - smooth_think) * 0.35f;
  smooth_speak += (speak_target - smooth_speak) * 0.35f;

  // ============================================================================
  // A. CLASSIC NEBULA & CLASSIC SOLARIS (Preserved Visuals, C++ Trig Optimized)
  // ============================================================================
  if (is_classic) {
    if (is_speaking) {
      uint32_t speak_elapsed = now_ms - speak_start_ms;
      for (int w = 0; w < 5; w++) {
        uint32_t launch_offset = static_cast<uint32_t>(w) * 400U;
        if (speak_elapsed < launch_offset) continue;
        uint32_t wave_t = (speak_elapsed - launch_offset) % 2000U;
        float progress = static_cast<float>(wave_t) * 0.0005f;
        int r_center = 36 + static_cast<int>(progress * 180.0f);

        float width_factor = sinf(progress * PI_F);
        int base_w = (w % 2 == 0) ? 7 : 4;
        int band_w = base_w + static_cast<int>(5.0f * width_factor);
        if (band_w < 3) band_w = 3;
        int half_w = band_w >> 1;

        float wave_color_phase = static_cast<float>(w) * 0.20f + progress * 0.30f +
                                 static_cast<float>(now_ms % 4000U) * 0.00025f;
        float fade = (1.0f - (progress * 0.45f)) * smooth_speak;
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

    // Helper: Draws a 30-segment closed harmonic silk ribbon using DIAL_SIN/DIAL_COS LUT
    auto draw_silk_ribbon = [&](float base_r, float a2, float a3, float a5,
                                int ph2, int ph3, int ph5,
                                esphome::Color col_main, esphome::Color col_glow,
                                bool double_stroke) {
      int prev_x = 0, prev_y = 0, first_x = 0, first_y = 0;
      int prev_gx = 0, prev_gy = 0, first_gx = 0, first_gy = 0;
      for (int i = 0; i < 60; i += 2) {
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
      float syl_env = (0.55f + 0.25f * sinf(t_sec * 11.5f) + 0.20f * cosf(t_sec * 19.3f)) * smooth_speak;
      if (syl_env < 0.15f) syl_env = 0.15f;

      int tick = static_cast<int>(now_ms / 35U);
      uint32_t speak_elapsed = now_ms - speak_start_ms;

      // 3 Expanding Acoustic Wave Rings launched progressively from the core (fast integer midpoint circles)
      for (int w = 0; w < 3; w++) {
        uint32_t launch_offset = static_cast<uint32_t>(w) * 600U;
        if (speak_elapsed < launch_offset) continue;
        uint32_t wave_t = (speak_elapsed - launch_offset) % 1800U;
        float prog = static_cast<float>(wave_t) * (1.0f / 1800.0f);
        float fade = sinf(prog * PI_F) * (0.45f + 0.45f * syl_env) * smooth_speak;
        int r_wave = 68 + static_cast<int>(prog * 144.0f);

        esphome::Color w_col = scale_color(get_va_radiation_color(false, prog + static_cast<float>(w) * 0.33f + t_sec * 0.25f), fade);
        esphome::Color w_glow = scale_color(w_col, 0.45f);
        it.circle(cx, cy, r_wave - 1, w_glow);
        it.circle(cx, cy, r_wave, w_col);
        it.circle(cx, cy, r_wave + 1, w_glow);
      }

      // 4 Interweaving Inner Silk Voice Ribbons (150 short inner-core line segments total)
      float base_r = 48.0f + 16.0f * syl_env;
      float a2 = 6.0f + 11.0f * syl_env;
      float a3 = 4.5f + 8.0f * syl_env;
      float a5 = 2.0f + 4.5f * syl_env;

      draw_silk_ribbon(base_r + 4.0f, a2, a3, a5, (tick) % 60, (tick * 2 + 15) % 60, (tick + 30) % 60,
                       c_royal_indigo, scale_color(c_royal_indigo, 0.4f), false);
      draw_silk_ribbon(base_r, -a2 * 0.9f, a3 * 1.1f, -a5, (tick + 20) % 60, (tick + 40) % 60, (tick * 2) % 60,
                       c_neon_magenta, scale_color(c_neon_magenta, 0.4f), false);
      draw_silk_ribbon(base_r - 2.0f, a2 * 0.8f, -a3, a5 * 1.2f, (tick * 2 + 7) % 60, (tick + 25) % 60, (tick + 12) % 60,
                       c_elec_cyan, c_deep_azure, true);
      draw_silk_ribbon(base_r - 5.0f, a2 * 0.5f, a3 * 0.5f, 0.0f, (tick + 35) % 60, (tick + 10) % 60, 0,
                       c_ice_white, c_elec_cyan, false);

      // Symmetrical Concentric Cognitive Core (matching Thinking aesthetic, zero offset 3D spot)
      int core_r = 22 + static_cast<int>(6.0f * syl_env);
      it.filled_circle(cx, cy, core_r + 4, scale_color(c_neon_magenta, 0.35f + 0.20f * syl_env));
      it.filled_circle(cx, cy, core_r, c_royal_indigo);
      it.circle(cx, cy, core_r + 1, c_elec_cyan);
      it.filled_circle(cx, cy, (core_r * 2) / 3, c_deep_azure);
      it.filled_circle(cx, cy, core_r / 2, c_ice_white);
      return;
    }

    if (is_thinking) {
      int tick = static_cast<int>(now_ms / 25U);
      float pulse = sinf(static_cast<float>(now_ms % 700U) * (2.0f * PI_F / 700.0f));

      // Smoothly contract silk ribbons from Listening radius (76px) into Thinking vortex (50px)
      float vortex_r = 72.0f - 22.0f * smooth_think;
      draw_silk_ribbon(vortex_r + 3.0f * pulse, 6.0f, 5.0f, 3.0f,
                       (tick * 2) % 60, (tick * 3) % 60, (tick) % 60,
                       c_royal_indigo, scale_color(c_royal_indigo, 0.4f), false);
      draw_silk_ribbon(vortex_r - 2.0f - 2.0f * pulse, -5.0f, 6.0f, -2.5f,
                       (tick * 2 + 30) % 60, (tick + 15) % 60, (tick * 2) % 60,
                       c_neon_magenta, scale_color(c_neon_magenta, 0.4f), false);
      draw_silk_ribbon(vortex_r - 4.0f, 4.0f, -4.0f, 2.0f,
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
          float k = static_cast<float>(5 - tail) * 0.20f * smooth_think;
          it.filled_circle(nx, ny, (tail == 0) ? 4 : (tail <= 2 ? 2 : 1),
                           (tail == 0) ? scale_color(c_ice_white, smooth_think) : scale_color(c_elec_cyan, k));
        }

        int base_ccw = (head_idx_ccw + n * 20 + 10) % 60;
        for (int tail = 4; tail >= 0; tail--) {
          int idx = (base_ccw + tail * 2) % 60;
          int nx = cx + static_cast<int>(114.0f * DIAL_SIN[idx]);
          int ny = cy - static_cast<int>(114.0f * DIAL_COS[idx]);
          float k = static_cast<float>(5 - tail) * 0.20f * smooth_think;
          it.filled_circle(nx, ny, (tail == 0) ? 4 : (tail <= 2 ? 2 : 1),
                           (tail == 0) ? scale_color(c_ice_white, smooth_think) : scale_color(c_neon_magenta, k));
        }
      }

      // Cognitive Core
      int core_r = 22 + static_cast<int>(4.0f * pulse);
      it.filled_circle(cx, cy, core_r + 4, scale_color(c_neon_magenta, 0.35f));
      it.filled_circle(cx, cy, core_r, c_royal_indigo);
      it.circle(cx, cy, core_r + 1, c_elec_cyan);
      it.filled_circle(cx, cy, (core_r * 2) / 3, c_deep_azure);
      it.filled_circle(cx, cy, core_r / 2, c_ice_white);
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
                     c_royal_indigo, scale_color(c_royal_indigo, 0.35f), false);
    draw_silk_ribbon(base_r, -a2, a3 * 0.9f, -a5, (tick + 15) % 60, (tick * 2) % 60, (tick + 10) % 60,
                     c_neon_magenta, scale_color(c_neon_magenta, 0.35f), false);
    draw_silk_ribbon(base_r - 4.0f, a2 * 0.8f, -a3, a5, (tick + 35) % 60, (tick + 12) % 60, (tick * 2) % 60,
                     c_elec_cyan, c_deep_azure, true);
    draw_silk_ribbon(base_r - 8.0f, a2 * 0.4f, a3 * 0.4f, 0.0f, (tick + 50) % 60, (tick + 30) % 60, 0,
                     c_ice_white, c_elec_cyan, false);

    // Symmetrical Concentric Cognitive Core (matching Thinking aesthetic, zero offset 3D spot)
    int core_r = 23 + static_cast<int>(4.0f * breath + 7.0f * smooth_vad);
    it.filled_circle(cx, cy, core_r + 4, scale_color(c_neon_magenta, 0.30f + 0.25f * smooth_vad));
    it.filled_circle(cx, cy, core_r, c_royal_indigo);
    it.circle(cx, cy, core_r + 1, c_elec_cyan);
    it.filled_circle(cx, cy, (core_r * 2) / 3, lerp_color(c_deep_azure, c_elec_cyan, smooth_vad * 0.5f));
    it.filled_circle(cx, cy, core_r / 2, c_ice_white);

    const char *status_str = va_status_text.empty() ? "Listening..." : va_status_text.c_str();
    it.print(cx, 401, font_small, c_elec_cyan, esphome::display::TextAlign::CENTER, status_str);
    return;
  }

  // ============================================================================
  // C. NEW FLAGSHIP "SOLARIS" — RADIAL ACOUSTIC CORONA & SOLAR ASTROLABE
  //    30-Ray Circular Spectrometer + Golden/Emerald Coronal Waves (LUT-driven)
  // ============================================================================
  const esphome::Color c_gold = col_solaris_gold;                 // #FFDC1E
  const esphome::Color c_amber(255, 105, 15);                     // Molten Solar Amber
  const esphome::Color c_crimson_ember(185, 35, 10);              // Deep Corona Crimson
  const esphome::Color c_emerald(35, 215, 115);                   // Solar Aurora Emerald
  const esphome::Color c_warm_white(255, 248, 215);               // Photosphere White-Gold

  if (is_speaking) {
    float t_sec = static_cast<float>(now_ms % 60000U) * 0.001f;
    float syl_env = (0.55f + 0.25f * sinf(t_sec * 11.5f) + 0.20f * cosf(t_sec * 19.3f)) * smooth_speak;
    if (syl_env < 0.15f) syl_env = 0.15f;
    int tick = static_cast<int>(now_ms / 35U);
    uint32_t speak_elapsed = now_ms - speak_start_ms;

    // 3 Expanding Coronal Shockwave Rings launched progressively from the core
    for (int w = 0; w < 3; w++) {
      uint32_t launch_offset = static_cast<uint32_t>(w) * 600U;
      if (speak_elapsed < launch_offset) continue;
      uint32_t wave_t = (speak_elapsed - launch_offset) % 1800U;
      float prog = static_cast<float>(wave_t) * (1.0f / 1800.0f);
      float fade = sinf(prog * PI_F) * (0.45f + 0.45f * syl_env) * smooth_speak;
      int r_wave = 82 + static_cast<int>(prog * 132.0f);
      esphome::Color ring_c = scale_color(get_va_radiation_color(true, prog + static_cast<float>(w) * 0.33f), fade);
      esphome::Color glow_c = scale_color(ring_c, 0.45f);
      it.circle(cx, cy, r_wave - 1, glow_c);
      it.circle(cx, cy, r_wave, ring_c);
      it.circle(cx, cy, r_wave + 1, glow_c);
    }

    // 30-Ray Circular Acoustic Spectrometer around the Solar Core
    const float r_inner = 42.0f;
    for (int i = 0; i < 60; i += 2) {
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
      it.line(xm, ym, x1, y1, (i % 4 == 0) ? c_gold : c_emerald);
    }

    // Pulsing Photosphere Core
    int core_r = 26 + static_cast<int>(6.0f * syl_env);
    it.filled_circle(cx, cy, core_r + 6, c_crimson_ember);
    it.filled_circle(cx, cy, core_r + 2, c_amber);
    it.filled_circle(cx, cy, core_r - 3, c_gold);
    it.circle(cx, cy, core_r - 5, c_warm_white);
    return;
  }

  if (is_thinking) {
    // 3-Ring Counter-Rotating Solar Astrolabe with Comet Trails
    const int head_1 = static_cast<int>((now_ms / 18U) % 60U);
    const int head_2 = (60 - static_cast<int>((now_ms / 22U) % 60U)) % 60;
    const int head_3 = static_cast<int>((now_ms / 28U) % 60U);

    // Subtle astrolabe meridian rings
    it.circle(cx, cy, 62, scale_color(c_amber, 0.30f * smooth_think));
    it.circle(cx, cy, 90, scale_color(c_gold, 0.25f * smooth_think));
    it.circle(cx, cy, 118, scale_color(c_emerald, 0.25f * smooth_think));

    for (int n = 0; n < 3; n++) {
      // Inner Ring (r = 62, CW, Gold)
      int b1 = (head_1 + n * 20) % 60;
      for (int tail = 5; tail >= 0; tail--) {
        int idx = (b1 - tail * 2 + 60) % 60;
        int nx = cx + static_cast<int>(62.0f * DIAL_SIN[idx]);
        int ny = cy - static_cast<int>(62.0f * DIAL_COS[idx]);
        it.filled_circle(nx, ny, (tail == 0) ? 4 : (tail <= 2 ? 2 : 1),
                         (tail == 0) ? scale_color(c_warm_white, smooth_think) : scale_color(c_gold, (6 - tail) * 0.16f * smooth_think));
      }
      // Middle Ring (r = 90, CCW, Emerald)
      int b2 = (head_2 + n * 20 + 10) % 60;
      for (int tail = 5; tail >= 0; tail--) {
        int idx = (b2 + tail * 2) % 60;
        int nx = cx + static_cast<int>(90.0f * DIAL_SIN[idx]);
        int ny = cy - static_cast<int>(90.0f * DIAL_COS[idx]);
        it.filled_circle(nx, ny, (tail == 0) ? 4 : (tail <= 2 ? 2 : 1),
                         (tail == 0) ? scale_color(c_warm_white, smooth_think) : scale_color(c_emerald, (6 - tail) * 0.16f * smooth_think));
      }
      // Outer Ring (r = 118, CW, Amber)
      int b3 = (head_3 + n * 20 + 5) % 60;
      for (int tail = 4; tail >= 0; tail--) {
        int idx = (b3 - tail * 2 + 60) % 60;
        int nx = cx + static_cast<int>(118.0f * DIAL_SIN[idx]);
        int ny = cy - static_cast<int>(118.0f * DIAL_COS[idx]);
        it.filled_circle(nx, ny, (tail == 0) ? 3 : 2,
                         (tail == 0) ? scale_color(c_gold, smooth_think) : scale_color(c_amber, (5 - tail) * 0.18f * smooth_think));
      }
    }

    float pulse = sinf(static_cast<float>(now_ms % 600U) * (2.0f * PI_F / 600.0f));
    int core_r = 25 + static_cast<int>(4.0f * pulse);
    it.filled_circle(cx, cy, core_r + 5, c_crimson_ember);
    it.filled_circle(cx, cy, core_r + 1, c_amber);
    it.filled_circle(cx, cy, core_r - 3, c_gold);
    it.circle(cx, cy, core_r - 5, c_warm_white);
    return;
  }

  // New Solaris Listening: Breathing 30-Ray Solar Corona + Emerald-Gold VAD Bloom
  float breath = sinf(static_cast<float>(now_ms % 1800U) * (2.0f * PI_F / 1800.0f));
  int tick = static_cast<int>(now_ms / 45U);

  int outer_r = 120 + static_cast<int>(6.0f * breath + 12.0f * smooth_vad);
  it.circle(cx, cy, outer_r, scale_color(c_emerald, 0.45f + 0.45f * smooth_vad));
  it.circle(cx, cy, outer_r - 1, scale_color(c_gold, 0.30f + 0.40f * smooth_vad));

  const float r_inner = 42.0f + 3.0f * breath;
  for (int i = 0; i < 60; i += 2) {
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
    it.line(xm, ym, x1, y1, (smooth_vad > 0.25f && (i % 4 == 0)) ? c_emerald : c_gold);
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
  else if (sim_opt == "Fog / Mist (Day)") { cond = "fog"; is_night = false; }
  else if (sim_opt == "Fog / Mist (Night)") { cond = "fog"; is_night = true; }
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
  if (c_in == "fog" || c_in == "foggy") return "FOG & MIST";
  if (c_in == "mist" || c_in == "misty" || c_in == "haze") return "MIST & HAZE";
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
  if (c == "rainy" || c == "pouring" || c == "heavy-rain" || c == "drizzle") return "Rain";
  if (c == "cloudy" || c == "overcast") return "Cloudy";
  if (c == "snowy") return "Snow";
  if (c == "lightning" || c == "lightning-rainy" || c == "thunderstorm") return "Storm";
  if (c == "snowy-rainy" || c == "sleet") return "Sleet";
  if (c == "windy" || c == "windy-variant" || c == "breezy") return "Windy";
  if (c == "fog" || c == "foggy") return "Fog";
  if (c == "mist" || c == "misty" || c == "haze") return "Mist";
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
  if (c_name == "sunny" || c_name == "clear-day" || c_name == "clear" || c_name == "clearday") {
    it.filled_circle(gx, gy, 7, esphome::Color(250, 185, 30));
    it.circle(gx, gy, 10, esphome::Color(255, 215, 80));
    // 4 cardinal + 4 diagonal micro sun rays
    esphome::Color ray_c(255, 225, 100);
    it.line(gx, gy - 12, gx, gy - 10, ray_c);
    it.line(gx, gy + 10, gx, gy + 12, ray_c);
    it.line(gx - 12, gy, gx - 10, gy, ray_c);
    it.line(gx + 10, gy, gx + 12, gy, ray_c);
  } else if (c_name == "clear-night" || c_name == "clearnight") {
    it.filled_circle(gx + 2, gy, 8, esphome::Color(225, 235, 250));
    it.filled_circle(gx - 1, gy - 1, 7, esphome::Color(0, 0, 0));
  } else if (c_name == "partlycloudy" || c_name == "partly-cloudy" || c_name == "mostlycloudy" || c_name == "mostlysunny" || c_name == "partlysunny") {
    it.filled_circle(gx - 4, gy - 4, 6, esphome::Color(245, 175, 30));
    it.filled_circle(gx + 2, gy + 1, 6, esphome::Color(175, 190, 210));
    it.filled_circle(gx - 3, gy + 2, 5, esphome::Color(155, 170, 190));
    it.filled_circle(gx + 7, gy + 3, 4, esphome::Color(145, 160, 180));
  } else if (c_name == "rainy" || c_name == "pouring" || c_name == "drizzle" || c_name == "heavy-rain") {
    it.filled_circle(gx, gy - 2, 6, esphome::Color(130, 145, 170));
    it.filled_circle(gx - 5, gy - 1, 5, esphome::Color(115, 130, 155));
    it.filled_circle(gx + 5, gy, 4, esphome::Color(115, 130, 155));
    it.line(gx - 4, gy + 6, gx - 6, gy + 10, esphome::Color(70, 170, 245));
    it.line(gx + 2, gy + 6, gx, gy + 10, esphome::Color(70, 170, 245));
  } else if (c_name == "snowy" || c_name == "snowy-rainy" || c_name == "sleet" || c_name == "hail") {
    it.filled_circle(gx, gy - 2, 6, esphome::Color(160, 175, 195));
    it.draw_pixel_at(gx - 3, gy + 7, esphome::Color(255, 255, 255));
    it.draw_pixel_at(gx + 3, gy + 7, esphome::Color(255, 255, 255));
    it.draw_pixel_at(gx, gy + 9, esphome::Color(255, 255, 255));
  } else if (c_name == "lightning" || c_name == "lightning-rainy" || c_name == "thunderstorm") {
    it.filled_circle(gx, gy - 3, 6, esphome::Color(110, 120, 140));
    it.line(gx - 1, gy + 3, gx - 3, gy + 7, esphome::Color(255, 220, 40));
    it.line(gx - 3, gy + 7, gx + 1, gy + 7, esphome::Color(255, 220, 40));
    it.line(gx + 1, gy + 7, gx - 1, gy + 11, esphome::Color(255, 220, 40));
  } else if (c_name == "windy" || c_name == "windy-variant" || c_name == "breezy") {
    it.line(gx - 9, gy - 3, gx + 5, gy - 3, esphome::Color(160, 190, 220));
    it.line(gx - 6, gy + 1, gx + 8, gy + 1, esphome::Color(180, 210, 235));
    it.line(gx - 4, gy + 5, gx + 4, gy + 5, esphome::Color(140, 170, 200));
    // Tiny windblown leaf accent
    it.line(gx + 6, gy - 5, gx + 9, gy - 3, esphome::Color(125, 195, 95));
    it.line(gx + 6, gy - 4, gx + 9, gy - 3, esphome::Color(230, 185, 55));
  } else if (c_name == "fog" || c_name == "foggy" || c_name == "mist" || c_name == "misty" || c_name == "haze") {
    // Layered horizontal mist veils with soft diffused sun/moon glow above
    it.filled_circle(gx, gy - 4, 5, esphome::Color(135, 155, 175));
    it.line(gx - 9, gy - 1, gx + 9, gy - 1, esphome::Color(185, 205, 225));
    it.line(gx - 7, gy + 2, gx + 10, gy + 2, esphome::Color(155, 178, 200));
    it.line(gx - 10, gy + 5, gx + 6, gy + 5, esphome::Color(130, 152, 175));
    it.line(gx - 5, gy + 8, gx + 8, gy + 8, esphome::Color(105, 125, 148));
  } else {
    it.filled_circle(gx, gy - 1, 7, esphome::Color(165, 180, 200));
    it.filled_circle(gx - 5, gy + 1, 5, esphome::Color(145, 160, 180));
    it.filled_circle(gx + 5, gy + 1, 5, esphome::Color(145, 160, 180));
  }
}

// Tracks the epoch offset for the Clear Day Origami bird flock so that switching to
// Clear Day immediately glides a flock across within ~0.5s, then waits calmly between passes!
inline uint32_t &clear_day_flock_epoch_ms() {
  static uint32_t ep = 0;
  return ep;
}

inline uint32_t &clear_day_last_seen_ms() {
  static uint32_t ls = 0;
  return ls;
}

// Origami-Inspired High-Altitude V-Flock of Migrating Birds:
// Glides across at a calm, unhurried pace for 9.2 seconds out of every 20.0-second cycle
// (leaving 10.8 seconds of calm clear sky between flocks).
inline bool is_clear_day_flock_active(uint32_t now_ms) {
  if (now_ms - clear_day_last_seen_ms() > 2500U) {
    // First entry into Clear Day / Partly Cloudy Day: align epoch so a flock enters within 0.4s!
    clear_day_flock_epoch_ms() = now_ms + 19600U;
  }
  clear_day_last_seen_ms() = now_ms;
  uint32_t rel_ms = now_ms - clear_day_flock_epoch_ms();
  return (rel_ms % 20000U) < 9200U;
}

// Tracks the epoch offset for the Night Shooting Star so entering Clear Night / Partly Cloudy Night
// showcases a shooting star within ~1.2s, followed by randomized 5..17s intervals thereafter.
inline uint32_t &night_meteor_epoch_ms() {
  static uint32_t ep = 0;
  return ep;
}

inline uint32_t &night_meteor_last_seen_ms() {
  static uint32_t ls = 0;
  return ls;
}

// Occasional Night Shooting Star (Clear Night & Partly Cloudy Night):
// Streaks across over the Moon, underneath clouds, and underneath all foreground data.
template<typename DisplayT>
inline void render_night_shooting_star(DisplayT &it, uint32_t now_ms, int cy) {
  static constexpr uint32_t METEOR_WINDOW_MS = 11500U;
  static constexpr uint32_t METEOR_DUR_MS    = 780U;

  if (now_ms - night_meteor_last_seen_ms() > 2500U) {
    // Align epoch on entry so the first shooting star appears ~1.2s after opening Clear/Partly Cloudy Night
    night_meteor_epoch_ms() = now_ms - 10300U;
  }
  night_meteor_last_seen_ms() = now_ms;

  uint32_t rel_ms = now_ms - night_meteor_epoch_ms();
  uint32_t slot_idx = rel_ms / METEOR_WINDOW_MS;
  uint32_t slot_ms  = rel_ms % METEOR_WINDOW_MS;

  int seed = static_cast<int>(slot_idx * 59U + 17U);
  // Slot 0 fires at 11500 - 10300 = 1200ms; subsequent slots randomize launch time across 800..7400ms
  uint32_t launch_ms = (slot_idx == 0U)
      ? 1200U
      : (800U + static_cast<uint32_t>(hash2d_01(seed, 7) * 6600.0f));

  if (slot_ms < launch_ms || slot_ms >= launch_ms + METEOR_DUR_MS) return;

  float u = static_cast<float>(slot_ms - launch_ms) * (1.0f / static_cast<float>(METEOR_DUR_MS));
  // Ease-out velocity curve as meteor burns through upper atmosphere
  float u_head = 1.0f - (1.0f - u) * (1.0f - u);
  float u_tail = (u > 0.28f) ? (1.0f - (1.0f - (u - 0.28f)) * (1.0f - (u - 0.28f))) : 0.0f;

  float rx   = hash2d_01(seed, 19);
  float ry   = hash2d_01(seed, 31);
  float rang = hash2d_01(seed, 43);
  float rlen = hash2d_01(seed, 61);
  float rdir = hash2d_01(seed, 79);

  int dir_x = (rdir < 0.52f) ? -1 : 1; // Slightly favor R->L or L->R randomly
  float ang = 0.30f + rang * 0.40f;    // ~17..40 deg downward entry angle
  float sa = 0.0f, ca = 1.0f;
  fast_sincos(ang, sa, ca);
  float vx = static_cast<float>(dir_x) * ca;
  float vy = sa;

  // Origin distributed across upper/mid night sky (including paths that cross over the Moon at 346,91)
  float x0 = (dir_x > 0) ? (75.0f + rx * 210.0f) : (180.0f + rx * 215.0f);
  float y0 = 42.0f + ry * 128.0f;
  float total_dist = 145.0f + rlen * 75.0f;

  float d_head = total_dist * u_head;
  float d_tail = total_dist * u_tail;
  float d_mid1 = d_tail + (d_head - d_tail) * 0.45f;
  float d_mid2 = d_tail + (d_head - d_tail) * 0.78f;

  int hx  = static_cast<int>(x0 + vx * d_head);
  int hy  = static_cast<int>(y0 + vy * d_head);
  int m2x = static_cast<int>(x0 + vx * d_mid2);
  int m2y = static_cast<int>(y0 + vy * d_mid2);
  int m1x = static_cast<int>(x0 + vx * d_mid1);
  int m1y = static_cast<int>(y0 + vy * d_mid1);
  int tx  = static_cast<int>(x0 + vx * d_tail);
  int ty  = static_cast<int>(y0 + vy * d_tail);

  // Fade envelope: crisp ignition, full brilliance through mid-flight, clean burn-out
  float fade = (u < 0.15f) ? (u * (1.0f / 0.15f)) : ((u > 0.72f) ? ((1.0f - u) * (1.0f / 0.28f)) : 1.0f);
  if (fade <= 0.02f) return;

  esphome::Color col_wake = scale_color(esphome::Color(45, 95, 165), fade);
  esphome::Color col_mid  = scale_color(esphome::Color(135, 205, 255), fade);
  esphome::Color col_head = scale_color(esphome::Color(255, 255, 255), fade);

  if (is_inside_bezel(tx, ty, cy, 6) && is_inside_bezel(m1x, m1y, cy, 6)) {
    it.line(tx, ty, m1x, m1y, col_wake);
  }
  if (is_inside_bezel(m1x, m1y, cy, 6) && is_inside_bezel(m2x, m2y, cy, 6)) {
    it.line(m1x, m1y, m2x, m2y, col_mid);
  }
  if (is_inside_bezel(m2x, m2y, cy, 6) && is_inside_bezel(hx, hy, cy, 6)) {
    it.line(m2x, m2y, hx, hy, col_head);
    if (u > 0.10f && u < 0.80f) {
      it.draw_pixel_at(hx, hy - 1, col_mid);
      it.draw_pixel_at(hx, hy + 1, col_mid);
      it.draw_pixel_at(hx - 1, hy, col_mid);
      it.draw_pixel_at(hx + 1, hy, col_mid);
    }
  }
}

// Returns true when the active or simulated weather condition includes live particle or celestial motion.
// Reusable across Sky & Weather (Page 5), future Home Page live backgrounds, and screensavers.
inline bool has_weather_motion(const std::string &cond, bool is_night, uint32_t now_ms = 0) {
  if (cond.find("rain") != std::string::npos ||
      cond.find("pouring") != std::string::npos ||
      cond.find("drizzle") != std::string::npos ||
      cond.find("snow") != std::string::npos ||
      cond.find("sleet") != std::string::npos ||
      cond.find("hail") != std::string::npos ||
      cond.find("lightning") != std::string::npos ||
      cond.find("thunder") != std::string::npos ||
      cond.find("wind") != std::string::npos ||
      cond.find("breez") != std::string::npos ||
      cond.find("fog") != std::string::npos ||
      cond.find("mist") != std::string::npos ||
      cond.find("haze") != std::string::npos ||
      cond.find("partly") != std::string::npos ||
      cond.find("mostlysunny") != std::string::npos ||
      cond.find("partlysunny") != std::string::npos) {
    return true;
  }
  if (is_night && (cond == "clear-night" || cond == "clearnight" ||
                   cond == "sunny" || cond == "clear" || cond == "clear-day")) {
    return true;
  }
  // Daytime Clear / Sunny: only spin up 20 FPS during the 9.2s window when the V-flock of birds is flying across!
  if (!is_night && (cond == "sunny" || cond == "clear-day" || cond == "clear" || cond == "clearday")) {
    return (now_ms == 0) ? true : is_clear_day_flock_active(now_ms);
  }
  return false;
}

// Draws a single articulated high-altitude black-silhouette bird in the Origami V-echelon formation
template<typename DisplayT>
inline void draw_migrating_bird(DisplayT &it, int bx, int by, int dir, float flap_phase, float scale, int cy) {
  if (!is_inside_bezel(bx, by, cy, 10)) return;
  // Smooth articulated wingbeat (inner elbow leads outer wingtip by 0.55 rad)
  float s1 = fast_sin(flap_phase);
  float s2 = fast_sin(flap_phase - 0.55f);
  int w_in  = static_cast<int>(4.5f * scale);
  int w_out = static_cast<int>(8.5f * scale);
  int dy_in  = static_cast<int>(-3.2f * scale * s1) - 1;
  int dy_out = dy_in + static_cast<int>(-3.6f * scale * s2);

  // Subtle dark silhouette palette (crisp charcoal-black wing spine + feathered dark-slate edge)
  esphome::Color col_core(6, 10, 16);
  esphome::Color col_edge(18, 32, 48);

  // Left & Right 2-segment arched wings (tilted slightly into flight direction `dir`)
  int lx_m = bx - w_in,  ly_m = by + dy_in - (dir > 0 ? 1 : 0);
  int lx_t = bx - w_out, ly_t = by + dy_out - (dir > 0 ? 2 : 0);
  int rx_m = bx + w_in,  ry_m = by + dy_in - (dir < 0 ? 1 : 0);
  int rx_t = bx + w_out, ry_t = by + dy_out - (dir < 0 ? 2 : 0);

  // Soft 1px feathered silhouette edge pass
  it.line(bx, by + 1, lx_m, ly_m + 1, col_edge);
  it.line(lx_m, ly_m + 1, lx_t, ly_t + 1, col_edge);
  it.line(bx, by + 1, rx_m, ry_m + 1, col_edge);
  it.line(rx_m, ry_m + 1, rx_t, ry_t + 1, col_edge);

  // Crisp black silhouette upper wing arch + body center
  it.line(bx, by, lx_m, ly_m, col_core);
  it.line(lx_m, ly_m, lx_t, ly_t, col_core);
  it.line(bx, by, rx_m, ry_m, col_core);
  it.line(rx_m, ry_m, rx_t, ry_t, col_core);
}

// Draws a single tumbling, fluttering windblown leaf with pointed blade, midrib vein, and petiole stem
template<typename DisplayT>
inline void draw_tumbling_leaf(DisplayT &it, int lx, int ly, float ang, float flutter,
                               float scale, esphome::Color col_main, esphome::Color col_vein, int cy) {
  if (!is_inside_bezel(lx, ly, cy, 8)) return;
  float sa = 0.0f, ca = 1.0f;
  fast_sincos(ang, sa, ca);

  float len = 7.0f * scale;
  float wid = (3.8f * scale) * (0.32f + 0.68f * flutter);

  int tx = lx + static_cast<int>(len * ca);
  int ty = ly + static_cast<int>(len * sa);
  int bx = lx - static_cast<int>(len * ca);
  int by = ly - static_cast<int>(len * sa);

  // Asymmetric leaf belly (slightly wider toward base)
  int mx = lx - static_cast<int>(1.2f * scale * ca);
  int my = ly - static_cast<int>(1.2f * scale * sa);
  int ux = mx - static_cast<int>(wid * sa);
  int uy = my + static_cast<int>(wid * ca);
  int dx = mx + static_cast<int>(wid * sa);
  int dy = my - static_cast<int>(wid * ca);

  // Leaf blade outline + inner chord fill
  it.line(bx, by, ux, uy, col_main);
  it.line(ux, uy, tx, ty, col_main);
  it.line(bx, by, dx, dy, col_main);
  it.line(dx, dy, tx, ty, col_main);
  it.line(ux, uy, dx, dy, col_main);
  // Central midrib vein + short petiole stem at base
  it.line(bx, by, tx, ty, col_vein);
  int sx = bx - static_cast<int>(3.0f * scale * ca);
  int sy = by - static_cast<int>(3.0f * scale * sa);
  it.line(bx, by, sx, sy, col_vein);
}

// Modular Full-Screen Atmospheric & Weather Motion Wallpaper Engine:
// Reusable across Sky & Weather (Page 5) and any future Home Page or Weather Screensaver.
// Exact Z-Order Pipeline (Back to Front):
//   0. Sky Canvas -> 1A. 64 Twinkling Night Stars -> 1B. Celestial Sun/Moon Icon ->
//   1C. Night Shooting Star (over Moon, under clouds & data) ->
//   1D. High-Altitude Black Silhouette Birds (over Sun, through hero temp zone, under clouds & data) ->
//   1E. Volumetric Clouds / Calm Fog & Mist / Windy Leaves / Rain / Snow / Lightning ->
//   (Foreground Hero Temperature & Telemetry rendered on top by caller)
template<typename DisplayT, typename SpanRangeT>
inline void render_weather_motion_wallpaper(DisplayT &it, int cx, int cy, uint32_t now_ms,
                                            const std::string &cond, bool is_night,
                                            const SpanRangeT &cloud_spans,
                                            double moon_cycle = 0.5,
                                            int cel_cx = 346, int cel_cy = 91,
                                            bool mask_hero_temp = true) {
  if (has_weather_motion(cond, is_night, now_ms)) {
    record_ambient_frame_ms(now_ms);
  }

  const bool is_fog_mist = (cond.find("fog") != std::string::npos ||
                            cond.find("mist") != std::string::npos ||
                            cond.find("haze") != std::string::npos);
  const bool is_windy = (cond.find("wind") != std::string::npos ||
                         cond.find("breez") != std::string::npos);
  const bool is_clear_day = (!is_night && (cond == "sunny" || cond == "clear-day" ||
                                           cond == "clear" || cond == "clearday"));
  const bool is_partly = (cond.find("partly") != std::string::npos ||
                          cond.find("mostlysunny") != std::string::npos ||
                          cond.find("partlysunny") != std::string::npos);
  const bool is_clear_or_partly_night =
      (is_night && (cond == "clear-night" || cond == "clearnight" ||
                    cond == "sunny" || cond == "clear" || cond == "clear-day" || is_partly));

  // LAYER 0: ATMOSPHERIC SKY CANVAS
  if (!is_night) {
    esphome::Color day_bg(28, 54, 84); // Clear / Calm daylight sky
    if (is_fog_mist) {
      day_bg = esphome::Color(32, 46, 60); // Cool coastal silver-slate fog sky
    } else if (cond.find("cloud") != std::string::npos && !is_partly) {
      day_bg = esphome::Color(22, 38, 56); // Overcast slate sky
    } else if (cond.find("rain") != std::string::npos ||
               cond.find("lightning") != std::string::npos ||
               cond.find("thunder") != std::string::npos) {
      day_bg = esphome::Color(18, 30, 46); // Stormy dark sky
    }
    it.filled_circle(cx, cy, 228, day_bg);
  } else if (is_fog_mist) {
    it.filled_circle(cx, cy, 228, esphome::Color(10, 15, 22));
  }

  // LAYER 1A: FULL-SCREEN NIGHT STARFIELD (64 twinkling stars via LUT fast_sin)
  if (is_clear_or_partly_night) {
    float t_star = static_cast<float>(now_ms % 62832U) * 0.003f;
    for (int k = 0; k < 64; k++) {
      int sx = WEATHER_STARS_X[k];
      int sy = WEATHER_STARS_Y[k];
      if (mask_hero_temp && sx >= 165 && sx <= 302 && sy >= 89 && sy <= 144) continue;
      if ((sx - cel_cx) * (sx - cel_cx) + (sy - cel_cy) * (sy - cel_cy) <= 784) continue;

      float twinkle = fast_sin(t_star + static_cast<float>(k) * 0.8f);
      uint8_t br = static_cast<uint8_t>(155 + static_cast<int>(90.0f * twinkle));
      esphome::Color star_c = (k % 3 == 0)
          ? esphome::Color(br, static_cast<uint8_t>(br * 0.94f), br)
          : esphome::Color(static_cast<uint8_t>(br * 0.75f), static_cast<uint8_t>(br * 0.85f), br);
      it.draw_pixel_at(sx, sy, star_c);
      if (k % 7 == 0 && br > 215) {
        esphome::Color gl_c(160, 190, 230);
        it.draw_pixel_at(sx + 1, sy, gl_c);
        it.draw_pixel_at(sx - 1, sy, gl_c);
        it.draw_pixel_at(sx, sy + 1, gl_c);
        it.draw_pixel_at(sx, sy - 1, gl_c);
      }
    }
  }

  // LAYER 1B: CELESTIAL CORNER POCKET ICON (SUN / MOON)
  // Drawn BEFORE Shooting Stars, Clear-Day Birds, and Clouds/Fog so:
  // - Shooting stars streak OVER the Moon (and UNDER clouds & data)
  // - Black-silhouette birds fly IN FRONT of the Sun (and UNDER clouds & data)
  if (cel_cx > 0 && cel_cy > 0) {
    if (is_night) {
      render_lunar_disc(it, cel_cx, cel_cy, 22, moon_cycle, false);
      if (is_fog_mist) {
        it.circle(cel_cx, cel_cy, 25, esphome::Color(55, 70, 88));
        it.circle(cel_cx, cel_cy, 29, esphome::Color(38, 50, 65));
      }
    } else {
      bool show_sun = (is_clear_day || is_partly || is_windy);
      if (show_sun) {
        it.filled_circle(cel_cx, cel_cy, 20, esphome::Color(245, 165, 25));
        it.filled_circle(cel_cx, cel_cy, 12, esphome::Color(255, 225, 90));
        it.circle(cel_cx, cel_cy, 26, esphome::Color(140, 85, 15));
      } else if (is_fog_mist) {
        it.filled_circle(cel_cx, cel_cy, 19, esphome::Color(135, 125, 92));
        it.filled_circle(cel_cx, cel_cy, 11, esphome::Color(188, 178, 135));
        it.circle(cel_cx, cel_cy, 24, esphome::Color(92, 106, 122));
        it.circle(cel_cx, cel_cy, 28, esphome::Color(68, 84, 102));
      }
    }
  }

  // LAYER 1C: OCCASIONAL NIGHT SHOOTING STAR (Clear Night & Partly Cloudy Night)
  // Streaks over the Moon & stars, underneath Partly Cloudy clouds, and underneath all foreground data!
  if (is_clear_or_partly_night) {
    render_night_shooting_star(it, now_ms, cy);
  }

  // LAYER 1D: CLEAR DAY (AND PARTLY CLOUDY DAY) HIGH-ALTITUDE BLACK-SILHOUETTE V-FLOCK OF BIRDS
  // Flies at high altitude through the hero temperature & sun zone (y = 74..140), in front of the Sun,
  // and underneath all foreground data!
  if (is_clear_day || (!is_night && is_partly)) {
    if (is_clear_day_flock_active(now_ms)) {
      uint32_t rel_ms = now_ms - clear_day_flock_epoch_ms();
      uint32_t cycle_idx = rel_ms / 20000U;
      uint32_t flight_ms = rel_ms % 20000U;
      float prog = static_cast<float>(flight_ms) * (1.0f / 9200.0f); // Calm 9.2s transit across sky

      // Deterministic per-pass direction (Left->Right vs Right->Left) and high-altitude corridor
      int dir = ((cycle_idx & 1U) == 0U) ? 1 : -1;
      float r_alt = hash2d_01(static_cast<int>(cycle_idx * 31U + 7U), 19);
      // High-altitude band (y = 74..138): flies through the hero temperature & sun region, never down below!
      int base_y = 74 + static_cast<int>(r_alt * 64.0f);

      int start_x = (dir > 0) ? 20 : 446;
      int end_x   = (dir > 0) ? 456 : 10;
      int lead_x  = start_x + static_cast<int>((end_x - start_x) * prog);
      int lead_y  = base_y + static_cast<int>(fast_sin(prog * PI_F) * -6.0f);

      float t_flap = now_ms * 0.0052f; // Subtle, unhurried wingbeat cadence
      static constexpr int8_t V_OFF[5][2] = {
        {  0,   0}, // Lead bird at apex
        {-20, -10}, // Upper echelon #1
        {-23,  11}, // Lower echelon #1
        {-41, -19}, // Upper echelon #2
        {-46,  21}  // Lower echelon #2 (asymmetrical trailing goose)
      };
      static constexpr float V_SCALE[5] = {1.05f, 0.96f, 0.96f, 0.88f, 0.86f};

      for (int b = 0; b < 5; b++) {
        int bx = lead_x + dir * V_OFF[b][0];
        int by = lead_y + V_OFF[b][1];
        draw_migrating_bird(it, bx, by, dir, t_flap - static_cast<float>(b) * 0.75f, V_SCALE[b], cy);
      }
    }
  }

  // LAYER 1E: VOLUMETRIC CUMULUS (Partly Cloudy with Gentle Buoyant Drift) & RLE OVERCAST CANOPY
  // Rendered after Shooting Stars and Birds so clouds sit naturally in front of meteors/birds and behind data!
  if (cond.find("cloud") != std::string::npos || cond.find("overcast") != std::string::npos) {
    if (is_partly) {
      float t_cld = now_ms * 0.0008f;
      int dx0 = static_cast<int>(fast_sin(t_cld) * 6.5f);
      int dy0 = static_cast<int>(fast_cos(t_cld * 1.3f) * 2.0f);
      int dx1 = static_cast<int>(fast_cos(t_cld * 0.9f + 1.8f) * 6.5f);
      int dy1 = static_cast<int>(fast_sin(t_cld * 1.2f + 1.1f) * 2.0f);
      int dx2 = static_cast<int>(fast_sin(t_cld * 1.1f + 3.6f) * 6.5f);
      int dy2 = static_cast<int>(fast_cos(t_cld * 1.4f + 2.4f) * 2.0f);

      render_cumulus_cloud(it,  95 + dx0,  95 + dy0, 0.95f, false, is_night);
      render_cumulus_cloud(it, 370 + dx1, 180 + dy1, 0.85f, false, is_night);
      render_cumulus_cloud(it, 100 + dx2, 255 + dy2, 0.80f, false, is_night);
    } else {
      esphome::Color c_shad = is_night ? esphome::Color(12, 16, 22) : esphome::Color(24, 38, 56);
      esphome::Color c_body = is_night ? esphome::Color(20, 28, 38) : esphome::Color(38, 56, 78);
      esphome::Color c_mid  = is_night ? esphome::Color(28, 38, 50) : esphome::Color(46, 66, 90);
      esphome::Color c_high = is_night ? esphome::Color(38, 50, 66) : esphome::Color(56, 78, 104);
      esphome::Color c_crst = is_night ? esphome::Color(58, 74, 98) : esphome::Color(76, 104, 134);
      esphome::Color c_brgt = is_night ? esphome::Color(68, 86, 112) : esphome::Color(90, 120, 154);
      esphome::Color pal[6] = {c_shad, c_body, c_mid, c_high, c_crst, c_brgt};

      for (uint32_t v : cloud_spans) {
        int sy = static_cast<int>(v & 0x1FFU);
        int sx = static_cast<int>((v >> 9) & 0x1FFU);
        int slen = static_cast<int>((v >> 18) & 0x1FFU);
        int scol = static_cast<int>((v >> 27) & 0x7U);
        it.horizontal_line(sx, sy, slen, pal[scol]);
      }
    }
  }

  // LAYER 1F: FOG & MIST — 6 CALM, MELLOW GLIDING BAY FOG BANKS + SUBTLE MIST MOTES
  if (is_fog_mist) {
    // Slowed down ~3.7x (0.00015f vs 0.00055f) with gentle ±22px horizontal glide and subtle row variation
    float t_fog = now_ms * 0.00015f;
    static constexpr int16_t FOG_Y[6]  = { 66, 108, 154, 210, 252, 294};
    static constexpr int16_t FOG_W[6]  = {230, 275, 255, 290, 260, 220};
    static constexpr int8_t  FOG_H[6]  = { 12,  14,  13,  15,  13,  11};
    static constexpr int16_t FOG_X0[6] = {195, 245, 215, 235, 220, 245};

    for (int b = 0; b < 6; b++) {
      float dir_s = (b & 1) ? -1.0f : 1.0f;
      int drift_x = static_cast<int>(fast_sin(t_fog * (0.72f + b * 0.06f) + b * 1.4f) * 22.0f * dir_s);
      int bank_cx = FOG_X0[b] + drift_x;
      int bank_cy = FOG_Y[b] + static_cast<int>(fast_cos(t_fog * 0.65f + b * 2.1f) * 1.5f);
      int half_h  = FOG_H[b];
      int base_hw = FOG_W[b] >> 1;

      for (int dy = -half_h; dy <= half_h; dy += 2) {
        int py = bank_cy + dy;
        int max_hw = circle_half_span(py, 6);
        if (max_hw <= 10) continue;

        // Vertical bell-curve profile (1.0 at center of bank -> 0.2 at feathered top/bottom edges)
        float norm_y = static_cast<float>(dy < 0 ? -dy : dy) / static_cast<float>(half_h + 1);
        float env = 1.0f - norm_y * norm_y;

        // Subtle, mellow horizontal wave shift per scanline
        int wave_shift = static_cast<int>(fast_sin(py * 0.07f + t_fog * 1.1f + b) * 7.0f);
        int row_hw = static_cast<int>(static_cast<float>(base_hw) * (0.55f + 0.45f * env));
        int x0 = bank_cx + wave_shift - row_hw;
        int x1 = bank_cx + wave_shift + row_hw;
        if (x0 < cx - max_hw) x0 = cx - max_hw;
        if (x1 > cx + max_hw) x1 = cx + max_hw;
        if (x1 <= x0 + 4) continue;

        uint8_t r_c = static_cast<uint8_t>((is_night ? 22.0f : 46.0f) + (is_night ? 32.0f : 56.0f) * env);
        uint8_t g_c = static_cast<uint8_t>((is_night ? 30.0f : 62.0f) + (is_night ? 40.0f : 64.0f) * env);
        uint8_t b_c = static_cast<uint8_t>((is_night ? 42.0f : 78.0f) + (is_night ? 48.0f : 68.0f) * env);
        it.horizontal_line(x0, py, x1 - x0, esphome::Color(r_c, g_c, b_c));
      }
    }

    // 18 Suspended Shimmering Mist Droplets drifting very slowly across the fog
    for (int m = 0; m < 18; m++) {
      int mx = (WEATHER_STARS_X[m * 3] + static_cast<int>((now_ms / 190U) + m * 23U)) % 380 + 43;
      int my = WEATHER_STARS_Y[m * 3] + static_cast<int>(fast_sin(t_fog * 1.4f + m) * 3.0f);
      if (!is_inside_bezel(mx, my, cy, 10)) continue;
      float shimmer = 0.5f + 0.5f * fast_sin(now_ms * 0.0011f + m * 1.1f);
      uint8_t mb = static_cast<uint8_t>((is_night ? 75.0f : 125.0f) + 55.0f * shimmer);
      esphome::Color m_col(static_cast<uint8_t>(mb * 0.82f), static_cast<uint8_t>(mb * 0.92f), mb);
      it.draw_pixel_at(mx, my, m_col);
      it.draw_pixel_at(mx + 1, my, m_col);
    }
  }

  // LAYER 1G: WINDY — SWEEPING AIR-CURRENT RIBBONS & 10 TUMBLING AUTUMN/FOREST LEAVES (UNTOUCHED)
  if (is_windy) {
    float t_sec = now_ms * 0.001f;

    // 1. 3 Translucent S-Curved Wind-Gust Streamlines sweeping across the sky
    static constexpr int16_t GUST_Y[3] = { 82, 168, 236};
    esphome::Color gust_hi  = is_night ? esphome::Color(70, 98, 130)  : esphome::Color(115, 155, 195);
    esphome::Color gust_dim = is_night ? esphome::Color(38, 56,  78)  : esphome::Color( 62,  92, 128);
    for (int g = 0; g < 3; g++) {
      int gx_head = ((static_cast<int>(now_ms / 5U) + g * 185) % 560) - 50;
      int gy_base = GUST_Y[g];
      int prev_x = 0, prev_y = 0;
      for (int s = 0; s <= 6; s++) {
        int sx = gx_head - s * 18;
        int sy = gy_base + static_cast<int>(fast_sin(sx * 0.025f + g * 1.7f) * 7.0f);
        if (s > 0 && is_inside_bezel(sx, sy, cy, 8) && is_inside_bezel(prev_x, prev_y, cy, 8)) {
          esphome::Color seg_c = (s >= 2 && s <= 4) ? gust_hi : gust_dim;
          it.line(sx, sy, prev_x, prev_y, seg_c);
        }
        prev_x = sx;
        prev_y = sy;
      }
    }

    // 2. 10 Tumbling, Swooping Windblown Leaves with 3D Flutter & Midrib Veins
    static constexpr uint8_t LEAF_SPD[10] = {155, 195, 140, 215, 175, 160, 205, 148, 185, 225};
    static constexpr int16_t LEAF_Y0[10]  = { 64,  96, 128, 162, 194, 224, 252, 282, 112, 238};
    for (int i = 0; i < 10; i++) {
      int lx = ((RAIN_X[i * 3] + static_cast<int>((static_cast<uint64_t>(now_ms) * LEAF_SPD[i]) / 1000ULL)) % 490) - 12;
      float gust_wave = fast_sin(t_sec * 2.6f + static_cast<float>(i) * 0.9f) * 16.0f +
                        fast_cos(lx * 0.018f + static_cast<float>(i)) * 8.0f;
      int ly = LEAF_Y0[i] + static_cast<int>(gust_wave);

      float spin_ang = t_sec * (2.8f + (i % 4) * 0.65f) + static_cast<float>(i) * 1.2f;
      float flutter  = fabsf(fast_cos(t_sec * (4.2f + (i % 3) * 0.8f) + static_cast<float>(i)));
      float l_scale  = (i % 3 == 0) ? 0.78f : 1.02f;

      esphome::Color c_leaf, c_vein;
      float night_scale = is_night ? 0.72f : 1.0f;
      int pal_id = i & 3;
      if (pal_id == 0) {
        c_leaf = scale_color(esphome::Color(110, 182, 82), night_scale);
        c_vein = scale_color(esphome::Color(165, 225, 130), night_scale);
      } else if (pal_id == 1) {
        c_leaf = scale_color(esphome::Color(235, 188, 48), night_scale);
        c_vein = scale_color(esphome::Color(255, 230, 120), night_scale);
      } else if (pal_id == 2) {
        c_leaf = scale_color(esphome::Color(228, 118, 42), night_scale);
        c_vein = scale_color(esphome::Color(255, 175, 95), night_scale);
      } else {
        c_leaf = scale_color(esphome::Color(82, 152, 92), night_scale);
        c_vein = scale_color(esphome::Color(135, 205, 145), night_scale);
      }

      draw_tumbling_leaf(it, lx, ly, spin_ang, flutter, l_scale, c_leaf, c_vein, cy);
    }
  }

  // LAYER 1H: PRE-SCATTERED FALLING RAIN (26 or 40 angled streaks)
  if (cond.find("rain") != std::string::npos ||
      cond.find("pouring") != std::string::npos ||
      cond.find("drizzle") != std::string::npos) {
    bool is_heavy = (cond == "pouring" || cond.find("heavy") != std::string::npos);
    int count = is_heavy ? 40 : 26;
    esphome::Color rain_c1 = is_night ? esphome::Color(120, 160, 215) : esphome::Color(145, 185, 230);
    esphome::Color rain_c2 = is_night ? esphome::Color(60, 90, 130)   : esphome::Color(90, 130, 175);
    int extra_len = is_heavy ? 4 : 0;
    for (int i = 0; i < count; i++) {
      int ry = (RAIN_Y[i] + static_cast<int>((static_cast<uint64_t>(now_ms) * static_cast<uint32_t>(RAIN_SPD[i])) / 1000ULL)) % 435 + 15;
      int rx = (RAIN_X[i] - (ry - 15) / 5) % 410 + 28;
      int len_r = RAIN_LEN[i] + extra_len;
      it.line(rx, ry, rx - 3, ry + len_r, (i & 1) ? rain_c1 : rain_c2);
    }
  }

  // LAYER 1I: CRYSTALLINE SNOWFLAKES (36 multi-tier flakes with LUT fast_sin lateral drift)
  if (cond.find("snow") != std::string::npos || cond.find("sleet") != std::string::npos) {
    float t_snow = static_cast<float>(now_ms % 62832U) * 0.0022f;
    esphome::Color c_core(255, 255, 255);
    esphome::Color c_arm2 = is_night ? esphome::Color(215, 235, 255) : esphome::Color(230, 245, 255);
    esphome::Color c_tip2 = is_night ? esphome::Color(160, 195, 235) : esphome::Color(180, 215, 245);
    esphome::Color c_tip1 = is_night ? esphome::Color(180, 215, 245) : esphome::Color(195, 225, 250);
    esphome::Color c_dot0 = is_night ? esphome::Color(190, 220, 250) : esphome::Color(210, 235, 255);
    for (int i = 0; i < 36; i++) {
      int sy = (SNOW_Y[i] + static_cast<int>((static_cast<uint64_t>(now_ms) * static_cast<uint32_t>(SNOW_SPD[i])) / 1000ULL)) % 430 + 15;
      int sx = SNOW_X[i] + static_cast<int>(fast_sin(t_snow + SNOW_PHASE[i]) * 8.0f);
      if (sx < 2 || sx > 463 || sy < 2 || sy > 463) continue;

      if (SNOW_TYPE[i] == 2) {
        it.draw_pixel_at(sx, sy, c_core);
        it.draw_pixel_at(sx + 1, sy, c_arm2); it.draw_pixel_at(sx + 2, sy, c_tip2);
        it.draw_pixel_at(sx - 1, sy, c_arm2); it.draw_pixel_at(sx - 2, sy, c_tip2);
        it.draw_pixel_at(sx, sy + 1, c_arm2); it.draw_pixel_at(sx, sy + 2, c_tip2);
        it.draw_pixel_at(sx, sy - 1, c_arm2); it.draw_pixel_at(sx, sy - 2, c_tip2);
        it.draw_pixel_at(sx + 1, sy + 1, c_tip2); it.draw_pixel_at(sx - 1, sy - 1, c_tip2);
        it.draw_pixel_at(sx + 1, sy - 1, c_tip2); it.draw_pixel_at(sx - 1, sy + 1, c_tip2);
      } else if (SNOW_TYPE[i] == 1) {
        it.draw_pixel_at(sx, sy, c_core);
        it.draw_pixel_at(sx + 1, sy, c_tip1);
        it.draw_pixel_at(sx - 1, sy, c_tip1);
        it.draw_pixel_at(sx, sy + 1, c_tip1);
        it.draw_pixel_at(sx, sy - 1, c_tip1);
      } else {
        it.draw_pixel_at(sx, sy, c_dot0);
        it.draw_pixel_at(sx + 1, sy, c_dot0);
        it.draw_pixel_at(sx, sy + 1, c_dot0);
        it.draw_pixel_at(sx + 1, sy + 1, c_dot0);
      }
    }
  }

  // LAYER 1J: THUNDERSTORM BRANCHING LIGHTNING STRIKE
  if (cond.find("lightning") != std::string::npos || cond.find("thunder") != std::string::npos) {
    uint32_t flash_step = (now_ms / 250U) % 18U; // Repeats every 4.5 seconds
    if (flash_step <= 1U) {
      if (flash_step == 0U) {
        it.filled_circle(cx, cy, 228, is_night ? esphome::Color(42, 58, 85) : esphome::Color(72, 98, 135));
      }
      esphome::Color bolt_c(255, 255, 255);
      esphome::Color glow_c(180, 220, 255);
      it.line(245, 25, 235, 60, bolt_c);
      it.line(235, 60, 252, 95, bolt_c);
      it.line(252, 95, 220, 140, bolt_c);
      it.line(220, 140, 195, 190, bolt_c);
      it.line(195, 190, 165, 250, bolt_c);
      it.line(235, 60, 275, 90, glow_c);
      it.line(275, 90, 290, 125, glow_c);
      it.line(220, 140, 180, 175, glow_c);
      it.line(195, 190, 215, 225, glow_c);
    }
  }
}

}  // namespace pocket_ui

using namespace pocket_ui;
