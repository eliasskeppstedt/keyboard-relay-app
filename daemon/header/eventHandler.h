#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "types.h"

// implement in event handler //

ReturnMsg handleEvent(KeyEvent* event);
void setMaps(KeyMapping* keyMapInfo, KeyStatus* keyMapStatus);
uint32_t getModifier(uint32_t vkCode);

// implement in os specific event handfler //

ReturnMsg runEventLoop();
KeyEvent* createEvent(void* osEvent);
ReturnMsg sendVKCodeEvent(KeyEvent* event);
ReturnMsg sendUnicodeEvent(KeyEvent* event); // bara support för unicodes med 2 code points atm
void registerHotKeys();
void printLastError();
void resetModifiers(KeyStatus* keyMapStatus);
void printKeyEvent(const KeyEvent* e);

#endif // EVENTHANDLER_