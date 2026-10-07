#include QMK_KEYBOARD_H
#include "print.h"
#include "math.h"
#include "mouth_bitmap.h"


int frame(int min, int value, int max){
    if (value < min) {
        return min;
    }
    if (value > max) {
        return max;
    }
    return value;
}

void set_led_color(uint8_t led, uint8_t red, uint8_t green, uint8_t blue);
void looped_user(void);

enum custom_keycodes {
    UWU_TEXT = SAFE_RANGE,
    OWO_TEXT,
    OH_REPEAT,
    HM_REPEAT
};

enum combo_ids {
    MYKEY_COMBO,
    BOOTLOADER_COMBO
};

static uint8_t actual_img = 4;

const uint16_t PROGMEM GameMode_combo[] = {UWU_TEXT, OWO_TEXT, COMBO_END};
const uint16_t PROGMEM bootloader_combo[] = {OH_REPEAT, HM_REPEAT, COMBO_END};


static uint16_t red_cheek = 0;
#define HOLD_REPEAT_DELAY 500
#define HOLD_REPEAT_INTERVAL 100
#define LED_BRIGHTNESS_MAX 128
#define LED_BRIGHTNESS_STEP 8


// to change when you change the var 
// - send_chars(clockwise == false ? "a" : "z"); 
// - send_chars("w"); to z and other
static bool azerty = true;
static bool game_mode = false;

static bool repeat_h_active;
static bool repeat_m_active;
static uint32_t repeat_h_started;
static uint32_t repeat_m_started;
static uint32_t repeat_h_last;
static uint32_t repeat_m_last;
static uint8_t led_brightness = LED_BRIGHTNESS_MAX / 2;

// LED order follows rgb_matrix.layout in FirmwareBK/keyboard.json.
static const uint32_t led_colors[RGB_MATRIX_LED_COUNT] = {
    0xff6dfb, 0xff3efa, 0xff00f8, 0x800000,
    0xff005c, 0x800000, 0xff00f8, 0xff3efa,
    0xff6dfb, 0xffc4fd, 0xffc4fd, 0xffffff,
};

static uint32_t led_colors_variable[RGB_MATRIX_LED_COUNT][3]; // variable because it can be dynamically changed

combo_t key_combos[] = {
    [MYKEY_COMBO] = COMBO_ACTION(GameMode_combo),
    [BOOTLOADER_COMBO] = COMBO_ACTION(bootloader_combo),
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        UWU_TEXT, OWO_TEXT, OH_REPEAT, HM_REPEAT
    ),
};

char* replace_char(char* str, char* find, char* replace){
    char *current_pos = strchr(str,find[0]);
    while (current_pos) {
        *current_pos = replace[0];
        current_pos = strchr(current_pos,find[0]);
    }
    return str;
}
char* azerty_to_qwerty(char *str) {
    char *azerty_keys = "azqwAZQWmM,?;.:/!-_1234567890";
    char *qwerty_keys = "qwazQWAZ;:mM,<.>/68!@#$%^&*()";
    
    for (int i = 0; str[i]; i++) {
        char *pos = strchr(azerty_keys, str[i]);
        if (pos) {
            int index = pos - azerty_keys;
            str[i] = qwerty_keys[index];
        }
    }
    return str;
}

void send_chars(char* chars) {
    if (azerty) {
        char stringBuffer[64]; // Création d'un espace de 20 cases en RAM
        memset(stringBuffer, 0, sizeof(stringBuffer));
        snprintf(stringBuffer, sizeof(stringBuffer), "%s", chars); // On copie le texte dans la RAM
        // replace_char(stringBuffer, "w", "z");
        // replace_char(stringBuffer, "m", ";");
        // replace_char(stringBuffer, "W", "?");
        // replace_char(stringBuffer, "!", "/");
        // replace_char(stringBuffer, "q", "a");
        // replace_char(stringBuffer, "M", "¤");
        // replace_char(stringBuffer, ":", "M");
        // replace_char(stringBuffer, "¤", ":");
        azerty_to_qwerty(stringBuffer);
        send_string(stringBuffer);
    }
}
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (red_cheek < 128){
        red_cheek += 32;
    }
    switch (keycode) {
        case UWU_TEXT:
            if (game_mode == false) {
                if (record->event.pressed) {
                    actual_img -= 1;
                    oled_task_user();
                    send_chars("UwU");
                    looped_user();
                } else {
                    actual_img += 1;
                    oled_task_user();
                }
            } else {
                if (record->event.pressed) {
                    send_chars("w");
                }
            }
            return false;

        case OWO_TEXT:
            if (game_mode == false) {
                if (record->event.pressed) {
                    actual_img -= 1;
                    oled_task_user();
                    send_chars("OwO");
                    looped_user();
                } else {
                    actual_img += 1;
                    oled_task_user();
                }
            } else {
                if (record->event.pressed) {
                    send_chars("n");
                }
            }
            return false;

        case OH_REPEAT:
            if (record->event.pressed) {
                send_chars("Oh");
                repeat_h_active = true;
                repeat_h_started = timer_read32();
                repeat_h_last = repeat_h_started;
                oled_invert(true);
            } else {
                repeat_h_active = false;
                oled_invert(false);
            }
            return false;

        case HM_REPEAT:
            if (record->event.pressed) {
                send_chars("Hm");
                repeat_m_active = true;
                repeat_m_started = timer_read32();
                repeat_m_last = repeat_m_started;
                oled_invert(true);
            } else {
                repeat_m_active = false;
                oled_invert(false);
            }
            return false;            
    }
    return true;
}

void process_combo_event(uint16_t combo_index, bool pressed) {
    if (combo_index == MYKEY_COMBO && pressed) {
        
        game_mode = !game_mode;
        if (game_mode) {
            register_code(KC_LEFT_GUI);
            tap_code16(KC_R);
            unregister_code(KC_LEFT_GUI);
            wait_ms(50);
            send_chars("https://bkball.rf.gd/");
            tap_code(KC_ENT);
            actual_img = 0;
        } else {
            actual_img = 4;
        }
        oled_task_user();
    }
    if (combo_index == BOOTLOADER_COMBO && pressed) {
        actual_img += 1;
        oled_task_user();
        bootloader_jump();
    }
}

void matrix_scan_user(void) {
    if (repeat_h_active &&
        timer_elapsed32(repeat_h_started) >= HOLD_REPEAT_DELAY &&
        timer_elapsed32(repeat_h_last) >= HOLD_REPEAT_INTERVAL) {
        send_chars("h");
        repeat_h_last = timer_read32();
    }

    if (repeat_m_active &&
        timer_elapsed32(repeat_m_started) >= HOLD_REPEAT_DELAY &&
        timer_elapsed32(repeat_m_last) >= HOLD_REPEAT_INTERVAL) {
        send_chars("m");
        repeat_m_last = timer_read32();
    }
}

void set_led_color(uint8_t led, uint8_t red, uint8_t green, uint8_t blue) {
    rgb_matrix_set_color(led, red*(led_brightness*led_brightness)/LED_BRIGHTNESS_MAX / LED_BRIGHTNESS_MAX, green*(led_brightness*led_brightness)/LED_BRIGHTNESS_MAX / LED_BRIGHTNESS_MAX, blue*(led_brightness*led_brightness)/LED_BRIGHTNESS_MAX / LED_BRIGHTNESS_MAX);
}

bool rgb_matrix_indicators_user(void) {
    for (uint8_t led = 0; led < RGB_MATRIX_LED_COUNT; led++) {
        // const uint32_t color[3] = led_colors_variable[led];
        const uint8_t red = led_colors_variable[led][0];
        const uint8_t green = led_colors_variable[led][1];
        const uint8_t blue = led_colors_variable[led][2];
        set_led_color(led, red, green, blue);
    }
    return true;
}

bool encoder_update_user(uint8_t index, bool clockwise) {
    
        if (game_mode == false) {
            switch (index) {
            case 0:
                tap_code(clockwise == false ? KC_VOLU : KC_VOLD);
                break;
            case 1:
                if (clockwise == false) {
                    
                    if (is_oled_on()) {
                        oled_on();
                    }
                    if (led_brightness <= LED_BRIGHTNESS_MAX - LED_BRIGHTNESS_STEP) {
                        led_brightness += LED_BRIGHTNESS_STEP;
                    } else {
                        led_brightness = LED_BRIGHTNESS_MAX;
                    }
                } else if (led_brightness >= LED_BRIGHTNESS_STEP) {
                    led_brightness -= LED_BRIGHTNESS_STEP;
                } else {
                    led_brightness = 0;
                }
                oled_set_brightness((led_brightness*led_brightness)/129*2);
                break;
            default:
                return false;
            }
        } else {
            switch (index) {
            case 0:
                send_chars(clockwise == false ? "a" : "z");
                break;
            case 1:
                send_chars(clockwise == false ? "o" : "p");
                break;
            }
        }
    
    return false;
}

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    return OLED_ROTATION_0;
}

uint32_t my_callback(uint32_t trigger_time, void *cb_arg) {
    looped_user();
    return 80; // clock
}

void keyboard_post_init_user(void) {
    rgb_matrix_enable_noeeprom();
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);

    for (int i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
        const uint32_t color = led_colors[i];
        const uint8_t red = ((color >> 16) & 0xFF);
        const uint8_t green = ((color >> 8) & 0xFF);
        const uint8_t blue = (color & 0xFF);
        led_colors_variable[i][0] = red;
        led_colors_variable[i][1] = green;
        led_colors_variable[i][2] = blue;
    }

    defer_exec(100, my_callback, NULL);
}

bool oled_task_user(void) {
    oled_set_cursor(0, 0);
    oled_write_raw_P(mouth_bitmap[actual_img], sizeof(mouth_bitmap[actual_img]));
    return false;
}


//

static int16_t r_modifier = 0;
static int16_t g_modifier = 0;
static int16_t b_modifier = 0;
static float seed = 0;
static uint8_t strenght = 8;
static uint16_t speed = 100;

void modify_modifier(void){
    seed += 1;
    r_modifier = (int)((sinf(2 * seed/speed) + sinf(M_PI * seed/speed))*strenght*6);
    g_modifier = (int)((sinf((float)2 * (seed + 300)/speed) + (float)sinf(M_PI * (seed + 300)/speed))*strenght*6 );
    b_modifier = (int)((sinf((float)2 * (seed + 600)/speed) + (float)sinf(M_PI * (seed + 600)/speed))*strenght*6 );
    if (red_cheek > 0) {
        r_modifier += red_cheek;
        red_cheek -= 16;
    }
}

void looped_user(void){
    modify_modifier();
    for (uint8_t led = 0; led < RGB_MATRIX_LED_COUNT; led++) {
        const uint32_t color = led_colors[led];
        const uint8_t red = ((color >> 16) & 0xFF);
        const uint8_t green = ((color >> 8) & 0xFF);
        const uint8_t blue = (color & 0xFF);
        led_colors_variable[led][0] = frame(0, red + r_modifier*red/256 - strenght*4 + red_cheek,255);
        led_colors_variable[led][1] = frame(0, green + g_modifier*green/256 - strenght*4,255);
        led_colors_variable[led][2] = frame(0, blue + b_modifier*blue/256 - strenght*4 ,255);
    }
}