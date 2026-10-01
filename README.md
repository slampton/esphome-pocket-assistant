# 🧭 Pocket Assistant

> **A pocket-sized smart companion for Home Assistant featuring native Voice Assistant, universal active album art, and a first-of-its-kind dynamic Music Assistant library browser & multi-room remote.**

[![Version](https://img.shields.io/badge/Version-v1.0.7-orange.svg)](https://github.com/slampton/esphome-pocket-assistant/releases)
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
* 🎙️ **Voice Assistant with Touch Modal Takeover & Tap-to-Dismiss**: Direct Assist satellite pipeline with dynamic 20 dB music ducking, kinetic AMOLED visual feedback, physical side-button abort, and full-screen touch modal interception allowing tap-to-dismiss without triggering background app cards.
* 🎚️ **Single-Authority Audio & Glitch-Free Volume**: Unified physical DAC control with priority-synchronized boot gain (no 100% startup blasts) and a configurable **Volume Step Size** entity (1%–10%, default 2%) to eliminate slider rubber-banding.
* ⏱️ **Vintage Chronograph & Lap Stopwatch**: Precision chronometer featuring an aged parchment Heuer-inspired dual-subdial dial, center sweep seconds, and crown button controls with hardware release-dwell latency compensation.
* 🎮 **Interactive Motion Games**: Real-time 20 FPS physics games (Marble Maze, Archery Target, CHOMP (Kids)) powered by the onboard 6-axis IMU.
* 🔋 **Intelligent Multi-Tier Power Management**: Instant AMOLED screen standby with modular pickup wake modes (Always On, Docked Only, or Button Only), selective Tap-to-Wake gating (`select.tap_wake_mode`), configurable standby auto-sleep timeout (5m, 10m, 15m, 30m, Disabled), physical pocket lock, 5-second abortable hibernation countdown, hardware deep sleep with accidental-bump rejection, and USB dock stay-awake override.
* 🛡️ **2-Tier Hierarchical System Submenu Hub with Software Updates**: Clean separation of Mode 0 observational telemetry (battery %, voltage, Wi-Fi RSSI/IP, IMU temp/orientation, firmware version, wake settings, live brightness, and sleep timeout) from an interactive menu hierarchy: Mode 1 (System Hub router), Mode 2 (Device Settings cyclers for Brightness, Start App, Motion Wake, Tap Wake), Mode 3 (Power Options for Hibernate Now, Auto-Sleep cycler, and Restart), Mode 4 (Protected Restart confirmation dialog), Mode 5 (On-Device Software Update management screen), and Mode 6 (Update confirmation dialog).
* 🔄 **Overhauled Radial OTA In-Progress Overlay**: A watchOS-grade 360° circular progress arc with 10 FPS smooth linear interpolation (`lerp`) and an orbiting illuminated pip that eliminates chunky multi-second jumps and displays real-time stage cues (`"Writing flash memory..."`, `"Verifying image..."`).
* 💡 **Live Display Brightness Readout**: Real-time on-screen telemetry on the System Dashboard updating instantaneously when cycling brightness via the physical top crown button.
* 🔄 **Dual Firmware Update Entities (Made for ESPHome Standard)**: Implements the official ESPHome `update:` platform via `http_request`. In Home Assistant, Pocket Assistant cleanly exposes both entities under the device: the core ESPHome compiler engine update (`update.pocket_assistant_firmware`) and the dedicated project release firmware entity (`update.pocket_assistant_pocket_assistant_firmware` tracking releases against `manifest.json`), with 1-click on-device and Home Assistant OTA updates.
* 🕒 **Modern Dial Clock Face**: High-contrast watch dial with Roman indices, polished polygon hands, 12-hour digital readout, and a sleek vertical battery level gauge with a golden charging lightning bolt.
* ⚙️ **Elevated System Telemetry**: High-legibility live telemetry readouts for battery percentage, voltage, charging state, Wi-Fi signal strength, and hardware states with generous vertical breathing room.
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
* **CHOMP (Kids)**: Tilt to roll treats into the animated cat's mouth.

### ⚙️ System Dashboard & Hierarchical Submenu Hub (`apps/system.yaml` / `core/ui.yaml`)
Consolidated hardware status, settings, software updates, and power controls organized into a clean 2-tier hierarchy:
* **Mode 0 (Telemetry Dashboard)**: 100% observational display with zero risk of accidental setting toggles. Shows real-time battery % and voltage, USB charging status, Wi-Fi RSSI and IP address, IMU temperature and orientation, firmware version (with live "Update Ready: vX.X.X" highlight and 1-tap shortcut to updates), Motion & Tap Wake modes, live screen brightness, and standby sleep timeout.
* **Mode 1 (System Hub Router)**: High-level category menu dividing controls into **Device Settings**, **Software Update**, and **Power Options**.
* **Mode 2 (Device Settings Submenu)**: Interactive cycler pills for **Brightness Preset** (40%–100%), **Default Start App** (Clock, Stopwatch, Music, Games, System), **Motion Wake Mode** (Always On, Docked Only, Disabled), and **Tap Wake Mode** (Always On, Docked Only, Disabled).
* **Mode 3 (Power Options Submenu)**: Instant **Hibernate Now**, one-touch **Auto-Sleep Timeout** cycler (5 min, 10 min, 15 min, 30 min, Disabled), and **Restart Device**.
* **Mode 4 (Restart Confirmation Modal)**: Protected dialog requiring deliberate confirmation (`Cancel` vs. `Restart`) to eliminate accidental restarts.
* **Mode 5 (Software Update Screen)**: Dedicated on-device firmware management displaying currently installed version vs. latest available release from Home Assistant / GitHub, changelog summary highlights, and an interactive **Update Now** (or **Check for Updates**) pill button.
* **Mode 6 (Update Confirmation Modal)**: Safety dialog requiring explicit confirmation (`Cancel` vs. `Install`) before triggering Home Assistant OTA downloads and flash execution.
* **Overhauled Radial OTA In-Progress Overlay**: A watchOS-grade 360° circular progress arc with 10 FPS smooth linear interpolation (`lerp`) and an orbiting illuminated pip that eliminates chunky multi-second jumps and displays real-time stage cues (`"Writing flash memory..."`, `"Verifying image..."`).

---

## 🚀 Installation & First-Time Setup

Getting Pocket Assistant up and running takes just three simple steps: flash the firmware, adopt the device in Home Assistant, and optionally enable the Music Assistant companion package.

---

### Step 1: Flash Firmware & Connect to Wi-Fi

Choose whichever installation method fits your workflow:

#### Method A: Pocket Assistant Web Installer (Recommended — 1-Click in Browser)
1. Plug your Pocket Assistant into your computer using a USB-C data cable.
2. Open the **[Pocket Assistant Web Installer](https://slampton.github.io/esphome-pocket-assistant/)** in a WebSerial-supported browser (Google Chrome, Microsoft Edge, or Opera).
3. Click **Install Pocket Assistant**, select your device's USB serial port, and follow the on-screen prompt.
4. When prompted, enter your local Wi-Fi SSID and password to bring the device online.

#### Method B: ESPHome Dashboard in Home Assistant (Remote Git Package)
If you manage your devices via the Home Assistant ESPHome add-on:
1. In Home Assistant, open **ESPHome Device Builder** from the sidebar.
2. Click **New Device**, name it `pocket-assistant`, and paste this clean remote package configuration:
   ```yaml
   substitutions:
     name: "pocket-assistant"
     friendly_name: "Pocket Assistant"
     version: "v3.5.4"
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
         - pocket-assistant-1.75c.yaml
   ```
3. Click **Save** and **Install** (select **Plug into this computer** for first-time USB flash, or **Over the air** if already online).

---

### Step 2: Adopt Device in Home Assistant

1. In Home Assistant, navigate to **Settings** -> **Devices & Services**.
2. Pocket Assistant will appear under **Discovered** devices.
3. Click **Configure**, enter your `pocket_assistant_encryption_key` (if prompted), and assign it to an area.

---

### Step 3: Enable Music Assistant Companion Package (Optional)

Pocket Assistant mirrors live playback metadata (title, artist, album, progress, and artwork) from whichever household speaker is active.

#### 1-Click Package Setup (Recommended)
1. Open your Home Assistant configuration directory (`/config`) using **Studio Code Server**, **File Editor**, or Samba.
2. Ensure package support is enabled in `/config/configuration.yaml` (one-time prerequisite):
   ```yaml
   homeassistant:
     packages: !include_dir_named packages
   ```
3. Copy [`homeassistant/packages/music_assistant_esphome_mirror.yaml`](homeassistant/packages/music_assistant_esphome_mirror.yaml) into `/config/packages/`.
4. Go to **Developer Tools** -> **YAML** -> **Template Entities** and click **Reload**.
5. Import the **Browse Script Blueprint** ([`homeassistant/blueprints/script/music_assistant_browse.yaml`](homeassistant/blueprints/script/music_assistant_browse.yaml)) via **Settings** -> **Automations & Scenes** -> **Blueprints**, click **Create Script**, and save it as `script.music_assistant_browse`.

<details>
<summary>👉 Click to view alternative manual templates.yaml or monolithic setup</summary>

<br>

##### Dedicated Template File (`template: !include templates.yaml`)
If your Home Assistant environment routes templates through a dedicated `templates.yaml` file:

1. **Add Helper to `configuration.yaml`** (or create it via **Settings -> Devices & Services -> Helpers**):
   ```yaml
   input_text:
     pocket_assistant_active_speaker:
       name: "Pocket Assistant Active Speaker"
       icon: mdi:speaker
   ```
2. **Append to `/config/templates.yaml`**:
   ```yaml
   - trigger:
       - platform: state
         entity_id: input_text.pocket_assistant_active_speaker
       - platform: time_pattern
         seconds: "/2"
     sensor:
       - name: "Pocket Assistant Target Title"
         unique_id: pocket_assistant_target_title
         state: >-
           {% set target = states('input_text.pocket_assistant_active_speaker') %}
           {% if not target or target in ['unknown', 'unavailable', ''] %}
             Idle
           {% else %}
             {{ state_attr(target, 'media_title') | default('Idle', true) }}
           {% endif %}

       - name: "Pocket Assistant Target Artist"
         unique_id: pocket_assistant_target_artist
         state: >-
           {% set target = states('input_text.pocket_assistant_active_speaker') %}
           {% if not target or target in ['unknown', 'unavailable', ''] %}
             None
           {% else %}
             {{ state_attr(target, 'media_artist') | default('None', true) }}
           {% endif %}

       - name: "Pocket Assistant Target Album"
         unique_id: pocket_assistant_target_album
         state: >-
           {% set target = states('input_text.pocket_assistant_active_speaker') %}
           {% if not target or target in ['unknown', 'unavailable', ''] %}
             None
           {% else %}
             {{ state_attr(target, 'media_album_name') | default('None', true) }}
           {% endif %}

       - name: "Pocket Assistant Target Duration"
         unique_id: pocket_assistant_target_duration
         state: >-
           {% set target = states('input_text.pocket_assistant_active_speaker') %}
           {% if not target or target in ['unknown', 'unavailable', ''] %}
             0
           {% else %}
             {{ state_attr(target, 'media_duration') | default(0, true) }}
           {% endif %}

       - name: "Pocket Assistant Target Position"
         unique_id: pocket_assistant_target_position
         state: >-
           {% set target = states('input_text.pocket_assistant_active_speaker') %}
           {% if not target or target in ['unknown', 'unavailable', ''] %}
             0
           {% else %}
             {{ state_attr(target, 'media_position') | default(0, true) }}
           {% endif %}

       - name: "Pocket Assistant Target State"
         unique_id: pocket_assistant_target_state
         state: >-
           {% set target = states('input_text.pocket_assistant_active_speaker') %}
           {% if not target or target in ['unknown', 'unavailable', ''] %}
             idle
           {% else %}
             {{ states(target) | default('idle', true) }}
           {% endif %}

       - name: "Pocket Assistant Target Art URL"
         unique_id: pocket_assistant_target_art_url
         state: >-
           {% set target = states('input_text.pocket_assistant_active_speaker') %}
           {% if not target or target in ['unknown', 'unavailable', '', 'media_player.pocket_assistant'] %}
             none
           {% else %}
             {% set pic = state_attr(target, 'entity_picture') or state_attr(target, 'media_image_url') %}
             {% if pic and pic not in ['none', 'unknown', 'unavailable'] %}
               {% if pic.startswith('http') %}
                 {{ pic }}
               {% elif pic.startswith('/') %}
                 {{ 'http://homeassistant.local:8123' ~ pic }}
               {% else %}
                 none
               {% endif %}
             {% else %}
               none
             {% endif %}
           {% endif %}
   ```
3. Navigate to **Developer Tools** -> **YAML** -> **Template Entities** and click **Reload**.

##### Monolithic Setup (`configuration.yaml`)
If your configuration is un-split, you can paste the full contents of `homeassistant/packages/music_assistant_esphome_mirror.yaml` directly into your `/config/configuration.yaml` (merging under existing `input_text:` and `template:` keys), then reload Template Entities.
</details>

## 🧭 Navigation & Hardware Controls

| Input / Control | Context / Screen | Behavior |
| :--- | :--- | :--- |
| **Top Crown (Short Click)** | **Screen Off (Standby)** | Wakes display immediately. If asleep $>30\text{ s}$, opens Default Start App; if $<30\text{ s}$, restores previous screen. |
| **Top Crown (Short Click)** | **Pages 0–3 (Clock, Stopwatch, Music, Games)** | Exits active game to menu, or toggles between active app and Default Start App. In Stopwatch, operates as precision Start/Stop with 200ms latency compensation. |
| **Top Crown (Short Click)** | **Page 4 (System App)** | **Cycles Display Brightness Preset** ($40\% \rightarrow 50\% \rightarrow \dots \rightarrow 100\% \rightarrow 40\%$) with instant on-screen feedback. |
| **Top Crown (Hold >1.5s)** | **Any Screen** | Initiates the **5-second abortable Hibernation Countdown**. (A short click or continuing to hold during countdown aborts it; releasing and waiting 5s enters hardware deep sleep $<100\mu\text{A}$). |
| **Top Crown (Hold ~0.5s)** | **Deep Sleep (Off)** | Boots the device (filtered by $>200\text{ms}$ anti-pocket-bump threshold). |
| **Side Button (Short Press)** | **Any Screen / Standby** | Activates Voice Assistant (Assist satellite listening mode). Pauses local music and routes I2S to microphone. |
| **Side Button (Short Press)** | **Assist Active** | **Instantly cancels Voice Assistant** via hardware hook, mutes audio, and returns to previous app. |
| **Anywhere on Screen (Tap)** | **Assist Active** | **Tap-to-Dismiss**: Aborts Voice Assistant modal without triggering underlying app controls. |
| **Left / Right Edge Swipe** | **Main App Deck** | Cycles through active apps: `Clock` $\leftrightarrow$ `Stopwatch` $\leftrightarrow$ `Music` $\leftrightarrow$ `Games` $\leftrightarrow$ `System`. |
| **Left / Right Edge Swipe** | **Library Submenus** | Paginates forward and back with bidirectional carousel wrap-around (Page 1 $\leftrightarrow$ Page 3). |
| **Perimeter Rotational Swipe** | **Music Player** | Adjusts volume with left-side HUD feedback. |
| **Center Screen Tap** | **Music Player** | Toggles Play / Pause. |
| **Center Screen Tap** | **Stopwatch** | Instant leading-edge capacitive Start / Stop across central dial. |

## 📂 Repository Layout

```text
esphome-pocket-assistant/
├── pocket-assistant-1.75c.yaml          # Master node configuration & substitutions
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
│   ├── games.yaml                 # 3 motion physics games (Marble, Archery, CHOMP (Kids))
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

### v3.5.4 (Current)
* **On-Device Software Update Screen (System Mode 5)**:
  * Added dedicated on-device firmware update screen in the System Application (`system_menu_mode == 5`), displaying currently installed version vs. latest available release version from Home Assistant (`update.pocket_assistant_firmware`).
  * Features live release highlights and changelog summary display.
  * Interactive **"Update Now"** (or **"Check for Updates"**) pill button with live state feedback.
  * System Hub router (`system_menu_mode == 1`) updated with dedicated **"Software Update"** pill button that dynamically illuminates in `col_gold` when a new version is available.
  * 1-Tap shortcut directly from the Telemetry Dashboard (`system_menu_mode == 0`) by tapping the version line.
* **Protected 2-Step Update Confirmation Dialog (System Mode 6)**:
  * Eliminates accidental OTA flashes with a dedicated confirmation modal (`system_menu_mode == 6`) requiring explicit touch confirmation (`[ Cancel ]` vs `[ Install ]`).
  * Displays pre-flash safety warnings ("Download & flash firmware", "Keep device powered on").
  * Automatically invokes Home Assistant `update.install` action and arms the on-device progress overlay.
* **Overhauled Radial OTA In-Progress Overlay**:
  * Replaced static text with a full-screen circular progress arc ($R = 210	ext{--}218	ext{ px}$) wrapping around the AMOLED bezel from $0^\circ$ to $360^\circ$ in `col_cyan` over a recessed track in `col_dark_gray`.
  * **Continuous 10 FPS Smooth Linear Interpolation (`lerp`)**: Interpolates progress between ESPHome's internal 1-second backend OTA ticks, eliminating chunky $5\%	ext{--}8\%$ visual jumps and providing fluid, watchOS-grade animation.
  * **Leading-Edge Orbiting Illuminated Pip**: A high-luminance glowing dot (white core with cyan aura) orbits smoothly at the head of the progress arc, providing instant visual confirmation of active data reception.
  * **Dynamic Real-Time Stage Status**: Contextual stage progression cues below the percentage readout (`"Connecting & preparing..."`, `"Writing flash memory..."`, `"Verifying image..."`, `"Flash complete. Rebooting..."`).

### v3.5.3
* **Live Display Brightness Readout**:
  * Added real-time on-screen brightness telemetry (`Brightness: XX% | Sleep: XX`) at line $y = 260$ on the Mode 0 Observational Dashboard.
  * Updates instantaneously upon physical crown button clicks, providing clear feedback when cycling brightness presets without opening submenus.
* **Configurable Standby Sleep Timeout (`select.sleep_timeout_mode`)**:
  * Implemented an ESPHome template select entity with options: `5 min` *(Default)*, `10 min`, `15 min`, `30 min`, and `Disabled`.
  * Preserved across reboots and deep sleep via `restore_value: true`, automatically exposed to Home Assistant for remote dashboard control.
  * Integrated a one-touch cycler pill button in View 3 (Power Options Submenu) at $y = 160$ with rebalanced 60px vertical spacing.
  * Dynamically evaluates inactivity timeout in Power Management Priority 4, honoring `prevent_deep_sleep_switch` and `Disabled` mode.
* **Streamlined Documentation & Web Builder Support**:
  * Re-architected installation instructions into a bite-sized 3-step guide highlighting the dedicated web installer (`https://slampton.github.io/esphome-pocket-assistant/`) for 1-click browser-based flashing.
  * Compartmentalized advanced split-template and monolithic YAML examples inside collapsible `<details>` blocks to eliminate visual clutter.
  * Added a complete, multi-context Hardware Button & Touch Input matrix.

### v3.5.2
* **C++ Lambda Compilation Hotfix**:
  * Resolved an ESP-IDF 5.5.5 / GCC compilation error (`error: expected ')' before '}' token` at line 3436 of `core/ui.yaml`) caused by an extraneous closing brace following the display lambda.
  * Performed automated AST balance verification across all 71 C++ lambdas in the repository.

### v3.5.1
* **2-Tier Hierarchical System Submenu Hub**:
  * Partitioned the System application into 5 clean modes: Mode 0 (Observational Telemetry Dashboard), Mode 1 (System Hub router), Mode 2 (Device Settings cyclers), Mode 3 (Power Options), and Mode 4 (Protected Restart confirmation modal).
  * Completely eliminated accidental setting changes on the initial System screen while keeping all diagnostics immediately visible.
* **Selective Tap-to-Wake Gating (`select.tap_wake_mode`)**:
  * Added `select.tap_wake_mode` with options `Always On`, `Docked Only`, and `Disabled`.
  * In standby on battery with `Docked Only` or `Disabled`, touch polling is ignored to eliminate phantom pocket touches while preserving motion pickup and crown button wake.
* **Voice Assistant Full Modal Touch Interception & Tap-to-Dismiss**:
  * Added full-screen touch interception during active Voice Assistant states (listening, thinking, responding), allowing users to tap anywhere on the screen to cleanly cancel Assist without click-through.

### v1.0.7 (Ergonomic Full-Screen Button Placement & Cold Boot Play Fix)
* **Mode-Aware Flank Button Placement (Full Screen & Letterbox)**:
  * Repositioned Previous Track (`x = 136`) and Next Track (`x = 330`) buttons in Full Screen and Letterbox modes, bringing them inward from the outer bezels to establish balanced 45px visual clearance to the center Play/Pause capsule (`x: 199..267`) and 39px clearance to outer volume/menu buttons.
  * Compact mode preserves wider placement (`x = 102`, `x = 364`) to clear the 216×216 album art square.
* **Synchronized Mode-Aware Touch Partitioning (Zero Dead Zones)**:
  * Dynamically shifts horizontal touch boundaries: in Full Screen/Letterbox, Previous Track expands to `x <= 184` and Next Track expands to `x >= 282`, providing generous 30px margins around both 18px capsules without overlapping Play/Pause (`x: 184..282`, 98px wide).
* **Cold Boot / Restart Play Command Resolution**:
  * Fixed a critical routing bug in `toggle_ha_music`: previously, when paused after boot, if `active_speaker_entity` was local, it only invoked the local ESPHome action `media_player.play: pocket_music_player` (a passive I2S receiver for Sendspin), failing to notify Home Assistant / Music Assistant to start the queue stream.
  * Now unconditionally dispatches `homeassistant.action: media_player.media_play` targeting `active_speaker_entity`, ensuring Music Assistant wakes the queue and streams immediately.
* **Instant Track Transition Visual Feedback**:
  * Added instant feedback text in `music_next_track` (`"Next Track >>"`) and `music_prev_track` (`"<< Prev Track"`), automatically cleared when the new track title arrives via `ha_track_title`.

### v1.0.6 (High-Speed Row-Major Album Art Engine & Zero-Overhead Scrim)
* **Root Cause Elimination of Full-Screen UI Lag**:
  * Uncovered critical cache-thrashing flaw in ESPHome's built-in  (): outer loop iterates  and inner loop iterates , traversing memory column-by-column across row-major RGB565 PSRAM buffers.
  * For 466x466 images, this generated 217,156 consecutive L1 cache misses on reads and 217,156 cache misses on writes (>434,000 cache misses per second on the 1s display update), locking the ESP32-S3 CPU for ~100ms every second, starving CST9220 I2C touch polling, and causing I2S audio buffer underruns.
* **Unified Row-Major Direct Pointer Scaling Engine**:
  * Replaced  in Full Screen and Letterbox modes with the high-speed row-major direct pointer engine (), achieving >96% L1 cache hit rate and direct 1:1 memory reads for native 466x466 art.
  * Added fixed-point integer scaling () to seamlessly handle arbitrary source image resolutions (scaling up or down smoothly).
* **Circular AMOLED Display Boundary Optimization ()**:
  * Integrated a 466-element static circular chord table, skipping all 46,600 invisible corner pixels outside the circular glass perimeter and reducing total pixel operations to ~155k.
* **Native Single-Pass 50% Dither Scrim & Zero-Cost Letterbox Matte Bars**:
  * Eliminated the secondary dither scrim pass over  = 320..395$ and the  loop: simply skips alternating pixels () during the art draw. Because the display buffer clears to black, skipped pixels render as pure OLED black at zero CPU cost.
  * In Letterbox mode, restricts drawing strictly to the active cinema window ( = 60..384$), eliminating 65,700 pixel draws and removing  overwrite passes.
  * Execution time drops from 100+ ms down to ~4.5–5 ms (Full Screen) and ~3 ms (Letterbox), restoring 98% CPU headroom for rock-solid audio continuity and instant touch response.

### v1.0.5 (Full-Screen Album Art Optimization, Non-Blocking Touch, and Audio Continuity)
* **Non-Blocking Touch Architecture (Eliminated Swallowed Taps)**:
  * Removed all synchronous, blocking `id(disp1).update();` calls directly within `touchscreen.on_touch` across all music player buttons (Next Track, Prev Track, Volume Up, Volume Down, Play/Pause).
  * Replaced with asynchronous `script.execute: throttled_disp_update`, reducing touch callback latency from ~90ms down to `< 0.1ms`. CST9220 I2C touch interrupts are acknowledged instantaneously without missing quick taps during playback.
* **Display Update Coalescing & Main Loop Starvation Protection**:
  * Upgraded `throttled_disp_update` with a 60ms debounce window and an 80ms minimum redraw rate-limit (`last_redraw`).
  * Routed `on_image_display` (Sendspin) and `on_download_finished` (`online_image`) through `throttled_disp_update`.
  * Track change sensor bursts from Home Assistant (`target_title`, `target_artist`, `target_album`, `target_state`, `target_position`) now coalesce into a single screen redraw instead of 5-6 back-to-back 100ms redraws, completely eliminating FreeRTOS main loop starvation.
* **Precomputed Trigonometric Chord Table for 50% Dither Scrim**:
  * Replaced dynamic per-row `sqrtf((float)(54289 - y_d * y_d))` calculations across $y = 320..395$ with a static `CHORD_HW[76]` lookup table, eliminating 75 floating-point square root operations per frame and dropping scrim execution time to `< 0.1ms`.
* **Hardware-Friendly JPEG Decoding (`format: JPEG`)**:
  * Switched `platform: online_image` (`remote_album_art`) from `format: AUTO` to `format: JPEG`, removing PNG header probing overhead and leveraging the ESP32-S3 optimized TJpgDec engine.
  * Removed premature `id(current_loaded_art_url) = "";` from `ha_track_title.on_value` to eliminate redundant re-downloads of identical artwork during track title metadata refreshes.
* **Single-Flight Transport Action Hardening**:
  * Converted `music_next_track`, `music_prev_track`, and `toggle_ha_music` from `mode: restart` to `mode: single`, ensuring network actions to Home Assistant / Music Assistant complete reliably without in-flight script aborts from rapid taps.
* **Audio Buffer Underrun & WebSocket Protection**:
  * Decoupling display blits from volume taps and track transitions prevents FreeRTOS audio task buffer exhaustion, eliminating audio dropouts and Sendspin WebSocket PONG timeout disconnects (`close_code=1006`).

### v3.5.0
* **Standardized Active Album Art Pipeline & Decoder Alignment**:
  * **Clean Multi-Format Image Decoding (`format: AUTO`)**: Configured `platform: online_image` to `format: AUTO` with a valid static boot image (`/static/icons/favicon-192x192.png`). Automatically decodes JPEG and PNG based on server `Content-Type` headers, eliminating `Incorrect PNG signature` mismatches when receiving Home Assistant media player streams.
  * **Clean Entity Picture Pass-Through**: Standardized `sensor.pocket_assistant_target_art_url` in `music_assistant_esphome_mirror.yaml` to cleanly route the active speaker's authentic `entity_picture` (or `media_image_url`) through Home Assistant (`http://homeassistant.local:8123`) without brittle string-replacement hacks (e.g. injecting `size=200` or `fmt=jpg`), preventing Music Assistant HTTP 400 Bad Request rejections.
  * **Graceful Progressive JPEG Fallback**: For tracks sourced with progressive JPEGs (which ESP32-S3 MCU decoders cannot decompress), ESPHome safely leaves the image width at 0, cleanly displaying the styled high-contrast vinyl record disc without crashing. Baseline JPEGs and local Sendspin streams display in full 216x216 color.
  * **Home Assistant Template Reload Protocol**: Clarified that package sensors in `/config/packages/` are reloaded exclusively via **Developer Tools -> YAML -> Template Entities** (or full HA restart), completely distinct from Blueprint scripts.
* **Single-Flight Protected Album Art Download Engine**:
  * Resolved the remote speaker artwork loading failure (e.g. transfer to Garage HiFi) caused by download cancellation cascades: multi-source triggers previously invoked `online_image.set_url` repeatedly, resetting connections mid-stream.
  * Implemented strict URL de-duplication via `current_loaded_art_url` and frame buffer status guards: `online_image.set_url` fires **only** when the target URL changes to a new resource or if the existing buffer is empty, allowing active downloads to finish uninterrupted.
  * Extended `http_request` client timeout from 5s to **15s** to safely accommodate high-resolution remote art downloads over congested networks without socket timeouts.
  * **Continuous Track-Change Synchronization**: Integrated automated Home Assistant entity updates (`homeassistant.update_entity: sensor.pocket_assistant_target_art_url`) and buffer clearing directly into `play_music_slot_1/2/3`, `music_next_track`, `music_prev_track`, and `ha_track_title.on_value`, ensuring that browsing new tracks or skipping songs immediately pulls fresh artwork without delay.
* **Balanced Vertical Layout & Header Buffer**:
  * Added a comfortable vertical buffer between `"MUSIC"` ($y = 54$) and the active speaker indicator ($y = 80$, $26\text{ px}$ spacing).
  * Notched album art ($216\times 216\text{ px}$) to span $y = 104$ to $320$ (centered at $y = 212$), creating a balanced $24\text{ px}$ clearance below the speaker indicator.
  * Vertically aligned Previous/Next track chevrons, paused play badge, vinyl disc, and borderless black Volume HUD overlay to the new $y = 212$ center axis.
  * Notched track metadata downward ($y = 338$ Title, $y = 364$ Artist, $y = 386$ Album), leaving a generous $36\text{ px}$ clearance above the bottom volume baseline ($y = 422$).
* **Crisp Anti-Glare Volume Carets & Consolidated Bottom Controls**:
  * Resized bottom volume arrows to $18\times 14\text{ px}$ with clean $17\text{ px}$ uniform margins on either side of `"VOL"` at $y = 422$.
  * Calibrated arrow color to anti-glare muted brass gold (`col_warm_gold: #A88424`), dropping peak luminance by 18% to eliminate subpixel bloom and blur against the deep black AMOLED background while maintaining sharp geometric edges.
  * Preserved full split-screen bottom touch targets: left half ($touch.x \in [100, 233]$) for Volume Down, right half ($touch.x \in [233, 366]$) for Volume Up.
* **Unified Elevated Header Baseline ($y = 54$) Across Apps**:
  * Standardized the app header title across all applications at **$y = 54$** (Music, Games, System), creating a unified visual baseline and relieving layout congestion.
  * Shifted Games cards ($y = 127, 207, 287$) and System telemetry ($y = 92, 125, 158$) / buttons ($y = 217, 282, 347$) upward, providing over $70\text{ px}$ of clearance at the bottom of the System page.
* **Borderless Center Volume HUD Overlay**: Transformed the Volume HUD into a prominent 38pt (`font_chrono_time`) high-contrast borderless solid black circular badge overlaid directly over the center of the album art during volume adjustments.
* **Incomplete Page Out-of-Bounds Glitch Resolution**: Hardened bounds checks in `music_assistant_browse.yaml` (`length > 1` for Slot 2, `length > 2` for Slot 3), eliminating `UndefinedError` crashes on partial pages.
* **Native Library Categories & 3-Page Submenu**:
  * Renamed "RADIO STATIONS" to "RADIO" to align with native Music Assistant conventions.
  * Separated "PODCASTS & BOOKS" into two distinct native categories: "PODCASTS" (`media_type: podcast`) and "AUDIOBOOKS" (`media_type: audiobook`).
  * Expanded Level 2 library menu to 3 clean pages with bidirectional carousel wrap-around (Page 1 <-> Page 3).
* **Clock Face Vertical Battery Gauge**: Replaced the horizontal battery icon on Page 0 with an 18px vertical battery gauge featuring a top terminal pip and dynamic bottom-up charge level fill.
* **Display Brightness Control & Persistence Restoration**: Restored `set_action:` percentage handler to `display_brightness_preset`, re-asserted NVS brightness at boot priority -100, and eliminated sleep overrides.

### v3.4.8
* **Home Assistant Script Blueprint Architecture**: Decoupled the music browsing engine into a reusable Home Assistant Script Blueprint (`homeassistant/blueprints/script/music_assistant_browse.yaml`), allowing any ESPHome device to generate its own browsing script with automatic RPC target resolution.
* **Purge of Vestigial Substitutions**: Completely removed legacy presets (`preset_1_*`, `preset_2_*`, `preset_3_*`) and hardcoded speaker slots (`speaker_2_*`, `speaker_3_*`) from `pocket-assistant-1.75c.yaml` and `core/ui.yaml`. Replaced with single `local_player_id: "media_player.pocket_assistant"`.
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
