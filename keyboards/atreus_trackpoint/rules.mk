# MCU & bootloader (Keyboardio Atreus — ATmega32U4 with Caterina)
MCU              = atmega32u4
BOOTLOADER       = caterina
F_CPU            = 16000000
F_USB            = $(F_CPU)
ARCH             = AVR8
MCU_LDSCRIPT     = avr5

# Core QMK features (lean — original firmware embeds almost no strings, so keep this minimal)
BOOTMAGIC_ENABLE = no
MOUSEKEY_ENABLE  = yes   # needed so the host accepts the mouse HID report
EXTRAKEY_ENABLE  = no
CONSOLE_ENABLE   = no
COMMAND_ENABLE   = no
NKRO_ENABLE      = no
BACKLIGHT_ENABLE = no
AUDIO_ENABLE     = no
SLEEP_LED_ENABLE = no

# Trackpoint
PS2_MOUSE_ENABLE = yes
PS2_ENABLE       = yes
PS2_DRIVER       = interrupt

# Indicator LEDs on the daughterboard (5x WS2811 chain via the LED signal)
RGBLIGHT_ENABLE  = yes
WS2812_DRIVER    = bitbang

# Layout
LAYOUTS          = ortho_4x12
