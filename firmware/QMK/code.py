"""KMK firmware for Froskeys."""

import board
import busio as io
from kmk.extensions.display import Display, ImageEntry
from kmk.extensions.display.ssd1306 import SSD1306
from kmk.extensions.media_keys import MediaKeys
from kmk.extensions.RGB import RGB, AnimationModes
from kmk.keys import KC, make_key
from kmk.kmk_keyboard import KMKKeyboard
from kmk.modules.combos import Chord, Combos
from kmk.modules.encoder import EncoderHandler
from kmk.modules.layers import Layers
from kmk.modules.macros import Delay, Macros, Press, Release, Tap
from kmk.scanners import DiodeOrientation

# What is this for
make_key(
    names=("MYKEY",),
    on_press=lambda *args: print("I pressed MYKEY"),
)

# Initialise keyboard hardware, modules and extensions
keyboard = KMKKeyboard()
combos = Combos([Chord((0, 1), KC.MYKEY, match_coord=True)])
encoder = EncoderHandler()
rgb = RGB(
    pixel_pin=board.D6, num_pixels=9, val_limit=128, animation_mode=AnimationModes.SWIRL
)

oled_i2c = io.I2C(sda=board.D4, scl=board.D5, frequency=400_000)
oled_display = Display(
    display=SSD1306(oled_i2c),
    width=128,
    height=32,
    flip=True,
    entries=[ImageEntry(0, 0, "froskeys-oled.bmp")],
)

# Load modules and extensions
keyboard.modules = [Macros()]
keyboard.extensions = [
    MediaKeys(),
    Macros(),
    Layers(),
    combos,
    encoder,
    oled_display,
    rgb,
]

# Set encoder parameters
encoder.pins = ((board.D3, board.D7, None),)
encoder.map = [
    ((KC.AUDIO_VOL_DOWN, KC.AUDIO_VOL_UP, KC.NO),),
    ((KC.BRIGHTNESS_DOWN, KC.BRIGHTNESS_UP, KC.NO),),
]

# Funny macro
PACMAN_SYU = KC.MACRO(
    Press(KC.LGUI),
    Tap(KC.T),
    Release(KC.LGUI),
    Delay(500),
    "fastfetch",
    Tap(KC.ENTER),
)

# Set keyboard parameters
keyboard.col_pins = (board.D0, board.D1, board.D2)
keyboard.row_pins = (board.D10, board.D9, board.D8)
keyboard.diode_orientation = DiodeOrientation.COL2ROW
keyboard.keymap = [
    # [
    #     KC.LCTL(KC.Z),
    #     KC.LCTL(KC.Y),
    #     KC.MEDIA_PLAY_PAUSE,
    #     KC.F2,
    #     KC.LCTL(KC.D),
    #     PACMAN_SYU,
    #     KC.LCTL(KC.C),
    #     KC.LCTL(KC.V),
    #     KC.KP_EQUAL_AS400,
    # ],
    [
        KC.E,
        KC.W,
        KC.R,
        KC.A,
        KC.S,
        KC.D,
        KC.C,
        KC.LSHIFT,
        KC.SPACE,
    ],
    [KC.TRNS] * 9,  # Transparent
]

if __name__ == "__main__":
    keyboard.go()
