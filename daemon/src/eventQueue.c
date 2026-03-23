#include <stdio.h>
#include "../header/eventQueue.h" 

EventQueue* Queue;

ReturnMsg push(KeyEvent* event)
{
    size_t oldHead = Queue->head;
    size_t next = (oldHead + 1) % MAX_QUEUE_SIZE;

    if (next == Queue->tail) 
        return RETURN_MSG_QUEUE_FULL;

    Queue->buffer[oldHead] = event;
    Queue->head = next;

    return RETURN_MSG_OK;
}

ReturnValue pop()
{
    size_t oldTail = Queue->tail;
    if (Queue->head == oldTail)
    {
        return (ReturnValue){
            .value.event = NULL,
            .msg = RETURN_MSG_QUEUE_EMPTY
        };
    }

    Queue->tail = (oldTail + 1) % MAX_QUEUE_SIZE;

    return (ReturnValue){
        .value.event = Queue->buffer[oldTail],
        .msg = RETURN_MSG_OK
    };
}

KeyEvent* peek()
{
    return Queue->buffer[Queue->head];
}


void setQueue(EventQueue* queue)
{
    Queue = queue;
}