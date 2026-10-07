# KMK port for BKpad

This QMK keymap uses four individual GPIO buttons, with a single 12-LED RGB chain.

## Build

Open your cmd, go in the QMK folder, and enter `sBuild.cmd`. When in the shell, enter `sBuild` and it will compile and send your `firmwarebk_kmk_port.uf2` directly in the QMK folder :D
The AI says at the first launch it inits QMK and may ask permissions.
(This is monothread compilation but this is a microcontroller and my pc is a 2C/2T amd cpu I will not implement multithreading)

## AI notes

- Each button connects its GPIO to GND only while pressed. The pins are scanned
  directly, not as row and column lines: `D2=GP28`, `D3=GP29`, `D0=GP26`,
  `D1=GP27`.
- D2 types `UwU`, D3 types `OwO`, D0 types `Oh` and then repeats `h` while held,
  and D1 types `Hm` and then repeats `m` while held. Repeats start after 500 ms.
- Holding D2 and D3 together emits the original `MYKEY` debug message to the
  QMK console instead of sending their strings.
- Encoder 1 (`D8/D7`, `GP2/GP1`) controls system volume. Encoder 2 (`D10/D9`,
  `GP3/GP4`) changes the RGB LED brightness.
- This is a single 12-LED build. Edit one `0xRRGGBB` value per LED in the
  `led_colors` array in `keymaps/kmk_port/keymap.c`. LED order matches the
  `rgb_matrix.layout` list in this folder's `keyboard.json`; encoder 2 adjusts
  brightness from 0 to 128.
- The WS2812 data line follows the KMK source: `D6` / `GP0`. The RGB matrix is
  explicitly enabled in solid-color mode during keyboard startup.
- `mouth.bmp` is converted to a 512-byte monochrome bitmap and displayed on the
  128x32 OLED. (Later, I compile this myself with `firmware\QMK\images\LCDAssistant.exe`)
- The OLED is configured with `OLED_ROTATION_0` so the bitmap appears upright.

Then everything is done myself hehe :3

Every image is done in gimp (glory to open source)