#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include "constants.h"

#ifdef __APPLE__

#include <CoreGraphics/CGEventTypes.h>
#include <CoreGraphics/CGEventTypes.h>

typedef int64_t RLKeyCode;
typedef CGEventFlags RLFlags;
typedef CGEventTimestamp RLTimestamp;
typedef CGEventType RLEventType;

#elif defined _WIN32

#endif

typedef struct ExtraKeyEventInfo{
    CGEventType osEventType;
} ExtraKeyEventInfo;

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
    bool isActive;
    RLKeyCode activeCode[UNICODE_MAX_CODE_POINTS];
} KeyStatus;

typedef struct KeyEvent{
    KeyType type;
    // virtual code or unicode
    RLKeyCode srcKeyCode;
    RLKeyCode code[UNICODE_MAX_CODE_POINTS];
    RLTimestamp timeStamp;
    RLFlags flags;
    bool keyDown;
    ExtraKeyEventInfo extraInfo;
} KeyEvent;

typedef struct ModifierState{
    bool isActive;
    int count;
} ModifierState;

typedef struct ReturnValue{    
    union value{
        unsigned long numeric;
        KeyEvent* event;
        void* p;
    } value;
    ReturnMsg msg;
} ReturnValue;

typedef struct EventQueue{
    KeyEvent* buffer[MAX_QUEUE_SIZE];
    size_t head;
    size_t tail;
} EventQueue;

typedef struct Settings{
    int tmp;
} Settings;

#endif // TYPES_H
