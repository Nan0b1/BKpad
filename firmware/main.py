

# display imports / thks to LyricSantan (I don't want to recreate the wheel)
import displayio
import adafruit_ssd1306
try:
    import adafruit_imageload
except Exception:
    adafruit_imageload = None

# OLED init
i2c = board.I2C() 
WIDTH = 128
HEIGHT = 32

try:
    display_bus = displayio.I2CDisplay(i2c, device_address=0x3C, reset=None)
    display = adafruit_ssd1306.SSD1306_I2C(WIDTH, HEIGHT, i2c)
    has_display = True
except Exception as e:
    # If display fails to init, continue without it
    print("init failed:", e)
    has_display = False

# draw image if present
if has_display and adafruit_imageload is not None:
    try:
        bmp, palette = adafruit_imageload.load("/mouth.bmp",
                                               bitmap=displayio.Bitmap,
                                               palette=displayio.Palette)
        mouth = displayio.TileGrid(bmp, pixel_shader=palette)
        g = displayio.Group()
        g.append(mouth)
        display.show(g)
    except Exception as e:
        # no image, fall back to text
        display.fill(0)
        display.text("BKpad", 0, 0, 1)
        display.show()
elif has_display:
    display.fill(0)
    display.text("BKpad", 0, 0, 1)
    display.show()



from kmk.modules.macros import Macros
macros = Macros()
keyboard.modules.append(macros)

UwU = KC.MACRO("UwU")
OwO = KC.MACRO("OwO")
Ohh = KC.MACRO("O","h")
Hmm = KC.MACRO("H","m")

# switches
KEY_PINS = [board.GP27, board.GP26,
            board.GP28, board.GP29]


# Rotary encoders
# Pins
ENC_left_A = board.GP2
ENC_left_B = board.GP1

ENC_right_A = board.GP3
ENC_right_B = board.GP4


""" ########################################## """

import board
from kmk.kmk_keyboard import KMKKeyboard
from kmk.scanners.keypad import KeysScanner


# Keyboard implementation class
class MyKeyboard(KMKKeyboard):
    def __init__(self):
        super().__init__()

        # create and register the scanner
        self.matrix = KeysScanner(
            # require argument:
            pins=KEY_PINS,
            value_when_pressed=False,
            # optional arguments with defaults:
            pull=True,
            interval=0.02, # Matrix sampling interval in ms
            debounce_threshold=None, # Number of samples needed to change state, values greater than 1 enable debouncing. Only applicable for CircuitPython >= 9.2.0
            max_events=64
        )

keyboard_class = MyKeyboard()
keyboard = keyboard_class.matrix
keyboard.keymap = [Ohh,
                   Hmm,
                   OwO,
                   UwU]

# Vars
from kmk.modules.encoder import EncoderHandler
encoder_handler = EncoderHandler()
encoder_handler.pins = (
    # regular direction encoder and a button
    (ENC_left_A, ENC_left_B, None,), # encoder #1
    (ENC_right_A, ENC_right_B, None,),# encoder #2
    )


from kmk.extensions.media_keys import MediaKeys
keyboard.extensions.append(MediaKeys())
encoder_handler.map = [ ((KC.BRIGHTNESS_DOWN, KC.BRIGHTNESS_UP, None), (KC.AUDIO_VOL_DOWN, KC.AUDIO_VOL_UP, None),),
                        ]

from kmk.extensions.RGB import RGB

rgb = RGB(pixel_pin=board.GP0, num_pixels=12)
keyboard.extensions.append(rgb)
rgb.set_rgb_fill(255, 255, 255)

# thanks to github.com/LyricSantana/lyrics_macropad/blob/main/Firmware/main.py
# KMK documentation is horrible for my little brain and their tutoral starts with 'test with your controller' which I don't have ._.
# So I'll take functions from there and not the documentation
# I do not copy paste all his code, I just copy single functions

# docs are simple but not enought for my little brain :(
# help
