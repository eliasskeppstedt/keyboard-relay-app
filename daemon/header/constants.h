#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdint.h>

#define PRINT_BOOL(str, bool) printf("%s: %s\n", str, bool ? "true" : "false");

#define VKC_COUNT 256
#define UNICODE_COUNT 0x10FFFF
#define NO_CODE 300
#define UNICODE_MAX_CODE_POINTS 4

#ifdef _WIN32

#define Esc = 0x1B; // byt efter mutli os support

#define LSHIFT          0xa0 // VK_LSHIFT
#define RSHIFT          0xa1 // VK_RSHIFT
#define LCTRL           0xa2 // VK_LCONTROL
#define RCTRL           0xa3 // VK_RCONTROL
#define LALT            0xa4 // VK_LMENU
#define RALT            0xa5 // VK_RMENU

#elif defined __APPLE__
// Defined in Events.h
#define Esc 0x35

#define LSHIFT  0x38
#define RSHIFT  0x3C
#define LCTRL   0x3B
#define RCTRL   0x3E
#define LALT    0x3A
#define RALT    0x3D
#define LMETA   0x37
#define RMETA   0x36
#define CAPS    0x39
#define FN      0x3F

#define Autorepeat 8 // kCGEventKeyboardAutorepeat
#define NullEvent 0 // kcgeventnull

#endif

enum UserData{
    USER_DATA_EVENT_INJECTED = 1,
    USER_DATA_IS_TOGGLE_KEY = 2,
};

enum ModifierKeys{
    MODIFIERKEY_LEFT_SHIFT, 
    MODIFIERKEY_RIGHT_SHIFT,
    MODIFIERKEY_LEFT_CTRL,
    MODIFIERKEY_RIGHT_CTRL,
    MODIFIERKEY_LEFT_ALT,
    MODIFIERKEY_RIGHT_ALT,
    MODIFIERKEY_LEFT_META,
    MODIFIERKEY_RIGHT_META,
    MODIFIERKEY_CAPS,
    MODIFIERKEY_COUNT,
    MODIFIERKEY_NOT_MODIFIER,
};

typedef enum KeyType{
    KEYTYPE_SRC_EVENT = 0,
    KEYTYPE_VIRTUAL_KEYCODE,
    KEYTYPE_UNICODE,
    KEYTYPE_MODIFIER,
    KEYTYPE_COUNT
} KeyType;

#define SYNT_EVENT                 6
#define SYNT_EVENT_FROM_TOGGLE_KEY 5
#define MODIFIER_KEY               4
#define KEY_UP                     3
#define RUN_ORIGINAL_EVENT         2
#define MODIFIER                   1
#define ERR_NIL                         0
#define ERR_BAD_HOOK                   -1
#define ERR_QUIT_BY_USER               -2
#define ERR_SYNT_EVENT_FAILED          -3
#define ERR_QUEUE_FULL                 -4
#define ERR_QUEUE_EMPTY                -5
#define ERR_EVENT_NOT_FOUND            -6
#define ERR_INVALID_UNICODE            -7
#define ERR_FILE                       -8
#define ERR_JSON                       -9
#define ERR_RUN_DAEMON                 -10
#define ERR_COUNT                      10

#define MAX_QUEUE_SIZE 101

#endif // CONSTANTS_H
