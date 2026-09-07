#include <Arduino.h>
#include <FastLED.h>

namespace {

constexpr uint8_t kRelayPin = 26;
constexpr uint8_t kLedPin = 27;

CRGB statusLed[1];

// The safe startup state leaves COM connected to NC.
bool relayEnabled = false;

void setRelay(bool enabled) {
  digitalWrite(kRelayPin, enabled ? HIGH : LOW);
  statusLed[0] = enabled ? CRGB::Red : CRGB::Green;
  FastLED.show();

  Serial.printf("Relay %s: COM-%s\n", enabled ? "ON" : "OFF",
                enabled ? "NO closed" : "NC closed");
}

}  // namespace

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("camgate safe standby: COM-NC closed");

  pinMode(kRelayPin, OUTPUT);
  FastLED.addLeds<SK6812, kLedPin, GRB>(statusLed, 1);
  FastLED.setBrightness(64);
  setRelay(relayEnabled);
}

void loop() {}
