#ifndef EVENTHANDLEROS_H
#define EVENTHANDLEROS_H

#include "event.h"
#include "eventHandler.h"

// implement in os specific event handfler //

int runEventLoop(KeyStatus* keyMapStatus, KeyMapping* keyMapInfo);
int sendEvent(RLEvent* event);
int setKeyStatus(RLEvent* event);
void registerHotKeys();
void printLastError();
void resetModifiers(KeyMapping* keyMapInfo);

#endif // EVENTHANDLEROS_
