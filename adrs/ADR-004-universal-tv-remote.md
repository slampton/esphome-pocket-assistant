# ADR-004: Universal Television & Media Remote via remote.send_command

## Status
Accepted (v1.3)

## Context
Users require remote control capabilities for smart TVs, set-top boxes (Android TV, Apple TV, Roku, Fire TV), and AV receivers (Marantz, Denon) without needing custom, device-specific firmware builds, complex individual button entities, or extra state-tracking helpers.

Initial on-device physical testing highlighted that blind thumb navigation across the horizontal D-pad axis frequently triggered false Volume Down presses due to insufficient vertical separation along the screen equator. Additionally, displaying an explicit top power icon conflicted with standard page header typography, and an off-state device triggered an asymmetric "Power Off?" confirmation prompt.

## Decision
1. **Universal Protocol Dispatch**:
   * Implement a universal Remote Control application on Page 7 using Home Assistant's standard `remote.send_command` service.
   * Configure the target entity via a single user substitution: `tv_remote_entity: "remote.tv"`.
   * Standardize on canonical command substitutions (`remote_cmd_up`, `remote_cmd_down`, `remote_cmd_left`, `remote_cmd_right`, `remote_cmd_select`, `remote_cmd_back`, `remote_cmd_home`, `remote_cmd_vol_up`, `remote_cmd_vol_down`, `remote_cmd_power`).

2. **4-Corner Radial Geometry & Equatorial Dead-Zone**:
   * Lower the bottom flank buttons (**Back** and **Volume Down**) down to $y = 326$ so they symmetrically mirror the top flank buttons (**Home** and **Volume Up** at $y = 140$) across the circular display equator ($cy = 233$, $\Delta y = \pm 93\text{px}$, $x = 71$ and $x = 395$, $r \approx 187\text{px}$).
   * Introduce an $85\text{px}$ vertical dead-zone ($y \in [191, 275]$) completely devoid of flank touch targets, protecting the horizontal D-pad axis against accidental volume adjustments during eyes-free operation.
   * Expand the outer D-pad radius from $124\text{px}$ to $132\text{px}$ (directional touch zone expanded to radius $148\text{px}$, $d^2 \le 21904$) for forgiving directional thumb acquisition.

3. **Compact Header & Universal Power Toggle**:
   * Standardize header typography to match Compact Music Player formatting: Title `"REMOTE"` at $y = 50$ (`font_date`, `theme_primary`), Subtitle `"Television"` at $y = 74$ (`font_small`, `theme_secondary`), and transient feedback toasts at $y = 74$.
   * Remove the standalone visible power icon. Tapping the header area ($x \in [120, 346], y \in [25, 95]$) triggers the Power Toggle modal.
   * Standardize confirmation prompt to a universal toggle contract: `"TOGGLE POWER?"` with subtitle `"Send power command to TV"` and dual-action `[ Cancel ]` / `[ Toggle ]` buttons at $y = 230$.
   * Hardware crown press immediately cancels and dismisses the confirmation modal.

## Consequences
* Out-of-the-box compatibility with Android TV, Google TV, Apple TV, Roku, Fire TV, and CEC-enabled AV receivers.
* Zero configuration bloat in Home Assistant or ESPHome—no additional state-tracking entities required.
* Complete elimination of false volume presses during blind D-pad navigation.
* Consistent horological visual design adhering to the circular AMOLED layout guidelines.
