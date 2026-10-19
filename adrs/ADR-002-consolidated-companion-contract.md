# ADR-002: Consolidated 2-Sensor Companion Contract

## Status
Accepted (v1.2)

## Context
Earlier releases required 12 separate Home Assistant helper entities and template sensors (3 for weather and 9 for media player mirroring). During active music playback, this generated 9 distinct database commits every 2 seconds, placing unnecessary load on the Home Assistant recorder and SQLite database.

## Decision
Consolidate the 12 entities into a single package (`pocket_assistant_companion.yaml`) exposing 3 input_text helpers, 1 embedded Music Assistant browse script (script.pocket_assistant_music_browse), and 2 primary sensors:
1. `sensor.pocket_assistant_weather_forecast` (State: 5-day string, Attributes: `high`, `low`)
2. `sensor.pocket_assistant_active_media` (State: playback state, Attributes: metadata & transport)

## Consequences
* Reduces Home Assistant recorder write volume by 89%.
* Simplifies user setup from 12 entities to 1 drop-in package.
* Synchronous metadata delivery to ESPHome via native API attribute mapping.
