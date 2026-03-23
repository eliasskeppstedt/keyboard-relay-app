#include <stdio.h>
#include <stdlib.h>
#include "../header/eventHandler.h"
#include "../header/types.h"
#include "../header/constants.h"
#include "../header/eventQueue.h"

ReturnMsg sendEvent(KeyEvent* event);
bool EscSeqIsPressed = false;

const unsigned short EscWin = 0x1B; // byt efter mutli os support
KeyMapping* KeyMapInfo; 
KeyStatus* KeyMapStatus;

ReturnMsg handleEvent(KeyEvent* incomingEvent) 
{
    if (!incomingEvent)
    {
        printf("Error: incoming event is null");
        return RETURN_MSG_EVENT_NOT_FOUND;
    }
    
    if (incomingEvent->vkCode == EscWin) 
    {   
        resetModifiers(KeyMapInfo);
        free(incomingEvent);
        return RETURN_MSG_QUIT_BY_USER;
    }

    KeyEvent* outgoingEvent;
    unsigned short originalVKCode = incomingEvent->vkCode;

    incomingEvent->type = KeyMapInfo[incomingEvent->vkCode].onPress.type;
    if (incomingEvent->type == KEYTYPE_UNICODE)
    { 
        incomingEvent->uniCode = KeyMapInfo[incomingEvent->vkCode].onPress.code;
    }
    else 
    {
        if (incomingEvent->type == KEYTYPE_VIRTUAL_KEYCODE)
        {
            incomingEvent->vkCode = (unsigned short)KeyMapInfo[incomingEvent->vkCode].onPress.code;
        }

        if (isModifier(incomingEvent->vkCode))
            incomingEvent->type = KEYTYPE_MODIFIER;

        KeyMapStatus[originalVKCode] = (KeyStatus){
            .activeCode = incomingEvent->vkCode,
            .isActive = incomingEvent->keyDown
        };
    }

    // hold functionality not implemented

    outgoingEvent = incomingEvent;
    return sendEvent(outgoingEvent);
}

void setMaps(KeyMapping* keyMapInfo, KeyStatus* keyMapStatus)
{
    KeyMapInfo = keyMapInfo;
    KeyMapStatus = keyMapStatus;
}

ReturnMsg sendEvent(KeyEvent* event)
{
    ReturnMsg returnMsg;
    if (event->type == KEYTYPE_UNICODE)
    {
        //hantera flagga här... tror jag
        returnMsg = sendUnicodeEvent(event);
    }
    else
    {
        returnMsg = sendVKCodeEvent(event);
    }
    
    free(event);
    return returnMsg;
}

bool isModifier(unsigned short vkCode)
{
    switch (vkCode)
    {
    case LSHIFT: break;
    case RSHIFT: break;
    case LCTRL : break;
    case RCTRL : break;
    case LALT  : break;
    case RALT  : break;
    default    : return false;
    }
    return true;
}