# radio_go_app

A terminal-based radio communication app written in Go. Part of the research_ecosystem offline cloud.

## What it does

Sends and receives text messages and small data files over HAM packet radio using the AX.25 protocol. Requires no internet connection. Communication happens entirely over radio frequencies between two stations running the same app.

## How it works

The app connects to a radio via a serial port. Direwolf acts as a software TNC, encoding outgoing data as audio tones and decoding incoming audio tones back into data. The radio transmits and receives those tones over the air.
```
your app -> serial port -> Direwolf -> audio -> radio -> air -> friend's radio -> Direwolf -> friend's app
```

## Mock mode

The app ships with mock_mode = true. In this mode it runs fully without any hardware connected — messages are printed and logged locally but not transmitted. Set mock_mode = false and connect a radio to use it for real.

## Requirements

- Go 1.21 or higher
- Direwolf running as software TNC
- A HAM radio connected via USB/serial cable
- A valid HAM radio license to transmit

## How to run
```bash
go mod init radio
go get go.bug.st/serial
go run radio.go
```

## Hardware setup

Connect your radio's data port to your PC via a USB serial cable. Update the serial port path in radio.go if needed:

- Linux: /dev/ttyUSB0
- macOS: /dev/tty.usbserial-XXXX

Set mock_mode = false in radio.go when hardware is connected.

## Data transfer

Designed for small data over slow packet radio speeds. Suitable for text messages, coordinates, and short JSON or KML reports. Not suitable for large file transfers — for bulk data exchange use physical USB swap.

## Part of research_ecosystem

This app is one component of research_ecosystem, a fully offline personal cloud designed to run from a USB drive with no internet dependency. Communication over HAM radio means the ecosystem functions even when all internet and cellular infrastructure is unavailable.
