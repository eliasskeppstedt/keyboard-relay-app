#include <stdio.h>
#include <stdlib.h>
#include "../header/eventHandler.h"
#include "../header/types.h"
#include "../header/constants.h"
#include "../header/eventQueue.h"

ReturnMsg sendEvent(KeyEvent* event);
bool EscSeqIsPressed = false;

KeyMapping* KeyMapInfo; 
KeyStatus* KeyMapStatus;

ReturnMsg handleEvent(KeyEvent* incomingEvent) 
{
    if (!incomingEvent)
    {
        printf("Error: incoming event is null");
        return RETURN_MSG_EVENT_NOT_FOUND;
    }
    
    if (incomingEvent->srcKeyCode == Esc) 
    {   
        resetModifiers(KeyMapInfo);
        free(incomingEvent);
        return RETURN_MSG_QUIT_BY_USER;
    }

    push(incomingEvent);
    if (incomingEvent->srcKeyCode == CAPSLOCK) // same for every toggle key
    {
        /*KeyEvent* toggleEvent = malloc(sizeof(KeyEvent));
        *toggleEvent = *incomingEvent;
        push(toggleEvent);*/
    }

    KeyEvent* outgoingEvent;
    ReturnMsg returnMsg;

    // hold functionality not implemented
    while(true) {
        ReturnValue returnValue = pop();
        if (!(returnValue.msg == RETURN_MSG_OK))
        {
            break;
        }        

        outgoingEvent = returnValue.value.event;

        RLKeyCode srcKeyCode = outgoingEvent->srcKeyCode;

        srcKeyCode = outgoingEvent->srcKeyCode;

        size_t actionCodeSize = 0; // for unnicode, which has multiple code points
        
        outgoingEvent->type = KeyMapInfo[srcKeyCode].onPress.type; 
        if (outgoingEvent->type != KEYTYPE_SRC_EVENT)
        {
            actionCodeSize = KeyMapInfo[srcKeyCode].onPress.size; // change when support for hold
            for (size_t i = 0; i < actionCodeSize; i++)
            {
                outgoingEvent->code[i] = KeyMapInfo[srcKeyCode].onPress.code[i];
            }
            if (outgoingEvent->type != KEYTYPE_UNICODE)
            {
                if (isModifier(outgoingEvent->code[0]))
                {
                    outgoingEvent->type = KEYTYPE_MODIFIER;
                }
            }
        }

        setKeyStatus(outgoingEvent);
        Autorepeat:

        if (outgoingEvent->type == KEYTYPE_UNICODE)
        {
            returnMsg = sendUnicodeEvent(outgoingEvent);
        }
        else
        {
            returnMsg = sendVKCodeEvent(outgoingEvent);
        }
        free(outgoingEvent);
    }

    return returnMsg;
}

ReturnMsg setKeyStatus(KeyEvent* event) 
{
    setStatusIsActive(event);
    
    if (event->keyDown)
    {
        for (size_t i = 0; i < UNICODE_MAX_CODE_POINTS; i++)
        {
            KeyMapStatus[event->srcKeyCode].activeCode[i] = event->code[i];
        }
    }
    else
    {
        KeyMapStatus[event->srcKeyCode].activeCode[0] = NO_CODE;
    }

    return RETURN_MSG_OK;
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
    case CAPSLOCK : return true;
    default       : return false;
    }
    return true;
}
