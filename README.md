# Pocket Assistant

A local-first handheld companion and media remote for Home Assistant and Music Assistant, built on the Waveshare 1.75" circular AMOLED ESP32-S3 development board.

[![Version](https://img.shields.io/badge/Version-v1.1-blue.svg)](https://github.com/slampton/esphome-pocket-assistant/releases)
[![ESPHome Version](https://img.shields.io/badge/ESPHome-2026.9.0%2B-blue.svg)](https://esphome.io)
[![Home Assistant](https://img.shields.io/badge/Home%20Assistant-Compatible-41BDF5.svg)](https://www.home-assistant.io)
[![Music Assistant](https://img.shields.io/badge/Music%20Assistant-2.0%2B-purple.svg)](https://music-assistant.io)
[![License](https://img.shields.io/badge/License-Apache%202.0-green.svg)](LICENSE)

Pocket Assistant started as a personal home lab project to explore what is possible when pairing modern ESP32-S3 hardware with Home Assistant, Music Assistant, and ESPHome. Rather than acting as a passive sensor display or an audio satellite alone, it combines local playback, multi-room speaker handoff, interactive library browsing, Voice Assistant support, and power-efficient sleep states in a circular handheld form factor.

---

## Quick Start & Installation

### Supported Hardware
* **Board**: Waveshare ESP32-S3-Touch-AMOLED-1.75C (1.75" 466×466 round AMOLED, CO5300 display, CST9220 touch, ES8311 DAC, ES7210 ADC, AXP2101 PMIC, QMI8658 6-axis IMU).

### 1. Flash the Firmware
* **Web Installer (Recommended)**: Connect the device via USB-C in a WebSerial-supported browser (Chrome, Edge, Opera) and visit the **[Pocket Assistant Web Installer](https://slampton.github.io/esphome-pocket-assistant/)** to install v1.1 with one click.
* **ESPHome Dashboard**: Alternatively, adopt the device using a minimal remote package include:
```yaml
substitutions:
  name: "pocket-assistant"
  friendly_name: "Pocket Assistant"
  version: "1.1"
  local_player_id: "media_player.pocket_assistant"

packages:
  remote_pocket_assistant:
    url: https://github.com/slampton/esphome-pocket-assistant
    ref: main
    files:
      - pocket-assistant-1.75c.yaml
```

### 2. Home Assistant & Music Assistant Setup
1. Adopt the discovered `pocket-assistant` device under **Settings -> Devices & Services**.
2. **Music Metadata Mirroring**: Copy `homeassistant/packages/music_assistant_esphome_mirror.yaml` into your `/config/packages/` directory and reload Template Entities under **Developer Tools -> YAML**.
3. **Library Browsing Blueprint**: Import `homeassistant/blueprints/script/music_assistant_browse.yaml` into Home Assistant (**Settings -> Automations & Scenes -> Blueprints**), create a script from it, and save it as `script.music_assistant_browse`.

---

## Core Capabilities

### Audio Pipeline & Music Assistant
* **Local & Remote Audio Control**: Functions as a standalone local media player via Sendspin FLAC streaming, or as a handheld remote managing external household speakers (Sonos, AirPlay, Chromecast, receivers).
* **Multi-Room Speaker Handoff & Takeover**: Transfer active playback queues between rooms with a dynamic 4-slot menu. The active speaker surfaces to Slot 1, while Pocket Assistant sits at Slot 2 for single-tap return.
* **Universal Active Album Art**: Displays full-color edge-to-edge artwork (Full Screen default, Letterbox, or Compact) whether streaming locally or controlling remote speakers, with automatic fallback to a high-contrast vinyl disc when no art is available.
* **Visual Playback Feedback**: Radial elapsed track progress arc, right-flank heads-up volume badge (centered between volume controls without obstructing album art), and instant optimistic play/pause state transitions.
* **4-Slot Library Browser**: Browse Music Assistant favorites, playlists, artists, albums, radio, podcasts, and audiobooks on-device with dual-action play (`▶`) and drill-down (`>`) touch cards.

### Voice Assistant Satellite
* **Push-to-Talk Assist**: Side button instantly routes audio to the ES7210 microphone, pauses local music, and activates the Assist satellite pipeline.
* **Dynamic Audio Ducking**: Software mixer ducks local playback by 20 dB during Voice Assistant speech synthesis.
* **Hardware & Touch Dismissal**: Physical side button immediately halts and mutes an active voice session; full-screen touch modal interception allows tapping the glass to dismiss without triggering background app controls.

### Multi-Tier Power Management
* **Tier 1 (Active)**: Full 40MHz QSPI AMOLED rendering and sensor polling.
* **Tier 2 (Display Standby)**: Screen turns off after an inactivity timeout (default 15s on battery), powering down the speaker amplifier and gyroscope while keeping the accelerometer active. Wakes in under 50ms upon physical pickup or top crown click.
* **Tier 3 (Deep Sleep Hibernation)**: Enters ultra-low-power sleep (<50µA) via deliberate crown long-press (>1.5s with a 5-second abortable countdown) or extended standby inactivity. Hardware pad holds isolate display, touch, and amplifier lines to eliminate battery drain.
* **Docked Screensaver Mode**: When connected to USB-C power, the device can optionally run an ambient screensaver (defaulting to the kinetic Aurora Borealis plasma ribbon) without entering deep sleep.

### Graphics & System Engine
* **High-Throughput QSPI Bus**: 40MHz Quad-SPI display interface pushes full 466×466 frames in ~12ms, maintaining over 88% CPU idle headroom.
* **Zero-Dead-Zone Touch Handling**: Continuous touch boundary tessellation across the entire round screen prevents dropped taps and misdirected inputs.
* **Touch Priority Yielding**: Hardware touch interrupt pin (GPIO11) is monitored during rendering; display redraws yield instantly when a touch is detected, eliminating input latency.
* **Radial OTA Progress Indicator**: Full-screen 360° circular progress arc with smooth 10 FPS interpolation and an orbiting pip during firmware updates.
* **Integrated Apps**: Vintage Heuer-inspired split-lap chronograph stopwatch with mechanical crown pusher latency compensation, modern watch face with vertical battery gauge, Sky & Weather 4-view astronomical and atmospheric suite (Current conditions, 5-Day forecast, 180° Sun Arc ephemeris, and 108px detailed Lunar maria globe), 6-axis IMU motion physics games (Marble Maze, Archery Target, Breakout with edge-to-edge paddle defense), and a 2-tier hierarchical system diagnostic hub.

---

## Technical Approach & Engineering Solutions

### Dynamic GPIO Matrix I2S Clock Multiplexing
The Waveshare 1.75C board routes both the ES7210 ADC (microphone) and ES8311 DAC (speaker) to shared physical clock lines (BCLK GPIO9, WS GPIO45, MCLK GPIO16). Operating standard duplex I2S caused bus contention and peripheral locking.

To resolve this, firmware configures two independent master I2S buses and dynamically reassigns the physical pins at runtime via ESP32-S3 ROM GPIO matrix calls:
* `route_i2s_to_mic`: Connects clock lines to internal I2S0 peripheral signals during voice capture.
* `route_i2s_to_spk`: Connects clock lines to internal I2S1 peripheral signals during playback.

This enables collision-free operation between local playback and Voice Assistant without requiring hardware board modifications.

### Server-Side Pagination for Music Assistant
Microcontrollers have limited RAM and cannot parse massive multi-megabyte JSON payloads from large music libraries without stalling the main loop and causing audio glitches.

The companion Home Assistant Script Blueprint solves this by offloading all query filtering, sorting, and pagination to Home Assistant. The ESP32 sends a compact RPC request with the target category, page number, and page size (4 items). Home Assistant queries Music Assistant, formats only the four requested items into primitive string parameters, and dispatches them back via native ESPHome API actions. The microcontroller updates its display buffer with zero client-side JSON overhead.

### Single-Authority Volume Control & Boot Gain Lock
To eliminate volume fighting between Home Assistant sliders and physical hardware:
* A single master media player (`pocket_music_player`) owns the ES8311 DAC.
* DAC gain initialization (`audio_dac.set_volume: 70%`) and initial muting are pinned to boot `priority: -100` (after hardware driver setup), preventing 0 dB (100% full-scale) power-on blasts.

---

## Hardware Controls Reference

| Control | State / Context | Function |
| :--- | :--- | :--- |
| **Top Crown (Short Click)** | Standby (Screen Off) | Wakes display to active brightness. |
| **Top Crown (Short Click)** | Stopwatch Page | Mechanical chronograph Start / Stop (with 200ms latency compensation). |
| **Top Crown (Short Click)** | Submenus / Overlays / Games | Back / Exit to parent application. |
| **Top Crown (Short Click)** | Main App Pages | Enters screen standby immediately. |
| **Top Crown (Long Press >1.5s)** | Any Screen | Starts 5-second abortable Hibernation Countdown. |
| **Side Button (Click)** | Normal / Standby | Activates Voice Assistant (listening mode). |
| **Side Button (Click)** | Voice Assistant Active | Cancels Voice Assistant immediately and mutes audio. |
| **Screen Tap (Center)** | Music Player | Toggles Play / Pause. |
| **Screen Tap (Right Flank)** | Music Player | Volume Up (+, top right), Volume Down (-, bottom right), Next Track (▶\|, middle right). |
| **Screen Tap (Left Flank)** | Music Player | Previous Track (\|◀, middle left), Music Menu (☰, bottom left). |
| **Subpage 2x2 Grid / Screen Tap** | Sky & Weather Page | 4-View Navigation: Standardized 2x2 system-sized pill buttons ([ Current | Forecast ] / [ Sun | Moon ]), central screen tap cycle, and upper-right night sky Moon shortcut to Moon view. |
| **Bottom Bar (< / >)** | Main App Pages | Navigates through app deck (Clock $\leftrightarrow$ Stopwatch $\leftrightarrow$ Music $\leftrightarrow$ Games $\leftrightarrow$ Sky & Weather $\leftrightarrow$ System). |

---

## Repository Structure

```text
esphome-pocket-assistant/
├── pocket-assistant.yaml          # Master node configuration & substitutions
├── pocket-assistant-1.75c.yaml    # Standalone 1.75C board entry configuration
├── va_cancel_helper.h             # C++ Voice Assistant cancellation hook
├── LICENSE                        # Apache 2.0 open-source license
├── README.md                      # Project documentation
├── boards/
│   └── waveshare_175c.yaml        # Hardware Abstraction Layer (HAL) pinouts & buses
├── core/
│   ├── audio.yaml                 # Dual I2S GPIO matrix, DAC gain lock, Voice Assistant
│   ├── power.yaml                 # Multi-tier power management, IMU motion wake, deep sleep
│   └── ui.yaml                    # UI engine, watch faces, chronograph, music, games, system
└── homeassistant/
    ├── blueprints/
    │   └── script/
    │       └── music_assistant_browse.yaml    # Script Blueprint for MA library browsing
    └── packages/
        └── music_assistant_esphome_mirror.yaml # Drop-in HA helper & sensor package
```

---

## License
Distributed under the Apache 2.0 License. See [`LICENSE`](LICENSE) for details.
