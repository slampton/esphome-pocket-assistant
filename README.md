# Pocket Assistant

A local-first handheld companion, voice satellite, and universal media remote for Home Assistant and Music Assistant, built on the Waveshare 1.75" circular AMOLED ESP32-S3 development board.

[![Build & Release Firmware](https://github.com/slampton/esphome-pocket-assistant/actions/workflows/build.yml/badge.svg)](https://github.com/slampton/esphome-pocket-assistant/actions/workflows/build.yml)
[![Version](https://img.shields.io/badge/Version-v1.3-blue.svg)](https://github.com/slampton/esphome-pocket-assistant/releases)
[![ESPHome Version](https://img.shields.io/badge/ESPHome-2026.10.0%2B-blue.svg)](https://esphome.io)
[![Home Assistant](https://img.shields.io/badge/Home%20Assistant-2026.10.0%2B-41BDF5.svg)](https://www.home-assistant.io)
[![Music Assistant](https://img.shields.io/badge/Music%20Assistant-2.10.7%2B-purple.svg)](https://music-assistant.io)
[![License](https://img.shields.io/badge/License-Apache%202.0-green.svg)](LICENSE)

Pocket Assistant started as a personal home lab project to explore what is possible when pairing modern ESP32-S3 hardware with Home Assistant, Music Assistant, and ESPHome. Rather than acting as a passive sensor display or a stationary voice satellite alone, it combines local FLAC/Opus audio playback, multi-room queue transfer and speaker group management, interactive library browsing, universal multi-device TV remote control, on-board wake word and push-to-talk Voice Assistant support, and a 3-tier power state machine (`<40 µA` hardware hibernate) in a 466×466 circular handheld form factor.

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

## Quick Start & Installation

### Prerequisites & Version Requirements
- **Home Assistant Core `2026.10.0` or newer**: Required for client-initiated native API dial-back (`api: outgoing_connection:`, introduced in `aioesphomeapi 46.4.0` / Home Assistant `2026.10.0`).
- **ESPHome `2026.10.0` or newer**: Required for `api: outgoing_connection:`, `nvs_cache_in_psram: true`, `flash_chip: gd`, and `microFLAC v0.3.0`.
- **Music Assistant `2.10.7` or newer** *(optional)*: Required for local Sendspin FLAC/Opus playback and on-device library browsing.

### Supported Hardware
- **Board**: Waveshare ESP32-S3-Touch-AMOLED-1.75C (1.75" 466×466 round AMOLED, CO5300 80 MHz QSPI display, CST9220 capacitive touch, ES8311 DAC, ES7210 dual-mic ADC, AXP2101 PMIC, QMI8658 6-axis IMU, 32MB GigaDevice Octal Flash, 8MB Octal PSRAM).

### 1. Flash the Firmware
- **Web Installer (Recommended)**: Connect the device via USB-C in a WebSerial-supported browser (Chrome, Edge, Opera) and visit the **[Pocket Assistant Web Installer](https://slampton.github.io/esphome-pocket-assistant/)** to install v1.3 with one click and configure Wi-Fi via Improv BLE.
- **ESPHome Device Builder (Take Control / Manual Adoption)**: When you click **Take Control** on a discovered device in ESPHome Device Builder, it automatically imports the remote package (`pocket-assistant-1.75c.yaml`). Add any `substitutions:` you wish to customize at the top of your device YAML:
```yaml
substitutions:
  name: "pocket-assistant"
  friendly_name: "Pocket Assistant"
  weather_entity: "weather.forecast_home"            # Primary Home Assistant weather entity (default Met.no entity)
  tv_remote_entity: "remote.none"                    # Slot 1: Primary device & Master Power target (e.g. "remote.living_room_tv")
  tv_remote_entity_2: "remote.none"                  # Slot 2: Optional secondary device (remote.* or media_player.*)
  tv_remote_entity_3: "remote.none"                  # Slot 3: Optional secondary device ("remote.none" hides slot)
  tv_remote_entity_4: "remote.none"                  # Slot 4: Optional secondary device ("remote.none" hides slot)

packages:
  slampton.pocket-assistant-175c:
    url: https://github.com/slampton/esphome-pocket-assistant
    ref: main
    file: pocket-assistant-1.75c.yaml

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

### 2. Home Assistant Companion Package Setup (Single Drop-In File, Zero Blueprints)
Pocket Assistant uses a single consolidated companion package (`homeassistant/packages/pocket_assistant_companion.yaml`) that installs **all 6 backend entities** in one step (3 `input_text` helpers, 2 `template` sensors, and 1 Music Assistant browse `script`—no separate Blueprint import required):
1. `input_text.pocket_assistant_active_speaker` — Tracks the active target media player entity ID (defaults to `media_player.pocket_assistant`).
2. `input_text.pocket_assistant_weather_entity` — Monitored weather entity ID (defaults to `weather.forecast_home`, with automatic fallback to your first `weather.*` entity).
3. `input_text.pocket_assistant_base_url` — Optional base URL override for custom Home Assistant ports (defaults to `http://homeassistant.local:8123` if blank).
4. `sensor.pocket_assistant_active_media` — Idle-guarded active media mirror sensor (`title`, `artist`, `album`, `duration`, `position`, `volume`, `media_type`, `supported_features`, `art_url`).
5. `sensor.pocket_assistant_weather_forecast` — Hourly 5-day daily forecast serializer (`DAY:HIGH:LOW:COND` string + `high` and `low` attributes).
6. `script.pocket_assistant_music_browse` — Built-in Music Assistant 5-slot discovery & pagination engine (speakers, favorites, playlists, artists, albums, radio, podcasts, audiobooks, tracks, and queues).

Choose whichever installation method matches your Home Assistant setup:

- **Option A: `packages/` Directory (Recommended — Works With Existing `templates.yaml` & `scripts.yaml`)**:
  > **Note**: Home Assistant's `packages:` loader merges entities cleanly alongside existing `template: !include templates.yaml` and `script: !include scripts.yaml` lines in `configuration.yaml` without conflict.
  1. Ensure your Home Assistant `configuration.yaml` enables packages under the `homeassistant:` block:
     ```yaml
     homeassistant:
       packages: !include_dir_named packages
     ```
  2. Copy `homeassistant/packages/pocket_assistant_companion.yaml` into your Home Assistant `/config/packages/` directory.
  3. Reload **Template Entities**, **Input Texts**, and **Scripts** under **Developer Tools -> YAML** (or restart Home Assistant), and adopt the discovered `pocket-assistant` device under **Settings -> Devices & Services**.

- **Option B: Manual Split Files (Without Enabling `packages:`)**:
  If you prefer not to use a `/config/packages/` directory:
  1. Copy the contents of the `template:` block from `homeassistant/packages/pocket_assistant_companion.yaml` into your `/config/templates.yaml`.
  2. Copy the `pocket_assistant_music_browse:` entry under `script:` into your `/config/scripts.yaml`.
  3. Copy the `input_text:` block into your `/config/configuration.yaml` (or create the 3 `input_text` helpers in the UI under **Settings -> Devices & Services -> Helpers**).
  4. Reload **Template Entities**, **Input Texts**, and **Scripts** under **Developer Tools -> YAML**.

---

## Core Applications & Capabilities

### Watch Faces & Complications (`Clock`)
- **Modern Roman Analog & Modern Digital Bold**: Switch between `.rodata` LUT-accelerated sword hands or a high-contrast digital typography face with 12-hour/24-hour formatting, live battery fuel gauge, and instant `on_time_sync` refresh.

### Audio Pipeline & Music Assistant (`Music`)
- **Local & Remote Audio Control**: Functions as a standalone local media player via Sendspin FLAC streaming, or as a handheld remote managing external household speakers (Sonos, AirPlay, Chromecast, AV receivers).
- **Multi-Room Queue Transfer & Group Management**: Direct 5-slot speaker browser with contextual action sheets: transfer active playback queues between rooms (`TRANSFER QUEUE`), control remote speaker volume and transport (`CONTROL ONLY`), join speakers into synchronized audio groups (`JOIN AUDIO`), or detach groups (`UNJOIN AUDIO`).
- **Full-Bleed Album Art & Track Inspector**: Supports **Full Screen** (466×466 circular bleed), **Letterbox**, and **Standard** artwork modes. Tapping the bottom metadata band opens the **Track & Album Inspector** with a direct position scrubber, `-15s`/`+15s` seek buttons, current album tracklist, artist discography, and active queue viewer.
- **5-Slot Library Browser**: Browse Music Assistant favorites, playlists, artists, albums, radio, podcasts, and audiobooks on-device with configurable sort orders (Alphabetical, Newest on Top, Oldest on Top).

### On-Board Wake Word & Voice Assistant Satellite
- **5 On-Board Wake Word Models + VAD**: Runs `micro_wake_word` in Octal PSRAM with 5 selectable wake words (**Okay Nabu**, **Hey Luna**, **Hey Jarvis**, **Hey Mycroft**, and **Alexa**), independent per-model sensitivity controls, NVS persistence, and configurable power modes (`Docked Only`, `Always On`, `Disabled`), plus side-button (`GPIO0`) push-to-talk.
- **5 Pure-Visual 20 FPS Profiles**: Choose between **Nebula** (default 3D silk ribbons, spectral waves & fluid purple-magenta orb), **Solaris** (radial solar corona & 3D gimbaled astrolabe), **Jarvis** (counter-rotating electric-cyan concentric HUD), **Alexa** (360° spinning royal-blue & cyan bezel ring), and **Gemini** (minimalist 3-dot wave, spinning orbit & capsule equalizer).
- **Dynamic Audio Ducking & Auto-Resume**: Automatically pauses or ducks active music during voice sessions and smoothly fades music back in after TTS responses finish draining.

### Universal Multi-Device TV & Media Remote (`Remote`)
- **Up to 4 Preconfigured Devices & Master Power**: Configure up to 4 target entities (`tv_remote_entity` through `tv_remote_entity_4`; unused slots set to `"remote.none"` are hidden automatically). The top **`Toggle Power`** button in `SETTINGS` always controls **Slot 1** as your Master Power target, while Slots 2–4 provide dedicated per-device power buttons.
- **7 Native Protocol Profiles**: Out-of-the-box support with automatic heuristic detection for **Android TV / Fire TV / Bravia / Shield TV**, **Denon / Marantz AVR**, **LG webOS**, **Apple TV**, **Roku**, **Samsung TV**, and **Custom**.

### 20 FPS Sky & Weather Suite (`Sky & Weather`)
- **4-View Meteorological & Astronomical Suite**:
  1. **Current Conditions**: Live 20 FPS / 13.3 FPS audio-aware motion wallpaper (`Rain`, `Pouring`, `Snow`, `Thunderstorm` with multi-pulse lightning, `Fog & Mist`, `Windy` 3D tumbling leaves, `Clear Day` migrating V-flock birds, `Clear Night` 72-star Fibonacci dome + shooting stars) with IMU wrist-tilt parallax.
  2. **5-Day Forecast**: High/low temperatures and vector condition icons.
  3. **Solar Arc**: Real-time sun elevation/azimuth horizon arc with local sunrise and sunset times.
  4. **Lunar Phase**: Telescopic `.rodata` Lunar Albedo LUT renderer (`LUNAR_ALBEDO_139`) with cubic Hermite terminator shading, Earthshine, and synodic illumination percentage.

### 20 FPS Motion Arcade (`Games`)
- **4 20 FPS Arcade & 3D IMU Tilt Games**: Includes **Marbles** (Cat's-Eye glass marbles with diamond bumper pegs), **Archery** (recurve bow range with crosswind ballistics and pinned arrows), **Breakout** (circular dome brick-breaker with multi-hit armor), and **Chomp for Kids** (kid-friendly tilt game with a hungry furry monster gobbling up scattered treats), complete with RTTTL arcade sound effects and a one-tap mute button.

### 8-Tool Utility Suite (`Tools`)
- **2-Page $2 \times 2$ Utility Grid**: Hosts **3-Slot Countdown Timers** (synced with Home Assistant Assist voice timers), **3-Slot Multi-Alarm Manager**, **20 FPS 1970 Heuer Split-Lap Stopwatch** (with physical Crown pusher), **3D Dual D6 Dice Roll**, **AMOLED Flashlight Torch**, **Pocket Calculator**, **20 FPS `300px` Bubble Level**, and **3D Struck Silver Home Assistant Coin Flip**.

### System Hub & Docked Screensavers (`System`)
- **Unified On-Device Configuration**: 2-page category menu (`Display`, `Sleep & Wake`, `Sound`, `Voice`, `Music`, `App Deck`, `Info`, `Actions`) mirrored 1:1 with the Home Assistant device configuration page, plus 5 docked screensavers (**Aurora Borealis**, **Starfield**, **Matrix Rain**, **Fireworks**, and **Ambient Clock**).

---

## Hardware Resource & 3-Tier Power Budget

| Resource / Power Tier | Specification / Measured Budget | Engineering Implementation |
| :--- | :--- | :--- |
| **Quad-SPI Frame Render** | `~12–38 ms` @ 20 FPS (`42 ms` idle / `75 ms` audio) | 80 MHz QSPI bus (`CO5300`), `.rodata` `constexpr CIRCLE_HW[466]` chord clipping, and zero-sqrt scanline rings |
| **Internal SRAM & IRAM** | `~155 KB` free internal SRAM (`54.6%` used) | `nvs_cache_in_psram: true` (`+10 KB` SRAM), `flash_chip: gd` (`+5 KB` IRAM/Flash), and `.rodata` LUTs |
| **Octal PSRAM (8 MB @ 80 MHz)** | FLAC decode, NVS cache & FreeRTOS stacks | `decode_memory: psram`, `buffer_memory: psram`, `task_stack_in_psram: true`, and `nvs_cache_in_psram: true` |
| **Tier 1: Active Mode** | `110 – 160 mA` | 80 MHz QSPI display active, 30 Hz CST9220 touch polling, 6-axis QMI8658 IMU (`CTRL7 = 0x03`), and Wi-Fi `NONE` power save |
| **Tier 2: Display Standby** | `15 – 25 mA` (`<5 ms` wake) | Display off, PA amp off (`GPIO46`), QMI8658 gyro off (`CTRL7 = 0x01` pickup wake or `0x00` off), `GPIO3` PMU IRQ gating |
| **Tier 3: Hardware Hibernate** | `< 40 µA` (`~400 ms` UI wake) | Explicit `global_preferences->sync()` + clean API disconnect followed by AXP2101 `0x10 = 0x01` rail cut |

---

## Hardware Controls Reference

| Control | State / Context | Function |
| :--- | :--- | :--- |
| **Top Crown (Short Click)** | Alarm Ringing | Snoozes alarm for 5 minutes. |
| **Top Crown (Short Click)** | Timer / Find My Ringing | Dismisses timer alert or Find My Device beacon. |
| **Top Crown (Short Click)** | Standby (Screen Off) | Instant `<5 ms` wake to active brightness via `GPIO3` hardware interrupt. |
| **Top Crown (Short Click)** | Stopwatch (`Tools > Stopwatch`) | Physical tactile **Start / Stop** chronograph pusher with 200ms dwell compensation. |
| **Top Crown (Short Click)** | Submenus / Overlays / Games / Tools | Back / Exit to parent menu or launcher. |
| **Top Crown (Short Click)** | Main App Carousel Pages | Enters Tier 2 Display Standby immediately. |
| **Top Crown (Long Press >1.5s)** | Any Screen | Initiates Tier 3 Hardware Hibernate (instant or 5-second abortable countdown). |
| **Side Button (`GPIO0` Click)** | Normal / Standby / Games | Activates Voice Assistant (push-to-talk), dismisses active alerts, or fires in active games. |
| **Side Button (`GPIO0` Click)** | Voice Assistant Active | Cancels Voice Assistant immediately and mutes audio. |
| **Bottom Chin (`<` / `>`)** | Main App Carousel Pages | Flips through the 7-app carousel (`Clock` $\leftrightarrow$ `Music` $\leftrightarrow$ `Remote` $\leftrightarrow$ `Weather` $\leftrightarrow$ `Games` $\leftrightarrow$ `Tools` $\leftrightarrow$ `System`). |
| **Right Flank Chevrons (`▲` / `▼`)** | Paged Menus & Browsers | Scrolls vertically or paginates across Music Library, Speakers, Weather views, Games, Tools, and System Hub. |

---

## Repository Structure (27 Canonical Files)

```text
esphome-pocket-assistant/
├── .github/
│   └── workflows/
│       └── build.yml                      # ESPHome 2026.10.0 CI build & GitHub Pages release workflow
├── adrs/
│   ├── ADR-001-hardware-pmu-irq.md        # AXP2101 GPIO3 hardware interrupt architecture
│   ├── ADR-002-consolidated-companion-contract.md # 2-sensor Home Assistant companion contract
│   ├── ADR-003-zero-exception-sscanf-parser.md    # Non-throwing sscanf parsing under -fno-exceptions
│   ├── ADR-004-universal-tv-remote.md     # Multi-platform TV remote & LG webOS dispatch
│   ├── ADR-005-flank-chevron-navigation.md        # Right-flank chevron & chin back-button geometry
│   └── ADR-006-tools-suite-consolidation.md       # 7-app carousel & 8-tool suite consolidation
├── boards/
│   └── waveshare_175c.yaml                # Hardware Abstraction Layer (Display, Touch, Audio, IMU, PMU & PSRAM)
├── core/
│   ├── audio.yaml                         # Audio mixer, resamplers, Sendspin, Micro Wake Word & Voice Assistant
│   ├── power.yaml                         # 3-Tier power state machine, GPIO3 IRQ gate & battery fuel gauge
│   └── ui.yaml                            # 20 FPS display dispatcher, 7 apps, 8 tools & touch router
├── homeassistant/
│   └── packages/
│       └── pocket_assistant_companion.yaml    # All-in-one HA helpers, sensors & Music Assistant browse script
├── .gitignore                             # Git ignore rules
├── .nojekyll                              # GitHub Pages static asset flag
├── LICENSE                                # Apache 2.0 open-source license
├── README.md                              # Project documentation & setup guide
├── SPECIFICATION.md                       # Canonical hardware, pinout, memory & software specification
├── index.html                             # GitHub Pages Web Installer interface
├── manifest.json                          # Web Installer firmware manifest
├── pocket-assistant-1.75c.yaml            # Remote package entry point for ESPHome adoption
├── pocket-assistant.yaml                  # Master node configuration, substitutions & outgoing API
├── ui_core.h                              # Display geometry, .rodata LUTs, 3D math, blitter & 30 Hz touch
├── ui_ambient_va.h                        # 20 FPS Docked Screensavers & 5-Profile Voice Assistant engine
├── ui_weather_sky.h                       # Telescopic Lunar Albedo LUTs & 20 FPS Weather Motion suite
├── ui_games.h                             # 360-Deg IMU Tilt & 4-Game 20 FPS Arcade suite
├── ui_tools_apps.h                        # 8-Tool Suite, Clock, Music Player, TV Remote & System Hub
├── ui_toolkit.h                           # Umbrella C++20 UI toolkit header
└── va_cancel_helper.h                     # C++ Voice Assistant lifecycle, NVS wake-word sync & API disconnect
```

---

## Development & AI Transparency

AI tools were used during the development and documentation of this project.

- **Human-Directed & Hardware-Verified**: Every schematic, bus architecture, GPIO matrix route, and register map is validated directly on physical Waveshare ESP32-S3-Touch-AMOLED-1.75C hardware. No unverified code is committed.
- **How AI Was Utilized**: AI assistance was used for rapid prototyping, mathematical modeling (such as trigonometric LUTs, procedural 3D cloud & weather shaders, and 3D lunar raytracing in `ui_toolkit.h`), automated schema validation, and technical documentation.
- **Local-First & Open Standards**: All firmware architecture adheres strictly to native ESPHome 2026.10+ standards, local-first principles, and official Home Assistant and Music Assistant design patterns.

Feedback, peer review, and pull requests from the community are always welcome.

---

## License
Distributed under the Apache 2.0 License. See [`LICENSE`](LICENSE) for details.
