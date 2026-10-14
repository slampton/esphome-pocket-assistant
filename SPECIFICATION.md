# Pocket Assistant — Technical Specification (v1.3)
**Hardware Platform**: Waveshare ESP32-S3-Touch-AMOLED-1.75C  
**Framework**: ESPHome 2026.10.0+ / ESP-IDF 5.5+  
**Target Resolution**: 466 × 466 px Circular Display  

---

## 1. Hardware Architecture & Pinout Map

### 1.1 Microcontroller, Flash & PSRAM Memory Architecture
* **MCU**: Espressif ESP32-S3 (Dual-core Xtensa LX7 @ 240 MHz).
* **Flash**: 32MB GigaDevice Octal SPI (`flash_size: 32MB`, `flash_chip: gd` under `esp32: framework: advanced:` trimming unused vendor tables to save ~5 KB of internal IRAM and Flash).
* **PSRAM**: 8MB Octal SPI @ 80 MHz (`mode: octal`, `speed: 80MHz`, `ignore_not_found: false`, `nvs_cache_in_psram: true` freeing ~10 KB of internal SRAM heap).
* **Read-Only Data Placement (`.rodata`)**: Precomputed trigonometric LUTs (`DIAL_SIN[60]`, `DIAL_COS[60]`), circular chord clipping table (`constexpr CIRCLE_HW[466]`, saving 932 B of `.bss` RAM), RLE cloud spans (`CLOUD_SPANS_PACKED`), weather/screensaver particle tables (`ui_toolkit.h`), and parameterized TV remote command tables (`remote_send_key`) reside in Flash (`.rodata`).

### 1.2 Display Subsystem
* **Controller**: CO5300 QSPI Controller (`mipi_spi`).
* **Panel**: 1.75-inch Circular AMOLED, 466 × 466 resolution, 16.7M colors (RGB565 over QSPI).
* **Interface Bus**: Quad-SPI (`display_qspi`) running at **80 MHz** (`data_rate: 80MHz`).
* **Pinout**:
  * CLK: `GPIO38`
  * D0 (MOSI): `GPIO4`
  * D1 (MISO): `GPIO5`
  * D2 (WP): `GPIO6`
  * D3 (HOLD): `GPIO7`
  * CS: `GPIO12`
  * Reset: `GPIO1`

### 1.3 Capacitive Touch Subsystem
* **Controller**: Hynitron CST9220.
* **Bus**: I2C `bus_internal` (`SDA: GPIO15`, `SCL: GPIO14` @ 400 kHz).
* **Reset**: `GPIO2` (Active Low).
* **Sampling Rate**: 30 Hz (33ms cycle) with standardized debounce constants (`TOUCH_DEBOUNCE_RAPID = 100ms`, `TOUCH_DEBOUNCE_BUTTON = 150ms`, `TOUCH_DEBOUNCE_SCROLL = 250ms` in `ui_toolkit.h`) and full-chin `< BACK` hit zone (`y in [385..465]`, `x in [50..416]`).

### 1.4 Power Management (PMIC) & Battery Fuel Gauge
* **Controller**: X-Powers AXP2101 PMIC.
* **Bus**: I2C `bus_internal` (`Address: 0x34`).
* **Interrupt**: `GPIO3` (Hardware PMU IRQ, Active Low).
* **Power Authority**: Top Crown hardware PEK button (Side button `GPIO0` is reserved exclusively for Voice Assistant push-to-talk and game actions).
* **Power Rails**:
  * `ALDO1`: 3.3V (Touch CST9220 power rail).
  * `ALDO2`: 3.3V (Display CO5300 logic / analog VCI rail).
  * `BLDO1`: 1.8V (Display VDDIO logic rail).
  * `ALDO3`: 3.3V (Audio DAC/ADC analog rail).
  * `DCDC1`: 3.3V (System main core rail).

### 1.5 Audio Subsystem
* **DAC**: Everest Semiconductor ES8311 (`Address: 0x18`, Mono Speaker DAC).
* **ADC**: Everest Semiconductor ES7210 (`Address: 0x40`, Dual-channel Microphone ADC @ 16 kHz, 37.5 dB gain).
* **Bus**: Dual I2S Audio Bus with shared clock lines (`LRCK: GPIO45`, `BCLK: GPIO9`, `MCLK: GPIO16`) dynamically switched at runtime via ESP32-S3 ROM GPIO matrix routing (`route_i2s_to_spk` / `route_i2s_to_mic`).
* **Speaker Output (DOUT)**: `GPIO8` (driven via FreeRTOS `resampler` + `mixer` pipeline with PSRAM task stacks and FLAC decode buffers).
* **Microphone Input (DIN)**: `GPIO10`.
* **Amp Enable (PA_EN)**: `GPIO46`.

### 1.6 Inertial Measurement Unit (IMU)
* **Sensor**: QST QMI8658 (`Address: 0x6B`, 6-Axis Accelerometer & Gyroscope).
* **Bus**: I2C `bus_internal` (`SDA: GPIO15`, `SCL: GPIO14`).
* **Power Modes (`CTRL7 = 0x08`)**: `0x03` in Active mode (Accel + Gyro), `0x01` in Tier 2 Standby (Accel only for motion pickup wake), `0x00` in Tier 3 Hibernate (Power-down).

---

## 2. Power Architecture, Outgoing API & Performance Budgets

### Power State Hierarchy
| State | Current Draw | Entry Condition | Wake Mechanism | Wake Latency |
| :--- | :--- | :--- | :--- | :--- |
| **Tier 1 (Active)** | 110 – 160 mA | Active touch / Crown click | Instant | 0 ms |
| **Tier 2 (Display Standby)** | 15 – 25 mA | Inactivity timeout / Crown click | Top Crown (`GPIO3`), In-hand pickup (`QMI8658`) | < 5 ms |
| **Tier 3 (Hardware Hibernate)** | < 40 µA | Hibernate timeout / Long Crown press | Top Crown hardware PEK interrupt only | Cold boot (~400ms UI, <1s API) |

### NVS Flush & Client-Initiated Outgoing API (`ESPHome 2026.10+` & `Home Assistant 2026.10+`)
* **Minimum Platform Dependency**: Client-initiated dial-back (`api: outgoing_connection:`) requires **ESPHome `2026.10.0+`** and **Home Assistant Core `2026.10.0+`** (`aioesphomeapi >= 46.4.0`, which wired up the inbound listener for client-initiated ESPHome connections).
* **Pre-Shutdown NVS Sync**: Before writing `0x10 = 0x01` to the AXP2101 PMIC in `enter_deep_sleep`, firmware explicitly invokes `global_preferences->sync()` so all dirty NVS preferences (`outgoing_connection` dial-back target IP, `saved_battery_level`, and UI configuration selects) are committed to flash prior to rail cutoff.
* **Client-Initiated Native API (`api: outgoing_connection:`)**: Uses zero-IP dial-back learning mode (`ESPPreferenceObject`) protected by Noise encryption (`api: encryption:`) so the ESP32-S3 automatically remembers Home Assistant's IP and initiates an outbound TCP connection immediately upon Wi-Fi association after waking from Hibernate.

### Frame Rendering, C++ Toolkit (`ui_toolkit.h`) & Timing Budgets
* **Chronograph & Standard Pages**: Target 10 FPS (100ms cycle).
* **Render Execution**: < 45ms per frame (`~12ms` QSPI DMA transfer).
* **CPU Idle Window**: $\ge$ 55ms guaranteed idle time per frame cycle to ensure zero-latency touch servicing.
* **Zero-Heap Hot Path**: Select option comparisons use zero-copy `esphome::StringRef` (`id(...).current_option()`), and option cyclers (`cycle_option` in `ui_toolkit.h`) operate over `std::initializer_list<const char *>` with zero heap allocations.

---

## 3. Home Assistant Companion Architecture (2-Sensor Contract)

### 3.1 Weather Sensor
* **Entity**: `sensor.pocket_assistant_weather_forecast`
* **State**: 5-day formatted string (`DAY:HIGH:LOW:COND;...`).
* **Attributes**: `high` (float), `low` (float).
* **Device Subscriptions**: Subscribes strictly to rendered attributes (`temperature`, `humidity`, `wind_speed`, `wind_bearing`, plus forecast `high` and `low`), avoiding unnecessary API subscription overhead.

### 3.2 Active Media Sensor (Idle-Guarded)
* **Entity**: `sensor.pocket_assistant_active_media`
* **State**: Playback state (`playing`, `paused`, `idle`).
* **Attributes**: `title`, `artist`, `album`, `duration`, `position`, `volume`, `media_type`, `art_url`.
* **Idle Polling Guard**: A template `condition:` gates the 2-second `time_pattern` trigger so attribute templates are evaluated only while the target speaker is `playing`/`buffering` or when its state, title, or volume changes—eliminating >95% of idle template evaluations in Home Assistant.

### 3.3 State Helpers (`input_text`)
* `input_text.pocket_assistant_active_speaker`: Active target media player entity ID.
* `input_text.pocket_assistant_weather_entity`: Monitored weather entity ID.
* `input_text.pocket_assistant_base_url`: Optional custom base URL override.

---

## 4. Canonical Application Deck (The 7 Core Apps)

The top-level App Deck carousel features 7 core applications with contiguous default order ranks (`1..7`). Dedicated timekeeping utilities (Stopwatch and Timer/Alarms) live inside the Tools Suite (`Page 8`).

| Page (`gui_page`) | Application Name | Primary Function & Controls |
| :--- | :--- | :--- |
| **0** | **Clock** | Analog and digital watch faces, time format cycler, date and battery complication displays. |
| **2** | **Music Player** | Full-bleed AMOLED album art, 5.0s auto-fading HUD, direct shortcut back-routing (`music_direct_nav`), `-10s/+30s` speech seek icons, parameterized `play_music_slot(slot)`, and 5-slot library browser. |
| **7** | **TV Remote** | Universal media remote with 5-way D-pad, equatorial dead-zone, and parameterized `remote_send_key(0..9)` supporting Android TV, Apple TV, Roku, Samsung TV, LG webOS (`webostv.button`), and Custom. |
| **5** | **Sky & Weather** | Astronomical & weather suite with current conditions, 5-day aligned forecast, Sun arc, and 3D raytraced Moon phases (`ui_toolkit.h`). |
| **3** | **Games Suite** | Vertical slot-machine reel launcher (`Orbitron Bold` header) featuring Breakout, Marble Maze, Archery Target, Treat Catcher, and Asteroids. |
| **8** | **Tools Suite** | 2-page $2 \times 3$ micro-app launcher: Countdown Timer, Multi-Alarm Manager, Heuer Chronometer, Dual D6 Dice, Flashlight Torch, Pocket Calculator, 2D Bubble Level, and HA Coin Flip. |
| **4** | **System Hub** | 2-page vertical menu (4 full-width pills per page with right-flank chevrons): Page 1 (`Display >`, `Clock >`, `Sleep & Wake >`, `Audio >`) and Page 2 (`App Deck >`, `Voice >`, `Info >`, `Actions >`). |

*Note: Legacy top-level carousel allocations `gui_page == 1` (Stopwatch) and `gui_page == 6` (Timer & Alarm) redirect transparently to Tools Subpage 3 and Subpage 1 respectively.*

---

## 5. Tools Suite Architecture (Page 8)

The Tools Suite operates as a modular, high-efficiency utility hub organized across a 2-page $2 \times 3$ grid navigated by right-flank chevrons ($x = 415$):

| Subpage | Tool Name | Description & Key Mechanics |
| :--- | :--- | :--- |
| **1** | **Timer** | Countdown timer with active perimeter progress ticks, quick steppers, Assist voice timer integration, and Crown exit to Tools menu. |
| **2** | **Alarms** | Multi-alarm manager (Daily, Weekdays, Weekends, Once) with dedicated edit views and Crown snooze/dismiss. |
| **3** | **Stopwatch** | Vintage 1970 Heuer Swiss chronometer with physical Crown tactile Start/Stop pusher (200ms dwell compensation), radial Euclidean LAP/RESET hitboxes, racing red split readout, and Rattrapante ghost hand. |
| **4** | **Dice Roll** | Dual simultaneous D6 dice rolling with 3D ivory cubes, hardware TRNG entropy, and omnidirectional shake/tap triggers. |
| **5** | **Flashlight** | 100% full-screen AMOLED pure white illumination torch with full-screen tap-to-exit. |
| **6** | **Calculator** | 4-quadrant Voronoi-partitioned keypad calculator with active entry clear on operator press, tip/discount percentages, and auto-reset lifecycle. |
| **7** | **Bubble Level** | Precision 2D spirit level utilizing QMI8658 IMU trigonometry with dynamic green leveling ring and numeric pitch/roll readouts. |
| **8** | **Coin Flip** | Commemorative Home Assistant obverse (Assist smile) and reverse (Network graph) coin with parabolic 3D flip physics and shake/tap triggers. |

---

## 6. Games Suite Architecture (Page 3)

The Games Suite features a vertical reel carousel launcher with high-contrast vector symbols and high-score retention:
1. **Breakout**: Classic paddle-and-brick arcade game with ball acceleration and paddle deflection physics.
2. **Marble Maze**: Precision 2D IMU tilt labyrinth with rolling ball momentum, hole hazards, and goal flag.
3. **Archery Target**: Timed precision bow-and-arrow targeting with oscillating reticle and bullseye scoring.
4. **Treat Catcher**: Moving basket arcade game catching falling treats with progressive difficulty and lives system.
5. **Asteroids**: 360° circular vector space arena with ship heading steered via QMI8658 IMU gyroscope orientation, photon cannon firing via screen tap or side button, and procedural drifting asteroid fields.
