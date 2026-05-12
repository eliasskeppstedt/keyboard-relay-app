// src/eventHandler.c

#include <stdio.h>
#include <stdlib.h>

#include "../header/eventHandler.h"
#include "../header/eventHandlerOs.h"
#include "../header/eventQueue.h"

RLHandleResult RLEventSend(RLEvent* event);
static RLHandleResult RLEventDrainQueue(void);

KeyMapping* KeyMapInfo; 
KeyStatus* KeyMapStatus;

RLHandleResult RLEventHandle(void* osEvent) 
{
    RLEvent* incomingEvent = RLEventCreate(osEvent);

    if (!incomingEvent)
    {
        return kRLHandleResultPassThrough;
    }
    
    KeyMapping keyInfo = KeyMapInfo[incomingEvent->srcKeyCode];
    KeyStatus keyStatus = KeyMapStatus[incomingEvent->srcKeyCode];
    
    if (!keyStatus.keyDown) // if detta är key up
    {
        if (keyInfo.onHold.outputType != kRLOutputTypeNone)
        {
            incomingEvent->action = kRLActionTypePendingHold;
            keyStatus.activeActionType = kRLActionTypePendingHold;
            RLError err = startHoldTimer(incomingEvent, 150); // TODO make user defined
            if (err != kRLErrorNone)
            {
                RLTimerInvalidate(incomingEvent->timer);
                return kRLHandleResultPassThrough;
            }
        }
        else if (keyStatus.activeActionType == kRLActionTypeHold)
        {
            incomingEvent->action = kRLActionTypeHold;
            incomingEvent->outputType = keyInfo.onHold.outputType;
        }
        else if (keyInfo.onPress.outputType != kRLOutputTypeNone)
        {
            incomingEvent->outputType = keyInfo.onPress.outputType;
            incomingEvent->action = kRLActionTypePress;
        }
    }
    else
    {
        incomingEvent->action = keyStatus.activeActionType;
        incomingEvent->outputType = keyStatus.activeOutputType;
    }

    RLError err = setKeyDownStatus(incomingEvent);
    if (err != kRLErrorNone)
    {
        printf("Could not set key (down) status, pass through src event\n");
        free(incomingEvent);
        return kRLHandleResultPassThrough;
    }

    err = push(incomingEvent);
    if (err == kRLErrorQueueFull)
    {
        printf("Error: %d", err);
        RLTimerInvalidate(incomingEvent->timer);
        free(incomingEvent);
        return kRLHandleResultError;
    }

    return RLEventDrainQueue();
}

RLHandleResult RLEventHandleTimer(RLEvent* outgoingEvent)
{
    if (!outgoingEvent)
        return kRLHandleResultPassThrough;

    KeyMapping keyInfo = KeyMapInfo[outgoingEvent->srcKeyCode];

    if (KeyMapStatus[outgoingEvent->srcKeyCode].keyDown)
    {
        outgoingEvent->action = kRLActionTypeHold;
        outgoingEvent->outputType = keyInfo.onHold.outputType;
    }
    else if (keyInfo.onPress.outputType != kRLOutputTypeNone)
    {
        outgoingEvent->action = kRLActionTypePress;
        outgoingEvent->outputType = keyInfo.onPress.outputType;
    }

    KeyMapStatus[outgoingEvent->srcKeyCode].activeActionType = kRLActionTypeHold;
    outgoingEvent->sendCode = KeyMapStatus[outgoingEvent->srcKeyCode].activeSendCode;


    RLTimerInvalidate(outgoingEvent->timer);

    return RLEventDrainQueue();
}

static RLHandleResult RLEventDrainQueue(void)
{
    RLEvent* outgoingEvent;

    RLHandleResult msg = kRLHandleResultHandled;
    while ((outgoingEvent = peek()))
    {
        RLKeyCode srcKeyCode = outgoingEvent->srcKeyCode;
        if (outgoingEvent->action == kRLActionTypePendingHold) 
        {
            if (KeyMapStatus[srcKeyCode].keyDown)
            {
                msg = kRLHandleResultPendingHold;
                break;
            }
            outgoingEvent->outputType = KeyMapInfo[outgoingEvent->srcKeyCode].onPress.outputType; // is SRC for no remap
            outgoingEvent->action = kRLActionTypePress;
            RLTimerInvalidate(outgoingEvent->timer);
        }

        outgoingEvent = pop();

        KeyAction keyInfoAction;
        if (outgoingEvent->action == kRLActionTypeHold)
            keyInfoAction = KeyMapInfo[srcKeyCode].onHold;
        else
            keyInfoAction = KeyMapInfo[srcKeyCode].onPress;
            
        if (keyInfoAction.outputType ==  kRLOutputTypeUnicode)
        {
            outgoingEvent->sendCode.unicode.length = keyInfoAction.sendCode.unicode.length;
            for (size_t i = 0; i < UNICODE_MAX_CODE_POINTS; i++)
            {
                outgoingEvent->sendCode.unicode.chars[i] = keyInfoAction.sendCode.unicode.chars[i];
            }
        }
        else if (keyInfoAction.outputType ==  kRLOutputTypeKeyCode)
        {
            outgoingEvent->sendCode.keyCode = keyInfoAction.sendCode.keyCode;
        } 
        else // kRLOutputTypeNone i keyInfoAction
        {
            outgoingEvent->sendCode.keyCode = outgoingEvent->srcKeyCode;
        }

        if (outgoingEvent->keyDown)
        {
            KeyMapStatus[srcKeyCode].activeSendCode = outgoingEvent->sendCode;
        }

        msg = RLEventSend(outgoingEvent);
        free(outgoingEvent);
    }

    return msg;
}

RLHandleResult RLEventSend(RLEvent* event)
{
    RLHandleResult msg;
    RLOutputType outputType = event->outputType;
    
    if (outputType == kRLOutputTypeUnicode)
        msg = RLEventUniSend(event);
    else// if (outputType == RL_CODE_TYPE_VIRTUAL)
        msg = RLEventVKCSend(event);

    return msg;
}

void setMaps(KeyMapping* keyMapInfo, KeyStatus* keyMapStatus)
{
    KeyMapInfo = keyMapInfo;
    KeyMapStatus = keyMapStatus;
}
