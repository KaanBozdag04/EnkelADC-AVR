# GPIO Blink – Arduino Uno

## Plattform
Arduino Uno R3 (ATmega328P)

## Funktion
Blinkar en LED kopplad till valfri GPIO-pin med valbar intervall (t.ex. 20ms, 100ms, 500ms, 1s, 2s). Blinkningen styrs av timer och interrupt.

## Komponenter
- Arduino Uno R3
- LED kopplad till digital pin 8 via 1kΩ resistor

## Instruktioner

### Bygga projektet
Kompilera med:
Varje gång man ändrar sekunder eller pin tex 13 för ljusdiodens blinkninng måste man öppna en ny terminal och skriva make clean och sedan make för att få upp nya versionen av koden.