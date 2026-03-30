#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "types.h"

// implement in event handler //

ReturnMsg handleEvent(KeyEvent* event);
void setMaps(KeyMapping* keyMapInfo, KeyStatus* keyMapStatus);
bool isModifier(unsigned short vkCode);

// implement in os specific event handfler //

ReturnMsg runEventLoop(KeyStatus* keyMapStatus, KeyMapping* keyMapInfo);
KeyEvent* createEvent(void* osEvent);
ReturnMsg sendVKCodeEvent(KeyEvent* event);
ReturnMsg sendUnicodeEvent(KeyEvent* event);
void registerHotKeys();
void printLastError();
void resetModifiers(KeyMapping* keyMapInfo);

#endif // EVENTHANDLER_