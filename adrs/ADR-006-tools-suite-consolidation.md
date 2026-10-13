# ADR-006: Tools Suite 2-Page 8-Icon Grid Architecture & Carousel Consolidation

## Status
Accepted (v1.3)

## Context
As the platform grew in Release v1.3, the addition of new utility tools risked bloating the top-level horizontal App Deck carousel from 7 apps to 9+ apps, creating excessive swipe fatigue. In addition, standalone timekeeping pages (Stopwatch on Page 1, Timer & Alarm on Page 6) occupied dedicated top-level carousel slots despite being modular utilities.

## Decision
1. **Consolidate Carousel to 7 Core Apps**: Retire standalone top-level allocations for Stopwatch (gui_page == 1) and Timer & Alarm (gui_page == 6), re-routing them permanently into a unified **Tools Suite** on Page 8 (gui_page == 8). Streamline the primary carousel to 7 focused applications: Clock (0), Music (2), TV Remote (7), Sky & Weather (5), Games (3), Tools (8), and System Hub (4).
2. **2-Page 2x3 Launcher Architecture**: Organize Tools into an expandable 2-page grid featuring 8 canonical micro-apps:
   * Page 1: Timer (Subpage 1), Alarms (Subpage 2), Stopwatch (Subpage 3), Dice Roll (Subpage 4), Flashlight (Subpage 5), Calculator (Subpage 6).
   * Page 2: Bubble Level (Subpage 7), Coin Flip (Subpage 8).
3. **Right-Flank Chevron Navigation**: Navigate between Tools pages using standardized right-flank chevrons (x = 415) matching the OS standard established in ADR-005.
4. **Universal Full-Chin Back Navigation**: Provide a standardized < BACK button across all subpages at y = 408, with hit testing extending across the entire lower bezel chin (y in [385..465]).

## Consequences
* Preserves a lean, rapid 7-app top-level carousel while expanding utility capacity to 8+ micro-apps.
* Retains 100% backward-compatibility via transparent redirect guards for legacy page allocations.
* Establishes a scalable framework for future utility micro-apps without carousel sprawl.
