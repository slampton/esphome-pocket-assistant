# 🧭 Pocket Assistant

> **A pocket-sized smart companion for Home Assistant with native Voice Assistant and a first-of-its-kind custom Music Assistant library browser & controller.**

[![Version](https://img.shields.io/badge/Version-v3.4.8-orange.svg)](https://github.com/slampton/esphome-pocket-assistant/releases)
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

### 3. Single-Authority Audio Architecture, Configurable Volume Controls & Ergonomic HUD
* **The Challenge**: Earlier iterations ran two separate media players—one for Music and one for Voice Assistant. Because both players ultimately commanded the same physical ES8311 DAC, adjusting the volume on one media player inadvertently overwrote the hardware gain register of the other, leading to cross-wired volume sliders. Furthermore, on boot the ES8311 chip initializes at raw 0 dB (100% volume), causing sudden loud blasts until a slider was nudged, and rapid double-I2C writes with network RPC loops caused audible micro-stutters during playback.
* **The Architectural Redesign**:
  * **Single Media Player Authority & Glitch-Free Local Volume**: `pocket_music_player` serves as the sole master media player entity in Home Assistant. When adjusting local volume, the firmware updates the ES8311 hardware DAC gain directly in 50µs and lets ESPHome's native state stream update Home Assistant/Music Assistant. Eliminating the redundant network RPC round-trip and doubling the I2S speaker buffer to 1000ms ensures completely glitch-free playback during volume adjustments.
  * **Configurable Volume Step Size Entity**: Exposes a persistent `number.pocket_assistant_volume_step_size` configuration slider in Home Assistant (range: 1% to 10%, step: 1%, default: 2%). When tapping on-screen volume carets, the firmware calculates the exact target (`current ± step`) smoothly with zero rubber-banding.
  * **Ergonomic Volume Display Position Entity**: Exposes a `select.pocket_assistant_volume_display_position` entity with options `Left` (default), `Right`, and `Hidden`. Placing the volume display to the left of the carets by default prevents a right-handed user's thumb from physically blocking the readout while pressing the buttons, rendered in high-contrast pure white text in the empty black bezel space outside the album art area.
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

### 2. Home Assistant Setup (Blueprint & Helpers)

To connect Pocket Assistant to Music Assistant and enable library browsing, speaker handoff, and remote metadata mirroring, set up the Home Assistant side:

#### A. Install the Browse Script Blueprint
Pocket Assistant uses a native Home Assistant Script Blueprint to dynamically fetch and paginate library data directly from Music Assistant without hardcoding entity IDs:
1. Copy [`homeassistant/blueprints/script/music_assistant_browse.yaml`](homeassistant/blueprints/script/music_assistant_browse.yaml) to your Home Assistant configuration directory under:
   `/config/blueprints/script/esphome/music_assistant_browse.yaml`
   *(Or import it via **Settings** -> **Automations & Scenes** -> **Blueprints**).*
2. Click **Create Script** from the Blueprint:
   * **Target ESPHome Device Name**: Leave as default (`pocket-assistant`), or enter your custom node name.
   * **Active Speaker Helper**: Leave as default (`input_text.pocket_assistant_active_speaker`).
3. Save the script with Entity ID: `script.music_assistant_browse` (or your custom entity ID matching `${browse_script}` in your ESPHome substitutions).

#### B. Configure the Active Speaker Helper & Metadata Sensors
Pocket Assistant mirrors playback metadata (title, artist, album, duration, progress) from whichever household speaker is currently active. You can set this up in two ways:

* **Option 1 (Fastest — Drop-in Package)**: Copy [`homeassistant/packages/music_assistant_esphome_mirror.yaml`](homeassistant/packages/music_assistant_esphome_mirror.yaml) to your `/config/packages/` folder. This automatically creates `input_text.pocket_assistant_active_speaker` and the 6 mirror template sensors (`sensor.pocket_assistant_target_*`).
* **Option 2 (Manual UI Setup)**:
  * Go to **Settings** -> **Devices & Services** -> **Helpers** -> **Create Helper** -> **Text**.
  * Name: `Pocket Assistant Active Speaker` (Entity ID: `input_text.pocket_assistant_active_speaker`).
  * Add the template sensors to your `configuration.yaml` if you want remote playback progress and metadata mirrored on-screen when controlling other speakers.

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
    ├── blueprints/
    │   └── script/
    │       └── music_assistant_browse.yaml    # Script Blueprint for MA library browsing
    └── packages/
        └── music_assistant_esphome_mirror.yaml # Drop-in HA helper & sensor package
```

---

## 📜 Version History & Changelog

### v3.4.8 (Current)
* **Home Assistant Script Blueprint Architecture**: Decoupled the music browsing engine into a reusable Home Assistant Script Blueprint (`homeassistant/blueprints/script/music_assistant_browse.yaml`), allowing any ESPHome device to generate its own browsing script with automatic RPC target resolution.
* **Complete Eradication of Legacy Names**: Standardized all entities, substitutions, and helper names to `pocket_assistant` / `pocket-assistant`, purging all legacy nomenclature.
* **ESPHome YAML Generalization**: Extracted all external Home Assistant sensor and helper dependencies in `core/ui.yaml` into top-level customizable substitutions in `pocket-assistant.yaml` (`browse_script`, `active_speaker_helper`, `target_*_sensor`).
* **Drop-in Companion Package**: Bundled `homeassistant/packages/music_assistant_esphome_mirror.yaml` for instant zero-friction creation of the active speaker helper and remote metadata template sensors.
* **Streamlined Repository & Clean Package**: Removed legacy static YAML scripts in favor of native blueprints and clean modular structure.

### v3.4.7
* **Standardized Bottom Navigation Hierarchy**: Moved `< MENU` to the bottom across all library browsing overlays, eliminating redundant top menu buttons and obsolete on-screen `[ CLOSE ]` buttons (since the physical crown button exits to player).
* **Dynamic Breadcrumb Navigation**: Bottom pill cleanly handles tier-by-tier navigation (`< MENU` returns to parent menu; `< BACK` steps up drilldowns like Tracks → Albums → Artists).
* **Reclaimed Screen Header & Centered Titles**: Lowered library and category titles from cramped $y = 48$ down to standardized $y = 80$ (matching $y = 82$ across Clock, Stopwatch, Music, Games, and System).
* **Expanded Card Pitch (75px / 31px Gaps)**: Reclaimed vertical real estate to expand card pitch from 70px to 75px ($y = 140, 215, 290$), providing generous 31px gaps between content pills and zero-dead-zone touch hitboxes.

### v3.4.6
* **Naked Enlarged Track Navigation Chevrons**: Replaced enclosed circular buttons with prominent, naked 22px-tall white double chevrons (`◀◀` and `▶▶`) on the Music Player.
* **Rich Antique Crimson Palette**: Recalibrated `col_rust` (`#9E453B`, 45% sat) and `col_red` (`#A84338`, 50% sat) to restore warm, rich, vibrant antique red tones without drifting into brown or causing neon subpixel blooming.
* **Expanded System Page Card Spacing**: Increased card pitch on the System dashboard from 60px to 75px ($y = 245, 320, 395$), providing generous 31px vertical separation.
* **Harmonized Music Menu Layout**: Aligned the Music Menu title to $y = 82$ and positioned the top cards at $y = 155, 235, 315$.
* **Streamlined Library Categories**: Replaced the 3-page categories carousel with 2 balanced pages of 3 items each by deprecating standalone "TRACKS" (preserving album track drilldown).

### v3.4.5
* **Artifact-Free Pill Button Geometry**: Overhauled button rendering across Music, Games, and System menus using concentric filled shapes.
* **Vertically Centered Pill Typography**: Corrected text vertical alignment across all menus, perfectly centering labels on the pill centerline (`y_mid`).
* **Synchronized Title Alignment**: Aligned Games page title to `y = 82` to match Music and System.

### v3.4.4
* **Ergonomic Volume Display Position Entity**: Added `select.pocket_assistant_volume_display_position` (`Left`, `Right`, `Hidden` — default: `Left`). Defaults to the left side of the volume carets so right-handed users' thumbs do not block the readout during button presses.
* **Glitch-Free Audio Volume Pipeline**: Eliminated redundant network RPC echo loops during local volume adjustment and expanded I2S physical speaker buffer duration to 1000ms.
* **High-Contrast Unobstructed Volume HUD**: Removed the volume overlay circle from on top of the album art. Placed clean, high-contrast white text (`74%`) in empty black bezel space beside the active volume caret.
* **Cold-Start Connection Status**: Added instant `"Connecting to stream..."` visual feedback when tapping Play from an idle state.

### v3.4.3
* **Glitch-Free Audio Volume Pipeline**: Eliminated redundant network RPC echo loops during local volume adjustment and expanded I2S physical speaker buffer duration to 1000ms.
* **Cold-Start Connection Status**: Added instant `"Connecting to stream..."` visual feedback when tapping Play from an idle state.

### v3.4.2
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
