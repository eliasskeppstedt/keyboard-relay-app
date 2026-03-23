#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include "constants.h"

typedef struct KeyAction{
    KeyType type;
    unsigned long code;
} KeyAction;

typedef struct KeyMapping{
    KeyAction onPress;
    KeyAction onHold;
} KeyMapping;

typedef struct KeyStatus{
    bool isActive;
    unsigned short activeCode;
    int count;
} KeyStatus;

typedef struct KeyEvent{
    KeyType type;
    // virtual code or unicode
    unsigned short originalVKCode;
    unsigned short vkCode;
    unsigned long uniCode;
    unsigned long timeStamp;
    unsigned long flags;
    bool keyDown;
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

#endif // TYPES_H