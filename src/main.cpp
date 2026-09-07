#include <Arduino.h>

namespace {

constexpr uint32_t kRelayIntervalMs = 10000;

// Atom Lite's native Grove DIN is GPIO26. Atomic ToUnit Base can route its
// Grove signal to one of these Atom Lite pins with its DIP switches. The
// diagnostic test enables one candidate at a time, so a multimeter can reveal
// which pin is actually wired to the relay.
constexpr uint8_t kRelayPins[] = {26, 19, 22, 23, 33};

size_t activePinIndex = 0;
uint32_t lastChangeMs = 0;

void disableAllRelayPins() {
  for (const uint8_t pin : kRelayPins) {
    digitalWrite(pin, LOW);
  }
}

void testNextPin() {
  disableAllRelayPins();
  const uint8_t pin = kRelayPins[activePinIndex];
  digitalWrite(pin, HIGH);
  Serial.printf("Testing GPIO%u: COM and NO should be closed for 10 seconds\n",
                pin);

  activePinIndex = (activePinIndex + 1) % (sizeof(kRelayPins) / sizeof(kRelayPins[0]));
}

}  // namespace

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("camgate relay test starting");

  for (const uint8_t pin : kRelayPins) {
    pinMode(pin, OUTPUT);
  }

  disableAllRelayPins();
  testNextPin();
  lastChangeMs = millis();
}

void loop() {
  if (millis() - lastChangeMs >= kRelayIntervalMs) {
    testNextPin();
    lastChangeMs += kRelayIntervalMs;
  }
}
