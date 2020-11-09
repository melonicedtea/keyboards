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

#define HIGHEST_LAYER 2 //replace X with your highest layer

static uint8_t current_layer = 0;

enum custom_keycodes {
    KC_LC = SAFE_RANGE, //cycle layers
};

enum layers {
	_BASE = 0,
	_WASD,
	_OSU,
};

void cycle_led(void);

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

	[_BASE] = LAYOUT(
		KC_7, KC_8, KC_9, \
		KC_4, KC_5, KC_6, \
		KC_1, KC_2, KC_3, \
		KC_LC, KC_0, KC_DOT \
	),
  
	[_WASD] = LAYOUT(
		KC_R, KC_F, KC_SPC, \
		KC_E, KC_D, KC_ENT, \
		KC_W, KC_S, KC_LC, \
		KC_Q, KC_A, KC_LSFT \
	),

    [_OSU] = LAYOUT(
		KC_F, KC_VOLU, KC_VOLD, \
		KC_D, KC_RIGHT, KC_ENT, \
		KC_UP, KC_DOWN, KC_LC, \
		KC_ESC, KC_LEFT, KC_WHOM \
	),
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
        default:
            return true;
    }
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