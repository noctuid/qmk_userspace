#pragma once

/* * Tap-Hold Settings */
// default is 200; not used for most of my dual-role keys, which just always act
// as modifier when held (see get_hold_on_other_key_press for exceptions);
// increase to be safe because I normally trigger space as modifier with
// permissive hold and have had issues with accidentally triggering the modifier
// on space when typing (though this was caused only by my e key; one of the
// keycap prongs broke, and it looked like pressing once was triggering the key
// multiple times; after replacing the key/keycap, the problem seemed to go
// away)
#define TAPPING_TERM 400

// use get_hold_on_other_key_press for configuring (I use dumb mode so keys mod
// tap keys always act as modifiers when pressed at the same time as another key
// for all keys except a couple):
#define HOLD_ON_OTHER_KEY_PRESS_PER_KEY

// don't treat mod tap keys specially; honor PERMISSIVE_HOLD
// this is now the default behavior
// #define IGNORE_MOD_TAP_INTERRUPT

// have tap-hold key act as hold (even if tapping term duration has not passed)
// when another key overlaps completely, i.e. the other key is pressed and
// released while the tap-hold key is still held; only applies for keys not
// handled by HOLD_ON_OTHER_KEY_PRESS (i.e. keys get_hold_on_other_key_press
// returns false for)
#define PERMISSIVE_HOLD

// disable tapping then holding tap-hold keys to repeat tap action
// occasionally using for backspace, so commented
// #define QUICK_TAP_TERM 0
// instead, define per-key and only allow for backspace (other wise a rapid
// press of escape followed by holding it will repeat escape, which I would
// never want, instead of shift)
#define QUICK_TAP_TERM_PER_KEY


/* * Cirque Trackpad Settings */
// doesn't work
// #define CIRQUE_PINNACLE_SECONDARY_TAP_ENABLE
