# ADR-003: Zero-Exception C sscanf Fallback Parser

## Status
Accepted (v1.2)

## Context
ESP-IDF builds for ESPHome run with C++ exceptions disabled (`-fno-exceptions`). Using `std::stof`, `std::stoi`, or `try/catch` causes immediate compiler termination. If Home Assistant weather attributes return `NaN` or `unavailable`, the weather forecast display would show invalid values.

## Decision
Implement a non-throwing fallback parser using C `sscanf` directly on the semicolon-delimited forecast string `DAY:HIGH:LOW:COND`. If `ha_weather_high` or `ha_weather_low` are `NaN`, the parser safely extracts the numeric values from the string without exceptions.

## Consequences
* Compile-time safety across all ESP-IDF versions.
* Resilient display rendering during Home Assistant weather entity reloads or network drops.
