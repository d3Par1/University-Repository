# Pico W 3-LED Traffic Light

This example runs on Raspberry Pi Pico W using the Arduino framework via PlatformIO. It cycles Red → Green → Yellow using three GPIO pins.

- Red: GP2
- Yellow: GP3
- Green: GP4
- Serial: 115200 baud

Wiring
- Connect each LED through a 220–330 Ω resistor to GND.
- Connect the anode of each LED to its GPIO pin.

Build & Upload (VS Code Tasks)
- Use: "PIO PicoW: Build+Upload" or build env `rpipicow_traffic`.

Build & Upload (CLI)
- `platformio run -e rpipicow_traffic`
- Hold BOOTSEL, plug Pico W, release; then
- `platformio run -e rpipicow_traffic -t upload`

Customize Pins
- Define `LED_RED_PIN`, `LED_YELLOW_PIN`, `LED_GREEN_PIN` in `platformio.ini` build flags or at top of `src/main.cpp`.
