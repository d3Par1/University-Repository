#include <Arduino.h>

int red = 10;
int yellow = 9;
int green = 8;

void setup() {
  pinMode(red, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(green, OUTPUT);
}

void loop() {
  // Red
  digitalWrite(red, HIGH);
  digitalWrite(yellow, LOW);
  digitalWrite(green, LOW);
  delay(5000);

  // Red + Yellow
  digitalWrite(yellow, HIGH);
  delay(1500);

  // Green
  digitalWrite(red, LOW);
  digitalWrite(yellow, LOW);
  digitalWrite(green, HIGH);
  delay(3500);

  // Green blink
  for (int i = 0; i < 3; i++) {
    digitalWrite(green, LOW);
    delay(250);
    digitalWrite(green, HIGH);
    delay(250);
  }

  // Yellow
  digitalWrite(green, LOW);
  digitalWrite(yellow, HIGH);
  delay(2000);
  digitalWrite(yellow, LOW);
}
