# EMF Reader

Arduino-Mega-basierter Detektor für elektrische Wechselfelder (50 Hz).
Feldstärke wird über 5 LEDs und einen zuschaltbaren Piezo-Buzzer angezeigt.

## Schaltplan
![Schaltplan](docs/EMF_reader.svg)

[Als PDF](docs/EMF_reader.pdf) · KiCad-Projekt in `hardware/`

## Funktionsweise
- Antenne → 2N3904-Verstärkerstufe → A0
- Peak-to-Peak-Amplitude pro 100-ms-Fenster → Stufe 1–5 (mit Hysterese)
- Taster (Pin 44) schaltet den Buzzer ein/aus

## Build
PlatformIO, Board: Arduino Mega 2560
