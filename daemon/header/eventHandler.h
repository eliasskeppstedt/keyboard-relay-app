#ifndef EVHANDLER_H
#define EVHANDLER_H

#include "event.h"

// implement in event handler //

int handleEvent(RLEvent* event);
void setMaps(KeyMapping* keyMapInfo, KeyStatus* keyMapStatus);
bool isModifier(RLKeyCode vkCode);

#endif // EVHANDLER_
