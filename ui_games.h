#pragma once

// ==============================================================================
// Pocket Assistant 20 FPS 5-Game Arcade Suite & 360-Deg IMU Engine (ui_games.h)
// ==============================================================================

#include "ui_core.h"

namespace pocket_ui {

// ============================================================================
// SECTION 8: REUSABLE 3D GLASS MARBLE PRIMITIVE & ARCADE MARBLES ENGINE
// ============================================================================

template<typename DisplayT>
inline void render_glass_marble(DisplayT &it, int bx, int by, int br,
                                esphome::Color base_col, esphome::Color hi_col,
                                int sh_x = 2, int sh_y = 3) {
  if (br <= 1) return;
  // 1. Tilt-reactive drop shadow
  if (br >= 4) {
    it.filled_circle(bx + sh_x, by + sh_y, br, esphome::Color(10, 14, 20));
  }
  // 2. Translucent glass sphere body
  it.filled_circle(bx, by, br, base_col);
  // 3. Outer dark glass rim & inner shaded refraction ring
  it.circle(bx, by, br, esphome::Color(14, 18, 24));
  if (br >= 6) {
    esphome::Color rim_shade(static_cast<uint8_t>((base_col.r * 170) >> 8),
                             static_cast<uint8_t>((base_col.g * 170) >> 8),
                             static_cast<uint8_t>((base_col.b * 170) >> 8));
    it.circle(bx, by, br - 1, rim_shade);
  }
  // 4. Inner Cat's-Eye ribbon swirl
  if (br >= 8) {
    it.circle(bx - 1, by - 1, br / 2, hi_col);
    it.line(bx - br / 3, by + br / 4, bx + br / 3, by - br / 4, hi_col);
  }
  // 5. Primary upper-left specular window glint & lower-right internal caustic
  it.filled_circle(bx - (br / 3), by - (br / 3), std::max(1, br / 4), esphome::Color(255, 255, 255));
  if (br >= 10) {
    it.filled_circle(bx + (br / 3), by + (br / 3), std::max(1, br / 6), hi_col);
  }
}

// ==============================================================================
// 3D ORTHONORMAL TANGENT-BASIS IMU CALIBRATION (GIMBAL-LOCK-FREE 360° TILT)
// ==============================================================================
// Replaces 1D scalar Euler subtraction (which hits a hard mathematical fold-back
// and sign-reversal singularity at 90.0° vertical when accel_z crosses 0.0g)
// with a true 3D orthonormal tangent frame (R_hat, P_hat, N_hat) anchored to the
// initial 3D gravity vector g0 = (ax0, ay0, az0) on the unit sphere S^2.
// Works identically whether starting flat (0°), handheld (45°), vertical (90°),
// laying on your back angled toward your face (95°..150°), or overhead (180°).
inline bool &game_imu_retare_flag() {
  static bool flag = false;
  return flag;
}

inline void compute_3d_game_tilt(
    float raw_ax, float raw_ay, float raw_az,
    float euler_roll_deg, float euler_pitch_deg,
    bool game_just_entered,
    float &out_roll_delta, float &out_pitch_delta) {
  constexpr float DEG_TO_RAD = 3.14159265f / 180.0f;
  constexpr float RAD_TO_DEG = 180.0f / 3.14159265f;

  float roll = std::isnan(euler_roll_deg) ? 0.0f : euler_roll_deg;
  float raw_pitch = std::isnan(euler_pitch_deg) ? -45.0f : euler_pitch_deg;

  // Derive clean_roll and clean_pitch strictly from ESPHome's native imu_roll and imu_pitch
  // (never raw_ax/raw_ay, whose physical QMI8658 chip axes are rotated 90° relative to roll/pitch):
  //   - When facing the sky (0°..85°), clean_roll == imu_roll and clean_pitch == imu_pitch (1:1 native).
  //   - When facing the ground (imu_roll near +/-180°), sin(r_rad) maps +/-180° -> 0°, +165° -> +15°,
  //     and -165° -> -15°, eliminating the +/-180° branch cut and sign reversal while keeping imu_pitch
  //     at its native [-90°, +90°] downhill angle.
  //   - At 90° vertical (imu_pitch = +/-90°), cos(p_rad) smoothly damps imu_roll's gimbal-lock spike.
  float r_rad = roll * DEG_TO_RAD;
  float p_rad = raw_pitch * DEG_TO_RAD;
  float gr = std::sin(r_rad) * std::cos(p_rad);
  if (gr > 1.0f) gr = 1.0f;
  if (gr < -1.0f) gr = -1.0f;
  float clean_roll = std::asin(gr) * RAD_TO_DEG;
  float clean_pitch = raw_pitch;

  float gp = std::sin(p_rad);
  float gz = std::cos(p_rad);
  if (!std::isnan(raw_ax) && !std::isnan(raw_ay) && !std::isnan(raw_az)) {
    float g_norm = sqrtf(raw_ax * raw_ax + raw_ay * raw_ay + raw_az * raw_az);
    if (g_norm > 0.05f) {
      gz = raw_az / g_norm;
    }
  }

  static float zero_roll = 0.0f;
  static float zero_pitch = -45.0f;
  static float zero_gp = -0.7071f;
  static float zero_gz = 0.7071f;
  static bool vertical_pitch_mode = false;
  static bool tare_initialized = false;

  if (!tare_initialized || game_just_entered || game_imu_retare_flag()) {
    tare_initialized = true;
    game_imu_retare_flag() = false;
    zero_roll = clean_roll;
    zero_pitch = clean_pitch;
    float pz_norm = sqrtf(gp * gp + gz * gz);
    if (pz_norm > 0.05f) {
      zero_gp = gp / pz_norm;
      zero_gz = gz / pz_norm;
    } else {
      zero_gp = gp;
      zero_gz = gz;
    }
    // Only use 2D sagittal circle unfolding when tared near 90° vertical (|gz| < 0.38, ~68°..112°)
    vertical_pitch_mode = (std::fabs(gz) < 0.38f);
  }

  out_roll_delta = clean_roll - zero_roll;
  if (vertical_pitch_mode) {
    float cross_pz = gp * zero_gz - gz * zero_gp;
    float dot_pz = gp * zero_gp + gz * zero_gz;
    out_pitch_delta = std::atan2(cross_pz, dot_pz) * RAD_TO_DEG;
  } else {
    out_pitch_delta = clean_pitch - zero_pitch;
  }
}

// Shared non-blocking arcade sound effect trigger (0 = none, 1..4 = chomp, 5 = level/wave clear,
// 6 = marble sink, 7 = bumper peg, 8 = bow release, 9 = bullseye, 10 = arrow hit,
// 11 = paddle bounce, 12 = brick break, 13 = armor hit, 14 = ball lost / miss)
inline int &pending_game_sfx() {
  static int sfx = 0;
  return sfx;
}

template<typename DisplayT, typename FontBigT, typename FontSmallT>
inline void render_game_marbles(DisplayT &it, int cx, int cy, uint32_t now_ms,
                                bool game_just_entered, float roll_delta, float pitch_delta,
                                float &marble_touch_x, float &marble_touch_y,
                                bool &game_btn_pressed, int &high_score_marbles,
                                FontBigT *font_date, FontSmallT *font_small,
                                esphome::Color theme_primary, esphome::Color theme_secondary,
                                esphome::Color col_bg, esphome::Color col_white,
                                esphome::Color col_gold, esphome::Color col_cyan,
                                esphome::Color col_green, esphome::Color col_red,
                                esphome::Color /*col_orange*/, esphome::Color col_magenta,
                                esphome::Color col_slate, esphome::Color col_gray,
                                esphome::Color col_dark_gray) {
  struct SimBall {
    float x, y, vx, vy, r;
    int col_idx;
    bool sunk;
    uint32_t sink_start_ms;
    float sink_x, sink_y;
  };

  static SimBall balls[5];
  static uint32_t last_phys_ms = 0;
  static uint32_t last_sink_ms = 0;
  static uint32_t wave_clear_ms = 0;
  static uint32_t popup_ms = 0;
  static int popup_pts = 0;
  static int popup_combo = 1;
  static int combo_streak = 0;
  static int wave_num = 1;
  static float time_left_f = 45.0f;
  static int game_score = 0;
  static bool round_over = false;
  static bool is_new_high = false;
  static float shockwave_r = -1.0f;
  static float shockwave_x = 233.0f;
  static float shockwave_y = 233.0f;
  static uint32_t peg_hit_ms[4] = {0, 0, 0, 0};

  auto spawn_wave_balls = [&](int w_idx) {
    static constexpr float RADII[5] = {20.0f, 18.0f, 16.0f, 14.0f, 12.0f};
    // Spawn on an outer ring (dist 108..134px) at golden-angle offsets so balls never overlap the 54px diamond pegs
    float base_ang = static_cast<float>(w_idx - 1) * 0.65f;
    for (int i = 0; i < 5; i++) {
      float ang = base_ang + static_cast<float>(i) * 1.256637f; // 72 deg spacing
      float s_a = 0.0f, c_a = 1.0f;
      fast_sincos(ang, s_a, c_a);
      float dist = 112.0f + static_cast<float>((i & 1) * 16);
      balls[i] = {static_cast<float>(cx) + c_a * dist,
                  static_cast<float>(cy) + s_a * dist,
                  0.0f, 0.0f, RADII[i], i, false, 0U, 0.0f, 0.0f};
    }
  };

  auto reset_full_game = [&]() {
    wave_num = 1;
    time_left_f = 45.0f;
    last_phys_ms = now_ms;
    last_sink_ms = 0;
    wave_clear_ms = 0;
    popup_ms = 0;
    combo_streak = 0;
    game_score = 0;
    round_over = false;
    is_new_high = false;
    shockwave_r = -1.0f;
    for (int p = 0; p < 4; p++) peg_hit_ms[p] = 0;
    spawn_wave_balls(1);
  };

  if (game_just_entered) {
    reset_full_game();
  }

  // Real elapsed dt (clamped to [0.015s .. 0.075s] so touch/timer coalescing never double-steps physics)
  float dt = (last_phys_ms == 0U) ? 0.05f : static_cast<float>(now_ms - last_phys_ms) * 0.001f;
  last_phys_ms = now_ms;
  if (dt < 0.012f) dt = 0.012f;
  if (dt > 0.075f) dt = 0.075f;

  const bool in_wave_pause = (wave_clear_ms > 0U && (now_ms - wave_clear_ms < 3000U));
  if (wave_clear_ms > 0U && (now_ms - wave_clear_ms >= 3000U)) {
    wave_clear_ms = 0U;
    spawn_wave_balls(wave_num);
  }

  if (!round_over && !in_wave_pause) {
    time_left_f -= dt;
    if (time_left_f <= 0.0f) {
      time_left_f = 0.0f;
      round_over = true;
      if (game_score > high_score_marbles) {
        high_score_marbles = game_score;
        is_new_high = true;
      }
    }
  }
  int time_remaining = static_cast<int>(ceilf(time_left_f));

  // Compute active Diamond Bumpers around the center hole:
  // Wave 1: 0 pegs (open bowl)
  // Wave 2: 1 peg  (Top)
  // Wave 3: 2 pegs (Top, Bottom)
  // Wave 4: 3 pegs (Top, Bottom, Left)
  // Wave 5+: 4 pegs (Full Diamond: Top, Bottom, Left, Right; gently orbits on Wave 6+)
  const int num_pegs = std::min(4, std::max(0, wave_num - 1));
  const float peg_ring_r = 54.0f;
  const float peg_r = 6.0f;
  static constexpr float PEG_BASE_ANG[4] = {
      -1.5707963f, // Top (12 o'clock)
       1.5707963f, // Bottom (6 o'clock)
       3.1415926f, // Left (9 o'clock)
       0.0f        // Right (3 o'clock)
  };
  float peg_x[4] = {0, 0, 0, 0};
  float peg_y[4] = {0, 0, 0, 0};
  float orbit_offset = (wave_num >= 6)
      ? (static_cast<float>(now_ms % 60000U) * 0.001f * 0.30f * static_cast<float>(std::min(4, wave_num - 5)))
      : 0.0f;
  for (int p = 0; p < num_pegs; p++) {
    float s_p = 0.0f, c_p = 1.0f;
    fast_sincos(PEG_BASE_ANG[p] + orbit_offset, s_p, c_p);
    peg_x[p] = static_cast<float>(cx) + c_p * peg_ring_r;
    peg_y[p] = static_cast<float>(cy) + s_p * peg_ring_r;
  }

  // Touch Scatter Impulse / Button Pop
  if (marble_touch_x >= 0.0f || game_btn_pressed) {
    float tx = (marble_touch_x >= 0.0f) ? marble_touch_x : static_cast<float>(cx);
    float ty = (marble_touch_y >= 0.0f) ? marble_touch_y : static_cast<float>(cy);
    marble_touch_x = -1.0f;
    game_btn_pressed = false;

    if (round_over) {
      reset_full_game();
      game_imu_retare_flag() = true;
    } else {
      shockwave_x = tx;
      shockwave_y = ty;
      shockwave_r = 12.0f;
      for (int i = 0; i < 5; i++) {
        if (balls[i].sunk) continue;
        float tdx = balls[i].x - tx;
        float tdy = balls[i].y - ty;
        float tdist = sqrtf(tdx * tdx + tdy * tdy);
        // Scale impulse slightly by inverse radius so lighter marbles pop livelier
        float mass_scale = 16.0f / balls[i].r;
        if (tdist < 22.0f) {
          float rnd_angle = static_cast<float>(i * 72 + (now_ms % 360U)) * (PI_F / 180.0f);
          float s_rnd = 0.0f, c_rnd = 1.0f;
          fast_sincos(rnd_angle, s_rnd, c_rnd);
          balls[i].vx += c_rnd * 620.0f * mass_scale;
          balls[i].vy += s_rnd * 620.0f * mass_scale;
        } else {
          float blast = (480.0f + (680.0f / (tdist * 0.08f + 1.0f))) * mass_scale;
          if (blast > 820.0f) blast = 820.0f;
          balls[i].vx += (tdx / tdist) * blast;
          balls[i].vy += (tdy / tdist) * blast;
        }
      }
    }
  }

  // Physics Step:
  //   - Lively glass-marble momentum (0.975 per 50ms step: fast & slippery enough to get away from you,
  //     just below 0.985 so they don't endlessly ice-skate)
  //   - Crisp tilt acceleration (34.0 px/s^2 per deg)
  const float friction = powf(0.975f, dt / 0.05f);
  const float bounce = 0.82f;
  const float arena_r = 208.0f;

  float ax = roll_delta * 34.0f;
  float ay = -pitch_delta * 34.0f;
  if (ax > 650.0f) ax = 650.0f; else if (ax < -650.0f) ax = -650.0f;
  if (ay > 650.0f) ay = 650.0f; else if (ay < -650.0f) ay = -650.0f;

  int sunk_count = 0;
  if (!round_over) {
    for (int i = 0; i < 5; i++) {
      if (balls[i].sunk) {
        sunk_count++;
        continue;
      }

      float dx = balls[i].x - static_cast<float>(cx);
      float dy = balls[i].y - static_cast<float>(cy);
      float dist_hole = sqrtf(dx * dx + dy * dy);

      // Center hole capture rim (r = 24px)
      if (dist_hole < 24.0f) {
        balls[i].sunk = true;
        balls[i].sink_start_ms = now_ms;
        balls[i].sink_x = balls[i].x;
        balls[i].sink_y = balls[i].y;
        balls[i].vx = 0.0f;
        balls[i].vy = 0.0f;
        sunk_count++;

        if (last_sink_ms > 0U && (now_ms - last_sink_ms < 3500U)) {
          combo_streak = std::min(4, combo_streak + 1);
        } else {
          combo_streak = 1;
        }
        last_sink_ms = now_ms;
        int pts = 200 + (combo_streak - 1) * 150;
        game_score += pts;
        popup_pts = pts;
        popup_combo = combo_streak;
        popup_ms = now_ms;
        pending_game_sfx() = 6; // Marble Sunk Chime
        continue;
      }

      // Flat free-rolling arena table (r = 42..180px) so marbles carry real speed and can get away
      // from you, with:
      //   1. Tight hole lip bevel (24..42px) so you must aim onto the rim to sink a marble
      //   2. Subtle outer-bezel lip (dist > 180px) so marbles don't get trapped against the wall
      if (dist_hole < 42.0f && dist_hole >= 24.0f) {
        float bevel_pull = (42.0f - dist_hole) * 9.5f;
        balls[i].vx -= (dx / dist_hole) * bevel_pull * dt;
        balls[i].vy -= (dy / dist_hole) * bevel_pull * dt;
      } else if (dist_hole > 180.0f) {
        float rim_nudge = (dist_hole - 180.0f) * 1.15f;
        balls[i].vx -= (dx / dist_hole) * rim_nudge * dt;
        balls[i].vy -= (dy / dist_hole) * rim_nudge * dt;
      }

      // Slight rolling-inertia differentiation by size (smaller marbles accelerate slightly faster)
      float inertia_scale = 0.92f + (20.0f - balls[i].r) * 0.02f;
      balls[i].vx = (balls[i].vx + ax * inertia_scale * dt) * friction;
      balls[i].vy = (balls[i].vy + ay * inertia_scale * dt) * friction;
      balls[i].x += balls[i].vx * dt;
      balls[i].y += balls[i].vy * dt;

      // Collide with active Diamond Bumper dots (r = 6)
      for (int p = 0; p < num_pegs; p++) {
        float pdx = balls[i].x - peg_x[p];
        float pdy = balls[i].y - peg_y[p];
        float pdist2 = pdx * pdx + pdy * pdy;
        float min_pdist = balls[i].r + peg_r;
        if (pdist2 < min_pdist * min_pdist && pdist2 > 0.0001f) {
          float pdist = sqrtf(pdist2);
          float pnx = pdx / pdist;
          float pny = pdy / pdist;
          balls[i].x = peg_x[p] + pnx * min_pdist;
          balls[i].y = peg_y[p] + pny * min_pdist;
          float v_dot_n = balls[i].vx * pnx + balls[i].vy * pny;
          if (v_dot_n < 0.0f) {
            balls[i].vx -= (1.0f + 0.88f) * v_dot_n * pnx;
            balls[i].vy -= (1.0f + 0.88f) * v_dot_n * pny;
          }
          // Pinball bumper kick + 200ms visual ring flash
          balls[i].vx += pnx * 38.0f;
          balls[i].vy += pny * 38.0f;
          if (now_ms - peg_hit_ms[p] > 180U) {
            game_score += 15;
            if (pending_game_sfx() == 0) pending_game_sfx() = 7; // Bumper Peg Ping
          }
          peg_hit_ms[p] = now_ms;
        }
      }

      // Horizontal bottom cushion wall at y = 374 (protects scorekeeper at y = 392 & < Back at y = 426!)
      float max_bot_y = 374.0f - balls[i].r;
      if (balls[i].y > max_bot_y) {
        balls[i].y = max_bot_y;
        if (balls[i].vy > 0.0f) {
          balls[i].vy = -balls[i].vy * bounce;
        }
      }

      // Outer circular arena boundary reflection (arena_r = 208) with light 7% tangential scrub
      float cur_dx = balls[i].x - static_cast<float>(cx);
      float cur_dy = balls[i].y - static_cast<float>(cy);
      float cur_dist = sqrtf(cur_dx * cur_dx + cur_dy * cur_dy);
      float max_dist = arena_r - balls[i].r;
      if (cur_dist > max_dist && cur_dist > 0.001f) {
        float nx = cur_dx / cur_dist;
        float ny = cur_dy / cur_dist;
        balls[i].x = static_cast<float>(cx) + nx * max_dist;
        balls[i].y = static_cast<float>(cy) + ny * max_dist;
        float v_dot_n = balls[i].vx * nx + balls[i].vy * ny;
        if (v_dot_n > 0.0f) {
          balls[i].vx -= (1.0f + bounce) * v_dot_n * nx;
          balls[i].vy -= (1.0f + bounce) * v_dot_n * ny;
        }
        float tx_w = -ny;
        float ty_w = nx;
        float v_dot_t = balls[i].vx * tx_w + balls[i].vy * ty_w;
        balls[i].vx -= 0.07f * v_dot_t * tx_w;
        balls[i].vy -= 0.07f * v_dot_t * ty_w;
      }
    }

    // Mass-weighted 2D elastic ball-to-ball collisions (m ~ r^2) with separating-velocity guard
    for (int i = 0; i < 5; i++) {
      if (balls[i].sunk) continue;
      for (int j = i + 1; j < 5; j++) {
        if (balls[j].sunk) continue;
        float cdx = balls[j].x - balls[i].x;
        float cdy = balls[j].y - balls[i].y;
        float min_dist = balls[i].r + balls[j].r;
        float cdist2 = cdx * cdx + cdy * cdy;
        if (cdist2 < min_dist * min_dist && cdist2 > 0.0001f) {
          float cdist = sqrtf(cdist2);
          float overlap = min_dist - cdist;
          float cnx = cdx / cdist;
          float cny = cdy / cdist;

          float mi = balls[i].r * balls[i].r;
          float mj = balls[j].r * balls[j].r;
          float inv_m_sum = 1.0f / (mi + mj);

          // Separate proportional to inverse mass
          balls[i].x -= cnx * overlap * (mj * inv_m_sum);
          balls[i].y -= cny * overlap * (mj * inv_m_sum);
          balls[j].x += cnx * overlap * (mi * inv_m_sum);
          balls[j].y += cny * overlap * (mi * inv_m_sum);

          // Relative velocity along collision normal
          float kx = balls[i].vx - balls[j].vx;
          float ky = balls[i].vy - balls[j].vy;
          float v_rel = cnx * kx + cny * ky;
          if (v_rel > 0.0f) {
            float imp = (1.0f + bounce) * v_rel * inv_m_sum;
            balls[i].vx -= imp * mj * cnx;
            balls[i].vy -= imp * mj * cny;
            balls[j].vx += imp * mi * cnx;
            balls[j].vy += imp * mi * cny;
          }
        }
      }
    }

    // Continuous Wave Progression with 1.5s Inter-Wave Anticipation Pause
    if (sunk_count >= 5 && wave_clear_ms == 0U) {
      bool any_animating = false;
      for (int i = 0; i < 5; i++) {
        if (now_ms - balls[i].sink_start_ms < 240U) any_animating = true;
      }
      if (!any_animating) {
        game_score += 300 + wave_num * 100;
        time_left_f = std::min(60.0f, time_left_f + 6.0f);
        time_remaining = static_cast<int>(ceilf(time_left_f));
        wave_num++;
        wave_clear_ms = now_ms;
        pending_game_sfx() = 5; // Wave Clear Fanfare!
        if (game_score > high_score_marbles) {
          high_score_marbles = game_score;
          is_new_high = true;
        }
      }
    }
  } else {
    for (int i = 0; i < 5; i++) {
      if (balls[i].sunk) sunk_count++;
    }
  }

  // ==========================================================================
  // RENDERING PIPELINE (Bottom Scorekeeper & Center Hole drawn first so marbles roll over)
  // ==========================================================================
  (void)theme_primary; (void)col_gray;
  // Subtle horizontal bottom cushion wall at y = 374 + Score at y = 392 + "< Back" at y = 426
  it.line(82, 374, 384, 374, col_slate * 0.45f);
  it.line(84, 375, 382, 375, col_dark_gray);
  char hud_buf[56];
  snprintf(hud_buf, sizeof(hud_buf), "W%d  •  %d/5  •  %ds  •  %d", wave_num, sunk_count, time_remaining, game_score);
  it.print(cx, 392, font_small, time_remaining < 10 ? col_red : col_gold, esphome::display::TextAlign::CENTER, hud_buf);
  render_back_button(it, cx, font_small, 426, "< Back");

  // Center Hole Target with Swirling Event Horizon
  it.circle(cx, cy, 34, col_dark_gray);
  it.circle(cx, cy, 30, col_slate);
  it.circle(cx, cy, 29, col_white);
  it.circle(cx, cy, 28, col_white);
  it.filled_circle(cx, cy, 25, col_bg);

  float swirl = static_cast<float>((now_ms / 25U) % 360U) * (PI_F / 180.0f);
  for (int s = 0; s < 4; s++) {
    float sa = swirl + static_cast<float>(s) * (PI_F * 0.5f);
    float s_sa = 0.0f, c_sa = 1.0f;
    fast_sincos(sa, s_sa, c_sa);
    it.line(cx + static_cast<int>(c_sa * 26.0f), cy + static_cast<int>(s_sa * 26.0f),
            cx + static_cast<int>(c_sa * 32.0f), cy + static_cast<int>(s_sa * 32.0f), col_slate);
  }

  // Render Diamond Bumper Dots (Wave 2..5+)
  for (int p = 0; p < num_pegs; p++) {
    int px = static_cast<int>(roundf(peg_x[p]));
    int py = static_cast<int>(roundf(peg_y[p]));
    bool peg_flashing = (peg_hit_ms[p] > 0U && (now_ms - peg_hit_ms[p] < 200U));
    if (peg_flashing) {
      it.circle(px, py, 10, col_gold);
      it.circle(px, py, 8, col_cyan);
    }
    it.filled_circle(px + 1, py + 2, 6, esphome::Color(10, 14, 20));
    it.filled_circle(px, py, 6, peg_flashing ? col_gold : col_slate);
    it.circle(px, py, 6, col_white);
    it.filled_circle(px - 1, py - 1, 2, peg_flashing ? col_white : col_cyan);
  }

  // Touch Pop Shockwave Ring
  if (shockwave_r >= 0.0f) {
    it.circle(static_cast<int>(shockwave_x), static_cast<int>(shockwave_y), static_cast<int>(shockwave_r), col_gold);
    it.circle(static_cast<int>(shockwave_x), static_cast<int>(shockwave_y), static_cast<int>(shockwave_r) + 2, col_cyan);
    shockwave_r += 26.0f;
    if (shockwave_r > 190.0f) shockwave_r = -1.0f;
  }

  // Render 3D Cat's-Eye Glass Marbles (including 240ms shrinking spiral-sink animation!)
  esphome::Color base_cols[5] = {col_gold, col_cyan, col_green, col_red, col_magenta};
  esphome::Color hi_cols[5] = {
      esphome::Color(255, 245, 157),
      esphome::Color(180, 255, 255),
      esphome::Color(185, 246, 202),
      esphome::Color(255, 138, 128),
      esphome::Color(248, 187, 208)
  };
  int sh_x = static_cast<int>(ax * 0.012f);
  int sh_y = static_cast<int>(ay * 0.012f);

  for (int i = 0; i < 5; i++) {
    if (balls[i].sunk) {
      uint32_t sink_el = now_ms - balls[i].sink_start_ms;
      if (sink_el < 240U) {
        float prog = static_cast<float>(sink_el) * (1.0f / 240.0f);
        int sx = static_cast<int>(balls[i].sink_x + (static_cast<float>(cx) - balls[i].sink_x) * prog);
        int sy = static_cast<int>(balls[i].sink_y + (static_cast<float>(cy) - balls[i].sink_y) * prog);
        int sr = std::max(3, static_cast<int>(balls[i].r * (1.0f - prog * 0.82f)));
        render_glass_marble(it, sx, sy, sr, base_cols[balls[i].col_idx], hi_cols[balls[i].col_idx], 0, 0);
      }
      continue;
    }
    render_glass_marble(it, static_cast<int>(balls[i].x), static_cast<int>(balls[i].y),
                        static_cast<int>(balls[i].r),
                        base_cols[balls[i].col_idx], hi_cols[balls[i].col_idx], sh_x, sh_y);
  }

  // Floating Sink Score / Combo Popup (+200, +350 2x!, etc.)
  if (!round_over && popup_ms > 0U && (now_ms - popup_ms < 650U)) {
    uint32_t pel = now_ms - popup_ms;
    int py = cy - 42 - static_cast<int>(pel / 22U);
    char pop_buf[24];
    if (popup_combo > 1) {
      snprintf(pop_buf, sizeof(pop_buf), "+%d %dx!", popup_pts, popup_combo);
    } else {
      snprintf(pop_buf, sizeof(pop_buf), "+%d", popup_pts);
    }
    it.print(cx, py, font_small, popup_combo > 1 ? col_cyan : col_gold, esphome::display::TextAlign::CENTER, pop_buf);
  }

  // 1.5s Inter-Wave Anticipation Banner ("WAVE X CLEAR! +6s" -> "GET READY! WAVE X+1")
  if (!round_over && in_wave_pause) {
    char w_buf[36];
    if (now_ms - wave_clear_ms < 1100U) {
      snprintf(w_buf, sizeof(w_buf), "WAVE %d CLEAR! +6s", wave_num - 1);
      it.print(cx, 126, font_date, col_gold, esphome::display::TextAlign::CENTER, w_buf);
    } else {
      snprintf(w_buf, sizeof(w_buf), "GET READY! WAVE %d", wave_num);
      it.print(cx, 126, font_date, col_cyan, esphome::display::TextAlign::CENTER, w_buf);
    }
  }

  // Round Over Summary Modal
  if (round_over) {
    it.filled_circle(cx, cy, 115, col_bg);
    it.circle(cx, cy, 115, is_new_high ? col_gold : theme_secondary);
    it.circle(cx, cy, 113, is_new_high ? col_gold : theme_secondary);

    if (is_new_high) {
      it.print(cx, 156, font_date, col_gold, esphome::display::TextAlign::CENTER, "NEW HIGH SCORE!");
    } else {
      if (wave_num == 1) {
        it.print(cx, 156, font_date, col_red, esphome::display::TextAlign::CENTER, "GAME OVER");
      } else {
        char w_end[32];
        snprintf(w_end, sizeof(w_end), "WAVE %d REACHED", wave_num);
        it.print(cx, 156, font_date, col_cyan, esphome::display::TextAlign::CENTER, w_end);
      }
    }

    char score_str[32];
    snprintf(score_str, sizeof(score_str), "Score: %d", game_score);
    it.print(cx, 198, font_date, col_white, esphome::display::TextAlign::CENTER, score_str);

    char best_str[32];
    snprintf(best_str, sizeof(best_str), "Best: %d", high_score_marbles);
    it.print(cx, 233, font_small, col_gold, esphome::display::TextAlign::CENTER, best_str);

    it.print(cx, 275, font_small, col_cyan, esphome::display::TextAlign::CENTER, "[ TAP TO REPLAY ]");
  }
}


// ============================================================================
// SECTION 9: REUSABLE 5-RING ARCHERY TARGET & ARCADE ARCHERY ENGINE
// ============================================================================

template<typename DisplayT>
inline void render_archery_target(DisplayT &it, int tx, int ty, int r_outer,
                                  esphome::Color col_white, esphome::Color col_black,
                                  esphome::Color col_cyan, esphome::Color col_red,
                                  esphome::Color col_gold, esphome::Color col_slate,
                                  esphome::Color col_dark_gray) {
  if (r_outer <= 4) return;
  int r_black = (r_outer * 76) / 100;
  int r_cyan  = (r_outer * 54) / 100;
  int r_red   = (r_outer * 34) / 100;
  int r_gold  = std::max(3, (r_outer * 16) / 100);

  // Drop shadow
  it.filled_circle(tx, ty + 3, r_outer, col_dark_gray);
  // 1. White outer ring
  it.filled_circle(tx, ty, r_outer, col_white);
  it.circle(tx, ty, r_outer, col_dark_gray);
  // 2. Black ring
  it.filled_circle(tx, ty, r_black, col_black);
  it.circle(tx, ty, r_black, col_slate);
  // 3. Cyan/Blue ring
  it.filled_circle(tx, ty, r_cyan, col_cyan);
  it.circle(tx, ty, r_cyan, esphome::Color(0, 175, 205));
  // 4. Red ring
  it.filled_circle(tx, ty, r_red, col_red);
  it.circle(tx, ty, r_red, esphome::Color(180, 20, 20));
  // 5. Gold bullseye + center pin
  it.filled_circle(tx, ty, r_gold, col_gold);
  it.draw_pixel_at(tx, ty, col_red);
}

template<typename DisplayT, typename FontBigT, typename FontSmallT>
inline void render_game_archery(DisplayT &it, int cx, int cy, uint32_t now_ms,
                                bool game_just_entered, float roll_delta, float pitch_delta,
                                float &marble_touch_x, bool &game_btn_pressed,
                                int &high_score_archery,
                                FontBigT *font_date, FontSmallT *font_small,
                                esphome::Color theme_secondary,
                                esphome::Color col_bg, esphome::Color col_white,
                                esphome::Color col_black, esphome::Color col_gold,
                                esphome::Color col_cyan, esphome::Color col_red,
                                esphome::Color col_slate, esphome::Color col_dark_gray) {
  struct PinnedArrow {
    float dx, dy;
    bool active;
  };

  static uint32_t last_phys_ms = 0;
  static int round_num = 1;
  static int arrows_left = 6;
  static int round_pts = 0;
  static int arch_score = 0;
  static int streak_mult = 1;
  static float target_phase = 0.0f;
  static float smooth_ax = 233.0f;
  static float smooth_ay = 112.0f;
  static bool arrow_in_flight = false;
  static float flight_z = 0.0f;
  static float launch_aim_x = 233.0f;
  static float launch_aim_y = 112.0f;
  static float launch_wind = 0.0f;
  static float wind_speed = 0.0f;
  static char hit_feedback[36] = "";
  static esphome::Color feedback_col(255, 215, 0);
  static uint32_t feedback_timer = 0;
  static uint32_t round_banner_ms = 0;
  static bool round_over = false;
  static bool is_new_high = false;
  static PinnedArrow pinned[10];
  static int pinned_count = 0;

  auto clear_pinned = [&]() {
    pinned_count = 0;
    for (int i = 0; i < 10; i++) pinned[i].active = false;
  };

  auto reset_full_game = [&]() {
    last_phys_ms = now_ms;
    round_num = 1;
    arrows_left = 6;
    round_pts = 0;
    arch_score = 0;
    streak_mult = 1;
    target_phase = 0.0f;
    smooth_ax = static_cast<float>(cx);
    smooth_ay = 112.0f;
    arrow_in_flight = false;
    flight_z = 0.0f;
    hit_feedback[0] = '\0';
    feedback_timer = 0;
    round_banner_ms = 0;
    round_over = false;
    is_new_high = false;
    clear_pinned();
  };

  if (game_just_entered) {
    reset_full_game();
  }

  float dt = (last_phys_ms == 0U) ? 0.05f : static_cast<float>(now_ms - last_phys_ms) * 0.001f;
  last_phys_ms = now_ms;
  if (dt < 0.012f) dt = 0.012f;
  if (dt > 0.075f) dt = 0.075f;

  // Target size & sway scale with round_num (simulating 30m -> 50m -> 70m -> 90m range!)
  const int target_r = std::max(26, 36 - (round_num - 1) * 2);
  const float sway_spd = 0.78f + static_cast<float>(std::min(6, round_num - 1)) * 0.16f;
  target_phase += sway_spd * dt;

  const float target_base_y = 112.0f;
  const float bow_y = 346.0f;
  float target_x = static_cast<float>(cx) + fast_sin(target_phase) * 102.0f;
  float target_y = target_base_y + fast_cos(target_phase * 1.4f) * 24.0f;

  // Crosswind scales smoothly with round_num (gentle in Round 1, brisk in Round 3+)
  float max_wind = std::min(8.0f, 2.5f + static_cast<float>(round_num - 1) * 1.3f);
  wind_speed = fast_sin(static_cast<float>(now_ms % 62832U) * 0.00065f) * max_wind;

  // EMA-smoothed Player Aim Reticle (centered around target_base_y = 112 so neutral posture aims at target level)
  float raw_ax = static_cast<float>(cx) + roll_delta * 6.8f;
  float raw_ay = target_base_y - pitch_delta * 6.8f;
  if (raw_ax < 60.0f) raw_ax = 60.0f; else if (raw_ax > 406.0f) raw_ax = 406.0f;
  if (raw_ay < 48.0f) raw_ay = 48.0f; else if (raw_ay > 280.0f) raw_ay = 280.0f;
  smooth_ax += (raw_ax - smooth_ax) * 0.45f;
  smooth_ay += (raw_ay - smooth_ay) * 0.45f;

  const int round_goal = 20 + std::min(5, round_num - 1) * 4;

  // Shoot Trigger (Button or Tap)
  if (game_btn_pressed || marble_touch_x >= 0.0f) {
    game_btn_pressed = false;
    marble_touch_x = -1.0f;

    if (round_over) {
      reset_full_game();
      game_imu_retare_flag() = true;
    } else if (!arrow_in_flight && arrows_left > 0 && (round_banner_ms == 0U || now_ms - round_banner_ms >= 3000U)) {
      arrow_in_flight = true;
      flight_z = 0.0f;
      launch_aim_x = smooth_ax;
      launch_aim_y = smooth_ay;
      launch_wind = wind_speed;
      arrows_left--;
      pending_game_sfx() = 8; // Bowstring Release Zip
    }
  }

  // True 3D Depth-Plane Ballistic Arrow Flight (z = 0.0 at bow -> 1.0 at target plane over 0.42s)
  float cur_arrow_x = static_cast<float>(cx);
  float cur_arrow_y = bow_y;
  float arrow_dx = 0.0f;
  float arrow_dy = -1.0f;

  if (arrow_in_flight) {
    flight_z += dt * (1.0f / 0.40f);
    float z = std::min(1.0f, flight_z);
    float wind_drift = launch_wind * 5.2f * z * z;
    float arc_lift = -26.0f * z * (1.0f - z);

    cur_arrow_x = static_cast<float>(cx) + (launch_aim_x - static_cast<float>(cx)) * z + wind_drift;
    cur_arrow_y = bow_y + (launch_aim_y - bow_y) * z + arc_lift;

    // Instantaneous tangent vector for angled arrow rendering
    float vx = (launch_aim_x - static_cast<float>(cx)) + launch_wind * 10.4f * z;
    float vy = (launch_aim_y - bow_y) - 26.0f * (1.0f - 2.0f * z);
    float vlen = sqrtf(vx * vx + vy * vy);
    if (vlen > 0.001f) {
      arrow_dx = vx / vlen;
      arrow_dy = vy / vlen;
    }

    if (flight_z >= 1.0f) {
      arrow_in_flight = false;
      float hit_dx = cur_arrow_x - target_x;
      float hit_dy = cur_arrow_y - target_y;
      float dist_hit = sqrtf(hit_dx * hit_dx + hit_dy * hit_dy);

      float r_out = static_cast<float>(target_r);
      int base_pts = 0;
      bool bullseye = false;
      if (dist_hit <= r_out * 0.18f) {
        base_pts = 10;
        bullseye = true;
      } else if (dist_hit <= r_out * 0.36f) {
        base_pts = 7;
      } else if (dist_hit <= r_out * 0.56f) {
        base_pts = 5;
      } else if (dist_hit <= r_out * 0.78f) {
        base_pts = 3;
      } else if (dist_hit <= r_out + 1.5f) {
        base_pts = 1;
      }

      if (base_pts > 0) {
        int earned = base_pts * streak_mult;
        arch_score += earned;
        round_pts += earned;

        // Pin arrow into the target board so it sways with the target!
        if (pinned_count < 10) {
          pinned[pinned_count++] = {hit_dx, hit_dy, true};
        }

        if (bullseye) {
          arrows_left = std::min(9, arrows_left + 1);
          snprintf(hit_feedback, sizeof(hit_feedback), "BULLSEYE! +%d (+1 ARR)", earned);
          feedback_col = col_gold;
          pending_game_sfx() = 9; // Bullseye Chime!
        } else if (streak_mult > 1) {
          snprintf(hit_feedback, sizeof(hit_feedback), "HIT +%d (%dx!)", earned, streak_mult);
          feedback_col = col_cyan;
          pending_game_sfx() = 10; // Target Hit
        } else {
          snprintf(hit_feedback, sizeof(hit_feedback), "HIT +%d", earned);
          feedback_col = col_gold;
          pending_game_sfx() = 10; // Target Hit
        }
        streak_mult = std::min(3, streak_mult + 1);
      } else {
        streak_mult = 1;
        snprintf(hit_feedback, sizeof(hit_feedback), "MISS!");
        feedback_col = col_red;
        pending_game_sfx() = 14; // Miss Tone
      }
      feedback_timer = now_ms;

      if (arch_score > high_score_archery) {
        high_score_archery = arch_score;
        is_new_high = true;
      }

      // Check Round Advancement or Match Over when quiver empties (with 1.5s Inter-Round Pause!)
      if (arrows_left <= 0) {
        if (round_pts >= round_goal) {
          arch_score += round_num * 25;
          if (arch_score > high_score_archery) {
            high_score_archery = arch_score;
            is_new_high = true;
          }
          round_num++;
          round_pts = 0;
          round_banner_ms = now_ms;
          pending_game_sfx() = 5; // Round Clear Fanfare!
        } else {
          round_over = true;
        }
      }
    }
  }

  // ==========================================================================
  // RENDERING PIPELINE
  // ==========================================================================
  // 1. 5-Ring Swaying Target
  int tx_i = static_cast<int>(roundf(target_x));
  int ty_i = static_cast<int>(roundf(target_y));
  render_archery_target(it, tx_i, ty_i, target_r,
                        col_white, col_black, col_cyan, col_red, col_gold, col_slate, col_dark_gray);

  // 2. Pinned Arrows sticking out of the swaying target board
  for (int i = 0; i < pinned_count; i++) {
    if (!pinned[i].active) continue;
    int px = tx_i + static_cast<int>(roundf(pinned[i].dx));
    int py = ty_i + static_cast<int>(roundf(pinned[i].dy));
    it.filled_circle(px, py, 2, col_black);
    it.line(px, py, px - 5, py + 11, col_gold);
    it.line(px - 5, py + 11, px - 8, py + 10, col_red);
    it.line(px - 5, py + 11, px - 3, py + 13, col_red);
  }

  // 3. Player Aim Reticle
  int rx_i = static_cast<int>(roundf(smooth_ax));
  int ry_i = static_cast<int>(roundf(smooth_ay));
  it.circle(rx_i, ry_i, 13, col_cyan);
  it.line(rx_i - 17, ry_i, rx_i - 5, ry_i, col_cyan);
  it.line(rx_i + 5, ry_i, rx_i + 17, ry_i, col_cyan);
  it.line(rx_i, ry_i - 17, rx_i, ry_i - 5, col_cyan);
  it.line(rx_i, ry_i + 5, rx_i, ry_i + 17, col_cyan);
  it.draw_pixel_at(rx_i, ry_i, col_gold);

  // 4. Recurve Bow at bottom of range (y = 356) angled toward crosshair
  float aim_dx = smooth_ax - static_cast<float>(cx);
  float aim_dy = smooth_ay - bow_y;
  float aim_len = sqrtf(aim_dx * aim_dx + aim_dy * aim_dy);
  float ux = (aim_len > 1.0f) ? (aim_dx / aim_len) : 0.0f;
  float uy = (aim_len > 1.0f) ? (aim_dy / aim_len) : -1.0f;
  // Perpendicular vector (px_v, py_v) for bow limbs
  float px_v = -uy;
  float py_v = ux;

  int bow_cx = cx;
  int bow_cy = static_cast<int>(bow_y);
  int left_tip_x  = bow_cx - static_cast<int>(px_v * 38.0f + ux * 10.0f);
  int left_tip_y  = bow_cy - static_cast<int>(py_v * 38.0f + uy * 10.0f);
  int right_tip_x = bow_cx + static_cast<int>(px_v * 38.0f - ux * 10.0f);
  int right_tip_y = bow_cy + static_cast<int>(py_v * 38.0f - uy * 10.0f);
  int left_mid_x  = bow_cx - static_cast<int>(px_v * 20.0f - ux * 4.0f);
  int left_mid_y  = bow_cy - static_cast<int>(py_v * 20.0f - uy * 4.0f);
  int right_mid_x = bow_cx + static_cast<int>(px_v * 20.0f + ux * 4.0f);
  int right_mid_y = bow_cy + static_cast<int>(py_v * 20.0f + uy * 4.0f);

  // Bow limbs (gold + slate double-stroke arch)
  it.line(bow_cx, bow_cy, left_mid_x, left_mid_y, col_gold);
  it.line(left_mid_x, left_mid_y, left_tip_x, left_tip_y, col_gold);
  it.line(bow_cx, bow_cy, right_mid_x, right_mid_y, col_gold);
  it.line(right_mid_x, right_mid_y, right_tip_x, right_tip_y, col_gold);
  it.line(bow_cx, bow_cy + 1, left_mid_x, left_mid_y + 1, col_slate);
  it.line(bow_cx, bow_cy + 1, right_mid_x, right_mid_y + 1, col_slate);
  it.filled_circle(bow_cx, bow_cy, 3, col_red); // Riser grip

  if (!arrow_in_flight && arrows_left > 0 && !round_over) {
    // Drawn bowstring + nocked arrow ready to fire
    int nock_x = bow_cx - static_cast<int>(ux * 14.0f);
    int nock_y = bow_cy - static_cast<int>(uy * 14.0f);
    int tip_x  = bow_cx + static_cast<int>(ux * 18.0f);
    int tip_y  = bow_cy + static_cast<int>(uy * 18.0f);
    it.line(left_tip_x, left_tip_y, nock_x, nock_y, col_slate);
    it.line(right_tip_x, right_tip_y, nock_x, nock_y, col_slate);
    it.line(nock_x, nock_y, tip_x, tip_y, col_white);
    it.filled_circle(tip_x, tip_y, 2, col_gold);
  } else {
    // Released taut bowstring
    it.line(left_tip_x, left_tip_y, right_tip_x, right_tip_y, col_slate);
  }

  // 5. In-Flight Perspective Arrow (angled along velocity vector with fletching)
  if (arrow_in_flight) {
    float z = std::min(1.0f, flight_z);
    float a_len = 32.0f - z * 16.0f;
    int head_x = static_cast<int>(roundf(cur_arrow_x));
    int head_y = static_cast<int>(roundf(cur_arrow_y));
    int tail_x = static_cast<int>(roundf(cur_arrow_x - arrow_dx * a_len));
    int tail_y = static_cast<int>(roundf(cur_arrow_y - arrow_dy * a_len));

    it.line(tail_x, tail_y, head_x, head_y, col_gold);
    it.line(tail_x + 1, tail_y, head_x + 1, head_y, col_white);
    it.filled_circle(head_x, head_y, z > 0.65f ? 2 : 3, col_white);
    // Crimson fletching feathers at tail
    int f_px = static_cast<int>(-arrow_dy * 4.0f);
    int f_py = static_cast<int>(arrow_dx * 4.0f);
    it.line(tail_x, tail_y, tail_x + f_px - static_cast<int>(arrow_dx * 4.0f),
            tail_y + f_py - static_cast<int>(arrow_dy * 4.0f), col_red);
    it.line(tail_x, tail_y, tail_x - f_px - static_cast<int>(arrow_dx * 4.0f),
            tail_y - f_py - static_cast<int>(arrow_dy * 4.0f), col_red);
  }

  // Clear pinned arrows at 1.1s for Get Ready phase, and unlock quiver at 3.0s
  if (round_banner_ms > 0U && (now_ms - round_banner_ms >= 1100U) && pinned_count > 0) {
    clear_pinned();
  }
  if (round_banner_ms > 0U && (now_ms - round_banner_ms >= 3000U)) {
    round_banner_ms = 0U;
    arrows_left = 6;
  }

  // 6. Hit Feedback & 3.0s Inter-Round Anticipation Banner
  if (!round_over && round_banner_ms > 0U && (now_ms - round_banner_ms < 3000U)) {
    char rb[36];
    if (now_ms - round_banner_ms < 1100U) {
      snprintf(rb, sizeof(rb), "ROUND %d CLEAR!", round_num - 1);
      it.print(cx, 225, font_date, col_gold, esphome::display::TextAlign::CENTER, rb);
    } else {
      snprintf(rb, sizeof(rb), "ROUND %d! GOAL %d", round_num, round_goal);
      it.print(cx, 225, font_date, col_cyan, esphome::display::TextAlign::CENTER, rb);
    }
  } else if (!round_over && hit_feedback[0] != '\0' && (now_ms - feedback_timer < 1100U)) {
    it.print(cx, 225, font_date, feedback_col, esphome::display::TextAlign::CENTER, hit_feedback);
  }

  // 7. Single Bottom Scorekeeper Line at y = 408 (Universal Clean Game Canvas Standard)
  char wind_buf[20];
  if (fabsf(wind_speed) < 1.0f) {
    snprintf(wind_buf, sizeof(wind_buf), "CALM");
  } else if (wind_speed > 0.0f) {
    snprintf(wind_buf, sizeof(wind_buf), "%.0f mph >", wind_speed);
  } else {
    snprintf(wind_buf, sizeof(wind_buf), "< %.0f mph", -wind_speed);
  }
  char hud_buf[56];
  snprintf(hud_buf, sizeof(hud_buf), "R%d  •  %d Arr  •  %s  •  %d", round_num, arrows_left, wind_buf, arch_score);
  it.print(cx, 392, font_small, col_gold, esphome::display::TextAlign::CENTER, hud_buf);
  render_back_button(it, cx, font_small, 426, "< Back");

  // 8. Round Over Summary Modal
  if (round_over) {
    it.filled_circle(cx, cy, 115, col_bg);
    it.circle(cx, cy, 115, is_new_high ? col_gold : theme_secondary);
    it.circle(cx, cy, 113, is_new_high ? col_gold : theme_secondary);

    if (is_new_high) {
      it.print(cx, 156, font_date, col_gold, esphome::display::TextAlign::CENTER, "NEW HIGH SCORE!");
    } else {
      char r_end[32];
      snprintf(r_end, sizeof(r_end), "ROUND %d REACHED", round_num);
      it.print(cx, 156, font_date, col_cyan, esphome::display::TextAlign::CENTER, r_end);
    }

    char score_str[32];
    snprintf(score_str, sizeof(score_str), "Score: %d", arch_score);
    it.print(cx, 198, font_date, col_white, esphome::display::TextAlign::CENTER, score_str);

    char best_str[32];
    snprintf(best_str, sizeof(best_str), "Best: %d", high_score_archery);
    it.print(cx, 233, font_small, col_gold, esphome::display::TextAlign::CENTER, best_str);

    it.print(cx, 275, font_small, col_cyan, esphome::display::TextAlign::CENTER, "[ TAP TO REPLAY ]");
  }
}


// ============================================================================
// SECTION 10: REUSABLE 3D BEVELED BRICK, CAPSULE PADDLE & BREAKOUT ENGINE
// ============================================================================

template<typename DisplayT>
inline void render_breakout_brick(DisplayT &it, int cx_b, int cy_b, int w, int h,
                                  esphome::Color body_col, esphome::Color hi_col,
                                  int hp, int max_hp, esphome::Color col_white) {
  int rx = cx_b - w / 2;
  int ry = cy_b - h / 2;
  // Subtle drop shadow beneath brick
  it.horizontal_line(rx + 1, ry + h, w - 1, esphome::Color(12, 16, 24));
  // Main brick body + dark outer frame
  it.filled_rectangle(rx, ry, w, h, body_col);
  it.rectangle(rx, ry, w, h, esphome::Color(18, 22, 30));
  // Top and left 3D specular highlight bevel
  it.horizontal_line(rx + 1, ry + 1, w - 2, hi_col);
  it.line(rx + 1, ry + 1, rx + 1, ry + h - 2, hi_col);
  // Bottom inner shadow line
  esphome::Color sh_col = body_col * 0.58f;
  it.horizontal_line(rx + 2, ry + h - 2, w - 3, sh_col);

  if (max_hp >= 2) {
    if (hp >= 2) {
      // Metallic corner rivets for armored 2-hit / 3-hit bricks
      it.draw_pixel_at(rx + 3, ry + 3, col_white);
      it.draw_pixel_at(rx + w - 4, ry + 3, col_white);
      it.draw_pixel_at(rx + 3, ry + h - 4, col_white);
      it.draw_pixel_at(rx + w - 4, ry + h - 4, col_white);
      if (hp >= 3) {
        // Reinforced fortress core bar for 3-hit bricks
        it.filled_rectangle(cx_b - 5, cy_b - 2, 10, 4, col_white);
      } else if (max_hp == 3) {
        // First fracture line on a 3-hit brick at hp == 2
        it.line(cx_b - 4, ry + 2, cx_b + 3, ry + h - 3, esphome::Color(20, 24, 32));
      }
    } else if (hp == 1) {
      // Cracked armor fracture lines across the brick face when 1 hit remains
      it.line(cx_b - 6, ry + 1, cx_b, cy_b, esphome::Color(16, 20, 28));
      it.line(cx_b, cy_b, cx_b + 6, ry + h - 2, esphome::Color(16, 20, 28));
      it.line(cx_b - 5, ry + 1, cx_b + 1, cy_b, hi_col);
    }
  }
}

template<typename DisplayT>
inline void render_breakout_paddle(DisplayT &it, int px, int py, int pw, int ph,
                                   esphome::Color pad_col, esphome::Color col_white) {
  int r = ph / 2;
  int x_l = px - pw / 2 + r;
  int x_r = px + pw / 2 - r;
  int body_w = x_r - x_l;
  if (body_w < 2) body_w = 2;

  // Drop shadow
  it.horizontal_line(x_l, py + r + 1, body_w, esphome::Color(12, 16, 24));
  // Seamless rounded capsule body (no internal rectangle seams)
  it.filled_circle(x_l, py, r, pad_col);
  it.filled_circle(x_r, py, r, pad_col);
  it.filled_rectangle(x_l, py - r, body_w, ph + 1, pad_col);
  // Crisp white top specular edge and dark bottom bevel
  it.horizontal_line(x_l, py - r, body_w, col_white);
  it.horizontal_line(x_l, py + r, body_w, pad_col * 0.55f);
  // Luminous inner energy core
  int core_w = (pw * 42) / 100;
  it.filled_rectangle(px - core_w / 2, py - 1, core_w, 3, col_white);
}

template<typename DisplayT, typename FontBigT, typename FontSmallT>
inline void render_game_breakout(DisplayT &it, int cx, int cy, uint32_t now_ms,
                                 bool game_just_entered, float roll_delta,
                                 float &marble_touch_x, bool &game_btn_pressed,
                                 int &high_score_breakout,
                                 FontBigT *font_date, FontSmallT *font_small,
                                 esphome::Color theme_primary, esphome::Color theme_secondary,
                                 esphome::Color col_bg, esphome::Color col_white,
                                 esphome::Color col_gold, esphome::Color col_cyan,
                                 esphome::Color col_orange, esphome::Color col_green,
                                 esphome::Color col_red, esphome::Color col_slate,
                                 esphome::Color col_dark_gray) {
  struct BreakoutBrick {
    int16_t x, y, w, h;
    uint8_t row;
    uint8_t hp;
    uint8_t max_hp;
  };
  struct ShrapnelPiece {
    float x, y, vx, vy;
    esphome::Color col;
  };

  static constexpr int NUM_BRICKS = 18;
  static BreakoutBrick bricks[NUM_BRICKS];
  static uint32_t last_phys_ms = 0;
  static int wave_num = 1;
  static int lives_left = 3;
  static int bo_score = 0;
  static int rally_combo = 0;
  static bool hit_brick_since_paddle = false;
  static bool ball_launched = false;
  static bool round_over = false;
  static bool is_new_high = false;

  static float ball_x = 233.0f;
  static float ball_y = 344.0f;
  static float ball_vx = 0.0f;
  static float ball_vy = 0.0f;
  static float trail_x[4] = {233.0f, 233.0f, 233.0f, 233.0f};
  static float trail_y[4] = {344.0f, 344.0f, 344.0f, 344.0f};
  static float smooth_paddle_x = 233.0f;

  // Shattering brick shrapnel + floating combo popup
  static ShrapnelPiece shrapnel[6];
  static uint32_t shrapnel_ms = 0;
  static int popup_x = 233;
  static int popup_y = 140;
  static uint32_t popup_ms = 0;
  static char popup_text[32] = "";
  static esphome::Color popup_col(255, 215, 0);

  static uint32_t banner_ms = 0;
  static char banner_text[32] = "";

  // Helper to populate the symmetric 4-5-5-4 dome brick rack centered at cx = 233
  auto init_brick_wave = [&](int wv) {
    static constexpr int ROW_COUNTS[4] = {4, 5, 5, 4};
    static constexpr int ROW_YS[4] = {82, 104, 126, 148};
    int idx = 0;
    for (int r = 0; r < 4; r++) {
      int cnt = ROW_COUNTS[r];
      int start_x = cx - ((cnt - 1) * 46) / 2;
      for (int c = 0; c < cnt; c++) {
        uint8_t hp = 1;
        if (wv == 2 && r == 0) {
          hp = 2;
        } else if (wv == 3 && (r == 0 || (r == 1 && c >= 1 && c <= 3))) {
          hp = 2;
        } else if (wv == 4) {
          if (r == 0) hp = 3;
          else if (r == 1) hp = 2;
        } else if (wv >= 5) {
          if (r == 0) hp = 3;
          else if (r == 1) hp = 2;
          else if (r == 2 && (c == 0 || c == 2 || c == 4)) hp = 2;
        }
        bricks[idx].x = (int16_t)(start_x + c * 46);
        bricks[idx].y = (int16_t)ROW_YS[r];
        bricks[idx].w = 40;
        bricks[idx].h = 14;
        bricks[idx].row = (uint8_t)r;
        bricks[idx].hp = hp;
        bricks[idx].max_hp = hp;
        idx++;
      }
    }
  };

  auto reset_ball_on_paddle = [&]() {
    ball_launched = false;
    ball_x = smooth_paddle_x;
    ball_y = 344.0f;
    ball_vx = 0.0f;
    ball_vy = 0.0f;
    for (int t = 0; t < 4; t++) {
      trail_x[t] = ball_x;
      trail_y[t] = ball_y;
    }
  };

  if (game_just_entered) {
    last_phys_ms = now_ms;
    wave_num = 1;
    lives_left = 3;
    bo_score = 0;
    rally_combo = 0;
    hit_brick_since_paddle = false;
    round_over = false;
    is_new_high = false;
    smooth_paddle_x = (float)cx;
    shrapnel_ms = 0;
    popup_ms = 0;
    banner_ms = 0;
    init_brick_wave(1);
    reset_ball_on_paddle();
  }

  float dt = (now_ms >= last_phys_ms) ? ((now_ms - last_phys_ms) * 0.001f) : 0.05f;
  last_phys_ms = now_ms;
  if (dt < 0.012f) dt = 0.012f;
  if (dt > 0.075f) dt = 0.075f;

  // Paddle geometry & circular chord clamping at y = 356
  const int paddle_y = 350;
  const int paddle_w = 58;
  const int paddle_h = 10;
  // At y = 356 (dy = 123 from cy = 233), r = 208 arena chord half-width is 167px.
  // Clamping paddle center within cx +/- 136 keeps the entire 58px capsule inside the dome.
  float raw_target_x = (float)cx + roll_delta * 8.4f;
  if (raw_target_x < (float)(cx - 136)) raw_target_x = (float)(cx - 136);
  if (raw_target_x > (float)(cx + 136)) raw_target_x = (float)(cx + 136);
  smooth_paddle_x += (raw_target_x - smooth_paddle_x) * 0.34f;
  float paddle_x = smooth_paddle_x;

  float target_speed = std::min(285.0f, 215.0f + (wave_num - 1) * 14.0f);

  // Touch Trigger: Launch Ball or Replay
  if (marble_touch_x >= 0.0f || game_btn_pressed) {
    marble_touch_x = -1.0f;
    game_btn_pressed = false;

    if (round_over) {
      wave_num = 1;
      lives_left = 3;
      bo_score = 0;
      rally_combo = 0;
      hit_brick_since_paddle = false;
      round_over = false;
      is_new_high = false;
      shrapnel_ms = 0;
      popup_ms = 0;
      banner_ms = 0;
      init_brick_wave(1);
      reset_ball_on_paddle();
      game_imu_retare_flag() = true;
    } else if (!ball_launched && (banner_ms == 0U || now_ms - banner_ms >= 3000U)) {
      float launch_ang = std::max(-0.52f, std::min(0.52f, roll_delta * 0.025f));
      if (std::fabs(launch_ang) < 0.14f) {
        launch_ang = (now_ms & 1) ? 0.22f : -0.22f;
      }
      ball_vx = target_speed * fast_sin(launch_ang);
      ball_vy = -target_speed * fast_cos(launch_ang);
      ball_launched = true;
      hit_brick_since_paddle = true;
    }
  }

  // Row palettes (Gold, Cyan, Orange, Emerald, plus Silver/Armor for hp >= 2)
  esphome::Color row_cols[4] = {col_gold, col_cyan, col_orange, col_green};
  esphome::Color row_his[4] = {
      esphome::Color(255, 245, 165),
      esphome::Color(185, 255, 255),
      esphome::Color(255, 195, 100),
      esphome::Color(185, 250, 205)};

  if (!round_over) {
    if (!ball_launched) {
      ball_x = paddle_x;
      ball_y = (float)(paddle_y - 11);
      for (int t = 0; t < 4; t++) {
        trail_x[t] = ball_x;
        trail_y[t] = ball_y;
      }
    } else {
      // Shift comet trail
      for (int t = 3; t > 0; t--) {
        trail_x[t] = trail_x[t - 1];
        trail_y[t] = trail_y[t - 1];
      }
      trail_x[0] = ball_x;
      trail_y[0] = ball_y;

      ball_x += ball_vx * dt;
      ball_y += ball_vy * dt;

      // 1. Circular Dome Arena Reflection (r = 202px ball center limit inside r = 208 wall)
      //    - Upper Dome (ball_y <= cy + 12): True circular arch reflection
      //    - Lower Flanks (ball_y > cy + 12, e.g. 4:00 & 8:00 positions): Vertical side-cushion
      //      reflection so the inward-curving lower bezel never kicks a falling ball back up into
      //      a 10:00 <-> 4:00 or 2:00 <-> 8:00 diameter loop!
      static int wall_bounces = 0;
      float b_dx = ball_x - (float)cx;
      float b_dy = ball_y - (float)cy;
      float b_dist = sqrtf(b_dx * b_dx + b_dy * b_dy);
      if (b_dist > 202.0f && ball_y < (float)(paddle_y - 4)) {
        float bnx = b_dx / b_dist;
        float bny = b_dy / b_dist;
        // Clamp ball position back inside dome so it never sticks in the wall
        ball_x = (float)cx + bnx * 201.0f;
        ball_y = (float)cy + bny * 201.0f;

        // Below the equator (4:00 / 8:00), use a horizontal side-wall normal (rny = 0)
        // so the lower curve banks the ball inward toward the paddle instead of upward!
        float rnx = bnx;
        float rny = (ball_y > (float)(cy + 12)) ? 0.0f : bny;
        float r_len = sqrtf(rnx * rnx + rny * rny);
        if (r_len > 0.001f) {
          rnx /= r_len;
          rny /= r_len;
        }

        float v_dot = ball_vx * rnx + ball_vy * rny;
        if (v_dot > 0.0f) {
          ball_vx -= 2.0f * v_dot * rnx;
          ball_vy -= 2.0f * v_dot * rny;
          wall_bounces++;

          // If the ball bounces off the wall 2+ times without touching a brick or paddle,
          // add a downward gravity deflection to break any chord/diameter oscillation.
          if (wall_bounces >= 2) {
            ball_vy += (ball_vy >= 0.0f) ? 55.0f : 70.0f;
          }
          // Enforce a healthy vertical speed floor (95 px/s) so chords never stall horizontally
          if (std::fabs(ball_vy) < 95.0f) {
            ball_vy = (ball_vy >= 0.0f) ? 98.0f : -98.0f;
          }
          // Re-normalize speed to target_speed
          float spd = sqrtf(ball_vx * ball_vx + ball_vy * ball_vy);
          if (spd > 1.0f) {
            ball_vx = (ball_vx / spd) * target_speed;
            ball_vy = (ball_vy / spd) * target_speed;
          }
        }
      }

      // 2. Constant-Speed Angle-Steered Paddle Reflection
      if (ball_vy > 0.0f && ball_y >= (float)(paddle_y - 10) && ball_y <= (float)(paddle_y + 8)) {
        float half_reach = paddle_w * 0.5f + 6.0f;
        if (std::fabs(ball_x - paddle_x) <= half_reach) {
          if (!hit_brick_since_paddle) {
            // Reset streak if the ball returned to the paddle without hitting any brick
            rally_combo = 0;
          }
          hit_brick_since_paddle = false;
          wall_bounces = 0;
          float offset = std::max(-1.0f, std::min(1.0f, (ball_x - paddle_x) / (paddle_w * 0.5f)));
          // Steer bounce angle up to +/- 60 degrees (1.047 rad) while maintaining constant speed
          float bounce_ang = offset * 1.047f;
          ball_vx = target_speed * fast_sin(bounce_ang);
          ball_vy = -target_speed * fast_cos(bounce_ang);
          ball_y = (float)(paddle_y - 11);
          pending_game_sfx() = 11; // Paddle Bounce Blip
        }
      }

      // 3. Brick Collisions with True X vs. Y Overlap Separation & Multi-Hit Armor
      const float ball_r = 6.0f;
      for (int b = 0; b < NUM_BRICKS; b++) {
        if (bricks[b].hp == 0) continue;
        float dx = ball_x - (float)bricks[b].x;
        float dy = ball_y - (float)bricks[b].y;
        float lim_x = bricks[b].w * 0.5f + ball_r;
        float lim_y = bricks[b].h * 0.5f + ball_r;
        if (std::fabs(dx) < lim_x && std::fabs(dy) < lim_y) {
          float overlap_x = lim_x - std::fabs(dx);
          float overlap_y = lim_y - std::fabs(dy);
          if (overlap_x < overlap_y) {
            ball_vx = -ball_vx;
            ball_x += (dx >= 0.0f) ? overlap_x : -overlap_x;
          } else {
            ball_vy = -ball_vy;
            ball_y += (dy >= 0.0f) ? overlap_y : -overlap_y;
          }

          bricks[b].hp--;
          pending_game_sfx() = (bricks[b].hp == 0) ? 12 : 13;
          rally_combo++;
          wall_bounces = 0;
          hit_brick_since_paddle = true;
          int mult = std::min(4, rally_combo);

          static constexpr int BASE_ROW_PTS[4] = {150, 120, 90, 60};
          int base_pts = (bricks[b].hp == 0) ? BASE_ROW_PTS[bricks[b].row] : 50;
          int pts = base_pts * mult;
          bo_score += pts;
          if (bo_score > high_score_breakout) {
            high_score_breakout = bo_score;
            is_new_high = true;
          }

          // Floating combo score popup
          popup_x = bricks[b].x;
          popup_y = bricks[b].y;
          popup_ms = now_ms;
          if (mult > 1) {
            snprintf(popup_text, sizeof(popup_text), "+%d %dx!", pts, mult);
            popup_col = col_gold;
          } else {
            snprintf(popup_text, sizeof(popup_text), "+%d", pts);
            popup_col = col_white;
          }

          // Spawn 6 shattering shrapnel fragments
          esphome::Color frag_c = (bricks[b].hp > 0) ? esphome::Color(200, 210, 225) : row_cols[bricks[b].row];
          for (int s = 0; s < 6; s++) {
            float ang = (float)s * 1.047f + 0.35f;
            float f_spd = (bricks[b].hp == 0) ? 95.0f : 55.0f;
            shrapnel[s].x = (float)bricks[b].x + fast_cos(ang) * 6.0f;
            shrapnel[s].y = (float)bricks[b].y + fast_sin(ang) * 4.0f;
            shrapnel[s].vx = fast_cos(ang) * f_spd;
            shrapnel[s].vy = fast_sin(ang) * f_spd - 25.0f;
            shrapnel[s].col = (s & 1) ? row_his[bricks[b].row] : frag_c;
          }
          shrapnel_ms = now_ms;
          break;
        }
      }

      // 4. Check Wave Clear vs. Ball Lost Below Gate
      int rem_check = 0;
      for (int b = 0; b < NUM_BRICKS; b++) {
        if (bricks[b].hp > 0) rem_check++;
      }
      if (rem_check == 0 && banner_ms == 0U) {
        int wave_bonus = wave_num * 300;
        bo_score += wave_bonus;
        if (bo_score > high_score_breakout) {
          high_score_breakout = bo_score;
          is_new_high = true;
        }
        wave_num++;
        if (lives_left < 3) lives_left++;
        snprintf(banner_text, sizeof(banner_text), "WAVE %d CLEAR! +%d", wave_num - 1, wave_bonus);
        banner_ms = now_ms;
        pending_game_sfx() = 5; // Wave Clear Fanfare!
        reset_ball_on_paddle();
      } else if (ball_y > 376.0f) {
        lives_left--;
        pending_game_sfx() = 14; // Ball Lost Tone
        rally_combo = 0;
        hit_brick_since_paddle = false;
        reset_ball_on_paddle();
        if (lives_left <= 0) {
          round_over = true;
          if (bo_score > high_score_breakout) {
            high_score_breakout = bo_score;
            is_new_high = true;
          }
        }
      }
    }
  }

  // Update shrapnel physics
  if (shrapnel_ms > 0 && (now_ms - shrapnel_ms < 360U)) {
    for (int s = 0; s < 6; s++) {
      shrapnel[s].x += shrapnel[s].vx * dt;
      shrapnel[s].y += shrapnel[s].vy * dt;
      shrapnel[s].vy += 180.0f * dt;
    }
  }

  // ==========================================================================
  // RENDERING PIPELINE (Clean Full-Screen Circular Dome Arena)
  // ==========================================================================

  // 1. Subtle Upper Circular Dome Wall (r = 208 & 207)
  it.circle(cx, cy, 208, col_dark_gray);
  it.circle(cx, cy, 207, col_slate * 0.35f);

  // 2. Bottom Scorekeeper HUD at y = 408
  int bricks_remaining = 0;
  for (int b = 0; b < NUM_BRICKS; b++) {
    if (bricks[b].hp > 0) bricks_remaining++;
  }
  char bo_hud[56];
  if (rally_combo >= 2) {
    int mult = std::min(4, rally_combo);
    snprintf(bo_hud, sizeof(bo_hud), "W%d  •  %d/3  •  %dx  •  %d", wave_num, lives_left, mult, bo_score);
  } else {
    snprintf(bo_hud, sizeof(bo_hud), "W%d  •  Balls:%d  •  Brk:%d  •  %d", wave_num, lives_left, bricks_remaining, bo_score);
  }
  it.print(cx, 392, font_small, col_gold, esphome::display::TextAlign::CENTER, bo_hud);
  render_back_button(it, cx, font_small, 426, "< Back");

  // 3. Draw 18-Brick Dome Formation (with Multi-Hit Armor & Crack States)
  for (int b = 0; b < NUM_BRICKS; b++) {
    if (bricks[b].hp == 0) continue;
    esphome::Color b_col = row_cols[bricks[b].row];
    esphome::Color h_col = row_his[bricks[b].row];
    if (bricks[b].hp >= 3) {
      b_col = esphome::Color(225, 55, 75);
      h_col = esphome::Color(255, 175, 185);
    } else if (bricks[b].hp == 2) {
      b_col = esphome::Color(145, 162, 182);
      h_col = esphome::Color(230, 240, 252);
    }
    render_breakout_brick(it, bricks[b].x, bricks[b].y, bricks[b].w, bricks[b].h,
                          b_col, h_col, bricks[b].hp, bricks[b].max_hp, col_white);
  }

  // 4. Draw Shattering Shrapnel Fragments & Floating Combo Popup
  if (shrapnel_ms > 0 && (now_ms - shrapnel_ms < 360U)) {
    for (int s = 0; s < 6; s++) {
      it.filled_rectangle((int)shrapnel[s].x - 2, (int)shrapnel[s].y - 2, 4, 4, shrapnel[s].col);
    }
  }
  if (popup_ms > 0 && (now_ms - popup_ms < 650U)) {
    int py_off = (int)((now_ms - popup_ms) * 22U / 650U);
    it.print(popup_x, popup_y - py_off, font_small, popup_col,
             esphome::display::TextAlign::CENTER, popup_text);
  }
  if (banner_ms > 0U) {
    uint32_t b_el = now_ms - banner_ms;
    if (b_el >= 1100U && bricks_remaining == 0) {
      init_brick_wave(wave_num);
    }
    if (b_el >= 3000U) {
      banner_ms = 0U;
    } else if (b_el < 1100U) {
      it.print(cx, 212, font_date, col_gold, esphome::display::TextAlign::CENTER, banner_text);
    } else {
      char rdy_buf[32];
      snprintf(rdy_buf, sizeof(rdy_buf), "GET READY! WAVE %d", wave_num);
      it.print(cx, 212, font_date, col_cyan, esphome::display::TextAlign::CENTER, rdy_buf);
    }
  }

  // 5. Draw Capsule Paddle at y = 356
  render_breakout_paddle(it, (int)paddle_x, paddle_y, paddle_w, paddle_h, theme_primary, col_white);

  // 6. Draw Fading Ball Comet Trail & 3D Specular Energy Ball
  if (ball_launched) {
    it.filled_circle((int)trail_x[3], (int)trail_y[3], 2, esphome::Color(65, 52, 18));
    it.filled_circle((int)trail_x[2], (int)trail_y[2], 3, esphome::Color(120, 95, 25));
    it.filled_circle((int)trail_x[0], (int)trail_y[0], 4, esphome::Color(185, 145, 35));
  }
  it.filled_circle((int)ball_x, (int)ball_y, 6, col_gold);
  it.circle((int)ball_x, (int)ball_y, 6, col_white);
  it.filled_circle((int)ball_x - 2, (int)ball_y - 2, 2, col_white);

  // 7. Launch Prompt or Game Over Scoreboard Overlay
  if (round_over) {
    it.filled_circle(cx, cy, 115, col_bg);
    it.circle(cx, cy, 115, is_new_high ? col_gold : theme_secondary);
    it.circle(cx, cy, 113, is_new_high ? col_gold : theme_secondary);

    if (is_new_high) {
      it.print(cx, 156, font_date, col_gold, esphome::display::TextAlign::CENTER, "NEW HIGH SCORE!");
    } else {
      if (wave_num == 1) {
        it.print(cx, 156, font_date, col_red, esphome::display::TextAlign::CENTER, "GAME OVER");
      } else {
        char w_end[32];
        snprintf(w_end, sizeof(w_end), "WAVE %d REACHED", wave_num);
        it.print(cx, 156, font_date, col_cyan, esphome::display::TextAlign::CENTER, w_end);
      }
    }

    char score_str[32];
    snprintf(score_str, sizeof(score_str), "Score: %d", bo_score);
    it.print(cx, 198, font_date, col_white, esphome::display::TextAlign::CENTER, score_str);

    char best_str[32];
    snprintf(best_str, sizeof(best_str), "Best: %d", high_score_breakout);
    it.print(cx, 233, font_small, col_gold, esphome::display::TextAlign::CENTER, best_str);

    it.print(cx, 275, font_small, col_cyan, esphome::display::TextAlign::CENTER, "[ TAP TO REPLAY ]");
  }
}


// ============================================================================
// SECTION 11: REUSABLE KID TREATS, MUPPET MONSTER & CHOMP GAME ENGINE
// ============================================================================

template<typename DisplayT>
inline void render_chomp_treat(DisplayT &it, int tx, int ty, int kind,
                               esphome::Color col_white, esphome::Color col_red,
                               esphome::Color col_gold, esphome::Color col_cyan,
                               esphome::Color col_green, esphome::Color col_magenta,
                               esphome::Color col_bg) {
  int k = kind % 5;
  // Soft drop shadow beneath every treat
  it.filled_circle(tx, ty + 3, 13, esphome::Color(14, 18, 26));

  if (k == 0) {
    // 0: CHOCOLATE CHIP COOKIE (Cookie Monster's favorite!)
    esphome::Color dough(218, 162, 84);
    esphome::Color crust(168, 114, 46);
    esphome::Color chip(58, 34, 18);
    it.filled_circle(tx, ty, 13, dough);
    it.circle(tx, ty, 13, crust);
    it.circle(tx, ty, 12, esphome::Color(238, 188, 112));
    // 6 chocolate chips scattered across the cookie
    it.filled_circle(tx - 5, ty - 4, 2, chip);
    it.filled_circle(tx + 4, ty - 5, 2, chip);
    it.filled_circle(tx, ty + 1, 2, chip);
    it.filled_circle(tx - 6, ty + 4, 2, chip);
    it.filled_circle(tx + 5, ty + 4, 2, chip);
    it.draw_pixel_at(tx - 1, ty - 7, chip);
    it.draw_pixel_at(tx + 2, ty + 7, chip);
  } else if (k == 1) {
    // 1: STRAWBERRY (Bright red berry + green leafy calyx + golden seeds)
    it.filled_circle(tx, ty + 1, 12, col_red);
    it.circle(tx, ty + 1, 12, esphome::Color(255, 130, 130));
    // Green leafy crown at top
    it.filled_triangle(tx - 8, ty - 8, tx, ty - 3, tx - 2, ty - 12, col_green);
    it.filled_triangle(tx + 8, ty - 8, tx, ty - 3, tx + 2, ty - 12, col_green);
    it.filled_triangle(tx - 4, ty - 12, tx + 4, ty - 12, tx, ty - 5, col_green);
    it.line(tx, ty - 11, tx, ty - 15, col_green);
    // Golden seeds + shine
    it.draw_pixel_at(tx - 4, ty - 1, col_gold);
    it.draw_pixel_at(tx + 4, ty - 1, col_gold);
    it.draw_pixel_at(tx, ty + 3, col_gold);
    it.draw_pixel_at(tx - 5, ty + 5, col_gold);
    it.draw_pixel_at(tx + 5, ty + 5, col_gold);
    it.draw_pixel_at(tx, ty + 8, col_gold);
    it.filled_circle(tx - 4, ty - 3, 2, col_white);
  } else if (k == 2) {
    // 2: FROSTED SPRINKLE DONUT (Golden ring + pink icing + colorful sprinkles)
    esphome::Color icing(255, 110, 178);
    it.filled_circle(tx, ty, 13, esphome::Color(215, 155, 78));
    it.filled_circle(tx, ty, 11, icing);
    it.circle(tx, ty, 13, esphome::Color(160, 105, 42));
    // Center donut hole
    it.filled_circle(tx, ty, 4, col_bg);
    it.circle(tx, ty, 4, esphome::Color(180, 70, 125));
    // Sprinkles
    it.line(tx - 7, ty - 5, tx - 5, ty - 7, col_cyan);
    it.line(tx + 5, ty - 7, tx + 7, ty - 5, col_gold);
    it.line(tx - 8, ty + 3, tx - 6, ty + 5, col_white);
    it.line(tx + 6, ty + 4, tx + 8, ty + 2, col_green);
    it.line(tx - 2, ty + 8, tx + 2, ty + 8, col_gold);
  } else if (k == 3) {
    // 3: WRAPPED CANDY (Magenta sweet + twisted wrapper bows on left & right)
    it.filled_triangle(tx - 9, ty, tx - 17, ty - 7, tx - 17, ty + 7, col_magenta);
    it.filled_triangle(tx + 9, ty, tx + 17, ty - 7, tx + 17, ty + 7, col_magenta);
    it.line(tx - 16, ty - 5, tx - 10, ty, col_white);
    it.line(tx + 16, ty - 5, tx + 10, ty, col_white);
    it.filled_circle(tx, ty, 10, col_magenta);
    it.circle(tx, ty, 10, col_white);
    // Candy swirl stripe
    it.line(tx - 5, ty - 6, tx + 5, ty + 6, col_white);
    it.line(tx - 4, ty - 6, tx + 6, ty + 6, esphome::Color(255, 190, 230));
    it.filled_circle(tx - 3, ty - 3, 2, col_white);
  } else {
    // 4: GOLDEN STAR BERRY (Juicy cyan-blue berry with golden star sparkle)
    it.filled_circle(tx, ty, 12, col_cyan);
    it.circle(tx, ty, 12, col_white);
    it.filled_circle(tx - 4, ty - 4, 3, col_white);
    it.line(tx, ty - 6, tx, ty + 6, col_gold);
    it.line(tx - 6, ty, tx + 6, ty, col_gold);
    it.filled_circle(tx, ty, 2, col_gold);
  }
}

template<typename DisplayT>
inline void render_chomp_monster(DisplayT &it, int mx, int my, int mr,
                                 float look_dx, float look_dy,
                                 bool mouth_open, bool is_chomping,
                                 bool party_hat, uint32_t now_ms,
                                 esphome::Color col_white, esphome::Color col_black,
                                 esphome::Color col_gold, esphome::Color col_red,
                                 esphome::Color col_magenta, esphome::Color col_dark_gray) {
  // Sesame Street / Cookie Monster inspired royal-azure furry palette
  esphome::Color fur_main(28, 148, 248);
  esphome::Color fur_dark(16, 96, 182);
  esphome::Color fur_hi(110, 198, 255);
  esphome::Color ear_inner(255, 158, 188);
  esphome::Color cheek_pink(255, 128, 166);

  // Happy chew bounce offset
  int bounce_y = 0;
  if (is_chomping) {
    bounce_y = ((now_ms / 70U) & 1) ? -3 : 2;
  }
  int cy_m = my + bounce_y;

  // 1. Drop shadow
  it.filled_circle(mx, cy_m + 4, mr, esphome::Color(12, 16, 24));

  // 2. Cute Rounded Teddy/Muppet Ears on upper left & right (NO horns!)
  int ear_r = (mr * 34) / 100;
  int ear_inner_r = (mr * 19) / 100;
  int ear_dx = (mr * 74) / 100;
  int ear_dy = (mr * 68) / 100;
  // Left ear
  it.filled_circle(mx - ear_dx, cy_m - ear_dy, ear_r, fur_dark);
  it.filled_circle(mx - ear_dx, cy_m - ear_dy, ear_r - 1, fur_main);
  it.circle(mx - ear_dx, cy_m - ear_dy, ear_r, fur_hi);
  it.filled_circle(mx - ear_dx + 1, cy_m - ear_dy + 1, ear_inner_r, ear_inner);
  // Right ear
  it.filled_circle(mx + ear_dx, cy_m - ear_dy, ear_r, fur_dark);
  it.filled_circle(mx + ear_dx, cy_m - ear_dy, ear_r - 1, fur_main);
  it.circle(mx + ear_dx, cy_m - ear_dy, ear_r, fur_hi);
  it.filled_circle(mx + ear_dx - 1, cy_m - ear_dy + 1, ear_inner_r, ear_inner);

  // 3. Fluffy Furry Head Silhouette (soft fur tufts around perimeter + main circle)
  int tuft_r = (mr * 28) / 100;
  it.filled_circle(mx - mr + 5, cy_m - 4, tuft_r, fur_main);
  it.filled_circle(mx + mr - 5, cy_m - 4, tuft_r, fur_main);
  it.filled_circle(mx - mr + 7, cy_m + 10, tuft_r, fur_main);
  it.filled_circle(mx + mr - 7, cy_m + 10, tuft_r, fur_main);
  it.filled_circle(mx, cy_m - mr + 3, tuft_r, fur_main);

  it.filled_circle(mx, cy_m, mr, fur_main);
  it.circle(mx, cy_m, mr, fur_hi);
  it.circle(mx, cy_m, mr - 1, fur_dark);

  (void)party_hat;

  // 4. Clean Furry Face (no cheek circles)

  // 5. Big Googly Cookie-Monster Eyes perched high on the head
  int eye_dx = (mr * 30) / 100;
  int eye_y = cy_m - (mr * 34) / 100;
  int eye_r = std::max(7, (mr * 27) / 100);
  int pup_r = std::max(3, (eye_r * 48) / 100);

  it.filled_circle(mx - eye_dx, eye_y, eye_r, col_white);
  it.circle(mx - eye_dx, eye_y, eye_r, col_dark_gray);
  it.filled_circle(mx + eye_dx, eye_y, eye_r, col_white);
  it.circle(mx + eye_dx, eye_y, eye_r, col_dark_gray);

  // Slightly googly independent pupil offsets during chomping, or tracking the nearest treat
  int pdx_l = (int)(look_dx * (float)(eye_r - pup_r - 1));
  int pdy_l = (int)(look_dy * (float)(eye_r - pup_r - 1));
  int pdx_r = pdx_l;
  int pdy_r = pdy_l;
  if (is_chomping) {
    // Classic Cookie Monster googly-eye roll while munching!
    pdx_l = ((now_ms / 90U) & 1) ? -2 : 2;
    pdy_l = -2;
    pdx_r = -pdx_l;
    pdy_r = 1;
  }
  it.filled_circle(mx - eye_dx + pdx_l, eye_y + pdy_l, pup_r, col_black);
  it.filled_circle(mx + eye_dx + pdx_r, eye_y + pdy_r, pup_r, col_black);
  // Eye specular glints
  it.draw_pixel_at(mx - eye_dx + pdx_l - 1, eye_y + pdy_l - 1, col_white);
  it.draw_pixel_at(mx + eye_dx + pdx_r - 1, eye_y + pdy_r - 1, col_white);

  // 6. Expressive Mouth (Wide-Open Hungry Cavity + Pink Tongue vs. Chomping vs. Big Muppet Smile)
  int mouth_y = cy_m + (mr * 26) / 100;
  if (is_chomping) {
    bool jaw_open = ((now_ms / 85U) & 1) != 0;
    int mr_open = jaw_open ? ((mr * 36) / 100) : ((mr * 20) / 100);
    it.filled_circle(mx, mouth_y, mr_open, col_black);
    it.filled_circle(mx, mouth_y + mr_open / 2, mr_open / 2, ear_inner);
    // Flying cookie crumbs while munching!
    esphome::Color crumb(218, 162, 84);
    int c_off = (int)((now_ms / 60U) % 8U);
    it.filled_circle(mx - mr / 2 - c_off, mouth_y - 2 + c_off / 2, 2, crumb);
    it.filled_circle(mx + mr / 2 + c_off, mouth_y - 4 + c_off / 2, 2, crumb);
    it.filled_circle(mx + 3, mouth_y + mr / 2 + c_off / 2, 2, crumb);
  } else if (mouth_open) {
    // Big wide-open dark muppet mouth with a cute pink tongue at bottom
    int m_rad = (mr * 44) / 100;
    it.filled_circle(mx, mouth_y + 2, m_rad, col_black);
    it.circle(mx, mouth_y + 2, m_rad, fur_dark);
    // Pink tongue
    int t_rad = (m_rad * 56) / 100;
    it.filled_circle(mx, mouth_y + 2 + m_rad - t_rad - 1, t_rad, cheek_pink);
  } else {
    // Wide, friendly Sesame Street muppet smile
    int sw = (mr * 48) / 100;
    it.filled_circle(mx, mouth_y - 2, sw, col_black);
    it.filled_rectangle(mx - sw, mouth_y - sw - 2, sw * 2 + 1, sw, fur_main);
    it.horizontal_line(mx - sw, mouth_y - 2, sw * 2 + 1, fur_dark);
    // Cute pink tongue peeking out at bottom of smile
    it.filled_circle(mx, mouth_y + sw / 2, sw / 3, cheek_pink);
  }
}

template<typename DisplayT, typename FontBigT, typename FontSmallT>
inline void render_game_chomp(DisplayT &it, int cx, int cy, uint32_t now_ms,
                              bool game_just_entered, float roll_delta, float pitch_delta,
                              float &marble_touch_x, bool &game_btn_pressed,
                              int &high_score_chomp,
                              FontBigT *font_date, FontSmallT *font_small,
                              esphome::Color col_bg, esphome::Color col_white,
                              esphome::Color col_black, esphome::Color col_gold,
                              esphome::Color col_cyan, esphome::Color col_red,
                              esphome::Color col_green, esphome::Color col_magenta,
                              esphome::Color col_orange, esphome::Color col_dark_gray) {
  struct KidTreat {
    float x, y, vx, vy;
    int kind;
    bool eaten;
  };
  struct ConfettiSpark {
    float x, y, vx, vy;
    esphome::Color col;
  };

  static constexpr int NUM_TREATS = 7;
  static KidTreat treats[NUM_TREATS];
  static float mon_x = 233.0f;
  static float mon_y = 233.0f;
  static float mon_vx = 0.0f;
  static float mon_vy = 0.0f;
  static uint32_t last_phys_ms = 0;
  static uint32_t entry_ms = 0;
  static bool post_play_tared = false;
  static int level_num = 1;
  static int treats_left = NUM_TREATS;
  static int treat_score = 0;
  static int next_kind = 0;
  static uint32_t chomp_timer = 0;
  static int chomp_word_idx = 0;
  static uint32_t level_clear_until_ms = 0;
  static ConfettiSpark confetti[16];

  auto spawn_level_platter = [&](int lv, uint32_t seed_base) {
    treats_left = NUM_TREATS;
    float base_ang = static_cast<float>(lv) * 0.45f;
    for (int i = 0; i < NUM_TREATS; i++) {
      // Distribute 7 treats in a staggered ring/spiral away from the monster
      int s = static_cast<int>(seed_base + i * 73U + lv * 37U);
      float ang = base_ang + static_cast<float>(i) * (2.0f * PI_F / static_cast<float>(NUM_TREATS)) +
                  (hash2d_01(s, 11) - 0.5f) * 0.35f;
      float rad = 76.0f + (static_cast<float>((i * 3) % 4) * 26.0f) + hash2d_01(s, 29) * 18.0f;
      float tx = static_cast<float>(cx) + fast_cos(ang) * rad;
      float ty = std::min(352.0f, static_cast<float>(cy) + fast_sin(ang) * rad);

      // Push away if too close to current monster position
      float dmx = tx - mon_x, dmy = ty - mon_y;
      if (dmx * dmx + dmy * dmy < 4900.0f) {
        tx = static_cast<float>(cx) - dmx * 1.1f;
        ty = static_cast<float>(cy) - dmy * 1.1f;
      }

      // Level 2+: Treats drift around the plate!
      float drift_spd = (lv <= 1) ? 0.0f : std::min(68.0f, 22.0f + static_cast<float>(lv - 2) * 12.0f);
      float v_ang = hash2d_01(s, 47) * (2.0f * PI_F);
      treats[i] = {tx, ty, fast_cos(v_ang) * drift_spd, fast_sin(v_ang) * drift_spd, next_kind++, false};
    }
  };

  if (game_just_entered) {
    last_phys_ms = now_ms;
    entry_ms = now_ms;
    post_play_tared = false;
    mon_x = static_cast<float>(cx);
    mon_y = static_cast<float>(cy);
    mon_vx = 0.0f;
    mon_vy = 0.0f;
    level_num = 1;
    treat_score = 0;
    next_kind = 0;
    chomp_timer = 0;
    level_clear_until_ms = 0;
    pending_game_sfx() = 0;
    spawn_level_platter(1, now_ms);
  } else if (!post_play_tared && (now_ms - entry_ms >= 220U)) {
    post_play_tared = true;
    game_imu_retare_flag() = true;
  }

  float dt = (now_ms >= last_phys_ms) ? ((now_ms - last_phys_ms) * 0.001f) : 0.05f;
  last_phys_ms = now_ms;
  if (dt < 0.012f) dt = 0.012f;
  if (dt > 0.075f) dt = 0.075f;

  // Check if Level Clear & Center-Reset Pause (2000ms total) finished -> Spawn Next Level!
  bool in_celebration = (level_clear_until_ms > 0U && now_ms < level_clear_until_ms);
  if (in_celebration) {
    // Smoothly return Monster to center (cx, cy) and hold still in the center before next level starts!
    mon_vx = 0.0f;
    mon_vy = 0.0f;
    mon_x += (static_cast<float>(cx) - mon_x) * 0.25f;
    mon_y += (static_cast<float>(cy) - mon_y) * 0.25f;
  } else if (level_clear_until_ms > 0U && now_ms >= level_clear_until_ms) {
    level_clear_until_ms = 0U;
    mon_x = static_cast<float>(cx);
    mon_y = static_cast<float>(cy);
    mon_vx = 0.0f;
    mon_vy = 0.0f;
    level_num++;
    spawn_level_platter(level_num, now_ms);
    in_celebration = false;
  }

  // Update drifting / dodging treats on Level 2+
  if (!in_celebration && level_num >= 2) {
    for (int i = 0; i < NUM_TREATS; i++) {
      if (treats[i].eaten) continue;

      // Level 3+: Treats shyly scoot away when the monster charges within 82px!
      if (level_num >= 3) {
        float edx = treats[i].x - mon_x;
        float edy = treats[i].y - mon_y;
        float ed2 = edx * edx + edy * edy;
        if (ed2 < 6724.0f && ed2 > 1.0f) {
          float ed = sqrtf(ed2);
          float evade = (1.0f - ed / 82.0f) * std::min(110.0f, static_cast<float>(level_num - 2) * 32.0f);
          treats[i].vx += (edx / ed) * evade * dt;
          treats[i].vy += (edy / ed) * evade * dt;
        }
      }

      // Clamp treat drift speed
      float max_t_spd = std::min(78.0f, 24.0f + static_cast<float>(level_num - 2) * 14.0f);
      float cur_spd = sqrtf(treats[i].vx * treats[i].vx + treats[i].vy * treats[i].vy);
      if (cur_spd > max_t_spd && cur_spd > 0.1f) {
        treats[i].vx = (treats[i].vx / cur_spd) * max_t_spd;
        treats[i].vy = (treats[i].vy / cur_spd) * max_t_spd;
      }

      treats[i].x += treats[i].vx * dt;
      treats[i].y += treats[i].vy * dt;

      // Bounce treats off the horizontal bottom wall (y = 358)
      if (treats[i].y > 358.0f) {
        treats[i].y = 358.0f;
        if (treats[i].vy > 0.0f) treats[i].vy = -treats[i].vy * 0.9f;
      }
      // Bounce treats inside the plate rim (r = 180)
      float bdx = treats[i].x - static_cast<float>(cx);
      float bdy = treats[i].y - static_cast<float>(cy);
      float bd = sqrtf(bdx * bdx + bdy * bdy);
      if (bd > 180.0f && bd > 0.1f) {
        float nx = bdx / bd, ny = bdy / bd;
        treats[i].x = static_cast<float>(cx) + nx * 180.0f;
        treats[i].y = static_cast<float>(cy) + ny * 180.0f;
        float v_dot = treats[i].vx * nx + treats[i].vy * ny;
        if (v_dot > 0.0f) {
          treats[i].vx -= 1.9f * v_dot * nx;
          treats[i].vy -= 1.9f * v_dot * ny;
        }
      }
    }
  }

  // Find nearest remaining uneaten snack for eye tracking
  float nearest_dist = 999.0f;
  float look_dx = 0.0f;
  float look_dy = 0.0f;

  for (int i = 0; i < NUM_TREATS; i++) {
    if (treats[i].eaten) continue;
    float dx = treats[i].x - mon_x;
    float dy = treats[i].y - mon_y;
    float d = sqrtf(dx * dx + dy * dy);
    if (d < nearest_dist) {
      nearest_dist = d;
      look_dx = dx / (d + 0.1f);
      look_dy = dy / (d + 0.1f);
    }
  }

  // Tap Screen or Press Button -> Playful hop boost in the direction the monster is looking
  if (marble_touch_x >= 0.0f || game_btn_pressed) {
    marble_touch_x = -1.0f;
    game_btn_pressed = false;
    if (!in_celebration && nearest_dist < 900.0f) {
      mon_vx += look_dx * 165.0f;
      mon_vy += look_dy * 165.0f;
    }
  }

  // Roaming Monster Physics (slightly livelier 0.925f damping + tighter 36px mouth-snap)
  float ax = fmaxf(-620.0f, fminf(620.0f, roll_delta * 32.0f));
  float ay = fmaxf(-620.0f, fminf(620.0f, -pitch_delta * 32.0f));

  float pull_x = 0.0f, pull_y = 0.0f;
  if (!in_celebration && nearest_dist < 38.0f) {
    float pull_mag = (1.0f - nearest_dist / 38.0f) * 110.0f;
    pull_x = look_dx * pull_mag;
    pull_y = look_dy * pull_mag;
  }

  float damping = powf(0.925f, dt / 0.05f);
  mon_vx = (mon_vx + (ax + pull_x) * dt) * damping;
  mon_vy = (mon_vy + (ay + pull_y) * dt) * damping;
  mon_x += mon_vx * dt;
  mon_y += mon_vy * dt;

  // Keep Monster above the y = 374 bottom wall (mon_r = 34 -> max mon_y = 340)
  if (mon_y > 340.0f) {
    mon_y = 340.0f;
    if (mon_vy > 0.0f) mon_vy = -mon_vy * 0.6f;
  }
  // Keep Monster inside the circular play bowl (max center r = 172)
  float mdx = mon_x - static_cast<float>(cx);
  float mdy = mon_y - static_cast<float>(cy);
  float mdist = sqrtf(mdx * mdx + mdy * mdy);
  const float max_mon_r = 172.0f;
  if (mdist > max_mon_r && mdist > 0.001f) {
    float nx = mdx / mdist;
    float ny = mdy / mdist;
    mon_x = static_cast<float>(cx) + nx * max_mon_r;
    mon_y = static_cast<float>(cy) + ny * max_mon_r;
    float v_dot = mon_vx * nx + mon_vy * ny;
    if (v_dot > 0.0f) {
      mon_vx -= 1.6f * v_dot * nx;
      mon_vy -= 1.6f * v_dot * ny;
    }
  }

  // Check if the Monster chomped any remaining treat (tighter 36px capture radius)
  if (!in_celebration) {
    for (int i = 0; i < NUM_TREATS; i++) {
      if (treats[i].eaten) continue;
      float dx = treats[i].x - mon_x;
      float dy = treats[i].y - mon_y;
      float dist = sqrtf(dx * dx + dy * dy);

      if (dist < 36.0f) {
        treats[i].eaten = true;
        treats_left--;
        treat_score++;
        chomp_timer = now_ms;
        chomp_word_idx = (chomp_word_idx + 1) % 5;
        if (treat_score > high_score_chomp) {
          high_score_chomp = treat_score;
        }

        if (treats_left <= 0) {
          // All 7 treats on the plate gobbled up -> Level Clear Celebration + Victory Fanfare!
          treat_score += 3; // Level Clear bonus
          if (treat_score > high_score_chomp) {
            high_score_chomp = treat_score;
          }
          level_clear_until_ms = now_ms + 3000U;
          in_celebration = true;
          pending_game_sfx() = 5; // Level Clear Fanfare!

          esphome::Color p_cols[5] = {col_gold, col_cyan, col_magenta, col_green, col_orange};
          for (int c = 0; c < 16; c++) {
            float ang = static_cast<float>(c) * (2.0f * PI_F / 16.0f);
            float spd = 105.0f + static_cast<float>((c % 3) * 34);
            confetti[c].x = mon_x + fast_cos(ang) * 20.0f;
            confetti[c].y = mon_y + fast_sin(ang) * 20.0f;
            confetti[c].vx = fast_cos(ang) * spd;
            confetti[c].vy = fast_sin(ang) * spd - 40.0f;
            confetti[c].col = p_cols[c % 5];
          }
        } else {
          // Regular treat chomped -> Play Cookie-Monster "Nom-Nom" Chomp SFX!
          pending_game_sfx() = 1 + ((treat_score + treats[i].kind) & 3); // Vary across 4 chomp patterns!
        }
        break;
      }
    }
  }

  if (in_celebration) {
    for (int c = 0; c < 16; c++) {
      confetti[c].x += confetti[c].vx * dt;
      confetti[c].y += confetti[c].vy * dt;
      confetti[c].vy += 110.0f * dt;
    }
  }

  bool is_chomping = in_celebration || (chomp_timer > 0U && (now_ms - chomp_timer < 520U));
  bool mouth_open  = (!in_celebration && nearest_dist < 82.0f);

  // ==========================================================================
  // RENDERING PIPELINE
  // ==========================================================================

  // 1. Play Bowl Rim, Bottom Wall (y = 374), Scorekeeper HUD (y = 392) & "< Back" (y = 426)
  it.circle(cx, cy, 208, col_dark_gray);
  it.line(82, 374, 384, 374, col_dark_gray);
  char t_buf[56];
  snprintf(t_buf, sizeof(t_buf), "Lv %d  •  Left:%d/7  •  Score:%d  •  Best:%d",
           level_num, treats_left, treat_score, high_score_chomp);
  it.print(cx, 392, font_small, col_gold, esphome::display::TextAlign::CENTER, t_buf);
  render_back_button(it, cx, font_small, 426, "< Back");

  // 2. Render Remaining Uneaten Treats on the Plate
  float t_bob = now_ms * 0.0045f;
  for (int i = 0; i < NUM_TREATS; i++) {
    if (treats[i].eaten) continue;
    int bob_y = static_cast<int>(fast_sin(t_bob + static_cast<float>(i) * 1.15f) * 2.5f);
    render_chomp_treat(it, static_cast<int>(treats[i].x), static_cast<int>(treats[i].y) + bob_y,
                       treats[i].kind, col_white, col_red, col_gold, col_cyan, col_green, col_magenta, col_bg);
  }

  // 3. Render Roaming Cookie-Monster Friend (wears Party Hat on Level 2+ or during celebration!)
  int imx = static_cast<int>(mon_x);
  int imy = static_cast<int>(mon_y);
  render_chomp_monster(it, imx, imy, 34, look_dx, look_dy, mouth_open, is_chomping,
                       (level_num >= 2 || in_celebration), now_ms,
                       col_white, col_black, col_gold, col_red, col_magenta, col_dark_gray);

  // 4. Level Clear Celebration Banner & Confetti vs. Chomp Word Popup
  if (in_celebration) {
    uint32_t rem_cel = level_clear_until_ms - now_ms;
    char p_msg[36];
    if (rem_cel > 1900U) {
      for (int c = 0; c < 16; c++) {
        it.filled_circle(static_cast<int>(confetti[c].x), static_cast<int>(confetti[c].y), 3, confetti[c].col);
      }
      snprintf(p_msg, sizeof(p_msg), "LEVEL %d CLEAR!", level_num);
      it.print(cx, imy - 62, font_date, col_gold, esphome::display::TextAlign::CENTER, p_msg);
    } else {
      // Quiet centered anticipation beat before the new treats pop onto the plate!
      snprintf(p_msg, sizeof(p_msg), "GET READY! LEVEL %d", level_num + 1);
      it.print(cx, imy - 62, font_date, col_cyan, esphome::display::TextAlign::CENTER, p_msg);
    }
  } else if (is_chomping) {
    static constexpr const char *CHOMP_WORDS[5] = {
        "NOM NOM!", "YUMMY!", "COOKIE!", "CHOMP!", "DELICIOUS!"};
    int pop_y = (imy > 150) ? (imy - 52) : (imy + 52);
    int pop_x = std::max(115, std::min(351, imx));
    it.print(pop_x, pop_y, font_date, col_gold, esphome::display::TextAlign::CENTER,
             CHOMP_WORDS[chomp_word_idx]);
    it.filled_circle(imx - 42, imy - 16, 3, col_gold);
    it.filled_circle(imx + 42, imy - 16, 3, col_cyan);
  }
}
// ==============================================================================
// SECTION 19: GAMES SUITE CAROUSEL LAUNCHER (PAGE 3, STATE 0 — 4 GAMES)
// ==============================================================================

template<typename DisplayT, typename FontTitleT, typename FontBoldT, typename FontSmallT>
inline void render_games_launcher(
    DisplayT &it, int cx,
    int &game_carousel_idx, bool menu_scroll_active, int menu_scroll_step,
    int menu_scroll_dir, int menu_scroll_target_page,
    int high_score_marbles, int high_score_archery, int high_score_breakout,
    int high_score_chomp,
    FontTitleT *font_games_title, FontBoldT *font_btn_bold, FontSmallT *font_small,
    esphome::Color theme_primary, esphome::Color theme_secondary,
    esphome::Color col_bg, esphome::Color col_black, esphome::Color col_white,
    esphome::Color col_silver, esphome::Color col_slate, esphome::Color col_dark_gray,
    esphome::Color col_gold, esphome::Color col_cyan, esphome::Color col_green,
    esphome::Color col_orange, esphome::Color col_red, esphome::Color col_magenta, bool game_sound_on = true) {
  (void)col_silver;
  using esphome::Color;
  using esphome::display::TextAlign;

  if (game_carousel_idx < 0 || game_carousel_idx >= 4) {
    game_carousel_idx = 0;
  }

  auto draw_game_icon = [&](int g_idx, int icon_x, int icon_y) {
    if (g_idx == 0) {
      // GAME 0: MARBLES
      it.filled_circle(icon_x, icon_y, 52, Color(18, 24, 28));
      it.circle(icon_x, icon_y, 52, col_slate);
      it.circle(icon_x, icon_y, 51, col_slate);
      it.circle(icon_x, icon_y, 46, Color(30, 40, 48));

      it.filled_circle(icon_x, icon_y, 15, col_black);
      it.circle(icon_x, icon_y, 15, Color(45, 55, 65));
      it.circle(icon_x, icon_y, 14, Color(45, 55, 65));

      for (int a = 0; a < 4; a++) {
        float sa = a * 1.570796f;
        it.line(icon_x + static_cast<int>(fast_cos(sa) * 18.0f),
                icon_y + static_cast<int>(fast_sin(sa) * 18.0f),
                icon_x + static_cast<int>(fast_cos(sa + 0.8f) * 40.0f),
                icon_y + static_cast<int>(fast_sin(sa + 0.8f) * 40.0f),
                Color(35, 45, 55));
      }
      render_glass_marble(it, icon_x - 24, icon_y - 18, 13, col_red, Color(255, 138, 128));
      render_glass_marble(it, icon_x + 22, icon_y - 22, 12, col_cyan, Color(180, 255, 255));
      render_glass_marble(it, icon_x - 20, icon_y + 22, 11, col_green, Color(185, 246, 202));
      render_glass_marble(it, icon_x + 24, icon_y + 18, 13, col_gold, Color(255, 245, 157));
      render_glass_marble(it, icon_x + 2, icon_y - 31, 10, col_magenta, Color(248, 187, 208));

    } else if (g_idx == 1) {
      // GAME 1: ARCHERY — Centered 5-ring target + bold arrow in bullseye
      render_archery_target(it, icon_x, icon_y, 46, col_white, col_black,
                            col_cyan, col_red, col_gold, col_slate, col_dark_gray);
      int tip_x = icon_x + 2, tip_y = icon_y + 2;
      int tail_x = icon_x + 56, tail_y = icon_y - 36;
      it.line(tip_x, tip_y + 5, tail_x - 8, tail_y + 10, Color(15, 18, 24));
      it.line(tip_x, tip_y + 6, tail_x - 8, tail_y + 11, Color(15, 18, 24));
      it.circle(tip_x, tip_y, 5, Color(180, 110, 0));
      it.filled_triangle(tip_x - 3, tip_y + 2, tip_x + 9, tip_y - 9, tip_x + 12, tip_y - 2, col_white);
      it.line(tip_x, tip_y, tail_x, tail_y, Color(238, 206, 148));
      it.line(tip_x, tip_y - 1, tail_x, tail_y - 1, Color(250, 228, 178));
      it.line(tip_x, tip_y + 1, tail_x, tail_y + 1, Color(182, 138, 76));
      for (int f = 0; f < 9; f++) {
        int fx = tail_x - 3 - f;
        int fy = tail_y + 2 + (f * 2) / 3;
        it.line(fx, fy - 1, fx + 3, fy - 11, Color(255, 52, 52));
        it.line(fx, fy + 1, fx + 10, fy + 4, Color(220, 32, 32));
      }
      it.filled_circle(tail_x + 1, tail_y - 1, 2, col_white);

    } else if (g_idx == 2) {
      // GAME 2: BREAKOUT — 3D Beveled Bricks, Shattering Hit & Compact Paddle
      int b_w = 26, b_h = 11, gap_x = 4, gap_y = 5;
      int start_bx = icon_x - static_cast<int>(2.0f * b_w + 1.5f * gap_x) + b_w / 2;
      int start_by = icon_y - 36;
      Color row_c[3] = {col_gold, col_cyan, col_orange};
      Color row_hi[3] = {Color(255, 245, 165), Color(185, 255, 255), Color(255, 195, 100)};
      for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 4; c++) {
          int bx = start_bx + c * (b_w + gap_x);
          int by = start_by + r * (b_h + gap_y);
          if (r == 2 && c == 2) {
            it.filled_rectangle(bx - 10, by - 5, 5, 4, row_c[r]);
            it.filled_rectangle(bx + 4, by - 7, 5, 4, row_hi[r]);
            it.filled_rectangle(bx - 6, by + 3, 4, 4, row_c[r]);
            it.filled_rectangle(bx + 5, by + 2, 5, 4, row_hi[r]);
            it.filled_circle(bx, by, 2, col_white);
            continue;
          }
          int hp = (r == 0 && (c == 1 || c == 2)) ? 2 : 1;
          Color bc = (hp == 2) ? Color(145, 162, 182) : row_c[r];
          Color hc = (hp == 2) ? Color(230, 240, 252) : row_hi[r];
          render_breakout_brick(it, bx, by, b_w, b_h, bc, hc, hp, hp, col_white);
        }
      }
      // Drop paddle to icon_y + 42 (bottom at +46, matching Archery/Chomp/Marbles bottom alignment!)
      int pad_x = icon_x - 6, pad_y = icon_y + 42, pad_w = 44, pad_h = 8;
      render_breakout_paddle(it, pad_x, pad_y, pad_w, pad_h, theme_primary, col_white);

      // 5-stage ascending comet trail from the lowered paddle up toward the shattering brick
      int ball_x = icon_x + 10, ball_y = icon_y + 7;
      it.filled_circle(pad_x + 3,  pad_y - 7,  2, Color(50, 40, 14));
      it.filled_circle(pad_x + 6,  pad_y - 14, 2, Color(85, 68, 20));
      it.filled_circle(pad_x + 9,  pad_y - 21, 3, Color(130, 102, 28));
      it.filled_circle(ball_x - 3, ball_y + 7, 4, Color(185, 145, 35));
      it.filled_circle(ball_x, ball_y, 5, col_gold);
      it.circle(ball_x, ball_y, 5, col_white);
      it.filled_circle(ball_x - 2, ball_y - 2, 1, col_white);

    } else if (g_idx >= 3) {
      // GAME 3: CHOMP FOR KIDS — Roaming Cookie-Monster Friend Chasing Scattered Treats
      render_chomp_treat(it, icon_x + 30, icon_y - 22, 0,
                         col_white, col_red, col_gold, col_cyan,
                         col_green, col_magenta, col_bg);
      render_chomp_treat(it, icon_x + 38, icon_y + 18, 2,
                         col_white, col_red, col_gold, col_cyan,
                         col_green, col_magenta, col_bg);
      render_chomp_treat(it, icon_x - 34, icon_y - 26, 1,
                         col_white, col_red, col_gold, col_cyan,
                         col_green, col_magenta, col_bg);
      render_chomp_monster(it, icon_x - 8, icon_y + 6, 32, 0.85f, -0.25f,
                           true, false, false, 0,
                           col_white, col_black, col_gold,
                           col_red, col_magenta, col_dark_gray);
    }
  };

  auto draw_game_card = [&](int g_idx, int icon_x, int icon_y, int text_center_x, int title_y, int score_y) {
    const char *title = "MARBLES";
    int score = high_score_marbles;
    if (g_idx == 1) {
      title = "ARCHERY";
      score = high_score_archery;
    } else if (g_idx == 2) {
      title = "BREAKOUT";
      score = high_score_breakout;
    } else if (g_idx >= 3) {
      title = "CHOMP FOR KIDS";
      score = high_score_chomp;
    }

    if (icon_y >= 40 && icon_y <= 380) {
      draw_game_icon(g_idx, icon_x, icon_y);
    }
    if (title_y >= 140 && title_y <= 295) {
      it.print(text_center_x, title_y, font_btn_bold, col_white, TextAlign::CENTER, title);
    }
    if (score_y >= 140 && score_y <= 295) {
      const char *lbl_str = "HIGH SCORE:";
      char val_str[20];
      if (score <= 0) {
        snprintf(val_str, sizeof(val_str), "--");
      } else if (score >= 1000000) {
        snprintf(val_str, sizeof(val_str), "%d,%03d,%03d", score / 1000000, (score / 1000) % 1000, score % 1000);
      } else if (score >= 1000) {
        snprintf(val_str, sizeof(val_str), "%d,%03d", score / 1000, score % 1000);
      } else {
        snprintf(val_str, sizeof(val_str), "%d", score);
      }

      int w_lbl = 0, w_val = 0, x_off = 0, base = 0, th = 0;
      font_small->measure(lbl_str, &w_lbl, &x_off, &base, &th);
      font_small->measure(val_str, &w_val, &x_off, &base, &th);
      const int gap_px = 8;
      int start_x = text_center_x - (w_lbl + gap_px + w_val) / 2;

      it.print(start_x, score_y, font_small, col_gold, TextAlign::CENTER_LEFT, lbl_str);
      it.print(start_x + w_lbl + gap_px, score_y, font_small, col_white, TextAlign::CENTER_LEFT, val_str);
    }
  };

  const int icon_cx = 110;
  const int card_cy = 220;
  const int text_cx = 285;
  const int base_title_y = 200;
  const int base_score_y = 240;

  if (menu_scroll_active) {
    int step = menu_scroll_step;
    if (step > 4) step = 4;
    int offset = SHARED_SCROLL_EASE_174[step];
    int out_shift = -menu_scroll_dir * offset;
    int in_shift = menu_scroll_dir * (174 - offset);

    draw_game_card(game_carousel_idx, icon_cx, card_cy + out_shift, text_cx, base_title_y + out_shift, base_score_y + out_shift);
    draw_game_card(menu_scroll_target_page, icon_cx, card_cy + in_shift, text_cx, base_title_y + in_shift, base_score_y + in_shift);
  } else {
    draw_game_card(game_carousel_idx, icon_cx, card_cy, text_cx, base_title_y, base_score_y);
  }

  // Top & Bottom Viewport Masks
  it.filled_rectangle(0, 0, 466, 158, col_bg);
  it.print(cx, 96, font_games_title, theme_primary, TextAlign::CENTER, "GAMES");
  it.filled_rectangle(0, 282, 466, 184, col_bg);

  // Combined [Speaker (44px) + 28px gap + PLAY (154px) = 226px] centered at cx = 233, y = 352 (aligned with Home Dock!)
  render_pill_button(it, 214, 324, 352, 22, theme_primary, col_dark_gray, "PLAY", col_white, font_btn_bold);

  // Game Sound Mute / Unmute Button at (142, 352, r = 22 -> outer left edge = 120)
  const int spk_x = 142, spk_y = 352;
  it.filled_circle(spk_x, spk_y, 22, col_dark_gray);
  it.circle(spk_x, spk_y, 22, game_sound_on ? theme_secondary : col_slate);
  it.circle(spk_x, spk_y, 21, game_sound_on ? theme_secondary : col_slate);
  Color spk_c = game_sound_on ? col_white : col_slate;
  // Speaker body & cone
  it.filled_rectangle(spk_x - 9, spk_y - 4, 5, 9, spk_c);
  it.filled_triangle(spk_x - 4, spk_y - 4, spk_x - 4, spk_y + 4, spk_x + 2, spk_y + 9, spk_c);
  it.filled_triangle(spk_x - 4, spk_y - 4, spk_x + 2, spk_y + 9, spk_x + 2, spk_y - 9, spk_c);
  if (game_sound_on) {
    // Sound wave arcs
    it.line(spk_x + 5, spk_y - 4, spk_x + 7, spk_y - 2, theme_primary);
    it.line(spk_x + 7, spk_y - 2, spk_x + 7, spk_y + 2, theme_primary);
    it.line(spk_x + 7, spk_y + 2, spk_x + 5, spk_y + 4, theme_primary);
    it.line(spk_x + 8, spk_y - 7, spk_x + 11, spk_y - 3, theme_secondary);
    it.line(spk_x + 11, spk_y - 3, spk_x + 11, spk_y + 3, theme_secondary);
    it.line(spk_x + 11, spk_y + 3, spk_x + 8, spk_y + 7, theme_secondary);
  } else {
    // Bold crimson diagonal mute slash across speaker
    it.line(spk_x - 11, spk_y - 10, spk_x + 10, spk_y + 11, col_red);
    it.line(spk_x - 10, spk_y - 10, spk_x + 11, spk_y + 11, col_red);
    it.line(spk_x - 11, spk_y - 9,  spk_x + 10, spk_y + 12, col_red);
  }

  // Right-Flank Continuous Carousel Chevrons
  it.filled_rectangle(395, 140, 71, 160, col_bg);
  render_scroll_chevron(it, 415, 162, true, theme_secondary, col_black, false);
  render_scroll_chevron(it, 415, 278, false, theme_secondary, col_black, false);
}
}  // namespace pocket_ui