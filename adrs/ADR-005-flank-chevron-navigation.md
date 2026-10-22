# ADR-005: Flank Chevron and Low-Profile Chin Back Navigation for Circular Displays

## Status
Accepted (v2.0)

## Context
Earlier designs for multi-page apps and submenus used stacked pill buttons or pagination dots across the bottom of the screen. On a 466×466 circular AMOLED display, bottom pill buttons and page indicators occupy up to 80px of vertical space in the narrowest part of the round viewport, constraining content layout.

## Decision
Adopt a uniform, high-density navigation pattern across all multi-page apps and submenus:
1. **Vertical Pagination**: Right-flank vertical chevrons (`▲` Up / `▼` Down) at `x = 415` (`y = 162` and `y = 278`), utilizing the wider horizontal chord of the circular display.
2. **Contextual Low-Profile `Home` / `< Back` (`y = 426`)**: Render `Home` (on top-level apps) or `< Back` (on nested submenus, tools, and active games) at `y = 426` in slate gray (`#9E9E9E`) with a 1px black drop shadow, paired with a focused chin touch zone (`y in [405..465]`, `x in [140..326]`) so bottom-row menu cards extend down to `y < 405` with zero dead zone.

## Consequences
* Reclaims ~80px of vertical display height for primary telemetry, cards, and controls.
* Delivers a clean, uncluttered navigation language across the entire operating system.