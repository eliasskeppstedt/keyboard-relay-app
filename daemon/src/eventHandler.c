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
    
    if (incomingEvent->originalVKCode == EscWin) 
    {   
        resetModifiers(KeyMapInfo);
        free(incomingEvent);
        return RETURN_MSG_QUIT_BY_USER;
    }

    KeyEvent* outgoingEvent;
    unsigned short originalVKCode = incomingEvent->originalVKCode;
    
    incomingEvent->type = KeyMapInfo[originalVKCode].onPress.type; 
    if (incomingEvent->type != KEYTYPE_VIRTUAL_KEYCODE_PASSTHROUGH)
    {
        for (size_t i = 0; i < KeyMapInfo[originalVKCode].onPress.size; i++)
        {
            incomingEvent->code[i] = KeyMapInfo[originalVKCode].onPress.code[i];
        }

        if (incomingEvent->type != KEYTYPE_UNICODE)
        {
            if (isModifier(incomingEvent->code[0]))
                incomingEvent->type = KEYTYPE_MODIFIER;
        }
    }
    
    // hold functionality not implemented

    outgoingEvent = incomingEvent;
    originalVKCode = outgoingEvent->originalVKCode;

    KeyStatus keyStatus = KeyMapStatus[outgoingEvent->originalVKCode];

    ReturnMsg returnMsg;
    if (outgoingEvent->type == KEYTYPE_UNICODE)
    {
        //hantera flagga här... tror jag
        returnMsg = sendUnicodeEvent(outgoingEvent);
    }
    else
    {   
        returnMsg = sendVKCodeEvent(outgoingEvent);
    }
    
    if (!outgoingEvent->keyDown)
    {
        if (keyStatus.count > 0)
        {
            keyStatus.count--;
        }
        if (keyStatus.count < 1)
        {
            keyStatus.isActive = false;
            for (size_t i = 0; i < KeyMapInfo[originalVKCode].onPress.size; i++)
            {
                keyStatus.activeCode[i] = NO_CODE;
            }
        }
    }
    else
    {
        for (size_t i = 0; i < KeyMapInfo[originalVKCode].onPress.size; i++)
        {
            keyStatus.activeCode[i] = outgoingEvent->code[i];
        }
        keyStatus.isActive = true;
        keyStatus.count++;
    }
    
    KeyMapStatus[outgoingEvent->originalVKCode] = keyStatus;
    free(outgoingEvent);

    return returnMsg;
}

void setMaps(KeyMapping* keyMapInfo, KeyStatus* keyMapStatus)
{
    KeyMapInfo = keyMapInfo;
    KeyMapStatus = keyMapStatus;
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