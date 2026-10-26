#include QMK_KEYBOARD_H
#include "print.h"
#include "math.h"
#include "mouth_bitmap.h"

/**
 * Welcome in the firmware's code!
 * I did some code with the help of the QMK functions.
 * It is not made to run on batteries and is power hungry :3
 * Go see https://github.com/Nan0b1/BKpad !
 */

// EEPROM vars, for persistent modifications
// sourced from qmk doc
typedef union { 
  uint32_t raw;
  struct { //it's the number of bits not the default value :3
    bool     ee_azerty :1;          // stored keyboard layout value, defaults to qwerty because the bit is set to 0
    uint8_t  ee_led_brightness:8;   // defaults to 0
    bool     ee_initialized:1;      // if first run: set default brightness value
  };
} user_config_t; 

user_config_t user_config;


/// @brief Blocks the value you enter from being out of the min max scope
/// @param min Minimum value the number can take (included)
/// @param value The value you want to not be out of scope
/// @param max Maximum value the number can take (included)
/// @return the value but rectified if out of scope
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
    GAME_COMBO,
    BOOTLOADER_COMBO,
    HIDDEN_COMBO
};

static uint8_t actual_img = 4;
static uint8_t old_img = 4;
static uint8_t custom_img = 4; // '\0' is null but if you do that the program will start showing nothing onto the oled :(

const uint16_t PROGMEM GameMode_combo[] = {UWU_TEXT, OWO_TEXT, COMBO_END};
const uint16_t PROGMEM bootloader_combo[] = {OH_REPEAT, HM_REPEAT, COMBO_END};
const uint16_t PROGMEM hidden_combo[] = {UWU_TEXT, OWO_TEXT, OH_REPEAT, COMBO_END};


static uint16_t red_cheek = 0;
#define HOLD_REPEAT_DELAY 500
#define HOLD_REPEAT_INTERVAL 100
#define LED_BRIGHTNESS_MAX 128
#define LED_BRIGHTNESS_STEP 8



static bool game_mode = false;

static bool repeat_h_active;
static bool repeat_m_active;
static uint32_t repeat_h_started;
static uint32_t repeat_m_started;
static uint32_t repeat_h_last;
static uint32_t repeat_m_last;


/// @brief LED order follows rgb_matrix.layout in FirmwareBK/keyboard.json.
const static uint32_t led_colors[2][RGB_MATRIX_LED_COUNT] = {
    {
        0xa460c2, 0xa937dc, 0xb100ff, 0x800019,
        0xff005c, 0x800019, 0xb100ff, 0xa937dc,
        0xa460c2, 0x91488e, 0x7c3779, 0x5c2f5a,
    },
    {
        0xa25000, 0x683300, 0x3e1f00, 
        0x802200, 0x3e1f00, 0x802200, 
        0x3e1f00, 0x683300, 0xa25000,
        0x5f3100, 0x5f3100, 0x5f3100,
    }
};


static uint8_t color_scene = 0;


static uint32_t led_colors_variable[RGB_MATRIX_LED_COUNT][3]; // variable because it can be dynamically changed

combo_t key_combos[] = {
    [GAME_COMBO] = COMBO_ACTION(GameMode_combo),
    [BOOTLOADER_COMBO] = COMBO_ACTION(bootloader_combo),
    [HIDDEN_COMBO] = COMBO_ACTION(hidden_combo),
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        UWU_TEXT, OWO_TEXT, OH_REPEAT, HM_REPEAT
    ),
};

/// @brief Turns a chain into its inverted equivalent to be written in azerty
/// @param str a character chain
/// @return the traduced character chain
char* azerty_to_qwerty(char *str) { // made with AI, I was not able to do this by myself in c when starting the code :(
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

/// @brief send_string() function but translated to azerty if user_config.ee_azerty is true
/// @param chars chars The string you want to send to the computer
void send_chars(char* chars) {
    if (user_config.ee_azerty) {
        char stringBuffer[64];
        memset(stringBuffer, 0, sizeof(stringBuffer));
        snprintf(stringBuffer, sizeof(stringBuffer), "%s", chars);
        azerty_to_qwerty(stringBuffer);
        send_string(stringBuffer);
    } else {
        send_string(chars);
    }
}

/// @brief Function automatically called when a button is pressed
/// @param keycode The keyboard key pressed
/// @param record
/// @return true if the keyboards continues processing the key, else false if you already done what you wanted and want to override qmk's function
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (red_cheek < 82){
        red_cheek += 48;
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
                    send_chars(user_config.ee_azerty ? "w" : "z");
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
                // oled_invert(true);
                actual_img += 1;
                oled_task_user();
            } else {
                repeat_h_active = false;
                // oled_invert(false);
                actual_img -= 1;
                oled_task_user();
            }
            return false;

        case HM_REPEAT:
            if (record->event.pressed) {
                send_chars("Hm");
                repeat_m_active = true;
                repeat_m_started = timer_read32();
                repeat_m_last = repeat_m_started;
                actual_img += 1;
                oled_task_user();
            } else {
                repeat_m_active = false;
                actual_img -= 1;
                oled_task_user();
            }
            return false;            
    }
    return true;
}


/// @brief Function automatically called when buttons are pressed
/// @param combo_index The name of the combo pressed
/// @param If pressed
void process_combo_event(uint16_t combo_index, bool pressed) {
    if (combo_index == GAME_COMBO && pressed) {
        game_mode = !game_mode;
        if (game_mode) {
            color_scene = 1;
            register_code(KC_LEFT_GUI);
            tap_code16(KC_R);
            unregister_code(KC_LEFT_GUI);
            wait_ms(55);
            send_chars("https://nan0b1.github.io/BKball/"); // new website
            tap_code(KC_ENT);
            actual_img = 0;
        } else {
            color_scene = 0;
            actual_img = 4;
        }
        oled_task_user();
    }
    if (combo_index == BOOTLOADER_COMBO && pressed) {
        custom_img = 7+user_config.ee_azerty;
        oled_task_user();
       user_config.ee_azerty = !user_config.ee_azerty;
        user_config.ee_azerty =user_config.ee_azerty;
        eeconfig_update_user(user_config.raw);
    }
    if (combo_index == HIDDEN_COMBO && pressed) {
        bootloader_jump();
    }
}

/// @brief Automatically called - Detects if a long press is done on the potentiometers
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

/// @brief Set a led color at the actual brightness (user_config.ee_led_brightness)
/// @param led LED id
/// @param red 0-255 red value
/// @param green 0-255 green value
/// @param blue 0-255 blue value
void set_led_color(uint8_t led, uint8_t red, uint8_t green, uint8_t blue) {
    rgb_matrix_set_color(led, 
        frame(0,red*(user_config.ee_led_brightness*user_config.ee_led_brightness)/LED_BRIGHTNESS_MAX / LED_BRIGHTNESS_MAX + red_cheek*(led==3 || led==5)/user_config.ee_led_brightness/LED_BRIGHTNESS_MAX*3,255), 
        green*(user_config.ee_led_brightness*user_config.ee_led_brightness)/LED_BRIGHTNESS_MAX / LED_BRIGHTNESS_MAX,
        blue*(user_config.ee_led_brightness*user_config.ee_led_brightness)/LED_BRIGHTNESS_MAX / LED_BRIGHTNESS_MAX
    );
}

/// @brief Updates all leds to their led_colors_variable's value
/// @return true: qmk doesn't need to continue processing
bool rgb_matrix_indicators_user(void) {
    for (uint8_t led = 0; led < RGB_MATRIX_LED_COUNT; led++) {
        const uint8_t red = led_colors_variable[led][0];
        const uint8_t green = led_colors_variable[led][1];
        const uint8_t blue = led_colors_variable[led][2];
        set_led_color(led, red, green, blue);
    }
    return true;
}

/// @brief Detects rotary encoders actions
/// @param index What encoder is updated
/// @param clockwise because I have 3 pins encoders
/// @return false to override the qmk fonction
bool encoder_update_user(uint8_t index, bool clockwise) {
    if (red_cheek < 106){
        red_cheek += 24;
    }
    if (game_mode == false) {
        switch (index) {
        case 0:
            tap_code(clockwise == false ? KC_VOLU : KC_VOLD);
            break;
        case 1:
            if (clockwise == false) {
                if (user_config.ee_led_brightness <= LED_BRIGHTNESS_MAX - LED_BRIGHTNESS_STEP) {
                    user_config.ee_led_brightness += LED_BRIGHTNESS_STEP;
                    eeconfig_update_user(user_config.raw);
                } else {
                    user_config.ee_led_brightness = LED_BRIGHTNESS_MAX;
                    eeconfig_update_user(user_config.raw);
                }
            } else if (user_config.ee_led_brightness >= LED_BRIGHTNESS_STEP) {
                user_config.ee_led_brightness -= LED_BRIGHTNESS_STEP;
                eeconfig_update_user(user_config.raw);
            } else {
                user_config.ee_led_brightness = 0;
                eeconfig_update_user(user_config.raw);
            }
            oled_set_brightness((user_config.ee_led_brightness*user_config.ee_led_brightness)/129*2);
            break;
        default:
            return false;
        }
    } else {
        switch (index) {
        case 0:
            send_chars(user_config.ee_azerty ? (clockwise == false ? "a" : "z") : (clockwise == false ? "q" : "w"));
            break;
        case 1:
            send_chars(user_config.ee_azerty ? (clockwise == false ? "o" : "p") : (clockwise == false ? "o" : "p"));
            break;
        }
    }
    
    return false;
}


/// @brief Function to have the screen in the right orientation
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    return OLED_ROTATION_0;
}

/// @brief Loops functions every 80ms
/// @param trigger_time Last triggered time
/// @return milliseconds before it calls itself
uint32_t my_callback(uint32_t trigger_time, void *cb_arg) {
    looped_user();
    return 80; // clock
}

/// @brief Init function when the keyboard starts up: leds startup, EEPROM reading and looped function initialisation
void keyboard_post_init_user(void) {
    rgb_matrix_enable_noeeprom();
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);

    for (int i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
        const uint32_t color = led_colors[color_scene][i];
        const uint8_t red = ((color >> 16) & 0xFF);
        const uint8_t green = ((color >> 8) & 0xFF);
        const uint8_t blue = (color & 0xFF);
        led_colors_variable[i][0] = red;
        led_colors_variable[i][1] = green;
        led_colors_variable[i][2] = blue;
    }

    user_config.raw = eeconfig_read_user();
    if (!user_config.ee_initialized) {
        user_config.ee_initialized = true;
        user_config.ee_led_brightness = 64;
        eeconfig_update_user(user_config.raw);
    }

    defer_exec(80, my_callback, NULL);
}

/*

*/

/// @brief updates the oled display. \n
/// If actual_img is set to a new value, the display is set to the image associated. \n
/// If a custom img is set, the custom img will be displayed untill the actual_img var is changed. \n
/// If a custom img is not set, custom_img == '\0'
/// @return false to override qmk's function
bool oled_task_user(void) {
    oled_set_cursor(0, 0);
    if (old_img != actual_img) {
        custom_img = '\0';
        oled_write_raw_P(mouth_bitmap[actual_img], sizeof(mouth_bitmap[actual_img]));
    } else if (custom_img != '\0') {
        oled_write_raw_P(mouth_bitmap[custom_img], sizeof(mouth_bitmap[custom_img]));
    }
    old_img = actual_img;
    return false;
}


//

static int16_t r_modifier = 0;
static int16_t g_modifier = 0;
static int16_t b_modifier = 0;
static float seed = 0;
static uint8_t strenght = 6;
static uint16_t speed = 100;

/// @brief Modify each led's rgb values with a non periodical function
void modify_modifier(void){
    seed += 1;
    r_modifier = (int)((sinf(2 * seed/speed) + sinf(M_PI * seed/speed))*strenght*6);
    g_modifier = (int)((sinf((float)2 * (seed + 300)/speed) + (float)sinf(M_PI * (seed + 300)/speed))*strenght*6 );
    b_modifier = (int)((sinf((float)2 * (seed + 600)/speed) + (float)sinf(M_PI * (seed + 600)/speed))*strenght*6 );
    if (red_cheek > 0) {
        red_cheek -= 4;
    }
}

static bool smol_animations = true;

/// @brief RGB animation
void looped_user(void){
    modify_modifier();
    for (uint8_t led = 0; led < RGB_MATRIX_LED_COUNT; led++) {
        const uint32_t color = led_colors[color_scene][led];
        const uint8_t red = ((color >> 16) & 0xFF);
        const uint8_t green = ((color >> 8) & 0xFF);
        const uint8_t blue = (color & 0xFF);
        uint8_t red_supp = 0;
        if (led == 3 || led == 5){
            red_supp += red_cheek;
        }
        led_colors_variable[led][0] = frame(0, red + (r_modifier + red_supp)*red/256*smol_animations - strenght*4,255);
        led_colors_variable[led][1] = frame(0, green + g_modifier*green/256*smol_animations - strenght*4,255);
        led_colors_variable[led][2] = frame(0, blue + b_modifier*blue/256*smol_animations - strenght*4 ,255);
    }
}

