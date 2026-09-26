# 🧭 Pocket Assistant

> **A pocket-sized smart companion for Home Assistant with native Voice Assistant and a first-of-its-kind custom Music Assistant library browser & controller.**

[![ESPHome Version](https://img.shields.io/badge/ESPHome-2026.9.0%2B-blue.svg)](https://esphome.io)
[![Home Assistant](https://img.shields.io/badge/Home%20Assistant-Compatible-41BDF5.svg)](https://www.home-assistant.io)
[![License](https://img.shields.io/badge/License-Apache%202.0-green.svg)](LICENSE)

Most ESPHome media displays are passive screens that only show what is already playing. **Pocket Assistant** transforms a handheld microcontroller into an interactive, local-first smart terminal bridging Home Assistant, Music Assistant, and Voice Assistant.

---

## ✨ Key Features

* 🎵 **Hierarchical Music Library Browsing**: Browse Artists, Albums, Tracks, Playlists, and Radio directly on-device with dual-action play (`▶`) and drill-down (`>`) touch cards.
* 🔊 **Multi-Room Handoff & Speaker Takeover**: Transfer active queues between household speakers (e.g. Garage HiFi, Kitchen Speaker) or take over remote playback on the fly.
* 🎙️ **Native Voice Assistant**: Bidirectional Assist satellite streaming over the encrypted Home Assistant Native API with custom kinetic AMOLED visual feedback.
* 🎮 **Interactive Motion Games**: Real-time 20 FPS physics games (Marble Maze, Archery Target, Treat Catcher) powered by the onboard 6-axis IMU.
* ⏱️ **Precision Lap Stopwatch**: Dedicated millisecond-resolution timer with tactile on-screen controls.
* 🔋 **Intelligent Power Management**: Multi-tier standby, accidental pocket-bump rejection on wakeup, and AMOLED framebuffer blackouts for maximum battery preservation.
* 🧩 **Modular Architecture**: Built on ESPHome's native `packages:` engine. Enable, disable, or reorder apps at runtime without touching core firmware.

---

## 🛠️ Supported Hardware

| Hardware | Display | Audio | IMU | PMU | Status |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Waveshare ESP32-S3-Touch-AMOLED-1.75C** | 1.75" Circular AMOLED (466×466, CO5300) | Dual I2S Master (ES8311 DAC + ES7210 Mic) | QMI8658 | AXP2101 | **Primary (Verified)** |
| **Waveshare ESP32-S3-Touch-LCD-1.85C** | 1.85" Circular LCD (360×360) | Onboard I2S DAC/Mic | QMI8658 | AXP2101 | *Target Roadmap* |

---

## ⚡ Technical Challenges Solved

Deploying a multi-function smart wearable with full-duplex voice assistance, high-speed graphics, and interactive streaming media on a single ESP32-S3 microcontroller required solving several architectural roadblocks that have traditionally limited ESP32-based devices.

### 1. The Shared I2S Clock Contention & Dynamic GPIO Matrix Fix
* **The Challenge**: The Waveshare 1.8" AMOLED architecture routes both the ES7210 microphone ADC (input) and ES8311 speaker DAC (output) through shared clock lines: **GPIO9 (BCLK)**, **GPIO45 (WS/LRCLK)**, and **GPIO16 (MCLK)**. In standard ESPHome configurations, attempting to run full-duplex I2S audio with shared clocks causes severe clock jitter, buffer underruns, microphone corruption, or complete DAC lockup.
* **The Breakthrough**: Rather than accepting half-duplex degradation or hardware compromises, Pocket Assistant utilizes **dynamic runtime GPIO Matrix multiplexing** via Espressif ROM routing (`esp_rom_gpio_connect_out_signal`). 
  * When Voice Assistant begins listening, the hardware clock lines are instantly routed to I2S0 peripheral signals (`signal 26`, `signal 27`, `signal 23`).
  * When media playback, TTS, or tactile feedback begins, the clock lines dynamically switch to I2S1 peripheral signals (`signal 28`, `signal 29`, `signal 21`).
  * This eliminates physical clock collision and allows the single ESP32-S3 to drive high-fidelity microphone input and speaker output without dedicated external multiplexer ICs.

### 2. First-of-its-Kind Music Assistant Wearable Integration
* **The Challenge**: Most smart home displays are passive dashboards that simply reflect what an external media player is already playing. Native library browsing on microcontrollers has historically been avoided due to memory constraints, slow JSON parsing, and complex state management.
* **The Solution**: Pocket Assistant features a custom-engineered client interface for **Music Assistant** and **Sendspin**:
  * **Interactive Hierarchical Browser**: Directly drill down from Artists $\rightarrow$ Albums $\rightarrow$ Tracks, or browse Playlists and Radios with dual-action play (`▶`) and browse (`>`) cards.
  * **Multi-Room Handoff & Speaker Takeover**: Move playback queues dynamically between household speakers (e.g., from the watch to a living room amplifier or kitchen speaker) or remotely control audio on other players directly from your wrist.
  * **Optimized Payload Windows**: Communicates with Home Assistant via lightweight, bounded RPC calls (`set_browse_slots`) that bypass heavy client-side JSON parsing and keep PSRAM usage minimal.

---

## 🔬 Hardware Optimizations

Pocket Assistant was engineered to extract every ounce of performance and battery efficiency from the ESP32-S3 hardware.

### 1. Asymmetric Graphics Engine & DMA Bus Balancing
* **Quad-SPI MIPI Engine**: The circular $466\times 466$ AMOLED display (CO5300 controller) operates over high-speed Quad SPI (GPIO4–GPIO7 with dedicated clock and chip select).
* **Asymmetric Frame Cadence**: To prevent high-speed display DMA transactions from starving the I2S audio FIFO, the graphics pipeline dynamically throttles its refresh rate based on audio state:
  * **Listening & Thinking**: Renders at $150\text{ ms}$ (~6.7 FPS) for an organic breathing kinetic aura while the audio output bus is quiet.
  * **TTS Speech Playback**: Throttled to a calibrated cadence ($350\text{--}800\text{ ms}$) during speech synthesis to guarantee $100\%$ glitch-free audio playback.
  * **Precision Chronometer**: Runs at an optimized $80\text{ ms}$ ($12.5\text{ FPS}$) cadence, delivering fluid mechanical hand sweeps while reducing CPU rendering overhead by $40\%$.

### 2. Hardware RTC Crystal Calibration on Boot
* ESP32 internal RC oscillators naturally suffer from frequency drift caused by temperature variations and sleep states.
* On boot, Pocket Assistant directly invokes Espressif's hardware assembly calibration routine (`rtc_clk_cal`) to measure and calibrate the internal RTC slow clock against the high-precision 40MHz main crystal over 1024 cycles.
* This establishes sub-millisecond hardware timekeeping that remains accurate across deep sleep cycles without constant NTP network synchronization.

### 3. Multi-Tier Intelligent Power Management
* **Tier 1 (Interactive AMOLED)**: Dynamic brightness controls ($40\%\text{--}100\%$) with calibrated gamma curves.
* **Tier 2 (Screen Standby)**: After inactivity, the AMOLED panel enters zero-power standby while FreeRTOS tasks remain active. The display wakes in under $50\text{ ms}$ upon detecting physical pickup or motion via the onboard QMI8658 6-axis IMU.
* **Tier 3 (Deep Sleep Hibernation)**: Long-pressing the top crown button ($> 1.0\text{ s}$) places the ESP32-S3 into deep sleep, reducing power draw to microamps.
* **Accidental Pocket-Bump Rejection**: When waking from deep sleep via the hardware crown button, the boot routine samples GPIO0 over a $300\text{ ms}$ window. If the button was not held for at least $200\text{ ms}$, the event is identified as an accidental pocket bump and the chip returns to deep sleep immediately without powering on the display.
* **USB Dock Safety Override**: When docked on USB power, battery fuel gauge registers ($0\times 34$) are read via I2C. Deep sleep is automatically inhibited so the watch remains an active, glanceable desk companion while charging.

---

## 🚀 Quick Start & Installation

### 1. Requirements
* Home Assistant with the **ESPHome** and **Music Assistant** add-ons/integrations installed.
* Supported ESP32-S3 hardware (Waveshare ESP32-S3-Touch-AMOLED-1.75C).

### 2. Home Assistant Companion Script
Import the companion script into Home Assistant (`Settings` → `Automations & Scenes` → `Scripts`):
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

## 🧭 Navigation & Gestures

* **Switch Apps**: Tap or swipe the **left edge** ($x < 14\%$) or **right edge** ($x > 86\%$) of the display to flip through your active app deck (Clock $\leftrightarrow$ Stopwatch $\leftrightarrow$ Music $\leftrightarrow$ Games $\leftrightarrow$ System).
* **Music Menus**: When inside music submenus or library browsers, edge touches turn list pages forward and back, completely guarding against accidental exits to other apps.
* **Top Crown Button**:
  * On Clock Face: Quick screen standby.
  * In Music: Toggles between Now Playing and the Library Selection Menu.
  * In Games: Exits active game to menu.
  * In System: Cycles calibrated brightness presets ($70\% 
ightarrow 80\% 
ightarrow 90\% 
ightarrow 100\%$).
* **Deep Sleep / Hibernation**: Press and hold the top crown button for $> 1.0	ext{ s}$ to enter deep sleep.

---

## 📂 Repository Layout

```text
esphome-pocket-assistant/
├── README.md                      # Documentation
├── LICENSE                        # Apache 2.0 License
├── .gitignore                     # Git ignore rules
├── pocket-assistant.yaml          # Master entry point (Substitutions & package includes)
├── boards/                        # Hardware Abstraction Layer
│   └── waveshare_175c.yaml        # Pinouts, QSPI, I2C, I2S multiplexer, AXP2101, QMI8658
├── core/                          # Core System Engines
│   ├── audio.yaml                 # Dual I2S master multiplexing, DAC/Mic, Voice Assistant
│   ├── power.yaml                 # Power management, sleep timers, pocket bump rejection
│   └── ui.yaml                    # Display rendering pipeline, typography, color palettes, router
├── apps/                          # Modular Application Components
│   ├── clock.yaml                 # Modern dial & digital clock
│   ├── stopwatch.yaml             # Precision lap stopwatch
│   ├── music.yaml                 # Music Assistant client, browser, and multi-room handoff
│   ├── games.yaml                 # 3 motion physics games
│   └── system.yaml                # System dashboard, brightness presets, diagnostic restart
└── homeassistant/
    └── script.pocket_assistant_browse.yaml # HA companion script
```

---

## 📜 License
Distributed under the Apache 2.0 License. See `LICENSE` for details.
