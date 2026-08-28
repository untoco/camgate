# camgate

**camgate** remotely switches external USB-C power for a GoPro from a phone.

The first version is a compact in-car controller: the phone sends a command to
an M5Stack Atom, and the Atom enables or cuts power to the dedicated GoPro
USB-C adapter. It does not control recording or replace the GoPro connection
used by the Onboard app.

## Component base

- **M5Stack Atom Lite** — controller with Bluetooth/Wi-Fi and one GPIO for the
  relay. It is sufficient for the first version; no display or extra memory is
  required.
- **M5Stack Mini 3A Relay Unit (U023)** — switches the USB-C adapter's input
  circuit. Its stated DC rating is 3 A at up to 30 V; the expected GoPro
  adapter input current is well below this, typically below 1 A.
- **Automotive 12 V to USB-C adapter** — dedicated GoPro power supply. It must
  be suitable for automotive voltage transients, not merely a generic USB-C
  mains charger.
- **12 V to 5 V converter for the Atom** — a separate, non-switched branch so
  the Atom remains powered when GoPro power is off.
- **1 A fuse** — installed close to the 12 V source for the dedicated GoPro
  power branch.
- Enclosure, terminal blocks, strain relief and automotive-grade wiring.

The M5Stack SSR Unit (BT136S) is deliberately excluded: it is an AC-only,
zero-crossing thyristor relay and cannot switch the car's 12 V DC supply.

## Electrical concept

Only the positive conductor is switched:

```text
12 V source -> 1 A fuse -> relay COM -> relay NO -> USB-C adapter +12 V
ground ------------------------------------------------> USB-C adapter ground
```

The Atom receives power from its own protected 12 V -> 5 V branch. Prefer an
ACC-switched source for the Atom, so the controller does not drain the vehicle
battery while parked.

The vehicle's accessory-socket fuse rating is not the relay load: the relay
carries only the actual current consumed by the dedicated USB-C adapter. The
final wiring, fuse location, voltage-transient tolerance, enclosure and
operation under vibration remain to be verified on the car.

## Status

Concept and component selection only. No wiring, firmware, vehicle installation
or endurance validation has been completed yet.
