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
