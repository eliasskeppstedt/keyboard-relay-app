#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "../header/eventHandler.h"
#include "../header/eventHandlerOs.h"
#include "../header/types.h"
#include "../header/constants.h"

LRESULT CALLBACK lowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

HHOOK Hook;

KeyStatus* KeyMapStatus;
KeyMapping* KeyMapInfo;

const RLEvent ModifierEvents[MODIFIERKEY_COUNT] = {
    [MODIFIERKEY_LEFT_SHIFT ] = { .code[0] = LSHIFT, .type = KEYTYPE_MODIFIER },
    [MODIFIERKEY_RIGHT_SHIFT] = { .code[0] = RSHIFT, .type = KEYTYPE_MODIFIER },
    [MODIFIERKEY_LEFT_CTRL  ] = { .code[0] = LCTRL , .type = KEYTYPE_MODIFIER },
    [MODIFIERKEY_RIGHT_CTRL ] = { .code[0] = RCTRL , .type = KEYTYPE_MODIFIER, .flags = KEYEVENTF_EXTENDEDKEY },
    [MODIFIERKEY_LEFT_ALT   ] = { .code[0] = LALT  , .type = KEYTYPE_MODIFIER },
    [MODIFIERKEY_RIGHT_ALT  ] = { .code[0] = RALT  , .type = KEYTYPE_MODIFIER, .flags = KEYEVENTF_EXTENDEDKEY },
};

Err runEventLoop(KeyStatus* keyMapStatus, KeyMapping* keyMapInfo) 
{
    KeyMapStatus = keyMapStatus;
    KeyMapInfo = keyMapInfo;
    registerHotKeys();
    Hook = SetWindowsHookEx(WH_KEYBOARD_LL, lowLevelKeyboardProc, NULL, 0);
    if (!Hook)
    {
        printf("Error: Windows hook could not be installed\n");
        return kRLErrorBadHook;
    }
    
    MSG msg;
    BOOL bRet;

    while( (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0 )
    { 
        if (bRet == -1)
        {
            printf("error occured");
            // handle the error and possibly exit
        }
        //else if (msg.message == WM_HOTKEY)
        //{
        //    // hantera hotkey,,, bör vara user defined
        //    switch (msg.wParam)
        //    {
        //    case 1:
        //        break;
        //    }
        //}
        else
        {
            TranslateMessage(&msg); 
            DispatchMessage(&msg); 
        }
    }

    UnhookWindowsHookEx(Hook);
    return kRLErrorNone;
}

LRESULT CALLBACK lowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) 
{
    KBDLLHOOKSTRUCT* kbDLLHookStruct = (KBDLLHOOKSTRUCT*)lParam;
    if (nCode < 0) goto CallNext;
    

    if ((kbDLLHookStruct->flags & LLKHF_INJECTED) ||
        (kbDLLHookStruct->dwExtraInfo == kRLUserDataEventInjected)) 
    {
        goto CallNext;
    }
    
    RLEvent* event = createEvent(kbDLLHookStruct);
    if (!event)
    {
        printf("Error: Could not create event\n");
        goto CallNext;
    }

    Err Err = RLEventHandle(event);

    switch (Err)
    {
    case ERR_QUEUE_FULL:
        printf("Error: Queue rached max size!\n");
        PostQuitMessage(0);
        return 1; 
    case kRLErrorQueueEmpty:
        printf("Error: Tried to pop from empty queue\n");
        PostQuitMessage(0);
        return 1; 
    case kRLErrorEventNotFound:
        perror("Error: Event not found\n");
        PostQuitMessage(0);
        return 1; 
    case kRLErrorQuitByUser:
        printf("Program exited by user escape sequence\n");
        PostQuitMessage(0);
        return 1; 
    case kRLErrorSyntheticEventFailed:
        printf("Program ate a modifier key\n");
        return 1; 
    case ERR_KEY_UP:
    case ERR_MODIFIER_KEY:
    case ERR_SYNT_EVENT:
        return 1; 
    case ERR_RUN_ORIGINAL_EVENT:
        printf("Message: unknown, run original event\n");
        break;
    default:
        printf("Error: unknown, abort event\n");
        // hmm
        return 1;
    }
    CallNext:
    return CallNextHookEx(Hook, nCode, wParam, lParam);
}

RLEvent* createEvent(void* osEvent)
{
    KBDLLHOOKSTRUCT* event = (KBDLLHOOKSTRUCT*)osEvent;
    RLEvent* rlEvent = malloc(sizeof(RLEvent));
    if (!rlEvent)
    {
        printf("Error: malloc failed for rlEvent\n");
        return NULL;
    }
    
    *rlEvent = (RLEvent) {
        .type = KEYTYPE_SRC_EVENT,
        .srcKeyCode = (unsigned short)event->vkCode,
        .code = (unsigned short)event->vkCode,
        .keyDown = !(event->flags & LLKHF_UP),
        .flags = (unsigned long)event->flags,
        .timeStamp = (unsigned long)event->time
    };
    return rlEvent;
}

Err sendVKCodeEvent(RLEvent* event) // check msg codesizes
{   
    WORD* vkCodes = (WORD*)event->code;
    DWORD flags = 0;

    if (event->flags & LLKHF_EXTENDED)
        flags |= KEYEVENTF_EXTENDEDKEY;
    if (!event->keyDown)
        flags |= KEYEVENTF_KEYUP;

    UINT pos = 0;
    INPUT input[1];

    input[pos++] = (INPUT){
        .type = INPUT_KEYBOARD,
        .ki.wVk = vkCodes[0],
        .ki.wScan = MapVirtualKey(vkCodes[0], MAPVK_VK_TO_VSC),
        .ki.dwFlags = flags,
        .ki.time = event->timeStamp,
        .ki.dwExtraInfo = kRLUserDataEventInjected
    };

    UINT inputsSent = SendInput(pos, input, sizeof(INPUT));

    if (inputsSent != pos)
    {
        fprintf(stderr, "Error: synt event failed (msg %lu)\n", GetLastError());
        return kRLErrorSyntheticEventFailed;
    }
    return ERR_SYNT_EVENT;
} 

Err sendUnicodeEvent(RLEvent* event)
{
    DWORD* codePoints = event->code;
    DWORD flags = KEYEVENTF_UNICODE | (!event->keyDown ? KEYEVENTF_KEYUP : 0);

    int size = KeyMapInfo[event->srcKeyCode].onPress.size;
    INPUT* inputs = malloc(sizeof(INPUT) * size * 2);
    UINT pos = 0;

    for (size_t i = 0; i < size; i++)
    {
        inputs[pos] = (INPUT){
            .type = INPUT_KEYBOARD,
            .ki.wVk = 0,
            .ki.dwFlags = flags,
            .ki.dwExtraInfo = kRLUserDataEventInjected
        };

        if (codePoints[i] > 0x10FFFF) // undefined unicode
        {
            printf("Error: codepoint too big");
            free(inputs);
            return kRLErrorInvalidUnicode;
        }
        if (codePoints[i] >= 0xD800 && codePoints[i] <= 0xDFFF) // surrogate code value
        {
            printf("Error: codepoint cant be a surrogate code value");
            free(inputs);
            return kRLErrorInvalidUnicode;
        }
        
        if (codePoints[i] <= 0xFFFF) // BMP
        {
            inputs[pos++].ki.wScan = (WORD)codePoints[i];
        }
        else // surrogate pair
        {
            DWORD value = codePoints[i] - 0x10000;
            WORD high = 0xD800 + (value >> 10);
            WORD low = 0xDC00 + (value & 0x3FF);

            inputs[pos++].ki.wScan = high;

            inputs[pos++] = (INPUT){
                .type = INPUT_KEYBOARD,
                .ki.wVk = 0,
                .ki.wScan = low,
                .ki.dwFlags = flags,
                .ki.dwExtraInfo = kRLUserDataEventInjected
            };
        }
        
    }
    
    UINT inputsSent = SendInput(pos, inputs, sizeof(INPUT));
    free(inputs);

    if (inputsSent != pos)
    {
        printLastError();
        return kRLErrorSyntheticEventFailed;
    }
    return ERR_SYNT_EVENT;
}

void registerHotKeys()
{
    // add hotkeys
}

void resetModifiers(KeyMapping* keyMapInfo)
{
    INPUT input[VKC_COUNT*2] = {0};
    UINT pos = 0;

    for (WORD vkCode = 0; vkCode < VKC_COUNT; vkCode++)
    {
        SHORT keyState;
        while (((keyState = GetKeyState(vkCode)) >> 15)  && !isModifier(vkCode))
        {  
            input[pos++] = (INPUT){
                .type = INPUT_KEYBOARD,
                .ki.wVk = vkCode,
                .ki.wScan = MapVirtualKey(vkCode, MAPVK_VK_TO_VSC),
                .ki.dwFlags = KEYEVENTF_KEYUP,
                .ki.dwExtraInfo = kRLUserDataEventInjected
            };
        }
    }
    for (size_t i = 0; i < MODIFIERKEY_COUNT; i++) // check msg codesize
    {
        WORD vkCode = (WORD)ModifierEvents[i].code[0];
        DWORD flags = KEYEVENTF_KEYUP | ModifierEvents[i].flags;

        if (KeyMapStatus[vkCode].isActive)
        {
            for (size_t j = 0; j < KeyMapStatus[vkCode].count; j++)
            {
                input[pos++] = (INPUT){
                    .type = INPUT_KEYBOARD,
                    .ki.wVk = vkCode,
                    .ki.wScan = MapVirtualKey(vkCode, MAPVK_VK_TO_VSC),
                    .ki.dwFlags = flags,
                    .ki.dwExtraInfo = kRLUserDataEventInjected
                };
            }
        }
    }
    
    if (pos > 0)
        SendInput(pos, input, sizeof(INPUT));
}

void printLastError()
{
    fprintf(stderr, "\nLast error: %lu\n", GetLastError());
}