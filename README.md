# 🧭 Pocket Assistant

> **A pocket-sized smart companion for Home Assistant with native Voice Assistant and a first-of-its-kind custom Music Assistant library browser & controller.**

[![Version](https://img.shields.io/badge/Version-v3.4.2-orange.svg)](https://github.com/slampton/esphome-pocket-assistant/releases)
[![ESPHome Version](https://img.shields.io/badge/ESPHome-2026.9.0%2B-blue.svg)](https://esphome.io)
[![Home Assistant](https://img.shields.io/badge/Home%20Assistant-Compatible-41BDF5.svg)](https://www.home-assistant.io)
[![License](https://img.shields.io/badge/License-Apache%202.0-green.svg)](LICENSE)

Most ESPHome media displays are passive screens that only show what is already playing. **Pocket Assistant** transforms a handheld microcontroller into an interactive, local-first smart terminal bridging Home Assistant, Music Assistant, and Voice Assistant.

---

## ✨ Key Features

* 🎵 **Hierarchical Music Library Browsing**: Browse Artists, Albums, Tracks, Playlists, and Radio directly on-device with dual-action play (`▶`) and drill-down (`>`) touch cards.
* 🔊 **Multi-Room Handoff & Speaker Takeover**: Transfer active queues between household speakers (e.g. Garage HiFi, Kitchen Speaker) or take over remote playback on the fly.
* 🎙️ **Native Voice Assistant with FreeRTOS Mixer Ducking**: Direct Assist satellite pipeline with dynamic 20 dB music ducking, kinetic AMOLED visual feedback, and push-to-talk/instant side button cancellation.
* 🎚️ **Single-Authority Audio & Configurable Volume Step**: Unified physical DAC control with priority-synchronized boot gain, plus a customizable **Volume Step Size** entity (1%–10%, default 2%) with absolute target setting to eliminate all slider rubber-banding.
* ⏱️ **Vintage Chronograph & Lap Stopwatch**: Precision chronometer featuring a classic vintage hour/chronograph face design with dual sub-dials, sweep center seconds, and tactile crown button controls with release-dwell latency compensation.
* 🎮 **Interactive Motion Games**: Real-time 20 FPS physics games (Marble Maze, Archery Target, Treat Catcher) powered by the onboard 6-axis IMU.
* 🔋 **Intelligent Multi-Tier Power Management**: Instant AMOLED screen standby (<50ms IMU pickup wake), physical pocket lock, deliberate hardware deep sleep hibernation, accidental pocket-bump rejection, and USB dock wake override.
* 🧩 **Modular Architecture**: Built on ESPHome's native `packages:` engine. Enable, disable, or reorder apps at runtime without touching core firmware.

---

## 🛠️ Hardware Platform

| Hardware | Display | Audio | IMU | PMU | Status |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Waveshare ESP32-S3-Touch-AMOLED-1.75C** | 1.75" Circular AMOLED (466×466, CO5300) | Dual I2S Master (ES8311 DAC + ES7210 Mic) | QMI8658 | AXP2101 | **Verified** |

---

## ⚡ Technical Challenges Solved

Deploying a multi-function, pocket-sized smart device with full-duplex voice assistance, high-speed graphics, and interactive streaming media on a single ESP32-S3 microcontroller required solving several architectural roadblocks that have traditionally limited ESP32-based devices.

### 1. The Shared I2S Clock Contention & Dynamic GPIO Matrix Fix
* **The Challenge**: The Waveshare 1.8" AMOLED architecture routes both the ES7210 microphone ADC (input) and ES8311 speaker DAC (output) through shared clock lines: **GPIO9 (BCLK)**, **GPIO45 (WS/LRCLK)**, and **GPIO16 (MCLK)**. In standard ESPHome configurations, attempting to run full-duplex I2S audio with shared clocks causes severe clock jitter, buffer underruns, microphone corruption, or complete DAC lockup.
* **The Breakthrough**: Rather than accepting half-duplex degradation or hardware compromises, Pocket Assistant utilizes **dynamic runtime GPIO Matrix multiplexing** via Espressif ROM routing (`esp_rom_gpio_connect_out_signal`). 
  * When Voice Assistant begins listening, the hardware clock lines are instantly routed to I2S0 peripheral signals (`signal 26`, `signal 27`, `signal 23`).
  * When media playback, TTS, or tactile feedback begins, the clock lines dynamically switch to I2S1 peripheral signals (`signal 28`, `signal 29`, `signal 21`).
  * This eliminates physical clock collision and allows the single ESP32-S3 to drive high-fidelity microphone input and speaker output without dedicated external multiplexer ICs.

### 2. First-of-its-Kind Music Assistant ESPHome Integration
* **The Challenge**: Most smart home displays in ESPHome are passive dashboards that simply reflect what an external media player is already playing. Full native library browsing on an ESPHome device has historically never been done due to microcontroller memory constraints, slow JSON parsing, and complex state management.
* **The Solution**: Pocket Assistant features a custom-engineered client interface for **Music Assistant** and **Sendspin**:
  * **Interactive Hierarchical Browser**: Directly drill down from Artists -> Albums -> Tracks, or browse Playlists and Radios with dual-action play (`▶`) and browse (`>`) cards.
  * **Multi-Room Handoff & Speaker Takeover**: Move playback queues dynamically between household speakers (e.g., from the watch to a living room amplifier or kitchen speaker) or remotely control audio on other players directly on-device from the palm of your hand.
  * **Optimized Payload Windows**: Communicates with Home Assistant via lightweight, bounded RPC calls (`set_browse_slots`) that bypass heavy client-side JSON parsing and keep PSRAM usage minimal.

### 3. Single-Authority Audio Architecture, Configurable Volume Step & FreeRTOS Mixer Ducking
* **The Challenge**: Earlier iterations ran two separate media players—one for Music and one for Voice Assistant. Because both players ultimately commanded the same physical ES8311 DAC, adjusting the volume on one media player inadvertently overwrote the hardware gain register of the other, leading to cross-wired volume sliders. Furthermore, on boot the ES8311 chip initializes at raw 0 dB (100% volume), causing sudden loud blasts until a slider was nudged, and fixed 5% hardware steps clashed with Home Assistant's 2% steps to cause visual rubber-banding.
* **The Architectural Redesign**:
  * **Single Media Player Authority**: `pocket_music_player` serves as the sole master media player entity in Home Assistant, directly governing the physical listening volume of the watch.
  * **Configurable Volume Step Size Entity**: Exposes a persistent `number.pocket_assistant_volume_step_size` configuration slider in Home Assistant (range: 1% to 10%, step: 1%, default: 2%). When tapping on-screen volume carets, the firmware calculates the exact target (`current ± step`) and commands Home Assistant via explicit `media_player.volume_set`, ensuring 100% lockstep synchronization with zero rubber-banding.
  * **Direct-to-Mixer Voice Pipeline**: Voice Assistant streams audio natively through `speaker: va_speaker` into the FreeRTOS software `mixing_speaker`, bypassing duplicate media player layers.
  * **Dynamic 20 dB Software Ducking**: When Voice Assistant listens or speaks, the mixer dynamically applies software ducking (`mixer_speaker.apply_ducking: -20 dB`) to active music playback, creating an unobtrusive background bed and smoothly recovering music over 1.0 second once speech finishes.
  * **Boot Volume Synchronization**: Codec initialization is anchored at `priority: -100` (strictly after hardware driver setup at priority 500). This guarantees that register `0x32` on the DAC is set to a comfortable 70% level before any audio stream can start, eliminating the power-on 100% volume spike.
  * **Push-to-Talk & Instant Dismissal**: A lightweight C++ helper (`va_cancel_helper.h`) binds to the side button (GPIO0). Pressing the button starts Assist; pressing it again while Assist is listening, thinking, or speaking aborts the pipeline, mutes audio, and restores the previous screen immediately.

---

## 🔬 Hardware Optimizations

Pocket Assistant was engineered to extract every ounce of performance and battery efficiency from the ESP32-S3 hardware.

### 1. Asymmetric Graphics Engine & DMA Bus Balancing
* **Quad-SPI MIPI Engine**: The circular 466x466 AMOLED display (CO5300 controller) operates over high-speed Quad SPI (GPIO4–GPIO7 with dedicated clock and chip select).
* **Asymmetric Frame Cadence**: To prevent high-speed display DMA transactions from starving the I2S audio FIFO, the graphics pipeline dynamically throttles its refresh rate based on audio state:
  * **Listening & Thinking**: Renders at 150 ms (~6.7 FPS) for an organic breathing kinetic aura while the audio output bus is quiet.
  * **TTS Speech Playback**: Throttled to a calibrated cadence (350–800 ms) during speech synthesis to guarantee 100% glitch-free audio playback.
  * **Precision Chronometer**: Runs at an optimized 80 ms (12.5 FPS) cadence, delivering fluid mechanical hand sweeps while reducing CPU rendering overhead by 40%.

### 2. Hardware RTC Crystal Calibration on Boot
* ESP32 internal RC oscillators naturally suffer from frequency drift caused by temperature variations and sleep states.
* On boot, Pocket Assistant directly invokes Espressif's hardware assembly calibration routine (`rtc_clk_cal`) to measure and calibrate the internal RTC slow clock against the high-precision 40MHz main crystal over 1024 cycles.
* This establishes sub-millisecond hardware timekeeping that remains accurate across deep sleep cycles without constant NTP network synchronization.

### 3. Multi-Tier Intelligent Power Management
* **Tier 1 (Interactive AMOLED)**: Calibrated brightness curve presets (40%–100%) switchable via the system dashboard or top crown button.
* **Tier 2 (Display Standby / Screen Sleep)**: After 30 seconds of inactivity on the clock face (or by tapping the top crown button), the AMOLED display blanks to true zero-power black (0 mA) while FreeRTOS tasks and network connections stay alive. The display wakes in under 50 ms upon detecting physical pickup or wrist motion via the QMI8658 6-axis IMU.
* **Tier 3 (Deep Sleep Hibernation)**: Holding the top crown button for > 1.5 s or the side boot button for > 1.0 s transitions the ESP32-S3 into true deep sleep hibernation, reducing total draw to microamps for days of shelf standby.
* **Accidental Pocket-Bump Rejection**: When waking from deep sleep, the boot routine qualifies button hold duration over a 300 ms window. If the crown button was not deliberately held for at least 200 ms, the event is flagged as an accidental pocket bump and the microcontroller immediately re-enters deep sleep without waking the display or initializing radio peripherals.
* **USB Dock Safety Override**: When connected to USB power, AXP2101 PMU fuel gauge registers (0x00 VBUS status) are continuously monitored. Deep sleep auto-abort is inhibited, allowing Pocket Assistant to remain continuously active as a glanceable desktop clock while docked.
* **Pocket Lock Mode**: Long-pressing the crown button for 1.5 s while active toggles a capacitive touch lockout to prevent accidental UI activations inside a pocket or bag.

---

## 📱 Application Suite

Pocket Assistant organizes on-device functionality into focused, modular applications switchable via edge swipes:

### 🕒 Modern Dial Clock (`apps/clock.yaml`)
* High-contrast 60-tick dial with Roman numeral indices and polygon-rendered analog hands.
* Integrated digital time readout, live date, battery percentage, charging lightning bolt, and Home Assistant connection indicator.

### ⏱️ Vintage Chronograph Stopwatch (`apps/stopwatch.yaml`)
* **Vintage Hour / Chronograph Face**: Modeled after classic mechanical pocket watch chronometers with high-contrast perimeter markers and bold 5-second interval numerals.
* **Dual Mechanical Sub-Dials**:
  * **12 O'Clock Sub-Dial**: High-speed 1/10th-second spinner with rotating mechanical hand.
  * **6 O'Clock Sub-Dial**: 60-minute elapsed accumulator with calibrated index markings.
* **Center Sweep Chronometer Hand**: Smooth 12.5 FPS sweep second hand with vintage teardrop counterbalance geometry.
* **Physical Crown Button Integration**: Start and stop timing via the physical top crown button with automatic 200 ms release-dwell latency compensation for true physical-stopwatch accuracy. On-screen touch controls provide dedicated **LAP** tracking and **RESET** functions.

### 🎵 Music Assistant Client & Multi-Room Remote (`apps/music.yaml`)
* Direct Music Assistant library browser: Artists, Albums, Tracks, Playlists, and Radio stations.
* Dual-action touch cards (`▶` to play immediately, `>` to drill down into sub-items).
* Active queue handoff and speaker takeover across household smart speakers.
* Full playback controls: play/pause, track skipping, dynamic volume slider, and track progress bar.

### 🎮 Motion Physics Games (`apps/games.yaml`)
* Real-time 20 FPS physics simulation powered by the onboard QMI8658 6-axis accelerometer:
  * **Marble Maze**: Tilt-guided labyrinth maze navigation.
  * **Archery Target**: Steady-hand gyroscope aiming.
  * **Treat Catcher**: Motion-controlled paddle game.

### ⚙️ System Dashboard & Diagnostics (`apps/system.yaml`)
* Telemetry readout: Battery percentage, millivolt voltage, charging state, Wi-Fi SSID, IP address, and RSSI signal strength.
* Brightness preset cycler (40% -> 100%) and safe on-device software restart button.

---

## 🚀 Quick Start & Installation

### 1. Requirements
* Home Assistant with the **ESPHome** and **Music Assistant** add-ons/integrations installed.
* Supported ESP32-S3 hardware (Waveshare ESP32-S3-Touch-AMOLED-1.75C).

### 2. Home Assistant Companion Script
Import the companion script into Home Assistant (`Settings` -> `Automations & Scenes` -> `Scripts`):
* File: [`homeassistant/script.pocket_assistant_browse.yaml`](homeassistant/script.pocket_assistant_browse.yaml)

### 3. Deploy Firmware (One-Click Remote Git Package)
In your Home Assistant **ESPHome Device Builder** dashboard, create a new device or edit your configuration with this minimal stub:

```yaml
substitutions:
  name: "pocket-assistant"
  friendly_name: "Pocket Assistant"

  # Target Speakers for Handoff & Remote Control (customize for your home)
  speaker_1_name: "Pocket Assistant"
  speaker_1_id: "media_player.pocket_assistant"
  speaker_2_name: "Garage HiFi"
  speaker_2_id: "media_player.garage_hifi"
  speaker_3_name: "Kitchen Speaker"
  speaker_3_id: "media_player.kitchen_speaker"

  # Quick Presets (Favorite playlists, radio stations, or albums)
  preset_1_name: "Radio Paradise"
  preset_1_type: "radio"
  preset_1_id: "Radio Paradise"
  preset_2_name: "Daily Favorites"
  preset_2_type: "playlist"
  preset_2_id: "Daily Favorites"
  preset_3_name: "Shoegaze Mix"
  preset_3_type: "playlist"
  preset_3_id: "Shoegaze"

wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password

api:
  encryption:
    key: !secret pocket_assistant_encryption_key

ota:
  - platform: esphome
    encryption:

packages:
  remote_pocket_assistant:
    url: https://github.com/slampton/esphome-pocket-assistant
    ref: main
    refresh: 0s
    files:
      - pocket-assistant.yaml
```

Ensure your `/config/secrets.yaml` contains `wifi_ssid`, `wifi_password`, and `pocket_assistant_encryption_key`.

---

## 🧭 Navigation & Controls

* **Switch Apps**: Tap or swipe the **left edge** ($x < 14%$) or **right edge** ($x > 86%$) of the display to flip through your active app deck (Clock <-> Stopwatch <-> Music <-> Games <-> System).
* **Music Menus**: When inside music submenus or library browsers, edge touches turn list pages forward and back, completely guarding against accidental exits to other apps.
* **Top Crown Button (AXP2101 PEK)**:
  * **On Clock Face**: Quick screen standby (Tier 2).
  * **In Stopwatch**: Starts and stops the chronometer timer with release-dwell compensation.
  * **In Music**: Toggles between Now Playing and the Library Selection Menu.
  * **In Games**: Exits active game to menu.
  * **In System**: Cycles calibrated brightness presets (40% -> 50% -> 60% -> 70% -> 80% -> 90% -> 100%).
  * **Hold (> 1.5 s)**: Toggles capacitive Pocket Lock or triggers deep sleep hibernation.
* **Side Button (GPIO0 / Boot Button)**:
  * **To Activate Assist**: Press the **Side Button**. Local music automatically ducks or pauses, the display turns on, and Assist begins listening with dynamic kinetic visual feedback.
  * **To Dismiss / Cancel Assist**: Press the **Side Button** again at any point during listening, thinking, or TTS speech to abort the pipeline, mute audio, and instantly restore the previous screen.
  * **Hold (> 1.0 s)**: Initiates deep sleep hibernation.
* **Deep Sleep Wakeup**:
  * Press and hold the **top crown button** for approximately **0.5 s (at least 200 ms)**, then **release it**. The release confirms a deliberate wake action (filtering out accidental transient pocket bumps), powers on the display, and returns immediately to your clock face.

---

## 📂 Repository Layout

```text
esphome-pocket-assistant/
├── README.md                      # Comprehensive Documentation
├── LICENSE                        # Apache 2.0 License
├── .gitignore                     # Git ignore rules
├── pocket-assistant.yaml          # Master entry point (Substitutions & package includes)
├── va_cancel_helper.h             # C++ Voice Assistant instant cancel helper
├── boards/                        # Hardware Abstraction Layer
│   └── waveshare_175c.yaml        # Pinouts, QSPI, I2C, I2S multiplexer, AXP2101, QMI8658
├── core/                          # Core System Engines
│   ├── audio.yaml                 # Dual I2S master multiplexing, DAC/Mic, Voice Assistant
│   ├── power.yaml                 # Power management, sleep timers, pocket bump rejection
│   └── ui.yaml                    # Display rendering pipeline, typography, color palettes, router
├── apps/                          # Modular Application Components
│   ├── clock.yaml                 # Modern dial & digital clock
│   ├── stopwatch.yaml             # Precision vintage chronograph & lap stopwatch
│   ├── music.yaml                 # Music Assistant client, browser, and multi-room handoff
│   ├── games.yaml                 # 3 motion physics games
│   └── system.yaml                # System dashboard, brightness presets, diagnostic restart
└── homeassistant/
    └── script.pocket_assistant_browse.yaml # HA companion script
```

---

## 📜 Version History & Changelog

### v3.4.2 (Current)
* **Configurable Volume Step Size Entity**: Added `number.pocket_assistant_volume_step_size` configuration slider (1%–10%, default 2%, NVS restored).
* **Absolute Target Volume Synchronization**: Replaced generic `volume_up`/`volume_down` RPC calls for local playback with explicit `media_player.volume_set` targeting, completely eliminating visual rubber-banding and step-size conflicts between ESPHome and Home Assistant.
* **Floating-Point Rounding Fix**: Updated HUD volume percentage display from truncation `(int)` to mathematical `(int)std::round()` to prevent off-by-one display artifacts.

### v3.4.1
* **Single Media Player Authority**: Eliminated duplicate `pocket_assist_player` to prevent hardware DAC gain collisions and cross-wired volume slider bugs.
* **FreeRTOS Mixer Ducking**: Integrated Assist voice satellite directly to `speaker: va_speaker` with automatic 20 dB dynamic music ducking and smooth 1-second recovery.
* **Boot Volume Synchronization**: Anchored hardware DAC setup at `priority: -100` to eliminate the power-on 0 dB (100% blasting) volume discrepancy.
* **Push-to-Talk & Instant Assist Cancellation**: Added `va_cancel_helper.h` side-button abort to dismiss Assist during speech/thinking.
* **Vintage Chronograph Face Design**: Stopwatch upgraded to a vintage pocket watch chronometer dial with perimeter markers, 1/10s spinner, 60-minute accumulator, and release-dwell compensation.
* **Power Management Refinements**: Fully documented Tier 2 Display Standby vs Tier 3 Deep Sleep Hibernation, IMU pickup wake, deliberate pocket-bump qualification, and USB dock safety override.

### v3.3.0
* Initial release of modular packages architecture (`core/`, `apps/`, `boards/`).
* Music Assistant hierarchical browser with dual-action play/drill cards.
* Multi-room speaker handoff and takeover queue switching.
* Dynamic I2S clock line GPIO matrix multiplexing.

---

## 📜 License
Distributed under the Apache 2.0 License. See `LICENSE` for details.
