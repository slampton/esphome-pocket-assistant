#pragma once

// ==============================================================================
// Pocket Assistant 8-Tool Suite, Clock, Music, TV Remote & System Hub (ui_tools_apps.h)
// ==============================================================================

#include "ui_core.h"
#include "ui_ambient_va.h"

namespace pocket_ui {

// ============================================================================
// SECTION 13: TOOLS SUITE PART 1 — LAUNCHER, STOPWATCH, DICE, LEVEL & COIN FLIP
// ============================================================================

template<typename DisplayT, typename FontSmallT>
inline void render_tool_launcher_card(DisplayT &it, int cx_card, int cy_card,
                                      int app_id, const char *label,
                                      FontSmallT *font_small,
                                      esphome::Color theme_secondary,
                                      esphome::Color col_white) {
  esphome::Color card_bg(22, 27, 36);
  esphome::Color card_border(48, 56, 70);
  render_modern_card(it, cx_card, cy_card, 124, 100, 16, card_bg, card_border, nullptr, col_white, font_small);

  int ix = cx_card;
  int iy = cy_card - 15;

  if (app_id == 1) {
    // 1. Timers: 2px-stroke hourglass with solid filled upper/lower sand bulbs (SF Symbol hourglass)
    it.filled_rectangle(ix - 11, iy - 14, 23, 2, col_white);
    it.filled_rectangle(ix - 11, iy + 13, 23, 2, col_white);
    // 2px tapered glass walls
    it.line(ix - 9, iy - 12, ix - 2, iy, col_white);
    it.line(ix - 8, iy - 12, ix - 1, iy, col_white);
    it.line(ix + 9, iy - 12, ix + 2, iy, col_white);
    it.line(ix + 8, iy - 12, ix + 1, iy, col_white);
    it.line(ix - 2, iy, ix - 9, iy + 12, col_white);
    it.line(ix - 1, iy, ix - 8, iy + 12, col_white);
    it.line(ix + 2, iy, ix + 9, iy + 12, col_white);
    it.line(ix + 1, iy, ix + 8, iy + 12, col_white);
    // Solid filled upper sand cone, falling stream & solid lower sand mound
    it.filled_triangle(ix - 5, iy - 7, ix + 5, iy - 7, ix, iy - 2, col_white);
    it.line(ix, iy - 2, ix, iy + 4, theme_secondary);
    it.filled_triangle(ix - 6, iy + 11, ix + 6, iy + 11, ix, iy + 4, col_white);

  } else if (app_id == 2) {
    // 2. Alarms: Twin solid filled bells, 2px clock bezel, angled legs & 2px hands (alarm.fill style)
    // Twin filled bells at upper-left & upper-right (carved cleanly by outer bezel gap)
    it.filled_circle(ix - 10, iy - 9, 4, col_white);
    it.filled_circle(ix + 10, iy - 9, 4, col_white);
    it.filled_circle(ix, iy + 2, 14, card_bg);
    // 2px clock case ring
    it.circle(ix, iy + 2, 12, col_white);
    it.circle(ix, iy + 2, 11, col_white);
    // 2px bottom legs
    it.line(ix - 8, iy + 12, ix - 11, iy + 16, col_white);
    it.line(ix - 7, iy + 12, ix - 10, iy + 16, col_white);
    it.line(ix + 8, iy + 12, ix + 11, iy + 16, col_white);
    it.line(ix + 7, iy + 12, ix + 10, iy + 16, col_white);
    // 2px hands + center cap
    it.line(ix,     iy + 2, ix,     iy - 5, col_white);
    it.line(ix + 1, iy + 2, ix + 1, iy - 5, col_white);
    it.line(ix,     iy + 2, ix + 5, iy + 5, col_white);
    it.line(ix,     iy + 3, ix + 5, iy + 6, col_white);
    it.filled_circle(ix, iy + 2, 2, col_white);

  } else if (app_id == 3) {
    // 3. Stopwatch: 2px chronometer bezel, bold top crown, angled pusher & 2px sweep hand
    // Top T-crown & angled 2 o'clock pusher
    it.filled_rectangle(ix - 4, iy - 16, 9, 2, col_white);
    it.filled_rectangle(ix - 1, iy - 14, 3, 4, col_white);
    it.line(ix + 9,  iy - 8, ix + 13, iy - 12, col_white);
    it.line(ix + 10, iy - 7, ix + 14, iy - 11, col_white);
    // 2px dial ring
    it.circle(ix, iy + 2, 12, col_white);
    it.circle(ix, iy + 2, 11, col_white);
    // 4 inner cardinal ticks
    it.line(ix, iy - 8, ix, iy - 6, theme_secondary);
    it.line(ix, iy + 10, ix, iy + 12, theme_secondary);
    it.line(ix - 10, iy + 2, ix - 8, iy + 2, theme_secondary);
    it.line(ix + 8, iy + 2, ix + 10, iy + 2, theme_secondary);
    // Center arbor & 2px diagonal chrono hand
    it.filled_circle(ix, iy + 2, 2, col_white);
    it.line(ix,     iy + 2, ix + 6, iy - 4, col_white);
    it.line(ix + 1, iy + 2, ix + 6, iy - 3, col_white);

  } else if (app_id == 4) {
    // 4. Dice Roll: Solid filled rounded cube with 5 crisp negative-space pips (die.face.5.fill)
    int d_r = 5;
    int x0 = ix - 13, y0 = iy - 13, s = 27;
    it.filled_rectangle(x0 + d_r, y0, s - 2 * d_r, s, col_white);
    it.filled_rectangle(x0, y0 + d_r, s, s - 2 * d_r, col_white);
    it.filled_circle(x0 + d_r, y0 + d_r, d_r, col_white);
    it.filled_circle(x0 + s - 1 - d_r, y0 + d_r, d_r, col_white);
    it.filled_circle(x0 + d_r, y0 + s - 1 - d_r, d_r, col_white);
    it.filled_circle(x0 + s - 1 - d_r, y0 + s - 1 - d_r, d_r, col_white);
    // 5 dark negative-space pips
    it.filled_circle(ix,     iy,     2, card_bg);
    it.filled_circle(ix - 7, iy - 7, 2, card_bg);
    it.filled_circle(ix + 7, iy - 7, 2, card_bg);
    it.filled_circle(ix - 7, iy + 7, 2, card_bg);
    it.filled_circle(ix + 7, iy + 7, 2, card_bg);

  } else if (app_id == 5) {
    // 5. Flashlight: Solid filled sculpted torch body & reflector head + 3 bold 2px light beams
    // Flared reflector head (solid filled trapezoid)
    it.filled_triangle(ix - 9, iy - 7, ix + 9, iy - 7, ix - 5, iy - 1, col_white);
    it.filled_triangle(ix + 9, iy - 7, ix - 5, iy - 1, ix + 5, iy - 1, col_white);
    // Solid filled handle barrel (separated by 2px card_bg collar groove at iy)
    it.filled_rectangle(ix - 5, iy + 1, 11, 14, col_white);
    it.filled_circle(ix, iy + 14, 5, col_white);
    // Negative-space power button slot on handle
    it.filled_rectangle(ix - 1, iy + 5, 3, 5, card_bg);
    // 3 bold 2px-thick radiating light beams
    it.line(ix,     iy - 10, ix,     iy - 16, col_white);
    it.line(ix + 1, iy - 10, ix + 1, iy - 16, col_white);
    it.line(ix - 6, iy - 10, ix - 9, iy - 15, col_white);
    it.line(ix - 5, iy - 10, ix - 8, iy - 15, col_white);
    it.line(ix + 6, iy - 10, ix + 9, iy - 15, col_white);
    it.line(ix + 5, iy - 10, ix + 8, iy - 15, col_white);

  } else if (app_id == 6) {
    // 6. Calculator: 2px rounded frame, solid filled display bar & 3x2 grid of solid keypad pills
    it.rectangle(ix - 12, iy - 15, 25, 31, col_white);
    it.rectangle(ix - 11, iy - 14, 23, 29, col_white);
    // Solid filled LCD display bar at top
    it.filled_rectangle(ix - 8, iy - 11, 17, 6, col_white);
    // 3 columns x 2 rows of solid filled keypad buttons
    for (int r = 0; r < 2; r++) {
      int ky = iy - 1 + r * 6;
      it.filled_rectangle(ix - 8, ky, 4, 3, col_white);
      it.filled_rectangle(ix - 2, ky, 4, 3, col_white);
      it.filled_rectangle(ix + 4, ky, 4, 3, col_white);
    }
    // Bottom row: standard key + wide [=] bar
    it.filled_rectangle(ix - 8, iy + 10, 4, 2, col_white);
    it.filled_rectangle(ix - 2, iy + 10, 10, 2, col_white);

  } else if (app_id == 7) {
    // 7. Bubble Level: 2px outer ring, 2px cardinal crosshairs, inner ring & solid center bubble
    it.circle(ix, iy, 14, col_white);
    it.circle(ix, iy, 13, col_white);
    it.circle(ix, iy, 7, theme_secondary);
    it.filled_circle(ix, iy, 4, col_white);
    // 4 bold 2px outer cardinal crosshair ticks
    it.line(ix,     iy - 17, ix,     iy - 13, col_white);
    it.line(ix + 1, iy - 17, ix + 1, iy - 13, col_white);
    it.line(ix,     iy + 13, ix,     iy + 17, col_white);
    it.line(ix + 1, iy + 13, ix + 1, iy + 17, col_white);
    it.line(ix - 17, iy,     ix - 13, iy,     col_white);
    it.line(ix - 17, iy + 1, ix - 13, iy + 1, col_white);
    it.line(ix + 13, iy,     ix + 17, iy,     col_white);
    it.line(ix + 13, iy + 1, ix + 17, iy + 1, col_white);

  } else if (app_id == 8) {
    // 8. Coin Flip: 2px outer medallion rim, inner struck ring & solid filled Home Assistant house
    it.circle(ix, iy, 15, col_white);
    it.circle(ix, iy, 14, col_white);
    it.circle(ix, iy, 11, theme_secondary);
    // Solid filled house emblem with negative-space center node/stem
    it.filled_triangle(ix - 7, iy, ix + 7, iy, ix, iy - 7, col_white);
    it.filled_rectangle(ix - 5, iy, 11, 7, col_white);
    it.filled_circle(ix, iy + 1, 2, card_bg);
    it.line(ix, iy + 3, ix, iy + 6, card_bg);
  }

  it.print(cx_card, cy_card + 26, font_small, col_white,
           esphome::display::TextAlign::CENTER, label);
}

template<typename DisplayT, typename FontBigT, typename FontSmallT>
inline void render_tools_launcher(DisplayT &it, int cx, int stuff_menu_page,
                                  bool menu_scroll_active, int menu_scroll_step,
                                  int menu_scroll_dir, int menu_scroll_target_page,
                                  FontBigT *font_date, FontSmallT *font_small,
                                  esphome::Color theme_primary, esphome::Color theme_secondary,
                                  esphome::Color col_bg, esphome::Color col_black,
                                  esphome::Color col_white) {
  const int col_left_x = 162;
  const int col_right_x = 298;
  const int row_0_y = 159;
  const int row_1_y = 283;

  auto draw_page_cards = [&](int page_idx, int y_shift) {
    int y0 = row_0_y + y_shift;
    int y1 = row_1_y + y_shift;
    if (page_idx == 0) {
      if (y0 >= 95 && y0 <= 355) {
        render_tool_launcher_card(it, col_left_x,  y0, 1, "Timers",     font_small, theme_secondary, col_white);
        render_tool_launcher_card(it, col_right_x, y0, 2, "Alarms",     font_small, theme_secondary, col_white);
      }
      if (y1 >= 95 && y1 <= 355) {
        render_tool_launcher_card(it, col_left_x,  y1, 5, "Flashlight", font_small, theme_secondary, col_white);
        render_tool_launcher_card(it, col_right_x, y1, 6, "Calculator", font_small, theme_secondary, col_white);
      }
    } else {
      if (y0 >= 95 && y0 <= 355) {
        render_tool_launcher_card(it, col_left_x,  y0, 3, "Stopwatch",  font_small, theme_secondary, col_white);
        render_tool_launcher_card(it, col_right_x, y0, 4, "Dice Roll",  font_small, theme_secondary, col_white);
      }
      if (y1 >= 95 && y1 <= 355) {
        render_tool_launcher_card(it, col_left_x,  y1, 7, "Level",      font_small, theme_secondary, col_white);
        render_tool_launcher_card(it, col_right_x, y1, 8, "Coin Flip",  font_small, theme_secondary, col_white);
      }
    }
  };

  if (menu_scroll_active) {
    int step = std::min(4, menu_scroll_step);
    int offset = SHARED_SCROLL_EASE_232[step];
    draw_page_cards(stuff_menu_page, -menu_scroll_dir * offset);
    draw_page_cards(menu_scroll_target_page, menu_scroll_dir * (232 - offset));
  } else {
    draw_page_cards(stuff_menu_page, 0);
  }

  // Header & Right-Flank Scroll Chevrons
  it.print(cx, 50, font_date, theme_primary, esphome::display::TextAlign::CENTER, "TOOLS");
  it.filled_rectangle(395, 140, 71, 160, col_bg);
  if (stuff_menu_page == 1) {
    render_scroll_chevron(it, 415, 162, true, theme_secondary, col_black, false);
  }
  if (stuff_menu_page == 0) {
    render_scroll_chevron(it, 415, 278, false, theme_secondary, col_black, false);
  }
}

template<typename DisplayT, typename FontBigT, typename FontSmallT>
inline void render_tool_stopwatch(DisplayT &it, int cx, int cy, uint32_t now_ms,
                                  bool stopwatch_running, uint32_t stopwatch_start_time,
                                  uint32_t stopwatch_elapsed_ms, uint32_t stopwatch_lap_ms,
                                  FontBigT *font_date, FontSmallT *font_small,
                                  esphome::Color col_white, esphome::Color col_silver,
                                  esphome::Color col_slate, esphome::Color col_dark_gray,
                                  esphome::Color col_red) {
  // Dual Perimeter Rings & Inner Rehaut Flange
  it.circle(cx, cy, 230, col_white);
  it.circle(cx, cy, 228, col_silver);
  it.circle(cx, cy, 197, col_dark_gray);
  it.circle(cx, cy, 196, col_silver);

  // Precision Perimeter Chapter Ring via .rodata DIAL_SIN / DIAL_COS
  for (int s = 0; s < 60; s++) {
    float t_sin = DIAL_SIN[s];
    float t_cos = DIAL_COS[s];
    if (s % 5 == 0) {
      float perp_x = -t_cos;
      float perp_y = t_sin;
      int x1 = cx + (int)(196.0f * t_sin);
      int y1 = cy - (int)(196.0f * t_cos);
      int x2 = cx + (int)(227.0f * t_sin);
      int y2 = cy - (int)(227.0f * t_cos);
      it.line(x1 + (int)perp_x, y1 + (int)perp_y, x2 + (int)perp_x, y2 + (int)perp_y, col_silver);
      it.line(x1 - (int)perp_x, y1 - (int)perp_y, x2 - (int)perp_x, y2 - (int)perp_y, col_slate);
      it.line(x1, y1, x2, y2, col_white);
    } else {
      int x1 = cx + (int)(210.0f * t_sin);
      int y1 = cy - (int)(210.0f * t_cos);
      int x2 = cx + (int)(227.0f * t_sin);
      int y2 = cy - (int)(227.0f * t_cos);
      it.line(x1, y1, x2, y2, col_slate);
      it.line(x1 + 1, y1, x2 + 1, y2, col_slate);
    }
  }

  static constexpr struct {
    int16_t x, y;
    const char *str;
  } CHRONO_NUMS[11] = {
      {233, 61, "0"},   {319, 84, "5"},   {382, 147, "10"}, {405, 233, "15"},
      {382, 319, "20"}, {319, 382, "25"}, {147, 382, "35"}, {84, 319, "40"},
      {61, 233, "45"},  {84, 147, "50"},  {147, 84, "55"}};
  for (const auto &n : CHRONO_NUMS) {
    it.print(n.x, n.y, font_small, col_white, esphome::display::TextAlign::CENTER, n.str);
  }

  uint32_t total_ms = stopwatch_running ? (now_ms - stopwatch_start_time) : stopwatch_elapsed_ms;
  int mins = (total_ms / 60000U) % 60U;
  int secs = (total_ms / 1000U) % 60U;
  int tenths = (total_ms / 100U) % 10U;

  it.print(cx, 132, font_small, col_silver, esphome::display::TextAlign::CENTER, "CHRONOGRAPH");

  char main_buf[32];
  snprintf(main_buf, sizeof(main_buf), "%02d:%02d.%d", mins, secs, tenths);
  it.print(cx, 238, font_date, col_white, esphome::display::TextAlign::CENTER, main_buf);

  if (stopwatch_lap_ms > 0) {
    int l_mins = (stopwatch_lap_ms / 60000U) % 60U;
    int l_secs = (stopwatch_lap_ms / 1000U) % 60U;
    int l_tenths = (stopwatch_lap_ms / 100U) % 10U;
    char lap_buf[32];
    snprintf(lap_buf, sizeof(lap_buf), "SPLIT  %02d:%02d.%d", l_mins, l_secs, l_tenths);
    it.print(cx, 274, font_small, col_red, esphome::display::TextAlign::CENTER, lap_buf);
  }

  const int btn_y = 332;
  const int btn_lap_x = 176;
  const int btn_rst_x = 290;
  const int btn_r = 36;

  it.filled_circle(btn_lap_x, btn_y, btn_r - 2, col_dark_gray);
  it.circle(btn_lap_x, btn_y, btn_r, col_silver);
  it.circle(btn_lap_x, btn_y, btn_r - 1, col_slate);
  it.circle(btn_lap_x, btn_y, btn_r - 4, (stopwatch_lap_ms > 0) ? col_red : col_slate);
  it.print(btn_lap_x, btn_y, font_small, col_silver, esphome::display::TextAlign::CENTER, "LAP");

  it.filled_circle(btn_rst_x, btn_y, btn_r - 2, col_dark_gray);
  it.circle(btn_rst_x, btn_y, btn_r, col_silver);
  it.circle(btn_rst_x, btn_y, btn_r - 1, col_slate);
  it.circle(btn_rst_x, btn_y, btn_r - 4, col_slate);
  it.print(btn_rst_x, btn_y, font_small, col_silver, esphome::display::TextAlign::CENTER, "RESET");

  // Split-Second (Rattrapante) Ghost Hand via fast_sincos
  if (stopwatch_lap_ms > 0) {
    float lap_angle = ((stopwatch_lap_ms % 60000U) / 60000.0f) * (2.0f * PI_F);
    float l_sin = 0.0f, l_cos = 1.0f;
    fast_sincos(lap_angle, l_sin, l_cos);
    it.line(cx, cy, cx + (int)(154.0f * l_sin), cy - (int)(154.0f * l_cos), col_red);
    it.line(cx + 1, cy, cx + (int)(154.0f * l_sin) + 1, cy - (int)(154.0f * l_cos), col_red);
    it.circle(cx + (int)(158.0f * l_sin), cy - (int)(158.0f * l_cos), 4, col_red);
  }

  // Main Central Arrow Hand (20 FPS High-Beat Sweep via fast_sincos)
  float chrono_angle = ((total_ms % 60000U) / 60000.0f) * (2.0f * PI_F);
  float s_sin = 0.0f, s_cos = 1.0f;
  fast_sincos(chrono_angle, s_sin, s_cos);

  int tip_x = cx + (int)(178.0f * s_sin);
  int tip_y = cy - (int)(178.0f * s_cos);
  int arr_base_x = cx + (int)(152.0f * s_sin);
  int arr_base_y = cy - (int)(152.0f * s_cos);
  int tail_x = cx - (int)(36.0f * s_sin);
  int tail_y = cy + (int)(36.0f * s_cos);

  float perp_x = s_cos;
  float perp_y = s_sin;
  int left_x = arr_base_x + (int)(8.0f * perp_x);
  int left_y = arr_base_y + (int)(8.0f * perp_y);
  int right_x = arr_base_x - (int)(8.0f * perp_x);
  int right_y = arr_base_y - (int)(8.0f * perp_y);

  it.line(tail_x + (int)perp_x, tail_y + (int)perp_y, arr_base_x + (int)perp_x, arr_base_y + (int)perp_y, col_silver);
  it.line(tail_x - (int)perp_x, tail_y - (int)perp_y, arr_base_x - (int)perp_x, arr_base_y - (int)perp_y, col_slate);
  it.line(tail_x, tail_y, arr_base_x, arr_base_y, col_white);

  it.filled_triangle(tip_x, tip_y, left_x, left_y, right_x, right_y, col_white);
  it.line(tip_x, tip_y, left_x, left_y, col_silver);
  it.line(tip_x, tip_y, right_x, right_y, col_slate);
  it.line(left_x, left_y, right_x, right_y, col_silver);

  it.filled_circle(tail_x, tail_y, 8, col_dark_gray);
  it.circle(tail_x, tail_y, 8, col_silver);
  it.filled_circle(tail_x, tail_y, 3, col_silver);

  it.filled_circle(cx, cy, 9, col_silver);
  it.circle(cx, cy, 9, col_slate);
  it.filled_circle(cx, cy, 6, col_dark_gray);
  it.filled_circle(cx, cy, 2, col_white);

  it.print(cx, 408, font_small, col_white, esphome::display::TextAlign::CENTER, "< BACK");
}

template<typename DisplayT, typename FontBigT, typename FontSmallT>
inline void render_tool_dice_roll(DisplayT &it, int cx, uint32_t now_ms,
                                  bool &dice_anim_active, uint32_t dice_anim_start_ms,
                                  int dice_val_d6, int dice_val_d6_2,
                                  FontBigT *font_date, FontSmallT *font_small,
                                  esphome::Color theme_primary, esphome::Color theme_secondary,
                                  esphome::Color col_white, esphome::Color col_dark_gray) {
  int cur_val_1 = dice_val_d6;
  int cur_val_2 = dice_val_d6_2;
  int hop_y1 = 0, hop_y2 = 0;
  int jostle_x1 = 0, jostle_x2 = 0;
  int shadow_extra = 0;
  float anim_p = 1.0f;

  if (dice_anim_active) {
    uint32_t elapsed = now_ms - dice_anim_start_ms;
    const uint32_t roll_dur = 680U;
    if (elapsed >= roll_dur) {
      dice_anim_active = false;
    } else {
      anim_p = static_cast<float>(elapsed) / static_cast<float>(roll_dur);
      float damp = 1.0f - anim_p;
      hop_y1 = -static_cast<int>(std::abs(fast_sin(anim_p * PI_F * 2.2f)) * 34.0f * damp);
      hop_y2 = -static_cast<int>(std::abs(fast_sin(anim_p * PI_F * 2.2f + 0.7f)) * 32.0f * damp);
      jostle_x1 = static_cast<int>(fast_sin(anim_p * PI_F * 6.0f) * 8.0f * damp);
      jostle_x2 = -static_cast<int>(fast_cos(anim_p * PI_F * 6.0f) * 8.0f * damp);
      shadow_extra = static_cast<int>(6.0f * damp);

      uint32_t step = elapsed / 55U;
      cur_val_1 = 1 + (static_cast<int>(hash2d_01(static_cast<int>(step), 17) * 60.0f) % 6);
      cur_val_2 = 1 + (static_cast<int>(hash2d_01(static_cast<int>(step) + 31, 53) * 60.0f) % 6);
    }
  }

  const int dice_base_y = 192;
  const int d1_cx = cx - 75 + jostle_x1;
  const int d2_cx = cx + 75 + jostle_x2;
  const int d1_cy = dice_base_y + hop_y1;
  const int d2_cy = dice_base_y + hop_y2;
  const int d_size = 132;
  const int cr = 16;

  // Opaque Back-Face Culled 3D Airborne Tumbling Cube Renderer (Zero see-through rear lines!)
  auto draw_tumbling_cube_3d = [&](int center_x, int center_y, float rx, float ry, float rz) {
    static constexpr float CUBE_V[8][3] = {
      {-1.0f, -1.0f, -1.0f}, { 1.0f, -1.0f, -1.0f}, { 1.0f,  1.0f, -1.0f}, {-1.0f,  1.0f, -1.0f},
      {-1.0f, -1.0f,  1.0f}, { 1.0f, -1.0f,  1.0f}, { 1.0f,  1.0f,  1.0f}, {-1.0f,  1.0f,  1.0f}
    };
    // 6 outward-oriented faces (CCW when viewed from outside the cube)
    static constexpr uint8_t CUBE_F[6][4] = {
      {0, 1, 2, 3}, // Face 0 (-Z): 1 pip
      {5, 4, 7, 6}, // Face 1 (+Z): 6 pips
      {4, 5, 1, 0}, // Face 2 (-Y): 2 pips
      {3, 2, 6, 7}, // Face 3 (+Y): 5 pips
      {4, 0, 3, 7}, // Face 4 (-X): 3 pips
      {1, 5, 6, 2}  // Face 5 (+X): 4 pips
    };

    const float half_s = 46.0f;
    ProjectedPoint pts[8];
    Vec3 rv[8];
    for (int i = 0; i < 8; i++) {
      rv[i] = rotate_3d_xyz({CUBE_V[i][0] * half_s, CUBE_V[i][1] * half_s, CUBE_V[i][2] * half_s}, rx, ry, rz);
      pts[i] = project_3d(rv[i], center_x, center_y, 260.0f, 280.0f);
    }

    // Soft floor shadow beneath airborne cube
    it.filled_circle(center_x + 6, center_y + 18 - hop_y1 / 2, 40, esphome::Color(16, 20, 28));

    esphome::Color c_border(158, 150, 138);
    esphome::Color c_pip(25, 28, 35);

    // Render only front-facing cube faces (screen-space cross > 0 -> 100% solid, opaque cube)
    for (int f = 0; f < 6; f++) {
      uint8_t i0 = CUBE_F[f][0], i1 = CUBE_F[f][1], i2 = CUBE_F[f][2], i3 = CUBE_F[f][3];
      int cross = (pts[i1].x - pts[i0].x) * (pts[i2].y - pts[i0].y) -
                  (pts[i1].y - pts[i0].y) * (pts[i2].x - pts[i0].x);
      if (cross <= 0) continue; // Cull back-facing faces completely

      // Directional 3D shading per face based on face center Z/Y normal orientation
      float nx = (rv[i0].x + rv[i1].x + rv[i2].x + rv[i3].x) * (0.25f / half_s);
      float ny = (rv[i0].y + rv[i1].y + rv[i2].y + rv[i3].y) * (0.25f / half_s);
      float nz = (rv[i0].z + rv[i1].z + rv[i2].z + rv[i3].z) * (0.25f / half_s);
      float light = fmaxf(0.74f, fminf(1.0f, 0.84f + (-nz * 0.12f - ny * 0.10f - nx * 0.06f)));
      esphome::Color face_col(
          static_cast<uint8_t>(248.0f * light),
          static_cast<uint8_t>(244.0f * light),
          static_cast<uint8_t>(236.0f * light));

      // Fill opaque quad (2 triangles) + draw crisp perimeter bevel edges
      it.filled_triangle(pts[i0].x, pts[i0].y, pts[i1].x, pts[i1].y, pts[i2].x, pts[i2].y, face_col);
      it.filled_triangle(pts[i0].x, pts[i0].y, pts[i2].x, pts[i2].y, pts[i3].x, pts[i3].y, face_col);
      it.line(pts[i0].x, pts[i0].y, pts[i1].x, pts[i1].y, c_border);
      it.line(pts[i1].x, pts[i1].y, pts[i2].x, pts[i2].y, c_border);
      it.line(pts[i2].x, pts[i2].y, pts[i3].x, pts[i3].y, c_border);
      it.line(pts[i3].x, pts[i3].y, pts[i0].x, pts[i0].y, c_border);

      // Bilinear helper to project pips directly onto the 3D quad face (u, v in [0..1])
      auto draw_face_pip = [&](float u, float v) {
        float px = (1.0f - u) * (1.0f - v) * pts[i0].x + u * (1.0f - v) * pts[i1].x +
                   u * v * pts[i2].x + (1.0f - u) * v * pts[i3].x;
        float py = (1.0f - u) * (1.0f - v) * pts[i0].y + u * (1.0f - v) * pts[i1].y +
                   u * v * pts[i2].y + (1.0f - u) * v * pts[i3].y;
        it.filled_circle(static_cast<int>(px), static_cast<int>(py), 3, c_pip);
      };

      // Standard D6 pip patterns per face (f = 0..5 -> 1..6 pips)
      if (f == 0 || f == 4 || f == 3) { // 1, 3, 5 center pip
        draw_face_pip(0.50f, 0.50f);
      }
      if (f >= 1) { // 2, 3, 4, 5, 6 diagonal pair
        draw_face_pip(0.26f, 0.26f);
        draw_face_pip(0.74f, 0.74f);
      }
      if (f == 1 || f == 3 || f == 5) { // 4, 5, 6 second diagonal pair
        draw_face_pip(0.74f, 0.26f);
        draw_face_pip(0.26f, 0.74f);
      }
      if (f == 1) { // 6 middle pair
        draw_face_pip(0.26f, 0.50f);
        draw_face_pip(0.74f, 0.50f);
      }
    }
  };
  auto draw_rounded_d6 = [&](int center_x, int center_y, int val) {
    int x0 = center_x - d_size / 2;
    int y0 = center_y - d_size / 2;
    int sx0 = x0 + 6 + shadow_extra;
    int sy0 = y0 + 8 + shadow_extra;

    esphome::Color sh_col(16, 20, 28);
    it.filled_rectangle(sx0 + cr, sy0, d_size - 2 * cr, d_size, sh_col);
    it.filled_rectangle(sx0, sy0 + cr, d_size, d_size - 2 * cr, sh_col);
    it.filled_circle(sx0 + cr, sy0 + cr, cr, sh_col);
    it.filled_circle(sx0 + d_size - cr, sy0 + cr, cr, sh_col);
    it.filled_circle(sx0 + cr, sy0 + d_size - cr, cr, sh_col);
    it.filled_circle(sx0 + d_size - cr, sy0 + d_size - cr, cr, sh_col);

    esphome::Color body_col(248, 246, 240);
    esphome::Color border_col(182, 176, 164);
    it.filled_rectangle(x0 + cr, y0, d_size - 2 * cr, d_size, body_col);
    it.filled_rectangle(x0, y0 + cr, d_size, d_size - 2 * cr, body_col);
    it.filled_circle(x0 + cr, y0 + cr, cr, body_col);
    it.filled_circle(x0 + d_size - cr, y0 + cr, cr, body_col);
    it.filled_circle(x0 + cr, y0 + d_size - cr, cr, body_col);
    it.filled_circle(x0 + d_size - cr, y0 + d_size - cr, cr, body_col);

    it.line(x0 + cr, y0 + 1, x0 + d_size - cr, y0 + 1, col_white);
    it.line(x0 + 1, y0 + cr, x0 + 1, y0 + d_size - cr, col_white);
    it.line(x0 + cr, y0 + d_size - 1, x0 + d_size - cr, y0 + d_size - 1, border_col);
    it.line(x0 + d_size - 1, y0 + cr, x0 + d_size - 1, y0 + d_size - cr, border_col);

    const int pr = 9;
    const int p_dist = 34;
    esphome::Color pip_c(25, 28, 35);
    esphome::Color pip_rim(85, 92, 105);
    auto draw_pip = [&](int px, int py) {
      it.filled_circle(px, py, pr, pip_c);
      it.draw_pixel_at(px + 4, py + 5, pip_rim);
      it.draw_pixel_at(px + 5, py + 4, pip_rim);
      it.draw_pixel_at(px + 3, py + 6, pip_rim);
    };

    bool p_center = (val == 1 || val == 3 || val == 5);
    bool p_tl_br  = (val >= 2);
    bool p_tr_bl  = (val >= 4);
    bool p_mid_lr = (val == 6);

    if (p_center) draw_pip(center_x, center_y);
    if (p_tl_br)  { draw_pip(center_x - p_dist, center_y - p_dist); draw_pip(center_x + p_dist, center_y + p_dist); }
    if (p_tr_bl)  { draw_pip(center_x + p_dist, center_y - p_dist); draw_pip(center_x - p_dist, center_y + p_dist); }
    if (p_mid_lr) { draw_pip(center_x - p_dist, center_y); draw_pip(center_x + p_dist, center_y); }
  };

  if (dice_anim_active && anim_p < 0.80f) {
    float spin = anim_p * 7.8f;
    draw_tumbling_cube_3d(d1_cx, d1_cy, spin * 1.1f, spin * 1.4f + 0.5f, spin * 0.7f);
    draw_tumbling_cube_3d(d2_cx, d2_cy, -spin * 1.2f + 0.8f, spin * 1.0f, -spin * 0.9f);
  } else {
    draw_rounded_d6(d1_cx, d1_cy, dice_val_d6);
    draw_rounded_d6(d2_cx, d2_cy, dice_val_d6_2);
  }

  if (dice_anim_active) {
    it.print(cx, 324, font_date, theme_secondary, esphome::display::TextAlign::CENTER, "ROLLING...");
  } else {
    char total_buf[32];
    snprintf(total_buf, sizeof(total_buf), "TOTAL: %d", cur_val_1 + cur_val_2);
    it.print(cx, 324, font_date, col_white, esphome::display::TextAlign::CENTER, total_buf);
  }

  render_back_button(it, cx, font_small, 426, "< BACK");
}
template<typename DisplayT, typename FontBigT, typename FontSmallT>
inline void render_tool_bubble_level(DisplayT &it, int cx, float ax, float ay, float az,
                                     FontBigT *font_date, FontSmallT *font_small,
                                     esphome::Color theme_primary, esphome::Color theme_secondary,
                                     esphome::Color col_white, esphome::Color col_gray,
                                     esphome::Color col_slate, esphome::Color col_dark_gray,
                                     esphome::Color col_green) {
  if (std::isnan(ax)) ax = 0.0f;
  if (std::isnan(ay)) ay = 0.0f;
  if (std::isnan(az)) az = 9.8f;

  float p_rad = std::atan2(ay, std::sqrt(ax * ax + az * az));
  float r_rad = std::atan2(ax, std::sqrt(ay * ay + az * az));
  float raw_p_deg = p_rad * (180.0f / PI_F);
  float raw_r_deg = r_rad * (180.0f / PI_F);

  // 20 FPS EMA Liquid Damping Filter (eliminates 5 Hz accelerometer step-jumping)
  static float smooth_p_deg = 0.0f;
  static float smooth_r_deg = 0.0f;
  static bool init_level = false;
  if (!init_level) {
    smooth_p_deg = raw_p_deg;
    smooth_r_deg = raw_r_deg;
    init_level = true;
  } else {
    smooth_p_deg += (raw_p_deg - smooth_p_deg) * 0.28f;
    smooth_r_deg += (raw_r_deg - smooth_r_deg) * 0.28f;
  }

  float total_tilt = std::sqrt(smooth_p_deg * smooth_p_deg + smooth_r_deg * smooth_r_deg);
  bool is_level = (total_tilt <= 0.5f);
  esphome::Color ring_c = is_level ? col_green : col_slate;

  // 1. Top Total-Tilt Degree Readout at y = 44 (replaces bottom 2-line readout)
  char deg_buf[16];
  snprintf(deg_buf, sizeof(deg_buf), "%.1f°", total_tilt);
  it.print(cx, 44, font_date, is_level ? col_green : col_white,
           esphome::display::TextAlign::CENTER, deg_buf);

  // 2. Maximized 300px-Diameter Bulls-Eye Spirit Vial (outer_r = 150 at level_cy = 226)
  //    Spans y = 76..376 with a clean 20px buffer below top degree text (y = 56)
  //    and a 20px buffer above the bottom < BACK button (y = 396..424).
  const int level_cy = 226;
  const int outer_r = 150;

  // 12-point 30-degree radial bezel ticks via .rodata DIAL_SIN / DIAL_COS
  for (int i = 0; i < 60; i += 5) {
    int x1 = cx + (int)(DIAL_SIN[i] * (float)(outer_r - 7));
    int y1 = level_cy - (int)(DIAL_COS[i] * (float)(outer_r - 7));
    int x2 = cx + (int)(DIAL_SIN[i] * (float)outer_r);
    int y2 = level_cy - (int)(DIAL_COS[i] * (float)outer_r);
    it.line(x1, y1, x2, y2, col_gray);
  }

  it.circle(cx, level_cy, outer_r, col_slate);
  it.circle(cx, level_cy, outer_r - 1, col_dark_gray);
  it.circle(cx, level_cy, 76, col_gray);
  it.circle(cx, level_cy, 29, ring_c);
  it.circle(cx, level_cy, 28, ring_c);

  // Cardinal Crosshairs
  it.line(cx - 156, level_cy, cx - 35, level_cy, ring_c);
  it.line(cx + 35, level_cy, cx + 156, level_cy, ring_c);
  it.line(cx, level_cy - 156, cx, level_cy - 35, ring_c);
  it.line(cx, level_cy + 35, cx, level_cy + 156, ring_c);
  it.filled_circle(cx, level_cy, 2, ring_c);

  // Spirit Level Bubble Position (clamped inside outer vial r = 126)
  float fbx = (float)cx - smooth_r_deg * 14.0f;
  float fby = (float)level_cy - smooth_p_deg * 14.0f;
  float bdx = fbx - (float)cx;
  float bdy = fby - (float)level_cy;
  float b_dist_sq = bdx * bdx + bdy * bdy;
  if (b_dist_sq > 126.0f * 126.0f) {
    float b_dist = std::sqrt(b_dist_sq);
    fbx = (float)cx + (bdx / b_dist) * 126.0f;
    fby = (float)level_cy + (bdy / b_dist) * 126.0f;
  }
  int bx = (int)fbx;
  int by = (int)fby;

  // 3D Glass Spirit Bubble (r = 22: Shadow, Meniscus Rim, Dual Specular Highlights)
  it.filled_circle(bx + 2, by + 3, 22, esphome::Color(12, 16, 22));
  esphome::Color bub_fill = is_level ? col_green : theme_secondary;
  it.filled_circle(bx, by, 22, bub_fill);
  it.circle(bx, by, 22, col_white);
  it.circle(bx, by, 20, col_white);
  it.filled_circle(bx - 7, by - 7, 4, col_white);
  it.filled_circle(bx + 8, by + 8, 2, col_white);

  render_back_button(it, cx, font_small, 426, "< BACK");
}

template<typename DisplayT, typename FontBigT, typename FontSmallT>
inline void render_tool_coin_flip(DisplayT &it, int cx, uint32_t now_ms,
                                  bool &coin_anim_active, uint32_t coin_anim_start_ms,
                                  int coin_side,
                                  FontBigT *font_date, FontSmallT *font_small,
                                  esphome::Color theme_primary, esphome::Color /*theme_secondary*/,
                                  esphome::Color /*col_white*/, esphome::Color col_dark_gray) {
  (void)now_ms;
  static uint32_t last_start_ms = 0;
  static int flip_frame = 0;
  static int start_side = 0;

  float scale_y = 1.0f;
  float y_hop = 0.0f;
  float z_zoom = 1.0f;
  float theta = (coin_side == 0) ? 0.0f : PI_F;
  bool show_heads = (coin_side == 0);
  bool is_edge_flash = false;

  if (coin_anim_active) {
    if (coin_anim_start_ms != last_start_ms) {
      last_start_ms = coin_anim_start_ms;
      flip_frame = 0;
      // Always start opposite to final side if needed so the toss visibly turns over 4 or 5 times!
      start_side = (coin_side == 0) ? 0 : 1;
    }

    // 11 guaranteed rendered frames per toss (~31% shorter duration, faster spin cadence):
    // Packs 4 razor-thin milled-edge flashes and 4 full face reversals into 11 crisp frames!
    static constexpr int TOTAL_FRAMES = 11;
    if (flip_frame >= TOTAL_FRAMES) {
      coin_anim_active = false;
      scale_y = 1.0f;
      y_hop = 0.0f;
      z_zoom = 1.0f;
      show_heads = (coin_side == 0);
    } else {
      static constexpr float STEP_SCALES[11] = {
        0.84f, 0.06f, 0.56f, 0.90f, 0.06f, 0.58f, 0.90f, 0.06f, 0.86f, 0.06f, 0.58f
      };
      // 0 = coin_side, 1 = opposite side (alternates on every edge crossing at frames 1, 4, 7, 9)
      static constexpr uint8_t STEP_OPP[11] = {
        0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0
      };

      float p = static_cast<float>(flip_frame + 1) / static_cast<float>(TOTAL_FRAMES + 1);
      float arc = fast_sin(p * PI_F);
      y_hop = -68.0f * arc;
      z_zoom = 0.96f + 0.20f * arc;

      scale_y = STEP_SCALES[flip_frame];
      is_edge_flash = (scale_y < 0.10f);
      int cur_side = STEP_OPP[flip_frame] ? (1 - coin_side) : coin_side;
      show_heads = (cur_side == 0);
      theta = static_cast<float>(flip_frame) * (PI_F * 0.36f);
      flip_frame++;
    }
  } else {
    last_start_ms = 0;
  }
  const int coin_base_y = 216;
  int coin_cy = coin_base_y + static_cast<int>(y_hop);
  const int r_outer = static_cast<int>(116.0f * z_zoom);

  // 1. Dynamic 3D Floor Shadow (shrinks as coin rises toward the viewer)
  int sh_base_y = 312;
  float shadow_scale = fmaxf(0.32f, 1.0f - (-y_hop / 95.0f));
  int sh_rx = static_cast<int>(94.0f * shadow_scale);
  int sh_ry = std::max(3, static_cast<int>((16.0f + 10.0f * scale_y) * shadow_scale));
  for (int sdy = -sh_ry; sdy <= sh_ry; sdy++) {
    int lut_idx = 233 + (sdy * 232) / sh_ry;
    if (lut_idx < 0) lut_idx = 0;
    if (lut_idx > 465) lut_idx = 465;
    int sdx = (sh_rx * CIRCLE_HW[lut_idx]) / 233;
    it.horizontal_line(cx - sdx, sh_base_y + sdy, 2 * sdx + 1, esphome::Color(12, 16, 22));
  }

  // All-Silver Struck Medallion Palette (Zero Blue — Pure 3D Engraved Silver Quarter / Dollar)
  esphome::Color col_rim_dark(64, 72, 84);
  esphome::Color col_rim_mid(134, 144, 160);
  esphome::Color col_bevel_hi(248, 252, 255);
  esphome::Color col_groove_sh(82, 92, 108);
  esphome::Color col_relief_shadow(52, 60, 72);
  esphome::Color col_relief_hi(252, 254, 255);
  esphome::Color col_engrave_recess(74, 84, 98);

  float spec_center = -0.35f + 0.65f * fast_sin(theta * 2.0f);
  int b_outer = std::max(4, static_cast<int>(r_outer * scale_y));
  int edge_h = is_edge_flash ? 18 : std::max(3, static_cast<int>(15.0f * (1.0f - scale_y * 0.78f)));

  // 2. O(1) Crescent 3D Milled Silver Edge Band (prominent 18px reeded silver bar on edge-flash frames!)
  for (int dy = 1; dy <= b_outer + edge_h; dy++) {
    int src_dy = (dy > edge_h) ? (dy - edge_h) : 0;
    int lut_idx = 233 + (src_dy * 232) / b_outer;
    if (lut_idx < 0) lut_idx = 0;
    if (lut_idx > 465) lut_idx = 465;
    int dx_outer = (r_outer * CIRCLE_HW[lut_idx]) / 233;
    esphome::Color ec = (dy >= b_outer + edge_h - 1)
                            ? col_rim_dark
                            : (is_edge_flash ? esphome::Color(195, 206, 222) : col_rim_mid);
    it.horizontal_line(cx - dx_outer, coin_cy + dy, 2 * dx_outer + 1, ec);
  }

  // 3. 3D Brushed-Silver Coin Field (Recessed Satin Pewter-Silver Field so Raised Relief Pops!)
  for (int dy = -b_outer; dy <= b_outer; dy++) {
    int lut_idx = 233 + (dy * 232) / b_outer;
    if (lut_idx < 0) lut_idx = 0;
    if (lut_idx > 465) lut_idx = 465;
    int dx_outer = (r_outer * CIRCLE_HW[lut_idx]) / 233;

    float u_y = static_cast<float>(dy) / static_cast<float>(b_outer);
    float d_spec = std::fabs(u_y - spec_center);
    float spec_boost = fmaxf(0.0f, 1.0f - d_spec * 2.1f);
    float sheen = (is_edge_flash ? 215.0f : 156.0f) - u_y * 18.0f + spec_boost * 42.0f;
    uint8_t sr = static_cast<uint8_t>(fminf(248.0f, sheen * 0.96f));
    uint8_t sg = static_cast<uint8_t>(fminf(250.0f, sheen * 0.99f));
    uint8_t sb = static_cast<uint8_t>(fminf(255.0f, sheen * 1.04f));
    it.horizontal_line(cx - dx_outer, coin_cy + dy, 2 * dx_outer + 1, esphome::Color(sr, sg, sb));

    it.draw_pixel_at(cx - dx_outer, coin_cy + dy, col_rim_dark);
    it.draw_pixel_at(cx + dx_outer, coin_cy + dy, col_rim_dark);
    if (dx_outer > 4) {
      it.draw_pixel_at(cx - dx_outer + 1, coin_cy + dy, col_bevel_hi);
      it.draw_pixel_at(cx - dx_outer + 2, coin_cy + dy, col_bevel_hi);
      it.draw_pixel_at(cx + dx_outer - 1, coin_cy + dy, col_rim_mid);
      it.draw_pixel_at(cx + dx_outer - 2, coin_cy + dy, col_rim_mid);
    }
    if (dx_outer > 11 && !is_edge_flash) {
      esphome::Color dish_l = (dy < 0) ? col_groove_sh : col_bevel_hi;
      esphome::Color dish_r = (dy < 0) ? col_bevel_hi  : col_groove_sh;
      it.draw_pixel_at(cx - dx_outer + 8, coin_cy + dy, dish_l);
      it.draw_pixel_at(cx - dx_outer + 9, coin_cy + dy, dish_l);
      it.draw_pixel_at(cx + dx_outer - 8, coin_cy + dy, dish_r);
      it.draw_pixel_at(cx + dx_outer - 9, coin_cy + dy, dish_r);
    }
  }

  // Vertical milled reeding ticks along the front 3D coin thickness band
  int reed_step = is_edge_flash ? 5 : 6;
  for (int rx_off = -r_outer + 6; rx_off <= r_outer - 6; rx_off += reed_step) {
    int lut_x = 233 + (rx_off * 232) / r_outer;
    if (lut_x < 0) lut_x = 0;
    if (lut_x > 465) lut_x = 465;
    int y_rim = coin_cy + (b_outer * CIRCLE_HW[lut_x]) / 233;
    esphome::Color reed_c = ((rx_off / reed_step) & 1) ? col_bevel_hi : col_rim_dark;
    it.line(cx + rx_off, y_rim + 1, cx + rx_off, y_rim + edge_h - 1, reed_c);
  }

  if (scale_y >= 0.96f && !coin_anim_active) {
    it.circle(cx, coin_cy, r_outer, col_rim_dark);
    it.circle(cx, coin_cy, r_outer - 1, col_bevel_hi);
    it.circle(cx, coin_cy, r_outer - 8, col_groove_sh);
    it.circle(cx, coin_cy, r_outer - 9, col_bevel_hi);
  }

  // 4. Enlarged (+30%) All-Silver 3D Struck Bas-Relief Home Assistant Emblem & Quarter Lettering
  if (scale_y > 0.22f) {
    int hx = cx;
    int hy = coin_cy - static_cast<int>(18.0f * scale_y * z_zoom);
    int hw = static_cast<int>(62.0f * z_zoom);
    int y_top = hy - static_cast<int>(56.0f * scale_y * z_zoom);
    int y_eave = hy - static_cast<int>(2.0f * scale_y * z_zoom);
    int y_bot = hy + static_cast<int>(54.0f * scale_y * z_zoom);

    // Pass 1: Deep 3D struck relief shadow (+3px lower-right) & gleaming upper-left bevel (-2px)
    for (int y = y_top; y <= y_bot; y++) {
      int cur_hw = 0;
      if (y < y_eave) {
        if (y_eave > y_top) {
          cur_hw = static_cast<int>(static_cast<float>(y - y_top) / static_cast<float>(y_eave - y_top) * static_cast<float>(hw));
        }
      } else {
        int dy_b = y_bot - y;
        cur_hw = (dy_b < 5 && scale_y > 0.75f) ? (hw - (5 - dy_b)) : hw;
      }
      if (cur_hw > 0) {
        it.horizontal_line(hx - cur_hw + 2, y + 2, 2 * cur_hw + 2, col_relief_shadow);
        it.horizontal_line(hx - cur_hw - 2, y - 1, 2 * cur_hw + 1, col_relief_hi);
      }
    }

    // Pass 2: Raised Brushed-Silver Bas-Relief House Plateau
    for (int y = y_top; y <= y_bot; y++) {
      int cur_hw = 0;
      if (y < y_eave) {
        if (y_eave > y_top) {
          cur_hw = static_cast<int>(static_cast<float>(y - y_top) / static_cast<float>(y_eave - y_top) * static_cast<float>(hw));
        }
      } else {
        int dy_b = y_bot - y;
        cur_hw = (dy_b < 5 && scale_y > 0.75f) ? (hw - (5 - dy_b)) : hw;
      }
      if (cur_hw > 0) {
        float rel_u = static_cast<float>(y - y_top) / static_cast<float>(y_bot - y_top + 1);
        uint8_t rl = static_cast<uint8_t>(232.0f - rel_u * 26.0f);
        it.horizontal_line(hx - cur_hw, y, 2 * cur_hw + 1, esphome::Color(rl, static_cast<uint8_t>(rl + 2), static_cast<uint8_t>(fminf(255.0f, rl + 7.0f))));
        it.draw_pixel_at(hx - cur_hw, y, col_groove_sh);
        it.draw_pixel_at(hx + cur_hw, y, col_relief_shadow);
      }
    }

    if (show_heads) {
      int eye_dx = static_cast<int>(26.0f * z_zoom);
      int eye_y = hy + static_cast<int>(7.0f * scale_y * z_zoom);
      int eye_r = std::max(2, static_cast<int>(8.0f * scale_y * z_zoom));
      it.filled_circle(hx - eye_dx + 1, eye_y + 1, eye_r, col_relief_hi);
      it.filled_circle(hx + eye_dx + 1, eye_y + 1, eye_r, col_relief_hi);
      it.filled_circle(hx - eye_dx, eye_y, eye_r, col_relief_shadow);
      it.filled_circle(hx + eye_dx, eye_y, eye_r, col_relief_shadow);
      it.filled_circle(hx - eye_dx + 1, eye_y + 1, std::max(1, eye_r - 2), col_engrave_recess);
      it.filled_circle(hx + eye_dx + 1, eye_y + 1, std::max(1, eye_r - 2), col_engrave_recess);

      int smile_top_y = hy + static_cast<int>(22.0f * scale_y * z_zoom);
      int smile_mid_y = hy + static_cast<int>(28.0f * scale_y * z_zoom);
      int smile_bot_y = hy + static_cast<int>(40.0f * scale_y * z_zoom);
      int smile_hw = static_cast<int>(20.0f * z_zoom);

      for (int y = smile_top_y; y <= smile_mid_y; y++) {
        for (int t = -2; t <= 1; t++) {
          it.draw_pixel_at(hx - smile_hw + t + 1, y + 1, col_relief_hi);
          it.draw_pixel_at(hx + smile_hw + t + 1, y + 1, col_relief_hi);
          it.draw_pixel_at(hx - smile_hw + t, y, col_relief_shadow);
          it.draw_pixel_at(hx + smile_hw + t, y, col_relief_shadow);
        }
      }
      for (int i = 0; i <= 32; i++) {
        float sa = static_cast<float>(i) * (PI_F / 32.0f);
        int sx = hx - static_cast<int>(smile_hw * fast_cos(sa));
        int sy = smile_mid_y + static_cast<int>((smile_bot_y - smile_mid_y) * fast_sin(sa));
        for (int t = -1; t <= 1; t++) {
          it.draw_pixel_at(sx + 1, sy + t + 1, col_relief_hi);
          it.draw_pixel_at(sx, sy + t, col_relief_shadow);
          it.draw_pixel_at(sx + t, sy, col_relief_shadow);
        }
      }

      if (scale_y > 0.76f) {
        int ty = coin_cy + static_cast<int>(74.0f * scale_y * z_zoom);
        if (!coin_anim_active) {
          it.print(cx + 2, ty + 2, font_date, col_relief_shadow, esphome::display::TextAlign::CENTER, "HEADS");
          it.print(cx - 1, ty - 1, font_date, col_relief_hi,     esphome::display::TextAlign::CENTER, "HEADS");
        }
        it.print(cx, ty, font_date, esphome::Color(220, 228, 240), esphome::display::TextAlign::CENTER, "HEADS");
      }
    } else {
      int H_total = y_bot - y_top;
      int branch_x     = static_cast<int>(31.0f * z_zoom);
      int top_node_y   = y_top + static_cast<int>(0.32f * static_cast<float>(H_total));
      int right_node_x = hx + branch_x;
      int right_node_y = y_top + static_cast<int>(0.59f * static_cast<float>(H_total));
      int left_node_x  = hx - branch_x;
      int left_node_y  = y_top + static_cast<int>(0.73f * static_cast<float>(H_total));

      int node_r = std::max(3, static_cast<int>(10.0f * scale_y * z_zoom));
      int line_hw = std::max(1, static_cast<int>(3.0f * scale_y * z_zoom));
      int branch_drop = static_cast<int>(29.0f * scale_y * z_zoom);

      for (int w = -line_hw; w <= line_hw; w++) {
        it.line(hx + w + 2, top_node_y + 1, hx + w + 2, y_bot, col_relief_hi);
        it.line(hx + w + 2, right_node_y + branch_drop + 1, right_node_x + w + 2, right_node_y + 1, col_relief_hi);
        it.line(hx + w + 2, left_node_y + branch_drop + 1, left_node_x + w + 2, left_node_y + 1, col_relief_hi);
      }
      it.filled_circle(hx + 2, top_node_y + 2, node_r, col_relief_hi);
      it.filled_circle(right_node_x + 2, right_node_y + 2, node_r, col_relief_hi);
      it.filled_circle(left_node_x + 2, left_node_y + 2, node_r, col_relief_hi);

      for (int w = -line_hw; w <= line_hw; w++) {
        it.line(hx + w, top_node_y, hx + w, y_bot, col_relief_shadow);
        it.line(hx + w, right_node_y + branch_drop, right_node_x + w, right_node_y, col_relief_shadow);
        it.line(hx + w, left_node_y + branch_drop, left_node_x + w, left_node_y, col_relief_shadow);
      }
      it.filled_circle(hx, top_node_y, node_r, col_relief_shadow);
      it.filled_circle(right_node_x, right_node_y, node_r, col_relief_shadow);
      it.filled_circle(left_node_x, left_node_y, node_r, col_relief_shadow);

      if (scale_y > 0.76f) {
        int ty = coin_cy + static_cast<int>(74.0f * scale_y * z_zoom);
        if (!coin_anim_active) {
          it.print(cx + 2, ty + 2, font_date, col_relief_shadow, esphome::display::TextAlign::CENTER, "TAILS");
          it.print(cx - 1, ty - 1, font_date, col_relief_hi,     esphome::display::TextAlign::CENTER, "TAILS");
        }
        it.print(cx, ty, font_date, esphome::Color(220, 228, 240), esphome::display::TextAlign::CENTER, "TAILS");
      }
    }
  }

  render_back_button(it, cx, font_small, 426, "< BACK");
}
// ============================================================================
// SECTION 14: TOOLS SUITE PART 2 — GLOBAL ALERT OVERLAY, TIMER, ALARMS & CALC
// ============================================================================

template<typename DisplayT, typename TimeT, typename FontTimeT, typename FontDateT,
         typename FontSmallT, typename FontMicroT>
inline void render_alert_ringing_overlay(DisplayT &it, int cx, int cy, uint32_t now_ms,
                                         bool alarm_ringing, int alarm_ringing_idx,
                                         int alarm1_recurrence, int alarm2_recurrence,
                                         const TimeT &now, bool is_24h_format,
                                         FontTimeT *font_time, FontDateT *font_date,
                                         FontSmallT *font_small, FontMicroT *font_micro,
                                         esphome::Color theme_primary,
                                         esphome::Color theme_secondary,
                                         esphome::Color col_bg, esphome::Color col_black,
                                         esphome::Color col_white, esphome::Color col_gray,
                                         esphome::Color col_red,
                                         bool find_my_active = false,
                                         const std::string &va_timer_name = "") {
  it.fill(col_bg);
  uint8_t pulse = (now_ms / 250U) % 2U;
  esphome::Color alert_color = find_my_active ? col_white : (alarm_ringing ? col_red : theme_secondary);
  esphome::Color ring_c = pulse ? alert_color : (find_my_active ? theme_secondary : esphome::Color(20, 20, 20));
  it.circle(cx, cy, 223, ring_c);
  it.circle(cx, cy, 224, ring_c);
  it.circle(cx, cy, 225, ring_c);
  it.circle(cx, cy, 226, ring_c);

  if (find_my_active) {
    // High-visibility expanding radar rings for Find My Pocket Assistant
    int radar_r = 60 + static_cast<int>((now_ms / 8U) % 145U);
    it.circle(cx, cy, radar_r, theme_primary);
    it.circle(cx, cy, radar_r + 1, pulse ? col_white : theme_secondary);
    int radar_r2 = 60 + static_cast<int>(((now_ms / 8U) + 72U) % 145U);
    it.circle(cx, cy, radar_r2, theme_secondary);

    it.print(cx, 75, font_small, pulse ? col_white : theme_secondary,
             esphome::display::TextAlign::CENTER, "LOCATING DEVICE");
    it.print(cx, 156, font_date, col_white,
             esphome::display::TextAlign::CENTER, "POCKET");
    it.print(cx, 196, font_date, theme_primary,
             esphome::display::TextAlign::CENTER, "ASSISTANT");
    it.print(cx, 244, font_small, theme_secondary,
             esphome::display::TextAlign::CENTER, "Acoustic Beacon Active");

    render_pill_button(it, 123, 343, 330, 22, theme_primary, theme_primary,
                       "TAP TO DISMISS", col_white, font_small);
    it.print(cx, 372, font_micro, col_gray,
             esphome::display::TextAlign::CENTER, "(or press Top Crown button)");
  } else if (alarm_ringing) {
    it.print(cx, 75, font_small, pulse ? col_red : col_white,
             esphome::display::TextAlign::CENTER, "ALARM RINGING");

    char cur_t_buf[16] = "--:--";
    char ampm_buf[8] = "";
    if (now.is_valid()) {
      format_time_hhmm(now.hour, now.minute, is_24h_format, cur_t_buf, sizeof(cur_t_buf), ampm_buf);
    }
    it.print(cx, 160, font_time, col_white, esphome::display::TextAlign::CENTER, cur_t_buf);
    if (!is_24h_format && ampm_buf[0] != 0) {
      it.print(cx + 120, 175, font_date, theme_secondary,
               esphome::display::TextAlign::LEFT, ampm_buf);
    }

    int r_idx = (alarm_ringing_idx == 2) ? alarm2_recurrence : alarm1_recurrence;
    static constexpr const char *REC_NAMES[4] = {"DAILY", "WEEKDAYS", "WEEKENDS", "ONCE"};
    it.print(cx, 225, font_small, theme_primary,
             esphome::display::TextAlign::CENTER, REC_NAMES[r_idx & 3]);

    render_pill_button(it, 133, 333, 295, 20, theme_secondary, theme_secondary,
                       "SNOOZE (+5m)", col_black, font_small);
    it.print(cx, 326, font_micro, col_gray,
             esphome::display::TextAlign::CENTER, "(or press Top Crown button)");

    render_pill_button(it, 143, 323, 370, 18, col_red, col_red,
                       "DISMISS", col_white, font_small);
  } else {
    it.print(cx, 75, font_small, pulse ? theme_secondary : col_white,
             esphome::display::TextAlign::CENTER, "TIMER EXPIRED");
    it.print(cx, 170, font_time, col_white, esphome::display::TextAlign::CENTER, "00:00");
    std::string t_lbl = va_timer_name.empty() ? "TIME IS UP!" : fit_text_to_width_font(va_timer_name, 260, font_small);
    it.print(cx, 230, font_small, theme_secondary,
             esphome::display::TextAlign::CENTER, t_lbl.c_str());

    render_pill_button(it, 133, 333, 330, 22, theme_primary, theme_primary,
                       "DISMISS", col_white, font_small);
    it.print(cx, 372, font_micro, col_gray,
             esphome::display::TextAlign::CENTER, "(or press Top Crown button)");
  }
}

// ==============================================================================
// 3-SLOT MULTI-TIMER & SAVED PRESET ENGINE
// ==============================================================================
struct MultiTimerSlot {
  std::string id;
  std::string name;
  int duration_sec;
  int remaining_sec;
  uint32_t target_ms;
  bool running;
  bool paused;
};

inline MultiTimerSlot *timer_slots() {
  static MultiTimerSlot slots[3] = {
    {"", "", 300, 300, 0, false, false},
    {"", "", 600, 600, 0, false, false},
    {"", "", 900, 900, 0, false, false}
  };
  return slots;
}

// 0 = 3-Timer Landing Page, 1..3 = Single-Timer Detail/Editor View for Slot 0..2
inline int &timer_view_slot() {
  static int idx = 0;
  return idx;
}

inline std::string format_timer_slot_label(int slot_idx) {
  const auto &s = timer_slots()[slot_idx];
  if (!s.name.empty()) return s.name;
  char buf[24];
  int mins = s.duration_sec / 60;
  int secs = s.duration_sec % 60;
  if (secs == 0) snprintf(buf, sizeof(buf), "%d Min Timer", mins);
  else snprintf(buf, sizeof(buf), "%dm %ds Timer", mins, secs);
  return std::string(buf);
}

inline std::string format_timer_header_upper(int slot_idx) {
  const auto &s = timer_slots()[slot_idx];
  if (!s.name.empty()) {
    std::string up = s.name;
    for (char &c : up) c = static_cast<char>(::toupper(static_cast<unsigned char>(c)));
    return up;
  }
  char buf[20];
  snprintf(buf, sizeof(buf), "TIMER %d", slot_idx + 1);
  return std::string(buf);
}

// Syncs the 3-slot pool with global summary variables (for power inhibition & HA sensors)
inline void sync_multi_timer_globals(bool &out_any_running, bool &out_any_paused,
                                     int &out_min_rem_sec, int &out_active_dur_sec,
                                     uint32_t &out_min_target_ms) {
  auto *slots = timer_slots();
  bool any_run = false;
  bool any_pau = false;
  int min_rem = 0;
  int act_dur = slots[0].duration_sec;
  uint32_t min_t = 0;

  for (int i = 0; i < 3; i++) {
    if (slots[i].running) {
      if (!any_run || slots[i].remaining_sec < min_rem) {
        min_rem = slots[i].remaining_sec;
        act_dur = slots[i].duration_sec;
        min_t   = slots[i].target_ms;
      }
      any_run = true;
    } else if (slots[i].paused && !any_run) {
      if (!any_pau) {
        min_rem = slots[i].remaining_sec;
        act_dur = slots[i].duration_sec;
      }
      any_pau = true;
    }
  }
  if (!any_run && !any_pau) {
    int v_idx = (timer_view_slot() >= 1 && timer_view_slot() <= 3) ? (timer_view_slot() - 1) : 0;
    min_rem = slots[v_idx].duration_sec;
    act_dur = slots[v_idx].duration_sec;
  }
  out_any_running    = any_run;
  out_any_paused     = any_pau;
  out_min_rem_sec    = min_rem;
  out_active_dur_sec = act_dur;
  out_min_target_ms  = min_t;
}

// Upserts a voice/HA timer into the 3-slot pool, rolling off the oldest idle preset if needed
inline int upsert_voice_timer_slot(const std::string &id_str, const std::string &name_str,
                                   int total_sec, int left_sec, bool is_act, uint32_t now_ms) {
  auto *slots = timer_slots();
  int target = -1;

  // 1. Match by existing UUID or same case-insensitive name
  for (int i = 0; i < 3; i++) {
    if (!id_str.empty() && slots[i].id == id_str) { target = i; break; }
    if (!name_str.empty() && !slots[i].name.empty()) {
      std::string a = slots[i].name, b = name_str;
      for (char &c : a) c = static_cast<char>(::tolower(static_cast<unsigned char>(c)));
      for (char &c : b) c = static_cast<char>(::tolower(static_cast<unsigned char>(c)));
      if (a == b) { target = i; break; }
    }
  }

  // 2. Otherwise insert into first non-running slot (rolling idle presets down so recent ones stay!)
  if (target < 0) {
    if (!slots[0].running && !slots[0].paused) {
      if (!slots[1].running && !slots[1].paused && !slots[2].running && !slots[2].paused) {
        slots[2] = slots[1];
        slots[1] = slots[0];
      }
      target = 0;
    } else if (!slots[1].running && !slots[1].paused) {
      if (!slots[2].running && !slots[2].paused) {
        slots[2] = slots[1];
      }
      target = 1;
    } else if (!slots[2].running && !slots[2].paused) {
      target = 2;
    } else {
      target = 2;
    }
  }

  if (total_sec <= 0) total_sec = std::max(60, left_sec);
  slots[target].id = id_str;
  if (!name_str.empty()) slots[target].name = name_str;
  slots[target].duration_sec  = total_sec;
  slots[target].remaining_sec = left_sec;
  slots[target].target_ms     = now_ms + static_cast<uint32_t>(left_sec) * 1000U;
  slots[target].running       = is_act;
  slots[target].paused        = !is_act;
  return target;
}

template<typename DisplayT, typename FontTimeT, typename FontDateT,
         typename FontBoldT, typename FontSmallT, typename FontChronoT>
inline void render_tool_timer(DisplayT &it, int cx, int cy,
                              bool /*timer_running*/, bool /*timer_paused*/,
                              int /*timer_remaining_sec*/, int /*timer_duration_sec*/,
                              FontTimeT *font_time, FontDateT *font_date,
                              FontBoldT *font_btn_bold, FontSmallT *font_small,
                              esphome::Color theme_primary, esphome::Color theme_secondary,
                              esphome::Color col_black, esphome::Color col_white,
                              esphome::Color col_slate, esphome::Color col_dark_gray,
                              esphome::Color col_red,
                              const std::string & /*va_timer_name*/,
                              FontChronoT *font_chrono_time,
                              esphome::Color col_gray) {
  auto *slots = timer_slots();
  int view_idx = timer_view_slot();

  if (view_idx == 0) {
    // -------------------------------------------------------------------------
    // TIMERS LANDING PAGE (3 Modern Rounded Timer Cards at y = 122, 210, 298)
    // -------------------------------------------------------------------------
    it.print(cx, 46, font_date, theme_primary, esphome::display::TextAlign::CENTER, "TIMERS");

    static constexpr int CARD_YS[3] = {122, 222, 322};
    for (int i = 0; i < 3; i++) {
      const auto &s = slots[i];
      int y_mid = CARD_YS[i];
      bool is_act = (s.running || s.paused);

      esphome::Color card_bg  = is_act ? esphome::Color(22, 28, 38) : esphome::Color(16, 20, 26);
      esphome::Color border_c = s.running ? theme_primary : (s.paused ? theme_secondary : esphome::Color(44, 52, 66));

      render_modern_card(it, cx, y_mid, 316, 74, 16, card_bg, border_c, nullptr, col_white, font_small);

      // Left vertical status accent stripe (4px wide)
      esphome::Color bar_c = s.running ? theme_primary : (s.paused ? theme_secondary : esphome::Color(52, 62, 76));
      it.filled_rectangle(86, y_mid - 21, 4, 42, bar_c);

      // Top Row: Bold MM:SS Time
      int disp_sec = is_act ? s.remaining_sec : s.duration_sec;
      char t_buf[16];
      snprintf(t_buf, sizeof(t_buf), "%02d:%02d", disp_sec / 60, disp_sec % 60);
      it.print(100, y_mid - 2, font_chrono_time, is_act ? col_white : col_slate,
               esphome::display::TextAlign::BASELINE_LEFT, t_buf);

      // Bottom Row: Timer Name + Status ("Chicken • Running", "Pasta • Saved", "5 Min Timer")
      std::string lbl = format_timer_slot_label(i);
      if (s.running) lbl += " • Running";
      else if (s.paused) lbl += " • Paused";
      else if (!s.name.empty()) lbl += " • Saved";
      lbl = fit_text_to_width_font(lbl, 186, font_small);
      esphome::Color sub_c = s.running ? theme_primary : (s.paused ? theme_secondary : col_gray);
      it.print(102, y_mid + 18, font_small, sub_c, esphome::display::TextAlign::CENTER_LEFT, lbl.c_str());

      // Subtle vertical separator at x = 298
      it.line(298, y_mid - 22, 298, y_mid + 22, esphome::Color(42, 50, 64));

      // Right Action Button: 1-Tap Play (▶) or Pause (||) Circle at (342, y_mid)
      int bx = 342;
      it.filled_circle(bx, y_mid, 18, is_act ? theme_primary : esphome::Color(30, 36, 48));
      it.circle(bx, y_mid, 18, is_act ? col_white : theme_secondary);
      if (s.running) {
        it.filled_rectangle(bx - 5, y_mid - 6, 3, 13, col_black);
        it.filled_rectangle(bx + 2, y_mid - 6, 3, 13, col_black);
      } else {
        esphome::Color tri_c = s.paused ? col_black : theme_primary;
        it.filled_triangle(bx - 4, y_mid - 7, bx - 4, y_mid + 7, bx + 7, y_mid, tri_c);
      }
    }

    render_back_button(it, cx, font_small, 426, "< BACK");
    return;
  }

  // ---------------------------------------------------------------------------
  // SINGLE-TIMER DETAIL & EDITOR VIEW (view_idx == 1..3)
  // ---------------------------------------------------------------------------
  int s_idx = (view_idx >= 1 && view_idx <= 3) ? (view_idx - 1) : 0;
  const auto &s = slots[s_idx];

  // 1. Continuous 5px Outer Bezel Progress Ring when running or paused
  if (s.running || s.paused) {
    float frac = (s.duration_sec > 0)
                     ? (static_cast<float>(s.remaining_sec) / static_cast<float>(s.duration_sec))
                     : 0.0f;
    render_progress_arc_r224(it, cx, cy, frac, s.paused ? theme_secondary : theme_primary);
  }

  // 2. Dynamic Uppercase Timer Title ("CHICKEN", "PASTA", or "TIMER 1") at y = 50
  std::string hdr = fit_text_to_width_font(format_timer_header_upper(s_idx), 230, font_date);
  it.print(cx, 50, font_date, theme_primary, esphome::display::TextAlign::CENTER, hdr.c_str());

  int disp_sec = (s.running || s.paused) ? s.remaining_sec : s.duration_sec;
  char t_str[16];
  snprintf(t_str, sizeof(t_str), "%02d:%02d", disp_sec / 60, disp_sec % 60);
  it.print(cx, 142, font_time, col_white, esphome::display::TextAlign::CENTER, t_str);

  if (s.running) {
    it.print(cx, 192, font_small, theme_secondary, esphome::display::TextAlign::CENTER, "Running");
  } else if (s.paused) {
    it.print(cx, 192, font_small, theme_primary, esphome::display::TextAlign::CENTER, "Paused");
  } else {
    it.print(cx, 192, font_small, col_slate, esphome::display::TextAlign::CENTER, s.name.empty() ? "Ready" : "Saved Preset");
  }

  // 3. Hero START / PAUSE / RESUME Pill at y = 248
  const char *hero_lbl = (!s.running && !s.paused) ? "START" : (s.running ? "PAUSE" : "RESUME");
  render_pill_button(it, 143, 323, 248, 22, theme_secondary, theme_secondary,
                     hero_lbl, col_black, font_btn_bold);

  // 4. Modern Stepper Cards (-5m, -1m, +1m, +5m) or RESET Card at y = 324
  esphome::Color card_bg(22, 27, 36);
  esphome::Color card_brd(52, 62, 78);
  if (!s.running && !s.paused) {
    static constexpr int STEP_XS[4] = {89, 185, 281, 377};
    static constexpr const char *STEP_LBLS[4] = {"-5m", "-1m", "+1m", "+5m"};
    for (int i = 0; i < 4; i++) {
      render_modern_card(it, STEP_XS[i], 324, 68, 46, 12, card_bg, card_brd,
                         STEP_LBLS[i], col_white, font_small);
    }
  } else {
    render_modern_card(it, cx, 324, 136, 46, 12, esphome::Color(28, 20, 24), col_red,
                       "RESET", col_red, font_small);
  }

  render_back_button(it, cx, font_small, 426, "< BACK");
}

template<typename DisplayT, typename TimeT, typename FontTimeT, typename FontChronoT,
         typename FontDateT, typename FontSmallT>
inline void render_tool_alarms(DisplayT &it, int cx, const TimeT &now, bool is_24h_format,
                               int alarm_edit_idx, uint32_t alarm_snooze_until,
                               int alarm_ringing_idx,
                               int alarm1_hour, int alarm1_minute, bool alarm1_enabled, int alarm1_recurrence,
                               int alarm2_hour, int alarm2_minute, bool alarm2_enabled, int alarm2_recurrence,
                               int alarm3_hour, int alarm3_minute, bool alarm3_enabled, int alarm3_recurrence,
                               FontTimeT *font_time, FontChronoT *font_chrono_time,
                               FontDateT *font_date, FontSmallT *font_small,
                               esphome::Color theme_primary, esphome::Color theme_secondary,
                               esphome::Color col_black, esphome::Color col_white,
                               esphome::Color col_gray, esphome::Color col_dark_gray,
                               esphome::Color col_gold) {
  (void)now; (void)col_gold;
  if (alarm_edit_idx == 0) {
    // -------------------------------------------------------------------------
    // ALARMS LANDING PAGE (3 Modern Rounded Alarm Cards at y = 122, 210, 298)
    // -------------------------------------------------------------------------
    it.print(cx, 46, font_date, theme_primary, esphome::display::TextAlign::CENTER, "ALARMS");

    static constexpr const char *REC_LABELS[4] = {"Daily", "Weekdays", "Weekends", "Once"};

    auto draw_modern_alarm_card = [&](int y_mid, int h, int m, bool en, int rec, int idx) {
      bool is_snoozed = (alarm_snooze_until > 0 && (alarm_ringing_idx == idx || alarm_ringing_idx == 0));

      esphome::Color card_bg  = en ? esphome::Color(22, 28, 38) : esphome::Color(14, 17, 22);
      esphome::Color border_c = en ? esphome::Color(58, 70, 88) : esphome::Color(34, 40, 50);

      render_modern_card(it, cx, y_mid, 316, 74, 16, card_bg, border_c, nullptr, col_white, font_small);

      if (is_snoozed) {
        render_pill_button(it, 114, 352, y_mid, 18, theme_secondary, theme_secondary,
                           "DISMISS SNOOZE", col_black, font_small);
        return;
      }

      // Left vertical status accent stripe (4px wide)
      esphome::Color bar_c = en ? theme_primary : esphome::Color(38, 44, 54);
      it.filled_rectangle(86, y_mid - 21, 4, 42, bar_c);

      // Top Row: Bold Time + AM/PM
      char t_buf[16];
      char ap_buf[8] = "";
      format_time_hhmm(h, m, is_24h_format, t_buf, sizeof(t_buf), ap_buf, true);
      esphome::Color time_col = en ? col_white : col_gray;
      it.print(100, y_mid - 2, font_chrono_time, time_col,
               esphome::display::TextAlign::BASELINE_LEFT, t_buf);
      if (!is_24h_format && ap_buf[0] != '\0') {
        int h12 = h % 12;
        if (h12 == 0) h12 = 12;
        int ap_x = (h12 >= 10) ? 214 : 190;
        it.print(ap_x, y_mid - 2, font_small, en ? theme_secondary : col_gray,
                 esphome::display::TextAlign::BASELINE_LEFT, ap_buf);
      }

      // Bottom Row: Clean Schedule Subtitle
      it.print(102, y_mid + 18, font_small, en ? theme_primary : col_gray,
               esphome::display::TextAlign::CENTER_LEFT, REC_LABELS[rec & 3]);

      // Subtle vertical separator between Edit Zone (left) and Toggle Switch (right)
      it.line(298, y_mid - 22, 298, y_mid + 22, border_c);

      // Centered Modern Toggle Switch (x = 330..356, y = y_mid)
      esphome::Color sw_bg = en ? theme_primary : esphome::Color(42, 48, 58);
      it.filled_circle(330, y_mid, 12, sw_bg);
      it.filled_circle(356, y_mid, 12, sw_bg);
      it.filled_rectangle(330, y_mid - 12, 26, 25, sw_bg);
      int knob_x = en ? 356 : 330;
      it.filled_circle(knob_x, y_mid, 9, en ? col_white : esphome::Color(180, 186, 196));
    };

    draw_modern_alarm_card(122, alarm1_hour, alarm1_minute, alarm1_enabled, alarm1_recurrence, 1);
    draw_modern_alarm_card(222, alarm2_hour, alarm2_minute, alarm2_enabled, alarm2_recurrence, 2);
    draw_modern_alarm_card(322, alarm3_hour, alarm3_minute, alarm3_enabled, alarm3_recurrence, 3);
  } else {
    it.print(cx, 50, font_date, theme_primary, esphome::display::TextAlign::CENTER, "EDIT ALARM");

    int cur_h = (alarm_edit_idx == 3) ? alarm3_hour : ((alarm_edit_idx == 2) ? alarm2_hour : alarm1_hour);
    int cur_m = (alarm_edit_idx == 3) ? alarm3_minute : ((alarm_edit_idx == 2) ? alarm2_minute : alarm1_minute);
    int cur_rec = (alarm_edit_idx == 3) ? alarm3_recurrence : ((alarm_edit_idx == 2) ? alarm2_recurrence : alarm1_recurrence);

    char t_buf[16];
    char ap_buf[8] = "";
    format_time_hhmm(cur_h, cur_m, is_24h_format, t_buf, sizeof(t_buf), ap_buf);
    it.print(cx, 142, font_time, col_white, esphome::display::TextAlign::CENTER, t_buf);
    if (!is_24h_format) {
      int h12 = cur_h % 12;
      if (h12 == 0) h12 = 12;
      int ap_x = (h12 >= 10) ? (cx + 107) : (cx + 84);
      it.print(ap_x, 151, font_small, theme_secondary, esphome::display::TextAlign::LEFT, ap_buf);
    }

    esphome::Color card_bg(22, 27, 36);
    esphome::Color card_brd(52, 62, 78);
    static constexpr int STEP_XS[4] = {89, 185, 281, 377};
    static constexpr const char *STEP_LBLS[4] = {"-1h", "+1h", "-5m", "+5m"};
    for (int i = 0; i < 4; i++) {
      render_modern_card(it, STEP_XS[i], 236, 68, 46, 12, card_bg, card_brd,
                         STEP_LBLS[i], col_white, font_small);
    }

    static constexpr const char *REC_NAMES[4] = {
        "Repeat: Daily", "Repeat: Weekdays", "Repeat: Weekends", "Repeat: Once"};
    render_modern_card(it, cx, 316, 264, 46, 12, card_bg, card_brd,
                       REC_NAMES[cur_rec & 3], col_white, font_small);
    it.line(118, 316, 124, 310, theme_secondary);
    it.line(118, 316, 124, 322, theme_secondary);
    it.line(348, 316, 342, 310, theme_secondary);
    it.line(348, 316, 342, 322, theme_secondary);
  }

  render_back_button(it, cx, font_small, 426, "< BACK");
}
template<typename DisplayT, typename FontDateT, typename FontBoldT, typename FontSmallT>
inline void render_tool_calculator(DisplayT &it, int cx, const std::string &calc_history,
                                   const std::string &calc_entry, int calc_op,
                                   bool calc_new_entry,
                                   FontDateT *font_date, FontBoldT *font_btn_bold,
                                   FontSmallT *font_small,
                                   esphome::Color theme_primary, esphome::Color col_white,
                                   esphome::Color col_slate, esphome::Color col_dark_gray) {
  // Bezel-safe right margins (x = 348 at y = 54, x = 364 at y = 86) so text never clips on the round screen
  if (!calc_history.empty()) {
    it.print(348, 54, font_small, col_slate, esphome::display::TextAlign::RIGHT, calc_history.c_str());
  }
  if (!calc_entry.empty()) {
    it.print(364, 86, font_date, col_white, esphome::display::TextAlign::RIGHT, calc_entry.c_str());
  }

  it.line(80, 112, 386, 112, esphome::Color(50, 58, 72));

  static constexpr int ROWS_Y[5] = {138, 192, 246, 300, 354};
  static constexpr int COLS_X[4] = {119, 195, 271, 347};
  static constexpr const char *KEY_LABELS[5][4] = {
      {"C", "±", "%", "/"},
      {"7", "8", "9", "x"},
      {"4", "5", "6", "-"},
      {"1", "2", "3", "+"},
      {"0", ".", "DEL", "="}};

  auto draw_calc_key = [&](int kx, int ky, const char *label,
                           esphome::Color bg_c, esphome::Color text_c, esphome::Color border_c) {
    const int kw = 68;
    const int kh = 44;
    const int kr = 8;
    int x0 = kx - kw / 2;
    int y0 = ky - kh / 2;
    it.filled_rectangle(x0 + kr, y0, kw - 2 * kr, kh, bg_c);
    it.filled_rectangle(x0, y0 + kr, kw, kh - 2 * kr, bg_c);
    it.filled_circle(x0 + kr, y0 + kr, kr, bg_c);
    it.filled_circle(x0 + kw - kr, y0 + kr, kr, bg_c);
    it.filled_circle(x0 + kr, y0 + kh - kr, kr, bg_c);
    it.filled_circle(x0 + kw - kr, y0 + kh - kr, kr, bg_c);

    it.line(x0 + kr, y0, x0 + kw - kr, y0, border_c);
    it.line(x0 + kr, y0 + kh - 1, x0 + kw - kr, y0 + kh - 1, border_c);
    it.line(x0, y0 + kr, x0, y0 + kh - kr, border_c);
    it.line(x0 + kw - 1, y0 + kr, x0 + kw - 1, y0 + kh - kr, border_c);

    it.print(kx, ky, font_btn_bold, text_c, esphome::display::TextAlign::CENTER, label);
  };

  for (int r = 0; r < 5; r++) {
    for (int c = 0; c < 4; c++) {
      esphome::Color bg_c;
      esphome::Color text_c = col_white;
      esphome::Color border_c;

      if (c == 3) {
        // Highlight active pending operator (+, -, x, /) with inverted white pill & amber text
        bool op_selected = calc_new_entry &&
                           ((r == 0 && calc_op == 4) || (r == 1 && calc_op == 3) ||
                            (r == 2 && calc_op == 2) || (r == 3 && calc_op == 1));
        if (op_selected) {
          bg_c = col_white;
          border_c = esphome::Color(255, 180, 80);
          text_c = esphome::Color(230, 115, 0);
        } else {
          bg_c = esphome::Color(230, 115, 0);
          border_c = esphome::Color(255, 145, 30);
        }
      } else if (r == 0) {
        bg_c = esphome::Color(65, 75, 90);
        border_c = esphome::Color(90, 105, 125);
      } else if (r == 4 && c == 2) {
        bg_c = esphome::Color(60, 50, 60);
        border_c = esphome::Color(85, 70, 85);
      } else {
        bg_c = esphome::Color(35, 40, 52);
        border_c = esphome::Color(55, 62, 78);
      }

      draw_calc_key(COLS_X[c], ROWS_Y[r], KEY_LABELS[r][c], bg_c, text_c, border_c);
    }
  }

  render_back_button(it, cx, font_small, 426, "< BACK");
}


// ============================================================================
// SECTION 15: MUSIC PLAYER ENGINE — NOW PLAYING, TRANSPORT HUD & LIBRARY MENUS
// ============================================================================

inline int &remote_vol_pct() {
  static int pct = 70;
  return pct;
}

inline int step_remote_vol_pct(int delta_pct) {
  int next = std::max(0, std::min(100, remote_vol_pct() + delta_pct));
  remote_vol_pct() = next;
  return next;
}

inline uint32_t &remote_play_override_ms() {
  static uint32_t ms = 0;
  return ms;
}

inline bool &remote_play_override_state() {
  static bool st = false;
  return st;
}

inline void set_remote_play_override(bool playing, uint32_t now_ms) {
  remote_play_override_state() = playing;
  remote_play_override_ms() = now_ms;
}

inline bool is_music_playing_effective(bool is_remote, bool local_playing,
                                       const std::string &ha_state, uint32_t now_ms) {
  if (!is_remote) {
    return local_playing;
  }
  if (remote_play_override_ms() > 0 && (now_ms - remote_play_override_ms() < 2500U)) {
    return remote_play_override_state();
  }
  return (ha_state == "playing");
}

template<typename DisplayT, typename FontDateT>
inline void render_menu_header(DisplayT &it, int cx, int y_pos, const std::string &title,
                               FontDateT *font_date, esphome::Color theme_primary) {
  if (title.empty()) return;
  int w = 0, x_off = 0, base = 0, text_h = 0;
  font_date->measure(title.c_str(), &w, &x_off, &base, &text_h);
  if (w <= 220) {
    it.print(cx, y_pos, font_date, theme_primary, esphome::display::TextAlign::CENTER, title.c_str());
  } else {
    std::string disp_title = fit_text_to_width_font(title, 246, font_date);
    it.print(110, y_pos, font_date, theme_primary, esphome::display::TextAlign::CENTER_LEFT, disp_title.c_str());
  }
}

template<typename DisplayT, typename LocalImageT, typename RemoteImageT,
         typename FontSmallT, typename FontMicroT>
inline void render_music_now_playing(
    DisplayT &it, int cx, int cy, uint32_t now_ms,
    const char *art_mode_opt, const char *overlay_mode_opt, bool music_hud_active,
    bool is_remote, bool is_playing,
    LocalImageT *local_art, RemoteImageT *remote_art,
    const std::string &ha_target_art_url, const std::string &current_loaded_art_url,
    const std::string &active_speaker_name, const char *friendly_name,
    const std::string &music_action_feedback, uint32_t music_feedback_timer,
    const std::string &active_playback_media_type,
    bool show_spk_shortcut, bool show_lib_shortcut,
    float dur, float music_pos_base, uint32_t music_pos_sync_time,
    const std::string &local_title, const std::string &local_artist, const std::string &local_album,
    const std::string &ha_title, const std::string &ha_artist, const std::string &ha_album,
    FontSmallT *font_small, FontMicroT *font_micro,
    esphome::Color theme_primary, esphome::Color theme_secondary,
    esphome::Color col_bg, esphome::Color col_black, esphome::Color col_white,
    esphome::Color col_gray, esphome::Color col_dark_gray) {
  std::string art_mode = (art_mode_opt && art_mode_opt[0]) ? art_mode_opt : "Full Screen";
  bool is_standard = (art_mode == "Standard");
  bool is_letterbox = (art_mode == "Letterbox");

  std::string overlay_mode = (overlay_mode_opt && overlay_mode_opt[0]) ? overlay_mode_opt : "Art Only";
  bool always_visible = (overlay_mode == "Always Visible");
  bool show_meta_always = always_visible || (overlay_mode == "Art + Metadata");
  bool show_controls = always_visible || music_hud_active;
  bool show_metadata = show_meta_always || music_hud_active;

  const int art_center_y = 233;
  bool apply_scrim = always_visible || music_hud_active;
  bool has_art = false;
  bool rendered_art = false;
  if (is_remote) {
    if (!ha_target_art_url.empty() && ha_target_art_url != "none" &&
        current_loaded_art_url == ha_target_art_url && remote_art && remote_art->get_width() > 0) {
      has_art = true;
      rendered_art = render_album_art_canvas(
          it, remote_art, cx, is_standard, is_letterbox, apply_scrim, show_metadata);
    }
  } else {
    if (local_art && local_art->get_width() > 0) {
      has_art = true;
      rendered_art = render_album_art_canvas(
          it, local_art, cx, is_standard, is_letterbox, apply_scrim, show_metadata);
    }
  }

  if (!rendered_art) {
    it.filled_circle(cx, art_center_y, 80, col_dark_gray);
    it.circle(cx, art_center_y, 80, is_remote ? theme_secondary : theme_primary);
    it.circle(cx, art_center_y, 64, col_bg);
    it.circle(cx, art_center_y, 48, col_dark_gray);
    it.circle(cx, art_center_y, 32, col_bg);
    it.filled_circle(cx, art_center_y, 16, is_remote ? theme_secondary : theme_primary);
  }

  std::string spk_title = active_speaker_name;
  if (spk_title.empty() || spk_title == "None") spk_title = friendly_name;
  bool is_feedback = (now_ms - music_feedback_timer < 1800U && !music_action_feedback.empty());
  if (is_feedback) {
    spk_title = music_action_feedback;
  } else if (now_ms < 4000U) {
    spk_title = "CONNECTING...";
  }
  for (char &c : spk_title) {
    c = (char)toupper((unsigned char)c);
  }
  spk_title = fit_text_to_width_font(spk_title, 280, font_small);

  if (show_controls || music_hud_active) {
    if (!is_standard) {
      it.print(cx + 1, 51, font_small, col_black, esphome::display::TextAlign::CENTER, spk_title.c_str());
    }
    it.print(cx, 50, font_small, col_white, esphome::display::TextAlign::CENTER, spk_title.c_str());
  }

  if (show_controls) {
    const int prev_btn_x = 124;
    const int next_btn_x = 342;
    const int left_flank_x = 71;
    const int right_flank_x = 395;

    bool is_speech_media = (active_playback_media_type == "podcast" ||
                            active_playback_media_type == "podcast_episode" ||
                            active_playback_media_type == "audiobook");

    // Zero-trig LUT circular skip glyph (replaces 256 sincosf calls with .rodata DIAL_SIN/DIAL_COS)
    auto draw_skip_glyph_lut = [&](int bx, int by, int dir_sign, const char *lbl, esphome::Color c) {
      if (dir_sign < 0) {
        it.filled_triangle(bx - 10, by - 15, bx - 1, by - 21, bx - 1, by - 9, c);
      } else {
        it.filled_triangle(bx + 10, by - 15, bx + 1, by - 21, bx + 1, by - 9, c);
      }
      for (int idx = 3; idx < 50; idx += 2) {
        int next_idx = std::min(50, idx + 2);
        float s1 = DIAL_SIN[idx] * (float)(-dir_sign);
        float c1 = DIAL_COS[idx];
        float s2 = DIAL_SIN[next_idx] * (float)(-dir_sign);
        float c2 = DIAL_COS[next_idx];
        it.line(bx + (int)(15.0f * s1), by - (int)(15.0f * c1),
                bx + (int)(15.0f * s2), by - (int)(15.0f * c2), c);
        it.line(bx + (int)(16.0f * s1), by - (int)(16.0f * c1),
                bx + (int)(16.0f * s2), by - (int)(16.0f * c2), c);
      }
      it.print(bx, by + 1, font_micro, c, esphome::display::TextAlign::CENTER, lbl);
      it.print(bx + 1, by + 1, font_micro, c, esphome::display::TextAlign::CENTER, lbl);
    };

    auto draw_prev_glyph = [&](int ox, int oy, esphome::Color c) {
      int px = prev_btn_x + ox;
      int py = art_center_y + oy;
      it.filled_rectangle(px - 14, py - 10, 3, 21, c);
      it.filled_triangle(px - 8, py, px + 6, py - 10, px + 6, py + 10, c);
    };

    auto draw_next_glyph = [&](int ox, int oy, esphome::Color c) {
      int nx = next_btn_x + ox;
      int ny = art_center_y + oy;
      it.filled_triangle(nx + 8, ny, nx - 6, ny - 10, nx - 6, ny + 10, c);
      it.filled_rectangle(nx + 11, ny - 10, 3, 21, c);
    };

    if (is_speech_media) {
      draw_skip_glyph_lut(prev_btn_x + 1, art_center_y + 1, -1, "10", col_black);
      draw_skip_glyph_lut(prev_btn_x, art_center_y, -1, "10", col_white);
      draw_skip_glyph_lut(next_btn_x + 1, art_center_y + 1, 1, "30", col_black);
      draw_skip_glyph_lut(next_btn_x, art_center_y, 1, "30", col_white);
    } else {
      draw_prev_glyph(1, 1, col_black);
      draw_prev_glyph(0, 0, col_white);
      draw_next_glyph(1, 1, col_black);
      draw_next_glyph(0, 0, col_white);
    }

    // Right Flank Volume Up (+) and Volume Down (-)
    it.filled_rectangle(right_flank_x - 6, 140, 15, 3, col_black);
    it.filled_rectangle(right_flank_x, 134, 3, 15, col_black);
    it.filled_rectangle(right_flank_x - 7, 139, 15, 3, col_white);
    it.filled_rectangle(right_flank_x - 1, 133, 3, 15, col_white);

    it.filled_rectangle(right_flank_x - 6, 326, 15, 3, col_black);
    it.filled_rectangle(right_flank_x - 7, 325, 15, 3, col_white);

    // Optional Left Flank Shortcuts
    if (show_spk_shortcut) {
      auto draw_spk = [&](int ox, int oy, esphome::Color c) {
        int sx = left_flank_x + ox;
        int sy = 140 + oy;
        it.filled_triangle(sx - 2, sy - 3, sx - 2, sy + 3, sx + 3, sy + 7, c);
        it.filled_triangle(sx - 2, sy - 3, sx + 3, sy + 7, sx + 3, sy - 7, c);
        it.filled_rectangle(sx - 6, sy - 3, 5, 7, c);
        it.line(sx + 5, sy - 3, sx + 7, sy - 1, c);
        it.line(sx + 7, sy - 1, sx + 7, sy + 1, c);
        it.line(sx + 7, sy + 1, sx + 5, sy + 3, c);
        it.line(sx + 8, sy - 6, sx + 11, sy - 3, c);
        it.line(sx + 11, sy - 3, sx + 11, sy + 3, c);
        it.line(sx + 11, sy + 3, sx + 8, sy + 6, c);
      };
      draw_spk(1, 1, col_black);
      draw_spk(0, 0, col_white);
    }

    if (show_lib_shortcut) {
      auto draw_lib = [&](int ox, int oy, esphome::Color c) {
        int lx = left_flank_x + ox;
        int ly = 326 + oy;
        it.filled_rectangle(lx - 8, ly - 7, 4, 15, c);
        it.filled_rectangle(lx - 2, ly - 7, 4, 15, c);
        it.line(lx + 4, ly + 7, lx + 8, ly - 7, c);
        it.line(lx + 5, ly + 7, lx + 9, ly - 7, c);
        it.line(lx + 7, ly + 7, lx + 11, ly - 6, c);
        it.line(lx + 8, ly + 7, lx + 12, ly - 6, c);
      };
      draw_lib(1, 1, col_black);
      draw_lib(0, 0, col_white);
    }

    // Center Hero Play/Pause Capsule (r = 34)
    it.filled_circle(cx, art_center_y, 34, col_dark_gray);
    it.circle(cx, art_center_y, 34, col_black);
    it.circle(cx, art_center_y, 33, col_gray);

    if (!is_playing) {
      it.filled_triangle(cx - 7, art_center_y - 15, cx - 7, art_center_y + 15, cx + 15, art_center_y, theme_primary);
    } else {
      it.filled_rectangle(cx - 9, art_center_y - 13, 6, 26, theme_primary);
      it.filled_rectangle(cx + 3, art_center_y - 13, 6, 26, theme_primary);
    }
  }

  if (show_metadata) {
    float current_pos = music_pos_base;
    if (is_playing && music_pos_sync_time > 0) {
      current_pos += (float)(now_ms - music_pos_sync_time) / 1000.0f;
    }
    float prog = 0.0f;
    if (!std::isnan(dur) && dur > 0.0f) {
      prog = std::max(0.0f, std::min(1.0f, current_pos / dur));
    }

    render_progress_arc_r224(it, cx, cy, prog, theme_secondary);

    std::string title = is_remote ? ha_title : ((local_title.empty() || local_title == "None") ? ha_title : local_title);
    std::string artist = is_remote ? ha_artist : ((local_artist.empty() || local_artist == "None") ? ha_artist : local_artist);
    std::string album = is_remote ? ha_album : ((local_album.empty() || local_album == "None") ? ha_album : local_album);

    if (title.empty() || title == "None") title = is_playing ? "Playing Audio" : "Idle";
    title = fit_text_to_width_font(title, 280, font_small);

    bool has_shadow = has_art;
    if (has_shadow) {
      it.print(cx + 1, 353, font_small, col_black, esphome::display::TextAlign::CENTER, title.c_str());
    }
    it.print(cx, 352, font_small, col_white, esphome::display::TextAlign::CENTER, title.c_str());

    if (!artist.empty() && artist != "None") {
      artist = fit_text_to_width_font(artist, 260, font_small);
      if (has_shadow) {
        it.print(cx + 1, 374, font_small, col_black, esphome::display::TextAlign::CENTER, artist.c_str());
      }
      it.print(cx, 373, font_small, col_white, esphome::display::TextAlign::CENTER, artist.c_str());
    }
    if (!album.empty() && album != "None") {
      album = fit_text_to_width_font(album, 240, font_small);
      if (has_shadow) {
        it.print(cx + 1, 395, font_small, col_black, esphome::display::TextAlign::CENTER, album.c_str());
      }
      it.print(cx, 394, font_small, col_white, esphome::display::TextAlign::CENTER, album.c_str());
    }
  }
}

template<typename DisplayT, typename FontDateT, typename FontSmallT, typename FontMicroT>
inline void render_music_menus(
    DisplayT &it, int cx, int music_overlay_mode,
    const std::string &active_speaker_entity, const std::string &active_speaker_name,
    const char *local_player_id,
    const std::string &selected_category_mode, const std::string &selected_category_title,
    int music_category_page, int music_browse_page, int music_browse_total_pages,
    bool menu_scroll_active, int menu_scroll_step, int menu_scroll_dir, int menu_scroll_target_page,
    const std::string &slot_1_name, const std::string &slot_1_id, const std::string &slot_1_type, bool slot_1_active,
    const std::string &slot_2_name, const std::string &slot_2_id, const std::string &slot_2_type, bool slot_2_active,
    const std::string &slot_3_name, const std::string &slot_3_id, const std::string &slot_3_type, bool slot_3_active,
    const std::string &slot_4_name, const std::string &slot_4_id, const std::string &slot_4_type, bool slot_4_active,
    const std::string &slot_5_name, const std::string &slot_5_id, const std::string &slot_5_type, bool slot_5_active,
    const std::string &target_speaker_id, const std::string &target_speaker_name,
    FontDateT *font_date, FontSmallT *font_small, FontMicroT *font_micro,
    esphome::Color theme_primary, esphome::Color theme_secondary,
    esphome::Color col_bg, esphome::Color col_black, esphome::Color col_white,
    esphome::Color col_slate, esphome::Color col_dark_gray,
    uint32_t now_ms = 0, bool is_playing = false, float dur = 0.0f,
    float music_pos_base = 0.0f, uint32_t music_pos_sync_time = 0,
    const std::string &cur_title = "", const std::string &cur_artist = "",
    const std::string &cur_album = "") {
  auto draw_right_chevrons = [&](bool show_up, bool show_down) {
    if (!show_up && !show_down) return;
    it.filled_rectangle(395, 140, 71, 160, col_bg);
    if (show_up) render_scroll_chevron(it, 415, 162, true, theme_secondary, col_black, false);
    if (show_down) render_scroll_chevron(it, 415, 278, false, theme_secondary, col_black, false);
  };

  if (music_overlay_mode == 5) {
    // SUBPAGE 5: MUSIC MENU (SPEAKERS, LIBRARY, SETTINGS)
    it.print(cx, 46, font_date, theme_primary, esphome::display::TextAlign::CENTER, "MUSIC");
    std::string spk_sub = fit_text_to_width_font(active_speaker_name, 260, font_small);
    it.print(cx, 78, font_small, col_white, esphome::display::TextAlign::CENTER, spk_sub.c_str());

    render_pill_button(it, 150, 316, 140, 18, theme_primary, col_dark_gray, "Speakers", col_white, font_small);
    render_pill_button(it, 150, 316, 200, 18, theme_primary, col_dark_gray, "Library", col_white, font_small);
    render_pill_button(it, 150, 316, 290, 18, theme_secondary, col_dark_gray, "Settings", col_white, font_small);

  } else if (music_overlay_mode == 1) {
    // LEVEL 1: SPEAKERS BROWSER (5-Slot Speaker Inventory)
    render_menu_header(it, cx, 50, "SPEAKERS", font_date, theme_primary);

    auto draw_speaker_card = [&](int y_mid, const std::string &name, const std::string &id_str, bool is_act) {
      if (name.empty()) return;
      bool is_current = (is_act || id_str == active_speaker_entity);
      esphome::Color border_c = is_current ? theme_primary : theme_secondary;
      esphome::Color txt_c = is_current ? theme_primary : col_white;

      render_pill_button(it, 110, 356, y_mid, 18, border_c, col_dark_gray, "", txt_c, font_small);
      if (is_current) {
        it.filled_circle(114, y_mid, 6, theme_primary);
      } else {
        it.circle(114, y_mid, 5, col_slate);
      }
      std::string disp_name = fit_text_to_width_font(name, 218, font_small);
      it.print(130, y_mid, font_small, txt_c, esphome::display::TextAlign::CENTER_LEFT, disp_name.c_str());
    };

    draw_speaker_card(104, slot_1_name, slot_1_id, slot_1_active);
    draw_speaker_card(162, slot_2_name, slot_2_id, slot_2_active);
    draw_speaker_card(220, slot_3_name, slot_3_id, slot_3_active);
    draw_speaker_card(278, slot_4_name, slot_4_id, slot_4_active);
    draw_speaker_card(336, slot_5_name, slot_5_id, slot_5_active);

    if (music_browse_total_pages > 1) {
      draw_right_chevrons(true, true);
    }

  } else if (music_overlay_mode == 2) {
    // LEVEL 2: LIBRARY CATEGORIES SUBMENU (Continuous 7-Item Eased Scroll)
    int current_offset = 0;
    if (menu_scroll_active) {
      int cur_step = std::max(0, std::min(4, menu_scroll_step));
      int shift = SHARED_SCROLL_EASE_116[cur_step];
      current_offset = (menu_scroll_dir == 1) ? shift : (116 - shift);
    } else {
      current_offset = (music_category_page == 2) ? 116 : 0;
    }

    static constexpr const char *CAT_LABELS[7] = {
        "FAVORITES", "PLAYLISTS", "ARTISTS", "ALBUMS", "PODCASTS", "RADIO", "AUDIOBOOKS"};
    for (int i = 0; i < 7; i++) {
      int y = 104 + i * 58 - current_offset;
      if (y >= 50 && y <= 390) {
        render_pill_button(it, 150, 316, y, 18, theme_secondary, col_dark_gray,
                           CAT_LABELS[i], col_white, font_small);
      }
    }

    it.filled_rectangle(0, 0, 466, 75, col_bg);
    it.filled_rectangle(0, 370, 466, 96, col_bg);
    render_menu_header(it, cx, 50, "LIBRARY", font_date, theme_primary);
    int cur_dot_page = menu_scroll_active ? menu_scroll_target_page : music_category_page;

    draw_right_chevrons(cur_dot_page == 2, cur_dot_page == 1);

  } else if (music_overlay_mode == 3) {
    // LEVEL 3: ITEM BROWSER (Dual Inline Play/Browse Cards - 5 Slots)
    bool is_podcast_list = (selected_category_mode == "podcast");
    bool is_drill = (selected_category_mode == "artist" || selected_category_mode == "album" ||
                     selected_category_mode == "artist_albums");

    render_menu_header(it, cx, 50, selected_category_title, font_date, theme_primary);

    auto draw_drilldown_icon = [&](int x_pos, int y_mid) {
      for (int dy : {-4, 0, 4}) it.filled_rectangle(x_pos, y_mid + dy, 13, 2, theme_secondary);
    };

    auto draw_slot_card = [&](int y_mid, const std::string &name, const std::string &mtype, bool is_act) {
      if (name.empty()) return;
      if (is_drill) {
        it.filled_circle(82, y_mid, 18, col_dark_gray);
        it.circle(82, y_mid, 18, theme_primary);
        it.filled_triangle(79, y_mid - 6, 79, y_mid + 6, 88, y_mid, theme_primary);

        it.filled_circle(138, y_mid, 18, theme_secondary);
        it.filled_circle(357, y_mid, 18, theme_secondary);
        it.filled_rectangle(138, y_mid - 18, 219, 37, theme_secondary);

        it.filled_circle(138, y_mid, 16, col_dark_gray);
        it.filled_circle(357, y_mid, 16, col_dark_gray);
        it.filled_rectangle(138, y_mid - 16, 219, 33, col_dark_gray);

        std::string disp_name = fit_text_to_width_font(name, 196, font_small);
        it.print(144, y_mid, font_small, col_white, esphome::display::TextAlign::CENTER_LEFT, disp_name.c_str());
        draw_drilldown_icon(350, y_mid);
      } else if (is_podcast_list) {
        render_pill_button(it, 110, 356, y_mid, 18, theme_secondary, col_dark_gray, "", col_white, font_small);
        std::string disp_name = fit_text_to_width_font(name, 196, font_small);
        it.print(126, y_mid, font_small, col_white, esphome::display::TextAlign::CENTER_LEFT, disp_name.c_str());
        draw_drilldown_icon(336, y_mid);
      } else if (mtype == "text") {
        std::string disp_name = fit_text_to_width_font(name, 236, font_small);
        render_pill_button(it, 110, 356, y_mid, 18, col_dark_gray, col_dark_gray, disp_name.c_str(), col_slate, font_small);
      } else {
        esphome::Color border_c = is_act ? theme_primary : theme_secondary;
        esphome::Color txt_c = is_act ? theme_primary : col_white;
        render_pill_button(it, 110, 356, y_mid, 18, border_c, col_dark_gray, "", txt_c, font_small);

        esphome::Color icon_c = (mtype == "radio") ? theme_secondary : theme_primary;
        it.filled_triangle(108, y_mid - 6, 108, y_mid + 6, 118, y_mid, icon_c);

        std::string disp_name = fit_text_to_width_font(name, 218, font_small);
        it.print(130, y_mid, font_small, txt_c, esphome::display::TextAlign::CENTER_LEFT, disp_name.c_str());
      }
    };

    draw_slot_card(104, slot_1_name, slot_1_type, slot_1_active);
    draw_slot_card(162, slot_2_name, slot_2_type, slot_2_active);
    draw_slot_card(220, slot_3_name, slot_3_type, slot_3_active);
    draw_slot_card(278, slot_4_name, slot_4_type, slot_4_active);
    draw_slot_card(336, slot_5_name, slot_5_type, slot_5_active);

    if (music_browse_total_pages > 1) {
      draw_right_chevrons(music_browse_page > 1, music_browse_page < music_browse_total_pages);
    }

  } else if (music_overlay_mode == 4) {
    // LEVEL 4: SPEAKER ACTION SHEET
    std::string spk_title = target_speaker_name.empty() ? "SPEAKER" : target_speaker_name;
    render_menu_header(it, cx, 50, spk_title, font_date, theme_primary);

    bool is_current = (target_speaker_id == active_speaker_entity);
    if (is_current) {
      it.print(cx, 76, font_small, theme_secondary, esphome::display::TextAlign::CENTER, "CURRENTLY CONTROLLING");
      bool is_remote_target = (target_speaker_id != local_player_id);
      if (is_remote_target) {
        render_pill_button(it, 110, 356, 142, 18, theme_secondary, col_dark_gray, "TRANSFER TO LOCAL", col_white, font_small);
        render_pill_button(it, 110, 356, 200, 18, theme_secondary, col_dark_gray, "UNJOIN AUDIO", col_white, font_small);
        render_pill_button(it, 110, 356, 258, 18, theme_secondary, col_dark_gray, "STOP PLAYBACK", col_white, font_small);
      } else {
        render_pill_button(it, 110, 356, 160, 18, theme_secondary, col_dark_gray, "UNJOIN AUDIO", col_white, font_small);
        render_pill_button(it, 110, 356, 230, 18, theme_secondary, col_dark_gray, "STOP PLAYBACK", col_white, font_small);
      }
    } else {
      it.print(cx, 76, font_small, col_slate, esphome::display::TextAlign::CENTER, "SPEAKER ACTIONS");
      render_pill_button(it, 110, 356, 130, 18, theme_secondary, col_dark_gray, "TRANSFER QUEUE", col_white, font_small);
      render_pill_button(it, 110, 356, 188, 18, theme_secondary, col_dark_gray, "CONTROL ONLY", col_white, font_small);
      render_pill_button(it, 110, 356, 246, 18, theme_secondary, col_dark_gray, "JOIN AUDIO", col_white, font_small);
      render_pill_button(it, 110, 356, 304, 18, theme_secondary, col_dark_gray, "UNJOIN / DETACH", col_white, font_small);
    }
  } else if (music_overlay_mode == 6) {
    // LEVEL 6: NOW PLAYING TRACK & ALBUM INSPECTOR
    std::string disp_t = cur_title.empty() ? "NOW PLAYING" : fit_text_to_width_font(cur_title, 240, font_date);
    it.print(cx, 44, font_date, theme_primary, esphome::display::TextAlign::CENTER, disp_t.c_str());

    std::string sub_meta = cur_artist;
    if (!cur_album.empty() && cur_album != "None") {
      sub_meta = sub_meta.empty() ? cur_album : (sub_meta + " • " + cur_album);
    }
    if (!sub_meta.empty() && sub_meta != "None") {
      sub_meta = fit_text_to_width_font(sub_meta, 275, font_small);
      it.print(cx, 74, font_small, col_slate, esphome::display::TextAlign::CENTER, sub_meta.c_str());
    }

    // 1. Top Pill: Album Tracks (y = 118) with 3-bar tracklist icon
    render_pill_button(it, 118, 348, 118, 18, theme_primary, col_dark_gray, "Album Tracks", col_white, font_small);
    for (int dy : {-4, 0, 4}) {
      it.filled_rectangle(332, 118 + dy, 12, 2, theme_secondary);
    }

    // 2. Center Equator (y = 188 .. 248): Wide Track Position Scrubber (x = 95..371, y = 233)
    float cur_pos = music_pos_base;
    if (is_playing && music_pos_sync_time > 0 && now_ms >= music_pos_sync_time) {
      cur_pos += static_cast<float>(now_ms - music_pos_sync_time) * 0.001f;
    }
    float valid_dur = (!std::isnan(dur) && dur > 0.0f) ? dur : 0.0f;
    if (valid_dur > 0.0f && cur_pos > valid_dur) cur_pos = valid_dur;
    if (cur_pos < 0.0f) cur_pos = 0.0f;
    float prog = (valid_dur > 0.0f) ? (cur_pos / valid_dur) : 0.0f;

    int el_s = static_cast<int>(cur_pos);
    int tot_s = static_cast<int>(valid_dur);
    char time_buf[40];
    if (tot_s > 0) {
      snprintf(time_buf, sizeof(time_buf), "%02d:%02d  /  %02d:%02d   (%d%%)",
               el_s / 60, el_s % 60, tot_s / 60, tot_s % 60, static_cast<int>(prog * 100.0f));
    } else {
      snprintf(time_buf, sizeof(time_buf), "%02d:%02d  /  --:--", el_s / 60, el_s % 60);
    }
    it.print(cx, 188, font_small, col_white, esphome::display::TextAlign::CENTER, time_buf);

    // Wide Equator Scrubber Bar (x = 95..371, width = 276px at y = 233) + Vector Seek Buttons (x = 50, x = 416)
    const int scrub_y = 233;
    const int bar_x0  = 95;
    const int bar_x1  = 371;
    const int bar_w   = bar_x1 - bar_x0;

    // Left -15s Seek Button (x = 50, y = 233, r = 20, subtle Tools-style border + crisp double-chevron «)
    esphome::Color btn_bg(22, 27, 36);
    esphome::Color btn_brd(58, 68, 84);
    it.filled_circle(50, scrub_y, 20, btn_bg);
    it.circle(50, scrub_y, 20, btn_brd);
    it.line(54, scrub_y - 6, 48, scrub_y, col_white);
    it.line(48, scrub_y,     54, scrub_y + 6, col_white);
    it.line(48, scrub_y - 6, 42, scrub_y, theme_secondary);
    it.line(42, scrub_y,     48, scrub_y + 6, theme_secondary);

    // Right +15s Seek Button (x = 416, y = 233, r = 20, subtle Tools-style border + crisp double-chevron »)
    it.filled_circle(416, scrub_y, 20, btn_bg);
    it.circle(416, scrub_y, 20, btn_brd);
    it.line(412, scrub_y - 6, 418, scrub_y, col_white);
    it.line(418, scrub_y,     412, scrub_y + 6, col_white);
    it.line(418, scrub_y - 6, 424, scrub_y, theme_secondary);
    it.line(424, scrub_y,     418, scrub_y + 6, theme_secondary);

    // Scrubber Track Groove (r=5 capsule) + 25%/50%/75% ticks
    it.filled_circle(bar_x0, scrub_y, 5, col_dark_gray);
    it.filled_circle(bar_x1, scrub_y, 5, col_dark_gray);
    it.filled_rectangle(bar_x0, scrub_y - 5, bar_w, 11, col_dark_gray);
    it.line(bar_x0, scrub_y - 5, bar_x1, scrub_y - 5, btn_brd);
    it.line(bar_x0, scrub_y + 5, bar_x1, scrub_y + 5, btn_brd);
    for (int q = 1; q <= 3; q++) {
      int tx = bar_x0 + (bar_w * q) / 4;
      it.line(tx, scrub_y - 8, tx, scrub_y - 6, col_slate);
      it.line(tx, scrub_y + 6, tx, scrub_y + 8, col_slate);
    }

    // Filled Elapsed Bar + 3D Playhead Knob
    int knob_x = bar_x0 + static_cast<int>(prog * static_cast<float>(bar_w));
    if (knob_x > bar_x0) {
      it.filled_circle(bar_x0, scrub_y, 4, theme_primary);
      it.filled_rectangle(bar_x0, scrub_y - 4, knob_x - bar_x0, 9, theme_primary);
      it.horizontal_line(bar_x0, scrub_y - 3, knob_x - bar_x0, col_white);
    }
    it.filled_circle(knob_x, scrub_y, 9, col_white);
    it.filled_circle(knob_x, scrub_y, 6, theme_secondary);
    it.filled_circle(knob_x, scrub_y, 2, col_black);

    // 3. Lower Row (y = 324): Two Subtle Tools-Launcher-Style Cards (Artist Discography & Up Next Queue)
    auto draw_subtle_tool_card = [&](int card_cx, int card_cy, bool is_queue, const char *label) {
      esphome::Color card_bg(22, 27, 36);
      esphome::Color card_border(48, 56, 70);
      render_modern_card(it, card_cx, card_cy, 128, 84, 16, card_bg, card_border, nullptr, col_white, font_small);

      int ix = card_cx;
      int iy = card_cy - 15;
      if (!is_queue) {
        // Artist Discography Icon: Vinyl Record with grooves & center label
        it.circle(ix, iy, 14, col_white);
        it.circle(ix, iy, 10, col_slate);
        it.filled_circle(ix, iy, 5, theme_primary);
        it.filled_circle(ix, iy, 2, col_bg);
      } else {
        // Active Play Queue Icon: Play triangle (▶) + 3 stacked queue bars (≡)
        it.filled_triangle(ix - 13, iy - 8, ix - 13, iy + 2, ix - 4, iy - 3, theme_primary);
        it.filled_rectangle(ix, iy - 7, 13, 2, col_white);
        it.filled_rectangle(ix, iy - 1, 13, 2, col_white);
        it.filled_rectangle(ix - 13, iy + 5, 26, 2, theme_secondary);
      }

      it.print(card_cx, card_cy + 20, font_small, col_white, esphome::display::TextAlign::CENTER, label);
    };

    draw_subtle_tool_card(160, 324, false, "Artist");
    draw_subtle_tool_card(306, 324, true,  "Queue");
  }

  render_back_button(it, cx, font_small, 426, "< BACK");
}

// =============================================================================
// SECTION 16: CLOCK PAGE ENGINE (DIGITAL & ANALOG WATCH FACES & BATTERY GAUGE)
// =============================================================================

template <typename DisplayT, typename FontT>
inline void render_battery_indicator(
    DisplayT &it, int cx, int by, float batt, bool is_charging,
    FontT *font_small, const esphome::Color &theme_primary,
    const esphome::Color &col_gold, const esphome::Color &col_green) {
  if (std::isnan(batt)) return;
  char batt_str[16];
  snprintf(batt_str, sizeof(batt_str), "%.0f%%", batt);
  int text_x = cx + 8;
  it.print(text_x, by, font_small, theme_primary, esphome::display::TextAlign::CENTER, batt_str);
  int bx = text_x - (batt >= 100.0f ? 36 : 28);
  if (is_charging) {
    it.filled_triangle(bx + 3, by - 9, bx - 3, by, bx + 1, by, col_gold);
    it.filled_triangle(bx - 1, by, bx + 2, by, bx - 2, by + 9, col_gold);
  } else {
    it.filled_rectangle(bx - 2, by - 9, 4, 2, col_green);
    it.rectangle(bx - 5, by - 7, 10, 16, col_green);
    int fill_h = static_cast<int>(std::max(1.0f, std::min(12.0f, (batt / 100.0f) * 12.0f)));
    it.filled_rectangle(bx - 3, (by + 7) - fill_h, 6, fill_h, col_green);
  }
}

template <typename DisplayT, typename TimeT, typename FontT>
inline void render_clock_page(
    DisplayT &it, int cx, int cy, const TimeT &now,
    bool clock_face_digital, bool is_24h_format,
    float batt, bool is_charging,
    FontT *font_time, FontT *font_date, FontT *font_small,
    const esphome::Color &theme_primary, const esphome::Color &theme_secondary,
    const esphome::Color &col_white, const esphome::Color &col_slate,
    const esphome::Color &col_gray, const esphome::Color &col_dark_gray,
    const esphome::Color &col_red, const esphome::Color &col_gold,
    const esphome::Color &col_green) {
  // 1. Always render the watch bezel rings, 60-tick perimeter chapter ring, and live battery gauge
  //    immediately on boot/wake so the watch face looks complete even before Home Assistant time syncs.
  it.circle(cx, cy, 230, col_dark_gray);
  it.circle(cx, cy, 228, col_slate);
  it.circle(cx, cy, 222, col_dark_gray);

  for (int i = 0; i < 60; i++) {
    if (!clock_face_digital && i == 30) continue;  // 6 o'clock pip sanctuary on Analog dial
    float s = DIAL_SIN[i];
    float c = DIAL_COS[i];
    int r_in = (i % 5 == 0) ? 212 : 216;
    const esphome::Color &tick_col = (i % 5 == 0) ? col_slate : col_gray;
    it.line(cx + static_cast<int>(r_in * s), cy - static_cast<int>(r_in * c),
            cx + static_cast<int>(220 * s), cy - static_cast<int>(220 * c), tick_col);
  }

  render_battery_indicator(it, cx, 348, batt, is_charging, font_small,
                           theme_primary, col_gold, col_green);
  render_back_button(it, cx, font_small, 408, "HOME");

  // 2. Pre-sync state: minimalist "--:--" placeholder without text clutter until HA time syncs
  if (!now.is_valid()) {
    if (clock_face_digital) {
      it.print(cx, 205, font_time, col_slate, esphome::display::TextAlign::CENTER, "--:--");
    } else {
      it.print(cx, 305, font_date, col_slate, esphome::display::TextAlign::CENTER, "--:--");
      it.filled_circle(cx, cy, 7, col_white);
      it.filled_circle(cx, cy, 4, col_red);
      it.filled_circle(cx, cy, 2, col_dark_gray);
    }
    return;
  }

  // 3. Synchronized live watch face rendering
  int hour = now.hour;
  int min = now.minute;
  int sec = now.second;
  int sec_idx = ((sec % 60) + 60) % 60;

  if (clock_face_digital) {
    // Highlight active second tick in theme primary (enlarged +25% in length & width)
    float s_sec = DIAL_SIN[sec_idx];
    float c_sec = DIAL_COS[sec_idx];
    float perp_x = -c_sec;
    float perp_y = -s_sec;
    for (int w = -1; w <= 1; w++) {
      int x1 = cx + static_cast<int>(205.0f * s_sec + w * perp_x);
      int y1 = cy - static_cast<int>(205.0f * c_sec - w * perp_y);
      int x2 = cx + static_cast<int>(220.0f * s_sec + w * perp_x);
      int y2 = cy - static_cast<int>(220.0f * c_sec - w * perp_y);
      it.line(x1, y1, x2, y2, theme_primary);
    }

    // Date Header: "MON, SEP 28" (Theme Primary)
    it.strftime(cx, 130, font_date, theme_primary, esphome::display::TextAlign::CENTER, "%a, %b %d", now);

    // Giant Bold Digital Time: "10:45" (12h) or "14:45" (24h)
    char dig_buf[16];
    char ap_str[8] = "";
    format_time_hhmm(hour, min, is_24h_format, dig_buf, sizeof(dig_buf), ap_str);
    it.print(cx, 205, font_time, col_white, esphome::display::TextAlign::CENTER, dig_buf);

    // AM/PM & Seconds Pill: (Theme Secondary)
    char sec_buf[32];
    if (is_24h_format) {
      snprintf(sec_buf, sizeof(sec_buf), "SEC  •  %02ds", sec);
    } else {
      snprintf(sec_buf, sizeof(sec_buf), "%s  •  %02ds", ap_str, sec);
    }
    it.print(cx, 262, font_small, theme_secondary, esphome::display::TextAlign::CENTER, sec_buf);
  } else {
    // Formatted date (Theme Primary) and digital time complication (uniform slate gray)
    it.strftime(cx, 104, font_date, theme_primary, esphome::display::TextAlign::CENTER, "%a, %b %d", now);
    char time_buf[32];
    if (is_24h_format) {
      snprintf(time_buf, sizeof(time_buf), "%02d:%02d", hour, min);
    } else {
      int h12 = hour % 12;
      if (h12 == 0) h12 = 12;
      snprintf(time_buf, sizeof(time_buf), "%d:%02d %s", h12, min, hour >= 12 ? "pm" : "am");
    }
    it.print(cx, 305, font_date, col_slate, esphome::display::TextAlign::CENTER, time_buf);

    // Zero-trig LUT angles for Hour, Minute, and Second hands
    float h_angle = ((hour % 12) + min / 60.0f + sec / 3600.0f) * (2.0f * PI_F / 12.0f);
    float m_angle = (min + sec / 60.0f) * (2.0f * PI_F / 60.0f);

    // Hour Hand: Bold White Sword Hand (base width 14px, length 88px)
    {
      float sin_h = 0.0f, cos_h = 1.0f;
      fast_sincos(h_angle, sin_h, cos_h);
      it.filled_triangle(cx - static_cast<int>(7.0f * cos_h), cy - static_cast<int>(7.0f * sin_h),
                         cx + static_cast<int>(7.0f * cos_h), cy + static_cast<int>(7.0f * sin_h),
                         cx + static_cast<int>(88.0f * sin_h), cy - static_cast<int>(88.0f * cos_h),
                         col_white);
    }

    // Minute Hand: Slender Slate Gray Sword Hand (base width 8px, length 148px)
    {
      float sin_m = 0.0f, cos_m = 1.0f;
      fast_sincos(m_angle, sin_m, cos_m);
      it.filled_triangle(cx - static_cast<int>(4.0f * cos_m), cy - static_cast<int>(4.0f * sin_m),
                         cx + static_cast<int>(4.0f * cos_m), cy + static_cast<int>(4.0f * sin_m),
                         cx + static_cast<int>(148.0f * sin_m), cy - static_cast<int>(148.0f * cos_m),
                         col_slate);
    }

    // Second Hand: Bold Red Needle (length 168px) with 4px circular counterweight via O(1) DIAL_SIN/COS
    {
      float sin_s = DIAL_SIN[sec_idx];
      float cos_s = DIAL_COS[sec_idx];
      int tip_x = cx + static_cast<int>(168.0f * sin_s);
      int tip_y = cy - static_cast<int>(168.0f * cos_s);
      int tail_x = cx - static_cast<int>(30.0f * sin_s);
      int tail_y = cy + static_cast<int>(30.0f * cos_s);
      it.line(tail_x, tail_y, tip_x, tip_y, col_red);
      it.filled_circle(tail_x, tail_y, 4, col_red);
    }

    // Center Pinion Cap (White ring, red center pip, dark gray arbor)
    it.filled_circle(cx, cy, 7, col_white);
    it.filled_circle(cx, cy, 4, col_red);
    it.filled_circle(cx, cy, 2, col_dark_gray);
  }
}

// =============================================================================
// SECTION 16B: INTERACTIVE HOME PAGE ENGINE (PAGE 1 — 20 FPS WEATHER + SMART STACK + DOCK)
// =============================================================================

struct HomeActivityItem {
  int kind; // 1 = Timer, 2 = Stopwatch, 3 = Music, 4 = Alarm
  std::string label;
};

inline int collect_home_activities(
    uint32_t now_ms, bool is_24h_format,
    bool stopwatch_running, uint32_t stopwatch_start_time, uint32_t stopwatch_elapsed_ms,
    bool music_playing, const std::string &music_title, const std::string &music_artist,
    uint32_t alarm_snooze_until,
    int alarm1_h, int alarm1_m, bool alarm1_en,
    int alarm2_h, int alarm2_m, bool alarm2_en,
    int alarm3_h, int alarm3_m, bool alarm3_en,
    HomeActivityItem out_items[4]) {
  int count = 0;

  // 1. Active or Paused Countdown Timer (from 3-slot pool)
  auto *t_slots = timer_slots();
  int best_t = -1;
  for (int i = 0; i < 3; i++) {
    if (t_slots[i].running) {
      if (best_t < 0 || t_slots[i].remaining_sec < t_slots[best_t].remaining_sec) best_t = i;
    }
  }
  if (best_t < 0) {
    for (int i = 0; i < 3; i++) {
      if (t_slots[i].paused) { best_t = i; break; }
    }
  }
  if (best_t >= 0) {
    const auto &s = t_slots[best_t];
    std::string t_name = s.name.empty() ? "Timer" : s.name;
    char buf[64];
    snprintf(buf, sizeof(buf), "%s  •  %02d:%02d%s",
             t_name.c_str(), s.remaining_sec / 60, s.remaining_sec % 60,
             s.paused ? " (Paused)" : "");
    out_items[count++] = {1, std::string(buf)};
  }

  // 2. Active or Paused Stopwatch
  uint32_t sw_ms = stopwatch_running ? (now_ms - stopwatch_start_time) : stopwatch_elapsed_ms;
  if (stopwatch_running || sw_ms > 0) {
    int mins = (sw_ms / 60000U) % 60U;
    int secs = (sw_ms / 1000U) % 60U;
    int tenths = (sw_ms / 100U) % 10U;
    char buf[48];
    snprintf(buf, sizeof(buf), "Stopwatch  •  %02d:%02d.%d", mins, secs, tenths);
    out_items[count++] = {2, std::string(buf)};
  }

  // 3. Now Playing Music
  if (music_playing) {
    std::string m_txt = music_title;
    if (m_txt.empty() || m_txt == "None" || m_txt == "Idle") m_txt = "Now Playing";
    else if (!music_artist.empty() && music_artist != "None") {
      m_txt = music_artist + " • " + m_txt;
    }
    out_items[count++] = {3, m_txt};
  }

  // 4. Armed or Snoozed Alarm
  if (alarm_snooze_until > 0) {
    out_items[count++] = {4, "Alarm  •  Snoozed"};
  } else if (alarm1_en || alarm2_en || alarm3_en) {
    int ah = alarm1_en ? alarm1_h : (alarm2_en ? alarm2_h : alarm3_h);
    int am = alarm1_en ? alarm1_m : (alarm2_en ? alarm2_m : alarm3_m);
    char t_buf[16], ap_buf[8] = "";
    format_time_hhmm(ah, am, is_24h_format, t_buf, sizeof(t_buf), ap_buf);
    char buf[48];
    if (!is_24h_format && ap_buf[0] != '\0') {
      snprintf(buf, sizeof(buf), "Alarm  •  %s %s", t_buf, ap_buf);
    } else {
      snprintf(buf, sizeof(buf), "Alarm  •  %s", t_buf);
    }
    out_items[count++] = {4, std::string(buf)};
  }

  return count;
}

inline int collect_home_dock_apps(bool show_music, bool show_remote, bool show_tools, bool show_games,
                                  int out_apps[4]) {
  int n = 0;
  if (show_music)  out_apps[n++] = 2; // Music
  if (show_remote) out_apps[n++] = 7; // Remote
  if (show_tools)  out_apps[n++] = 8; // Tools
  if (show_games)  out_apps[n++] = 3; // Games
  return n;
}

// Supports 1..5 dock icons at y = 332 (r = 29 / 58px diameter; 5-icon v2.1 Room Control ready!)
inline void get_home_dock_xs(int count, int cx, int out_xs[5]) {
  if (count == 1) {
    out_xs[0] = cx;
  } else if (count == 2) {
    out_xs[0] = cx - 56;
    out_xs[1] = cx + 56;
  } else if (count == 3) {
    out_xs[0] = cx - 96;
    out_xs[1] = cx;
    out_xs[2] = cx + 96;
  } else if (count == 4) {
    out_xs[0] = 104;
    out_xs[1] = 190;
    out_xs[2] = 276;
    out_xs[3] = 362;
  } else if (count >= 5) {
    out_xs[0] = 93;
    out_xs[1] = 163;
    out_xs[2] = 233;
    out_xs[3] = 303;
    out_xs[4] = 373;
  }
}

template <typename DisplayT, typename TimeT, typename StrT, typename FontTimeT, typename FontDateT, typename FontSmallT>
inline void render_home_page(
    DisplayT &it, int cx, int cy, uint32_t now_ms, int home_subpage,
    int &home_activity_idx,
    const TimeT &now, bool is_24h_format,
    float batt, bool is_charging,
    bool show_weather_bg,
    bool dock_music, bool dock_remote, bool dock_tools, bool dock_games,
    const StrT &weather_sim_opt, float weather_temp,
    const std::string &weather_cond_raw, const std::string &sun_state,
    const std::string &moon_phase_raw, float accel_x, float accel_y,
    bool stopwatch_running, uint32_t stopwatch_start_time, uint32_t stopwatch_elapsed_ms,
    bool music_playing, const std::string &music_title, const std::string &music_artist,
    uint32_t alarm_snooze_until,
    int alarm1_h, int alarm1_m, bool alarm1_en,
    int alarm2_h, int alarm2_m, bool alarm2_en,
    int alarm3_h, int alarm3_m, bool alarm3_en,
    FontTimeT *font_time, FontDateT *font_date, FontSmallT *font_small,
    esphome::Color theme_primary, esphome::Color theme_secondary,
    esphome::Color col_bg, esphome::Color col_black, esphome::Color col_white,
    esphome::Color col_slate, esphome::Color col_dark_gray,
    esphome::Color col_gold, esphome::Color col_green) {
  using esphome::Color;
  using esphome::display::TextAlign;

  if (home_subpage == 1) {
    // -------------------------------------------------------------------------
    // SUBPAGE 1: HOME SETTINGS (Canonical 5-Card Template: 30px Top/Bottom Buffer, 19px Gap)
    //   - Title at y = 48 (bottom at y = 58)
    //   - 5 cards (w = 276, h = 44, r = 12) at y_mid = {110, 173, 236, 299, 362} (63px stride)
    //   - Top of Slot 0 = y 88 (30px below title); Bottom of Slot 4 = y 384 (31px above < BACK)
    // -------------------------------------------------------------------------
    it.print(cx, 48, font_small, theme_primary, TextAlign::CENTER, "HOME SETTINGS");

    const Color card_bg_on(22, 27, 36);
    const Color card_bg_off(15, 18, 24);
    const Color card_brd_on(52, 62, 78);
    const Color card_brd_off(34, 40, 50);

    auto draw_setting_card = [&](int y_mid, const char *label, bool is_on) {
      Color bg_c  = is_on ? card_bg_on  : card_bg_off;
      Color brd_c = is_on ? card_brd_on : card_brd_off;
      Color txt_c = is_on ? col_white   : col_slate;
      render_modern_card(it, cx, y_mid, 276, 44, 12, bg_c, brd_c, label, txt_c, font_small);
    };

    char buf[48];
    snprintf(buf, sizeof(buf), "Weather Sky: %s", show_weather_bg ? "On" : "Off");
    draw_setting_card(110, buf, show_weather_bg);

    snprintf(buf, sizeof(buf), "Music Icon: %s", dock_music ? "Shown" : "Hidden");
    draw_setting_card(173, buf, dock_music);

    snprintf(buf, sizeof(buf), "Remote Icon: %s", dock_remote ? "Shown" : "Hidden");
    draw_setting_card(236, buf, dock_remote);

    snprintf(buf, sizeof(buf), "Tools Icon: %s", dock_tools ? "Shown" : "Hidden");
    draw_setting_card(299, buf, dock_tools);

    snprintf(buf, sizeof(buf), "Games Icon: %s", dock_games ? "Shown" : "Hidden");
    draw_setting_card(362, buf, dock_games);

    render_back_button(it, cx, font_small, 426, "< BACK");
    return;
  }

  // ---------------------------------------------------------------------------
  // SUBPAGE 0: MAIN INTERACTIVE HOME DASHBOARD
  // ---------------------------------------------------------------------------
  std::string cond = weather_cond_raw;
  bool is_night = (sun_state == "below_horizon" || cond == "clear-night" ||
                   cond == "clearnight" || (cond.find("night") != std::string::npos));
  apply_weather_sim_override(weather_sim_opt, cond, is_night);

  if (show_weather_bg) {
    double moon_cycle = 0.5;
    if (is_night) {
      compute_synodic_moon_phase(now.is_valid(), now.is_valid() ? static_cast<int64_t>(now.timestamp) : 0LL,
                                 moon_phase_raw, moon_cycle, nullptr, nullptr);
    }
    // Sweet-spot celestial coordinates (351, 86): 28px inside bezel edge + clear of 4-digit time at y=128
    render_weather_motion_wallpaper(it, cx, cy, now_ms, cond, is_night,
                                    moon_cycle, 351, 86, true, accel_x, accel_y);
  }

  // ZONE 1 (y = 52): Larger Centered Date Header in font_date (27pt, Tap -> HOME SETTINGS)
  char date_bar[32] = "Pocket Assistant";
  if (now.is_valid()) {
    TimeT now_copy = now;
    now_copy.strftime(date_bar, sizeof(date_bar), "%a, %b %d");
  }
  it.print(cx + 1, 53, font_date, col_black, TextAlign::CENTER, date_bar);
  it.print(cx,     52, font_date, col_white, TextAlign::CENTER, date_bar);

  // ZONE 2 (y = 128): Hero Digital Clock (77pt Roboto, Tap -> Opens Full Clock Face)
  char time_str[16] = "--:--";
  if (now.is_valid()) {
    format_time_hhmm(now.hour, now.minute, is_24h_format, time_str, sizeof(time_str));
  }
  it.print(cx + 1, 129, font_time, col_black, TextAlign::CENTER, time_str);
  it.print(cx,     128, font_time, col_white, TextAlign::CENTER, time_str);

  // ZONE 3 (y = 200): Stepped-Up Weather Summary in font_date (27pt, Tap -> Opens Sky & Weather)
  std::string cond_title = format_weather_title(cond);
  char w_line[64];
  if (!std::isnan(weather_temp)) {
    snprintf(w_line, sizeof(w_line), "%.0f° • %s", weather_temp, cond_title.c_str());
  } else {
    snprintf(w_line, sizeof(w_line), "--° • %s", cond_title.c_str());
  }
  std::string fit_w = fit_text_to_width_font(std::string(w_line), 340, font_date);
  Color cond_col = (!show_weather_bg || is_night) ? theme_secondary : Color(255, 225, 110);
  it.print(cx + 1, 201, font_date, col_black, TextAlign::CENTER, fit_w.c_str());
  it.print(cx,     200, font_date, cond_col,  TextAlign::CENTER, fit_w.c_str());

  // ZONE 4 (y = 264): Smart Live Activity Stack (Timer, Stopwatch, Music, Alarm)
  HomeActivityItem act_items[4];
  int act_count = collect_home_activities(
      now_ms, is_24h_format,
      stopwatch_running, stopwatch_start_time, stopwatch_elapsed_ms,
      music_playing, music_title, music_artist,
      alarm_snooze_until,
      alarm1_h, alarm1_m, alarm1_en,
      alarm2_h, alarm2_m, alarm2_en,
      alarm3_h, alarm3_m, alarm3_en,
      act_items);

  static int prev_act_count = 0;
  if (act_count > prev_act_count) {
    home_activity_idx = 0;
  }
  prev_act_count = act_count;

  if (act_count > 0) {
    if (home_activity_idx < 0 || home_activity_idx >= act_count) {
      home_activity_idx = 0;
    }
    const auto &cur_act = act_items[home_activity_idx];
    // Centered at y = 258 (y = 236..280): exact 23px buffer below weather line (213) and 23px above dock (303)!
    const int pill_y = 258;
    render_modern_card(it, cx, pill_y, 292, 44, 22, Color(16, 22, 32), theme_primary,
                       nullptr, col_white, font_small);

    int gx = 112, gy = pill_y;
    if (cur_act.kind == 1) {
      it.line(gx - 5, gy - 7, gx + 5, gy - 7, theme_secondary);
      it.line(gx - 5, gy + 7, gx + 5, gy + 7, theme_secondary);
      it.line(gx - 4, gy - 6, gx + 4, gy + 6, col_white);
      it.line(gx + 4, gy - 6, gx - 4, gy + 6, col_white);
      it.filled_triangle(gx - 3, gy + 6, gx + 3, gy + 6, gx, gy + 2, col_gold);
    } else if (cur_act.kind == 2) {
      it.circle(gx, gy + 1, 7, col_white);
      it.line(gx - 2, gy - 8, gx + 2, gy - 8, theme_secondary);
      it.line(gx, gy + 1, gx + 4, gy - 3, col_gold);
    } else if (cur_act.kind == 3) {
      it.filled_triangle(gx - 4, gy - 6, gx - 4, gy + 6, gx + 6, gy, theme_primary);
    } else {
      it.filled_triangle(gx, gy - 7, gx - 6, gy + 4, gx + 6, gy + 4, col_gold);
      it.filled_rectangle(gx - 7, gy + 4, 15, 2, col_white);
      it.filled_circle(gx, gy + 7, 2, col_gold);
    }

    std::string fit_act = fit_text_to_width_font(cur_act.label, 226, font_small);
    it.print(128, pill_y, font_small, col_white, TextAlign::CENTER_LEFT, fit_act.c_str());

    if (act_count > 1) {
      render_scroll_chevron(it, 415, 162, true,  theme_secondary, col_black, true);
      render_scroll_chevron(it, 415, 278, false, theme_secondary, col_black, true);
    }
  }

  // ZONE 5 (y = 332): Enlarged Auto-Centering Quick-Launch Icon Dock (r = 29 / 58px diameter, 5-icon ready)
  int dock_apps[4] = {0, 0, 0, 0};
  int dock_count = collect_home_dock_apps(dock_music, dock_remote, dock_tools, dock_games, dock_apps);
  if (dock_count > 0) {
    int dock_xs[5] = {0, 0, 0, 0, 0};
    get_home_dock_xs(dock_count, cx, dock_xs);
    const int dock_y = 332;
    const int dock_r = 29;

    auto draw_dock_glyph = [&](int app_id, int bx, int by, Color c, bool is_shadow) {
      if (app_id == 2) {
        // Music: Enlarged Beamed Eighth Notes (scaled for r = 29)
        it.filled_circle(bx - 9, by + 9, 5, c);
        it.filled_circle(bx + 8, by + 6, 5, c);
        it.filled_rectangle(bx - 5, by - 12, 3, 21, c);
        it.filled_rectangle(bx + 11, by - 15, 3, 21, c);
        it.line(bx - 5, by - 12, bx + 13, by - 15, c);
        it.line(bx - 5, by - 11, bx + 13, by - 14, c);
        it.line(bx - 5, by - 10, bx + 13, by - 13, c);
        it.line(bx - 5, by - 9,  bx + 13, by - 12, c);
      } else if (app_id == 7) {
        // Remote: Enlarged Concentric D-Pad Ring + 4 Directional Arrows + Center Dot (scaled for r = 29)
        it.circle(bx, by, 17, c);
        it.circle(bx, by, 16, c);
        it.filled_circle(bx, by, 4, c);
        it.filled_triangle(bx, by - 13, bx - 4, by - 8, bx + 4, by - 8, c);
        it.filled_triangle(bx, by + 13, bx - 4, by + 8, bx + 4, by + 8, c);
        it.filled_triangle(bx - 13, by, bx - 8, by - 4, bx - 8, by + 4, c);
        it.filled_triangle(bx + 13, by, bx + 8, by - 4, bx + 8, by + 4, c);
      } else if (app_id == 8) {
        // Tools: Sleek 45-Degree Machined Open-End Wrench (scaled for r = 29)
        for (int w = -3; w <= 3; w++) {
          it.line(bx - 10 + w, by + 10, bx + 6 + w, by - 6, c);
          it.line(bx - 10, by + 10 + w, bx + 6, by - 6 + w, c);
        }
        it.filled_circle(bx - 10, by + 10, 5, c);
        it.filled_circle(bx + 8, by - 8, 9, c);
        if (!is_shadow) {
          it.filled_circle(bx + 12, by - 12, 5, col_black);
          it.filled_triangle(bx + 6, by - 6, bx + 15, by - 9, bx + 9, by - 15, col_black);
          it.filled_circle(bx - 10, by + 10, 2, col_black);
        }
      } else if (app_id == 3) {
        // Games: Xbox Wireless Controller (Sides flare OUTWARD from top shoulders ±10px to lower palms ±18px!)
        // 1. Narrower top LB/RB shoulder bumpers (±10px) flaring outward down to ±18px palm swells
        static constexpr int8_t XB_OUTER[22] = {
           9, 11, 12, 13, 14, 15, 15, 16, 16, 17, 17, 18, 18, 18, 18, 18, 17, 17, 16, 15, 14, 12
        };
        static constexpr int8_t XB_INNER[22] = {
           0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  5,  7,  8,  9,  9, 10, 10, 11, 11
        };
        for (int row = 0; row < 22; row++) {
          int dy = row - 10; // spans by - 10 .. by + 11
          int ox = XB_OUTER[row];
          int ix = XB_INNER[row];
          if (ix == 0) {
            it.horizontal_line(bx - ox, by + dy, 2 * ox + 1, c);
          } else {
            it.horizontal_line(bx - ox, by + dy, ox - ix + 1, c);
            it.horizontal_line(bx + ix, by + dy, ox - ix + 1, c);
          }
        }
        // Rounded bulbous lower-outer palm ends at (bx ± 13, by + 7, r = 5)
        it.filled_circle(bx - 13, by + 7, 5, c);
        it.filled_circle(bx + 13, by + 7, 5, c);

        if (!is_shadow) {
          // 1. Top-center Xbox Nexus Guide button
          it.filled_circle(bx, by - 6, 2, col_black);
          it.draw_pixel_at(bx, by - 6, c);
          // 2. Asymmetrical Left Stick (upper-left at bx - 9, by - 3)
          it.filled_circle(bx - 9, by - 3, 3, col_black);
          it.filled_circle(bx - 9, by - 3, 1, c);
          // 3. Lower-Left D-Pad cross (+) at (bx - 5, by + 1)
          it.horizontal_line(bx - 7, by + 1, 5, col_black);
          it.vertical_line(bx - 5, by - 1, 5, col_black);
          // 4. Upper-Right 4-button ABXY Diamond at (bx + 9, by - 3)
          it.draw_pixel_at(bx + 9,  by - 5, col_black);
          it.draw_pixel_at(bx + 9,  by - 1, col_black);
          it.draw_pixel_at(bx + 7,  by - 3, col_black);
          it.draw_pixel_at(bx + 11, by - 3, col_black);
          // 5. Asymmetrical Right Stick (lower-right at bx + 5, by + 1)
          it.filled_circle(bx + 5, by + 1, 3, col_black);
          it.filled_circle(bx + 5, by + 1, 1, c);
        }
      }
    };

    for (int i = 0; i < dock_count; i++) {
      int bx = dock_xs[i];
      it.circle(bx + 1, dock_y + 1, dock_r, col_black);
      it.circle(bx,     dock_y,     dock_r, Color(95, 118, 148));
      draw_dock_glyph(dock_apps[i], bx + 1, dock_y + 1, col_black, true);
      draw_dock_glyph(dock_apps[i], bx,     dock_y,     col_white, false);
    }
  }

  // ZONE 6 (y = 424): Visually Separated Battery Readout (Left) & Enlarged 8-Tooth Settings Gear (Right)
  const int chin_y = 424;
  int batt_val = std::isnan(batt) ? 100 : static_cast<int>(roundf(batt));
  char batt_str[16];
  snprintf(batt_str, sizeof(batt_str), "%d%%", batt_val);

  // 1. Left Group: Battery Icon + Percentage (centered around x = 211)
  int text_x = cx - 22;
  it.print(text_x + 1, chin_y + 1, font_small, col_black, TextAlign::CENTER, batt_str);
  it.print(text_x,     chin_y,     font_small, col_white, TextAlign::CENTER, batt_str);

  int b_icon_x = text_x - (batt_val >= 100 ? 34 : 27);
  if (is_charging) {
    it.filled_triangle(b_icon_x + 3, chin_y - 9, b_icon_x - 3, chin_y, b_icon_x + 1, chin_y, col_gold);
    it.filled_triangle(b_icon_x - 1, chin_y, b_icon_x + 2, chin_y, b_icon_x - 2, chin_y + 9, col_gold);
  } else {
    it.filled_rectangle(b_icon_x - 2, chin_y - 9, 4, 2, col_green);
    it.rectangle(b_icon_x - 5, chin_y - 7, 10, 16, col_green);
    int fill_h = static_cast<int>(std::max(1.0f, std::min(12.0f, (static_cast<float>(batt_val) / 100.0f) * 12.0f)));
    it.filled_rectangle(b_icon_x - 3, (chin_y + 7) - fill_h, 6, fill_h, col_green);
  }

  // 2. Right Group: Enlarged, Crisp 8-Tooth Settings Gear at x = cx + 44 (277, y = 424, r_outer = 11px)
  int gear_x = cx + 44;
  int gear_y = chin_y;
  auto draw_gear_large = [&](int gx, int gy, Color c) {
    // 4 Cardinal 3px-wide teeth (r = 11)
    it.filled_rectangle(gx - 1, gy - 11, 3, 23, c);
    it.filled_rectangle(gx - 11, gy - 1, 23, 3, c);
    // 4 Diagonal 3px-thick teeth (at +/- 8px)
    for (int d = -1; d <= 1; d++) {
      it.line(gx - 8 + d, gy - 8, gx + 8 + d, gy + 8, c);
      it.line(gx - 8, gy - 8 + d, gx + 8, gy + 8 + d, c);
      it.line(gx - 8 + d, gy + 8, gx + 8 + d, gy - 8, c);
      it.line(gx - 8, gy + 8 + d, gx + 8, gy - 8 + d, c);
    }
    // Solid gear wheel body (r = 8)
    it.filled_circle(gx, gy, 8, c);
  };
  draw_gear_large(gear_x + 1, gear_y + 1, col_black);
  draw_gear_large(gear_x,     gear_y,     col_white);
  it.circle(gear_x, gear_y, 5, theme_secondary);
  it.filled_circle(gear_x, gear_y, 4, col_black);
}
// =============================================================================
// SECTION 17: UNIVERSAL TV & MEDIA REMOTE ENGINE (MULTI-DEVICE & MULTI-PROTOCOL)
// =============================================================================

inline bool is_configured_remote_slot(const std::string &ent) {
  if (ent.empty()) return false;
  return ent != "none" && ent != "remote.none" && ent != "media_player.none";
}

inline std::string format_remote_entity_fallback(const std::string &ent) {
  if (!is_configured_remote_slot(ent)) return "";
  std::string s = ent;
  size_t dot = s.find('.');
  if (dot != std::string::npos) s = s.substr(dot + 1);
  bool cap = true;
  for (size_t i = 0; i < s.length(); i++) {
    if (s[i] == '_') {
      s[i] = ' ';
      cap = true;
    } else if (cap) {
      s[i] = static_cast<char>(::toupper(static_cast<unsigned char>(s[i])));
      cap = false;
    }
  }
  // Uppercase common AV acronyms (TV, LG, AVR)
  auto replace_word = [&](const std::string &from, const std::string &to) {
    size_t pos = 0;
    while ((pos = s.find(from, pos)) != std::string::npos) {
      bool left_ok = (pos == 0 || s[pos - 1] == ' ');
      bool right_ok = (pos + from.length() == s.length() || s[pos + from.length()] == ' ');
      if (left_ok && right_ok) {
        s.replace(pos, from.length(), to);
      }
      pos += to.length();
    }
  };
  replace_word("Tv", "TV");
  replace_word("Lg", "LG");
  replace_word("Avr", "AVR");
  return s;
}

inline const char *detect_remote_profile(const std::string &ent, const std::string &name,
                                         const char *default_profile = "Android TV / Fire TV / Bravia") {
  std::string s = ent + " " + name;
  for (char &c : s) c = static_cast<char>(::tolower(static_cast<unsigned char>(c)));
  if (s.find("marantz") != std::string::npos || s.find("denon") != std::string::npos ||
      s.find("cinema") != std::string::npos || s.find("avr") != std::string::npos) {
    return "Denon / Marantz AVR";
  }
  if (s.find("webos") != std::string::npos || s.find("lg_tv") != std::string::npos ||
      s.find("lg tv") != std::string::npos || s.find("lgtv") != std::string::npos) {
    return "LG webOS";
  }
  if (s.find("apple") != std::string::npos) {
    return "Apple TV";
  }
  if (s.find("roku") != std::string::npos) {
    return "Roku";
  }
  if (s.find("samsung") != std::string::npos || s.find("tizen") != std::string::npos) {
    return "Samsung TV";
  }
  if (s.find("shield") != std::string::npos || s.find("android") != std::string::npos ||
      s.find("fire") != std::string::npos || s.find("bravia") != std::string::npos ||
      s.find("chromecast") != std::string::npos || s.find("google") != std::string::npos) {
    return "Android TV / Fire TV / Bravia";
  }
  if (ent.rfind("media_player.", 0) == 0) {
    return "Denon / Marantz AVR";
  }
  return default_profile;
}

inline int collect_configured_remote_slots(const std::string ents[4], int out_slots[4]) {
  int count = 0;
  for (int i = 0; i < 4; i++) {
    if (is_configured_remote_slot(ents[i])) {
      out_slots[count++] = i;
    }
  }
  return count;
}

inline int resolve_active_remote_slot(int active_slot, const std::string ents[4]) {
  if (active_slot >= 0 && active_slot < 4 && is_configured_remote_slot(ents[active_slot])) {
    return active_slot;
  }
  for (int i = 0; i < 4; i++) {
    if (is_configured_remote_slot(ents[i])) return i;
  }
  return 0;
}

inline bool &avr_setup_menu_open() {
  static bool open = false;
  return open;
}

struct RemoteCustomCmds {
  const char *up;
  const char *down;
  const char *left;
  const char *right;
  const char *select;
  const char *back;
  const char *home;
  const char *vol_up;
  const char *vol_down;
  const char *power;
};

inline void resolve_remote_dispatch(
    int key_idx, int target_slot,
    const std::string slot_profiles[4],
    const std::string ents[4], const std::string names[4],
    const RemoteCustomCmds &custom_cmds,
    std::string &out_ent, std::string &out_prof,
    std::string &out_cmd, std::string &out_feedback) {
  static constexpr const char *kFeedback[10] = {
    "Up", "Down", "Left", "Right", "Select",
    "Back", "Home", "Vol +", "Vol -", "Toggle Power"
  };
  int idx = (key_idx >= 0 && key_idx < 10) ? key_idx : 0;
  int resolved_slot = 0;
  std::string dev_name;
  if (target_slot == -1) {
    resolved_slot = resolve_active_remote_slot(0, ents);
    out_ent = ents[resolved_slot];
    dev_name = names[resolved_slot].empty() ? format_remote_entity_fallback(out_ent) : names[resolved_slot];
    out_feedback = "Power: " + dev_name;
  } else {
    resolved_slot = resolve_active_remote_slot(target_slot, ents);
    out_ent = ents[resolved_slot];
    dev_name = names[resolved_slot].empty() ? format_remote_entity_fallback(out_ent) : names[resolved_slot];
    out_feedback = (idx == 9) ? ("Power: " + dev_name) : kFeedback[idx];
  }

  const std::string &cfg_prof = slot_profiles[resolved_slot];
  if (!cfg_prof.empty() && cfg_prof != "Auto-Detect") {
    out_prof = cfg_prof;
  } else {
    out_prof = detect_remote_profile(out_ent, dev_name, "Android TV / Fire TV / Bravia");
  }

  if (out_prof == "Denon / Marantz AVR") {
    static constexpr const char *kAvrCmds[10] = {
      "/goform/formiPhoneAppDirect.xml?MNCUP",
      "/goform/formiPhoneAppDirect.xml?MNCDN",
      "/goform/formiPhoneAppDirect.xml?MNCLT",
      "/goform/formiPhoneAppDirect.xml?MNCRT",
      "/goform/formiPhoneAppDirect.xml?MNENT",
      "/goform/formiPhoneAppDirect.xml?MNRTN",
      "/goform/formiPhoneAppDirect.xml?MNMEN%20ON",
      "VOLUME_UP", "VOLUME_DOWN", "POWER"
    };
    if (idx == 6) {
      bool &open = avr_setup_menu_open();
      open = !open;
      out_cmd = open ? "/goform/formiPhoneAppDirect.xml?MNMEN%20ON"
                     : "/goform/formiPhoneAppDirect.xml?MNMEN%20OFF";
    } else {
      out_cmd = kAvrCmds[idx];
    }
    return;
  }

  if (out_prof == "LG webOS") {
    static constexpr const char *kWebosBtns[10] = {
      "UP", "DOWN", "LEFT", "RIGHT", "ENTER",
      "BACK", "HOME", "VOLUMEUP", "VOLUMEDOWN", "POWER"
    };
    out_cmd = kWebosBtns[idx];
    return;
  }

  struct RemoteCmdRow {
    const char *apple_cmd;
    const char *roku_cmd;
    const char *samsung_cmd;
    const char *custom_cmd;
    const char *android_cmd;
  };
  const RemoteCmdRow kCmds[10] = {
    {"up",          "up",          "KEY_UP",      custom_cmds.up,       "DPAD_UP"},
    {"down",        "down",        "KEY_DOWN",    custom_cmds.down,     "DPAD_DOWN"},
    {"left",        "left",        "KEY_LEFT",    custom_cmds.left,     "DPAD_LEFT"},
    {"right",       "right",       "KEY_RIGHT",   custom_cmds.right,    "DPAD_RIGHT"},
    {"select",      "select",      "KEY_ENTER",   custom_cmds.select,   "DPAD_CENTER"},
    {"menu",        "back",        "KEY_RETURN",  custom_cmds.back,     "BACK"},
    {"top_menu",    "home",        "KEY_HOME",    custom_cmds.home,     "HOME"},
    {"volume_up",   "volume_up",   "KEY_VOLUP",   custom_cmds.vol_up,   "VOLUME_UP"},
    {"volume_down", "volume_down", "KEY_VOLDOWN", custom_cmds.vol_down, "VOLUME_DOWN"},
    {"wakeup",      "power",       "KEY_POWER",   custom_cmds.power,    "POWER"}
  };
  if (out_prof == "Apple TV") out_cmd = kCmds[idx].apple_cmd;
  else if (out_prof == "Roku") out_cmd = kCmds[idx].roku_cmd;
  else if (out_prof == "Samsung TV") out_cmd = kCmds[idx].samsung_cmd;
  else if (out_prof == "Custom") out_cmd = kCmds[idx].custom_cmd;
  else out_cmd = kCmds[idx].android_cmd;
}

template <typename DisplayT, typename FontT>
inline void render_tv_remote_page(
    DisplayT &it, int cx, int cy, uint32_t now_ms,
    int remote_subpage, int &active_remote_slot,
    const std::string ents[4], const std::string names[4],
    const std::string &remote_action_feedback, uint32_t remote_feedback_timer,
    FontT *font_date, FontT *font_small,
    const esphome::Color &theme_primary, const esphome::Color &theme_secondary,
    const esphome::Color &col_white, const esphome::Color &col_slate,
    const esphome::Color &col_gray, const esphome::Color &col_dark_gray,
    const esphome::Color &col_red) {
  int slot = resolve_active_remote_slot(active_remote_slot, ents);
  active_remote_slot = slot;

  std::string rem_name = names[slot];
  if (rem_name.empty()) {
    rem_name = format_remote_entity_fallback(ents[slot]);
    if (rem_name.empty()) rem_name = "TV Remote";
  }

  bool is_fb = (now_ms - remote_feedback_timer < 1500U && !remote_action_feedback.empty());

  if (remote_subpage == 1) {
    // -------------------------------------------------------------------------
    // SUBPAGE 1: SETTINGS (SYSTEM POWER + UP TO 4 PRECONFIGURED DEVICES)
    // -------------------------------------------------------------------------
    if (is_fb) {
      std::string fb_str = remote_action_feedback;
      for (char &c : fb_str) c = static_cast<char>(::toupper(static_cast<unsigned char>(c)));
      std::string fit_fb = fit_text_to_width_font(fb_str, 280, font_small);
      it.print(cx, 48, font_small, theme_primary, esphome::display::TextAlign::CENTER, fit_fb.c_str());
    } else {
      it.print(cx, 48, font_small, theme_primary, esphome::display::TextAlign::CENTER, "SETTINGS");
    }

    // Slot 0 (y = 104): Master System Power Button (Always targets tv_remote_power_entity)
    render_pill_button(it, 110, 356, 104, 18, col_red, col_dark_gray, "Toggle Power", col_white, font_small);

    // Slots 1..4 (y = 162, 220, 278, 336): Preconfigured Devices + Per-Device Power Override Circle
    int valid_slots[4] = {0, 0, 0, 0};
    int valid_count = collect_configured_remote_slots(ents, valid_slots);
    for (int row = 0; row < valid_count; row++) {
      int s_idx = valid_slots[row];
      int y_mid = 162 + row * 58;
      bool is_sel = (s_idx == slot);

      std::string d_name = names[s_idx];
      if (d_name.empty()) d_name = format_remote_entity_fallback(ents[s_idx]);

      esphome::Color border_c = is_sel ? theme_primary : theme_secondary;
      esphome::Color text_c = is_sel ? theme_primary : col_white;

      // Uniform pill width across all 4 slots (x1 = 110, x2 = 306 -> outer edges 92..324)
      // Styled identically to Speakers menu: left indicator dot at x = 114, left-aligned label at x = 130
      render_pill_button(it, 110, 306, y_mid, 18, border_c, col_dark_gray, "", text_c, font_small);
      if (is_sel) {
        it.filled_circle(114, y_mid, 6, theme_primary);
      } else {
        it.circle(114, y_mid, 5, col_slate);
      }
      std::string fit_lbl = fit_text_to_width_font(d_name, 182, font_small);
      it.print(130, y_mid, font_small, text_c, esphome::display::TextAlign::CENTER_LEFT, fit_lbl.c_str());

      if (row > 0) {
        // Secondary Devices (Slots 2..4): Individual Override Power Button right-aligned with Toggle Power
        // Top Toggle Power has x2 = 356 (right outer edge = 374); p_cx = 356 aligns right outer edge at 374
        // and leaves a clean 14px gap (324..338) between the device pill and the power button.
        const int p_cx = 356;
        it.filled_circle(p_cx, y_mid, 18, col_dark_gray);
        it.circle(p_cx, y_mid, 18, col_red);
        for (int i = 6; i < 54; i += 2) {
          int x1 = p_cx + static_cast<int>(7.0f * DIAL_SIN[i]);
          int y1 = y_mid - static_cast<int>(7.0f * DIAL_COS[i]);
          int x2 = p_cx + static_cast<int>(7.0f * DIAL_SIN[i + 2]);
          int y2 = y_mid - static_cast<int>(7.0f * DIAL_COS[i + 2]);
          it.line(x1, y1, x2, y2, col_white);
        }
        it.filled_rectangle(p_cx - 1, y_mid - 9, 3, 8, col_white);
      }
    }

    // Standardized Centered < BACK Button at y = 410
    render_back_button(it, cx, font_small, 426, "< BACK");
    return;
  }

  // ---------------------------------------------------------------------------
  // SUBPAGE 0: MAIN REMOTE CONTROL INTERFACE
  // ---------------------------------------------------------------------------
  if (is_fb) {
    std::string fb_str = remote_action_feedback;
    for (char &c : fb_str) c = static_cast<char>(::toupper(static_cast<unsigned char>(c)));
    std::string fit_fb = fit_text_to_width_font(fb_str, 280, font_small);
    it.print(cx, 50, font_small, theme_primary, esphome::display::TextAlign::CENTER, fit_fb.c_str());
  } else {
    std::string upper_name = rem_name;
    for (char &c : upper_name) c = static_cast<char>(::toupper(static_cast<unsigned char>(c)));
    std::string disp_title = fit_text_to_width_font(upper_name, 280, font_small);
    it.print(cx, 50, font_small, theme_primary, esphome::display::TextAlign::CENTER, disp_title.c_str());
  }

  // Flank Buttons Symmetrical Coordinates (dy = +/- 93 from cy = 233)
  const int left_fx = 71;
  const int right_fx = 395;
  const int top_fy = 140;
  const int btm_fy = 326;
  const int f_r = 18;

  // 1. HOME: Top-Left (71, 140)
  it.filled_circle(left_fx, top_fy, f_r, col_dark_gray);
  it.circle(left_fx, top_fy, f_r, col_gray);
  it.filled_triangle(left_fx, top_fy - 8, left_fx - 9, top_fy - 1, left_fx + 9, top_fy - 1, col_white);
  it.filled_rectangle(left_fx - 6, top_fy - 1, 13, 8, col_white);
  it.filled_rectangle(left_fx - 2, top_fy + 2, 5, 5, col_dark_gray);

  // 2. VOL UP: Top-Right (395, 140)
  it.filled_circle(right_fx, top_fy, f_r, col_dark_gray);
  it.circle(right_fx, top_fy, f_r, col_gray);
  it.filled_rectangle(right_fx - 7, top_fy - 1, 15, 3, col_white);
  it.filled_rectangle(right_fx - 1, top_fy - 7, 3, 15, col_white);

  // 3. BACK: Bottom-Left (71, 326)
  it.filled_circle(left_fx, btm_fy, f_r, col_dark_gray);
  it.circle(left_fx, btm_fy, f_r, col_gray);
  it.line(left_fx + 3, btm_fy - 7, left_fx - 4, btm_fy, col_white);
  it.line(left_fx + 4, btm_fy - 7, left_fx - 3, btm_fy, col_white);
  it.line(left_fx - 4, btm_fy, left_fx + 3, btm_fy + 7, col_white);
  it.line(left_fx - 3, btm_fy, left_fx + 4, btm_fy + 7, col_white);

  // 4. VOL DOWN: Bottom-Right (395, 326)
  it.filled_circle(right_fx, btm_fy, f_r, col_dark_gray);
  it.circle(right_fx, btm_fy, f_r, col_gray);
  it.filled_rectangle(right_fx - 7, btm_fy - 1, 15, 3, col_white);

  // Concentric D-Pad with Expanded Outer Radius (r_out = 132)
  const int dpad_r_in = 48;
  const int dpad_r_out = 132;
  it.circle(cx, cy, dpad_r_out, col_gray);
  it.circle(cx, cy, dpad_r_in, col_gray);

  // 45-degree sector dividers
  const int d_in = 34;
  const int d_out = 93;
  it.line(cx + d_in, cy + d_in, cx + d_out, cy + d_out, col_gray);
  it.line(cx - d_in, cy + d_in, cx - d_out, cy + d_out, col_gray);
  it.line(cx + d_in, cy - d_in, cx + d_out, cy - d_out, col_gray);
  it.line(cx - d_in, cy - d_in, cx - d_out, cy - d_out, col_gray);

  // Directional Arrows
  it.filled_triangle(cx, cy - 97, cx - 10, cy - 82, cx + 10, cy - 82, col_white);
  it.filled_triangle(cx, cy + 97, cx - 10, cy + 82, cx + 10, cy + 82, col_white);
  it.filled_triangle(cx - 97, cy, cx - 82, cy - 10, cx - 82, cy + 10, col_white);
  it.filled_triangle(cx + 97, cy, cx + 82, cy - 10, cx + 82, cy + 10, col_white);

  // Center OK Button (Radius 40)
  it.filled_circle(cx, cy, 40, col_dark_gray);
  it.circle(cx, cy, 40, theme_primary);
  it.print(cx, cy, font_date, col_white, esphome::display::TextAlign::CENTER, "OK");
}

// =============================================================================
// SECTION 18: SYSTEM SETTINGS & DIAGNOSTICS ENGINE (PAGE 4)
// =============================================================================

template <typename DisplayT, typename FontDateT, typename FontSmallT>
inline void render_system_page(
    DisplayT &it, int cx, uint32_t now_ms, const char *version_str,
    int system_menu_mode, int system_dashboard_page,
    int sleep_menu_page, int display_menu_page,
    bool menu_scroll_active, int menu_scroll_step, int menu_scroll_dir, int menu_scroll_target_page,
    int &update_check_state, uint32_t &update_check_timer,
    const std::string &ha_fw_latest, const std::string &ha_fw_summary,
    float batt, bool is_charging, float batt_v,
    float wifi_sig, const std::string &ip_str, float imu_temp_c,
    uint32_t free_heap_kb, bool ha_time_valid, bool ble_active,
    // Sleep & Wake values
    const std::string &pickup_wake_opt, const std::string &tap_wake_opt,
    const std::string &screen_off_opt, bool docked_ss_on, const std::string &ss_style_opt,
    const std::string &dock_inact_opt, const std::string &hibernate_to_opt,
    bool instant_crown_hib, bool stay_awake_on,
    // Display & Clock values
    bool clock_face_digital, bool is_24h_format,
    const std::string &palette_opt, const std::string &default_app_opt,
    const std::string &auto_bright_opt, const std::string &manual_bright_opt,
    int day_start_hr, int night_start_hr,
    const std::string &day_brightness_opt, const std::string &night_brightness_opt,
    // Sound values
    int spk_vol_pct, int alert_vol_pct, int vol_step_pct,
    // Music values
    const std::string &art_mode_opt, const std::string &overlay_style_opt,
    const std::string &album_sort_opt, const std::string &artist_sort_opt,
    const std::string &podcast_sort_opt,
    // Voice & Per-Model Wake Word Sensitivity values
    const std::string &va_ww_mode_opt, const std::string &va_ww_active_opt,
    const std::string &sens_nabu_opt, const std::string &sens_luna_opt,
    const std::string &sens_jarvis_opt, const std::string &sens_mycroft_opt,
    const std::string &sens_alexa_opt,
    const std::string &va_profile_opt, bool va_wake_scr,
    // Fonts & Colors
    FontDateT *font_date, FontSmallT *font_small,
    const esphome::Color &theme_primary, const esphome::Color &theme_secondary,
    const esphome::Color &col_bg, const esphome::Color &col_black,
    const esphome::Color &col_white, const esphome::Color &col_slate,
    const esphome::Color &col_dark_gray, const esphome::Color &col_gold,
    const esphome::Color &col_red, bool game_sound_on = true) {
  char buf[64];
  auto draw_back = [&]() {
    render_back_button(it, cx, font_small, 426, "< BACK");
  };
  auto draw_card_y = [&](int y_mid, const char *label, esphome::Color border_c,
                         esphome::Color text_c, esphome::Color bg_c) {
    render_pill_button(it, 110, 356, y_mid, 18, border_c, bg_c, label, text_c, font_small);
  };
  auto draw_slot_card = [&](int slot_idx, const char *label, esphome::Color border_c,
                            esphome::Color text_c, esphome::Color bg_c) {
    render_pill_button(it, 110, 356, 104 + slot_idx * 58, 18, border_c, bg_c, label, text_c, font_small);
  };
  auto draw_right_chevrons = [&](bool show_up, bool show_down) {
    if (!show_up && !show_down) return;
    it.filled_rectangle(395, 140, 71, 160, col_bg);
    if (show_up) render_scroll_chevron(it, 415, 162, true, theme_secondary, col_black, false);
    if (show_down) render_scroll_chevron(it, 415, 278, false, theme_secondary, col_black, false);
  };
  auto compute_scroll_offset = [&](int cur_page, const int16_t ease_lut[5], int max_shift) -> int {
    if (menu_scroll_active) {
      int step = std::max(0, std::min(4, menu_scroll_step));
      int shift = ease_lut[step];
      return (menu_scroll_dir == 1) ? shift : (max_shift - shift);
    }
    return (cur_page == 2) ? max_shift : 0;
  };

  if (system_menu_mode == 0) {
    // -------------------------------------------------------------------------
    // VIEW 0: MAIN SYSTEM DASHBOARD (2 Pages x 4 Categories)
    // Page 1: Display, Sleep & Wake, Sound, Voice
    // Page 2: Music, Home, Info, Actions
    // -------------------------------------------------------------------------
    int current_offset = compute_scroll_offset(system_dashboard_page, SHARED_SCROLL_EASE_232, 232);
    static constexpr const char *kSysLabels[8] = {
      "Display", "Sleep & Wake", "Sound", "Voice",
      "Music",   "Home",         "Info",  "Actions"
    };
    for (int i = 0; i < 8; i++) {
      int y = 132 + i * 58 - current_offset;
      if (y >= 40 && y <= 390) {
        render_pill_button(it, 150, 316, y, 18, theme_secondary, col_dark_gray, kSysLabels[i], col_white, font_small);
      }
    }

    it.filled_rectangle(0, 0, 466, 106, col_bg);
    it.filled_rectangle(0, 332, 466, 134, col_bg);

    it.print(cx, 46, font_date, theme_primary, esphome::display::TextAlign::CENTER, "SYSTEM");

    bool has_update = is_newer_semver(ha_fw_latest, version_str);
    if (has_update) {
      snprintf(buf, sizeof(buf), "Update Ready: v%s >", ha_fw_latest.c_str());
      it.print(cx, 78, font_small, col_gold, esphome::display::TextAlign::CENTER, buf);
    } else {
      snprintf(buf, sizeof(buf), "Pocket Assistant v%s", version_str);
      it.print(cx, 78, font_small, col_white, esphome::display::TextAlign::CENTER, buf);
    }

    char b_main[32];
    snprintf(b_main, sizeof(b_main), "Battery: %.0f%%", std::isnan(batt) ? 100.0f : batt);
    if (is_charging) {
      const char *chg_str = " • Charging";
      int w_main = 0, w_chg = 0, x_off = 0, base = 0, text_h = 0;
      font_small->measure(b_main, &w_main, &x_off, &base, &text_h);
      font_small->measure(chg_str, &w_chg, &x_off, &base, &text_h);
      int start_x = cx - (w_main + w_chg) / 2;
      it.print(start_x, 370, font_small, theme_primary, esphome::display::TextAlign::CENTER_LEFT, b_main);
      it.print(start_x + w_main, 370, font_small, theme_secondary, esphome::display::TextAlign::CENTER_LEFT, chg_str);
    } else {
      it.print(cx, 370, font_small, theme_primary, esphome::display::TextAlign::CENTER, b_main);
    }

    draw_back();
    int active_page = menu_scroll_active ? menu_scroll_target_page : system_dashboard_page;
    draw_right_chevrons(active_page == 2, active_page == 1);

  } else if (system_menu_mode == 1) {
    // -------------------------------------------------------------------------
    // VIEW 1: SLEEP & WAKE SUBMENU (9 Items across 2 Pages, 232px 4-Row Scroll)
    // -------------------------------------------------------------------------
    int current_offset = compute_scroll_offset(sleep_menu_page, SHARED_SCROLL_EASE_232, 232);
    char sleep_labels[9][36];
    snprintf(sleep_labels[0], sizeof(sleep_labels[0]), "Motion: %s", pickup_wake_opt.c_str());
    snprintf(sleep_labels[1], sizeof(sleep_labels[1]), "Tap: %s", tap_wake_opt.c_str());
    snprintf(sleep_labels[2], sizeof(sleep_labels[2]), "Screen Off: %s", screen_off_opt.c_str());
    snprintf(sleep_labels[3], sizeof(sleep_labels[3]), "Screensaver: %s", docked_ss_on ? "On" : "Off");
    const char *short_s = (ss_style_opt == "Aurora Borealis") ? "Aurora"
                          : ((ss_style_opt == "Matrix Rain") ? "Matrix" : ss_style_opt.c_str());
    snprintf(sleep_labels[4], sizeof(sleep_labels[4]), "Style: %s", short_s);
    snprintf(sleep_labels[5], sizeof(sleep_labels[5]), "Dock Inact: %s", dock_inact_opt.c_str());
    snprintf(sleep_labels[6], sizeof(sleep_labels[6]), "Auto-Hibernate: %s", hibernate_to_opt.c_str());
    snprintf(sleep_labels[7], sizeof(sleep_labels[7]), "Hold Crown: %s", instant_crown_hib ? "Instant" : "5s Delay");
    snprintf(sleep_labels[8], sizeof(sleep_labels[8]), "Stay Awake: %s", stay_awake_on ? "On" : "Off");

    for (int i = 0; i < 9; i++) {
      int y = 104 + i * 58 - current_offset;
      if (y >= 50 && y <= 390) {
        draw_card_y(y, sleep_labels[i], theme_secondary, col_white, col_dark_gray);
      }
    }

    it.filled_rectangle(0, 0, 466, 75, col_bg);
    it.filled_rectangle(0, 370, 466, 96, col_bg);
    render_menu_header(it, cx, 50, "SLEEP & WAKE", font_date, theme_primary);
    draw_back();
    int active_page = menu_scroll_active ? menu_scroll_target_page : sleep_menu_page;
    draw_right_chevrons(active_page == 2, active_page == 1);

  } else if (system_menu_mode == 2) {
    // -------------------------------------------------------------------------
    // VIEW 2: DISPLAY SUBMENU (9 Items across 2 Pages, 232px 4-Row Scroll)
    // Slot 0: Start Screen (Home / Clock), Slot 1: Clock Face, Slot 2: Format, Slot 3: Palette, Slot 4..8: Brightness
    // -------------------------------------------------------------------------
    int current_offset = compute_scroll_offset(display_menu_page, SHARED_SCROLL_EASE_232, 232);
    char disp_labels[9][36];
    snprintf(disp_labels[0], sizeof(disp_labels[0]), "Start Screen: %s",
             (default_app_opt == "Clock") ? "Clock" : "Home");
    snprintf(disp_labels[1], sizeof(disp_labels[1]), "Clock Face: %s", clock_face_digital ? "Digital" : "Analog");
    snprintf(disp_labels[2], sizeof(disp_labels[2]), "Format: %s", is_24h_format ? "24-Hour" : "12-Hour");
    const char *pal_s = (palette_opt == "Default Classic") ? "Blue & Gold" : palette_opt.c_str();
    snprintf(disp_labels[3], sizeof(disp_labels[3]), "Palette: %s", pal_s);
    const char *ab_s = (auto_bright_opt == "Sun Synchronized") ? "Sun"
                       : ((auto_bright_opt == "Time Schedule") ? "Sched" : "Off");
    snprintf(disp_labels[4], sizeof(disp_labels[4]), "Auto Bright: %s", ab_s);
    snprintf(disp_labels[5], sizeof(disp_labels[5]), "Brightness: %s",
             manual_bright_opt.empty() ? "90%" : manual_bright_opt.c_str());
    int nh12 = (night_start_hr > 12) ? (night_start_hr - 12) : night_start_hr;
    snprintf(disp_labels[6], sizeof(disp_labels[6]), "Schedule: %dAM-%dPM", day_start_hr, nh12);
    snprintf(disp_labels[7], sizeof(disp_labels[7]), "Day Bri: %s", day_brightness_opt.c_str());
    snprintf(disp_labels[8], sizeof(disp_labels[8]), "Night Bri: %s", night_brightness_opt.c_str());

    bool ab_off = (auto_bright_opt == "Disabled" || auto_bright_opt.empty());
    bool ab_sched = (auto_bright_opt == "Time Schedule");

    for (int i = 0; i < 9; i++) {
      int y = 104 + i * 58 - current_offset;
      if (y >= 50 && y <= 390) {
        bool inactive = (i == 5 && !ab_off) || (i == 6 && !ab_sched) || ((i == 7 || i == 8) && ab_off);
        if (inactive) {
          draw_card_y(y, disp_labels[i], col_dark_gray, col_slate, col_bg);
        } else {
          draw_card_y(y, disp_labels[i], theme_secondary, col_white, col_dark_gray);
        }
      }
    }

    it.filled_rectangle(0, 0, 466, 75, col_bg);
    it.filled_rectangle(0, 370, 466, 96, col_bg);
    render_menu_header(it, cx, 50, "DISPLAY", font_date, theme_primary);
    draw_back();
    int active_page = menu_scroll_active ? menu_scroll_target_page : display_menu_page;
    draw_right_chevrons(active_page == 2, active_page == 1);

  } else if (system_menu_mode == 3) {
    // -------------------------------------------------------------------------
    // VIEW 3: ACTIONS SUBMENU (5 One-Shot / On-Demand System Controls)
    // -------------------------------------------------------------------------
    render_menu_header(it, cx, 50, "ACTIONS", font_date, theme_primary);
    draw_slot_card(0, "Screensaver Now", theme_secondary, col_white, col_dark_gray);
    draw_slot_card(1, "Hibernate Now", theme_secondary, col_white, col_dark_gray);
    draw_slot_card(2, "Software Update >", theme_secondary, col_white, col_dark_gray);
    snprintf(buf, sizeof(buf), "BLE Setup: %s", ble_active ? "Active" : "Off");
    draw_slot_card(3, buf, ble_active ? theme_primary : theme_secondary,
                   ble_active ? theme_primary : col_white, col_dark_gray);
    draw_slot_card(4, "Restart Device", col_red, col_white, col_dark_gray);
    draw_back();

  } else if (system_menu_mode == 4) {
    // -------------------------------------------------------------------------
    // VIEW 4: RESTART CONFIRMATION DIALOG
    // -------------------------------------------------------------------------
    it.print(cx, 130, font_date, theme_primary, esphome::display::TextAlign::CENTER, "RESTART?");
    render_pill_button(it, 125, 195, 230, 20, col_slate, col_dark_gray, "Cancel", col_white, font_small);
    render_pill_button(it, 271, 341, 230, 20, col_red, col_dark_gray, "Restart", col_white, font_small);

  } else if (system_menu_mode == 5) {
    // -------------------------------------------------------------------------
    // VIEW 5: SOFTWARE UPDATE SCREEN
    // -------------------------------------------------------------------------
    render_menu_header(it, cx, 50, "UPDATE", font_date, theme_primary);
    snprintf(buf, sizeof(buf), "Installed: v%s", version_str);
    it.print(cx, 95, font_small, col_slate, esphome::display::TextAlign::CENTER, buf);

    bool has_update = is_newer_semver(ha_fw_latest, version_str);
    if (update_check_state == 1) {
      if (now_ms - update_check_timer > 1500U) {
        update_check_state = has_update ? 0 : 2;
        update_check_timer = now_ms;
      }
    } else if (update_check_state == 2) {
      if (now_ms - update_check_timer > 3500U) {
        update_check_state = 0;
      }
    }

    if (has_update) {
      snprintf(buf, sizeof(buf), "Available: v%s", ha_fw_latest.empty() ? "2.0.x" : ha_fw_latest.c_str());
      it.print(cx, 125, font_small, theme_secondary, esphome::display::TextAlign::CENTER, buf);
      std::string summ = ha_fw_summary;
      if (summ.empty() || summ == "unavailable" || summ == "unknown") {
        summ = "Firmware update ready.";
      }
      summ = fit_text_to_width_font(summ, 280, font_small);
      it.print(cx, 160, font_small, col_white, esphome::display::TextAlign::CENTER, summ.c_str());
      render_pill_button(it, 110, 356, 215, 20, theme_primary, col_dark_gray, "Update Now", theme_secondary, font_small);
    } else {
      snprintf(buf, sizeof(buf), "Latest: v%s", version_str);
      it.print(cx, 125, font_small, theme_secondary, esphome::display::TextAlign::CENTER, buf);
      if (update_check_state == 1) {
        it.print(cx, 160, font_small, theme_secondary, esphome::display::TextAlign::CENTER, "Checking Home Assistant...");
        render_pill_button(it, 110, 356, 215, 20, theme_primary, col_dark_gray, "Checking...", theme_secondary, font_small);
      } else if (update_check_state == 2) {
        it.print(cx, 160, font_small, col_slate, esphome::display::TextAlign::CENTER, "No updates available");
        render_pill_button(it, 110, 356, 215, 20, theme_secondary, col_dark_gray, "Up to date", col_white, font_small);
      } else {
        it.print(cx, 160, font_small, col_slate, esphome::display::TextAlign::CENTER, "Firmware is up to date");
        render_pill_button(it, 110, 356, 215, 20, theme_primary, col_dark_gray, "Check for Updates", col_white, font_small);
      }
    }
    draw_back();

  } else if (system_menu_mode == 6) {
    // -------------------------------------------------------------------------
    // VIEW 6: UPDATE CONFIRMATION DIALOG
    // -------------------------------------------------------------------------
    it.print(cx, 100, font_date, theme_primary, esphome::display::TextAlign::CENTER, "INSTALL UPDATE?");
    it.print(cx, 145, font_small, col_white, esphome::display::TextAlign::CENTER, "Download & flash firmware");
    it.print(cx, 175, font_small, theme_secondary, esphome::display::TextAlign::CENTER, "Keep device powered on");
    render_pill_button(it, 125, 195, 240, 20, col_slate, col_dark_gray, "Cancel", col_white, font_small);
    render_pill_button(it, 271, 341, 240, 20, theme_primary, col_dark_gray, "Install", col_white, font_small);

  } else if (system_menu_mode == 7) {
    // -------------------------------------------------------------------------
    // VIEW 7: SOUND SUBMENU (4 Items on 1 Page)
    // -------------------------------------------------------------------------
    render_menu_header(it, cx, 50, "SOUND", font_date, theme_primary);
    snprintf(buf, sizeof(buf), "Speaker Vol: %d%%", spk_vol_pct);
    draw_slot_card(0, buf, theme_secondary, col_white, col_dark_gray);
    snprintf(buf, sizeof(buf), "Alert Vol: %d%%", alert_vol_pct);
    draw_slot_card(1, buf, theme_secondary, col_white, col_dark_gray);
    snprintf(buf, sizeof(buf), "Vol Step: %d%%", vol_step_pct);
    draw_slot_card(2, buf, theme_secondary, col_white, col_dark_gray);
    snprintf(buf, sizeof(buf), "Game Sound: %s", game_sound_on ? "On" : "Muted");
    draw_slot_card(3, buf, game_sound_on ? theme_secondary : col_slate,
                   game_sound_on ? col_white : col_slate, col_dark_gray);
    draw_back();

  } else if (system_menu_mode == 8) {
    // -------------------------------------------------------------------------
    // VIEW 8: SYSTEM INFO & OS DIAGNOSTICS
    // -------------------------------------------------------------------------
    render_menu_header(it, cx, 50, "SYSTEM INFO", font_date, theme_primary);
    const int info_x = 88;
    bool has_update = is_newer_semver(ha_fw_latest, version_str);

    if (has_update) {
      snprintf(buf, sizeof(buf), "Version: v%s (Update: v%s)", version_str, ha_fw_latest.c_str());
      it.print(info_x, 86, font_small, col_gold, esphome::display::TextAlign::LEFT, buf);
    } else {
      snprintf(buf, sizeof(buf), "Version: v%s (Up to date)", version_str);
      it.print(info_x, 86, font_small, col_white, esphome::display::TextAlign::LEFT, buf);
    }

    uint32_t up_sec = now_ms / 1000U;
    uint32_t up_d = up_sec / 86400U;
    uint32_t up_h = (up_sec % 86400U) / 3600U;
    uint32_t up_m = (up_sec % 3600U) / 60U;
    snprintf(buf, sizeof(buf), "Uptime: %lud %02luh %02lum",
             (unsigned long)up_d, (unsigned long)up_h, (unsigned long)up_m);
    it.print(info_x, 122, font_small, col_white, esphome::display::TextAlign::LEFT, buf);

    snprintf(buf, sizeof(buf), "Home Assistant: %s", ha_time_valid ? "Connected" : "Syncing...");
    it.print(info_x, 158, font_small, col_white, esphome::display::TextAlign::LEFT, buf);

    if (std::isnan(wifi_sig)) {
      snprintf(buf, sizeof(buf), "Wi-Fi: -- dBm  •  BLE: %s", ble_active ? "Active" : "Off");
    } else {
      snprintf(buf, sizeof(buf), "Wi-Fi: %.0f dBm  •  BLE: %s", wifi_sig, ble_active ? "Active" : "Off");
    }
    it.print(info_x, 194, font_small, col_white, esphome::display::TextAlign::LEFT, buf);

    snprintf(buf, sizeof(buf), "IP: %s", ip_str.empty() ? "Connecting..." : ip_str.c_str());
    it.print(info_x, 230, font_small, col_white, esphome::display::TextAlign::LEFT, buf);

    snprintf(buf, sizeof(buf), "Battery: %.0f%% (%.2fV)%s",
             std::isnan(batt) ? 100.0f : batt,
             std::isnan(batt_v) ? 4.15f : batt_v,
             is_charging ? " • Chg" : "");
    it.print(info_x, 266, font_small, col_white, esphome::display::TextAlign::LEFT, buf);

    float temp_c = std::isnan(imu_temp_c) ? 30.0f : imu_temp_c;
    float temp_f = temp_c * 1.8f + 32.0f;
    snprintf(buf, sizeof(buf), "Temp: %.1fC  (%.1fF)", temp_c, temp_f);
    it.print(info_x, 302, font_small, col_white, esphome::display::TextAlign::LEFT, buf);

    snprintf(buf, sizeof(buf), "ESP32-S3  |  %lu KB Free SRAM", (unsigned long)free_heap_kb);
    it.print(info_x, 338, font_small, col_white, esphome::display::TextAlign::LEFT, buf);

    draw_back();

  } else if (system_menu_mode == 9) {
    // -------------------------------------------------------------------------
    // VIEW 9: MUSIC SETTINGS SUBMENU
    // -------------------------------------------------------------------------
    render_menu_header(it, cx, 50, "MUSIC", font_date, theme_primary);
    snprintf(buf, sizeof(buf), "Art: %s", art_mode_opt.c_str());
    draw_slot_card(0, buf, theme_secondary, col_white, col_dark_gray);
    snprintf(buf, sizeof(buf), "Overlay: %s", overlay_style_opt.c_str());
    draw_slot_card(1, buf, theme_secondary, col_white, col_dark_gray);
    snprintf(buf, sizeof(buf), "Album Sort: %s", album_sort_opt.c_str());
    draw_slot_card(2, buf, theme_secondary, col_white, col_dark_gray);
    snprintf(buf, sizeof(buf), "Artist Discog: %s", artist_sort_opt.c_str());
    draw_slot_card(3, buf, theme_secondary, col_white, col_dark_gray);
    snprintf(buf, sizeof(buf), "Podcast Sort: %s", podcast_sort_opt.c_str());
    draw_slot_card(4, buf, theme_secondary, col_white, col_dark_gray);
    draw_back();

  } else if (system_menu_mode == 11) {
    // -------------------------------------------------------------------------
    // VIEW 11: VOICE SUBMENU (5 Voice Assistant & Wake Word Settings on 1 Page)
    // -------------------------------------------------------------------------
    render_menu_header(it, cx, 50, "VOICE", font_date, theme_primary);
    snprintf(buf, sizeof(buf), "Wake Mode: %s", va_ww_mode_opt.c_str());
    draw_slot_card(0, buf, theme_secondary, col_white, col_dark_gray);
    const bool ww_en = (va_ww_mode_opt != "Disabled");
    snprintf(buf, sizeof(buf), "Wake Word: %s", va_ww_active_opt.empty() ? "Okay Nabu" : va_ww_active_opt.c_str());
    draw_slot_card(1, buf, ww_en ? theme_secondary : col_slate,
                   ww_en ? col_white : col_slate, col_dark_gray);
    draw_slot_card(2, "Sensitivity (Per Model) >", ww_en ? theme_secondary : col_slate,
                   ww_en ? col_white : col_slate, col_dark_gray);
    snprintf(buf, sizeof(buf), "Profile: %s", va_profile_opt.c_str());
    draw_slot_card(3, buf, theme_secondary, col_white, col_dark_gray);
    snprintf(buf, sizeof(buf), "Wake Screen: %s", va_wake_scr ? "ON" : "OFF");
    draw_slot_card(4, buf, theme_secondary, col_white, col_dark_gray);
    draw_back();

  } else if (system_menu_mode == 12) {
    // -------------------------------------------------------------------------
    // VIEW 12: PER-MODEL WAKE WORD SENSITIVITY SUBMENU
    // -------------------------------------------------------------------------
    render_menu_header(it, cx, 50, "SENSITIVITY", font_date, theme_primary);
    auto short_sens = [](const std::string &s) -> const char * {
      if (s == "Slightly sensitive") return "Low";
      if (s == "Very sensitive") return "High";
      return "Medium";
    };
    snprintf(buf, sizeof(buf), "Okay Nabu: %s", short_sens(sens_nabu_opt));
    draw_slot_card(0, buf, theme_secondary, col_white, col_dark_gray);
    snprintf(buf, sizeof(buf), "Hey Luna: %s", short_sens(sens_luna_opt));
    draw_slot_card(1, buf, theme_secondary, col_white, col_dark_gray);
    snprintf(buf, sizeof(buf), "Hey Jarvis: %s", short_sens(sens_jarvis_opt));
    draw_slot_card(2, buf, theme_secondary, col_white, col_dark_gray);
    snprintf(buf, sizeof(buf), "Hey Mycroft: %s", short_sens(sens_mycroft_opt));
    draw_slot_card(3, buf, theme_secondary, col_white, col_dark_gray);
    snprintf(buf, sizeof(buf), "Alexa: %s", short_sens(sens_alexa_opt));
    draw_slot_card(4, buf, theme_secondary, col_white, col_dark_gray);
    draw_back();
  }
}


// ==============================================================================

}  // namespace pocket_ui