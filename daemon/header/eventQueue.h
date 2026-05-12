#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

#include "types.h" 

RLError push(RLEvent* event);
RLEvent* pop();
RLEvent* peek();

void setQueue(RLEventQueue* queue);

#endif // EVENTQUEUE_H
