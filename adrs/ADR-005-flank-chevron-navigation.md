# ADR-005: Flank Chevron and Bottom Pip Navigation for Circular Displays

## Status
Accepted (v2.0)

## Context
Earlier designs for multi-page apps (Sky & Weather, Timer & Alarm) used 2x2 or 2x1 pill buttons across the bottom of the screen. On a 466x466 circular AMOLED display, bottom pill buttons occupy up to 80px of vertical space in the narrowest part of the round viewport, constraining content layout and breaking visual consistency with the Music and Games carousels.

## Decision
Adopt a uniform UI navigation pattern across all multi-page apps:
1. Primary navigation: Right-flank vertical chevrons (▲ Up / ▼ Down) at x=415, utilizing the wider horizontal diameter.
2. Subpage indication: Discrete pagination dots/pips along the bottom curve (y=440), doubling as optional tap targets.

## Consequences
* Reclaims ~80px of vertical display height for primary telemetry and controls.
* Delivers a unified, consistent navigation language across the entire operating system.
