# ADR-006: Hub-and-Spoke Home OS & 8-Tool Suite Consolidation

## Status
Accepted (v2.0)

## Context
As the platform expanded in Release v2.0, a linear horizontal `< / >` carousel across all applications and utilities created swipe fatigue. In addition, standalone timekeeping pages (Stopwatch and Timers/Alarms) occupied dedicated top-level slots despite being modular utilities.

## Decision
1. **Hub-and-Spoke OS Anchored on Home (`gui_page == 1`)**: Replace the linear bottom-chevron carousel with an interactive **Home Hub** featuring live 20 FPS weather, direct tap zones for `Clock` (`Page 0`), `Sky & Weather` (`Page 5`), and `System Hub` (`Page 4`), a **Smart Live Activity Stack** (`y = 264`), and an auto-centering 4-icon **Quick-Launch Dock** (`Music`, `Remote`, `Tools`, `Games` at `y = 356`).
2. **2-Page 2×2 Tools Suite (`gui_page == 8`)**: Consolidate all 8 utility micro-apps into a 2-page $2 \times 2$ grid with monochromatic filled-core iconography:
   * Page 1: Timers (Subpage 1, 3-slot with Assist voice sync), Alarms (Subpage 2, 3-slot), Flashlight (Subpage 5), Calculator (Subpage 6).
   * Page 2: Stopwatch (Subpage 3), Dice Roll (Subpage 4), Bubble Level (Subpage 7), Coin Flip (Subpage 8).
3. **Right-Flank Chevron & Full-Chin Back Navigation**: Navigate between Tools launcher pages using right-flank chevrons (`x = 415`), and return to the parent launcher or Home Hub via the universal `< BACK` chin target (`y in [385..465]`) or a short press of the physical Top Crown button.

## Consequences
* Eliminates carousel swipe fatigue—every core app and active background tool is 1 tap away from the Home Hub.
* Establishes a clean, scalable framework for utility micro-apps inside the Tools Suite.