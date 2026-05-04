#include <stdio.h>
#include <stdlib.h>

#include "../../header/event.h"
#include "../../header/types.h"

RLEvent* RLEventCreate(void* osEvent) {
    CGEventRef event = (CGEventRef) osEvent;
    if (!event)
    {
        printf("Could not convert void pointer to CGEventRef\n");
        return NULL;
    }

    RLEvent* rlEvent = malloc(sizeof(RLEvent));

    if (!rlEvent)
    {
        printf("rlEvent malloc failed\n");
        return NULL;
    }

    *rlEvent = (RLEvent){
        .srcKeyCode = CGEventGetIntegerValueField(event,kCGKeyboardEventKeycode),
        .sendType   = RL_KEY_SRC,
        .flags      = CGEventGetFlags(event),
    };

    return rlEvent;
}