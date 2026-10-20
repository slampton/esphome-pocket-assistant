# ADR-005: Flank Chevron and Low-Profile Chin Back Navigation for Circular Displays

## Status
Accepted (v2.0)

## Context
Earlier designs for multi-page apps and submenus used stacked pill buttons or pagination dots across the bottom of the screen. On a 466×466 circular AMOLED display, bottom pill buttons and page indicators occupy up to 80px of vertical space in the narrowest part of the round viewport, constraining content layout.

## Decision
Adopt a uniform, high-density navigation pattern across all multi-page apps and submenus:
1. **Vertical Pagination**: Right-flank vertical chevrons (`▲` Up / `▼` Down) at `x = 415` (`y = 162` and `y = 278`), utilizing the wider horizontal chord of the circular display.
2. **Universal Low-Profile `< BACK` (`y = 426`)**: Render `< BACK` at `y = 426` in crisp white typography with a 1px black drop shadow (zero pill border, zero pagination dots/numbers), with full-chin touch hit-testing (`y in [385..465]`, `x in [50..416]`).

## Consequences
* Reclaims ~80px of vertical display height for primary telemetry, cards, and controls.
* Delivers a clean, uncluttered navigation language across the entire operating system.