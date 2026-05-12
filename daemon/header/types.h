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
typedef UniChar          RLUnicode;
typedef UniCharCount     RLUnicodeLen;
typedef CGEventType      OSEventType;
typedef CFAbsoluteTime   RLAbsoluteTime;

#elif defined _WIN32

#endif

typedef union RLSendCode{
    RLKeyCode keyCode;
    struct {
        RLUnicode chars[UNICODE_MAX_CODE_POINTS];
        RLUnicodeLen length;
    } unicode;
} RLSendCode;

typedef struct RLEvent{
    RLKeyCode srcKeyCode;
    RLOutputType outputType;
    OSEventType osEventType;
    RLActionType action;
    //RLInputEventKind inputEventKind;
    RLSendCode sendCode;
    bool keyDown;
    RLFlags flags;
    RLTimestamp timestamp;
    void* timer;
} RLEvent;

typedef struct RLEventQueue {
    RLEvent* buffer[MAX_QUEUE_SIZE];
    size_t head;
    size_t tail;
} RLEventQueue;

typedef struct SpecialKey{
    RLFlags flag;
} SpecialKey;

typedef struct ExtraRLEventInfo{
    CGEventType osEventType;
} ExtraRLEventInfo;

typedef struct KeyAction{
    RLOutputType outputType;
    RLSendCode sendCode;
} KeyAction;

typedef struct KeyMapping{
    KeyAction onPress;
    KeyAction onHold;
} KeyMapping;

typedef struct KeyStatus{
    bool keyDown;
    RLSendCode activeSendCode;
    RLOutputType activeOutputType;
    RLActionType activeActionType;
} KeyStatus;

typedef struct ModifierState{
    bool isActive;
    int count;
} ModifierState;

typedef struct ReturnMsg{    
    void* value;
    int msg;
} ReturnMsg;

typedef struct Settings{
    int tmp;
} Settings;

typedef struct TimerContext{
    RLEvent* event;
} TimerContext;

#endif // TYPES_H
