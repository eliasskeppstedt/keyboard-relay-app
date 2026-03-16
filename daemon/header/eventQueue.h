#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

#include "../header/types.h"  

ReturnMsg push(KeyEvent* event);
ReturnValue pop();
KeyEvent* peek();

void setQueue(EventQueue* queue);

#endif // EVENTQUEUE_H