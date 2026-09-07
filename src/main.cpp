#include <Arduino.h>

namespace {

constexpr uint8_t kRelayPin = 26;
constexpr uint32_t kRelayIntervalMs = 5000;

bool relayEnabled = true;
uint32_t lastChangeMs = 0;

void setRelay(bool enabled) {
  digitalWrite(kRelayPin, enabled ? HIGH : LOW);
  Serial.printf("Relay %s: COM-%s\n", enabled ? "ON" : "OFF",
                enabled ? "NO closed" : "NC closed");
}

}  // namespace

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("camgate direct Grove relay test starting");

  pinMode(kRelayPin, OUTPUT);
  setRelay(relayEnabled);
  lastChangeMs = millis();
}

void loop() {
  if (millis() - lastChangeMs >= kRelayIntervalMs) {
    relayEnabled = !relayEnabled;
    setRelay(relayEnabled);
    lastChangeMs += kRelayIntervalMs;
  }
}
