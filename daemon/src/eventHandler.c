#include <stdio.h>
#include <stdlib.h>
#include "../header/eventHandler.h"
#include "../header/event.h"
#include "../header/types.h"
#include "../header/constants.h"
#include "../header/eventQueue.h"

int sendEvent(RLEvent* event);
bool EscSeqIsPressed = false;

KeyMapping* KeyMapInfo; 
KeyStatus* KeyMapStatus;

int handleEvent(RLEvent* incomingEvent) 
{
    if (!incomingEvent)
    {
        printf("Error: incoming event is null");
        return ERR_EVENT_NOT_FOUND;
    }

    push(incomingEvent);
    ReturnMsg pop();

    return ERR_NIL;
}

void setMaps(KeyMapping* keyMapInfo, KeyStatus* keyMapStatus)
{
    KeyMapInfo = keyMapInfo;
    KeyMapStatus = keyMapStatus;
}

bool isModifier(RLKeyCode vkCode)
{
    switch (vkCode)
    {
    case LSHIFT   : return true;
    case RSHIFT   : return true;
    case LCTRL    : return true;
    case RCTRL    : return true;
    case LALT     : return true;
    case RALT     : return true;
    case LMETA    : return true;
    case RMETA    : return true;
    //case CAPS : return true;
    default       : return false;
    }
    return true;
}
