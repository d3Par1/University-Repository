# Examples

- Customize timings: edit `kSequence` durations in `src/main.cpp`.
- Change pins via build flags:

```
; in platformio.ini under [env:rpipicow_traffic]
build_flags = -DLED_RED_PIN=6 -DLED_YELLOW_PIN=7 -DLED_GREEN_PIN=8
```
