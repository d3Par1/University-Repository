# Practical 1: Cyber-Physical System (Sensors + RGB LED) with OOP & Power Strategies

UA: Проєкт демонструє використання ООП, бібліотечного підходу, патерну Strategy (енергозбереження) для взаємодії мікроконтролера з потенціометром, фоторезистором (LDR) та RGB світлодіодом.

## Features
- Абстрактний базовий клас `Device` + менеджер `DeviceManager`.
- Уніфіковані сенсори: `Potentiometer`, `PhotoSensor` (усереднення показів, швидке опитування).
- Керування RGB світлодіодом з плавним переходом кольорів.
- Патерн Strategy для режимів живлення: `NormalPowerStrategy`, `LowPowerStrategy`.
- Обмеження максимальної яскравості у режимі енергозбереження.
- PlantUML діаграма класів (`uml/classes.puml`).
- Розширюваність: легко додати нові пристрої або стратегії.

## Hardware (Arduino Uno приклад)
| Component | Pin |
|-----------|-----|
| Potentiometer wiper | A0 |
| Photoresistor divider output | A1 |
| RGB LED Red | 9 (PWM) |
| RGB LED Green | 10 (PWM) |
| RGB LED Blue | 11 (PWM) |
| GND | Common ground |

Photoresistor: створіть подільник напруги (LDR + резистор 10kΩ). Вузол між ними -> A1.

## Directory Structure
```
CPS-CyberPhysicalSystems/
  lib/               # Класи пристроїв та стратегії
  examples/Pr1_Demo/ # Головний скетч
  uml/               # UML (PlantUML)
  docs/              # (майбутня документація / звіт)
```

## Class Overview
- `Device`: чисто віртуальний інтерфейс.
- `DeviceManager`: зберігає масив вказівників на `Device`, ініціалізує та оновлює їх.
- `AnalogSensorBase`: базовий клас для аналогових сенсорів із згладжуванням (усереднення N семплів).
- `Potentiometer`: нормалізоване значення 0..1.
- `PhotoSensor`: рівень освітлення 0..1.
- `RGBLed`: встановлення кольору, яскравості, плавне перехідне оновлення.
- `PowerStrategy`: інтерфейс; реалізації визначають інтервал опитування сенсорів і максимальну яскравість.

## Power / Energy Saving
Normal:
- Частота опитування: 50 ms
- Макс. яскравість LED: 100%

LowPower:
- Частота опитування: 200 ms
- Макс. яскравість LED: 40%

Перемикання режимів через Serial: введіть `n` або `l`.

## Example Logic (Pr1_Demo)
1. Зчитується значення потенціометра -> масштабування яскравості (обмежене стратегією).
2. Освітленість (LDR) керує кольором: темніше довкілля -> червоніший/тепліший відтінок.
3. Плавні переходи кольорів через easing.
4. Щосекунди надсилається статус усіх пристроїв у Serial Monitor.

## Building / Upload
1. Відкрийте Arduino IDE.
2. Скопіюйте папку `CPS-CyberPhysicalSystems` у вашу робочу директорію або додайте шлях як проект.
3. Відкрийте `examples/Pr1_Demo/Pr1_Demo.ino`.
4. Виберіть плату (Arduino Uno / Raspberry Pi Pico (Arduino core)).
5. Завантажте скетч.
6. Відкрийте Serial Monitor @ 115200 baud.

(Для PlatformIO) Створіть `platformio.ini` (не додано тут) з необхідною платформою та додайте `lib/` до include path.

## UML Diagram
Файл: `uml/classes.puml`
Візуалізація (локально):
```
java -jar plantuml.jar uml/classes.puml
```
Або використайте онлайн редактор PlantUML.

## Extending
Додайте нову стратегію:
```cpp
class UltraLowPowerStrategy : public PowerStrategy {
 public:
  const char* name() const override { return "UltraLow"; }
  uint16_t sensorIntervalMs() const override { return 1000; }
  float ledMaxBrightness() const override { return 0.2f; }
};
```

Додайте новий сенсор: створіть клас, що наслідує `Device` або `AnalogSensorBase` і додайте до `DeviceManager`.

## Possible Improvements
- Додати SPI/I2C сенсори (температура, IMU).
- Зберігати конфігурацію в EEPROM.
- Патерн Observer для подій зміни значень.
- Юніт-тести (для не-апаратної логіки) з використанням симулятора.
- FSM (стан-машина) для сценаріїв освітлення.

## License
Освітній приклад у межах навчального репозиторію.

---
EN: This project showcases OOP, a lightweight library layout, Strategy pattern for power profiles, and integration of analog sensors driving an RGB LED.

Happy hacking!
