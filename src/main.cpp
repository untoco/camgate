#include <Arduino.h>

namespace {

constexpr uint32_t kRelayIntervalMs = 5000;

// Atom Lite's native Grove DIN is GPIO26. Atomic ToUnit Base can route its
// Grove signal to one of these Atom Lite pins with its DIP switches. During
// the bench test all candidates change together; afterwards keep only the
// pin confirmed by the actual base wiring.
constexpr uint8_t kRelayPins[] = {26, 19, 22, 23, 33};

bool relayEnabled = false;
uint32_t lastChangeMs = 0;

void setRelay(bool enabled) {
  for (const uint8_t pin : kRelayPins) {
    digitalWrite(pin, enabled ? HIGH : LOW);
  }

  Serial.printf("Relay: %s\n", enabled ? "ON" : "OFF");
}

}  // namespace

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("camgate relay test starting");

  for (const uint8_t pin : kRelayPins) {
    pinMode(pin, OUTPUT);
  }

  setRelay(false);
  lastChangeMs = millis();
}

void loop() {
  if (millis() - lastChangeMs >= kRelayIntervalMs) {
    relayEnabled = !relayEnabled;
    setRelay(relayEnabled);
    lastChangeMs += kRelayIntervalMs;
  }
}
