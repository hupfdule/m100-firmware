// -*- mode: c++ -*-
// Copyright 2016-2022 Keyboardio, inc. <jesse@keyboard.io>
// See "LICENSE" for license details

// ---- #includes {{{1
/**
 * These #include directives pull in the Kaleidoscope firmware core,
 * as well as the Kaleidoscope plugins we use in the Model 100's firmware
 */

// The Kaleidoscope core
#include "Kaleidoscope.h"

// Support for storing the keymap in EEPROM
#include "Kaleidoscope-EEPROM-Settings.h"
#include "Kaleidoscope-EEPROM-Keymap.h"

// Support for communicating with the host via a simple Serial protocol
#include "Kaleidoscope-FocusSerial.h"

// Support for querying the firmware version via Focus
#include "Kaleidoscope-FirmwareVersion.h"

// Support for keys that move the mouse
#include "Kaleidoscope-MouseKeys.h"

// Support for macros
#include "Kaleidoscope-Macros.h"

// Support for controlling the keyboard's LEDs
#include "Kaleidoscope-LEDControl.h"

// Support for "Numpad" mode, which is mostly just the Numpad specific LED mode
//#include "Kaleidoscope-NumPad.h"

// Support for the "Boot greeting" effect, which pulses the 'LED' button for 10s
// when the keyboard is connected to a computer (or that computer is powered on)
#include "Kaleidoscope-LEDEffect-BootGreeting.h"

// Support for LED modes that set all LEDs to a single color
#include "Kaleidoscope-LEDEffect-SolidColor.h"

// Support for an LED mode that makes all the LEDs 'breathe'
#include "Kaleidoscope-LEDEffect-Breathe.h"

// Support for an LED mode that makes a red pixel chase a blue pixel across the keyboard
#include "Kaleidoscope-LEDEffect-Chase.h"

// Support for LED modes that pulse the keyboard's LED in a rainbow pattern
#include "Kaleidoscope-LEDEffect-Rainbow.h"

// Support for an LED mode that lights up the keys as you press them
#include "Kaleidoscope-LED-Stalker.h"

// Support for an LED mode that prints the keys you press in letters 4px high
#include "Kaleidoscope-LED-AlphaSquare.h"

// Support for shared palettes for other plugins, like Colormap below
#include "Kaleidoscope-LED-Palette-Theme.h"

// Support for an LED mode that prints the keys you press in letters 4px high
#include "Kaleidoscope-LED-ActiveModColor.h"

// Support for assingnign different chars for shifted and unshifted positions
#include "Kaleidoscope-CharShift.h"

// Support for an LED mode that lets one configure per-layer color maps
#include "Kaleidoscope-Colormap.h"

// Support for turning the LEDs off after a certain amount of time
#include "Kaleidoscope-IdleLEDs.h"

// Support for setting and saving the default LED mode
#include "Kaleidoscope-DefaultLEDModeConfig.h"

// Support for changing the brightness of the LEDs
#include "Kaleidoscope-LEDBrightnessConfig.h"

// Support for Keyboardio's internal keyboard testing mode
#include "Kaleidoscope-HardwareTestMode.h"

// Support for host power management (suspend & wakeup)
#include "Kaleidoscope-HostPowerManagement.h"

// Support for magic combos (key chords that trigger an action)
#include "Kaleidoscope-MagicCombo.h"

// Support for USB quirks, like changing the key state report protocol
#include "Kaleidoscope-USB-Quirks.h"

// Support for secondary actions on keys
#include "Kaleidoscope-Qukeys.h"

// Support for one-shot modifiers and layer keys
#include "Kaleidoscope-OneShot.h"
#include "Kaleidoscope-Escape-OneShot.h"

// Support for dynamic, Chrysalis-editable macros
#include "Kaleidoscope-DynamicMacros.h"

// Support for SpaceCadet keys
#include "Kaleidoscope-SpaceCadet.h"

// Support for editable layer names
#include "Kaleidoscope-LayerNames.h"

// Support for the GeminiPR Stenography protocol
#include "Kaleidoscope-Steno.h"

// Support for the Chord plugin
#include <Kaleidoscope-Chord.h>

// Support for symbolic names for a german QWERTZ layout
#include "QWERTZ_T1.h"

// Support for additional symbolic names
#include "hupfdule-keys.h"

// }}}1

// ---- Macros {{{1
/**
 * This 'enum' is a list of all the macros used by the Model 100's firmware
 * The names aren't particularly important. What is important is that each
 * is unique.
 *
 * These are the names of your macros. They'll be used in two places.
 * The first is in your keymap definitions. There, you'll use the syntax
 * `M(MACRO_NAME)` to mark a specific keymap position as triggering `MACRO_NAME`
 *
 * The second usage is in the 'switch' statement in the `macroAction` function.
 * That switch statement actually runs the code associated with a macro when
 * a macro key is pressed.
 */
enum {
  MACRO_VERSION_INFO,
  MACRO_ANY,
  MACRO_UNDEAD_CARET,
  MACRO_UNDEAD_BACKTICK,
  MACRO_QU,
  MACRO_UNDERSCORE_SEPARATOR,
  MACRO_DIR_UP,
  MACRO_APOSTROPHE_T,
};

/**
 * versionInfoMacro handles the 'firmware version info' macro
 * When a key bound to the macro is pressed, this macro
 * prints out the firmware build information as virtual keystrokes
 */
static void versionInfoMacro(uint8_t key_state) {
  if (keyToggledOn(key_state)) {
    Macros.type(PSTR("Keyboardio Model 100 - Firmware version "));
    Macros.type(PSTR(KALEIDOSCOPE_FIRMWARE_VERSION));
  }
}

/**
 * anyKeyMacro is used to provide the functionality of the 'Any' key.
 *
 * When the 'any key' macro is toggled on, a random alphanumeric key is
 * selected. While the key is held, the function generates a synthetic
 * keypress event repeating that randomly selected key.
 */
static void anyKeyMacro(KeyEvent &event) {
  if (keyToggledOn(event.state)) {
    event.key.setKeyCode(Key_A.getKeyCode() + (uint8_t)(millis() % 36));
    event.key.setFlags(0);
  }
}

/**
 * Simulate the keypress of the (dead) Caret (^) and afterwards simulate the
 * keypress of the spacebar to behave like a non-dead Caret.
 */
static const macro_t *undeadCaretMacro(KeyEvent &event) {
  if (keyToggledOn(event.state)) {
    return MACRO(T(Caret),
                 T(Spacebar)
           );
  }
  return MACRO_NONE;
}

/**
 * Simulate the keypress of the shifted (dead) Acute (') and afterwards simulate the
 * keypress of the spacebar to behave like a non-dead Backtick.
 */
static const macro_t *undeadBacktickMacro(KeyEvent &event) {
  if (keyToggledOn(event.state)) {
    return MACRO(D(LeftShift),
                 T(Backtick),
                 U(LeftShift),
                 T(Spacebar)
           );
  }
  return MACRO_NONE;
}

/**
 * Press the Q key and directly afterwards the U key as this is the most
 * used combination in written texts.
 */
static const macro_t *quMacro(KeyEvent &event) {
  if (keyToggledOn(event.state)) {
    Macros.tap(Key_Q);
    // Cancel a OneShot modifier to be able to write “Qu”.
    // See  https://community.keyboard.io/t/macro-that-respects-one-shot-modifier-keys/6163
    OneShot.cancel();
    Macros.tap(Key_U);
    return MACRO_NONE;
  }
  return MACRO_NONE;
}

/**
 * Write _-_ which is used often as a separator.
 */
static const macro_t *underscoreSeparatorMacro(KeyEvent &event) {
  if (keyToggledOn(event.state)) {
    return MACRO(D(LeftShift),
                 T(Minus),
                 U(LeftShift),
                 T(Minus),
                 D(LeftShift),
                 T(Minus),
                 U(LeftShift)
           );
  }
  return MACRO_NONE;
}

/**
 * Write ../ which is often used in the shell to specify the parent
 * directory.
 */
static const macro_t *dirUpMacro(KeyEvent &event) {
  if (keyToggledOn(event.state)) {
    return MACRO(T(Period),
                 T(Period),
                 T(Slash)
           );
  }
  return MACRO_NONE;
}

/**
 * Enter “’t” as this is a common suffix in english, e.g. in “don’t”.
 */
static const macro_t *apostropheTMacro(KeyEvent &event) {
  if (keyToggledOn(event.state)) {
    return MACRO(T(CapsLock),
                 T(GreaterThan),
                 T(SingleQuote),
                 T(T)
           );
  }
  return MACRO_NONE;
}

/**
 * macroAction dispatches keymap events that are tied to a macro
 * to that macro. It takes two uint8_t parameters.
 *
 * The first is the macro being called (the entry in the 'enum' earlier in this file).
 * The second is the state of the keyswitch. You can use the keyswitch state to figure out
 * if the key has just been toggled on, is currently pressed or if it's just been released.
 *
 * The 'switch' statement should have a 'case' for each entry of the macro enum.
 * Each 'case' statement should call out to a function to handle the macro in question.
 */
const macro_t *macroAction(uint8_t macro_id, KeyEvent &event) {
  switch (macro_id) {

  case MACRO_VERSION_INFO:
    versionInfoMacro(event.state);
    break;

  case MACRO_ANY:
    anyKeyMacro(event);
    break;

  case MACRO_UNDEAD_CARET:
    return undeadCaretMacro(event);
    break;

  case MACRO_UNDEAD_BACKTICK:
    return undeadBacktickMacro(event);
    break;

  case MACRO_QU:
    return quMacro(event);
    break;

  case MACRO_UNDERSCORE_SEPARATOR:
    return underscoreSeparatorMacro(event);
    break;

  case MACRO_DIR_UP:
    return dirUpMacro(event);
    break;

  case MACRO_APOSTROPHE_T:
    return apostropheTMacro(event);
    break;
  }


  return MACRO_NONE;
}

// }}}1

// ---- Keymaps {{{1
/** {{{3
 * The Model 100's key layouts are defined as 'keymaps'. By default, there are three
 * keymaps: The standard QWERTY keymap, the "Function layer" keymap and the "Numpad"
 * keymap.
 *
 * Each keymap is defined as a list using the 'KEYMAP_STACKED' macro, built
 * of first the left hand's layout, followed by the right hand's layout.
 *
 * Keymaps typically consist mostly of `Key_` definitions. There are many, many keys
 * defined as part of the USB HID Keyboard specification. You can find the names
 * (if not yet the explanations) for all the standard `Key_` defintions offered by
 * Kaleidoscope in these files:
 *    https://github.com/keyboardio/Kaleidoscope/blob/master/src/kaleidoscope/key_defs/keyboard.h
 *    https://github.com/keyboardio/Kaleidoscope/blob/master/src/kaleidoscope/key_defs/consumerctl.h
 *    https://github.com/keyboardio/Kaleidoscope/blob/master/src/kaleidoscope/key_defs/sysctl.h
 *    https://github.com/keyboardio/Kaleidoscope/blob/master/src/kaleidoscope/key_defs/keymaps.h
 *
 * Additional things that should be documented here include
 *   using ___ to let keypresses fall through to the previously active layer
 *   using XXX to mark a keyswitch as 'blocked' on this layer
 *   using ShiftToLayer() and LockLayer() keys to change the active keymap.
 *   keeping NUM and FN consistent and accessible on all layers
 *
 * The PROG key is special, since it is how you indicate to the board that you
 * want to flash the firmware. However, it can be remapped to a regular key.
 * When the keyboard boots, it first looks to see whether the PROG key is held
 * down; if it is, it simply awaits further flashing instructions. If it is
 * not, it continues loading the rest of the firmware and the keyboard
 * functions normally, with whatever binding you have set to PROG. More detail
 * here: https://community.keyboard.io/t/how-the-prog-key-gets-you-into-the-bootloader/506/8
 *
 * The "keymaps" data structure is a list of the keymaps compiled into the firmware.
 * The order of keymaps in the list is important, as the ShiftToLayer(#) and LockLayer(#)
 * macros switch to key layers based on this list.
 *
 * A key defined as 'ShiftToLayer(FUNCTION)' will switch to FUNCTION while held.
 * Similarly, a key defined as 'LockLayer(NUMPAD)' will switch to NUMPAD when tapped.
 */

/**
 * Layers are "0-indexed" -- That is the first one is layer 0. The second one is layer 1.
 * The third one is layer 2.
 * This 'enum' lets us use names like QWERTY, FUNCTION, and NUMPAD in place of
 * the numbers 0, 1 and 2.
 * }}}3
 */
enum {
  BONE,
  QWERTY,
  NUMPAD,
  FUNCTION,
  SYMBOL,
  UMLAUT,
  MOUSE,
};  // layers


/*
 * This comment temporarily turns off astyle's indent enforcement
 * so we can make the keymaps actually resemble the physical key layout better
 */
// clang-format off

KEYMAPS(

  [BONE] = KEYMAP_STACKED  // {{{2
  (___,                   Key_1,  Key_2,         Key_3,       Key_4,              Key_5,       ___,
   Key_PageUp,            Key_Q,  Key_T,         Key_U,       Key_A,              Key_F,       Key_Tab,
   LT(NUMPAD, CapsLock),  Key_C,  Key_D,         Key_E,       Key_I,              Key_O,
   Key_PageDown,          Key_X,  Key_V,         CS(0),       CS(1),              Key_Minus,   Key_Escape,
   OSM(LeftShift),  Key_Backspace,   OSM(LeftAlt),   OSM(LeftControl),
   ShiftToLayer(FUNCTION),

   LockLayer(SYMBOL),     Key_6,  Key_7,         Key_8,       Key_9,              Key_0,       LockLayer(NUMPAD),
   Key_Enter,             Key_P,  Key_H,         Key_L,       Key_M,              Key_W,       Key_Eszett,
                          Key_B,  Key_N,         Key_R,       Key_S,              Key_G,       Key_J,
   Key_RightAlt,          Key_Y,  Key_Z,         Key_K,       Key_Backslash,      Key_Slash,   OSL(UMLAUT),
   OSM(RightControl), Key_LeftGui,    Key_Spacebar,  OSM(RightShift),
   ShiftToLayer(FUNCTION)), // }}}2


  [QWERTY] = KEYMAP_STACKED // {{{2
  (___, Key_1, Key_2, Key_3, Key_4, Key_5, ___,
   ___, Key_Q, Key_W, Key_E, Key_R, Key_T, ___,
   ___, Key_A, Key_S, Key_D, Key_F, Key_G,
   ___, Key_Y, Key_X, Key_C, Key_V, Key_B, ___,
   ___, ___, ___, ___,
   ___,

   ___, Key_6, Key_7, Key_8,     Key_9,      Key_0,         ___,
   ___, Key_Z, Key_U, Key_I,     Key_O,      Key_P,         Key_Equals,
        Key_H, Key_J, Key_K,     Key_L,      Key_Semicolon, Key_Quote,
   ___, Key_N, Key_M, Key_Comma, Key_Period, Key_Slash,     Key_Minus,
   ___, ___, ___, ___,
   ___), // }}}2


  [NUMPAD] =  KEYMAP_STACKED // {{{2
  (___, ___, ___, ___, ___, ___, ___,
   ___, ___, ___, ___, ___, ___, ___,
   ___, ___, ___, ___, ___, ___,
   ___, ___, ___, ___, ___, ___, ___,
   ___, ___, ___, ___,
   ___,

   M(MACRO_VERSION_INFO),  ___,                ___,   Key_Equals,    ___,   Key_KeypadSubtract, ___,
   ___,                    Key_Colon,          Key_7, Key_8,         Key_9, Key_KeypadAdd,      ___,
                           Key_Period,         Key_4, Key_5,         Key_6, Key_KeypadDivide,   Key_Equals,
   ___,                    Key_Comma,          Key_1, Key_2,         Key_3, Key_KeypadMultiply, Key_Enter,
   ___, ___, Key_0, Key_Enter,
   ___), // }}}2


  [FUNCTION] =  KEYMAP_STACKED // {{{2
  (___,                        Key_F1,                 Key_F2,                   Key_F3,                   Key_F4,           Key_F5,        Key_LEDEffectNext,
   Key_Tab,                    LCTRL(Key_W),           Key_mouseScrollL,         Key_mouseScrollUp,        Key_mouseScrollR, LALT(Key_F),   Key_Spacebar,
   LCTRL(Key_R),               LCTRL(Key_S),           Key_mouseBtnR,            Key_mouseScrollDn,        Key_mouseBtnL,    LALT(Key_B),
   ___,                        Key_PrintScreen,        Key_Insert,               Key_Home,                 Key_End,          Key_Delete,    Key_Enter,
   ___,                        Key_Delete,             ___,                      ___,
   ___,

   Consumer_ScanPreviousTrack, Key_F6,                 Key_F7,                   Key_F8,                   Key_F9,           Key_F10,       Key_F11,
   Consumer_PlaySlashPause,    Consumer_ScanNextTrack, Key_Escape,               Key_Tab,                  ___,              ___,           Key_F12,
                               Key_LeftArrow,          Key_DownArrow,            Key_UpArrow,              Key_RightArrow,   Key_Slash,     ___,
   Key_PcApplication,          Key_Backspace,          Consumer_VolumeDecrement, Consumer_VolumeIncrement, Consumer_Mute,    Key_Backslash, ___,
   ___,                        ___,                    Key_Enter,                ___,
   ___), // }}}2


  [SYMBOL] = KEYMAP_STACKED // {{{2
  (___, ___,                 ___,                 ___,                  ___,                   ___,             ___,
   ___, Key_Caret,           Key_Underscore,      Key_LeftBracket,      Key_RightBracket,      Key_DoubleQuote, ___,
   ___, Key_Backslash,       Key_Slash,           Key_LeftCurlyBracket, Key_RightCurlyBracket, Key_Asterisk,
   ___, Key_Pound,           Key_Tilde,           Key_Pipe,             Key_Dollar,            Key_Backtick,    ___,
   ___, ___,                 ___,                 ___,
   ___,

   ___, ___,                 ___,                 ___,                  ___,                   ___,             ___,
   ___, Key_ExclamationMark, Key_LessThan,        Key_GreaterThan,      Key_Equals,            Key_Percent,     ___,
        Key_QuestionMark,    Key_LeftParentheses, Key_RightParentheses, Key_Minus,             Key_Semicolon,   Key_Colon,
   ___, Key_Plus,            Key_DoubleQuote,     Key_SingleQuote,      Key_At,                Key_Ampersand,   ___,
   ___, ___,                 ___,                 ___,
   ___), // }}}2


  [UMLAUT] =  KEYMAP_STACKED // {{{2
  (___, ___,         ___,                   ___,         ___,             ___,                           ___,
   ___, M(MACRO_QU), M(MACRO_APOSTROPHE_T), Key_UUmlaut, Key_AUmlaut,     ___,                           ___,
   ___, ___,         Key_Eszett,            ___,         Key_J,           Key_OUmlaut,
   ___, ___,         ___,                   ___,         M(MACRO_DIR_UP), M(MACRO_UNDERSCORE_SEPARATOR), ___,
   ___, ___,         ___,                   ___,
   ___,

   ___, ___,         ___,                   ___,         ___,             ___,                           ___,
   ___, ___,         ___,                   ___,         ___,             ___,                           ___,
        ___,         ___,                   ___,         Key_Eszett,      ___,                           ___,
   ___, ___,         ___,                   ___,         ___,             ___,                           ___,
   ___, ___,         ___,                   ___,
   ___), // }}}2


  [MOUSE] =  KEYMAP_STACKED // {{{2
  (___, ___, ___,          ___,           ___,          ___,           ___,
   ___, ___, Key_mouseUpL, Key_mouseUp,   Key_mouseUpR, Key_mouseBtnR, ___,
   ___, ___, Key_mouseL,   Key_mouseDn,   Key_mouseR,   Key_mouseBtnL,
   ___, ___, Key_mouseDnL, ___,           Key_mouseDnR, Key_mouseBtnM, ___,
   ___, ___, ___,          ___,
   ___,

   ___, ___, ___,          ___,           ___,          ___,           ___,
   ___, ___, ___,          ___,           ___,          ___,           ___,
        ___, ___,          ___,           ___,          ___,           ___,
   ___, ___, ___,          ___,           ___,          ___,           ___,
   ___, ___, ___,          ___,
   ___) // }}}2
) // KEYMAPS(

/* Re-enable astyle's indent enforcement */
// clang-format on

// }}}1

// ---- Colors {{{1
// These 'solid' color effect definitions define a rainbow of
// LED color modes calibrated to draw 500mA or less on the
// Keyboardio Model 100.


static kaleidoscope::plugin::LEDSolidColor solidRed    (160,   0,   0);
static kaleidoscope::plugin::LEDSolidColor solidOrange (140,  70,   0);
static kaleidoscope::plugin::LEDSolidColor solidYellow (130, 100,   0);
static kaleidoscope::plugin::LEDSolidColor solidGreen  (  0, 160,   0);
static kaleidoscope::plugin::LEDSolidColor solidBlue   (  0,  70, 130);
static kaleidoscope::plugin::LEDSolidColor solidIndigo (  0,   0, 170);
static kaleidoscope::plugin::LEDSolidColor solidViolet (130,   0, 120);

// Colors names of the EGA palette, for convenient use in colormaps. Should
// match the palette definition below. Optional, one can just use the indexes
// directly, too.
enum {
  __B,  // BLACK → No Backlight
  BLUE,
  GREEN,
  CYAN,
  RED,
  MAGENTA,
  BROWN,
  LIGHT_GRAY,
  DARK_GRAY,
  BRIGHT_BLUE,
  BRIGHT_GREEN,
  BRIGHT_CYAN,
  BRIGHT_RED,
  BRIGHT_MAGENTA,
  YELLOW,
  WHITE
};

// Define an EGA palette. Conveniently, that's exactly 16 colors, just like the
// limit of LEDPaletteTheme.
PALETTE(
    CRGB(0x00, 0x00, 0x00),  // [0x0] black
    CRGB(0x00, 0x00, 0xaa),  // [0x1] blue
    CRGB(0x00, 0xaa, 0x00),  // [0x2] green
    CRGB(0x00, 0xaa, 0xaa),  // [0x3] cyan
    CRGB(0xaa, 0x00, 0x00),  // [0x4] red
    CRGB(0xaa, 0x00, 0xaa),  // [0x5] magenta
    CRGB(0xaa, 0x55, 0x00),  // [0x6] brown
    CRGB(0xaa, 0xaa, 0xaa),  // [0x7] light gray
    CRGB(0x55, 0x55, 0x55),  // [0x8] dark gray
    CRGB(0x55, 0x55, 0xff),  // [0x9] bright blue
    CRGB(0x55, 0xff, 0x55),  // [0xa] bright green
    CRGB(0x55, 0xff, 0xff),  // [0xb] bright cyan
    CRGB(0xff, 0x55, 0x55),  // [0xc] bright red
    CRGB(0xff, 0x55, 0xff),  // [0xd] bright magenta
    CRGB(0xff, 0xff, 0x55),  // [0xe] yellow
    CRGB(0xff, 0xff, 0xff)   // [0xf] white
)

COLORMAPS(
  [BONE] = COLORMAP_STACKED  // {{{2
  (__B, __B, __B, __B, __B, __B, __B,
   __B, __B, __B, __B, __B, __B, __B,
   __B, __B, __B, __B, __B, __B,
   __B, __B, __B, __B, __B, __B, __B,
   __B, __B, __B, __B,
   __B,

   __B, __B, __B, __B, __B, __B, __B,
   __B, __B, __B, __B, __B, __B, __B,
        __B, __B, __B, __B, __B, __B,
   __B, __B, __B, __B, __B, __B, __B,
   __B, __B, __B, __B,
   __B),     // }}}2


  [QWERTY] = COLORMAP_STACKED // {{{2
  (__B,   BROWN, BROWN, __B,   __B,   __B,   __B,
   __B,   __B,   __B,   __B,   __B,   __B,   __B,
   __B,   __B,   __B,   __B,   __B,   __B,
   __B,   __B,   __B,   __B,   __B,   __B,   __B,
   __B,   __B,   __B,   __B,
   __B,

   __B,   __B,   __B,   __B,   __B,   __B,   __B,
   __B,   __B,   __B,   __B,   __B,   __B,   __B,
          __B,   __B,   __B,   __B,   __B,   __B,
   __B,   __B,   __B,   __B,   __B,   __B,   __B,
   __B, __B,   __B,   __B,
   __B), // }}}2


  [NUMPAD] =  COLORMAP_STACKED // {{{2
  (__B,   __B,   __B,   __B,   __B,   __B,   __B,
   __B,   __B,   __B,   __B,   __B,   __B,   __B,
   __B,   __B,   __B,   __B,   __B,   __B,
   __B,   __B,   __B,   __B,   __B,   __B,   __B,
   __B,   __B,   __B,   __B,
   __B,

   __B,   __B,   __B,   BLUE,  __B,   BLUE,  __B,
   __B,   GREEN, RED,   RED,   RED,   BLUE,  __B,
          GREEN, RED,   RED,   RED,   BLUE,  BLUE,
   __B,   GREEN, RED,   RED,   RED,   BLUE,  YELLOW,
   __B, __B, RED, YELLOW,
   __B), // }}}2


  [FUNCTION] =  COLORMAP_STACKED // {{{2
  (__B,      DARK_GRAY, DARK_GRAY,    DARK_GRAY,    DARK_GRAY,    DARK_GRAY,  __B,
   __B,      YELLOW,    BRIGHT_GREEN, BRIGHT_GREEN, BRIGHT_GREEN, YELLOW,     __B,
   YELLOW,   YELLOW,    GREEN,        BRIGHT_GREEN, GREEN,        YELLOW,
   __B,      __B,       __B,          BLUE,         BLUE,         RED,        __B,
   __B,      RED,       __B,          __B,
   __B,

   MAGENTA,  DARK_GRAY, DARK_GRAY,    DARK_GRAY,    DARK_GRAY,    DARK_GRAY,  DARK_GRAY,
   MAGENTA,  MAGENTA,   __B,          __B,          __B,          __B,        DARK_GRAY,
             BLUE,      BLUE,         BLUE,         BLUE,         LIGHT_GRAY, __B,
   __B,      RED,       MAGENTA,      MAGENTA,      MAGENTA,      LIGHT_GRAY, __B,
   __B,      __B,       __B,          __B,
   __B),     // }}}2


  [SYMBOL] = COLORMAP_STACKED // {{{2
  (__B, __B,  __B,  __B,  __B,  __B,  __B,
   __B, BLUE, BLUE, BLUE, BLUE, BLUE, __B,
   __B, BLUE, BLUE, BLUE, BLUE, BLUE,
   __B, BLUE, BLUE, BLUE, BLUE, BLUE, __B,
   __B, __B,  __B,  __B,
   __B,

   __B, __B,  __B,  __B,  __B,  __B,  __B,
   __B, BLUE, BLUE, BLUE, BLUE, BLUE, __B,
        BLUE, BLUE, BLUE, BLUE, BLUE, BLUE,
   __B, BLUE, BLUE, BLUE, BLUE, BLUE, __B,
   __B, __B,  __B,  __B,
   __B), // }}}2


  [UMLAUT] = COLORMAP_STACKED // {{{2
  (__B, __B,   __B,          __B,         __B,          __B,         __B,
   __B, GREEN, GREEN,        BRIGHT_BLUE, BRIGHT_BLUE,  __B,         __B,
   __B, __B,   BRIGHT_GREEN, __B,         BRIGHT_GREEN, BRIGHT_BLUE,
   __B, __B,   __B,          __B,         GREEN,        GREEN,       __B,
   __B, __B,   __B,          __B,
   __B,

   __B, __B,   __B,          __B,         __B,          __B,         __B,
   __B, __B,   __B,          __B,         __B,          __B,         __B,
        __B,   __B,          __B,         BRIGHT_GREEN, __B,         __B,
   __B, __B,   __B,          __B,         __B,          __B,         __B,
   __B, __B,   __B,          __B,
   __B), // }}}2


  [MOUSE] = COLORMAP_STACKED // {{{2
  (__B, __B, __B,          __B,   __B,          __B,     __B,
   __B, __B, BRIGHT_GREEN, GREEN, BRIGHT_GREEN, MAGENTA, __B,
   __B, __B, GREEN,        GREEN, GREEN,        MAGENTA,
   __B, __B, BRIGHT_GREEN, __B,   BRIGHT_GREEN, MAGENTA, __B,
   __B, __B, __B,          __B,
   __B,

   __B, __B, __B,          __B,   __B,          __B,     __B,
   __B, __B, __B,          __B,   __B,          __B,     __B,
        __B, __B,          __B,   __B,          __B,     __B,
   __B, __B, __B,          __B,   __B,          __B,     __B,
   __B, __B, __B,          __B,
   __B) // }}}2
)
// }}}1

// ---- Kaleidoscope internal {{{1
/**
 * toggleLedsOnSuspendResume toggles the LEDs off when the host goes to sleep,
 * and turns them back on when it wakes up.
 */
void toggleLedsOnSuspendResume(kaleidoscope::plugin::HostPowerManagement::Event event) {
  switch (event) {
  case kaleidoscope::plugin::HostPowerManagement::Suspend:
  case kaleidoscope::plugin::HostPowerManagement::Sleep:
    LEDControl.disable();
    break;
  case kaleidoscope::plugin::HostPowerManagement::Resume:
    LEDControl.enable();
    break;
  }
}

/**
 * hostPowerManagementEventHandler dispatches power management events (suspend,
 * resume, and sleep) to other functions that perform action based on these
 * events.
 */
void hostPowerManagementEventHandler(kaleidoscope::plugin::HostPowerManagement::Event event) {
  toggleLedsOnSuspendResume(event);
}

// }}}1

// ---- Combos {{{1

/**
 * This 'enum' is a list of all the magic combos used by the Model 100's
 * firmware The names aren't particularly important. What is important is that
 * each is unique.
 *
 * These are the names of your magic combos. They will be used by the
 * `USE_MAGIC_COMBOS` call below.
 */
enum {
  // Toggle between Boot (6-key rollover; for BIOSes and early boot) and NKRO
  // mode.
  COMBO_TOGGLE_NKRO_MODE,
  // Enter test mode
  COMBO_ENTER_TEST_MODE
};

/** Wrappers, to be used by MagicCombo. **/

/**
 * This simply toggles the keyboard protocol via USBQuirks, and wraps it within
 * a function with an unused argument, to match what MagicCombo expects.
 */
static void toggleKeyboardProtocol(uint8_t combo_index) {
  USBQuirks.toggleKeyboardProtocol();
}

/**
 * Toggles between using the built-in keymap, and the EEPROM-stored one.
 */
static void toggleKeymapSource(uint8_t combo_index) {
  if (Layer.getKey == Layer.getKeyFromPROGMEM) {
    Layer.getKey = EEPROMKeymap.getKey;
  } else {
    Layer.getKey = Layer.getKeyFromPROGMEM;
  }
}

/**
 * This enters the hardware test mode
 */
static void enterHardwareTestMode(uint8_t combo_index) {
  HardwareTestMode.runTests();
}


/**
 * Magic combo list, a list of key combo and action pairs the firmware should
 * recognise.
 */
USE_MAGIC_COMBOS({.action = toggleKeyboardProtocol,
                  // Left Fn + Esc + Shift
                  .keys = {R3C6, R2C6, R3C7}},
                 {.action = enterHardwareTestMode,
                  // Left Fn + Prog + LED
                  .keys = {R3C6, R0C0, R0C6}},
                 {.action = toggleKeymapSource,
                  // Left Fn + Prog + Shift
                  .keys = {R3C6, R0C0, R3C7}});

// }}}1

// First, tell Kaleidoscope which plugins you want to use.
// The order can be important. For example, LED effects are
// added in the order they're listed here.
KALEIDOSCOPE_INIT_PLUGINS( // {{{1
  // ----------------------------------------------------------------------
  // Chrysalis plugins   {{{2

  // The EEPROMSettings & EEPROMKeymap plugins make it possible to have an
  // editable keymap in EEPROM.
  EEPROMSettings,
  EEPROMKeymap,

  // Focus allows bi-directional communication with the host, and is the
  // interface through which the keymap in EEPROM can be edited.
  Focus,

  // FocusSettingsCommand adds a few Focus commands, intended to aid in
  // changing some settings of the keyboard, such as the default layer (via the
  // `settings.defaultLayer` command)
  FocusSettingsCommand,

  // FocusEEPROMCommand adds a set of Focus commands, which are very helpful in
  // both debugging, and in backing up one's EEPROM contents.
  FocusEEPROMCommand,

  // The FirmwareVersion plugin lets Chrysalis query the version of the firmware
  // programmatically.
  FirmwareVersion,

  // The LayerNames plugin allows Chrysalis to display - and edit - custom layer
  // names, to be shown instead of the default indexes.
  LayerNames,

  // Enables setting, saving (via Chrysalis), and restoring (on boot) the
  // default LED mode.
  DefaultLEDModeConfig,

  // Enables controlling (and saving) the brightness of the LEDs via Focus.
  LEDBrightnessConfig,

  // }}}2
  // ----------------------------------------------------------------------
  // Keystroke-handling plugins  {{{2

  // The Qukeys plugin enables the "Secondary action" functionality in
  // Chrysalis. Keys with secondary actions will have their primary action
  // performed when tapped, but the secondary action when held.
  Qukeys,

  // SpaceCadet can turn your shifts into parens on tap, while keeping them as
  // Shifts when held. SpaceCadetConfig lets Chrysalis configure some aspects of
  // the plugin.
  SpaceCadet,
  SpaceCadetConfig,

  // Enables the "Sticky" behavior for modifiers, and the "Layer shift when
  // held" functionality for layer keys.
  OneShot,
  OneShotConfig,
  EscapeOneShot,
  EscapeOneShotConfig,

  // The macros plugin adds support for macros
  Macros,

  // Enables dynamic, Chrysalis-editable macros.
  DynamicMacros,

  // The MouseKeys plugin lets you add keys to your keymap which move the mouse.
  // The MouseKeysConfig plugin lets Chrysalis configure some aspects of the
  // plugin.
  MouseKeys,
  MouseKeysConfig,

  // The MagicCombo plugin lets you use key combinations to trigger custom
  // actions - a bit like Macros, but triggered by pressing multiple keys at the
  // same time.
  MagicCombo,

  // Allow assigning different chars to shifted and unshifted positions of a key
  CharShift,

  // Enables the GeminiPR Stenography protocol. Unused by default, but with the
  // plugin enabled, it becomes configurable - and then usable - via Chrysalis.
  GeminiPR,

  // Enable the Chord plugin to produce different keys when pressing some
  // keys simultaneously.
  Chord,

  // }}}2
  // ----------------------------------------------------------------------
  // LED mode plugins  {{{2

  // The boot greeting effect pulses the LED button for 10 seconds after the
  // keyboard is first connected
  BootGreetingEffect,

  // LEDControl provides support for other LED modes
  LEDControl,

  // We start with the LED effect that turns off all the LEDs.
  LEDOff,

  // The rainbow effect changes the color of all of the keyboard's keys at the same time
  // running through all the colors of the rainbow.
  LEDRainbowEffect,

  // The rainbow wave effect lights up your keyboard with all the colors of a rainbow
  // and slowly moves the rainbow across your keyboard
  LEDRainbowWaveEffect,

  // The chase effect follows the adventure of a blue pixel which chases a red pixel across
  // your keyboard. Spoiler: the blue pixel never catches the red pixel
  LEDChaseEffect,

  // These static effects turn your keyboard's LEDs a variety of colors
  solidRed,
  solidOrange,
  solidYellow,
  solidGreen,
  solidBlue,
  solidIndigo,
  solidViolet,

  // The breathe effect slowly pulses all of the LEDs on your keyboard
  LEDBreatheEffect,

  // The AlphaSquare effect prints each character you type, using your
  // keyboard's LEDs as a display
  AlphaSquareEffect,

  // The stalker effect lights up the keys you've pressed recently
  StalkerEffect,

  // The LED Palette Theme plugin provides a shared palette for other plugins,
  // like Colormap below
  LEDPaletteTheme,

  // The Colormap effect makes it possible to set up per-layer colormaps
  ColormapEffect,
  DefaultColormap,

  // The numpad plugin is responsible for lighting up the 'numpad' mode
  // with a custom LED effect
  //NumPad,

  // The HostPowerManagement plugin allows us to turn LEDs off when then host
  // goes to sleep, and resume them when it wakes up.
  HostPowerManagement,

  // Turns LEDs off after a configurable amount of idle time.
  IdleLEDs,
  PersistentIdleLEDs,

  // Enable ActiveModColorEffect
  ActiveModColorEffect,

  // }}}2
  // ----------------------------------------------------------------------
  // Miscellaneous plugins  {{{2

  // The USBQuirks plugin lets you do some things with USB that we aren't
  // comfortable - or able - to do automatically, but can be useful
  // nevertheless. Such as toggling the key report protocol between Boot (used
  // by BIOSes) and Report (NKRO).
  USBQuirks,

  // The hardware test mode, which can be invoked by tapping Prog, LED and the
  // left Fn button at the same time.
  HardwareTestMode

  // }}}2
);

/**
 * The 'setup' function is one of the two standard Arduino sketch functions.
 * It's called when your keyboard first powers up. This is where you set up
 * Kaleidoscope and any plugins.
 */
void setup() {
  // First, call Kaleidoscope's internal setup function
  Kaleidoscope.setup();

  // Set the hue of the boot greeting effect to something that will result in a
  // nice green color.
  BootGreetingEffect.hue = 85;

  // While we hope to improve this in the future, the NumPad plugin
  // needs to be explicitly told which keymap layer is your numpad layer
  //NumPad.numPadLayer = NUMPAD;

  // We configure the AlphaSquare effect to use RED letters
  AlphaSquare.color = CRGB(255, 0, 0);

  // Set the rainbow effects to be reasonably bright, but low enough
  // to mitigate audible noise in some environments.
  LEDRainbowEffect.brightness(170);
  LEDRainbowWaveEffect.brightness(160);

  // Set the action key the test mode should listen for to Left Fn
  HardwareTestMode.setActionKey(R3C6);

  // The LED Stalker mode has a few effects. The one we like is called
  // 'BlazingTrail'. For details on other options, see
  // https://github.com/keyboardio/Kaleidoscope/blob/master/docs/plugins/LED-Stalker.md
  StalkerEffect.variant = STALKER(BlazingTrail);

  // To make the keymap editable without flashing new firmware, we store
  // additional layers in EEPROM. For now, we reserve space for eight layers. If
  // one wants to use these layers, just set the default layer to one in EEPROM,
  // by using the `settings.defaultLayer` Focus command, or by using the
  // `keymap.onlyCustom` command to use EEPROM layers only.
  EEPROMKeymap.setup(8);

  // We need to tell the Colormap plugin how many layers we want to have custom
  // maps for. To make things simple, we set it to eight layers, which is how
  // many editable layers we have (see above).
  ColormapEffect.max_layers(8);
  DefaultColormap.setup();

  // For Dynamic Macros, we need to reserve storage space for the editable
  // macros. A kilobyte is a reasonable default.
  DynamicMacros.reserve_storage(1024);

  // If there's a default layer set in EEPROM, we should set that as the default
  // here.
  Layer.move(EEPROMSettings.default_layer());

  // To avoid any surprises, SpaceCadet is turned off by default. However, it
  // can be permanently enabled via Chrysalis, so we should only disable it if
  // no configuration exists.
  SpaceCadetConfig.disableSpaceCadetIfUnconfigured();

  // Editable layer names are stored in EEPROM too, and we reserve 16 bytes per
  // layer for them. We need one extra byte per layer for bookkeeping, so we
  // reserve 17 / layer in total.
  LayerNames.reserve_storage(17 * 8);

  // Unless configured otherwise with Chrysalis, we want to make sure that the
  // firmware starts with LED effects off. This avoids over-taxing devices that
  // don't have a lot of power to share with USB devices
  DefaultLEDModeConfig.activateLEDModeIfUnconfigured(&ColormapEffect);

  // ---- Qukeys config
  QUKEYS(
    kaleidoscope::plugin::Qukey(0, KeyAddr(3,  3), ShiftToLayer(NUMPAD)),    // ,/Layer-shift to Number
    kaleidoscope::plugin::Qukey(0, KeyAddr(3,  4), ShiftToLayer(SYMBOL)),    // ./Layer-shift to Symbol
    kaleidoscope::plugin::Qukey(0, KeyAddr(3, 11), ShiftToLayer(SYMBOL)),    // z/Layer-shift to Symbol
  )
//  Qukeys.setHoldTimeout(1000);
  // Try to avoid primary values, when secondary modifier was meant (for , and z as layer shift to symbol layer)
  // Use smaller numbers, if primary values appear where a modifier was wanted
  Qukeys.setOverlapThreshold(50);
//  Qukeys.setMinimumHoldTime(100);
//  Qukeys.setMinimumPriorInterval(80);
  // Disable tap repeat as it reduces my ability to enter the same
  // character twice.
  Qukeys.setMaxIntervalForTapRepeat(0);

  // ---- CharShift config
  CS_KEYS(
    kaleidoscope::plugin::CharShift::KeyPair(Key_Comma, Key_Semicolon),               // CS(0)   , → ;
    kaleidoscope::plugin::CharShift::KeyPair(Key_Period, LSHIFT(Key_Semicolon)),      // CS(1)   . → :
  );

  // ---- Chords config
  CHORDS(
    CHORD(Key_K, Key_Backslash),    LockLayer  (SYMBOL),  // Toggle symbol layer on
    CHORD(Key_SingleQuote, Key_At), UnlockLayer(SYMBOL),  // Toggle symbol layer off (same physical keys as for toggling on)
    CHORD(Key_N, Key_R, Key_S),     LockLayer  (MOUSE),   // Switch to mouse layer
  )

} // }}}1

/**
 * loop is the second of the standard Arduino sketch functions.
 * As you might expect, it runs in a loop, never exiting.
 *
 * For Kaleidoscope-based keyboard firmware, you usually just want to
 * call Kaleidoscope.loop(); and not do anything custom here.
 */
void loop() {
  Kaleidoscope.loop();
}

// vim: set foldmethod=marker :
