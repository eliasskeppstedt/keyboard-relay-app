#include <stdio.h>
#include <stdlib.h>

#include "../../header/eventHandlerOs.h"

CGEventRef eventTapCallback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void* refcon);
void printCGEvent(CGEventRef event);

RLFlags curFlags = 0;

const ModKey ModKeys[VKC_COUNT] = {
    [LSHIFT ] = { .flag = kCGEventFlagMaskShift      },
    [RSHIFT ] = { .flag = kCGEventFlagMaskShift      },
    [LCTRL  ] = { .flag = kCGEventFlagMaskControl    },
    [RCTRL  ] = { .flag = kCGEventFlagMaskControl    },
    [LALT   ] = { .flag = kCGEventFlagMaskAlternate  },
    [RALT   ] = { .flag = kCGEventFlagMaskAlternate  },
    [LMETA  ] = { .flag = kCGEventFlagMaskCommand    },
    [RMETA  ] = { .flag = kCGEventFlagMaskCommand    },
    [CAPS   ] = { .flag = kCGEventFlagMaskAlphaShift },
};

KeyStatus* KeyMapStatus;
KeyMapping* KeyMapInfo;

CGEventRef eventTapCallback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void* refcon) 
{
    int64_t userData = CGEventGetIntegerValueField(event, kCGEventSourceUserData);
    if (userData == USER_DATA_EVENT_INJECTED) 
        goto ReturnEvent;

    RLEvent* rlEvent = RLEventCreate(event);
    
    if (!rlEvent) 
        goto ReturnEvent;

    switch (handleEvent(rlEvent))
    {
    case ERR_NIL:
        return NULL;
    case ERR_EVENT_NOT_FOUND:
        break;
    default:
        break;
    }
    
    ReturnEvent:
    return event;
}

// also sets keyDown for modifier keys
int setKeyStatus(RLEvent* event) 
{
    return ERR_NIL;
}

int sendEvent(RLEvent* rlEvent) 
{
    /*
    CGEventSourceRef src = CGEventSourceCreate(kCGEventSourceStateHIDSystemState);
    CGEventRef event = CGEventCreateKeyboardEvent(src, rlEvent->code[0], rlEvent->keyDown);    
    CGEventSetIntegerValueField(event, kCGEventSourceUserData, USER_DATA_EVENT_INJECTED);
    CGEventSetFlags(event, curFlags);
    CGEventSetTimestamp(event, CFAbsoluteTimeGetCurrent());
    CGEventPost(kCGHIDEventTap, event);
    printCGEvent(event);
    CFRelease(src);
    CFRelease(event);
    */
    return ERR_NIL;
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


int runEventLoop(KeyStatus* keyMapStatus, KeyMapping* keyMapInfo) 
{
    KeyMapStatus = keyMapStatus;
    KeyMapInfo = keyMapInfo;

    CFRunLoopRef runLoop = CFRunLoopGetMain();
    if (!runLoop) 
    {
        printf("Failed to set up run loop. [Write suggestions on why this may happen, else contact support blablabla] ");
        return ERR_RUN_DAEMON;
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
        return ERR_RUN_DAEMON;
    }

    CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(
        kCFAllocatorDefault, 
        eventTap, 
        0
    );
    if (!runLoopSource) 
    {
        printf("Failed to set up run loop source. [Write suggestions on why this may happen, else contact support blablabla] ");
        return ERR_RUN_DAEMON;
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
    return ERR_NIL;
}

void printCGEvent(CGEventRef event)
{
    CGEventType type = CGEventGetType(event);
    const char* typeName = "UNKNOWN_CG_EVENT_TYPE";

    switch (type)
    {
    case kCGEventNull: typeName = "kCGEventNull"; break;
    case kCGEventKeyDown: typeName = "kCGEventKeyDown"; break;
    case kCGEventKeyUp: typeName = "kCGEventKeyUp"; break;
    case kCGEventFlagsChanged: typeName = "kCGEventFlagsChanged"; break;
    case kCGEventScrollWheel: typeName = "kCGEventScrollWheel"; break;
    case kCGEventTabletPointer: typeName = "kCGEventTabletPointer"; break;
    case kCGEventTabletProximity: typeName = "kCGEventTabletProximity"; break;
    case kCGEventTapDisabledByTimeout: typeName = "kCGEventTapDisabledByTimeout"; break;
    case kCGEventTapDisabledByUserInput: typeName = "kCGEventTapDisabledByUserInput"; break;
    default: break;
    }
    printf("New ");
    printf(
        "CGEvent {\n"
        "  type=%u (%s),\n"
        "  timestamp=%llu,\n"
        "  flags=0x%llx,\n"
        "  keycode=%lld,\n"
        "  autorepeat=%lld,\n"
        "  keyboardType=%lld,\n"
        "  sourceUserData=%lld\n"
        "}\n",
        (unsigned int)type,
        typeName,
        (unsigned long long)CGEventGetTimestamp(event),
        (unsigned long long)CGEventGetFlags(event),
        (long long)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode),
        (long long)CGEventGetIntegerValueField(event, kCGKeyboardEventAutorepeat),
        (long long)CGEventGetIntegerValueField(event, kCGKeyboardEventKeyboardType),
        (long long)CGEventGetIntegerValueField(event, kCGEventSourceUserData)
    );
}
