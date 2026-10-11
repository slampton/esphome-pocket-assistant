# ADR-004: Universal Television & Media Remote via remote.send_command

## Status
Accepted (v1.3)

## Context
Users require remote control capabilities for smart TVs, set-top boxes (Android TV, Apple TV, Roku, Fire TV), and AV receivers (Marantz, Denon) without needing custom, device-specific firmware builds or complex individual button entities.

## Decision
Implement a universal Remote Control application on Page 7 using Home Assistant's standard `remote.send_command` service. Configure the target entity via a single user substitution `tv_remote_entity: "remote.living_room_tv"`. 
Adopt a standardized command set:
* D-Pad: `up`, `down`, `left`, `right`, `center`
* Navigation: `back`, `home`
* Power: `power` (with confirmation modal dialog)
* Volume: `volume_up`, `volume_down` pass-through

## Consequences
* Out-of-the-box compatibility with Android TV, Google TV, Apple TV, Roku, and CEC-enabled receivers.
* No bloat in ESPHome configuration—one single substitution controls any remote entity.
* Safety against accidental TV power shutoff via a dedicated confirmation screen.
