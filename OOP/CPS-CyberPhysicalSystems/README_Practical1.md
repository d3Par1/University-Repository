# Practical 1 – Fundamentals of Working with Microcontrollers (Refactored SystemCore)

Demonstrates an integrated OOP microcontroller architecture with:
- Unified `SystemCore` module (namespaced `sys`) encapsulating devices, strategies, scenarios, energy management, and a command interface.
- Design patterns: Strategy (power modes), Polymorphism (scenarios, devices), Composition (group → LEDs), Command pattern–style serial parsing.
- Energy adaptation: inactivity detection + ambient-light influenced brightness + dynamic sampling rate.
- Runtime extensibility: serial commands to change scenarios and query status.

## Hardware (example wiring)
- Arduino UNO
- RGB LED (common cathode) pins → D9 (R), D10 (G), D11 (B) via resistors (e.g. 220Ω)
- Potentiometer → A0 (wiper), +5V, GND
- Photoresistor + 10k resistor voltage divider → A1 (junction). One side LDR to +5V; other side LDR to resistor; resistor other side to GND; junction to A1.

Adjust pins in `src/main.cpp` if different.

## Project Structure (Key)
```
platformio.ini
OOP/CPS-CyberPhysicalSystems/
  lib/
    SystemCore/
      SystemCore.h          <-- All cohesive system classes (interfaces, sensors, RGBLed, scenarios, group, energy manager, command interface)
    (legacy earlier headers retained for reference - not required by new main)
  src/
    main.cpp                <-- Uses SystemCore constructs only
  uml/
    Practical1.puml         <-- UML (legacy diagram; update optional)
```

## SystemCore Components
- `Config` – central configuration (pins, timings, thresholds, intervals).
- `IDevice` – uniform lifecycle interface.
- `PowerStrategy` + `NormalStrategy`, `LowPowerStrategy` – adaptive performance/power profile.
- `AnalogSensor` base + `Potentiometer`, `PhotoSensor` – normalized sensor inputs.
- `RGBLed` – easing color engine + HSV helper.
- `IScenario` + `StaticColorScenario`, `PulseScenario`, `RainbowScenario` – pluggable animations.
- `LedGroup` – multi-LED + scenario dispatcher (currently first LED drives animation pattern).
- `EnergyManager` – orchestrates strategy switching, sensor sampling, inactivity, ambient adaptation.
- `CommandInterface` – serial text command parsing (scenario & status control).

## Energy Optimization Logic
- Inactivity (no pot delta > 0.02 for `inactivityTimeoutMs`) switches to low power strategy.
- Low power reduces sensor interval and clamps max brightness.
- Ambient light thresholds: dark → boost brightness, bright → reduce.

## Scenario Rotation & Commands
- Automatic rotation cycles Pulse → Rainbow → Static White by `scenarioRotateMs`.
- Serial commands (send lines at 9600 baud):
  - `pulse` – switch to pulse scenario
  - `rainbow` – switch to rainbow scenario
  - `static R G B` – set static color (0–255 each)
  - `status` – print current runtime status

## UML Diagram
See `uml/Practical1.puml` (PlantUML). Render using VS Code PlantUML extension or online server.

## Build & Upload
PlatformIO tasks (status bar) or CLI:
```
pio run
pio run -t upload
pio device monitor
```
Ensure correct board or change in `platformio.ini` (`board = uno`).

## Serial Output (9600 baud)
Every `statusIntervalMs` prints:
```
Strategy=<Name> Pot=<0..1> Light=<0..1> InactiveMs=<ms> ScenarioIndex=<n>
```

## Configuration & Extending
- Change defaults: edit the `Config cfg;` in `main.cpp` before `setup()` finishes (or add a function to override serially).
- Add LED instances: create more `RGBLed` objects and call `group.add(&ledN);` before `group.init()`.
- Add new animation: derive from `IScenario` and implement `apply()`.
- Add power modes: derive from `PowerStrategy` and plug into `EnergyManager::attachStrategies` (extend to array if multiple modes).
- Increase complexity: separate classes into `.cpp` files if code size grows; keep single-header form for educational clarity.

## Testing Without Hardware
- Stub `analogRead` / `millis` in a host test harness (wrap with `#ifndef ARDUINO`).
- Add a compile flag to inject synthetic sensor patterns.

## Academic Notes
Illustrates layered abstraction, cohesive module design, strategy-driven adaptation, runtime polymorphism, and command-driven configurability.

## License
See root `LICENSE`.
