/* Copyright 2018 Jack Humbert
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H

#define HIGHEST_LAYER 3 //replace X with your highest layer

static uint8_t current_layer = 0;

enum custom_keycodes {
    KC_LC, //cycle layers
    ALT_TAB,
};

enum layers {
	_NUM = 0,
	_WASD,
	_OSU,
    _ARROWS,
};

bool is_alt_tab_active = false;    // ADD this near the begining of keymap.c
uint16_t alt_tab_timer = 0;        // we will be using them soon.   

bool is_tap = false;
bool is_double_tap = false;
uint8_t presses = 0;

bool idler = false;
uint16_t idler_timer = 0; 

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

	[_NUM] = LAYOUT(
		KC_7, KC_8, KC_9, \
		KC_4, KC_5, KC_6, \
		KC_1, KC_2, KC_3, \
		KC_LC, KC_0, KC_DOT \
	),
  
	[_WASD] = LAYOUT(
		KC_R, KC_F, KC_SPC, \
		KC_E, KC_D, ALT_TAB, \
		KC_W, KC_S, KC_LC, \
		KC_Q, KC_A, KC_LSFT \
	),

    [_OSU] = LAYOUT(
		KC_F, KC_VOLU, KC_VOLD, \
		KC_D, KC_RIGHT, KC_SPC, \
		KC_UP, KC_DOWN, KC_LC, \
		KC_ESC, KC_LEFT, KC_WHOM \
    ),                                   

    [_ARROWS] = LAYOUT(
		KC_LC, KC_VOLD, KC_VOLU, \
		KC_HOME, KC_WHOM, KC_PGUP, \
		KC_END, KC_UP, KC_PGDN, \
		KC_LEFT, KC_DOWN, KC_RIGHT \
	),
};

void cycle_led(void) {
    switch (current_layer) {
        case 0:
            // LED 00
            writePinHigh(B0);
            writePinHigh(D5);
            break;
        case 1:
            // LED 01
            writePinHigh(B0);
            writePinLow(D5);
            break;
        case 2:
            // LED 10
            writePinLow(B0);
            writePinHigh(D5);
            break;
        case 3:
            // LED 11
            writePinLow(B0);
            writePinLow(D5);
            break;
    }
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_LC:
            if (record->event.pressed) {
                if (current_layer == HIGHEST_LAYER) {
                    for (int i = 1; i <= HIGHEST_LAYER; i++) {
                        layer_off(i);
                    }
                    current_layer = 0;
                } else {
                    current_layer++;
                    layer_on(current_layer);
                }
            }
            cycle_led();
            return false;

        case ALT_TAB:
            if (is_double_tap) {
                if (record->event.pressed) {
                    if (!is_alt_tab_active) {
                        is_alt_tab_active = true;
                        register_code(KC_LALT);
                    }
                    alt_tab_timer = timer_read();
                    register_code(KC_TAB);
                } else {
                    unregister_code(KC_TAB);
                }
            } 
            
            else if (!is_double_tap)
            {
                if (record->event.pressed) {
                    if (!is_tap)
                    {
                        is_tap = true;   
                    }
                    alt_tab_timer = timer_read();
                    register_code(KC_LALT);
                    presses++;
                }
                else {
                    unregister_code(KC_LALT);                   
                }
            }
            return false;

        default:
            return true;    
    }
};

void matrix_scan_user(void) {  // The very important timer.
    if (is_alt_tab_active) {
        if (timer_elapsed(alt_tab_timer) > 1000) {
            unregister_code(KC_LALT);
            is_alt_tab_active = false;

            is_double_tap = false;
        }
    }

    if (is_tap) {
        if (timer_elapsed(alt_tab_timer) > 200) { // after 200 ms
            if (presses > 1) { // if tapped twice within 200 ms
                is_double_tap = true; // activate super alt+tab

                /* hold-LALT and tap-TAB begin */
                register_code(KC_LALT);
                tap_code(KC_TAB); 
                /* hold-LALT and tap-TAB end*/

                /* idler start */
                if (!idler) {
                    idler = true;
                }
                idler_timer = timer_read(); 
                /* idler end*/
            }
            presses = 0;
            is_tap  = false;
        }
    }

    /* when not using super alt+tab, close view after 1200 ms */
    if (idler) {
        if (timer_elapsed(idler_timer) > 1200) {
            if(!is_alt_tab_active) { // if not using super alt+tab
            unregister_code(KC_LALT); // release alt
            }
            idler = false;
        }
    }
}
