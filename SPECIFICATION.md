# Pocket Assistant — Technical Specification (v1.3)
**Hardware Platform**: Waveshare ESP32-S3-Touch-AMOLED-1.75C  
**Framework**: ESPHome 2026.9.0+ / ESP-IDF 5.5+  
**Target Resolution**: 466 × 466 px Circular Display  

---

## 1. Hardware Architecture & Pinout Map

### 1.1 Microcontroller & Memory
* **MCU**: Espressif ESP32-S3 (Dual-core Xtensa LX7 @ 240 MHz).
* **Flash**: 32MB Octal SPI.
* **PSRAM**: 8MB Octal SPI @ 80 MHz (`mode: octal`, `speed: 80MHz`).

### 1.2 Display Subsystem
* **Controller**: CO5300 QSPI Controller.
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
* **Bus**: I2C `bus_internal` (`SDA: GPIO15`, `SCL: GPIO14`, 400 kHz).
* **Interrupt**: `GPIO11` (Active Low).
* **Reset**: `GPIO2`.
* **Polling Cadence**: 33ms (30 Hz) with hierarchical debouncing.

### 1.4 Power Management Unit (PMU)
* **PMIC**: X-Powers AXP2101 (@ I2C address `0x34`).
* **Hardware Interrupt**: `GPIO3` (Connected to AXP2101 IRQ pin).
* **Power Rails**:
  * ALDO1–4, BLDO1–2, DLDO1–2 powering AMOLED logic, panel power, I2S codecs, and sensors.
  * Deep sleep / Hibernate: Cuts peripheral LDO rails (`0x10 = 0x01`), retaining only RTC and wake circuitry.

### 1.5 Physical Buttons
* **Top Rotary / Push Crown**:
  * Connected to AXP2101 PWRKEY / PEK pin.
  * Interrupted via `GPIO3`.
  * Short click: Wake, start/stop timers, dismiss/snooze alarms, exit submenus, sleep.
  * Long press (>1.5s): Triggers 5-second abortable countdown to full hardware hibernation.
* **Side Push Button**:
  * Connected to `GPIO0` (internal pull-up).
  * Exclusively dedicated to Voice Assistant Push-To-Talk (PTT) and in-game controls.
  * Fully decoupled from deep-sleep/hibernation wake to prevent accidental pocket activation.

### 1.6 Motion Sensing
* **Sensor**: QST QMI8658 6-Axis IMU (Accelerometer + Gyroscope) @ I2C address `0x6B`.
* **Ranges**: Accelerometer 4G, Gyroscope 2048 DPS.
* **Capabilities**: Screen tilt detection, in-hand pickup detection, physics simulations, digital bubble level.

### 1.7 Audio Pipeline & Multiplexed I2S
* **Audio DAC**: Everest Semiconductor ES8311 @ I2C address `0x18`.
* **Audio ADC**: Everest Semiconductor ES7210 @ I2C address `0x40`.
* **Shared Physical Clock Lines**: `BCLK: GPIO9`, `WS: GPIO45`, `MCLK: GPIO16`.
* **Dynamic Multiplexing**:
  * `route_i2s_to_mic`: Connects physical clocks to I2S0 for audio recording.
  * `route_i2s_to_spk`: Connects physical clocks to I2S1 for audio playback and voice synthesis.

---

## 2. Power States & Performance Budgets

| Power State | Current Draw | Wake Mechanism | Latency |
| :--- | :--- | :--- | :--- |
| **Tier 1 (Active)** | 110 – 160 mA | Active touch / Crown click | Instant |
| **Tier 2 (Display Standby)** | 15 – 25 mA | Top Crown (`GPIO3`), In-hand pickup (`QMI8658`) | < 5 ms |
| **Tier 3 (Hardware Hibernate)** | < 40 µA | Top Crown hardware interrupt only | Cold boot (~800ms) |

### Frame Rendering & Timing Budgets
* **Chronograph & Standard Pages**: Target 10 FPS (100ms cycle).
* **Render Execution**: < 45ms per frame.
* **CPU Idle Window**: $\ge$ 55ms guaranteed idle time per frame cycle to ensure 0-latency touch response.

---

## 3. Home Assistant Companion Architecture (2-Sensor Contract)

### 3.1 Weather Sensor
* **Entity**: `sensor.pocket_assistant_weather_forecast`
* **State**: 5-day formatted string (`DAY:HIGH:LOW:COND;...`).
* **Attributes**: `high` (float), `low` (float).
* **Fallback**: On-device C `sscanf` parser for `DAY:HIGH:LOW:COND` format if attributes are unavailable.

### 3.2 Active Media Sensor
* **Entity**: `sensor.pocket_assistant_active_media`
* **State**: Playback state (`playing`, `paused`, `idle`).
* **Attributes**: `title`, `artist`, `album`, `duration`, `position`, `volume`, `media_type`, `art_url`.
* **Database Optimization**: Reduces Home Assistant database writes during active playback by 89%.

### 3.3 State Helpers (`input_text`)
* `input_text.pocket_assistant_active_speaker`: Active target media player entity ID.
* `input_text.pocket_assistant_weather_entity`: Monitored weather entity ID.
* `input_text.pocket_assistant_base_url`: Optional custom base URL override.

---

## 4. Canonical Application Suite (Pages 0–7)

| Page | Application Name | Primary Function & Controls |
| :--- | :--- | :--- |
| **0** | **Clock** | Analog and digital watch faces, time format cycler, date and battery complication displays. |
| **1** | **Stopwatch** | Horological 10 FPS split-lap chronograph with Rattrapante ghost hand and 55ms CPU idle window. |
| **2** | **Music Player** | Full-bleed AMOLED album art, YouTube-style auto-fading HUD, volume steppers, and 5-slot library browser. |
| **3** | **Games Suite** | Vertical carousel launcher with Breakout, Marble Maze, Archery Target, and Treat Catcher. |
| **4** | **System Hub** | 2x4 balanced hub with submenus for Display, Clock, Sleep & Wake, Audio, Voice, App Deck, and Diagnostics. |
| **5** | **Sky & Weather** | Astronomical & weather suite with current conditions, 5-day aligned forecast, Sun arc, and Moon phases. |
| **6** | **Timer & Alarm** | Local RTC countdown timer with perimeter arc, quick presets, and dual configurable alarms with crown snooze. |
| **7** | **TV Remote** | Universal media remote (`remote.send_command`) with 5-way D-pad, Home/Vol/Back flank controls, and power off modal. |
