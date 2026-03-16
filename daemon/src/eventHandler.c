#include <stdio.h>
#include <stdlib.h>
#include "../header/eventHandler.h"
#include "../header/types.h"
#include "../header/constants.h"
#include "../header/eventQueue.h"

ReturnMsg sendEvent(KeyEvent* event);
bool EscSeqIsPressed = false;

const uint32_t EscWin = 0x1B; // byt efter mutli os support
KeyMapping* KeyMapInfo; 
KeyStatus* KeyMapStatus;

ReturnMsg handleEvent(KeyEvent* incomingEvent) 
{
    if (incomingEvent->vkCode == EscWin) 
    {   
        resetModifiers(KeyMapStatus);
        free(incomingEvent);
        return RETURN_MSG_QUIT_BY_USER;
    }

    KeyEvent* outgoingEvent;

    incomingEvent->type = KeyMapInfo[incomingEvent->vkCode].onPress.type;
    if (incomingEvent->type == KEYTYPE_UNICODE)
    { 
        incomingEvent->uniCode = KeyMapInfo[incomingEvent->vkCode].onPress.code;
    }
    else 
    {
        if (incomingEvent->type == KEYTYPE_VIRTUAL_KEYCODE)
        {
            incomingEvent->vkCode = KeyMapInfo[incomingEvent->vkCode].onPress.code;
        }

        if (getModifier(incomingEvent->vkCode))
            incomingEvent->type = KEYTYPE_MODIFIER;

            
        KeyMapStatus[incomingEvent->vkCode] = (KeyStatus){
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

uint32_t getModifier(uint32_t vkCode)
{
    switch (vkCode)
    {
    case LSHIFT: return LSHIFT;
    case RSHIFT: return RSHIFT;
    case LCTRL : return LCTRL;
    case RCTRL : return RCTRL;
    case LALT  : return LALT;
    case RALT  : return RALT;
    default    : return 0;
    }
}