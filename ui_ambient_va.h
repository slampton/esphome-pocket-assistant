#pragma once

// ==============================================================================
// Pocket Assistant 20 FPS Docked Screensavers & Voice Assistant Engine (ui_ambient_va.h)
// ==============================================================================

#include "ui_core.h"

namespace pocket_ui {

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
  float t = now_ms * 0.0010f;

  // 1. Subtle Polar Night Starfield in the upper/mid sky dome (shines softly behind the aurora veils)
  float t_twinkle = now_ms * 0.0025f;
  for (int k = 0; k < 28; k++) {
    float r_norm = sqrtf((static_cast<float>(k) + 0.5f) * (1.0f / 28.0f));
    float r_px = 24.0f + r_norm * 196.0f;
    float ang = static_cast<float>(k) * 2.39996323f + 0.4f;
    float sa = 0.0f, ca = 1.0f;
    fast_sincos(ang, sa, ca);
    int sx = cx + static_cast<int>(ca * r_px);
    int sy = cy - static_cast<int>( std::fabs(sa) * r_px * 0.85f) + 18; // Concentrated in upper/mid polar sky
    if (!is_inside_bezel(sx, sy, cy, 8)) continue;
    uint8_t br = static_cast<uint8_t>(110 + static_cast<int>(55.0f * fast_sin(t_twinkle + k * 1.1f)));
    it.draw_pixel_at(sx, sy, esphome::Color(static_cast<uint8_t>(br * 0.82f), static_cast<uint8_t>(br * 0.90f), br));
  }

  // 2. 3 Full-Bodied Polar Curtains with Zero-Division Soft-Max Overlap Blending
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

    // Zero-Division Hue-Preserving Soft-Max Blend:
    // For each RGB channel, takes max(c1, c2, c3) + 25% of secondary contributions ((sum - max) >> 2).
    // Eliminates 3 hardware divisions per pixel (~100,000 divisions/frame saved!) while preserving
    // deep indigo, royal blue, and seafoam overlap blends without white saturation.
    for (int py = col_bot; py >= col_top; py -= 2) {
      int r_sum = 0, g_sum = 0, b_sum = 0;
      int r_max = 0, g_max = 0, b_max = 0;

      if (py <= y1 && py >= top1) {
        uint32_t w1 = AURORA_PLUME_LUT[(((y1 - py) * step1_fp) >> 10) & 63];
        r_sum = r_max = (p1_r * static_cast<int>(w1)) >> 8;
        g_sum = g_max = (p1_g * static_cast<int>(w1)) >> 8;
        b_sum = b_max = (p1_b * static_cast<int>(w1)) >> 8;
      }
      if (py <= y2 && py >= top2) {
        uint32_t w2 = AURORA_PLUME_LUT[(((y2 - py) * step2_fp) >> 10) & 63];
        int r2 = (p2_r * static_cast<int>(w2)) >> 8;
        int g2 = (p2_g * static_cast<int>(w2)) >> 8;
        int b2 = (p2_b * static_cast<int>(w2)) >> 8;
        r_sum += r2; if (r2 > r_max) r_max = r2;
        g_sum += g2; if (g2 > g_max) g_max = g2;
        b_sum += b2; if (b2 > b_max) b_max = b2;
      }
      if (py <= y3 && py >= top3) {
        uint32_t w3 = AURORA_PLUME_LUT[(((y3 - py) * step3_fp) >> 10) & 63];
        int r3 = (p3_r * static_cast<int>(w3)) >> 8;
        int g3 = (p3_g * static_cast<int>(w3)) >> 8;
        int b3 = (p3_b * static_cast<int>(w3)) >> 8;
        r_sum += r3; if (r3 > r_max) r_max = r3;
        g_sum += g3; if (g3 > g_max) g_max = g3;
        b_sum += b3; if (b3 > b_max) b_max = b3;
      }

      int r = r_max + ((r_sum - r_max) >> 2);
      int g = g_max + ((g_sum - g_max) >> 2);
      int b = b_max + ((b_sum - b_max) >> 2);

      if ((r | g | b) > 5) {
        uint8_t cr = static_cast<uint8_t>(r > 255 ? 255 : r);
        uint8_t cg = static_cast<uint8_t>(g > 255 ? 255 : g);
        uint8_t cb = static_cast<uint8_t>(b > 255 ? 255 : b);
        it.draw_pixel_at(x, py, esphome::Color(cr, cg, cb));
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
  struct Star3D { float x, y, z; uint8_t tint; };
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
    float dist = 36.0f + star_rand() * 640.0f;
    stars[i].x = fast_cos(ang) * dist;
    stars[i].y = fast_sin(ang) * dist;
    stars[i].z = init_z ? (35.0f + star_rand() * 920.0f) : (840.0f + star_rand() * 140.0f);
    stars[i].tint = static_cast<uint8_t>(i & 3);
  };
  if (!stars_init) {
    for (int i = 0; i < 90; i++) respawn_star(i, true);
    stars_init = true;
    last_star_ms = now_ms;
  }

  float speed = 13.5f;
  if (now_ms > 0) {
    uint32_t dt = now_ms - last_star_ms;
    last_star_ms = now_ms;
    if (dt >= 20U && dt <= 300U) {
      speed = static_cast<float>(dt) * 0.26f;
    }
  }

  // Gentle 3D spacecraft banking & vanishing-point drift for fluid 3D flight feel
  float t_flight = now_ms * 0.00045f;
  int vp_x = cx + static_cast<int>(fast_sin(t_flight) * 24.0f);
  int vp_y = cy + static_cast<int>(fast_cos(t_flight * 0.8f) * 18.0f);
  float bank_s = 0.0f, bank_c = 1.0f;
  fast_sincos(0.28f * fast_sin(t_flight * 0.7f), bank_s, bank_c);

  for (int i = 0; i < 90; i++) {
    stars[i].z -= speed;
    if (stars[i].z <= 28.0f) {
      respawn_star(i, false);
      continue;
    }

    // Rotate star (x, y) by gentle 3D spacecraft bank angle
    float rx = stars[i].x * bank_c - stars[i].y * bank_s;
    float ry = stars[i].x * bank_s + stars[i].y * bank_c;

    float k = 230.0f / stars[i].z;
    int sx = vp_x + static_cast<int>(rx * k);
    int sy = vp_y + static_cast<int>(ry * k);
    if (!is_inside_bezel(sx, sy, cy, 6)) {
      respawn_star(i, false);
      continue;
    }

    // Smooth continuous depth-luminance ramp (0.12 at z=960 -> 1.0 at z=30: zero bucket popping!)
    float depth_u = fmaxf(0.0f, fminf(1.0f, 1.0f - (stars[i].z * (1.0f / 960.0f))));
    float lum = 0.14f + 0.86f * depth_u * (0.45f + 0.55f * depth_u);

    // Subtle stellar spectral tints (0: Ice-Cyan, 1: Pure White-Blue, 2: Solar Gold, 3: Soft Rose-Amber)
    uint8_t sr, sg, sb;
    if (stars[i].tint == 0) {
      sr = static_cast<uint8_t>(165.0f * lum); sg = static_cast<uint8_t>(235.0f * lum); sb = static_cast<uint8_t>(255.0f * lum);
    } else if (stars[i].tint == 2) {
      sr = static_cast<uint8_t>(255.0f * lum); sg = static_cast<uint8_t>(230.0f * lum); sb = static_cast<uint8_t>(175.0f * lum);
    } else if (stars[i].tint == 3) {
      sr = static_cast<uint8_t>(255.0f * lum); sg = static_cast<uint8_t>(195.0f * lum); sb = static_cast<uint8_t>(215.0f * lum);
    } else {
      sr = static_cast<uint8_t>(215.0f * lum); sg = static_cast<uint8_t>(235.0f * lum); sb = static_cast<uint8_t>(255.0f * lum);
    }
    esphome::Color head_col(sr, sg, sb);

    if (depth_u < 0.28f) {
      it.draw_pixel_at(sx, sy, head_col);
    } else {
      float tail_z = stars[i].z + speed * (1.2f + 1.4f * depth_u);
      float prev_k = 230.0f / tail_z;
      int px = vp_x + static_cast<int>(rx * prev_k);
      int py = vp_y + static_cast<int>(ry * prev_k);
      esphome::Color tail_col(static_cast<uint8_t>(sr >> 1), static_cast<uint8_t>(sg >> 1), static_cast<uint8_t>(sb >> 1));
      it.line(px, py, sx, sy, tail_col);
      it.draw_pixel_at(sx, sy, head_col);
      if (depth_u > 0.74f) {
        it.filled_circle(sx, sy, (depth_u > 0.88f) ? 2 : 1, head_col);
      }
    }
  }
}

static constexpr uint16_t MATRIX_GLYPH_MASK[32] = {
  0x121, 0x181, 0x069, 0x085, 0x183, 0x06B, 0x083, 0x129,
  0x0A9, 0x065, 0x0A3, 0x105, 0x214, 0x12B, 0x182, 0x007,
  0x301, 0x381, 0x108, 0x056, 0x141, 0x187, 0x223, 0x207,
  0x114, 0x300, 0x097, 0x067, 0x107, 0x028, 0x07D, 0x204
};

template<typename DisplayT>
inline void draw_matrix_katakana_glyph(DisplayT &it, int sx, int gy, uint16_t mask,
                                       esphome::Color col, bool slim_tail) {
  if (slim_tail) {
    if (mask & 0x080) it.line(sx, gy - 3, sx, gy + 3, col);
    else if (mask & 0x100) it.line(sx + 2, gy - 2, sx - 2, gy + 3, col);
    else it.line(sx - 2, gy - 3, sx + 2, gy - 3, col);
    if (mask & 0x002) it.line(sx - 2, gy, sx + 2, gy, col);
    return;
  }
  if (mask & 0x001) it.line(sx - 3, gy - 4, sx + 3, gy - 4, col);
  if (mask & 0x002) it.line(sx - 3, gy,     sx + 3, gy,     col);
  if (mask & 0x004) it.line(sx - 3, gy + 4, sx + 3, gy + 4, col);
  if (mask & 0x008) it.line(sx - 3, gy - 4, sx - 3, gy,     col);
  if (mask & 0x010) it.line(sx - 3, gy,     sx - 3, gy + 4, col);
  if (mask & 0x020) it.line(sx + 3, gy - 4, sx + 3, gy,     col);
  if (mask & 0x040) it.line(sx + 3, gy,     sx + 3, gy + 4, col);
  if (mask & 0x080) it.line(sx,     gy - 4, sx,     gy + 4, col);
  if (mask & 0x100) it.line(sx + 3, gy - 2, sx - 3, gy + 4, col);
  if (mask & 0x200) it.line(sx - 2, gy - 2, sx + 2, gy + 2, col);
}

template<typename DisplayT>
inline void render_screensaver_matrix(DisplayT &it, int cx, int cy, uint32_t now_ms) {
  (void)cx;
  static constexpr int NUM_COLS = 22;
  static constexpr int CELL_H = 14;

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

    int min_row = (y_min - drift_y) / CELL_H;
    int max_row = (y_max - drift_y) / CELL_H;
    int visible_rows = max_row - min_row + 1;
    if (visible_rows <= 2) continue;

    float col_h1 = hash2d_01(i * 37 + 11, i * 19 + 5);
    float col_h2 = hash2d_01(i * 53 + 23, i * 29 + 7);
    float rows_per_sec = 5.4f + col_h1 * 7.0f;
    float cycle_rows_f = static_cast<float>(visible_rows) + 30.0f + col_h2 * 18.0f;

    float init_phase_frac = fmodf(static_cast<float>(i) * 0.6180339887f + col_h1 * 0.37f, 1.0f);
    float total_travel = t_sec * rows_per_sec + init_phase_frac * cycle_rows_f;

    uint32_t pass_idx = static_cast<uint32_t>(total_travel / cycle_rows_f);
    float local_pos = total_travel - static_cast<float>(pass_idx) * cycle_rows_f;

    int pass_seed = static_cast<int>(pass_idx * 43U + static_cast<uint32_t>(i) * 109U);
    float r_len   = hash2d_01(pass_seed, 13);
    float r_tier  = hash2d_01(pass_seed, 29);
    float r_delay = hash2d_01(pass_seed, 47);

    int slen = 5 + static_cast<int>(r_len * 9.9f);
    float slen_f = static_cast<float>(slen);
    // Hoist reciprocal tail length outside the inner glyph loop (saves ~250 float divisions/frame!)
    float inv_tail_span = 1.0f / (slen_f > 2.4f ? (slen_f - 2.2f) : 1.0f);

    bool is_bg_stream = (r_tier < 0.30f);
    float bright_scale = is_bg_stream ? 0.54f : 1.0f;

    float start_row_f = static_cast<float>(min_row) - 1.0f - r_delay * 8.0f;
    float head_row_f = start_row_f + local_pos;
    int head_row_top = static_cast<int>(floorf(head_row_f));

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

      uint32_t cell_hash = static_cast<uint32_t>(i * 73856093) ^
                           static_cast<uint32_t>(row * 19349663) ^
                           (pass_idx * 83492791U);
      uint32_t glitch_tick = (dist < 1.1f)
          ? (now_ms / 85U)
          : (((cell_hash & 7U) == 0U) ? (now_ms / (190U + (cell_hash & 255U))) : 0U);
      uint16_t mask = MATRIX_GLYPH_MASK[(cell_hash + glitch_tick * 19U) & 31U];

      esphome::Color g_col;
      if (dist < 0.95f) {
        float lead_env = (dist < 0.30f) ? (0.45f + (dist * (1.0f / 0.30f)) * 0.55f) : 1.0f;
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
        float u = (dist - 0.95f) * (1.0f / 1.25f);
        float r_c = (115.0f * (1.0f - u) + 24.0f * u) * bright_scale;
        float g_c = (255.0f * (1.0f - u) + 225.0f * u) * bright_scale;
        float b_c = (140.0f * (1.0f - u) + 58.0f * u) * bright_scale;
        g_col = esphome::Color(static_cast<uint8_t>(r_c), static_cast<uint8_t>(g_c), static_cast<uint8_t>(b_c));
      } else {
        float tail_u = (dist - 2.2f) * inv_tail_span;
        if (tail_u > 1.0f) tail_u = 1.0f;
        float inv_u = 1.0f - tail_u;
        float fade = (16.0f + 198.0f * inv_u * (0.65f + 0.35f * inv_u)) * bright_scale;

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

// Precomputed 36-Point Golden-Spiral Unit Sphere LUT in .rodata Flash (432 B Flash, 0 B RAM):
// Eliminates 108 sqrtf() calls and 216 trig lookups per frame in render_screensaver_fireworks!
static constexpr Vec3 FIB_SPHERE_36[36] = {
  { 0.2333f,  0.9722f,  0.0000f}, {-0.3016f,  0.9167f,  0.2624f}, { 0.0445f,  0.8611f, -0.5065f},
  { 0.3471f,  0.8056f,  0.4800f}, {-0.6258f,  0.7500f, -0.2145f}, { 0.6775f,  0.6944f, -0.2423f},
  {-0.4359f,  0.6389f,  0.6337f}, {-0.0566f,  0.5833f, -0.8103f}, { 0.5624f,  0.5278f,  0.6365f},
  {-0.8588f,  0.4722f, -0.1987f}, { 0.8170f,  0.4167f, -0.3986f}, {-0.4052f,  0.3611f,  0.8399f},
  {-0.2286f,  0.3056f, -0.9243f}, { 0.7661f,  0.2500f,  0.5921f}, {-0.9628f,  0.1944f,  0.1885f},
  { 0.7254f,  0.1389f, -0.6742f}, {-0.1459f,  0.0833f,  0.9858f}, {-0.5313f,  0.0278f, -0.8467f},
  { 0.9469f, -0.0278f,  0.3204f}, {-0.8805f, -0.0833f,  0.4666f}, { 0.3827f, -0.1389f, -0.9134f},
  { 0.2896f, -0.1944f,  0.9372f}, {-0.8184f, -0.2500f, -0.5174f}, { 0.9438f, -0.3056f, -0.1264f},
  {-0.6337f, -0.3611f,  0.6842f}, { 0.0445f, -0.4167f, -0.9080f}, { 0.5798f, -0.4722f,  0.6640f},
  {-0.8501f, -0.5278f, -0.0085f}, { 0.6766f, -0.5833f, -0.4493f}, {-0.1882f, -0.6389f,  0.7459f},
  {-0.3360f, -0.6944f, -0.6363f}, { 0.6540f, -0.7500f,  0.0987f}, {-0.5480f, -0.8056f,  0.2255f},
  { 0.1476f, -0.8611f, -0.4865f}, { 0.2333f, -0.9167f,  0.3249f}, {-0.2333f, -0.9722f, -0.0145f}
};

template<typename DisplayT>
inline void render_screensaver_fireworks(DisplayT &it, int cx, int cy, uint32_t now_ms) {
  static constexpr uint32_t SHELL_CYCLE_MS = 3600U;
  static constexpr uint32_t SHELL_STAGGER_MS = 1200U;
  static constexpr int NUM_SPARKS = 36;

  for (int s = 0; s < 3; s++) {
    uint32_t t_offset = now_ms + static_cast<uint32_t>(s) * SHELL_STAGGER_MS;
    uint32_t cycle_idx = t_offset / SHELL_CYCLE_MS;
    uint32_t phase_ms = t_offset % SHELL_CYCLE_MS;
    float phase = static_cast<float>(phase_ms) * (1.0f / static_cast<float>(SHELL_CYCLE_MS));

    int seed_base = static_cast<int>(cycle_idx * 17U + static_cast<uint32_t>(s) * 53U);
    float rx = hash2d_01(seed_base, 11);
    float ry = hash2d_01(seed_base, 29);
    float rtype = hash2d_01(seed_base, 47);
    float rhue = hash2d_01(seed_base, 73);

    int bx = cx + static_cast<int>((rx - 0.5f) * 220.0f);
    int by = (cy - 32) + static_cast<int>((ry - 0.5f) * 155.0f);
    int dx_b = bx - cx, dy_b = by - cy;
    if (dx_b * dx_b + dy_b * dy_b > 14400) {
      bx = cx + (dx_b * 105) / 155;
      by = cy + (dy_b * 105) / 155;
    }

    int launch_x = cx + static_cast<int>((hash2d_01(seed_base, 91) - 0.5f) * 120.0f);
    int launch_h = circle_half_span(launch_x, 10);
    int launch_y = cy + (launch_h > 40 ? launch_h : 180);

    if (phase < 0.22f) {
      float u = phase * (1.0f / 0.22f);
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

    float bp = (phase - 0.22f) * (1.0f / 0.78f);
    float expand = 1.0f - (1.0f - bp) * (1.0f - bp) * (1.0f - bp);
    float bp_tail = (bp > 0.15f) ? (bp - 0.15f) : (bp * 0.35f);
    float expand_tail = 1.0f - (1.0f - bp_tail) * (1.0f - bp_tail) * (1.0f - bp_tail);

    float grav_head = 54.0f * bp * bp;
    float grav_tail = 54.0f * bp_tail * bp_tail;
    float fade = (1.0f - bp);
    float base_bright = (bp < 0.12f) ? 1.0f : (fade * (1.12f - 0.12f * bp));
    if (base_bright > 1.0f) base_bright = 1.0f;

    if (bp < 0.16f) {
      float fl = 1.0f - (bp * 6.25f);
      int flash_r = static_cast<int>(4.0f + bp * 90.0f);
      it.circle(bx, by, flash_r, esphome::Color(
          static_cast<uint8_t>(255.0f * fl),
          static_cast<uint8_t>(240.0f * fl),
          static_cast<uint8_t>(185.0f * fl)));
    }

    int shell_style = static_cast<int>(rtype * 4.0f) & 3;
    float max_radius = 98.0f + 32.0f * hash2d_01(seed_base, 61);

    // Precompute 3D Euler trig once per shell (saves 35 * 3 = 105 fast_sincos calls per shell!)
    float rot_x = hash2d_01(seed_base, 101) * PI_F + bp * 0.55f;
    float rot_y = hash2d_01(seed_base, 113) * PI_F - bp * 0.45f;
    float rot_z = hash2d_01(seed_base, 127) * PI_F + bp * 0.30f;
    float sx_r, cx_r, sy_r, cy_r, sz_r, cz_r;
    fast_sincos(rot_x, sx_r, cx_r);
    fast_sincos(rot_y, sy_r, cy_r);
    fast_sincos(rot_z, sz_r, cz_r);

    for (int k = 0; k < NUM_SPARKS; k++) {
      const Vec3 &u = FIB_SPHERE_36[k];
      float y1 = u.y * cx_r - u.z * sx_r;
      float z1 = u.y * sx_r + u.z * cx_r;
      float x2 = u.x * cy_r + z1 * sy_r;
      float z2 = -u.x * sy_r + z1 * cy_r;
      Vec3 dir_3d{x2 * cz_r - y1 * sz_r, x2 * sz_r + y1 * cz_r, z2};

      bool is_pistil = (shell_style == 1 && (k & 3) == 0);
      float jitter = is_pistil ? 0.52f : (0.88f + 0.24f * hash2d_01(seed_base + k, k * 7));
      float spark_rmax = max_radius * jitter;

      float r_h = spark_rmax * expand;
      float r_t = spark_rmax * expand_tail;

      Vec3 v_head{dir_3d.x * r_h, dir_3d.y * r_h + grav_head, dir_3d.z * r_h};
      Vec3 v_tail{dir_3d.x * r_t, dir_3d.y * r_t + grav_tail, dir_3d.z * r_t};

      ProjectedPoint p_head = project_3d(v_head, bx, by, 260.0f, 320.0f);
      ProjectedPoint p_tail = project_3d(v_tail, bx, by, 260.0f, 320.0f);
      if (!is_inside_bezel(p_head.x, p_head.y, cy, 5) || !is_inside_bezel(p_tail.x, p_tail.y, cy, 5)) continue;

      float depth_boost = fmaxf(0.55f, fminf(1.15f, p_head.scale * 1.12f));
      float bright = fminf(1.0f, base_bright * depth_boost);

      esphome::Color spark_col;
      if (shell_style == 0) {
        spark_col = esphome::Color(
            static_cast<uint8_t>(255.0f * bright),
            static_cast<uint8_t>((215.0f * fade + 35.0f) * bright),
            static_cast<uint8_t>(110.0f * fade * fade * bright));
      } else if (shell_style == 1) {
        if (is_pistil) {
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
        float h = rhue + static_cast<float>(k) * (1.0f / static_cast<float>(NUM_SPARKS));
        spark_col = hsv_to_rgb(h, 0.82f, bright);
      } else {
        if ((k % 3) == 1) {
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

      int mx = (p_head.x + p_tail.x) >> 1;
      int my = (p_head.y + p_tail.y) >> 1;
      esphome::Color tail_col(
          static_cast<uint8_t>((spark_col.r * 95U) >> 8),
          static_cast<uint8_t>((spark_col.g * 95U) >> 8),
          static_cast<uint8_t>((spark_col.b * 95U) >> 8));
      it.line(p_tail.x, p_tail.y, mx, my, tail_col);
      it.line(mx, my, p_head.x, p_head.y, spark_col);

      if (bp < 0.58f) {
        esphome::Color tip_col(
            static_cast<uint8_t>(spark_col.r + ((255 - spark_col.r) >> 1)),
            static_cast<uint8_t>(spark_col.g + ((255 - spark_col.g) >> 1)),
            static_cast<uint8_t>(spark_col.b + ((255 - spark_col.b) >> 1)));
        it.draw_pixel_at(p_head.x, p_head.y, tip_col);
        if (bp < 0.36f && dir_3d.z < -0.15f) {
          it.filled_circle(p_head.x, p_head.y, 1, tip_col);
        }
      } else if (bp > 0.62f && ((k + (phase_ms / 90U)) % 4 == 0)) {
        uint8_t gl = static_cast<uint8_t>(255.0f * fade);
        it.draw_pixel_at(p_head.x, p_head.y, esphome::Color(gl, gl, static_cast<uint8_t>(gl * 0.8f)));
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
  (void)col_dark_gray;
  if (ss_style == "Ambient Clock") {
    int min_val = now.is_valid() ? now.minute : 0;
    int drift_x = (min_val % 10) * 4 - 18;
    int drift_y = ((min_val / 10) % 6) * 4 - 10;

    char amb_time[16];
    if (now.is_valid()) {
      format_time_hhmm(now.hour, now.minute, is_24h, amb_time, sizeof(amb_time));
      it.print(cx + drift_x, cy - 20 + drift_y, font_time, col_slate, esphome::display::TextAlign::CENTER, amb_time);
      it.strftime(cx + drift_x, cy + 45 + drift_y, font_small, esphome::Color(90, 102, 118),
                  esphome::display::TextAlign::CENTER, "%a, %b %d", now);
    } else {
      it.print(cx + drift_x, cy - 20 + drift_y, font_time, col_slate, esphome::display::TextAlign::CENTER, "--:--");
    }
    return;
  }

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

// Complete Voice Assistant Visual Engine supporting 3 pure-visual profiles (zero text):
//   - "Nebula": Flagship 3D Multi-Ribbon Silk Wave Orb (Siri / Gemini style, LUT-driven, default)
//   - "Solaris": Flagship Radial Acoustic Corona & Solar Astrolabe Spectrometer
//   - "Original": Original Nebula visual design with trig & loop optimizations
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

  (void)va_status_text;
  (void)font_small;
  const bool is_original = (profile_opt == "Original" || profile_opt == "Classic Nebula");
  const bool is_solaris = (profile_opt == "Solaris");

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
  // A. "ORIGINAL" (Original Nebula Visual Design, C++ Trig Optimized, Pure Visual)
  // ============================================================================
  if (is_original) {
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
    return;
  }

  // ============================================================================
  // B. FLAGSHIP "NEBULA" — TRUE 3D GYROSCOPIC SILK WAVE ORB (rotate_3d_xyz + project_3d)
  // ============================================================================
  if (!is_solaris) {
    const esphome::Color c_elec_cyan(0, 229, 255);
    const esphome::Color c_royal_indigo(115, 65, 255);
    const esphome::Color c_neon_magenta(255, 45, 145);
    const esphome::Color c_deep_azure(20, 95, 195);
    const esphome::Color c_ice_white(215, 248, 255);

    float t_sec = static_cast<float>(now_ms % 60000U) * 0.001f;

    // Draws a 30-segment closed 3D harmonic silk ribbon tilted in 3D space (rx, ry, rz)
    // with depth-sorted luminance (brighter when arcing in front of the core, dimmer behind)
    auto draw_silk_ribbon_3d = [&](float base_r, float a2, float a3, float a5,
                                   int ph2, int ph3, int ph5,
                                   float rx, float ry, float rz,
                                   esphome::Color col_main, esphome::Color col_glow,
                                   bool double_stroke) {
      int prev_x = 0, prev_y = 0, first_x = 0, first_y = 0;
      int prev_gx = 0, prev_gy = 0, first_gx = 0, first_gy = 0;
      float prev_z = 0.0f;
      for (int i = 0; i < 60; i += 2) {
        float w2 = DIAL_SIN[(i * 2 + ph2) % 60];
        float w3 = DIAL_COS[(i * 3 + ph3) % 60];
        float w5 = DIAL_SIN[(i * 5 + ph5) % 60];
        float r = base_r + a2 * w2 + a3 * w3;
        float z_local = a5 * w5 * 3.2f; // Out-of-plane 3D silk undulation

        float ux = DIAL_SIN[i];
        float uy = -DIAL_COS[i];
        Vec3 v3 = rotate_3d_xyz({r * ux, r * uy, z_local}, rx, ry, rz);
        ProjectedPoint p = project_3d(v3, cx, cy, 260.0f, 320.0f);

        Vec3 vg3 = rotate_3d_xyz({(r + 2.5f) * ux, (r + 2.5f) * uy, z_local}, rx, ry, rz);
        ProjectedPoint pg = project_3d(vg3, cx, cy, 260.0f, 320.0f);

        if (i == 0) {
          first_x = p.x; first_y = p.y;
          first_gx = pg.x; first_gy = pg.y;
        } else {
          // Depth shading: v3.z < 0 is closer to viewer (bright), v3.z > 0 is behind core (dimmer)
          float z_avg = 0.5f * (v3.z + prev_z);
          float depth_k = fmaxf(0.45f, fminf(1.12f, 0.84f - (z_avg / (base_r + 10.0f)) * 0.36f));
          esphome::Color seg_c = scale_color(col_main, depth_k);
          if (double_stroke && z_avg < 12.0f) {
            it.line(prev_gx, prev_gy, pg.x, pg.y, scale_color(col_glow, depth_k));
          }
          it.line(prev_x, prev_y, p.x, p.y, seg_c);
        }
        prev_x = p.x; prev_y = p.y;
        prev_gx = pg.x; prev_gy = pg.y;
        prev_z = v3.z;
      }
      if (double_stroke) {
        it.line(prev_gx, prev_gy, first_gx, first_gy, col_glow);
      }
      it.line(prev_x, prev_y, first_x, first_y, col_main);
    };

    if (is_speaking) {
      float syl_env = (0.55f + 0.25f * sinf(t_sec * 11.5f) + 0.20f * cosf(t_sec * 19.3f)) * smooth_speak;
      if (syl_env < 0.15f) syl_env = 0.15f;

      int tick = static_cast<int>(now_ms / 35U);
      uint32_t speak_elapsed = now_ms - speak_start_ms;

      // 3 Expanding Acoustic Wave Rings
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

      // 4 Precessing 3D Gyroscopic Silk Voice Ribbons
      float base_r = 52.0f + 16.0f * syl_env;
      float a2 = 6.0f + 11.0f * syl_env;
      float a3 = 4.5f + 8.0f * syl_env;
      float a5 = 2.5f + 5.0f * syl_env;

      draw_silk_ribbon_3d(base_r + 4.0f, a2, a3, a5, (tick) % 60, (tick * 2 + 15) % 60, (tick + 30) % 60,
                          0.52f * fast_sin(t_sec * 0.9f), 0.48f * fast_cos(t_sec * 0.7f), t_sec * 0.4f,
                          c_royal_indigo, scale_color(c_royal_indigo, 0.4f), false);
      draw_silk_ribbon_3d(base_r, -a2 * 0.9f, a3 * 1.1f, -a5, (tick + 20) % 60, (tick + 40) % 60, (tick * 2) % 60,
                          -0.55f * fast_cos(t_sec * 0.8f), 0.50f * fast_sin(t_sec * 1.1f), -t_sec * 0.35f,
                          c_neon_magenta, scale_color(c_neon_magenta, 0.4f), false);
      draw_silk_ribbon_3d(base_r - 2.0f, a2 * 0.8f, -a3, a5 * 1.2f, (tick * 2 + 7) % 60, (tick + 25) % 60, (tick + 12) % 60,
                          0.42f * fast_cos(t_sec * 1.2f), -0.45f * fast_sin(t_sec * 0.85f), t_sec * 0.5f,
                          c_elec_cyan, c_deep_azure, true);
      draw_silk_ribbon_3d(base_r - 5.0f, a2 * 0.5f, a3 * 0.5f, a5 * 0.6f, (tick + 35) % 60, (tick + 10) % 60, (tick) % 60,
                          0.28f * fast_sin(t_sec * 1.4f), 0.28f * fast_cos(t_sec * 1.4f), -t_sec * 0.25f,
                          c_ice_white, c_elec_cyan, false);

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

      float vortex_r = 74.0f - 20.0f * smooth_think;
      draw_silk_ribbon_3d(vortex_r + 3.0f * pulse, 6.0f, 5.0f, 3.5f,
                          (tick * 2) % 60, (tick * 3) % 60, (tick) % 60,
                          0.68f * fast_sin(t_sec * 1.3f), 0.55f * fast_cos(t_sec * 1.1f), t_sec * 0.8f,
                          c_royal_indigo, scale_color(c_royal_indigo, 0.4f), false);
      draw_silk_ribbon_3d(vortex_r - 2.0f - 2.0f * pulse, -5.0f, 6.0f, -3.0f,
                          (tick * 2 + 30) % 60, (tick + 15) % 60, (tick * 2) % 60,
                          -0.62f * fast_cos(t_sec * 1.2f), 0.65f * fast_sin(t_sec * 1.4f), -t_sec * 0.7f,
                          c_neon_magenta, scale_color(c_neon_magenta, 0.4f), false);
      draw_silk_ribbon_3d(vortex_r - 4.0f, 4.0f, -4.0f, 2.5f,
                          (tick + 45) % 60, (tick * 2 + 20) % 60, (tick * 3) % 60,
                          0.48f * fast_cos(t_sec * 1.6f), -0.52f * fast_sin(t_sec * 1.2f), t_sec * 0.9f,
                          c_elec_cyan, c_deep_azure, true);

      // 6 3D Tilted Orbital Neural Nodes with 5-Step Fading Comet Trails
      const int head_idx_cw = static_cast<int>((now_ms / 18U) % 60U);
      const int head_idx_ccw = (60 - static_cast<int>((now_ms / 22U) % 60U)) % 60;
      float orb_rx1 = 0.52f * fast_sin(t_sec * 0.9f), orb_ry1 = 0.45f * fast_cos(t_sec * 0.8f);
      float orb_rx2 = -0.48f * fast_cos(t_sec * 0.85f), orb_ry2 = 0.52f * fast_sin(t_sec * 0.95f);

      for (int n = 0; n < 3; n++) {
        int base_cw = (head_idx_cw + n * 20) % 60;
        for (int tail = 4; tail >= 0; tail--) {
          int idx = (base_cw - tail * 2 + 60) % 60;
          Vec3 v_cw = rotate_3d_xyz({88.0f * DIAL_SIN[idx], -88.0f * DIAL_COS[idx], 0.0f}, orb_rx1, orb_ry1, 0.0f);
          ProjectedPoint p_cw = project_3d(v_cw, cx, cy, 260.0f, 320.0f);
          float k = static_cast<float>(5 - tail) * 0.20f * smooth_think;
          int rad = (tail == 0) ? (v_cw.z < 0.0f ? 4 : 3) : (tail <= 2 ? 2 : 1);
          it.filled_circle(p_cw.x, p_cw.y, rad,
                           (tail == 0) ? scale_color(c_ice_white, smooth_think) : scale_color(c_elec_cyan, k));
        }

        int base_ccw = (head_idx_ccw + n * 20 + 10) % 60;
        for (int tail = 4; tail >= 0; tail--) {
          int idx = (base_ccw + tail * 2) % 60;
          Vec3 v_ccw = rotate_3d_xyz({118.0f * DIAL_SIN[idx], -118.0f * DIAL_COS[idx], 0.0f}, orb_rx2, orb_ry2, 0.0f);
          ProjectedPoint p_ccw = project_3d(v_ccw, cx, cy, 260.0f, 320.0f);
          float k = static_cast<float>(5 - tail) * 0.20f * smooth_think;
          int rad = (tail == 0) ? (v_ccw.z < 0.0f ? 4 : 3) : (tail <= 2 ? 2 : 1);
          it.filled_circle(p_ccw.x, p_ccw.y, rad,
                           (tail == 0) ? scale_color(c_ice_white, smooth_think) : scale_color(c_neon_magenta, k));
        }
      }

      int core_r = 22 + static_cast<int>(4.0f * pulse);
      it.filled_circle(cx, cy, core_r + 4, scale_color(c_neon_magenta, 0.35f));
      it.filled_circle(cx, cy, core_r, c_royal_indigo);
      it.circle(cx, cy, core_r + 1, c_elec_cyan);
      it.filled_circle(cx, cy, (core_r * 2) / 3, c_deep_azure);
      it.filled_circle(cx, cy, core_r / 2, c_ice_white);
      return;
    }

    // Nebula Listening: 4 Precessing 3D Gyroscopic Silk Ribbons + VAD Bloom
    float breath = sinf(static_cast<float>(now_ms % 2000U) * (2.0f * PI_F / 2000.0f));
    int tick = static_cast<int>(now_ms / 45U);

    float base_r = 80.0f + 6.0f * breath + 22.0f * smooth_vad;
    float a2 = 5.0f + 10.0f * smooth_vad;
    float a3 = 4.0f + 8.0f * smooth_vad;
    float a5 = 2.5f + 5.5f * smooth_vad;

    draw_silk_ribbon_3d(base_r + 5.0f, a2, a3, a5, (tick) % 60, (tick + 20) % 60, (tick + 40) % 60,
                        0.48f * fast_sin(t_sec * 0.65f), 0.42f * fast_cos(t_sec * 0.55f), t_sec * 0.28f,
                        c_royal_indigo, scale_color(c_royal_indigo, 0.35f), false);
    draw_silk_ribbon_3d(base_r, -a2, a3 * 0.9f, -a5, (tick + 15) % 60, (tick * 2) % 60, (tick + 10) % 60,
                        -0.50f * fast_cos(t_sec * 0.60f), 0.46f * fast_sin(t_sec * 0.75f), -t_sec * 0.24f,
                        c_neon_magenta, scale_color(c_neon_magenta, 0.35f), false);
    draw_silk_ribbon_3d(base_r - 4.0f, a2 * 0.8f, -a3, a5, (tick + 35) % 60, (tick + 12) % 60, (tick * 2) % 60,
                        0.38f * fast_cos(t_sec * 0.80f), -0.42f * fast_sin(t_sec * 0.62f), t_sec * 0.32f,
                        c_elec_cyan, c_deep_azure, true);
    draw_silk_ribbon_3d(base_r - 8.0f, a2 * 0.4f, a3 * 0.4f, a5 * 0.5f, (tick + 50) % 60, (tick + 30) % 60, (tick) % 60,
                        0.22f * fast_sin(t_sec * 0.95f), 0.22f * fast_cos(t_sec * 0.95f), -t_sec * 0.18f,
                        c_ice_white, c_elec_cyan, false);

    int core_r = 23 + static_cast<int>(4.0f * breath + 7.0f * smooth_vad);
    it.filled_circle(cx, cy, core_r + 4, scale_color(c_neon_magenta, 0.30f + 0.25f * smooth_vad));
    it.filled_circle(cx, cy, core_r, c_royal_indigo);
    it.circle(cx, cy, core_r + 1, c_elec_cyan);
    it.filled_circle(cx, cy, (core_r * 2) / 3, lerp_color(c_deep_azure, c_elec_cyan, smooth_vad * 0.5f));
    it.filled_circle(cx, cy, core_r / 2, c_ice_white);
    return;
  }

  // ============================================================================
  // C. FLAGSHIP "SOLARIS" — RADIAL ACOUSTIC CORONA & 3D GIMBALED SOLAR ASTROLABE
  // ============================================================================
  const esphome::Color c_gold = col_solaris_gold;
  const esphome::Color c_amber(255, 105, 15);
  const esphome::Color c_crimson_ember(185, 35, 10);
  const esphome::Color c_emerald(35, 215, 115);
  const esphome::Color c_warm_white(255, 248, 215);

  float t_sec = static_cast<float>(now_ms % 60000U) * 0.001f;

  if (is_speaking) {
    float syl_env = (0.55f + 0.25f * sinf(t_sec * 11.5f) + 0.20f * cosf(t_sec * 19.3f)) * smooth_speak;
    if (syl_env < 0.15f) syl_env = 0.15f;
    float breath = sinf(static_cast<float>(now_ms % 1800U) * (2.0f * PI_F / 1800.0f));
    int tick = static_cast<int>(now_ms / 35U);
    uint32_t speak_elapsed = now_ms - speak_start_ms;

    // 3 Expanding Coronal Shockwave Rings radiating outward from the solar corona
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

    // Center Solar Corona & Photosphere Core matching Listening mode aesthetics
    const float r_inner = 42.0f + 3.0f * breath;
    for (int i = 0; i < 60; i += 2) {
      float wave = DIAL_SIN[(i * 3 + tick) % 60] * 0.6f + DIAL_COS[(i * 2 + tick) % 60] * 0.4f;
      float bar_len = (16.0f + 22.0f * syl_env) + (6.0f + 10.0f * syl_env) * wave;
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
      it.line(xm, ym, x1, y1, (i % 4 == 0) ? c_emerald : c_gold);
    }

    int core_r = 27 + static_cast<int>(4.0f * breath + 5.0f * syl_env);
    it.filled_circle(cx, cy, core_r + 5, c_crimson_ember);
    it.filled_circle(cx, cy, core_r + 1, c_amber);
    it.filled_circle(cx, cy, core_r - 3, c_gold);
    it.circle(cx, cy, core_r - 5, c_warm_white);
    return;
  }
  if (is_thinking) {
    // 3D Gimbaled Armillary Sphere / Solar Astrolabe (rotate_3d_xyz + project_3d)
    const int head_1 = static_cast<int>((now_ms / 18U) % 60U);
    const int head_2 = (60 - static_cast<int>((now_ms / 22U) % 60U)) % 60;
    const int head_3 = static_cast<int>((now_ms / 28U) % 60U);

    float rx1 = 0.62f * fast_sin(t_sec * 1.1f),  ry1 = 0.52f * fast_cos(t_sec * 0.9f);
    float rx2 = -0.58f * fast_cos(t_sec * 0.95f), ry2 = 0.60f * fast_sin(t_sec * 1.15f);
    float rx3 = 0.42f * fast_cos(t_sec * 0.75f),  ry3 = -0.48f * fast_sin(t_sec * 0.85f);

    auto draw_3d_meridian = [&](float rad, float rx, float ry, esphome::Color col) {
      int px = 0, py = 0, fx = 0, fy = 0;
      for (int i = 0; i < 60; i += 3) {
        Vec3 v = rotate_3d_xyz({rad * DIAL_SIN[i], -rad * DIAL_COS[i], 0.0f}, rx, ry, 0.0f);
        ProjectedPoint p = project_3d(v, cx, cy, 260.0f, 320.0f);
        if (i == 0) { fx = p.x; fy = p.y; }
        else { it.line(px, py, p.x, p.y, col); }
        px = p.x; py = p.y;
      }
      it.line(px, py, fx, fy, col);
    };

    draw_3d_meridian(66.0f,  rx1, ry1, scale_color(c_amber,   0.34f * smooth_think));
    draw_3d_meridian(94.0f,  rx2, ry2, scale_color(c_gold,    0.30f * smooth_think));
    draw_3d_meridian(122.0f, rx3, ry3, scale_color(c_emerald, 0.30f * smooth_think));

    for (int n = 0; n < 3; n++) {
      int b1 = (head_1 + n * 20) % 60;
      for (int tail = 5; tail >= 0; tail--) {
        int idx = (b1 - tail * 2 + 60) % 60;
        Vec3 v = rotate_3d_xyz({66.0f * DIAL_SIN[idx], -66.0f * DIAL_COS[idx], 0.0f}, rx1, ry1, 0.0f);
        ProjectedPoint p = project_3d(v, cx, cy, 260.0f, 320.0f);
        it.filled_circle(p.x, p.y, (tail == 0) ? 4 : (tail <= 2 ? 2 : 1),
                         (tail == 0) ? scale_color(c_warm_white, smooth_think) : scale_color(c_gold, (6 - tail) * 0.16f * smooth_think));
      }
      int b2 = (head_2 + n * 20 + 10) % 60;
      for (int tail = 5; tail >= 0; tail--) {
        int idx = (b2 + tail * 2) % 60;
        Vec3 v = rotate_3d_xyz({94.0f * DIAL_SIN[idx], -94.0f * DIAL_COS[idx], 0.0f}, rx2, ry2, 0.0f);
        ProjectedPoint p = project_3d(v, cx, cy, 260.0f, 320.0f);
        it.filled_circle(p.x, p.y, (tail == 0) ? 4 : (tail <= 2 ? 2 : 1),
                         (tail == 0) ? scale_color(c_warm_white, smooth_think) : scale_color(c_emerald, (6 - tail) * 0.16f * smooth_think));
      }
      int b3 = (head_3 + n * 20 + 5) % 60;
      for (int tail = 4; tail >= 0; tail--) {
        int idx = (b3 - tail * 2 + 60) % 60;
        Vec3 v = rotate_3d_xyz({122.0f * DIAL_SIN[idx], -122.0f * DIAL_COS[idx], 0.0f}, rx3, ry3, 0.0f);
        ProjectedPoint p = project_3d(v, cx, cy, 260.0f, 320.0f);
        it.filled_circle(p.x, p.y, (tail == 0) ? 3 : 2,
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

  // Solaris Listening: Breathing 30-Ray Solar Corona + Photosphere Core
  float breath = sinf(static_cast<float>(now_ms % 1800U) * (2.0f * PI_F / 1800.0f));
  int tick = static_cast<int>(now_ms / 45U);

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
}
}  // namespace pocket_ui