#!/usr/bin/env bash
# Flash a hex file to the Atreus.
#
# Usage:
#   ./flash.sh                              # flash dump/atreus-flash.hex (the backup)
#   ./flash.sh path/to/built.hex            # flash an arbitrary hex
#
# How it works:
#   1. Waits for the Caterina bootloader to enumerate as /dev/cu.usbmodem*.
#   2. Runs avrdude with the avr109 protocol.
#
# To put the keyboard into bootloader mode:
#   - Try the keymap binding first (often Fn+Esc on the stock layout).
#   - Or open the case and press the reset button on the underside of the PCB.
#   The bootloader window is ~8 seconds.

set -euo pipefail
HERE="$(cd -- "$(dirname -- "$0")" && pwd)"
HEX="${1:-$HERE/dump/atreus-flash.hex}"

if [[ ! -f "$HEX" ]]; then
    echo "hex not found: $HEX" >&2
    exit 1
fi

if ! command -v avrdude >/dev/null; then
    echo "avrdude not installed (brew install avrdude)" >&2
    exit 1
fi

echo "Waiting for bootloader port (trigger it now)..."
PORT=""
for _ in $(seq 1 120); do
    PORT=$(/bin/ls -1 /dev/cu.usbmodem* 2>/dev/null | head -1 || true)
    [[ -n "$PORT" ]] && break
    sleep 1
done

if [[ -z "$PORT" ]]; then
    echo "Timed out waiting for bootloader" >&2
    exit 1
fi

sleep 1   # let macOS finish enumerating
echo "Flashing $HEX -> $PORT"
avrdude -p atmega32u4 -c avr109 -P "$PORT" -b 57600 -D -U "flash:w:$HEX:i"
