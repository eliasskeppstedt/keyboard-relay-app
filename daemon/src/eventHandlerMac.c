
#include <stdio.h>
#include <stdlib.h>
#include <CoreGraphics/CGEvent.h>
#include <CoreGraphics/CGEventTypes.h>
#include <CoreGraphics/CGEventSource.h>
#include <CoreFoundation/CFRunLoop.h>
#include <CoreFoundation/CoreFoundation.h>

#include "../header/eventHandler.h"

CGEventRef eventTapCallback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void* refcon);

static unsigned long K_CG_EVENT_TAP_OPTION_DEFAULT = 0x00000000; // for Mac OS X v10.4 support
static unsigned long EVENT_MASK = (
    CGEventMaskBit(kCGEventKeyDown) |
    CGEventMaskBit(kCGEventKeyUp) |
    CGEventMaskBit(kCGEventFlagsChanged)
);
RLFlags curFlags = 0;

const KeyEvent ModifierEvents[MODIFIERKEY_COUNT] = {
    [MODIFIERKEY_LEFT_SHIFT  ] = { .srcKeyCode = LSHIFT, .type = KEYTYPE_MODIFIER, .flags = kCGEventFlagMaskShift },
    [MODIFIERKEY_RIGHT_SHIFT ] = { .srcKeyCode = RSHIFT, .type = KEYTYPE_MODIFIER, .flags = kCGEventFlagMaskShift},
    [MODIFIERKEY_LEFT_CTRL   ] = { .srcKeyCode = LCTRL , .type = KEYTYPE_MODIFIER, .flags = kCGEventFlagMaskControl},
    [MODIFIERKEY_RIGHT_CTRL  ] = { .srcKeyCode = RCTRL , .type = KEYTYPE_MODIFIER, .flags = kCGEventFlagMaskControl },
    [MODIFIERKEY_LEFT_ALT    ] = { .srcKeyCode = LALT  , .type = KEYTYPE_MODIFIER, .flags = kCGEventFlagMaskAlternate },
    [MODIFIERKEY_RIGHT_ALT   ] = { .srcKeyCode = RALT  , .type = KEYTYPE_MODIFIER, .flags = kCGEventFlagMaskAlternate },
    [MODIFIERKEY_LEFT_META   ] = { .srcKeyCode = LMETA , .type = KEYTYPE_MODIFIER, .flags = kCGEventFlagMaskCommand },
    [MODIFIERKEY_RIGHT_META  ] = { .srcKeyCode = RMETA , .type = KEYTYPE_MODIFIER, .flags = kCGEventFlagMaskCommand },
};

KeyStatus* KeyMapStatus;
KeyMapping* KeyMapInfo;

ReturnMsg runEventLoop(KeyStatus* keyMapStatus, KeyMapping* keyMapInfo) 
{
    KeyMapStatus = keyMapStatus;
    KeyMapInfo = keyMapInfo;
    
    printf("Setting upp run loop... ");
    CFRunLoopRef runLoop = CFRunLoopGetMain();
    if (!runLoop) 
    {
        printf("Failed to set up run loop. [Write suggestions on why this may happen, else contact support blablabla] ");
        return RETURN_MSG_ERROR;
    }
    printf("ok\n");

    printf("Setting upp event tap... ");
    CFMachPortRef eventTap = CGEventTapCreate(
        kCGHIDEventTap,                // tap; window server, login session, specific annotation
        kCGHeadInsertEventTap,         // places; head or tail
        K_CG_EVENT_TAP_OPTION_DEFAULT, // options; default or listen only
        EVENT_MASK,                    // eventsOfInterest; mouse, keyboard, etc
        eventTapCallback,            
        NULL                           // userInfo
    );

    if (!eventTap) 
    {
        printf("Failed to set up event tap. [Write suggestions on why this may happen, else contact support blablabla] ");
        return RETURN_MSG_ERROR;
    }
    printf("ok\n");

    printf("Setting upp run loop source... ");
    CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(
        kCFAllocatorDefault, 
        eventTap, 
        0
    );
    if (!runLoopSource) 
    {
        printf("Failed to set up run loop source. [Write suggestions on why this may happen, else contact support blablabla] ");
        return RETURN_MSG_ERROR;
    }
    printf("ok\n");

    printf("Adding source to run loop... \n");
    CFRunLoopAddSource(
        runLoop, 
        runLoopSource, 
        kCFRunLoopCommonModes
    );

    printf("Enabeling event tap... \n");
    CGEventTapEnable(
        eventTap, 
        true
    );

    printf("Starting run loop...\n\n");
    CFRunLoopRun();

    CFMachPortInvalidate(eventTap);
    CFRelease(runLoopSource);
    CFRelease(eventTap);

    printf("\nEvent loop closed\n");
    return 0;
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


CGEventRef eventTapCallback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void* refcon) 
{
    int64_t evSrcUserData = CGEventGetIntegerValueField(event, kCGEventSourceUserData);
    // CFRunLoopStop(CFRunLoopGetCurrent());
    if (evSrcUserData & USER_DATA_EVENT_INJECTED)
    {
        goto CallNext;
    }

    KeyEvent* keyEvent = createEvent(event);
    if (!keyEvent)
    {
        printf("Coult not create event\n");
        return event;
    }
    if (CGEventGetIntegerValueField(event, kCGKeyboardEventAutorepeat))
    {
       keyEvent->extraInfo.osEventType = RLAutorepeat;
    }

    ReturnMsg returnMsg = handleEvent(keyEvent);

    switch (returnMsg)
    {
    case RETURN_MSG_QUEUE_FULL:
        printf("Error: Queue rached max size!\n");
        exit(RETURN_MSG_QUEUE_FULL);
        return NULL; 
    case RETURN_MSG_QUEUE_EMPTY:
        printf("Error: Tried to pop from empty queue\n");
        exit(RETURN_MSG_QUEUE_EMPTY);
        return NULL; 
    case RETURN_MSG_EVENT_NOT_FOUND:
        perror("Error: Event not found\n");
        exit(RETURN_MSG_EVENT_NOT_FOUND);
        return NULL; 
    case RETURN_MSG_QUIT_BY_USER:
        printf("Program exited by user escape sequence\n");
        exit(RETURN_MSG_QUIT_BY_USER);
        return NULL; 
    case RETURN_MSG_SYNT_EVENT_FAILED:
        printf("Program ate a modifier key\n");
        return NULL; 
    case RETURN_MSG_KEY_UP:
        return NULL; 
    case RETURN_MSG_MODIFIER_KEY:
        return NULL; 
    case RETURN_MSG_SYNT_EVENT:
        return NULL;
    case RETURN_MSG_OK:
        // uh inget händer här vid msg ok;
    default: // ex run original event 
        printf("Error: unknown, return event (%d)\n", returnMsg);
        // hmm
        CallNext:
        
        return event;
    }
}

KeyEvent* createEvent(void* osEvent) {
    CGEventRef event = (CGEventRef) osEvent;
    if (!event)
    {
        printf("Could not convert void pointer to CGEventRef\n");
        return NULL;
    }

    KeyEvent* keyEvent = malloc(sizeof(KeyEvent));
    if (!keyEvent)
    {
        printf("keyEvent malloc failed\n");
        return NULL;
    }

    RLKeyCode srcKeyCode = CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);

    *keyEvent = (KeyEvent){
        .type = KEYTYPE_SRC_EVENT,
        // virtual code or unicode
        .srcKeyCode = srcKeyCode,
        .code[0] = srcKeyCode,
        .timeStamp = CGEventGetTimestamp(event),
        .flags = CGEventGetFlags(event), 
        .keyDown = CGEventGetType(event) == kCGEventKeyDown, // does not work for modifiers! corrected separately
        .extraInfo = (ExtraKeyEventInfo){
            .osEventType = CGEventGetType(event)
        }
    };

    return keyEvent;
}

// also sets keyDown for modifier keys
ReturnMsg setStatusIsActive(KeyEvent* event) 
{
    if (isModifier(event->srcKeyCode))
    {
        RLFlags modFlags = 0;
        for (size_t i = 0; i < MODIFIERKEY_COUNT; i++)
        {
            if (event->code[0] == ModifierEvents[i].srcKeyCode)
            {
                modFlags = ModifierEvents[i].flags;
                break;
            }
        }
        if (event->flags & modFlags)
        {
            event->keyDown = true;
        }
        else
        {
            event->keyDown = false;
        }
    }

    KeyMapStatus[event->srcKeyCode].isActive = event->keyDown;

    if (event->srcKeyCode == CAPSLOCK /*caps*/)
    {
    }
    return RETURN_MSG_OK;
}

ReturnMsg sendVKCodeEvent(KeyEvent* keyEvent) 
{
    if (isModifier(keyEvent->code[0]))
    {  
        for (size_t i = 0; i < MODIFIERKEY_COUNT; i++)
        {
            if (ModifierEvents[i].srcKeyCode == keyEvent->code[0])
            {
                if (keyEvent->keyDown)
                {    
                    curFlags |= keyEvent->flags;
                    curFlags |= ModifierEvents[i].flags;
                }
                else
                {
                    curFlags &= ~ModifierEvents[i].flags;
                }
                break;
            }
        }
    }

    CGEventSourceRef src = CGEventSourceCreate(kCGEventSourceStateHIDSystemState);
    CGEventRef event = CGEventCreateKeyboardEvent(src, keyEvent->code[0], keyEvent->keyDown);    
        PRINT_BOOL(keyEvent->keyDown);
    if (keyEvent->extraInfo.osEventType == RLAutorepeat) 
    {
        CGEventSetIntegerValueField(event, kCGKeyboardEventAutorepeat, 1);
    }
    CGEventSetIntegerValueField(event, kCGEventSourceUserData, USER_DATA_EVENT_INJECTED);
    CGEventSetFlags(event, curFlags);
    CGEventSetTimestamp(event, CFAbsoluteTimeGetCurrent());
    CGEventPost(kCGHIDEventTap, event);

    CFRelease(src);
    CFRelease(event);
    return RETURN_MSG_SYNT_EVENT;
}

ReturnMsg sendUnicodeEvent(KeyEvent* event) 
{
    return RETURN_MSG_SYNT_EVENT;
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
