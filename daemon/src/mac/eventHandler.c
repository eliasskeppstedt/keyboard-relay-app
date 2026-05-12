// src/mac/eventHandler.c

#include <stdio.h>
#include <stdlib.h>

#include "../../header/eventHandler.h"
#include "../../header/eventHandlerOs.h"
#include "../../header/mac.h"

CGEventRef eventTapCallback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void* refcon);
//void printCGEvent(CGEventRef event);
void timerCallback(CFRunLoopTimerRef timer, void* refcon);

RLFlags CurFlags = 0;
const SpecialKey SpecialKeys[VKC_COUNT] = {
    [kVK_Shift               ] = { .flag = kCGEventFlagMaskShift       },
    [kVK_RightShift          ] = { .flag = kCGEventFlagMaskShift       },
    [kVK_Control             ] = { .flag = kCGEventFlagMaskControl     },
    [kVK_RightControl        ] = { .flag = kCGEventFlagMaskControl     },
    [kVK_Option              ] = { .flag = kCGEventFlagMaskAlternate   },
    [kVK_RightOption         ] = { .flag = kCGEventFlagMaskAlternate   },
    [kVK_Command             ] = { .flag = kCGEventFlagMaskCommand     },
    [kVK_RightCommand        ] = { .flag = kCGEventFlagMaskCommand     },
    [kVK_CapsLock            ] = { .flag = kCGEventFlagMaskAlphaShift  },
    [kVK_Function            ] = { .flag = kCGEventFlagMaskSecondaryFn },
    /*
    [kVK_ANSI_Keypad0        ] = { .flag = kCGEventFlagMaskNumericPad  },
    [kVK_ANSI_Keypad1        ] = { .flag = kCGEventFlagMaskNumericPad  },
    [kVK_ANSI_Keypad2        ] = { .flag = kCGEventFlagMaskNumericPad  },
    [kVK_ANSI_Keypad3        ] = { .flag = kCGEventFlagMaskNumericPad  },
    [kVK_ANSI_Keypad4        ] = { .flag = kCGEventFlagMaskNumericPad  },
    [kVK_ANSI_Keypad5        ] = { .flag = kCGEventFlagMaskNumericPad  },
    [kVK_ANSI_Keypad6        ] = { .flag = kCGEventFlagMaskNumericPad  },
    [kVK_ANSI_Keypad7        ] = { .flag = kCGEventFlagMaskNumericPad  },
    [kVK_ANSI_Keypad8        ] = { .flag = kCGEventFlagMaskNumericPad  },
    [kVK_ANSI_Keypad9        ] = { .flag = kCGEventFlagMaskNumericPad  },
    [kVK_ANSI_KeypadDecimal  ] = { .flag = kCGEventFlagMaskNumericPad  },
    [kVK_ANSI_KeypadPlus     ] = { .flag = kCGEventFlagMaskNumericPad  },
    [kVK_ANSI_KeypadMinus    ] = { .flag = kCGEventFlagMaskNumericPad  },
    [kVK_ANSI_KeypadMultiply ] = { .flag = kCGEventFlagMaskNumericPad  },
    [kVK_ANSI_KeypadDivide   ] = { .flag = kCGEventFlagMaskNumericPad  },
    [kVK_ANSI_KeypadEquals   ] = { .flag = kCGEventFlagMaskNumericPad  },
    [kVK_ANSI_KeypadEnter    ] = { .flag = kCGEventFlagMaskNumericPad  },
    [kVK_ANSI_KeypadClear    ] = { .flag = kCGEventFlagMaskNumericPad  },
    */
};

KeyStatus* KeyMapStatus;
KeyMapping* KeyMapInfo;

CGEventRef eventTapCallback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void* refcon) 
{
    int64_t userData = CGEventGetIntegerValueField(event, kCGEventSourceUserData);
    if (userData == kRLUserDataEventInjected) 
        goto ReturnEvent;

    switch (RLEventHandle(event))
    {
    case kRLHandleResultError:
        CFRunLoopStop(CFRunLoopGetMain());
        break;
    case kRLHandleResultHandled:
        return NULL;
    case kRLHandleResultPendingHold:
        return NULL;
    case kRLHandleResultPassThrough:
        break;
    default: 
        break;
    }
    
    ReturnEvent:
    return event;
}

RLEvent* RLEventCreate(void* osEvent) {
    CGEventRef event = (CGEventRef) osEvent;
    if (!event)
    {
        printf("Could not convert void pointer to CGEventRef\n");
        return NULL;
    }

    RLEvent* rlEvent = malloc(sizeof(RLEvent));
    if (!rlEvent)
    {
        printf("rlEvent malloc failed\n");
        return NULL;
    }

    *rlEvent = (RLEvent){
        .srcKeyCode = CGEventGetIntegerValueField(event,kCGKeyboardEventKeycode),
        .sendCode.keyCode = CGEventGetIntegerValueField(event,kCGKeyboardEventKeycode),
        .osEventType = CGEventGetType(event),
        //.inputEventKind = kRLInputEventKindKeyboard,
        .outputType = kRLOutputTypeKeyCode,
        .action = kRLActionTypePress,
        .flags = CGEventGetFlags(event),
        .timestamp  = CGEventGetTimestamp(event),
    };

    return rlEvent;
}

RLError setKeyDownStatus(RLEvent* event) 
{
    if (event->osEventType == kCGEventFlagsChanged) // modifiers
    {
        KeyMapStatus[event->srcKeyCode].keyDown = event->flags & SpecialKeys[event->srcKeyCode].flag;
    }
    else
    {
        KeyMapStatus[event->srcKeyCode].keyDown = event->osEventType == kCGEventKeyDown;
    }

    event->keyDown = KeyMapStatus[event->srcKeyCode].keyDown;

    return kRLErrorNone;
}

RLHandleResult RLEventVKCSend(RLEvent* rlEvent) 
{
    if (isModifier(rlEvent->sendCode.keyCode))
    {
        if (KeyMapStatus[rlEvent->srcKeyCode].keyDown)
            CurFlags |= SpecialKeys[rlEvent->sendCode.keyCode].flag;
        else
            CurFlags &= ~SpecialKeys[rlEvent->sendCode.keyCode].flag;
    }
    
    CGEventSourceRef src = CGEventSourceCreate(kCGEventSourceStateHIDSystemState);
    if (!src)
    {
        printf("create event src for new keycode cgevent failed, pass through src event\n");
        return kRLHandleResultPassThrough;
    }
    
    CGEventRef event = CGEventCreateKeyboardEvent(
        src, 
        rlEvent->sendCode.keyCode, 
        rlEvent->keyDown
    );
    if (!event)
    {
        CFRelease(src);
        printf("create keyboard event for new keycode cgevent failed, pass through src event\n");
        return kRLHandleResultPassThrough;
    }

    CGEventSetIntegerValueField(event, kCGEventSourceUserData, kRLUserDataEventInjected);
    CGEventSetFlags(event, CurFlags);
    CGEventSetTimestamp(event, CFAbsoluteTimeGetCurrent());

    CGEventPost(kCGHIDEventTap, event);

    CFRelease(src);
    CFRelease(event);
    return kRLHandleResultHandled;
}

RLHandleResult RLEventUniSend(RLEvent* rlEvent) 
{
    CGEventSourceRef src = CGEventSourceCreate(kCGEventSourceStateHIDSystemState);
    if (!src)
    {
        printf("create event src for new unicode cgevent failed, pass through src event\n");
        return kRLHandleResultPassThrough;
    }

    CGEventRef event = CGEventCreateKeyboardEvent(
        src, 
        0,
        rlEvent->keyDown
    );
    if (!event)
    {
        CFRelease(src);
        printf("create keyboard event for new keycode cgevent failed, pass through src event\n");
        return kRLHandleResultPassThrough;
    }

    CGEventKeyboardSetUnicodeString(
        event, 
        rlEvent->sendCode.unicode.length, 
        rlEvent->sendCode.unicode.chars
    );

    CGEventSetIntegerValueField(event, kCGEventSourceUserData, kRLUserDataEventInjected);
    CGEventSetFlags(event, CurFlags);
    CGEventSetTimestamp(event, CFAbsoluteTimeGetCurrent());

    CGEventPost(kCGHIDEventTap, event);

    CFRelease(src);
    CFRelease(event);
    return kRLHandleResultHandled;
}


bool isModifier(RLKeyCode keyCode)
{
    return keyCode < VKC_COUNT && SpecialKeys[keyCode].flag != 0;
}

void registerHotKeys() 
{
    // uhm...
}

void printLastError() 
{
    // TODO
}

void resetModifiers(KeyMapping* keyMapInfo) 
{
    // TODO
}

void timerCallback(CFRunLoopTimerRef timer, void* refcon)
{
    TimerContext* ctx = (TimerContext*)refcon;
    if (!ctx || !ctx->event) {
        CFRunLoopStop(CFRunLoopGetMain());
        return;
    }
    RLEventHandleTimer(ctx->event);
    free(ctx);
}

RLError startHoldTimer(RLEvent* event, RLAbsoluteTime delayMillisec)
{
    TimerContext* ctx = malloc(sizeof(TimerContext));
    if (!ctx)
        return kRLErrorMallocFailed;

    ctx->event = event;

    CFRunLoopTimerContext timerCtx = {
        .version = 0,
        .info = ctx,
        .retain = NULL,
        .release = NULL,
        .copyDescription = NULL
    };

    CFAbsoluteTime delaySeconds = delayMillisec / 1000.0;

    CFRunLoopTimerRef timer = CFRunLoopTimerCreate(
        kCFAllocatorDefault,
        CFAbsoluteTimeGetCurrent() + delaySeconds,
        0, 0, 0,
        timerCallback,
        &timerCtx
    );

    if (!timer) {
        free(ctx);
        return kRLErrorTimerFailed;
    }

    event->timer = (CFRunLoopTimerRef)CFRetain(timer);
    CFRunLoopAddTimer(CFRunLoopGetMain(), timer, kCFRunLoopCommonModes);
    CFRelease(timer);

    return kRLErrorNone;
}

void RLTimerInvalidate(void* refcon)
{
    CFRunLoopTimerRef timer = (CFRunLoopTimerRef)refcon;
    if (!timer)
    {
        printf("no timer on event\n");
        return;
    }
    CFRunLoopTimerInvalidate(timer);
}

int runEventLoop(KeyStatus* keyMapStatus, KeyMapping* keyMapInfo) 
{
    KeyMapStatus = keyMapStatus;
    KeyMapInfo = keyMapInfo;

    CFRunLoopRef runLoop = CFRunLoopGetMain();
    if (!runLoop) 
    {
        printf("Failed to set up run loop. [Write suggestions on why this may happen, else contact support blablabla] ");
        return kRLErrorRunDaemon;
    } 
    
    CFMachPortRef eventTap = CGEventTapCreate(
    kCGHIDEventTap,                // tap; window server, login session, specific annotation
        kCGHeadInsertEventTap,     // places; head or tail
        _kCGEventTapOptionDefault, // options; default or listen only
        EVENT_MASK,                // eventsOfInterest; mouse, keyboard, etc
        eventTapCallback,            
        NULL                       // userInfo
    );
    if (!eventTap) 
    {
        printf("Failed to set up event tap. [Write suggestions on why this may happen, else contact support blablabla] ");
        return kRLErrorRunDaemon;
    }

    CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(
        kCFAllocatorDefault, 
        eventTap, 
        0
    );
    if (!runLoopSource) 
    {
        printf("Failed to set up run loop source. [Write suggestions on why this may happen, else contact support blablabla] ");
        return kRLErrorRunDaemon;
    }

    CFRunLoopAddSource(
        runLoop, 
        runLoopSource, 
        kCFRunLoopCommonModes
    );

    CGEventTapEnable(
        eventTap, 
        true
    );

    printf("Starting run loop\n\n");
    CFRunLoopRun();

    CFMachPortInvalidate(eventTap);
    CFRelease(runLoopSource);
    CFRelease(eventTap);

    printf("\nEvent loop closed\n");
    return kRLErrorNone;
}