#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdint.h>

#define PRINT_BOOL(bool) printf("%s\n", bool ? "true" : "false");

#define VKC_COUNT 256
#define UNICODE_COUNT 0x10FFFF
#define NO_CODE 300
#define UNICODE_MAX_CODE_POINTS 22

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
#define CAPSLOCK 0x39

#define RLAutorepeat 8 // kCGEventKeyboardAutorepeat

#define RLNullEvent 0 // kcgeventnull

#endif

typedef enum UserData{
    USER_DATA_EVENT_INJECTED = 1,
    USER_DATA_IS_TOGGLE_KEY = 2,
} UserData;

typedef enum ModifierKeys{
    MODIFIERKEY_LEFT_SHIFT, 
    MODIFIERKEY_RIGHT_SHIFT,
    MODIFIERKEY_LEFT_CTRL,
    MODIFIERKEY_RIGHT_CTRL,
    MODIFIERKEY_LEFT_ALT,
    MODIFIERKEY_RIGHT_ALT,
    MODIFIERKEY_LEFT_META,
    MODIFIERKEY_RIGHT_META,
    MODIFIERKEY_CAPSLOCK,
    MODIFIERKEY_COUNT,
    MODIFIERKEY_NOT_MODIFIER,
} ModifierKeys;

typedef enum KeyType{
    KEYTYPE_SRC_EVENT = 0,
    KEYTYPE_VIRTUAL_KEYCODE,
    KEYTYPE_UNICODE,
    KEYTYPE_MODIFIER,
    KEYTYPE_COUNT
} KeyType;

typedef enum ReturnMsg{
    RETURN_MSG_OK = 0,
    RETURN_MSG_BAD_HOOK,
    RETURN_MSG_QUIT_BY_USER,
    RETURN_MSG_SYNT_EVENT,
    RETURN_MSG_SYNT_EVENT_FROM_TOGGLE_KEY,
    RETURN_MSG_SYNT_EVENT_FAILED,
    RETURN_MSG_QUEUE_FULL,
    RETURN_MSG_QUEUE_EMPTY,
    RETURN_MSG_EVENT_NOT_FOUND,
    RETURN_MSG_INVALID_UNICODE,
    RETURN_MSG_MODIFIER_KEY,
    RETURN_MSG_KEY_UP,
    RETURN_MSG_RUN_ORIGINAL_EVENT,
    RETURN_MSG_FILE_ERROR,
    RETURN_MSG_JSON_ERROR,
    RETURN_MSG_MODIFIER_ERROR,
    RETURN_MSG_ERROR,
    RETURN_MSG_COUNT
} ReturnMsg;

#define MAX_QUEUE_SIZE 101

#endif // CONSTANTS_H
