# ADR-006: Hub-and-Spoke Home OS & 8-Tool Suite Consolidation

## Status
Accepted (v2.0)

## Context
As the platform expanded in Release v2.0, a linear horizontal `< / >` carousel across all applications and utilities created swipe fatigue. In addition, standalone timekeeping pages (Stopwatch and Timers/Alarms) occupied dedicated top-level slots despite being modular utilities.

## Decision
1. **Hub-and-Spoke OS Anchored on Home (`gui_page == 1`)**: Replace the linear bottom-chevron carousel with an interactive **Home Hub** featuring live 20 FPS weather, a transient 8-second wake greeting, direct tap zones for `Clock` (`Page 0`), `Sky & Weather` (`Page 5`), and `System Hub` (`Page 4`), a **Smart Live Activity Stack** (`y = 264`), and a **3-Icon Sliding Window Quick-Launch Dock** (`Music`, `Remote`, `Games` -> `Tools` at `y = 352`).
2. **2-Page 2×3 Tools Suite (`gui_page == 8`)**: Consolidate all 8 utility micro-apps into a 2-page $2 \times 3$ (6-per-page) grid with monochromatic filled-core iconography:
   * Page 1 (6 Tools): Timers (Subpage 1, 3-slot with Assist voice sync), Alarms (Subpage 2, 3-slot), Stopwatch (Subpage 3), Bubble Level (Subpage 7), Flashlight (Subpage 5), Calculator (Subpage 6).
   * Page 2 (2 Tools + Expansion Slots): Dice Roll (Subpage 4), Coin Flip (Subpage 8).
3. **Contextual `"Home"` vs. `"< Back"` Chin Navigation**: Top-level apps display `"Home"` at `y = 426` (`x in [155..311], y in [402..465]`), while nested submenus, micro-apps, and active games display `"< Back"`.

## Consequences
* Eliminates carousel swipe fatigue—every core app and active background tool is 1 tap away from the Home Hub.
* Provides a scalable 6-per-page utility launcher and a sliding-window Home Dock ready for v2.1 Room Control.