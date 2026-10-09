# Pocket Assistant

A local-first handheld companion and media remote for Home Assistant and Music Assistant, built on the Waveshare 1.75" circular AMOLED ESP32-S3 development board.

[![Version](https://img.shields.io/badge/Version-v1.2-blue.svg)](https://github.com/slampton/esphome-pocket-assistant/releases)
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
* **Web Installer (Recommended)**: Connect the device via USB-C in a WebSerial-supported browser (Chrome, Edge, Opera) and visit the **[Pocket Assistant Web Installer](https://slampton.github.io/esphome-pocket-assistant/)** to install v1.2 with one click.
* **ESPHome Dashboard**: Alternatively, adopt the device using a minimal remote package include:
```yaml
substitutions:
  name: "pocket-assistant"
  friendly_name: "Pocket Assistant"
  version: "1.2"
  local_player_id: "media_player.pocket_assistant"
  weather_entity: "weather.forecast_home" # Set to your primary Home Assistant weather entity (e.g. weather.home)

packages:
  remote_pocket_assistant:
    url: https://github.com/slampton/esphome-pocket-assistant
    ref: main
    files:
      - pocket-assistant-1.75c.yaml
```

### 2. Home Assistant Companion Package Setup
Pocket Assistant uses a single consolidated companion package (`pocket_assistant_companion.yaml`) containing 2 high-efficiency template sensors (Active Media and 5-Day Weather Forecast).

Choose whichever method matches your Home Assistant configuration:
* **Option A: `packages/` Directory (Recommended)**:
  Copy `homeassistant/packages/pocket_assistant_companion.yaml` into your Home Assistant `/config/packages/` directory.
* **Option B: Split `templates.yaml`**:
  If you organize template sensors via `template: !include templates.yaml` in `configuration.yaml`, copy the contents of the `template:` block from `pocket_assistant_companion.yaml` directly into your `templates.yaml`, and add the `input_text:` helpers to `configuration.yaml` (or `input_texts.yaml`).

After adding the configuration, reload Template Entities under **Developer Tools -> YAML** (or restart Home Assistant).

### 3. Music Assistant Setup
1. Adopt the discovered `pocket-assistant` device under **Settings -> Devices & Services**.
2. **Library Browsing Blueprint**: Import `homeassistant/blueprints/script/music_assistant_browse.yaml` into Home Assistant (**Settings -> Automations & Scenes -> Blueprints**), create a script from it, and save it as `script.music_assistant_browse`.

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
* **Tier 1 (Active)**: High-performance 40MHz Quad-SPI AMOLED rendering, real-time sensor processing, and capacitive touch interaction.
* **Tier 2 (Display Standby)**: Screen turns off after an inactivity timeout (default 15s on battery), powering down the speaker amplifier and gyroscope while keeping motion wake armed. Wakes in <5ms via hardware PMU interrupt (GPIO3) upon crown button press, or upon physical pickup.
* **Tier 3 (Deep Sleep Hibernation)**: Enters ultra-low-power sleep (<40µA) via deliberate crown long-press (>1.5s with a 5-second abortable countdown) or extended standby inactivity. The AXP2101 PMIC cuts all peripheral power rails, while the side button is completely decoupled to prevent accidental wake in pockets. Wakes exclusively via the top crown hardware pin.
* **Docked Screensaver Mode**: When connected to USB-C power, the device can optionally run an ambient screensaver (kinetic Aurora Borealis plasma ribbon, Ambient Clock, Starfield, Matrix Rain, or Solar Flare) without entering sleep.

### Graphics & System Engine
* **High-Throughput QSPI Bus**: 40MHz Quad-SPI display interface pushes full 466×466 frames in ~12ms, maintaining over 88% CPU idle headroom.
* **Zero-Dead-Zone Touch Handling**: Continuous touch boundary tessellation across the entire round screen prevents dropped taps and misdirected inputs.
* **Touch Priority Yielding**: Hardware touch interrupt pin (GPIO11) is monitored during rendering; display redraws yield instantly when a touch is detected, eliminating input latency.
* **Radial OTA Progress Indicator**: Full-screen 360° circular progress arc with smooth 10 FPS interpolation and an orbiting pip during firmware updates.
* **Precision 10 FPS Stopwatch (Page 1)**: Horological split-lap chronograph inspired by vintage Heuer and Speedmaster timepieces, featuring a high-contrast matte black AMOLED dial, faceted stainless steel pointer, machined piston pushers, Rattrapante split ghost hand, and a guaranteed 55ms CPU idle window per cycle for zero-latency touch responsiveness.
* **Customizable App Deck & Hierarchical System Hub (Page 4)**: 8-button balanced 2x4 system dashboard with dedicated submenus for Display, Clock & Time, Sleep & Wake, Audio, Voice, App Deck, and System Diagnostics. The App Deck submenu allows toggling visibility and default start application for all apps on-device.
* **Integrated Apps Suite**: Modern watch face with complication format toggling, Sky & Weather 4-view astronomical and atmospheric suite (Current conditions with realistic daylight sky and night starfield, 5-day aligned forecast, Sun arc, and lunar telemetry Moon view), 6-axis IMU motion physics games (Marble Maze, Archery Target, Breakout with paddle defense), and Countdown Timer & Multi-Alarm Suite with local RTC countdown and crown snooze.

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
| **Top Crown (Short Click)** | Alarm Ringing | Snoozes alarm for 5 minutes. |
| **Top Crown (Short Click)** | Timer Ringing | Dismisses timer alert. |
| **Top Crown (Short Click)** | Standby (Screen Off) | Instant hardware interrupt wake (<5ms) to active brightness. |
| **Top Crown (Short Click)** | Stopwatch Page | Starts or stops the chronograph timer. |
| **Top Crown (Short Click)** | Timer Page | Toggles Timer Start / Pause / Resume. |
| **Top Crown (Short Click)** | Submenus / Overlays / Games | Back / Exit to parent application. |
| **Top Crown (Short Click)** | Main App Pages | Enters screen standby immediately. |
| **Top Crown (Long Press >1.5s)** | Any Screen | Starts 5-second abortable Hibernation Countdown (or instant hibernate if configured). |
| **Side Button (Click)** | Normal / Standby | Activates Voice Assistant (listening mode), or fires in-game action. |
| **Side Button (Click)** | Voice Assistant Active | Cancels Voice Assistant immediately and mutes audio. |
| **Screen Tap (Upper 70%)** | Stopwatch Page | Start / Stop toggle with generous zero-deadzone touch envelope. |
| **Screen Tap (Lower Quadrants)** | Stopwatch Page | Left quadrant: LAP (Rattrapante ghost hand); Right quadrant: RESET. |
| **Screen Tap (Center)** | Music Player | Toggles Play / Pause. |
| **Screen Tap (Right Flank)** | Music Player | Volume Up (+, top right), Volume Down (-, bottom right), Next Track (▶\|, middle right). |
| **Screen Tap (Left Flank)** | Music Player | Previous Track (\|◀, middle left), Music Menu (☰, bottom left). |
| **Subpage 2x2 Grid / Screen Tap** | Sky & Weather Page | 4-View Navigation: Standardized 2x2 pill buttons ([ Current \| Forecast ] / [ Sun \| Moon ]), central screen tap cycles sequentially through all 4 views, and upper-right celestial shortcut. |
| **Bottom Bar (< / >)** | Main App Pages | Navigates through active app deck (Clock $\leftrightarrow$ Stopwatch $\leftrightarrow$ Music $\leftrightarrow$ Games $\leftrightarrow$ System $\leftrightarrow$ Sky & Weather $\leftrightarrow$ Timer & Alarm). |

---

## Home Assistant Backend Architecture

Pocket Assistant uses a streamlined 2-sensor companion backend architecture designed to minimize database write volume, eliminate network polling overhead, and provide reliable local telemetry.

### 1. Weather Telemetry & 5-Day Forecast Pipeline
* **Dual Ingestion Architecture**:
  * **Current Conditions**: The watch face and Sky & Weather app subscribe directly to your primary Home Assistant weather entity (`weather_entity`) for live temperature, humidity, pressure, and current condition state.
  * **Daily & 5-Day Forecast**: Modern Home Assistant weather entities expose forecast arrays via the `weather.get_forecasts` action. The companion package fetches the daily forecast hourly (and on startup) and serializes it into `sensor.pocket_assistant_weather_forecast`:
    * **State**: Semicolon-delimited 5-day forecast string (`DAY:HIGH:LOW:COND;...`).
    * **Attributes**: Structured `high` and `low` floats for today's forecast.
  * **Resilient On-Device Fallback**: If attribute updates are ever unavailable or reloading, Page 5 firmware automatically parses today's high and low directly from the forecast string, preventing display gaps.
  * **Dynamic Entity Re-Targeting**: Change the monitored weather service on the fly without re-flashing firmware by updating `input_text.pocket_assistant_weather_entity`.

### 2. Active Media Mirror & Music Assistant Integration
* **Consolidated Media Mirroring**: Active playback metadata is unified into a single companion entity, `sensor.pocket_assistant_active_media`:
  * **State**: Playback state (`playing`, `paused`, `idle`).
  * **Attributes**: `title`, `artist`, `album`, `duration`, `position`, `volume`, `media_type`, `art_url`.
  * **Database Optimization**: Consolidating 9 separate legacy helper sensors into 1 attribute-rich entity cuts Home Assistant state database writes during active playback by 89% while delivering synchronous metadata packets to the device over the ESPHome native API.
  * **Active Speaker Routing**: Dynamically follow any external media player by setting `input_text.pocket_assistant_active_speaker`.
* **Server-Side Pagination Blueprint**: `homeassistant/blueprints/script/music_assistant_browse.yaml` offloads multi-megabyte JSON library queries from device memory to Home Assistant, returning paginated 4-item batches directly to on-screen interactive cards.

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
        └── pocket_assistant_companion.yaml     # Consolidated Home Assistant companion package
```

---

## Development & AI Transparency

AI tools were used during the development and documentation of this project.

* **Human-Directed & Hardware-Verified**: Every schematic, bus architecture, GPIO matrix route, and register map is validated directly on physical Waveshare ESP32-S3-Touch-AMOLED-1.75C hardware. No unverified code is committed.
* **How AI Was Utilized**: AI assistance was used for rapid prototyping, mathematical modeling (such as trigonometric vector math for the mechanical chronometer dial and UI layout geometry), automated AST schema validation, and technical documentation.
* **Local-First & Open Standards**: All firmware architecture adheres strictly to native ESPHome standards, local-first principles, and official Home Assistant design patterns.

Feedback, peer review, and pull requests from the community are always welcome.

---

## License
Distributed under the Apache 2.0 License. See [`LICENSE`](LICENSE) for details.
