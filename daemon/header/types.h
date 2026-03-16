#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include "constants.h"

typedef struct KeyAction{
    KeyType type;
    uint32_t code;
} KeyAction;

typedef struct KeyMapping{
    KeyAction onPress;
    KeyAction onHold;
} KeyMapping;

typedef struct KeyStatus{
    bool isActive;
    uint32_t activeCode;
} KeyStatus;

typedef struct KeyEvent{
    KeyType type;
    // virtual code or unicode
    uint32_t originalVKCode;
    uint32_t vkCode;
    uint32_t uniCode;
    uint32_t timeStamp;
    uint32_t flags;
    bool keyDown;
} KeyEvent;

typedef struct ReturnValue{    
    union value{
        uint32_t u32;
        KeyEvent* event;
        void* p;
    } value;
    ReturnMsg msg;
} ReturnValue;

typedef struct EventQueue{
    KeyEvent* buffer[MAX_QUEUE_SIZE];
    uint8_t head;
    uint8_t tail;
} EventQueue;

#endif // TYPES_H