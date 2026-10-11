# ADR-001: Hardware PMU Interrupt on GPIO3 for Low-Latency Wake

## Status
Accepted (v1.2)

## Context
Previously, standby wake and button detection relied on periodic I2C polling of the AXP2101 power management IC at 10 Hz. This prevented the ESP32-S3 from maintaining deep light sleep, consumed excess battery power (~35mA standby), and introduced up to 1,000ms latency when pressing the crown button to wake the screen.

## Decision
Connect and configure the AXP2101 hardware IRQ pin to ESP32-S3 `GPIO3`. On standby, the CPU enters light sleep and wakes immediately when the AXP2101 asserts the IRQ line.

## Consequences
* Wake latency reduced from ~1,000ms to < 5ms.
* Standby current draw reduced significantly by eliminating continuous I2C bus traffic.
* Responsive tactile feel on crown button presses.
