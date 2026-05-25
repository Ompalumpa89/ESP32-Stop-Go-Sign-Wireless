# ESP32 Stop/Go Sign with ESP-NOW and WS2812B

Wireless stop/go sign using two ESP32 DevKit boards and ESP-NOW.

## Hardware

### Board 1 - Sender / Button
- ESP32 DevKit
- Uses the onboard BOOT button on GPIO 0
- MAC: `E8:31:CD:D6:EF:C8`

### Board 2 - Receiver / Lamp
- ESP32 DevKit
- MAC: `0C:DC:7E:62:9E:40`
- WS2812B data input connected to GPIO 4
- 13 WS2812B LEDs

## LED layout

- LED 0-5: green side
- LED 6: middle LED, switches between green and red
- LED 7-12: red side

## Modes

The sender cycles through these modes using the BOOT button:

1. Rainbow
2. Green
3. Red
4. Off
5. Back to Rainbow

## Arduino IDE setup

Install this library:

- Adafruit NeoPixel

Recommended board selection:

- DOIT ESP32 DEVKIT V1

Serial Monitor:

- 115200 baud

## Wiring

### WS2812B

- ESP32 GPIO 4 -> DIN on first WS2812B
- LED 5V -> 5V power supply
- LED GND -> power supply GND
- ESP32 GND -> same GND

Common GND is required.

Use an external 5V power supply for the LEDs if brightness is high.

## Notes

The receiver code attempts to keep the built-in ESP32 LED on GPIO 2 off. On some ESP32 DevKit boards, the visible LED is a power LED and cannot be disabled in software.
