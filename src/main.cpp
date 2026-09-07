#include <Arduino.h>
#include <FastLED.h>

#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

namespace {

constexpr char kDeviceName[] = "camgate";
constexpr char kServiceUuid[] = "72269b15-38ee-4303-9255-003b4a9d2410";
constexpr char kCommandUuid[] = "72269b15-38ee-4303-9255-003b4a9d2411";
constexpr char kStateUuid[] = "72269b15-38ee-4303-9255-003b4a9d2412";

constexpr uint8_t kRelayPin = 26;
constexpr uint8_t kButtonPin = 39;
constexpr uint8_t kLedPin = 27;
constexpr uint32_t kDebounceMs = 40;

constexpr uint8_t kCommandPowerOn = 0x00;
constexpr uint8_t kCommandPowerOff = 0x01;
constexpr uint8_t kCommandTogglePower = 0x02;

CRGB statusLed[1];
BLECharacteristic* stateCharacteristic = nullptr;

// Startup de-energises the relay: COM -> NC feeds power to the GoPro adapter.
bool relayEnabled = false;
bool phoneConnected = false;
bool lastRawButtonPressed = false;
bool stableButtonPressed = false;
uint32_t lastButtonChangeMs = 0;
volatile int8_t pendingCommand = -1;

void publishRelayState() {
  if (stateCharacteristic == nullptr) {
    return;
  }

  const uint8_t state = relayEnabled ? 0x01 : 0x00;
  stateCharacteristic->setValue(const_cast<uint8_t*>(&state), 1);
  if (phoneConnected) {
    stateCharacteristic->notify();
  }
}

void setRelay(bool enabled, const char* source) {
  relayEnabled = enabled;
  digitalWrite(kRelayPin, relayEnabled ? HIGH : LOW);
  statusLed[0] = relayEnabled ? CRGB::Red : CRGB::Green;
  FastLED.show();
  publishRelayState();

  Serial.printf("Relay %s via %s: COM-%s\n", relayEnabled ? "ON" : "OFF",
                source, relayEnabled ? "NO closed" : "NC closed");
}

class ServerCallbacks final : public BLEServerCallbacks {
  void onConnect(BLEServer*) override {
    phoneConnected = true;
    Serial.println("BLE client connected");
  }

  void onDisconnect(BLEServer*) override {
    phoneConnected = false;
    BLEDevice::startAdvertising();
    Serial.println("BLE client disconnected; advertising restarted");
  }
};

class CommandCallbacks final : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* characteristic) override {
    const std::string value = characteristic->getValue();
    if (value.size() != 1) {
      Serial.println("BLE command rejected: expected one byte");
      return;
    }

    const uint8_t command = static_cast<uint8_t>(value[0]);
    if (command > kCommandTogglePower) {
      Serial.printf("BLE command rejected: 0x%02X\n", command);
      return;
    }

    pendingCommand = static_cast<int8_t>(command);
  }
};

void startBle() {
  BLEDevice::init(kDeviceName);
  BLEServer* server = BLEDevice::createServer();
  server->setCallbacks(new ServerCallbacks());

  BLEService* service = server->createService(kServiceUuid);
  BLECharacteristic* commandCharacteristic = service->createCharacteristic(
      kCommandUuid, BLECharacteristic::PROPERTY_WRITE |
                        BLECharacteristic::PROPERTY_WRITE_NR);
  commandCharacteristic->setCallbacks(new CommandCallbacks());

  stateCharacteristic = service->createCharacteristic(
      kStateUuid, BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_NOTIFY);
  stateCharacteristic->addDescriptor(new BLE2902());
  publishRelayState();

  service->start();
  BLEAdvertising* advertising = BLEDevice::getAdvertising();
  advertising->addServiceUUID(kServiceUuid);
  advertising->setScanResponse(true);
  BLEDevice::startAdvertising();
  Serial.println("BLE advertising as camgate");
}

void processBleCommand() {
  const int8_t command = pendingCommand;
  if (command < 0) {
    return;
  }

  pendingCommand = -1;
  switch (command) {
    case kCommandPowerOn:
      setRelay(false, "BLE");
      break;
    case kCommandPowerOff:
      setRelay(true, "BLE");
      break;
    case kCommandTogglePower:
      setRelay(!relayEnabled, "BLE");
      break;
  }
}

void processButton() {
  const bool rawButtonPressed = digitalRead(kButtonPin) == LOW;

  if (rawButtonPressed != lastRawButtonPressed) {
    lastRawButtonPressed = rawButtonPressed;
    lastButtonChangeMs = millis();
  }

  if (millis() - lastButtonChangeMs >= kDebounceMs &&
      rawButtonPressed != stableButtonPressed) {
    stableButtonPressed = rawButtonPressed;
    if (stableButtonPressed) {
      setRelay(!relayEnabled, "button");
    }
  }
}

}  // namespace

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("camgate controller starting");

  pinMode(kRelayPin, OUTPUT);
  pinMode(kButtonPin, INPUT);
  FastLED.addLeds<SK6812, kLedPin, GRB>(statusLed, 1);
  FastLED.setBrightness(64);
  setRelay(false, "startup");
  startBle();
}

void loop() {
  processBleCommand();
  processButton();
}
