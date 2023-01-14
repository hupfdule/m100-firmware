/*
 * Key definitions matching the german QWERTZ T1 keyboard layout.
 */

/* Base layer */
#define Key_DE_A                Key_A
#define Key_DE_B                Key_B
#define Key_DE_C                Key_C
#define Key_DE_D                Key_D
#define Key_DE_E                Key_E
#define Key_DE_F                Key_F
#define Key_DE_G                Key_G
#define Key_DE_H                Key_H
#define Key_DE_I                Key_I
#define Key_DE_J                Key_J
#define Key_DE_K                Key_K
#define Key_DE_L                Key_L
#define Key_DE_M                Key_M
#define Key_DE_N                Key_N
#define Key_DE_O                Key_O
#define Key_DE_P                Key_P
#define Key_DE_Q                Key_Q
#define Key_DE_R                Key_R
#define Key_DE_S                Key_S
#define Key_DE_T                Key_T
#define Key_DE_U                Key_U
#define Key_DE_V                Key_V
#define Key_DE_W                Key_W
#define Key_DE_X                Key_X
#define Key_DE_Y                Key_Z
#define Key_DE_Z                Key_Y
#define Key_DE_1                Key_1
#define Key_DE_2                Key_2
#define Key_DE_3                Key_3
#define Key_DE_4                Key_4
#define Key_DE_5                Key_5
#define Key_DE_6                Key_6
#define Key_DE_7                Key_7
#define Key_DE_8                Key_8
#define Key_DE_9                Key_9
#define Key_DE_0                Key_0
#define Key_DE_UUmlaut          Key(HID_KEYBOARD_LEFT_BRACKET_AND_LEFT_CURLY_BRACE, KEY_FLAGS)
#define Key_DE_AUmlaut          Key(HID_KEYBOARD_QUOTE_AND_DOUBLEQUOTE, KEY_FLAGS)
#define Key_DE_OUmlaut          Key(HID_KEYBOARD_SEMICOLON_AND_COLON, KEY_FLAGS)
#define Key_DE_Eszett           Key(HID_KEYBOARD_MINUS_AND_UNDERSCORE, KEY_FLAGS)
#define Key_DE_Circumflex       Key(HID_KEYBOARD_GRAVE_ACCENT_AND_TILDE, KEY_FLAGS)
#define Key_DE_Acute            Key(HID_KEYBOARD_EQUALS_AND_PLUS, KEY_FLAGS)
#define Key_DE_Plus             Key(HID_KEYBOARD_RIGHT_BRACKET_AND_RIGHT_CURLY_BRACE, KEY_FLAGS)
#define Key_DE_Pound            Key(HID_KEYBOARD_BACKSLASH_AND_PIPE, KEY_FLAGS)
#define Key_DE_LeftAngleBracket Key(HID_KEYBOARD_NON_US_BACKSLASH_AND_PIPE, KEY_FLAGS)
#define Key_DE_Comma            Key(HID_KEYBOARD_COMMA_AND_LESS_THAN, KEY_FLAGS)
#define Key_DE_Period           Key(HID_KEYBOARD_PERIOD_AND_GREATER_THAN, KEY_FLAGS)
#define Key_DE_Minus            Key(HID_KEYBOARD_SLASH_AND_QUESTION_MARK, KEY_FLAGS)

/* Shifted layer */
#define Key_DE_Degrees           LSHIFT(KEY_DE_Circumflex)
#define Key_DE_ExclamationMark   LSHIFT(Key_DE_1)
#define Key_DE_DoubleQuote       LSHIFT(Key_DE_2)
#define Key_DE_ParagraphMark     LSHIFT(Key_DE_3)
#define Key_DE_Dollar            LSHIFT(Key_DE_4)
#define Key_DE_Percent           LSHIFT(Key_DE_5)
#define Key_DE_Ampersand         LSHIFT(Key_DE_6)
#define Key_DE_Slash             LSHIFT(Key_DE_7)
#define Key_DE_LeftParentheses   LSHIFT(Key_DE_8)
#define Key_DE_RightParentheses  LSHIFT(Key_DE_9)
#define Key_DE_Equals            LSHIFT(Key_DE_0)
#define Key_DE_QuestionMark      LSHIFT(Key_DE_Eszett)
#define Key_DE_Grave             LSHIFT(Key_DE_Acute)
#define Key_DE_Asterisk          LSHIFT(Key_DE_Plus)
#define Key_DE_SingleQuote       LSHIFT(Key_DE_Pound)
#define Key_DE_RightAngleBracket LSHIFT(Key_DE_LeftAngleBracket)
#define Key_DE_Semicolon         LSHIFT(Key_DE_Comma)
#define Key_DE_Colon             LSHIFT(Key_DE_Period)
#define Key_DE_Underscore        LSHIFT(Key_DE_Minus)

/* AltGr layer */
#define Key_DE_Superscript2      RALT(Key_DE_2)
#define Key_DE_Superscript3      RALT(Key_DE_3)
#define Key_DE_LeftCurlyBracket  RALT(Key_DE_7)
#define Key_DE_LeftBracket       RALT(Key_DE_8)
#define Key_DE_RightBracket      RALT(Key_DE_9)
#define Key_DE_RightCurlyBracket RALT(Key_DE_0)
#define Key_DE_Backslash         RALT(Key_DE_Eszett)
#define Key_DE_At                RALT(Key_DE_Q)
#define Key_DE_Euro              RALT(Key_DE_E)
#define Key_DE_Tilde             RALT(Key_DE_Plus)
#define Key_DE_Pipe              RALT(Key_DE_LeftAngleBracket)
#define Key_DE_Micro             RALT(Key_DE_M)

/* Aliases */
#define Key_DE_Caret             Key_DE_Circumflex
#define Key_DE_Backtick          Key_DE_Acute
#define Key_DE_LessThan          Key_DE_LeftAngleBracket
#define Key_DE_GreaterThan       Key_DE_RightAngleBracket
#define Key_DE_Quote             Key_DE_DoubleQuote
#define Key_DE_Hash              Key_DE_Pound
