#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdint.h>

#define PRINT_BOOL(bool) printf("%s\n", bool ? "true" : "false");
#define INFO_EVENT_INJECTED 1
#define VKC_COUNT 256

#define NO_CODE 300
#define VKC_MAX 256

#ifdef _WIN32

#define LSHIFT          0xa0 // VK_LSHIFT
#define RSHIFT          0xa1 // VK_RSHIFT
#define LCTRL           0xa2 // VK_LCONTROL
#define RCTRL           0xa3 // VK_RCONTROL
#define LALT            0xa4 // VK_LMENU
#define RALT            0xa5 // VK_RMENU

#endif // _WIN32

typedef enum ModifierKeys{
    MODIFIERKEY_LEFT_SHIFT, 
    MODIFIERKEY_RIGHT_SHIFT,
    MODIFIERKEY_LEFT_CTRL,
    MODIFIERKEY_RIGHT_CTRL,
    MODIFIERKEY_LEFT_ALT,
    MODIFIERKEY_RIGHT_ALT,
    MODIFIERKEY_COUNT,
} ModifierKeys;

typedef enum KeyType{
    KEYTYPE_VIRTUAL_KEYCODE_PASSTHROW = 0,
    KEYTYPE_VIRTUAL_KEYCODE,
    KEYTYPE_UNICODE,
    KEYTYPE_MODIFIER,
} KeyType;

typedef enum ReturnMsg{
    RETURN_MSG_OK = 0,
    RETURN_MSG_QUIT_BY_USER,
    RETURN_MSG_SYNT_EVENT,
    RETURN_MSG_SYNT_EVENT_FAILED,
    RETURN_MSG_QUEUE_FULL,
    RETURN_MSG_QUEUE_EMPTY,
    RETURN_MSG_EVENT_NOT_FOUND,
    RETURN_MSG_INVALID_UNICODE,
    RETURN_MSG_MODIFIER_KEY,
    RETURN_MSG_KEY_UP,
    RETURN_MSG_RUN_ORIGINAL_EVENT
} ReturnMsg;

#define MAX_QUEUE_SIZE 101

#endif // CONSTANTS_H