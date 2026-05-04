#include <stdio.h>
#include "../header/eventQueue.h" 

RLEventQueue* Queue;

int push(RLEvent* event)
{
    size_t oldHead = Queue->head;
    size_t next = (oldHead + 1) % MAX_QUEUE_SIZE;

    if (next == Queue->tail) 
        return ERR_QUEUE_FULL;

    Queue->buffer[oldHead] = event;
    Queue->head = next;

    return ERR_NIL;
}

ReturnMsg pop()
{
    size_t oldTail = Queue->tail;
    if (Queue->head == oldTail)
    {
        return (ReturnMsg){
            .msg = NULL,
            .err = ERR_QUEUE_EMPTY
        };
    }

    Queue->tail = (oldTail + 1) % MAX_QUEUE_SIZE;

    return (ReturnMsg){
        .msg = Queue->buffer[oldTail],
        .err = ERR_NIL
    };
}

RLEvent* peek()
{
    return Queue->buffer[Queue->head];
}


void setQueue(RLEventQueue* queue)
{
    Queue = queue;
}