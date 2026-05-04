#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

#include "types.h" 
#include "event.h"
#include "constants.h"

typedef struct RLEventQueue {
    RLEvent* buffer[MAX_QUEUE_SIZE];
    size_t head;
    size_t tail;
} RLEventQueue;

int push(RLEvent* event);
ReturnMsg pop();
RLEvent* peek();

void setQueue(RLEventQueue* queue);

#endif // EVENTQUEUE_H
