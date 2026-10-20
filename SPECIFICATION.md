# Pocket Assistant — Technical Specification (v2.0)
**Hardware Platform**: Waveshare ESP32-S3-Touch-AMOLED-1.75C  
**Framework**: ESPHome 2026.10.0+ / ESP-IDF 5.5.6+  
**Target Resolution**: 466 × 466 px Circular Display  

---

## 1. Hardware Architecture & Pinout Map (`boards/waveshare_175c.yaml`)

### 1.1 Microcontroller, Flash & PSRAM Memory Architecture
* **MCU**: Espressif ESP32-S3 (Dual-core Xtensa LX7 @ 240 MHz).
* **Flash**: 32MB GigaDevice Octal SPI (`flash_size: 32MB`, `flash_chip: gd` under `esp32: framework: advanced:` trimming unused vendor tables to save ~5 KB of internal IRAM and Flash).
* **PSRAM**: 8MB Octal SPI @ 80 MHz (`mode: octal`, `speed: 80MHz`, `ignore_not_found: false`, `nvs_cache_in_psram: true` freeing ~10 KB of internal SRAM heap, `CONFIG_FREERTOS_TASK_CREATE_ALLOW_EXT_MEM: "y"`, and XIP instruction/rodata fetch).
* **Read-Only Data Placement (`.rodata`)**: Precomputed trigonometric LUTs (`DIAL_SIN[60]`, `DIAL_COS[60]`), circular chord clipping table (`constexpr CIRCLE_HW[466]`, saving 932 B of `.bss` RAM), telescopic Lunar Albedo LUTs (`LUNAR_ALBEDO_139` & `LUNAR_ALBEDO_45`), 3D Fibonacci sphere LUT (`FIB_SPHERE_36`), and parameterized TV remote command tables reside in Flash (`.rodata`).

### 1.2 Display Subsystem
* **Controller**: CO5300 QSPI Controller (`mipi_spi`, defined in `boards/waveshare_175c.yaml` and extended via `!extend disp1` in `core/ui.yaml`).
* **Panel**: 1.75-inch Circular AMOLED, 466 × 466 resolution, 16.7M colors (RGB565 over QSPI).
* **Interface Bus**: Quad-SPI (`display_qspi`) running at **80 MHz** (`data_rate: 80MHz`).
* **Pinout**:
  * CLK: `GPIO38`
  * D0..D3: `GPIO4`, `GPIO5`, `GPIO6`, `GPIO7`
  * CS: `GPIO12`
  * Reset: `GPIO1`

### 1.3 Capacitive Touch Subsystem
* **Controller**: Hynitron CST9220 (`id: touch_screen` in `boards/waveshare_175c.yaml`, extended via `!extend touch_screen` in `core/ui.yaml`).
* **Bus**: I2C `bus_internal` (`SDA: GPIO15`, `SCL: GPIO14` @ 400 kHz).
* **Interrupt & Reset**: `INT: GPIO11` (`INPUT_PULLUP`), `RST: GPIO2` (Active Low).
* **Sampling Rate**: 30 Hz (33ms cycle) with standardized debounce constants (`TOUCH_DEBOUNCE_RAPID = 100ms`, `TOUCH_DEBOUNCE_BUTTON = 150ms`, `TOUCH_DEBOUNCE_SCROLL = 250ms` in `ui_core.h`) and full-chin `< BACK` hit zone (`y in [385..465]`, `x in [50..416]`).

### 1.4 Power Management (PMIC) & Battery Fuel Gauge
* **Controller**: X-Powers AXP2101 PMIC (`Address: 0x34` on `bus_internal`).
* **Interrupt**: `GPIO3` (Hardware PMU IRQ, Active Low with `GPIO_PULLUP_ENABLE` gating the 20 Hz Crown button check to eliminate >95% of idle I2C bus traffic per ADR-001).
* **Power Authority**: Top Crown hardware PEK button (Side button `boot_button` on `GPIO0` is reserved exclusively for Voice Assistant push-to-talk, alert dismissal, and game actions).
* **Power Rails**:
  * `ALDO1`: 3.3V (Touch CST9220 power rail).
  * `ALDO2`: 3.3V (Display CO5300 logic / analog VCI rail).
  * `BLDO1`: 1.8V (Display VDDIO logic rail).
  * `ALDO3`: 3.3V (Audio DAC/ADC analog rail).
  * `DCDC1`: 3.3V (System main core rail).

### 1.5 Audio Subsystem
* **DAC**: Everest Semiconductor ES8311 (`Address: 0x18`, Mono Speaker DAC).
* **ADC**: Everest Semiconductor ES7210 (`Address: 0x40`, Dual-channel Microphone ADC @ 16 kHz, 37.5 dB gain).
* **Bus**: Dual I2S Audio Bus with shared clock lines (`LRCK: GPIO45`, `BCLK: GPIO9`, `MCLK: GPIO16`) dynamically switched at runtime via HAL scripts (`hal_route_i2s_to_spk` / `hal_route_i2s_to_mic`).
* **Speaker Output (DOUT)**: `GPIO8` (`physical_speaker`, driven via FreeRTOS `resampler` + `mixer` pipeline with PSRAM task stacks, 15s `announcement_mixing_input`, and `microFLAC v0.3.0` PSRAM decode buffers).
* **Microphone Input (DIN)**: `GPIO10` (`pocket_mic`).
* **Amp Enable (PA_EN)**: `GPIO46` (`speaker_enable`).

### 1.6 Inertial Measurement Unit (IMU)
* **Sensor**: QST QMI8658 (`Address: 0x6B`, 6-Axis Accelerometer & Gyroscope).
* **Bus**: I2C `bus_internal` (`SDA: GPIO15`, `SCL: GPIO14`).
* **Power Modes (`CTRL7 = 0x08` via `hal_set_imu_power_mode`)**: `0x03` in Active mode (Accel + Gyro), `0x01` in Tier 2 Standby with Pickup Wake enabled (Accel only), `0x00` in Tier 2 Standby with Pickup Wake disabled or Tier 3 Hibernate (Power-down).

---

## 2. Power Architecture, Outgoing API & Performance Budgets

### Power State Hierarchy
| State | Current Draw | Entry Condition | Wake Mechanism | Wake Latency |
| :--- | :--- | :--- | :--- | :--- |
| **Tier 1 (Active)** | 110 – 160 mA | Active touch / Crown click | Instant | 0 ms |
| **Tier 2 (Display Standby)** | 15 – 25 mA | Inactivity timeout / Crown click | Top Crown (`GPIO3`), In-hand pickup (`QMI8658`), Touch tap | < 5 ms |
| **Tier 3 (Hardware Hibernate)** | < 40 µA | Hibernate timeout / Long Crown press | Top Crown hardware PEK interrupt or USB VBUS | Cold boot (~400ms UI, <1s API) |

### NVS Flush, Smart On-Demand BLE & Client-Initiated Outgoing API (`ESPHome 2026.10+`)
* **Minimum Platform Dependency**: Client-initiated dial-back (`api: outgoing_connection:`) requires **ESPHome `2026.10.0+`** and **Home Assistant Core `2026.10.0+`** (`aioesphomeapi >= 46.4.0`).
* **Smart On-Demand BLE (`esp32_ble` + `improv_ble`)**: Configured with `enable_on_boot: false` and `use_psram: true` (`+40 KB` internal SRAM saved, `0` 2.4 GHz radio coexistence contention on normal boot/wake). Automatically enables at boot if no Wi-Fi credentials exist, after 15 seconds if configured Wi-Fi is unreachable while awake, or manually via `System -> Actions -> BLE Setup`.
* **Pre-Shutdown NVS Sync & Clean API Disconnect**: Before writing `0x10 = 0x01` to the AXP2101 PMIC in `enter_deep_sleep` (`hal_enter_hardware_hibernate`), firmware invokes `clean_disconnect_api_before_hibernate(id(va))` and `global_preferences->sync()` so Home Assistant closes the socket cleanly and all dirty NVS preferences are committed to flash prior to rail cutoff.

### Frame Rendering, 5-Header C++20 Toolkit (`ui_toolkit.h`) & Timing Budgets
* **20 FPS / 13.3 FPS Audio-Aware Pacer (`should_step_ambient_anim`)**: Docked Screensavers (`Aurora Borealis`, `Starfield`, `Matrix Rain`, `Fireworks`), Sky & Weather motion wallpapers, Voice Assistant visual profiles (`Nebula`, `Solaris`, `Jarvis`, `Alexa`, `Gemini`), active Games (`1..4`), and interactive Tools (`Stopwatch`, `Bubble Level`, `Dice Roll`, `Coin Flip`) run at **20 FPS (`42ms` cadence)** when local audio is idle, and automatically pace at **`~13.3 FPS` (`75ms`)** during active Sendspin/TTS FLAC streaming.
* **Modular 5-Header C++20 UI Architecture (`namespace pocket_ui`)**:
  1. `ui_core.h`: Overridable display geometry (`POCKET_UI_SCREEN_W`, `POCKET_UI_ROUND_DISPLAY`), `.rodata` LUTs (`DIAL_SIN/COS`, `CIRCLE_HW`), 3D projection (`Vec3`, `rotate_3d_xyz`, `project_3d`), album art blitter, 280-glyph UTF-8 sanitizer, and 30 Hz touch helpers.
  2. `ui_ambient_va.h`: 20 FPS Docked Screensavers, zero-sqrt `render_progress_arc_r224`, and 5-Profile 20 FPS Voice Assistant engine.
  3. `ui_weather_sky.h`: `.rodata` Lunar Albedo LUTs (`LUNAR_ALBEDO_139/45`), zero-sqrt `render_lunar_disc`, procedural 3D `render_cumulus_cloud` & `render_wispy_cloud_veil`, and 20 FPS Weather Motion suite.
  4. `ui_games.h`: 360° Gimbal-Lock-Free IMU Tilt (`compute_3d_game_tilt`) and 4-Game 20 FPS Arcade suite.
  5. `ui_tools_apps.h`: 8-Tool Suite, Clock, Music Player, TV Remote, and System Hub.

---

## 3. Home Assistant All-in-One Companion Architecture (6-Entity Single-File Contract)

All Home Assistant backend entities are consolidated into a single drop-in package file (`homeassistant/packages/pocket_assistant_companion.yaml`) requiring zero Blueprint imports:

### 3.1 State Helpers (`input_text`)
1. `input_text.pocket_assistant_active_speaker`: Active target media player entity ID (defaults to `media_player.pocket_assistant`).
2. `input_text.pocket_assistant_weather_entity`: Monitored weather entity ID (defaults to `weather.forecast_home`, with automatic fallback discovery to `weather.home` or the first valid `weather.*` entity).
3. `input_text.pocket_assistant_base_url`: Optional custom base URL override (defaults to `http://homeassistant.local:8123` if blank).

### 3.2 Template Sensors (`sensor`)
4. `sensor.pocket_assistant_active_media`: Idle-guarded playback state and metadata mirror (`title`, `artist`, `album`, `duration`, `position`, `volume`, `media_type`, `supported_features`, `art_url`). Evaluates on the 2-second tick only while the target speaker is `playing`/`buffering` or when state/title/volume changes.
5. `sensor.pocket_assistant_weather_forecast`: Hourly 5-day formatted forecast serializer (`DAY:HIGH:LOW:COND;...` state + `high` and `low` attributes).

### 3.3 Embedded Music Assistant Browse Script (`script`)
6. `script.pocket_assistant_music_browse`: Server-side pagination engine for Music Assistant speakers, favorites, playlists, artists, albums, radio, podcasts (with `mass_queue.get_podcast_episodes` support), audiobooks, album tracks, and active queues. Automatically receives `target_device: "${name}"` from the device and pushes 5-slot pages back via `esphome.<target_device>_set_browse_slots`.

---

## 4. Hub-and-Spoke Application Architecture (8 Core Pages)

Pocket Assistant v2.0 uses a hub-and-spoke navigation model anchored on the **Interactive Home Hub (`gui_page == 1`, default Start Screen)** with a universal low-profile **`< BACK`** chin target (`y = 426`):

| Page (`gui_page`) | Application Name | Primary Function & Controls |
| :--- | :--- | :--- |
| **1** | **Home Hub (Default)** | 20 FPS live weather motion wallpaper (`Home: Weather Background`), date header (`y = 52` -> `HOME SETTINGS`), 77pt hero time (`y = 128` -> `Clock`), weather summary (`y = 200` -> `Sky & Weather`), Smart Live Activity Stack (`y = 264` for active Timer, Stopwatch, Music, Alarm), 4-icon Quick-Launch Dock (`y = 356`), and bottom chin Battery + Settings Gear (`y = 424` -> `System Hub`). |
| **0** | **Clock** | Analog (`.rodata` LUT sword hands) and digital watch faces, 12h/24h time format, date, battery fuel gauge (`y = 348`), and `"HOME"` chin return (`y = 408`). |
| **2** | **Music Player** | Full-bleed AMOLED album art, 5.0s auto-fading HUD, Track & Album Inspector (`Mode 6` with equator scrubber, `-15s/+15s` seek, album tracks, artist discography, and queue), and 5-slot Music Assistant browser. |
| **7** | **TV Remote** | Universal 4-device media remote (`tv_remote_entity`..`4`) with independent per-slot platform profiles (`Remote: Slot 1..4 Profile`, defaulting to `"Auto-Detect"`), Slot-1 Master Power, per-device power overrides, radial `r <= 146 px` D-pad, and 7 native protocol engines (Android TV/Shield, Denon/Marantz AVR, LG webOS, Apple TV, Roku, Samsung TV, Custom). |
| **5** | **Sky & Weather** | 20 FPS meteorological & astronomical suite (`render_weather_page`) with live weather motion wallpaper, `-5px` optical hero temp + 3px degree ring, 5-day forecast, Sun arc, `.rodata` telescopic Lunar Albedo LUTs (`LUNAR_ALBEDO_139` & `LUNAR_ALBEDO_45`), and `r = 26` filled-core subpage buttons. |
| **3** | **Games Suite** | Vertical slot-machine reel launcher (`render_games_launcher`) with audio mute toggle and 4 continuous-wave 20 FPS arcade games: Marbles, Archery, Breakout, and Chomp for Kids. |
| **8** | **Tools Suite** | 2-page $2 \times 2$ micro-app launcher hosting 8 utilities with monochromatic filled-core iconography. |
| **4** | **System Hub** | 2-page vertical menu (`render_system_page`): Page 1 (`Display`, `Sleep & Wake`, `Sound`, `Voice`) and Page 2 (`Music`, `Home`, `Info`, `Actions`), mirrored 1:1 with Home Assistant configuration categories. |

---

## 5. Tools Suite Architecture (Page 8)

| Subpage | Tool Name | Description & Key Mechanics |
| :--- | :--- | :--- |
| **1** | **Timers** | 3-slot multi-timer manager (`MultiTimerSlot[3]`) with 1-tap play/pause cards, detail editor, perimeter progress arc, and Home Assistant Assist voice timer synchronization (`on_timer_started` / `updated` / `cancelled` / `finished`). |
| **2** | **Alarms** | 3-slot multi-alarm manager (Daily, Weekdays, Weekends, Once) with toggle switches, dedicated edit views, and Crown snooze/dismiss. |
| **3** | **Stopwatch** | Vintage 1970 Heuer Swiss chronometer with physical Crown tactile Start/Stop pusher (200ms dwell compensation), LAP/RESET buttons, racing red split readout, and Rattrapante ghost hand. |
| **4** | **Dice Roll** | Dual simultaneous D6 dice rolling with back-face culled 3D tumbling cubes, hardware TRNG entropy, and omnidirectional shake/tap triggers. |
| **5** | **Flashlight** | 100% full-screen AMOLED pure white illumination torch with sleep inhibition and full-screen tap-to-exit. |
| **6** | **Calculator** | Keypad calculator with active operator highlight, tip/discount percentages, non-throwing `sscanf` parsing, and auto-reset lifecycle. |
| **7** | **Bubble Level** | Precision 20 FPS `300px` spirit level utilizing QMI8658 IMU trigonometry with EMA liquid damping and sleep inhibition while leveling. |
| **8** | **Coin Flip** | Commemorative 3D struck silver Home Assistant obverse (Assist smile) and reverse (Network graph) medallion with 11-frame milled-edge flip physics and shake/tap triggers. |

---

## 6. Games Suite Architecture (Page 3)

The Games Suite features a vertical reel carousel launcher with high-contrast vector symbols, game audio mute/unmute control, and NVS high-score retention across 4 20 FPS arcade games:
1. **Marbles**: Precision 3D IMU tilt arena with 5 Cat's-Eye glass marbles, center capture hole, and progressive diamond bumper pegs.
2. **Archery**: Timed multi-round recurve bow range with swaying 5-ring target, crosswind ballistics, and pinned arrows.
3. **Breakout**: Circular dome brick-breaker with multi-hit armored bricks, shattering shrapnel, and angle-steered capsule paddle.
4. **Chomp for Kids**: Kid-friendly tilt game where a hungry furry monster rolls around the play bowl gobbling up scattered cookies and treats with party milestones.
