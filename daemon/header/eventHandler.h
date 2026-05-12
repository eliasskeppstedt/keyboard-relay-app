#ifndef EVHANDLER_H
#define EVHANDLER_H

#include "types.h"

// implement in event handler //

RLHandleResult RLEventHandle(void* osEvent);
RLHandleResult RLEventHandleTimer(RLEvent* event);
void setMaps(KeyMapping* keyMapInfo, KeyStatus* keyMapStatus);

#endif // EVHANDLER_
