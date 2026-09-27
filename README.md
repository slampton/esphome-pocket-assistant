# 🧭 Pocket Assistant

> **A pocket-sized smart companion for Home Assistant with native Voice Assistant and a first-of-its-kind Music Assistant library browser & controller.**

[![Version](https://img.shields.io/badge/Version-v3.4.8-orange.svg)](https://github.com/slampton/esphome-pocket-assistant/releases)
[![ESPHome Version](https://img.shields.io/badge/ESPHome-2026.9.0%2B-blue.svg)](https://esphome.io)
[![Home Assistant](https://img.shields.io/badge/Home%20Assistant-Compatible-41BDF5.svg)](https://www.home-assistant.io)
[![License](https://img.shields.io/badge/License-Apache%202.0-green.svg)](LICENSE)

Most ESPHome media displays are passive screens that only reflect what an external player is already playing. **Pocket Assistant** transforms a handheld microcontroller into an interactive, local-first smart terminal bridging Home Assistant, Music Assistant, and Voice Assistant.

---

## ✨ Key Features

* 🎵 **Hierarchical Music Library Browsing**: Browse Artists, Albums, Tracks, Playlists, Radio, Podcasts, and Audiobooks directly on-device with dual-action play (`▶`) and drill-down (`>`) touch cards.
* 🔊 **Smart Speaker Handoff**: Transfer active queues between household speakers (e.g., Living Room, Office, or Whole-House audio). The active speaker always floats to the top, followed by the handheld device for instant return.
* 🎙️ **Voice Assistant with Software Mixer Ducking**: Direct Assist satellite pipeline with dynamic 20 dB music ducking, kinetic AMOLED visual feedback, and instant push-to-talk/side-button cancellation.
* 🎚️ **Single-Authority Audio & Glitch-Free Volume**: Unified physical DAC control with priority-synchronized boot gain (no 100% startup blasts) and a configurable **Volume Step Size** entity (1%–10%) to eliminate slider rubber-banding.
* ⏱️ **Vintage Chronograph & Lap Stopwatch**: Precision chronometer featuring a classic dual-subdial dial, center sweep seconds, and crown button controls with hardware release-dwell latency compensation.
* 🎮 **Interactive Motion Games**: Real-time 20 FPS physics games (Marble Maze, Archery Target, Treat Catcher) powered by the onboard 6-axis IMU.
* 🔋 **Intelligent Multi-Tier Power Management**: Instant AMOLED screen standby (<50ms IMU pickup wake), physical pocket lock, hardware deep sleep hibernation with accidental-bump rejection, and USB dock stay-awake override.
* 🧩 **Modular Architecture**: Built on ESPHome's native `packages:` engine. Customize or reorder apps at runtime without touching core firmware.

---

## 🛠️ Hardware Platform

| Hardware | Display | Audio | IMU | PMU | Status |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Waveshare ESP32-S3-Touch-AMOLED-1.75C** | 1.75\" Circular AMOLED (466×466, CO5300) | Dual I2S Master (ES8311 DAC + ES7210 Mic) | QMI8658 | AXP2101 | **Verified** |

---

## ⚡ Technical Highlights & Architectural Solutions

Building an interactive, pocket-sized smart device with voice assistance, smooth graphics, and streaming media on a single ESP32-S3 required solving several microcontroller hurdles:

### 1. Dynamic GPIO Matrix I2S Clock Multiplexing
* **The Challenge**: The board routes both the ES7210 microphone ADC (input) and ES8311 speaker DAC (output) through shared clock lines: **GPIO9 (BCLK)**, **GPIO45 (WS/LRCLK)**, and **GPIO16 (MCLK)**. Standard configurations risk bus collisions, clock jitter, and driver lockup.
* **The Solution**: Pocket Assistant uses **runtime GPIO Matrix multiplexing** via Espressif ROM routing (`esp_rom_gpio_connect_out_signal`). When Voice Assistant listens, clock lines route to the I2S0 peripheral; when media or TTS plays, they instantly route to I2S1. This enables independent master clocking without dedicated external multiplexer hardware.

### 2. Native Dynamic Music Assistant Browsing
* **The Challenge**: Full library browsing on microcontrollers is typically bottlenecked by RAM limits and heavy JSON parsing.
* **The Solution**: Rather than parsing large API payloads on-chip, Pocket Assistant pairs with a lightweight **Home Assistant Script Blueprint**. Home Assistant handles pagination and filtering, dispatching compact, 3-slot RPC payloads (`set_browse_slots`) directly into ESPHome display memory with near-zero latency and minimal PSRAM overhead.

### 3. Single-Authority Audio & Software Mixer Ducking
* **Single Master Player**: A single master media player entity controls the ES8311 DAC gain directly in hardware (50µs), eliminating redundant network echo loops and volume slider rubber-banding.
* **Dynamic 20 dB Voice Ducking**: Voice Assistant streams into a FreeRTOS software `mixing_speaker`, automatically ducking active background music by 20 dB while listening or speaking, then recovering smoothly over 1.0 second.
* **Boot Volume Normalization**: Hardware DAC initialization is anchored at `priority: -100` (after hardware setup), guaranteeing a calibrated 70% level before any audio stream starts and eliminating power-on 0 dB volume spikes.
* **Instant Assist Dismissal**: A lightweight C++ helper (`va_cancel_helper.h`) binds to the side button (GPIO0) to instantly abort listening, thinking, or speech and restore the previous screen.

---

## 🔬 Hardware & Performance Optimizations

* **Asymmetric Graphics Cadence**: Dynamically throttles rendering cadence (150ms during voice listening, 80ms during stopwatch timing, 350ms during speech playback) to balance CPU load and eliminate I2S audio FIFO starvation.
* **Hardware RTC Calibration**: Measures and calibrates the internal RTC slow clock against the 40MHz crystal on boot (`rtc_clk_cal`) over 1024 cycles, providing sub-millisecond offline timekeeping across sleep cycles.
* **Multi-Tier Power Management**:
  * **Tier 1 (Interactive AMOLED)**: Smooth 466x466 circular UI with calibrated brightness curves (40%–100%).
  * **Tier 2 (Display Standby)**: AMOLED shuts off to true black (0 mA) after 30 seconds of inactivity or a top-crown tap; wakes in <50ms upon physical pickup via the QMI8658 6-axis IMU.
  * **Tier 3 (Deep Sleep Hibernation)**: Long-pressing the side button (>1.0s) puts the device into microamp hibernation for extended shelf standby.
  * **Pocket-Bump Rejection**: Waking from deep sleep requires holding the crown button for >200ms upon release, filtering out accidental transient pocket bumps.
  * **USB Dock Override**: Automatically detects USB power via AXP2101 registers, keeping the screen active as a glanceable desktop clock while docked.

---

## 📱 Application Suite

Pocket Assistant organizes functionality into focused, modular applications switchable via edge swipes:

### 🕒 Modern Dial Clock (`apps/clock.yaml`)
* High-contrast dial with Roman numeral indices, custom polygon analog hands, and cyan center second sweep.
* Integrated digital time, date, battery percentage, charging lightning bolt, and Home Assistant connection indicator.

### ⏱️ Vintage Chronograph Stopwatch (`apps/stopwatch.yaml`)
* Vintage dial modeled after classic mechanical chronometers with bold 5-second interval markers.
* Dual subdials: high-speed 1/10th-second spinner at 12 o'clock and 60-minute elapsed accumulator at 6 o'clock.
* Start and stop timing via the physical top crown button (with 200ms release-dwell compensation) or full-face center touch. Dedicated touch pushers for **LAP** and **RESET**.

### 🎵 Music Assistant Client & Multi-Room Remote (`apps/music.yaml`)
* **100% Dynamic Database Polling**: Directly queries your live Music Assistant library without hardcoded presets or speaker lists.
* **3-Page Library Carousel**: Dedicated browser for Favorites, Playlists, Artists, Albums, Tracks, Radio, Podcasts, and Audiobooks.
* **Smart Speaker Handoff**: The currently active speaker automatically floats to the top (highlighted in green), followed by the handheld device and remaining household speakers alphabetically.
* **Dual-Action Touch Cards**: Play immediately (`▶`) or drill down (`>`) into artists, albums, and tracks. Full player controls with volume HUD carets and track progress bar.

### 🎮 Motion Physics Games (`apps/games.yaml`)
* Real-time 20 FPS physics simulation powered by the onboard QMI8658 6-axis accelerometer:
  * **Marble Maze**: Tilt-guided labyrinth navigation.
  * **Archery Target**: Steady-hand gyroscope aiming.
  * **Treat Catcher**: Motion-controlled paddle game.

### ⚙️ System Dashboard & Diagnostics (`apps/system.yaml`)
* Telemetry readout: Battery percentage, millivolt voltage, charging state, Wi-Fi SSID, IP address, and RSSI signal strength.
* Clean action buttons to cycle brightness presets (40% -> 100%), change the default wake app, or perform a safe software restart.

---

## 🚀 Quick Start & Installation

### 1. Requirements
* Home Assistant with the **ESPHome** and **Music Assistant** integrations installed.
* Supported ESP32-S3 hardware (Waveshare ESP32-S3-Touch-AMOLED-1.75C).

### 2. Home Assistant Setup (Blueprint & Helpers)

#### A. Install the Browse Script Blueprint
Pocket Assistant uses a native Home Assistant Script Blueprint to fetch and paginate library data without hardcoded entity IDs:
1. Copy [`homeassistant/blueprints/script/music_assistant_browse.yaml`](homeassistant/blueprints/script/music_assistant_browse.yaml) to your Home Assistant configuration directory under:
   `/config/blueprints/script/esphome/music_assistant_browse.yaml`
   *(Or import it via **Settings** -> **Automations & Scenes** -> **Blueprints**).*
2. Click **Create Script** from the Blueprint:
   * **Target ESPHome Device Name**: Leave as default (`pocket-assistant`), or enter your custom node name.
   * **Active Speaker Helper**: Leave as default (`input_text.pocket_assistant_active_speaker`).
3. Save the script with Entity ID: `script.music_assistant_browse`.

#### B. Configure the Active Speaker Helper & Metadata Sensors
Pocket Assistant mirrors playback metadata from whichever household speaker is currently active. You can set this up in two ways:

* **Option 1 (Fastest — Drop-in Package)**: Copy [`homeassistant/packages/music_assistant_esphome_mirror.yaml`](homeassistant/packages/music_assistant_esphome_mirror.yaml) into your `/config/packages/` folder. This automatically creates `input_text.pocket_assistant_active_speaker` and the 6 mirror template sensors (`sensor.pocket_assistant_target_*`).
* **Option 2 (Manual UI Setup)**:
  * Go to **Settings** -> **Devices & Services** -> **Helpers** -> **Create Helper** -> **Text**.
  * Name: `Pocket Assistant Active Speaker` (Entity ID: `input_text.pocket_assistant_active_speaker`).

### 3. Deploy Firmware (One-Click Remote Git Package)
In your Home Assistant **ESPHome Device Builder** dashboard, create a new device or edit your configuration with this minimal stub:

```yaml
substitutions:
  name: "pocket-assistant"
  friendly_name: "Pocket Assistant"
  version: "v3.4.8"

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

* **Switch Apps**: Tap or swipe the **left edge** ($x < 14\%$) or **right edge** ($x > 86\%$) of the display to flip through the active app deck (Clock <-> Stopwatch <-> Music <-> Games <-> System).
* **Music Menus**: Inside library menus and browsers, edge touches paginate list views forward and back without exiting to other apps.
* **Top Crown Button (AXP2101 PEK)**:
  * **On Clock Face**: Quick screen standby (Tier 2).
  * **In Stopwatch**: Starts and stops the chronometer with release-dwell latency compensation.
  * **In Music**: Toggles between Now Playing and the Library Selection Menu.
  * **In Games**: Exits active game to menu.
  * **In System**: Cycles calibrated brightness presets (40% -> 100%).
  * **Hold (> 1.5 s)**: Toggles capacitive Pocket Lock.
* **Side Button (GPIO0 / Boot Button)**:
  * **Activate Assist**: Press the side button. Background music ducks 20 dB, the display wakes, and Assist begins listening with visual feedback.
  * **Dismiss Assist**: Press the side button again during listening, thinking, or TTS speech to abort immediately, unmute audio, and restore the screen.
  * **Hold (> 1.0 s)**: Initiates deep sleep hibernation.
* **Deep Sleep Wakeup**:
  * Press and hold the **top crown button** for ~0.5 s (>200 ms) and **release it**. The release confirms a deliberate wake action, filtering out accidental pocket bumps.

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
    ├── blueprints/
    │   └── script/
    │       └── music_assistant_browse.yaml    # Script Blueprint for MA library browsing
    └── packages/
        └── music_assistant_esphome_mirror.yaml # Drop-in HA helper & sensor package
```

---

## 📜 Version History & Changelog

### v3.4.8 (Current)
* **Smart-Sorted Dynamic Speaker Handoff**: Priority-ranks the active speaker at Slot 1 (highlighted green), followed by the handheld device at Slot 2 for instant return, and remaining household speakers alphabetically.
* **Dedicated Podcasts & Audiobooks Categories**: Split spoken-word audio into separate native categories (`podcasts`, `audiobooks`) alongside `radio`, `albums`, `artists`, `playlists`, and `favorites` across a clean 3-page carousel.
* **Purged Legacy Substitutions**: Removed hardcoded presets (`preset_*`) and fixed speaker targets (`speaker_2_*`, `speaker_3_*`). All media lists and speaker targets are now 100% dynamically discovered from Music Assistant.
* **Home Assistant Script Blueprint Architecture**: Decoupled the music browsing engine into a reusable Home Assistant Script Blueprint (`homeassistant/blueprints/script/music_assistant_browse.yaml`).
* **Companion Setup Package**: Bundled `homeassistant/packages/music_assistant_esphome_mirror.yaml` for instant zero-friction creation of the active speaker helper and remote metadata template sensors.
* **System Page Declutter**: Removed redundant `(Tap)` instructional labels from Brightness, Start App, and Restart buttons for a cleaner instrument aesthetic.
* **Complete Eradication of Legacy Names**: Standardized all entities, substitutions, and helper names to `pocket_assistant` / `pocket-assistant`.

### v3.4.7
* **Standardized Bottom Navigation Hierarchy**: Moved `< MENU` to the bottom across all library browsing overlays, eliminating redundant top menu buttons and obsolete on-screen `[ CLOSE ]` buttons.
* **Dynamic Breadcrumb Navigation**: Bottom pill cleanly handles tier-by-tier navigation (`< MENU` returns to parent menu; `< BACK` steps up drilldowns like Tracks → Albums → Artists).
* **Reclaimed Screen Header & Centered Titles**: Lowered library and category titles from cramped $y = 48$ down to standardized $y = 80$.
* **Expanded Card Pitch (75px / 31px Gaps)**: Reclaimed vertical real estate to expand card pitch from 70px to 75px ($y = 140, 215, 290$), providing generous 31px gaps between content pills and zero-dead-zone touch hitboxes.

### v3.4.6
* **Naked Enlarged Track Navigation Chevrons**: Replaced enclosed circular buttons with prominent, naked 22px-tall white double chevrons (`◀◀` and `▶▶`) on the Music Player.
* **Rich Antique Crimson Palette**: Recalibrated `col_rust` (`#9E453B`, 45% sat) and `col_red` (`#A84338`, 50% sat) to restore warm, rich, vibrant antique red tones without subpixel blooming.
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
Distributed under the Apache 2.0 License. See `LICENSE` for details.
