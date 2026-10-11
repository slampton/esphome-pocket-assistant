# ADR-004: Universal Television & Media Remote via remote.send_command

## Status
Accepted (v1.3)

## Context
Users require remote control capabilities for smart TVs, set-top boxes (Android TV, Apple TV, Roku, Fire TV), and AV receivers (Marantz, Denon) without needing custom, device-specific firmware builds, complex individual button entities, or extra state-tracking helpers.

On-device physical validation revealed three UX opportunities:
1. Blind thumb navigation across the center D-pad occasionally missed the OK button and triggered directional presses (specifically Right) due to a conservative center touch radius ($44\text{px}$).
2. Stacking a static "REMOTE" title above a secondary entity subtitle crowded the upper hemisphere and departed from our standard single-header design.
3. Prompting for confirmation on every power press felt redundant if the action could instead reside on an intentional, dedicated settings subpage modeled after our system dashboard ergonomics.

## Decision
1. **Universal Protocol Dispatch**:
   * Implement a universal Remote Control application on Page 7 using Home Assistant's standard `remote.send_command` service.
   * Configure the target entity via a single user substitution: `tv_remote_entity: "remote.tv"`.
   * Standardize on canonical command substitutions (`remote_cmd_up`, `remote_cmd_down`, `remote_cmd_left`, `remote_cmd_right`, `remote_cmd_select`, `remote_cmd_back`, `remote_cmd_home`, `remote_cmd_vol_up`, `remote_cmd_vol_down`, `remote_cmd_power`).

2. **4-Corner Radial Geometry & Equatorial Dead-Zone**:
   * Lower the bottom flank buttons (**Back** and **Volume Down**) down to $y = 326$ so they symmetrically mirror the top flank buttons (**Home** and **Volume Up** at $y = 140$) across the circular display equator ($cy = 233$, $\Delta y = \pm 93\text{px}$, $x = 71$ and $x = 395$, $r \approx 187\text{px}$).
   * Introduce an $85\text{px}$ vertical dead-zone ($y \in [191, 275]$) completely devoid of flank touch targets, protecting the horizontal D-pad axis against accidental volume adjustments during eyes-free operation.
   * Expand the outer D-pad radius from $124\text{px}$ to $132\text{px}$ (directional touch zone: $52\text{px} < r \le 148\text{px}$, $2704 < d^2 \le 21904$).

3. **Expanded OK Center Touch Target**:
   * Expand the center OK button touch radius from $44\text{px}$ to $52\text{px}$ ($d^2 \le 2704$, a +40% touch area increase to a full $104\text{px}$ diameter) while preserving a balanced $40\text{px}$ visual radius.
   * Eliminates false directional triggering when pressing OK.

4. **Unified Entity Header & Device Settings Subpage (Subpage 1)**:
   * **Main Page Title ($y = 50$)**: Replace the static "REMOTE" header and secondary subtitle line with the active device name (e.g. "Shield TV" or "TV") rendered in `font_date`, `theme_primary`, freeing vertical breathing room above the D-pad.
   * **Transient Action Feedback**: Action feedback toasts (`Toggle Power`, `Vol +`, `Up`, etc.) display in place of the title at $y = 50$ for 1.5 seconds after a button tap before returning to the device title.
   * **Device Settings Subpage**: Tapping the title header ($x \in [110, 356], y \in [25, 80]$) opens Subpage 1 (`remote_subpage = 1`).
   * **System Dashboard Visual Hierarchy**: Header models the System Hub typography and spacing: Title `"DEVICE SETTINGS"` at $y = 46$ (`font_date`, `theme_primary`), active device name at $y = 78$ (`font_small`, `col_white`), providing an exact $32\text{px}$ vertical buffer.
   * **Direct Power Toggle**: Rendered via standard `draw_pill_button` at $y = 200$ (h = 44px, r = 22px, red accent outline with zero internal graphical artifacts) for immediate power toggling without redundant confirmation prompts.
   * **Navigation**: Returns to the main remote interface via bottom `[ Back ]` pill button ($y = 356$), top header tap, or hardware crown button press.

## Consequences
* Out-of-the-box compatibility with Android TV, Google TV, Apple TV, Roku, Fire TV, and CEC-enabled AV receivers.
* Main title dynamically reflects the user's specific target device (e.g., "Shield TV").
* Direct power toggling from the settings subpage eliminates modal interruptions.
* Clean architectural foundation for interactive multi-device switching without breaking changes.
* Complete elimination of false volume presses and missed center OK button taps.
* Clean, uncluttered AMOLED aesthetics conforming to project design standards.
