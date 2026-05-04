#ifndef EVENT_H
#define EVENT_H

#include "constants.h"
#include "types.h"

typedef struct RLEvent{
    RLKeyCode srcKeyCode;
    RLKeyType sendType;
    union {
        RLKeyCode keyCode;
        struct {
            RLUnicode chars[UNICODE_MAX_CODE_POINTS];
            RLUnicodeLen length;
        } unicode;
    } sendCode;
    RLFlags flags;
    RLTimestamp timestamp;
} RLEvent;

RLEvent* RLEventCreate(void* osEvent);

#endif // EVENT_H
