// src/eventQueue.c

#include <stdio.h>
#include "../header/eventQueue.h" 

RLEventQueue* Queue;

RLError push(RLEvent* event)
{
    size_t oldHead = Queue->head;
    size_t next = (oldHead + 1) % MAX_QUEUE_SIZE;

    if (next == Queue->tail) 
        return kRLErrorQueueFull;

    Queue->buffer[oldHead] = event;
    Queue->head = next;

    return kRLErrorNone;
}

RLEvent* pop()
{
    size_t oldTail = Queue->tail;
    if (Queue->head == oldTail)
        return NULL;

    Queue->tail = (oldTail + 1) % MAX_QUEUE_SIZE;

    RLEvent* event = Queue->buffer[oldTail];
    Queue->buffer[oldTail] = NULL;

    return event;
}

RLEvent* peek()
{
    if (Queue->head == Queue->tail)
        return NULL;
    return Queue->buffer[Queue->tail];
}


void setQueue(RLEventQueue* queue)
{
    Queue = queue;
}
