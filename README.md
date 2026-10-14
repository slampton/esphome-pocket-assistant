# Pocket Assistant

A local-first handheld companion, voice satellite, and universal media remote for Home Assistant and Music Assistant, built on the Waveshare 1.75" circular AMOLED ESP32-S3 development board.

[![Build & Release Firmware](https://github.com/slampton/esphome-pocket-assistant/actions/workflows/build.yml/badge.svg)](https://github.com/slampton/esphome-pocket-assistant/actions/workflows/build.yml)
[![Version](https://img.shields.io/badge/Version-v1.3-blue.svg)](https://github.com/slampton/esphome-pocket-assistant/releases)
[![ESPHome Version](https://img.shields.io/badge/ESPHome-2026.10.0%2B-blue.svg)](https://esphome.io)
[![Home Assistant](https://img.shields.io/badge/Home%20Assistant-Compatible-41BDF5.svg)](https://www.home-assistant.io)
[![Music Assistant](https://img.shields.io/badge/Music%20Assistant-2.10%2B-purple.svg)](https://music-assistant.io)
[![License](https://img.shields.io/badge/License-Apache%202.0-green.svg)](LICENSE)

Pocket Assistant started as a personal home lab project to explore what is possible when pairing modern ESP32-S3 hardware with Home Assistant, Music Assistant, and ESPHome. Rather than acting as a passive sensor display or a stationary voice satellite alone, it combines local FLAC/Opus audio playback, multi-room queue transfer and speaker group management, interactive 5-slot library browsing, universal TV remote control, push-to-talk Voice Assistant support, and a 3-tier power state machine (`<40 µA` hardware hibernate) in a 466×466 circular handheld form factor.

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

### Supported Hardware
- **Board**: Waveshare ESP32-S3-Touch-AMOLED-1.75C (1.75" 466×466 round AMOLED, CO5300 80 MHz QSPI display, CST9220 capacitive touch, ES8311 DAC, ES7210 ADC, AXP2101 PMIC, QMI8658 6-axis IMU, 32MB GigaDevice Octal Flash, 8MB Octal PSRAM).

### 1. Flash the Firmware
- **Web Installer (Recommended)**: Connect the device via USB-C in a WebSerial-supported browser (Chrome, Edge, Opera) and visit the **[Pocket Assistant Web Installer](https://slampton.github.io/esphome-pocket-assistant/)** to install v1.3 with one click and configure Wi-Fi via Improv BLE.
- **ESPHome Device Builder**: Alternatively, adopt the device using a minimal remote package include in your ESPHome node configuration:
```yaml
substitutions:
  name: "pocket-assistant"
  friendly_name: "Pocket Assistant"
  weather_entity: "weather.home"                     # Primary Home Assistant weather entity
  tv_remote_entity: "remote.tv"                      # Target TV remote.* or LG webOS media_player.* entity

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

### 2. Home Assistant Companion Package Setup
Pocket Assistant uses a single consolidated companion package (`homeassistant/packages/pocket_assistant_companion.yaml`) containing 3 persistent state helpers (`input_text`) and 2 high-efficiency template sensors (`sensor.pocket_assistant_active_media` and `sensor.pocket_assistant_weather_forecast`).

Choose whichever deployment method matches your Home Assistant configuration:

- **Option A: `packages/` Directory (Recommended)**:
  1. Ensure your Home Assistant `configuration.yaml` enables packages:
     ```yaml
     homeassistant:
       packages: !include_dir_named packages
     ```
  2. Copy `homeassistant/packages/pocket_assistant_companion.yaml` directly from this repository into your Home Assistant `/config/packages/` directory.

- **Option B: Split `templates.yaml` & Helpers**:
  If your Home Assistant environment organizes template sensors via `template: !include templates.yaml`:
  1. Copy the contents of the `template:` block from `pocket_assistant_companion.yaml` directly into your `/config/templates.yaml`.
  2. Create the 3 required `input_text` helpers either by copying the `input_text:` block into `/config/configuration.yaml` or creating them under **Settings -> Devices & Services -> Helpers**:
     * `input_text.pocket_assistant_active_speaker` (Active target media player entity ID, e.g. `media_player.living_room`)
     * `input_text.pocket_assistant_weather_entity` (Monitored weather entity ID, default `weather.home`)
     * `input_text.pocket_assistant_base_url` (Optional base URL override, e.g. `http://homeassistant.local:8123`)

After adding the configuration, reload Template Entities and Input Texts under **Developer Tools -> YAML** (or restart Home Assistant).

### 3. Music Assistant Setup
1. Adopt the discovered `pocket-assistant` device under **Settings -> Devices & Services**.
2. **Library Browsing Blueprint**: Import `homeassistant/blueprints/script/music_assistant_browse.yaml` into Home Assistant (**Settings -> Automations & Scenes -> Blueprints**), create a script from it, and save it as `script.music_assistant_browse`.

---

## Core Capabilities

### Audio Pipeline & Music Assistant (`Page 2`)
- **Local & Remote Audio Control**: Functions as a standalone local media player via Sendspin FLAC streaming, or as a handheld remote managing external household speakers (Sonos, AirPlay, Chromecast, receivers).
- **Multi-Room Queue Transfer & Group Management**: Direct 5-slot speaker browser with contextual action sheets: transfer active playback queues between rooms (`TRANSFER QUEUE`), control remote speaker volume and transport (`CONTROL ONLY`), join speakers into synchronized audio groups (`JOIN AUDIO`), or cleanly detach/unjoin groups (`UNJOIN AUDIO`).
- **Universal Active Album Art**: Displays full-color artwork across three distinct presentation modes: **Full Screen** (466×466 circular bleed), **Letterbox** (wide aspect), and **Standard** (uncropped 328×328 inscribed square with AMOLED black crescents for title and metadata), with automatic fallback to a high-contrast vinyl disc when no art is available.
- **Clean Art-First Floating HUD & Speech Transport**: Artwork breathes with zero on-screen clutter during playback; tapping the glass summons floating vector transport controls with 1px drop shadows, `font_small` top speaker/status header, and radial elapsed progress before auto-fading after 5 seconds. Automatically switches between track skip (`|◀` / `▶|`) for music and circular seek icons (`-10s` replay / `+30s` skip) for podcasts and audiobooks.
- **5-Slot Library Browser & Configurable Sorting**: Browse Music Assistant favorites, playlists, artists, albums, radio, podcasts (with series-to-episode drill-down via `mass_queue.get_podcast_episodes`), and audiobooks on-device with dual-action play (`▶`) and drill-down (`>`) cards, right-flank chevron pagination, and configurable sort orders (Alphabetical, Newest on Top, Oldest on Top).

### Voice Assistant Satellite
- **Push-to-Talk Assist**: Side button (`GPIO0`) routes audio to the ES7210 microphone, pauses active music, and activates the Home Assistant Assist satellite pipeline with two selectable visual profiles (**Nebula** and **Solaris**).
- **Dynamic Audio Ducking**: Software mixer smoothly ducks local playback during Voice Assistant speech synthesis and automatically resumes paused playback once TTS finishes draining.
- **Hardware & Touch Dismissal**: Pressing the physical side button or tapping the glass during an active voice session immediately halts and mutes the session without leaking taps to the underlying app.

### Universal TV & Media Remote Control (`Page 7`)
- **Table-Driven Multi-Platform Dispatch (`remote_send_key`)**: Parameterized table-driven command engine supporting **6 native TV platforms** out of the box with zero YAML edits required to switch profiles:
  1. **Android TV / Google TV / Shield TV / Fire TV / Bravia** (`remote.send_command`, `DPAD_*`)
  2. **Apple TV** (`remote.send_command`, `up`, `down`, `menu`, `top_menu`, `wakeup`)
  3. **Roku** (`remote.send_command`, `up`, `down`, `back`, `home`, `power`)
  4. **Samsung Smart TV** (`remote.send_command`, `KEY_UP`, `KEY_RETURN`, `KEY_HOME`, `KEY_POWER`)
  5. **LG webOS Smart TV** (`webostv.button` targeting `media_player.*` directly—no `remote.*` entity required)
  6. **Custom / Generic Remote** (User-configurable `${remote_cmd_*}` substitutions)
- **Automatic Heuristic Detection**: Automatically detects the target platform on first boot from the entity ID or friendly name (`apple`, `roku`, `samsung`, `webos`, `lg`) while respecting manual selections persisted in NVS flash.

### 7-App Carousel & 8-Tool Suite Architecture
- **7 Top-Level Carousel Apps**: Navigated via full-chin bottom chevrons (`y = 424`):
  * **Clock (`Page 0`)**: Modern Roman Analog and Modern Digital Bold watch faces with date and battery complications.
  * **Music Player (`Page 2`)**: Full-bleed album art, floating HUD, multi-room speaker manager, and 5-slot Music Assistant library browser.
  * **TV Remote (`Page 7`)**: Concentric 5-way D-pad with 85px equatorial volume dead-zone and device settings subpage.
  * **Sky & Weather (`Page 5`)**: 4-view meteorological & astronomical suite (Current Conditions with live particle rain/snow/starfield, 5-Day Forecast, Solar Arc, and 3D Raytraced Lunar Phase).
  * **Games Suite (`Page 3`)**: Slot-machine reel launcher featuring 5 arcade and 6-axis IMU motion games (**Breakout**, **Marble Maze**, **Archery Target**, **Treat Catcher**, and **Asteroids**).
  * **Tools Suite (`Page 8`)**: 2-page $2 \times 3$ utility grid with right-flank chevrons hosting **8 micro-apps**: **Countdown Timer**, **Multi-Alarm Manager**, **1970 Heuer Split-Lap Stopwatch**, **Dual D6 Dice Roll**, **AMOLED Flashlight Torch**, **Pocket Calculator**, **2D IMU Bubble Level**, and **Home Assistant Coin Flip**.
  * **System Hub (`Page 4`)**: 2-page vertical settings menu (4 full-width pills per page with right-flank chevrons): Page 1 (`Display >`, `Clock >`, `Sleep & Wake >`, `Audio >`) and Page 2 (`App Deck >`, `Voice >`, `Info >`, `Actions >`).

---

## Hardware Resource & 3-Tier Power Budget

| Resource / Power Tier | Specification / Measured Budget | Engineering Implementation |
| :--- | :--- | :--- |
| **Quad-SPI Frame Render** | `~12–42 ms` (`<45 ms` budget @ 10 FPS) | 80 MHz QSPI bus (`CO5300`), RLE cloud spans (`cloud_spans.h`), and `.rodata` `constexpr CIRCLE_HW[466]` chord clipping (`ui_toolkit.h`) |
| **Internal SRAM & IRAM** | `+15.9 KB` reclaimed in v1.3 | `nvs_cache_in_psram: true` (`+10 KB` SRAM), `flash_chip: gd` (`+5 KB` IRAM/Flash), and `.rodata` LUT migration (`+932 B` SRAM) |
| **Octal PSRAM (8 MB @ 80 MHz)** | FLAC decode, NVS cache & FreeRTOS stacks | `decode_memory: psram`, `buffer_memory: psram`, `task_stack_in_psram: true`, and `nvs_cache_in_psram: true` |
| **Tier 1: Active Mode** | `110 – 160 mA` | 80 MHz QSPI display active, 30 Hz CST9220 touch polling, 6-axis QMI8658 IMU (`CTRL7 = 0x03`), and Wi-Fi `NONE` power save |
| **Tier 2: Display Standby** | `15 – 25 mA` (`<5 ms` wake) | Display off, PA amp off (`GPIO46`), QMI8658 gyro powered down (`CTRL7 = 0x01` accel-only pickup wake), Wi-Fi modem sleep, `GPIO3` PMU IRQ armed |
| **Tier 3: Hardware Hibernate** | `< 40 µA` (`~400 ms` UI wake) | Explicit `global_preferences->sync()` followed by AXP2101 `0x10 = 0x01` rail cut; instant reconnect via `api: outgoing_connection:` dial-back learning |

---

## Hardware Controls Reference

| Control | State / Context | Function |
| :--- | :--- | :--- |
| **Top Crown (Short Click)** | Alarm Ringing | Snoozes alarm for 5 minutes. |
| **Top Crown (Short Click)** | Timer Ringing | Dismisses timer alert. |
| **Top Crown (Short Click)** | Standby (Screen Off) | Instant `<5 ms` wake to active brightness via `GPIO3` hardware interrupt. |
| **Top Crown (Short Click)** | Stopwatch (`Tools > Subpage 3`) | Physical tactile **Start / Stop** chronograph pusher with 200ms dwell compensation. |
| **Top Crown (Short Click)** | Submenus / Overlays / Games / Tools | Back / Exit to parent menu or launcher (`stuff_subpage = 0` for Timer and other tools). |
| **Top Crown (Short Click)** | Main App Carousel Pages | Enters Tier 2 Display Standby immediately. |
| **Top Crown (Long Press >1.5s)** | Any Screen | Starts 5-second abortable Tier 3 Hibernation Countdown (or wakes from Hibernate). |
| **Side Button (`GPIO0` Click)** | Normal / Standby / Games | Activates Voice Assistant (push-to-talk listening mode), or fires weapon in Asteroids. |
| **Side Button (`GPIO0` Click)** | Voice Assistant Active | Cancels Voice Assistant immediately and mutes audio. |
| **Bottom Chin (`<` / `>`)** | Main App Carousel Pages | Navigates through the 7-app carousel (`Clock` $\leftrightarrow$ `Music` $\leftrightarrow$ `Remote` $\leftrightarrow$ `Weather` $\leftrightarrow$ `Games` $\leftrightarrow$ `Tools` $\leftrightarrow$ `System`). |
| **Right Flank Chevrons (`▲` / `▼`)** | Paged Menus & Browsers | Scrolls vertically or paginates across Music Library, Speakers, Weather views, Tools grid, and System Hub. |

---

## Repository Structure (25 Canonical Files)

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
│   └── waveshare_175c.yaml                # Hardware Abstraction Layer (HAL), PSRAM NVS cache & flash_chip: gd
├── core/
│   ├── audio.yaml                         # Dual I2S GPIO matrix, ES8311/ES7210, Sendspin & Voice Assistant
│   ├── power.yaml                         # 3-Tier power state machine, AXP2101 telemetry & NVS pre-hibernate sync
│   └── ui.yaml                            # 10 FPS display dispatcher, 7 apps, 8 tools & parameterized scripts
├── homeassistant/
│   ├── blueprints/
│   │   └── script/
│   │       └── music_assistant_browse.yaml    # Server-side pagination blueprint for Music Assistant
│   └── packages/
│       └── pocket_assistant_companion.yaml    # Consolidated HA helpers & idle-guarded template sensors
├── .gitignore                             # Git ignore rules
├── .nojekyll                              # GitHub Pages static asset flag
├── LICENSE                                # Apache 2.0 open-source license
├── README.md                              # Project documentation & setup guide
├── SPECIFICATION.md                       # Canonical hardware, pinout, memory & software specification
├── cloud_spans.h                          # Pre-rasterized RLE cumulus cloud span tables (.rodata)
├── index.html                             # GitHub Pages Web Installer interface
├── manifest.json                          # Web Installer firmware manifest
├── pocket-assistant-1.75c.yaml            # Remote package entry point for ESPHome adoption
├── pocket-assistant.yaml                  # Master node configuration, substitutions & outgoing API
├── ui_toolkit.h                           # Reusable C++ UI primitives, LUTs, raytracers & touch toolkit
└── va_cancel_helper.h                     # C++ Voice Assistant cancellation & socket drain helper
```

---

## Development & AI Transparency

AI tools were used during the development and documentation of this project.

- **Human-Directed & Hardware-Verified**: Every schematic, bus architecture, GPIO matrix route, and register map is validated directly on physical Waveshare ESP32-S3-Touch-AMOLED-1.75C hardware. No unverified code is committed.
- **How AI Was Utilized**: AI assistance was used for rapid prototyping, mathematical modeling (such as trigonometric LUTs, RLE cloud span compression, and 3D lunar raytracing in `ui_toolkit.h`), automated schema validation, and technical documentation.
- **Local-First & Open Standards**: All firmware architecture adheres strictly to native ESPHome 2026.10+ standards, local-first principles, and official Home Assistant and Music Assistant design patterns.

Feedback, peer review, and pull requests from the community are always welcome.

---

## License
Distributed under the Apache 2.0 License. See [`LICENSE`](LICENSE) for details.
