# Pocket Assistant

A local-first handheld companion, voice satellite, and universal media remote for Home Assistant and Music Assistant, built on the Waveshare 1.75" circular AMOLED ESP32-S3 development board.

[![Build & Release Firmware](https://github.com/slampton/esphome-pocket-assistant/actions/workflows/build.yml/badge.svg)](https://github.com/slampton/esphome-pocket-assistant/actions/workflows/build.yml)
[![Version](https://img.shields.io/badge/Version-v2.0-blue.svg)](https://github.com/slampton/esphome-pocket-assistant/releases)
[![ESPHome Version](https://img.shields.io/badge/ESPHome-2026.10.0%2B-blue.svg)](https://esphome.io)
[![Home Assistant](https://img.shields.io/badge/Home%20Assistant-2026.10.0%2B-41BDF5.svg)](https://www.home-assistant.io)
[![Music Assistant](https://img.shields.io/badge/Music%20Assistant-2.10.7%2B-purple.svg)](https://music-assistant.io)
[![License](https://img.shields.io/badge/License-Apache%202.0-green.svg)](LICENSE)

Pocket Assistant pairs a 466×466 circular AMOLED touchscreen, dual microphones, a speaker, a 6-axis IMU, and an AXP2101 power management IC with ESPHome, Home Assistant, and Music Assistant. Instead of acting as a passive wall display or stationary voice puck, it works as an untethered handheld and docked desk companion—combining an interactive Home Hub with live 20 FPS weather, on-board wake word and push-to-talk Voice Assistant, local FLAC streaming and multi-room speaker control, a 4-device universal TV remote, motion-controlled arcade games, and a 3-tier power state machine (`<40 µA` hardware hibernate).

<p align="center">
  <img width="30%" alt="Clock & Watch Faces" src="https://github.com/user-attachments/assets/abdd0e28-8108-4c94-8e7a-bb57b551cb2b" />
  <img width="30%" alt="Voice Assistant" src="https://github.com/user-attachments/assets/ea6c7aa3-625c-4e8e-b9ac-3aea752844f8" />
  <img width="30%" alt="Music Player & Library" src="https://github.com/user-attachments/assets/f65e9f1b-d211-4396-bcd0-2b36597e6f77" />
</p>
<p align="center">
  <img width="30%" alt="Sky & Weather Suite" src="https://github.com/user-attachments/assets/03a5e563-834b-4627-8146-005888f30950" />
  <img width="30%" alt="Motion Physics Games" src="https://github.com/user-attachments/assets/4deb32e4-7dae-4d46-b03a-886e50cb91d1" />
  <img width="30%" alt="Timers & Alarms" src="https://github.com/user-attachments/assets/0445a944-c109-455b-9d47-b5cad09d6276" />
</p>

---

## Highlights in v2.0

* **Interactive Home Hub & Smart Live Activity Card**: A single glanceable home screen with a live 20 FPS weather sky, tap-to-open hero clock and weather summary, a contextual **Wake Greeting**, an auto-prioritized **Live Activity Card** (with an 8.0s upward carousel reel, urgency pinning for running Stopwatch / Timers `<= 60s`, inline Play/Pause/Disarm, and a translucent multi-activity overlay), and a **3-Icon Sliding Window Dock** (`Music`, `Remote`, `Games` -> `Tools`).
* **On-Board Wake Word + 5 Voice Assistant Visual Profiles**: Runs 5 selectable `micro_wake_word` models in Octal PSRAM (**Okay Nabu**, **Hey Luna**, **Hey Jarvis**, **Hey Mycroft**, **Alexa**) with per-model sensitivity tuning, NVS persistence, and 5 20 FPS visual profiles (**Nebula**, **Solaris**, **Jarvis**, **Alexa**, and **Gemini**). *(Note: Because the board shares I2S clock lines between the mic and speaker, hands-free wake word is active whenever the local speaker is idle—including while controlling external room speakers—and automatically pauses during local speaker playback. While local music is playing, press the **Side Button (`GPIO0`)** to automatically pause playback, speak to Assist, and auto-resume music afterward.)*
* **Lossless Music Assistant Player & Track Inspector**: Stream FLAC audio locally via Sendspin or control household speakers (Sonos, AirPlay, Cast, Denon/Marantz). Transfer queues between rooms, group/ungroup speakers, browse your library on-device, or tap the metadata bar to open the **Track & Album Inspector** with a 276px position scrubber, `-15s`/`+15s` seek, album tracklist, artist discography, and Up Next queue.
* **4-Device Universal TV & Media Remote**: Control up to 4 preconfigured TVs, streamers, or AVRs with automatic protocol detection (**Android TV / Shield / Fire TV / Bravia**, **Denon / Marantz AVR**, **LG webOS**, **Apple TV**, **Roku**, **Samsung TV**, and **Custom**), Slot-1 Master Power, and per-device power overrides.
* **20 FPS 3D Weather & Telescopic Lunar Suite**: Procedural 3D cumulus and wispy cloud layers with IMU wrist-tilt parallax, Ghibli-style tapered rain filaments with horizon splashes, 3D helical snowfall, multi-pulse lightning, tumbling 3D leaves, migrating birds, a 5-day forecast, solar horizon arc, and a `.rodata` telescopic Lunar Albedo renderer.
* **20 FPS 360° Motion Arcade**: Gimbal-lock-free 3D IMU calibration powers 4 arcade games—**Marbles**, **Archery**, **Breakout**, and **Chomp for Kids**—complete with RTTTL sound effects and persistent high scores.
* **8-Tool Utility Suite & Find My Device**: Built-in **3-Slot Timers** (synced 2-way with Home Assistant Assist voice timers), **3-Slot Alarms**, **1970 Heuer Split-Lap Stopwatch** (with physical Crown pusher), **3D Dual D6 Dice Roll**, **Flashlight Torch**, **Calculator**, **20 FPS Bubble Level**, **3D Silver Home Assistant Coin Flip**, and an acoustic **Find My Device** radar beacon.
* **5 Docked 20 FPS Screensavers & `<40 µA` Hibernate**: **Aurora Borealis**, **Starfield**, **Matrix Rain**, **Fireworks**, and **Ambient Clock** when docked to USB, plus `<5 ms` hardware IRQ standby wake and `<40 µA` AXP2101 rail-cut hibernate on battery.

---

## Quick Start & Installation

### Requirements
* **Board**: Waveshare ESP32-S3-Touch-AMOLED-1.75C (466×466 CO5300 QSPI AMOLED, CST9220 touch, ES8311 DAC, ES7210 dual-mic ADC, AXP2101 PMIC, QMI8658 6-axis IMU, 32MB Octal Flash, 8MB Octal PSRAM).
* **Home Assistant Core `2026.10.0+`** & **ESPHome `2026.10.0+`**.
* **Music Assistant `2.10.7+`** *(optional, for local Sendspin streaming and library browsing)*.
* **[Actions for Music Assistant (`mass_queue`)](https://github.com/droans/mass_queue)** *(optional HACS integration, required for browsing individual podcast episodes and full Up Next play queues)*.

### 1. Flash the Firmware
* **Web Installer (Recommended)**: Connect your board via USB-C in Chrome or Edge and open the **[Pocket Assistant Web Installer](https://slampton.github.io/esphome-pocket-assistant/)** to flash v2.0 and connect to Wi-Fi via Improv BLE.
* **ESPHome Device Builder (Take Control / Custom Substitutions)**: When adopting the device in ESPHome Device Builder, override any `substitutions:` you want at the top of your YAML:
```yaml
substitutions:
  name: "pocket-assistant"
  friendly_name: "Pocket Assistant"
  weather_entity: "weather.forecast_home"            # Primary Home Assistant weather entity
  tv_remote_entity: "remote.none"                    # Slot 1: Primary TV/Device & Master Power target
  tv_remote_entity_2: "media_player.none"            # Slot 2: Optional 2nd device (remote.* or media_player.*)
  tv_remote_entity_3: "remote.none"                  # Slot 3: Optional 3rd device ("remote.none" hides slot)
  tv_remote_entity_4: "remote.none"                  # Slot 4: Optional 4th device ("remote.none" hides slot)
  tv_remote_profile: "Auto-Detect"                   # Slot 1..4 Profile ("Auto-Detect", "Android TV / Fire TV / Bravia",
  tv_remote_profile_2: "Auto-Detect"                 #   "Denon / Marantz AVR", "LG webOS", "Apple TV", "Roku",
  tv_remote_profile_3: "Auto-Detect"                 #   "Samsung TV", or "Custom")
  tv_remote_profile_4: "Auto-Detect"
  mass_queue_config_entry: ""                        # Optional: mass_queue Config Entry ID for Podcasts & Queue

packages:
  slampton.pocket-assistant-175c:
    url: https://github.com/slampton/esphome-pocket-assistant
    ref: main
    file: pocket-assistant-1.75c.yaml
    refresh: 0s

esphome:
  name: ${name}
  name_add_mac_suffix: false
  friendly_name: ${friendly_name}

api:
  encryption:
    key: "!secret api_encryption_key"

wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password
```

### 2. Install the Home Assistant Companion Package (Single File, Zero Blueprints)
Copy [`homeassistant/packages/pocket_assistant_companion.yaml`](homeassistant/packages/pocket_assistant_companion.yaml) into your Home Assistant `/config/packages/` folder. It sets up all **6 companion entities** in one place (no Blueprints required):
1. `input_text.pocket_assistant_active_speaker` — Active media player target (defaults to `media_player.pocket_assistant`).
2. `input_text.pocket_assistant_weather_entity` — Weather entity (defaults to `weather.forecast_home`, auto-falling back to your first `weather.*` entity).
3. `input_text.pocket_assistant_base_url` — Optional base URL override for custom HA ports (defaults to `http://homeassistant.local:8123`).
4. `sensor.pocket_assistant_active_media` — Idle-guarded media metadata mirror (`title`, `artist`, `album`, `duration`, `position`, `volume`, `media_type`, `supported_features`, `art_url`).
5. `sensor.pocket_assistant_weather_forecast` — Hourly 5-day forecast serializer (`DAY:HIGH:LOW:COND` + `high`/`low`).
6. `script.pocket_assistant_music_browse` — Paged 5-slot Music Assistant browser for speakers, favorites, playlists, artists, albums, radio, podcasts, audiobooks, album tracks, and active queues.

Ensure `configuration.yaml` includes packages under `homeassistant:`:
```yaml
homeassistant:
  packages: !include_dir_named packages
```
Then reload **Template Entities**, **Input Texts**, and **Scripts** under **Developer Tools -> YAML** (or restart Home Assistant).

---

## Navigation & Hardware Controls

### Hub-and-Spoke Touch Navigation
Pocket Assistant v2.0 uses a fast hub-and-spoke layout centered on the **Home Hub (`Page 1`)**:
* **Top Date Header (`y = 52`)**: Tap to open **Home Settings** (toggle live weather sky and individual dock icons).
* **Sun / Moon Corner Icon (`360, 86`)**: Tap to jump directly to the **Sun** or **Moon** telemetry view.
* **Hero Digital Clock (`y = 128`)**: Tap to open the full **Clock** watch face (`Analog` or `Digital`).
* **Weather Summary Line (`y = 200`)**: Tap to open **Sky & Weather** (`Current Conditions`).
* **Wake Greeting & Live Activity Card (`y = 271`)**: Shows a contextual **Wake Greeting** on cold boot / Hibernate wake, then transitions to the **Live Activity Card** whenever a Timer, Stopwatch, Music queue, or Alarm is active. Tap the card body to open that activity, tap the inline action circle to Play/Pause/Disarm in place, or tap the right `▼` chevron (when 2+ activities are active) to open the **Translucent Activity Overlay**.
* **3-Icon Sliding Window Dock (`y = 352`)**: Shows **Music**, **TV Remote**, and **Games** on Page 0 with a right `>` chevron to slide **Tools** into view.
* **Bottom Chin (`y = 421` / `426`)**: Tap the **Battery & Settings Gear** on Home (`y = 421`) to open the **System Hub**; tap **`Home`** (on top-level apps) or **`< Back`** (on nested submenus, tools, and active games) at `y = 426` to return.

### Physical Buttons
| Control | Context | Action |
| :--- | :--- | :--- |
| **Top Crown (Short Click)** | Standby (Screen Off / Screensaver) | Instant `<5 ms` wake via `GPIO3` hardware interrupt. |
| **Top Crown (Short Click)** | Alarm / Timer / Find My Ringing | Snoozes alarm (+5 min) or dismisses Timer / Find My alert. |
| **Top Crown (Short Click)** | Voice Assistant Active | Cancels Voice Assistant and mutes TTS audio. |
| **Top Crown (Short Click)** | Stopwatch (`Tools -> Stopwatch`) | Tactile **Start / Stop** chronograph pusher (with 200ms dwell compensation). |
| **Top Crown (Short Click)** | Any Screen (except Stopwatch) | Turns the screen off and enters **Tier 2 Standby**. |
| **Top Crown (Long Press >1.5s)** | Any Screen | Initiates **Tier 3 Hardware Hibernate** (`<40 µA`, instant or 5s countdown). |
| **Side Button (`GPIO0` Click)** | Normal / Local Music / Standby / Games | Triggers **Voice Assistant** (auto-pauses & resumes local music when hands-free wake word is paused by local speaker output), dismisses alerts, or fires in games. |

---

## Power & Resource Profile

| Mode / Resource | Measured Budget | Implementation |
| :--- | :--- | :--- |
| **Tier 1: Active Mode** | `110 – 160 mA` | 80 MHz QSPI display @ 20 FPS (`42 ms` idle / `75 ms` audio), 30 Hz touch, 6-axis IMU active |
| **Tier 2: Display Standby** | `15 – 25 mA` (`<5 ms` wake) | Display & PA amp off, IMU gyro off (`0x01` pickup wake or `0x00` off), `GPIO3` PMU IRQ gating |
| **Tier 3: Hardware Hibernate** | `< 40 µA` (`~400 ms` UI wake) | `global_preferences->sync()` + clean API disconnect + AXP2101 `0x10 = 0x01` rail cut |
| **Memory Footprint** | `~155 KB` free internal SRAM | 8MB Octal PSRAM @ 80 MHz (`microFLAC`, `micro_wake_word`, NVS cache, FreeRTOS stacks) + `.rodata` LUTs |

---

## Repository Layout

```text
esphome-pocket-assistant/
├── .github/workflows/build.yml        # ESPHome 2026.10 CI build & GitHub Pages Web Installer workflow
├── adrs/                              # Architectural Decision Records (ADR-001 through ADR-006)
├── boards/waveshare_175c.yaml         # Board HAL (QSPI Display, CST9220 Touch, Dual I2S, IMU, PMU & PSRAM)
├── core/
│   ├── audio.yaml                     # Mixer, resamplers, Sendspin, 5-model Micro Wake Word & Voice Assistant
│   ├── power.yaml                     # 3-tier power state machine, GPIO3 IRQ gate & battery fuel gauge
│   └── ui.yaml                        # 20 FPS display loop, Home Hub, apps, tools & 30 Hz touch router
├── homeassistant/packages/
│   └── pocket_assistant_companion.yaml # Single-file HA companion package (helpers, sensors & browse script)
├── pocket-assistant-1.75c.yaml        # Remote package entry point for ESPHome adoption
├── pocket-assistant.yaml              # Master node configuration, substitutions & API actions
├── ui_core.h                          # Geometry, .rodata LUTs, 3D projection, blitter & touch helpers
├── ui_ambient_va.h                    # 20 FPS Docked Screensavers & 5-Profile Voice Assistant engine
├── ui_weather_sky.h                   # Telescopic Lunar Albedo LUTs & 20 FPS 3D Weather Motion engine
├── ui_games.h                         # 360-Deg IMU tilt & 4-Game 20 FPS Arcade suite
├── ui_tools_apps.h                    # Home Hub, 8-Tool Suite, Clock, Music, TV Remote & System Hub
├── ui_toolkit.h                       # Umbrella C++20 UI toolkit header
├── va_cancel_helper.h                 # Voice Assistant session lifecycle, NVS wake-word sync & API disconnect
└── SPECIFICATION.md                   # Complete hardware, pinout, memory & software architecture spec
```

---

## Development & License

Every hardware route, register map, and UI flow is validated on physical Waveshare ESP32-S3-Touch-AMOLED-1.75C hardware. AI tools were used to assist with mathematical LUT generation, procedural 3D/weather shaders in `ui_toolkit.h`, and documentation.

Distributed under the Apache 2.0 License. See [`LICENSE`](LICENSE) for details.