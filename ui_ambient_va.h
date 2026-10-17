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

    // New Nebula Listening: Fluid Breathing Silk Aura + Smooth VAD Bloom (no outer containment circle)
    float breath = sinf(static_cast<float>(now_ms % 2000U) * (2.0f * PI_F / 2000.0f));
    int tick = static_cast<int>(now_ms / 45U);

    float base_r = 76.0f + 6.0f * breath + 22.0f * smooth_vad;
    float a2 = 5.0f + 10.0f * smooth_vad;
    float a3 = 4.0f + 8.0f * smooth_vad;
    float a5 = 2.0f + 5.0f * smooth_vad;

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

  // New Solaris Listening: Breathing 30-Ray Solar Corona + Photosphere Core (no outer green ring, no text)
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

// ------------------------------------------------------------------------------

}  // namespace pocket_ui