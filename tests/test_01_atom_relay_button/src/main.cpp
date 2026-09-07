#include <Arduino.h>
#include <FastLED.h>

namespace {

constexpr uint8_t kRelayPin = 26;
constexpr uint8_t kButtonPin = 39;
constexpr uint8_t kLedPin = 27;
constexpr uint32_t kDebounceMs = 40;

CRGB statusLed[1];

// The safe startup state leaves COM connected to NC.
bool relayEnabled = false;
bool lastRawButtonPressed = false;
bool stableButtonPressed = false;
uint32_t lastButtonChangeMs = 0;

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
  Serial.println("camgate button relay test starting");

  pinMode(kRelayPin, OUTPUT);
  pinMode(kButtonPin, INPUT);
  FastLED.addLeds<SK6812, kLedPin, GRB>(statusLed, 1);
  FastLED.setBrightness(64);
  setRelay(relayEnabled);
}

void loop() {
  const bool rawButtonPressed = digitalRead(kButtonPin) == LOW;

  if (rawButtonPressed != lastRawButtonPressed) {
    lastRawButtonPressed = rawButtonPressed;
    lastButtonChangeMs = millis();
  }

  if (millis() - lastButtonChangeMs >= kDebounceMs &&
      rawButtonPressed != stableButtonPressed) {
    stableButtonPressed = rawButtonPressed;

    if (stableButtonPressed) {
      relayEnabled = !relayEnabled;
      setRelay(relayEnabled);
    }
  }
}
