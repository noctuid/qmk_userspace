// Copyright 2024 splitkb.com (support@splitkb.com)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "os_detection.h"

// * Layers
enum layers {
    _COLEMAK_DHK = 0,
    _QWERTY,
    _TEKKEN,
    _FIGHT,
    _NAV,
    _SYM,
    _FUNCTION,
    _ADJUST,
};

#define QWERTY DF(_QWERTY)
#define TEKKEN DF(_TEKKEN)
#define FIGHT DF(_FIGHT)
#define COLEMAK DF(_COLEMAK_DHK)

#define ADJUST MO(_ADJUST)
#define FKEYS MO(_FUNCTION)

// * Key Aliases
#define C_BSPC C(KC_BACKSPACE)
#define C_TAB C(KC_TAB)
#define S_TAB S(KC_TAB)
#define CS_TAB C(S(KC_TAB))


#define KC_C_A C(KC_LALT)

// * Tap Dance
enum td_keycodes {
    TD_DEBUG_FLASH,
};

void quantum_tap_dance(tap_dance_state_t *state, void *user_data) {
    switch (state->count) {
        case 1:
            debug_enable ^= 1;
            break;
        case 2:
            // bootloader for flashing
            reset_keyboard();
            break;
    }
}

tap_dance_action_t tap_dance_actions[] = {
    // tap once to toggle debug mode, twice for bootloader
    [TD_DEBUG_FLASH] = ACTION_TAP_DANCE_FN(quantum_tap_dance),
};

#define DB_FLASH TD(TD_DEBUG_FLASH)

// * Dual-Role/Tap-Hold Keys and Custom Functionality
// tap-hold/dual-role aliases
#define DR_CTL MT(MOD_LCTL, KC_TAB)
#define DR_SFT MT(MOD_LSFT, KC_ESC)
#define DR_NAVS LT(_NAV, KC_SPACE)
// sxhkd hotkeys
#define DR_HOT MT(MOD_LGUI | MOD_LCTL, KC_LEFT_BRACKET)

// win/super/window management
#define DR_WIN MT(MOD_LGUI, KC_LEFT_PAREN)
#define DR_SYM LT(_SYM, KC_ENTER)
#define DR_ALT MT(MOD_LALT, KC_LEFT_CURLY_BRACE)

#define DR_NAV LT(_NAV, KC_BACKSPACE)
#define DR_RAT LT(_NAV, KC_DELETE)

#define DR_Z LT(_NAV, KC_Z)

// to be able to access media previous on nav layer
#define DR_SCAB MT(MOD_LSFT, CS_TAB)

// necessary since QMK's dual-role implementation is ...
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
    case C_TAB:
        os_variant_t os = detected_host_os();
        if (record->event.pressed) {
            if (os == OS_MACOS || os == OS_IOS) {
                register_code16(G(KC_TAB));
            } else {
                register_code16(C(KC_TAB));
            }
        } else {
            if (os == OS_MACOS || os == OS_IOS) {
                unregister_code16(G(KC_TAB));
            } else {
                unregister_code16(C(KC_TAB));
            }
        }
        // Return false to ignore further processing of key
        return false;
    case DR_WIN:
        // necessary since mod-tap only supports basic keycodes by default
        if (record->tap.count && record->event.pressed) {
            tap_code16(KC_LEFT_PAREN);
            // Return false to ignore further processing of key
            return false;
        }
        break;
    case DR_ALT:
        // necessary since mod-tap only supports basic keycodes by default
        if (record->tap.count && record->event.pressed) {
            tap_code16(KC_LEFT_CURLY_BRACE);
            // Return false to ignore further processing of key
            return false;
        }
        break;
    case DR_SCAB:
        // necessary since mod-tap only supports basic keycodes by default
        if (record->tap.count && record->event.pressed) {
            os_variant_t os = detected_host_os();
            if (os == OS_MACOS || os == OS_IOS) {
                tap_code16(G(S(KC_TAB)));
            } else {
                tap_code16(C(S(KC_TAB)));
            }
            // Return false to ignore further processing of key
            return false;
        }
        break;
    /* case RGB */
    }
    // also toggle
    return true;
}

// dumb mode for every key except z (which is potentially typed in rapid sucession with other keys)
bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case DR_Z:
            // do not immediately select the hold action when another key is pressed
            return false;
        case DR_NAVS:
            // do not immediately select the hold action when another key is pressed
            return false;
        default:
            // select the hold action when another key is pressed
            return true;
    }
}

// only allow double tap then hold to key-repeat for backspace
uint16_t get_quick_tap_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case DR_NAV:
            return QUICK_TAP_TERM;
        default:
            // disable by default
            return 0;
    }
}

// * Key Overrides
// shift + next for previous
const key_override_t media_next_key_override = ko_make_basic(MOD_MASK_SHIFT, KC_MEDIA_NEXT_TRACK, KC_MEDIA_PREV_TRACK);

// This globally defines all key overrides to be used
const key_override_t *key_overrides[] = {
    &media_next_key_override
};



// * Keymaps
// full key list:
// https://docs.qmk.fm/#/keycodes

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/*
 * Base Layer: Colemak Dhk
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |RGB Next|   Q  |   W  |   F  |   P  |   B  |                              |   J  |   L  |   U  |   Y  | ;  : |  KP +  |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * | RGB Tog|   A  |   R  |   S  |   T  |   G  |                              |   K  |   N  |   E  |   I  |   O  |  ' "   |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * | Quantum|      |   X  |   C  |   D  |   V  | Nav/Z|CapsWd|  |F-keys|Rat/Dl|   M  |   H  | ,  < | . >  | /  ? |        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |Adjust| LCTL | LSFT | Space|Hotkey|  | Super| Nav  | Sym  | Alt  | Menu |
 *                        |      | Tab  | Esc  |      |  [   |  |   (  | Bkspc| Enter|   {  |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_COLEMAK_DHK] = LAYOUT_split_3x6_5_hlc(
     RGB_MOD , KC_Q   , KC_W   , KC_F   , KC_P   , KC_B   ,                                        KC_J  , KC_L  , KC_U  , KC_Y   ,KC_SCLN, KC_PPLS,
     RGB_TOG , KC_A   , KC_R   , KC_S   , KC_T   , KC_G   ,                                        KC_K  , KC_N  , KC_E  , KC_I   , KC_O  ,KC_QUOTE,
     DB_FLASH, _______, KC_X   , KC_C   , KC_D   , KC_V   , DR_Z  ,CW_TOGG,     FKEYS   , DR_RAT , KC_M  , KC_H  ,KC_COMM, KC_DOT ,KC_SLSH,KC_ENTER,
                                 ADJUST , DR_CTL , DR_SFT ,DR_NAVS, DR_HOT,     DR_WIN  , DR_NAV , DR_SYM, DR_ALT, KC_APP,

     KC_MUTE, KC_NO,  KC_NO, KC_NO, KC_NO,                                                                 KC_MUTE, KC_NO, KC_NO, KC_NO, KC_NO
    ),

/*
 * Base Layer: QWERTY
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |RGB Next|   Q  |   W  |   E  |   R  |   T  |                              |   Y  |   U  |   I  |   O  |   P  |  KP +  |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * | RGB Tog|   A  |   S  |   D  |   F  |   G  |                              |   H  |   J  |   K  |   L  |  ; : |  ' "   |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * | Quantum|      |   X  |   C  |   V  |   B  | Nav/Z|CapsWd|  |F-keys|Rat/Dl|   N  |   M  | ,  < | . >  | /  ? |        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |Adjust| LCTL | LSFT | Space|  Alt |  | Super| Space| Sym  | Alt  | Menu |
 *                        |      | Tab  | Esc  |      |      |  |   (  |      | Enter|  {   |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_QWERTY] = LAYOUT_split_3x6_5_hlc(
     RGB_MOD , KC_Q   , KC_W    , KC_E   , KC_R   , KC_T   ,                                        KC_Y  , KC_U  , KC_I  , KC_O   , KC_P  , KC_PPLS,
     RGB_TOG , KC_A   , KC_S    , KC_D   , KC_F   , KC_G   ,                                        KC_H  , KC_J  , KC_K  , KC_L   ,KC_SCLN,KC_QUOTE,
     DB_FLASH, _______, KC_X    , KC_C   , KC_V   , KC_B   , DR_NAV,CW_TOGG,     FKEYS   , DR_RAT , KC_N  , KC_M  ,KC_COMM, KC_DOT ,KC_SLSH, _______,
                                  ADJUST , DR_CTL , DR_SFT , KC_SPC,KC_LALT,     DR_WIN  , KC_SPC , DR_SYM, DR_ALT, KC_APP,


     KC_MUTE, KC_NO,  KC_NO, KC_NO, KC_NO,                                                                  KC_MUTE, KC_NO, KC_NO, KC_NO, KC_NO
    ),

/*
 * Base Layer: Tekken
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |RGB Next|   Q  |   W  |   E  |   R  |   T  |                              |   Y  |   U  |   I  |   O  |   P  |  KP +  |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * | RGB Tog|   A  |   S  |   D  |   F  |   G  |                              |   H  |   J  |   K  |   L  |  ; : |  ' "   |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * | Quantum|      |   X  |   C  |   V  |   B  | Nav/Z|CapsWd|  |F-keys|Rat/Dl|   N  |   M  | ,  < | . >  | /  ? |        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |Adjust| LCTL | LSFT | Space|Hotkey|  |   F  |   ,  |   S  |  Alt | Menu |
 *                        |      | Tab  | Esc  |      |  [   |  |      |      |      |   {  |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_TEKKEN] = LAYOUT_split_3x6_5_hlc(
     RGB_MOD , KC_Q   , KC_W    , KC_E   , KC_R   , KC_T   ,                                        KC_Y  , KC_U  , KC_I  , KC_O   , KC_P  , KC_PPLS,
     RGB_TOG , KC_A   , KC_S    , KC_D   , KC_F   , KC_G   ,                                        KC_H  , KC_J  , KC_K  , KC_L   ,KC_SCLN,KC_QUOTE,
     DB_FLASH, _______, KC_X    , KC_C   , KC_V   , KC_B   , DR_NAV,CW_TOGG,     FKEYS   , DR_SYM , KC_N  , KC_M  ,KC_COMM, KC_DOT ,KC_SLSH, _______,
                                  ADJUST , DR_CTL , DR_SFT , KC_SPC, DR_HOT,     KC_F    , KC_COMM, KC_S  , DR_ALT, KC_APP,

     KC_MUTE, KC_NO,  KC_NO, KC_NO, KC_NO,                                                                  KC_MUTE, KC_NO, KC_NO, KC_NO, KC_NO
    ),

/*
 * Base Layer: Fight
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |RGB Next|   Q  |   W  |   F  |   P  |   B  |                              |   J  |   L  |   U  |   Y  | ;  : |  KP +  |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * | RGB Tog|   A  |   R  |   S  |   T  |   G  |                              |   K  |   N  |   E  |   I  |   O  |  ' "   |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * | Quantum|      |   X  |   C  |   D  |   V  | Nav/Z|CapsWd|  |F-keys|Rat/Dl|   M  |   H  | ,  < | . >  | /  ? |        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |Adjust| LCTL | LSFT | Space|Hotkey|  |   R  |   M  |   T  | Alt  | Menu |
 *                        |      | Tab  | Esc  |      |  [   |  |      |      |      |   {  |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_FIGHT] = LAYOUT_split_3x6_5_hlc(
     RGB_MOD , KC_Q   , KC_T   , KC_S   , KC_R   , KC_B   ,                                        KC_J  , KC_L  , KC_U  , KC_Y   ,KC_SCLN, KC_PPLS,
     RGB_TOG , KC_A   , KC_R   , KC_S   , KC_T   , KC_G   ,                                        KC_K  , KC_N  , KC_E  , KC_I   , KC_O  ,KC_QUOTE,
     DB_FLASH, _______, KC_X   , KC_C   , KC_D   , KC_V   , DR_Z  ,CW_TOGG,     FKEYS   , DR_RAT , KC_M  , KC_H  ,KC_COMM, KC_DOT ,KC_SLSH, _______,
                                 ADJUST , DR_CTL , KC_D   , KC_SPC, DR_HOT,     KC_R    , KC_M   , KC_T  , DR_ALT, KC_APP,

     KC_MUTE, KC_NO,  KC_NO, KC_NO, KC_NO,                                                                 KC_MUTE, KC_NO, KC_NO, KC_NO, KC_NO
    ),

/*
 * Nav Layer: Navigation, media
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |        |      |C+bspc| Rat↑ | PrtSc|      |                              |Brght↓|PgDown| Home | End  | PgUp | Brght↑ |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |      | Rat← | Rat↓ | Rat→ |      |                              |M Next|   ↓  |   →  |   ↑  | Play |        |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |        |      | Rat1 | Rat2 | Rat3 |      |Wheel↓|Wheel↑|  |      |      |VolMut|   ←  | VolDn| VolUp|      |        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      | S-Tab| LSFT | C-Tab|      |  |      |      |      |      |      |
 *                        |      |      |CS-Tab|      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_NAV] = LAYOUT_split_3x6_5_hlc(
      _______, _______, C_BSPC , KC_MS_U, KC_PSCR, _______,                                     KC_BRID, KC_PGDN, KC_HOME, KC_END  , KC_PGUP, KC_BRIU,
      _______, _______, KC_MS_L, KC_MS_D, KC_MS_R, _______,                                     KC_MNXT, KC_DOWN, KC_UP  , KC_RIGHT, KC_MPLY, _______,
      _______, _______, KC_BTN1, KC_BTN2, KC_BTN3, _______, KC_WH_D, KC_WH_U, _______, _______, KC_MUTE, KC_LEFT, KC_VOLD, KC_VOLU , _______, _______,
                                 _______, S_TAB  , DR_SCAB, C_TAB  , _______, _______, _______, _______, _______, _______,

      KC_MUTE, KC_NO,  KC_NO, KC_NO, KC_NO,                                                               KC_MUTE, KC_NO, KC_NO, KC_NO, KC_NO
    ),

/*
 * Sym Layer: Numbers and symbols
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |        |  `   |  =   |  ]   |  _   |  \   |                              |   @  |  -   |  )   |  }   |  !   |        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |  1   |  2   |  3   |  4   |  |   |                              |   &  |  7   |  8   |  9   |  0   |        |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |        |      |  ~   |  #   |  5   |  ^   |      |      |  |      |      |   $  |  6   |  +   |  *   |  %   |        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_SYM] = LAYOUT_split_3x6_5_hlc(
     QK_LLCK , KC_GRV , KC_EQL , KC_RBRC, KC_UNDS, KC_BSLS,                                     KC_AT  , KC_MINS, KC_RPRN, KC_RCBR, KC_EXLM, _______,
     _______ , KC_1   , KC_2   , KC_3   , KC_4   , KC_PIPE,                                     KC_AMPR, KC_7   , KC_8   , KC_9   , KC_0   , _______,
     _______ , _______, KC_TILD, KC_HASH, KC_5   , KC_CIRC, _______, _______, _______, _______, KC_DLR , KC_6   , KC_ASTR, KC_PLUS, KC_PERC, _______,
                                 _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,

     KC_MUTE, KC_NO,  KC_NO, KC_NO, KC_NO,                                                               KC_MUTE, KC_NO, KC_NO, KC_NO, KC_NO
    ),

/*
 * Function Layer: Function keys, other keys I don't really need
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |        |      | F11  | F12  |      |      |                              |      |      | Power| Sleep| Wake |        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |  F1  |  F2  |  F3  |  F4  |      |                              |      |  F7  |  F8  |  F9  |  F10 |        |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |        |      |      |      |  F5  |      |      |      |  |      |      |      |  F6  |      |      |      |        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      | C-Alt|      |      |  |      |      |      |      |      |
 *                        |      |      | (VT) |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_FUNCTION] = LAYOUT_split_3x6_5_hlc(
     _______, _______, KC_F11 , KC_F12 , _______, _______,                                     _______, KC_PWR , KC_SLEP, KC_WAKE, _______, _______,
     _______, KC_F1  , KC_F2  , KC_F3  , KC_F4  , _______,                                     _______, KC_F7  , KC_F8  , KC_F9  , KC_F10  , _______,
     _______, _______, _______, _______, KC_F5  , _______, _______, _______, _______, _______, _______, KC_F6  , _______, _______, _______, _______,
                                _______, _______, KC_C_A , _______, _______, _______, _______, _______, _______, _______,

     KC_MUTE, KC_NO, KC_NO, KC_NO, KC_NO,                                                                KC_MUTE, KC_NO, KC_NO, KC_NO, KC_NO
    ),

/*
 * Adjust Layer: Default layer settings, RGB, and other keyboard commands
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |        |QWERTY|      | FIGHT|      |      |                              |      |      |      |      |      |        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |      |      |      |TEKKEN|      |                              | TOG  | HUI  | SAI  | VAI  | SPD+ |        |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |        |      |      |Colmak| Debug|      |      |      |  |      |      |      | HUD  | SAD  | VAD  | SPD- |        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_ADJUST] = LAYOUT_split_3x6_5_hlc(
      _______, QWERTY , _______, FIGHT  , _______, _______,                                    _______, _______, _______, _______, _______, _______,
      _______, _______, _______, _______, TEKKEN , _______,                                    RGB_TOG, RGB_HUI, RGB_SAI, RGB_VAI, RGB_SPI, _______,
      _______, _______, _______, COLEMAK, DB_TOGG, _______,_______, _______, _______, _______, _______, RGB_HUD, RGB_SAD, RGB_VAD, RGB_SPD, _______,
                                 _______, _______, _______,_______, _______, _______, _______, _______, _______, _______,

      KC_MUTE, KC_NO,  KC_NO, KC_NO, KC_NO,                                                             KC_MUTE, KC_NO, KC_NO, KC_NO, KC_NO
    ),

// /*
//  * Halcyon Layer template
//  *
//  * ,-------------------------------------------.                              ,-------------------------------------------.
//  * |        |      |      |      |      |      |                              |      |      |      |      |      |        |
//  * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
//  * |        |      |      |      |      |      |                              |      |      |      |      |      |        |
//  * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
//  * |        |      |      |      |      |      |      |      |  |      |      |      |      |      |      |      |        |
//  * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
//  *                        |      |      |      |      |      |  |      |      |      |      |      |
//  *                        |      |      |      |      |      |  |      |      |      |      |      |
//  *                        `----------------------------------'  `----------------------------------'
//  * ,-----------------------------------.                                              ,-----------------------------------.
//  * |      |      |       |      |      |                                              |      |      |       |      |      |
//  * `-----------------------------------'                                              `-----------------------------------'
//  */
//     [_LAYERINDEX] = LAYOUT_split_3x6_5_hlc(
//       _______, _______, _______, _______, _______, _______,                                     _______, _______, _______, _______, _______, _______,
//       _______, _______, _______, _______, _______, _______,                                     _______, _______, _______, _______, _______, _______,
//       _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
//                                  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
//
//       _______, _______, _______, _______, _______,                                                       _______, _______, _______, _______, _______
//     ),
};


// Local Variables:
// fill-column: 200
// End:
