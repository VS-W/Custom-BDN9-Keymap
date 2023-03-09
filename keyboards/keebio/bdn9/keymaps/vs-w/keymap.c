#include QMK_KEYBOARD_H

enum encoder_names {
  _LEFT,
  _RIGHT,
};

enum custom_keycodes {
  MJMACRO = SAFE_RANGE,
  MJSPEED,
  MJRNG,
  MS,
  MLAYER,
  AUTOF5,
  AUTOLMB,
  ALT_TAB
};

bool is_alt_tab_active = false;

bool mouse_jiggle_mode = false;
bool mouse_jiggle_rng = false;
bool toggle_speed = false;
uint16_t spam_timer = false;
uint16_t spam_interval = 10;

bool mouse_layer = false;
int mss = 0;

bool auto_lmb = false;
int auto_lmb_interval = 300;
bool auto_f5 = false;
int auto_f5_interval = 1000;

int rgbindex = 0;
bool reversergb = false;
uint16_t rgb_timer = false;
uint16_t rgb_interval = 200;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        // Toggle, change the time between each mouse jiggler action (2 seconds or 10 milliseconds).
        case MJSPEED: {
            if (record->event.pressed) {
                toggle_speed ^= 1;
                if (toggle_speed) {
                    spam_interval = 2000;
                } else {
                    spam_interval = 10;
                }
            }
            return false;
        }

        // Auto-refresher, F5.
        case AUTOF5: {
            if (record->event.pressed) {
                rgbindex = 255;
                auto_f5 ^= 1;
                if (auto_f5) {
                    spam_timer = timer_read();
                    rgb_timer = timer_read();
                } else {
                    rgbindex = 255;
                    backlight_level(255);
                }
            }
            return false;
        }

        // Auto-clicker, left mouse button.
        case AUTOLMB: {
            if (record->event.pressed) {
                rgbindex = 255;
                auto_lmb ^= 1;
                if (auto_lmb) {
                    spam_timer = timer_read();
                    rgb_timer = timer_read();
                } else {
                    rgbindex = 255;
                    backlight_level(255);
                }
            }
            return false;
        }

        // Sets whether to send random mouse movements or force mosue to top-left when running the mouse jiggler macro.
        case MJRNG: {
            if (record->event.pressed) {
                mouse_jiggle_rng ^= 1;
            }
            return false;
        }

        // Toggle mouse key layer and sets a low backlight level to indicate that layer is active.
        case MLAYER: {
            if (record->event.pressed) {
                mouse_layer ^= 1;
                if (mouse_layer) {
                    layer_move(3);
                    backlight_level(1);
                } else {
                    layer_move(0);
                    backlight_level(255);
                }
            }
            return false;
        }

        // Toggle mouse speed.
        case MS: {
            if (record->event.pressed) {
                switch(mss) {
                    case 0:
                        tap_code(KC_ACL0);
                        mss++;
                        break;
                    case 1:
                        tap_code(KC_ACL1);
                        mss = 0;
                        break;
                }
            }
            return false;
        }

        // Toggle mouse jiggler on/off. Sets backlight_level and mouse accel speed back to default on disable.
        case MJMACRO: {
            if (record->event.pressed) {
                rgbindex = 0;
                reversergb = false;
                mouse_jiggle_mode ^= 1;
                spam_timer = timer_read();
                rgb_timer = timer_read();
                if (!mouse_jiggle_mode) {
                    backlight_level(255);
                    tap_code(KC_ACL0);
                } else {
                    tap_code(KC_ACL2);
                }
            }
            return false;
        }

        // Send alt-tab, hold alt until released.
        case ALT_TAB: {
            if (record->event.pressed) {
                if (!is_alt_tab_active) {
                    is_alt_tab_active = true;
                    register_code(KC_LALT);
                    tap_code(KC_TAB);
                }
            } else {
                is_alt_tab_active = false;
                unregister_code(KC_LALT);
            }
            return false;
        }
    }
    return true;
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/*  
    Default layer, functions matching the keycaps.

    | Knob 1: Vol Dn/Up |              | Knob 2: Page Dn/Up    |
    | Press: Mute       | PrtScr       | Press: ALT-TAB Rotary |
    | Hold: Layer 1     | Up           | Hold: Layer 2         |
    | Left              | Down         | Right                 |
*/
    [0] = LAYOUT(
        KC_MUTE, KC_PSCR, ALT_TAB,
        MO(1)  , KC_UP  , MO(2),
        KC_LEFT, KC_DOWN, KC_RGHT
    ),

/*  
    Layer 1, mostly mouse related macros.

    | To MouseKey layer | MouseJiggler | Toggle MJ randomness  |
    | Held: Layer 1     | X            | Toggle MJ Speed       |
    | Auto-click LMB    | Y            | Z                     |
*/
    // Via MO(1)
    [1] = LAYOUT(
        MLAYER , MJMACRO, MJRNG  ,
        KC_TRNS, KC_X   , MJSPEED,
        AUTOLMB, KC_Y   , KC_Z   
    ),

/*  
    MouseKey layer.

    | Exit MouseKeys    | Change Speed | Press: ALT-TAB Rotary |
    | Left mouse button | Mouse Up     | Right mouse button    |
    | Mouse Left        | Mouse Down   | Mouse Right           |
*/
    // Via MLAYER (toggled)
    [3] = LAYOUT(
        MLAYER , MS     , ALT_TAB,
        KC_BTN1, KC_MS_U, KC_BTN2,
        KC_MS_L, KC_MS_D, KC_MS_R
    ),
    
/*  
    Layer 2, other macros. Mostly unused.

    | Press: Mute       | Auto-F5      | Press: ALT-TAB Rotary |
    | V                 | W            | Held: Layer 2         |
    | X                 | Y            | Z                     |
*/
    // Via MO(2)
    [2] = LAYOUT(
        KC_MUTE, AUTOF5 , ALT_TAB,
        KC_V   , KC_W   , KC_TRNS,
        KC_X   , KC_Y   , KC_Z   
    ),
};

void matrix_scan_user(void) {
    if (mouse_jiggle_mode && timer_elapsed(spam_timer) >= spam_interval) {
        spam_timer = timer_read();
        if (mouse_jiggle_rng) {
            switch(rand() % 4) {
                case 0:
                    tap_code(KC_MS_UP);
                    break;
                case 1:
                    tap_code(KC_MS_LEFT);
                    break;
                case 2:
                    tap_code(KC_MS_DOWN);
                    break;
                case 3:
                    tap_code(KC_MS_RIGHT);
                    break;
            }
        } else {
            switch(rand() % 2) {
                case 0:
                    tap_code(KC_MS_UP);
                    break;
                case 1:
                    tap_code(KC_MS_LEFT);
                    break;
            }
        }
    }
    if (auto_f5 && timer_elapsed(spam_timer) >= auto_f5_interval) {
        spam_timer = timer_read();
        tap_code(KC_F5);
    }
    if (auto_f5 && timer_elapsed(rgb_timer) >= (auto_f5_interval / 2)) {
        rgb_timer = timer_read();
        if (rgbindex > 0) {
            rgbindex = 0;
        } else {
            rgbindex = 1;
        }
        backlight_level(rgbindex);
    }

    if (auto_lmb && timer_elapsed(spam_timer) >= auto_lmb_interval) {
        spam_timer = timer_read();
        tap_code(KC_BTN1);
    }
    if (auto_lmb && timer_elapsed(rgb_timer) >= (auto_lmb_interval / 2)) {
        rgb_timer = timer_read();
        if (rgbindex > 0) {
            rgbindex = 0;
        } else {
            rgbindex = 1;
        }
        backlight_level(rgbindex);
    }
    if (mouse_jiggle_mode && timer_elapsed(rgb_timer) >= rgb_interval) {
        rgb_timer = timer_read();
        if (reversergb) {
            rgbindex = rgbindex - 1;
        } else {
            rgbindex = rgbindex + 1;
        }
        if (rgbindex > 7) {
            rgbindex = 8;
            reversergb = true;
        }
        if (rgbindex < 0) {
            rgbindex = 0;
            reversergb = false;
        }
        backlight_level(rgbindex);
    }
}

bool encoder_update_user(uint8_t index, bool clockwise) {
    if (index == _LEFT) {
        if (clockwise) {
            tap_code(KC_VOLU);
        } else {
            tap_code(KC_VOLD);
        }
    } else if (index == _RIGHT) {
        if (clockwise) {
            if (is_alt_tab_active) {
                tap_code(KC_TAB);
            } else {
                tap_code(KC_PGDN);
            }
        } else {
            if (is_alt_tab_active) {
                tap_code16(S(KC_TAB));
            } else {
                tap_code(KC_PGUP);
            }
        }
    }
    return false;
}