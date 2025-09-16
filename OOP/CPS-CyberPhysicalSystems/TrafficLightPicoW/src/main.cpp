#include <Arduino.h>

// GPIO pins for the three LEDs (Pico W uses GP numbers)
#ifndef LED_RED_PIN
#define LED_RED_PIN 2
#endif
#ifndef LED_YELLOW_PIN
#define LED_YELLOW_PIN 3
#endif
#ifndef LED_GREEN_PIN
#define LED_GREEN_PIN 4
#endif

enum class Phase : uint8_t { Red, Green, Yellow };

struct PhaseInfo { Phase phase; unsigned long durationMs; };

// Timings (adjust as needed)
static const PhaseInfo kSequence[] = {
    { Phase::Red,    5000 },
    { Phase::Green,  5000 },
    { Phase::Yellow, 2000 }
};
static const uint8_t kSeqLen = sizeof(kSequence)/sizeof(kSequence[0]);

static uint8_t seqIndex = 0;
static unsigned long phaseStart = 0;

static void applyPhase(Phase p) {
  // Active-high LEDs with series resistors to GND
  switch (p) {
    case Phase::Red:
      digitalWrite(LED_RED_PIN, HIGH);
      digitalWrite(LED_YELLOW_PIN, LOW);
      digitalWrite(LED_GREEN_PIN, LOW);
      break;
    case Phase::Green:
      digitalWrite(LED_RED_PIN, LOW);
      digitalWrite(LED_YELLOW_PIN, LOW);
      digitalWrite(LED_GREEN_PIN, HIGH);
      break;
    case Phase::Yellow:
      digitalWrite(LED_RED_PIN, LOW);
      digitalWrite(LED_YELLOW_PIN, HIGH);
      digitalWrite(LED_GREEN_PIN, LOW);
      break;
  }
}

void setup() {
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_YELLOW_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);

  Serial.begin(115200);
  delay(50);
  Serial.println("Pico W Traffic Light (3 LEDs)");
  Serial.println("Pins: RED=GP2, YELLOW=GP3, GREEN=GP4");

  seqIndex = 0;
  phaseStart = millis();
  applyPhase(kSequence[seqIndex].phase);
}

void loop() {
  unsigned long now = millis();
  const PhaseInfo &cur = kSequence[seqIndex];
  if (now - phaseStart >= cur.durationMs) {
    seqIndex = (seqIndex + 1) % kSeqLen;
    phaseStart = now;
    applyPhase(kSequence[seqIndex].phase);
    Serial.print("Phase -> ");
    switch (kSequence[seqIndex].phase) {
      case Phase::Red: Serial.println("RED"); break;
      case Phase::Green: Serial.println("GREEN"); break;
      case Phase::Yellow: Serial.println("YELLOW"); break;
    }
  }
}
