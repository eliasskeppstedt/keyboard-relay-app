#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <stdint.h>

#include "constants.h"

#ifdef __APPLE__
#include "mac.h"

typedef uint32_t         RLKeyCode;
typedef CGEventFlags     RLFlags;
typedef CGEventTimestamp RLTimestamp;
typedef CGEventType      RLEventType;
typedef UniChar          RLUnicode;
typedef UniCharCount     RLUnicodeLen;

#elif defined _WIN32

#endif

typedef struct ModKey{
    RLFlags flag;
} ModKey;

typedef enum RLKeyType{
    RL_KEY_SRC,
    RL_KEY_VIRTUAL,
    RL_KEY_UNICODE,
} RLKeyType;

typedef struct ExtraRLEventInfo{
    CGEventType osEventType;
} ExtraRLEventInfo;

typedef struct KeyAction{
    KeyType type;
    RLKeyCode code[UNICODE_MAX_CODE_POINTS];
    int size;
} KeyAction;

typedef struct KeyMapping{
    KeyAction onPress;
    KeyAction onHold;
} KeyMapping;

typedef struct KeyStatus{
    bool keyDown;
    RLKeyType keyType;
    union {
        RLKeyCode keyCode;
        struct {
            RLUnicode chars[UNICODE_MAX_CODE_POINTS];
            RLUnicodeLen length;
        } unicode;
    } activeCode;
} KeyStatus;

typedef struct ModifierState{
    bool isActive;
    int count;
} ModifierState;

typedef struct ReturnMsg{    
    void* msg;
    int err;
} ReturnMsg;

typedef struct Settings{
    int tmp;
} Settings;

#endif // TYPES_H
