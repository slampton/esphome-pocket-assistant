# 🧭 Pocket Assistant

> **A pocket-sized smart companion for Home Assistant featuring native Voice Assistant, universal active album art, and a first-of-its-kind dynamic Music Assistant library browser & multi-room remote.**

[![Version](https://img.shields.io/badge/Version-v3.4.9-orange.svg)](https://github.com/slampton/esphome-pocket-assistant/releases)
[![ESPHome Version](https://img.shields.io/badge/ESPHome-2026.9.0%2B-blue.svg)](https://esphome.io)
[![Home Assistant](https://img.shields.io/badge/Home%20Assistant-Compatible-41BDF5.svg)](https://www.home-assistant.io)
[![Music Assistant](https://img.shields.io/badge/Music%20Assistant-2.0%2B-purple.svg)](https://music-assistant.io)
[![License](https://img.shields.io/badge/License-Apache%202.0-green.svg)](LICENSE)

Most ESPHome media controllers are passive displays that only reflect what an external speaker is already playing. **Pocket Assistant** transforms an ultra-compact circular AMOLED microcontroller into an interactive, local-first handheld console bridging Home Assistant, Music Assistant, and Voice Assistant.

---

## ✨ Key Features

* 🎵 **Native Music Assistant Library Browsing**: Browse Favorites, Playlists, Artists, Albums, Radio, Podcasts, and Audiobooks directly on-device with dual-action play (`▶`) and drill-down (`>`) touch cards across a 3-page category menu with bidirectional carousel wrap-around.
* 🖼️ **Universal Active Album Art**: Displays crisp, full-color 200×200 album art whether playing locally through the handheld speaker or handed off to any external household speaker (Sonos, AirPlay, Chromecast, DLNA, Marantz receivers).
* 🔊 **Smart Speaker Handoff & Takeover**: Transfer active playback queues between rooms with intelligent priority sorting: the active speaker floats to Slot 1 (highlighted green), the handheld device sits at Slot 2 for 1-tap return, and previously used speakers remain pinned at Slot 3.
* 🎙️ **Voice Assistant with Software Mixer Ducking**: Direct Assist satellite pipeline with dynamic 20 dB music ducking, kinetic AMOLED visual feedback, and instant push-to-talk/side-button cancellation.
* 🎚️ **Single-Authority Audio & Glitch-Free Volume**: Unified physical DAC control with priority-synchronized boot gain (no 100% startup blasts) and a configurable **Volume Step Size** entity (1%–10%, default 2%) to eliminate slider rubber-banding.
* ⏱️ **Vintage Chronograph & Lap Stopwatch**: Precision chronometer featuring an aged parchment Heuer-inspired dual-subdial dial, center sweep seconds, and crown button controls with hardware release-dwell latency compensation.
* 🎮 **Interactive Motion Games**: Real-time 20 FPS physics games (Marble Maze, Archery Target, Treat Catcher) powered by the onboard 6-axis IMU.
* 🔋 **Intelligent Multi-Tier Power Management**: Instant AMOLED screen standby (<50ms IMU pickup wake), physical pocket lock, hardware deep sleep hibernation with accidental-bump rejection, and USB dock stay-awake override.
* 🕒 **Modern Dial Clock Face**: High-contrast watch dial with Roman indices, polished polygon hands, 12-hour digital readout, and a sleek vertical battery level gauge with a golden charging lightning bolt.
* ⚙️ **Elevated System Telemetry**: High-legibility 22pt live telemetry readouts for battery percentage, voltage, charging state, and Wi-Fi signal strength with generous vertical breathing room.
* 🧩 **Modular Architecture**: Built on ESPHome's native `packages:` engine. Customize or reorder apps at runtime without touching core firmware.

---

## 💡 Design Goals & Architecture

Pocket Assistant was created to explore how far a modern ESP32-S3 microcontroller can be pushed when paired with Home Assistant and Music Assistant. Rather than building a dedicated appliance that only does one thing, the project emphasizes:

1. **Local-First Fluidity**: UI rendering runs directly on the ESP32-S3 via native C++ lambdas at 12–20 FPS, providing responsive, zero-cloud interaction.
2. **True Integration Depth**: Voice Assistant and Music Assistant share the same audio pipeline cleanly without resource locks, using dynamic hardware clock multiplexing and software mixing.
3. **Open & Reusable Building Blocks**: Features like the Music Assistant browsing engine are intentionally decoupled from device-specific firmware so they can benefit the wider ESPHome community.

---

## 🌐 Reusable Music Assistant Script Blueprint

A major contribution of this project is the **Music Assistant Browse Script Blueprint** ([`homeassistant/blueprints/script/music_assistant_browse.yaml`](homeassistant/blueprints/script/music_assistant_browse.yaml)).

### The Problem It Helps Solve
Embedding full Music Assistant API interactions inside microcontroller firmware is challenging. Parsing massive JSON responses for multi-thousand-item music libraries quickly exhausts microcontroller RAM, stalls the display loop, and causes audio stuttering.

### How It Works
The blueprint offloads all sorting, filtering, and pagination to Home Assistant:
1. The ESPHome device sends a tiny RPC request with the category (`artist`, `album`, `radio`, `podcast`, etc.), page number, and page size (e.g., 3 items per page).
2. The Home Assistant blueprint queries Music Assistant natively via the `music_assistant` integration actions.
3. The blueprint formats only the requested slice into lightweight primitive variables and calls the device's native `set_browse_slots` action.
4. The device immediately updates its screen buffer with zero client-side JSON parsing overhead.

### Using It in Other Projects
This blueprint is completely device-agnostic. While created for Pocket Assistant, it can power **any** ESPHome display device (e.g., e-ink dashboards, desktop stream decks, smart thermostats) simply by specifying the target device's name. Suggestions, improvements, and pull requests from the community are warmly welcomed!

---

## 🛠️ Hardware Platform

| Component | Specification | Details |
| :--- | :--- | :--- |
| **Microcontroller** | ESP32-S3R8 | Dual-core Xtensa LX7 @ 240MHz, 32MB Flash, 8MB Octal PSRAM |
| **Development Board** | Waveshare ESP32-S3-Touch-AMOLED-1.75C | Circular pocket watch form factor |
| **Display** | 1.75" Circular AMOLED (CO5300) | 466×466 resolution, 16.7M colors, MIPI Quad-SPI @ 40MHz |
| **Touchscreen** | CST9217 / CST9220 | Capacitive multi-touch, I2C `0x5A`, interrupt-driven |
| **Audio DAC / Amp** | Everest ES8311 | Low-power mono audio DAC, I2C `0x18`, integrated Class-D amp |
| **Audio ADC (Mic)** | Everest ES7210 | High-performance audio ADC, I2C `0x40`, analog MEMS mic |
| **Power Management** | X-Powers AXP2101 | Advanced PMU, I2C `0x34`, LiPo charging & fuel gauge |
| **Motion Sensing** | QST QMI8658 | 6-axis IMU (3-axis accelerometer + 3-axis gyroscope), I2C `0x6B` |

---

## ⚡ Hardware & Implementation Details

### 1. Dynamic GPIO Matrix I2S Clock Multiplexing
The Waveshare 1.75C hardware physically ties the I2S clock lines (BCLK GPIO9, WS GPIO45, MCLK GPIO16) between both the ES7210 ADC (microphone) and ES8311 DAC (speaker). Configuring standard duplex I2S causes clock contention and peripheral locking (`[i2s_audio.speaker.std:401]: Parent bus is busy`).

Pocket Assistant resolves this through **Dynamic GPIO Matrix Multiplexing**:
* Two independent I2S buses are declared (`i2s_input_bus` for mic, `i2s_output_bus` for speaker).
* On state changes, firmware dynamically reconnects the physical pins to the appropriate internal peripheral signals via Espressif ROM functions:
  ```cpp
  // Route clocks to ES8311 Speaker DAC
  esp_rom_gpio_connect_out_signal(GPIO_NUM_9, 28, false, false);   // I2S1 BCLK
  esp_rom_gpio_connect_out_signal(GPIO_NUM_45, 29, false, false);  // I2S1 WS
  esp_rom_gpio_connect_out_signal(GPIO_NUM_16, 34, false, false);  // I2S1 MCLK
  ```
This achieves 100% collision-free coexistence between local music playback and Voice Assistant capture.

### 2. Universal Active Album Art Across Speaker Transfers
Unlike displays that only show artwork from their own stream, Pocket Assistant features a **universal album art pipeline**:
* When playing locally, Sendspin renders artwork directly.
* When playback is transferred to an external speaker (e.g. Garage HiFi), the Home Assistant companion sensor (`sensor.pocket_assistant_target_art_url`) dynamically tracks the active speaker's `entity_picture` and routes it through Music Assistant's local image proxy (`http://homeassistant.local:8095/imageproxy?size=200&fmt=jpg`).
* ESPHome fetches and displays the full-color 200×200 artwork via `online_image`.
* When transferring back to local playback or when a track lacks artwork, `online_image.release` instantly frees memory, cleanly falling back to the high-contrast vinyl record disc.

### 3. Dynamic Roving Speaker Metadata Synchronization
When controlling remote household speakers, playback progress, title, artist, and state are mirrored through lightweight Home Assistant template sensors provided in the companion package. The active speaker is selected via `input_text.pocket_assistant_active_speaker` and dynamically sorted to Slot 1 in the handoff menu.

### 4. Single-Authority Audio & Software Mixer Ducking
To prevent volume slider fighting between Home Assistant and ESPHome:
* A single master media player (`pocket_music_player`) commands the ES8311 DAC directly.
* Voice Assistant routes TTS responses to `speaker: va_speaker` with automatic 20 dB dynamic music ducking.
* Hardware boot gain is locked at `priority: -100` to prevent 0 dB (100% blasting) volume spikes on startup.

---

## 🔬 Hardware & Performance Optimizations

* **Asymmetric Graphics Cadence**: Displays dynamically adjust rendering framerates based on the active screen:
  * Stopwatch: 125ms (8 FPS, matching an authentic 28,800 bph mechanical chronometer escapement).
  * Voice Assistant: 150ms / 350ms pulsing animations.
  * Motion Games: 50ms (20 FPS physics loop).
  * System / Clock: Event-driven or 1-second cadence.
* **Touch Priority Yielding**: Display rendering checks the CST9220 hardware interrupt pin (GPIO11). If a finger touches the screen during a frame push, the display update yields immediately, giving touch handling zero-latency priority.
* **Hardware RTC Calibration**: During boot, the internal RTC slow clock is calibrated against the 40MHz crystal via `rtc_clk_cal(0, 1024)` for drift-free offline timekeeping.
* **Multi-Tier Power Management**:
  * **Tier 1 (Interactive)**: Full 40MHz QSPI AMOLED rendering.
  * **Tier 2 (Display Standby)**: Screen sleeps after inactivity; wakes instantly (<50ms) upon pickup via QMI8658 motion sensing.
  * **Tier 3 (Deep Sleep Hibernation)**: Ultra-low-power hibernation entered via deliberate long-press (>1.0s) of the top crown button. Accidental pocket bumps (<200ms) are automatically rejected. Docking to USB power overrides sleep.

---

## 📱 Application Suite

### 🕒 Modern Dial Clock (`apps/clock.yaml`)
High-contrast monochrome watch dial featuring Roman numeral indices, polished polygon hands, 12-hour digital readout (`%I:%M %p`), and a vertical battery gauge with an 18px charging lightning bolt.

### ⏱️ Vintage Chronograph Stopwatch (`apps/stopwatch.yaml`)
1970s Heuer-inspired vintage chronometer on warm opaline parchment (`#D8CEB8`) featuring:
* 1/10-second high-speed spinning subdial (1 full revolution/sec).
* Arrowhead sweep seconds hand with spearhead and counterweight.
* Rattrapante split-second lap hand in rust red (`#A83828`).
* Large digital readout (Roboto 38pt) and split record.
* Leading-edge touch start/stop across the dial plus 200ms key-dwell latency compensation for the physical crown pusher.

### 🎵 Music Assistant Client & Multi-Room Remote (`apps/music.yaml`)
Complete handheld controller for Music Assistant:
* Now Playing display with vinyl disc or full-color 200×200 active album art.
* Radial elapsed track progress arc.
* 3-page library browser (Favorites, Playlists, Artists, Albums, Radio, Podcasts, Audiobooks) with bidirectional carousel wrap-around.
* Multi-room speaker handoff with active speaker priority sorting and 1-tap takeover.

### 🎮 Motion Physics Games (`apps/games.yaml`)
Interactive accelerometer-driven games running at 20 FPS:
* **Marble Maze**: Guide a steel ball through moving obstacles into the goal.
* **Archery Target**: Steady your aim against synthetic wind drift.
* **Treat Catcher**: Tilt to roll treats into the animated cat's mouth.

### ⚙️ System Dashboard & Diagnostics (`apps/system.yaml`)
Consolidated hardware status and controls:
* High-legibility 22pt telemetry for battery percentage, voltage, charging state, and Wi-Fi RSSI.
* One-touch display brightness preset cycler (40% to 100%).
* Configurable default boot app selector.
* Safe diagnostic device restart button.

---

## 🚀 Quick Start & Installation

### 1. Requirements
* Home Assistant with the **ESPHome** and **Music Assistant** integrations installed.
* Supported ESP32-S3 hardware (Waveshare ESP32-S3-Touch-AMOLED-1.75C).

### 2. Home Assistant Setup (Blueprint & Companion Package)

#### A. Install the Browse Script Blueprint
Pocket Assistant uses a native Home Assistant Script Blueprint to dynamically fetch and paginate library data directly from Music Assistant without hardcoding entity IDs:
1. Copy [`homeassistant/blueprints/script/music_assistant_browse.yaml`](homeassistant/blueprints/script/music_assistant_browse.yaml) to your Home Assistant configuration directory under:
   `/config/blueprints/script/esphome/music_assistant_browse.yaml`
   *(Or import it via **Settings** -> **Automations & Scenes** -> **Blueprints**).*
2. Click **Create Script** from the Blueprint:
   * **Target ESPHome Device Name**: Leave as default (`pocket-assistant`), or enter your custom node name.
   * **Active Speaker Helper**: Leave as default (`input_text.pocket_assistant_active_speaker`).
3. Save the script with Entity ID: `script.music_assistant_browse` (matching `${browse_script}` in your ESPHome substitutions).

#### B. Configure the Active Speaker Helper & Metadata Sensors
Pocket Assistant mirrors playback metadata (title, artist, album, duration, progress, artwork) from whichever household speaker is currently active:
* Copy [`homeassistant/packages/music_assistant_esphome_mirror.yaml`](homeassistant/packages/music_assistant_esphome_mirror.yaml) to your `/config/packages/` folder. This automatically creates `input_text.pocket_assistant_active_speaker` and the 7 mirror template sensors (`sensor.pocket_assistant_target_*`).

### 3. Deploy Firmware (One-Click Remote Git Package)
In your Home Assistant **ESPHome Device Builder** dashboard, create a new device or edit your configuration with this clean, minimal stub:

```yaml
substitutions:
  name: "pocket-assistant"
  friendly_name: "Pocket Assistant"
  version: "v3.4.9"

  # Local Media Player Identity
  local_player_id: "media_player.pocket_assistant"

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

| Gesture / Input | Scope | Action |
| :--- | :--- | :--- |
| **Left / Right Edge Swipe** | Main Apps | Cycle between active app deck (Clock <-> Stopwatch <-> Music <-> Games <-> System) |
| **Left / Right Edge Swipe** | Library Submenus | Paginates forward and back with bidirectional carousel wrap-around (Page 1 <-> Page 3) |
| **Top Crown Button (Short Press)** | Sleep / Any App | Wake display / Toggle between current app and Now Playing player screen |
| **Top Crown Button (Long Press >1s)** | Any App | Enter hardware deep sleep hibernation |
| **Side Button (Short Press)** | Any Screen | Activate Voice Assistant (Assist satellite listening) |
| **Side Button (Press while Active)** | Voice Assistant | Instantly cancel Assist and dismiss acoustic overlay |
| **Rotational Perimeter Swipe** | Music Player | Adjust volume up / down with left-side HUD feedback |
| **Center Screen Tap** | Music Player | Toggle Play / Pause |
| **Center Screen Tap** | Stopwatch | Instant Start / Stop (zero dead zones across central dial) |

---

## 📂 Repository Layout

```text
esphome-pocket-assistant/
├── pocket-assistant.yaml          # Master node configuration & substitutions
├── va_cancel_helper.h             # C++ side-button Assist cancellation hook
├── LICENSE                        # Apache 2.0 open-source license
├── README.md                      # Architecture guide & documentation
├── boards/
│   └── waveshare_175c.yaml        # Hardware Abstraction Layer (HAL) pinouts & buses
├── core/
│   ├── audio.yaml                 # Dual I2S GPIO matrix, DAC gain lock, Voice Assistant
│   ├── power.yaml                 # Multi-tier power management, IMU motion wake, deep sleep
│   └── ui.yaml                    # Display rendering pipeline, typography, color palettes
├── apps/
│   ├── clock.yaml                 # Modern dial watch face with vertical battery gauge
│   ├── stopwatch.yaml             # Vintage Heuer chronometer with dual subdials
│   ├── music.yaml                 # Music Assistant client, album art, roving remote
│   ├── games.yaml                 # 3 motion physics games (Marble, Archery, Treat Catcher)
│   └── system.yaml                # Elevated telemetry dashboard, brightness presets, restart
└── homeassistant/
    ├── blueprints/
    │   └── script/
    │       └── music_assistant_browse.yaml    # Script Blueprint for MA library browsing
    └── packages/
        └── music_assistant_esphome_mirror.yaml # Drop-in HA helper & sensor package
```

---

## 📜 Version History & Changelog

### v3.4.9 (Current)
* **Consolidated Inline Volume Controls & Left HUD Baseline**:
  * Consolidated the vertically stacked arrows and "VOL" text into single horizontal lines positioned symmetrically at $y = 46$ (top) and $y = 420$ (bottom), halfway between the previous carets and labels ($187\text{ px}$ from center, providing $35\text{ px}$ of radial clearance from the outer dial ring).
  * Rendered crisp gold filled triangles inline directly following the label (`VOL ▲` for Volume Up, `VOL ▼` for Volume Down).
  * Standardized the volume percentage HUD to permanently dock on the left side on the exact same horizontal baseline ($y = 46$ / $y = 420$), cleanly eliminating the deprecated `vol_display_position` configuration entity while opening vertical screen buffer space.
* **Remote Album Art Continuous Refresh & Speed Optimization**:
  * Upgraded `homeassistant/packages/music_assistant_esphome_mirror.yaml` to trigger-based template sensors with periodic `/2s` evaluation and state triggers, resolving the issue where Home Assistant failed to track dynamic `entity_picture` updates when playing new tracks on remote speakers (e.g., Garage HiFi).
  * Implemented instant frame buffer flushing (`id(remote_album_art).release();`) upon track selection (`play_music_slot_1/2/3`) and track skipping (`music_next_track`, `music_prev_track`), eliminating stale artwork lingering while new artwork loads.
  * Added automated `homeassistant.update_entity` calls and track title listeners to immediately trigger remote image downloads upon speaker handoffs and library playback.
* **Display Brightness Control & Persistence Restoration**:
  * Restored the missing `set_action:` percentage handler to `display_brightness_preset`, restoring instant brightness cycling via the Page 4 on-screen button, the physical top crown pusher, and the Home Assistant entity.
  * Added boot initialization at `priority: -100` to re-assert the user's NVS-restored brightness preset after all hardware drivers initialize, preventing default resets on reboot.
  * Removed the hardcoded 90% override in `enter_deep_sleep`, ensuring the user's custom brightness setting is preserved through sleep cycles.
* **Incomplete Page Out-of-Bounds Glitch Resolution**: Hardened bounds checks in `music_assistant_browse.yaml` (`length > 1` for Slot 2, `length > 2` for Slot 3), eliminating `UndefinedError` crashes on partial pages (such as Artist page 21 with 2 items, Radio with 2 stations, or Audiobooks with 1 book).
* **Native Library Categories & 3-Page Submenu**:
  * Renamed "RADIO STATIONS" to "RADIO" to align with native Music Assistant conventions.
  * Separated "PODCASTS & BOOKS" into two distinct native categories: "PODCASTS" (`media_type: podcast`) and "AUDIOBOOKS" (`media_type: audiobook`).
  * Expanded Level 2 library menu to 3 clean pages (Page 1: Favorites/Playlists/Artists, Page 2: Albums/Tracks/Radio, Page 3: Podcasts/Audiobooks) with bidirectional carousel wrap-around (Page 1 <-> Page 3).
* **Clock Face Vertical Battery Gauge**: Replaced the horizontal battery icon on Page 0 with an 18px vertical battery gauge featuring a top terminal pip and dynamic bottom-up charge level fill.
* **System Telemetry Typography & Baseline Elevation**: Increased live data font size for battery, status, and Wi-Fi signal to 22pt (`font_digital_time`) and shifted vertical baseline coordinates upward ($y = 120, 153, 186$) for comfortable clearance above control buttons.
* **Dynamic Active Speaker Name Sync**: Upgraded `ha_active_speaker_sync` in `core/ui.yaml` to dynamically parse and format the friendly name for any household speaker entity on boot without hardcoded substitution tables.

### v3.4.8
* **Home Assistant Script Blueprint Architecture**: Decoupled the music browsing engine into a reusable Home Assistant Script Blueprint (`homeassistant/blueprints/script/music_assistant_browse.yaml`), allowing any ESPHome device to generate its own browsing script with automatic RPC target resolution.
* **Purge of Vestigial Substitutions**: Completely removed legacy presets (`preset_1_*`, `preset_2_*`, `preset_3_*`) and hardcoded speaker slots (`speaker_2_*`, `speaker_3_*`) from `pocket-assistant.yaml` and `core/ui.yaml`. Replaced with single `local_player_id: "media_player.pocket_assistant"`.
* **Universal Active Album Art Pipeline**: Integrated dual local/remote album art rendering. Local playback streams via Sendspin, while remote speaker handoffs leverage Music Assistant's native local HTTP image proxy on port 8095 (`?size=200&fmt=jpg`) paired with ESPHome's native `online_image` and `http_request` components.
* **Smart-Sorted Dynamic Speaker Handoff**: Script Blueprint intelligently sorts the currently active speaker to Slot 1 (highlighted green), followed by Pocket Assistant at Slot 2 for 1-tap return, and all remaining household speakers alphabetically.
* **Audio Output Hardware Fix**: Resolved silent playback on waking from sleep by adding explicit `switch.turn_on: speaker_enable` in `pocket_music_player.on_play` and asserting `ALWAYS_ON` restore mode.
* **Display Brightness Preset Persistence**: Restored `set_action:` to `display_brightness_preset`, ensuring on-screen and Home Assistant brightness adjustments persist cleanly across reboots.

### v3.4.7
* **Standardized Bottom Navigation Hierarchy**: Moved `< MENU` to the bottom across all library browsing overlays, eliminating redundant top menu buttons and obsolete on-screen `[ CLOSE ]` buttons (since the physical crown button exits to player).
* **Dynamic Breadcrumb Navigation**: Bottom pill cleanly handles tier-by-tier navigation (`< MENU` returns to parent menu; `< BACK` steps up drilldowns like Tracks → Albums → Artists).
* **Reclaimed Screen Header & Centered Titles**: Lowered library and category titles from cramped $y = 48$ down to standardized $y = 80$ (matching $y = 82$ across Clock, Stopwatch, Music, Games, and System).

### v3.4.6
* **Expanded System Page Card Spacing**: Increased card pitch on the System dashboard from 60px to 75px ($y = 245, 320, 395$), providing generous 31px vertical separation.
* **Harmonized Music Menu Layout**: Aligned the Music Menu title to $y = 82$ and positioned the top cards at $y = 155, 235, 315$.

### v3.4.5
* **Artifact-Free Pill Button Geometry**: Overhauled button rendering across Music, Games, and System menus using concentric filled shapes.
* **Vertically Centered Pill Typography**: Corrected text vertical alignment across all menus, perfectly centering labels on the pill centerline.
* **Synchronized Title Alignment**: Aligned Games page title to `y = 82` to match Music and System.

### v3.4.0 – v3.4.4 Highlights
* **Ergonomic Volume HUD**: Left-side volume readout placement prevents right-handed thumbs from obstructing text during adjustment.
* **Single Master Media Player Authority**: Direct ES8311 physical DAC control with 1000ms I2S buffer to eliminate audio stuttering.
* **Push-to-Talk & Instant Assist Cancellation**: C++ helper (`va_cancel_helper.h`) for immediate Voice Assistant dismissal.
* **Vintage Chronograph Watchface**: Mechanical pocket watch chronometer dial with 1/10s spinner, 60-minute accumulator, and release-dwell compensation.
* **Multi-Tier Power Management**: Tier 1 interactive, Tier 2 <50ms IMU pickup standby, Tier 3 deep sleep hibernation with accidental-bump rejection.

### v3.3.0
* Initial release of modular packages architecture (`core/`, `apps/`, `boards/`).
* Music Assistant hierarchical browser with dual-action play/drill cards.
* Multi-room speaker handoff and takeover queue switching.
* Dynamic I2S clock line GPIO matrix multiplexing.

---

## 📜 License
Distributed under the Apache 2.0 License. See [`LICENSE`](LICENSE) for details.
