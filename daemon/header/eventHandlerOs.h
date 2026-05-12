#ifndef EVENTHANDLEROS_H
#define EVENTHANDLEROS_H

#include "types.h"

int runEventLoop(KeyStatus* keyMapStatus, KeyMapping* keyMapInfo);

RLEvent* RLEventCreate(void* osEvent);
RLHandleResult RLEventVKCSend(RLEvent* event);
RLHandleResult RLEventUniSend(RLEvent* event);

bool isModifier(RLKeyCode keyCode);
RLError setKeyDownStatus(RLEvent* event);
RLError startHoldTimer(RLEvent* event, RLAbsoluteTime delayMillisec);
void RLTimerInvalidate(void* refcon);
void registerHotKeys();
void printLastError();
void resetModifiers(KeyMapping* keyMapInfo);

#endif // EVENTHANDLEROS_
