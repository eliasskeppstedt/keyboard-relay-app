#include <windows.h>
#include <stdio.h>
#include "../header/eventHandler.h"
#include "../header/types.h"
#include "../header/constants.h"

LRESULT CALLBACK lowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

HHOOK Hook;

KeyStatus* KeyMapStatus;
const KeyEvent ModifierEvents[MODIFIERKEY_COUNT] = {
    [MODIFIERKEY_LEFT_SHIFT ] = { .vkCode = LSHIFT, .type = KEYTYPE_MODIFIER },
    [MODIFIERKEY_RIGHT_SHIFT] = { .vkCode = RSHIFT, .type = KEYTYPE_MODIFIER },
    [MODIFIERKEY_LEFT_CTRL  ] = { .vkCode = LCTRL , .type = KEYTYPE_MODIFIER },
    [MODIFIERKEY_RIGHT_CTRL ] = { .vkCode = RCTRL , .type = KEYTYPE_MODIFIER },
    [MODIFIERKEY_LEFT_ALT   ] = { .vkCode = LALT  , .type = KEYTYPE_MODIFIER },
    [MODIFIERKEY_RIGHT_ALT  ] = { .vkCode = RALT  , .type = KEYTYPE_MODIFIER },
};

ReturnMsg runEventLoop(KeyStatus* keyMapStatus) 
{
    KeyMapStatus = keyMapStatus;
    registerHotKeys();
    Hook = SetWindowsHookEx(WH_KEYBOARD_LL, lowLevelKeyboardProc, NULL, 0);
    if (!Hook)
    {
        printf("Error: Windows hook could not be installed\n");
        return RETURN_MSG_BAD_HOOK;
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
        /*else if (msg.message == WM_HOTKEY)
        {
            // hantera hotkey,,, bör vara user defined
            switch (msg.wParam)
            {
            case 1:
                break;
            }
        }*/
        else
        {
            TranslateMessage(&msg); 
            DispatchMessage(&msg); 
        }
    }

    UnhookWindowsHookEx(Hook);
    return RETURN_MSG_OK;
}

LRESULT CALLBACK lowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) 
{
    KBDLLHOOKSTRUCT* kbDLLHookStruct = (KBDLLHOOKSTRUCT*)lParam;
    if (nCode < 0) goto CallNext;
    

    if ((kbDLLHookStruct->flags & LLKHF_INJECTED) ||
        (kbDLLHookStruct->dwExtraInfo == INFO_EVENT_INJECTED)) 
    {
        goto CallNext;
    }
    
    KeyEvent* event = createEvent(kbDLLHookStruct);
    if (!event)
    {
        printf("Error: Could not create event\n");
        goto CallNext;
    }

    ReturnMsg returnMsg = handleEvent(event);

    switch (returnMsg)
    {
    case RETURN_MSG_QUEUE_FULL:
        printf("Error: Queue rached max size!\n");
        PostQuitMessage(0);
        return 1; 
    case RETURN_MSG_QUEUE_EMPTY:
        printf("Error: Tried to pop from empty queue\n");
        PostQuitMessage(0);
        return 1; 
    case RETURN_MSG_EVENT_NOT_FOUND:
        perror("Error: Event not found\n");
        PostQuitMessage(0);
        return 1; 
    case RETURN_MSG_QUIT_BY_USER:
        printf("Program exited by user escape sequence\n");
        PostQuitMessage(0);
        return 1; 
    case RETURN_MSG_SYNT_EVENT_FAILED:
        printf("Program ate a modifier key\n");
        return 1; 
    case RETURN_MSG_KEY_UP:
    case RETURN_MSG_MODIFIER_KEY:
    case RETURN_MSG_SYNT_EVENT:
        return 1; 
    case RETURN_MSG_RUN_ORIGINAL_EVENT:
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

KeyEvent* createEvent(void* osEvent)
{
    KBDLLHOOKSTRUCT* event = (KBDLLHOOKSTRUCT*)osEvent;
    KeyEvent* keyEvent = malloc(sizeof(KeyEvent));
    if (!keyEvent)
    {
        printf("Error: malloc failed for keyEvent\n");
        return NULL;
    }
    
    *keyEvent = (KeyEvent) {
        .type = KEYTYPE_VIRTUAL_KEYCODE_PASSTHROW,
        .originalVKCode = (unsigned short)event->vkCode,
        .uniCode = U'\0',
        .vkCode = (unsigned short)event->vkCode,
        .keyDown = !(event->flags & LLKHF_UP),
        .flags = (unsigned long)event->flags,
        .timeStamp = (unsigned long)event->time
    };
    return keyEvent;
}

ReturnMsg sendVKCodeEvent(KeyEvent* event)
{   
    WORD vkCode = event->vkCode;
    DWORD flags = 0;

    if (event->flags & LLKHF_EXTENDED)
        flags |= KEYEVENTF_EXTENDEDKEY;

    UINT pos = 0;
    INPUT input[1];

    if (!event->keyDown)
    {
        flags |= KEYEVENTF_KEYUP;
        KeyMapStatus[event->originalVKCode].count--;
        if (KeyMapStatus[event->originalVKCode].count < 1)
        {
            KeyMapStatus[event->originalVKCode].isActive = false;
            KeyMapStatus[event->originalVKCode].count = 0;
        }
    }
    else
    {
        KeyMapStatus[event->originalVKCode].isActive = true;
        KeyMapStatus[event->originalVKCode].count++;
    }

    input[pos++] = (INPUT){
        .type = INPUT_KEYBOARD,
        .ki.wVk = vkCode,
        .ki.wScan = MapVirtualKey(vkCode, MAPVK_VK_TO_VSC),
        .ki.dwFlags = flags,
        .ki.time = event->timeStamp,
        .ki.dwExtraInfo = INFO_EVENT_INJECTED
    };

    if (SendInput(pos, input, sizeof(INPUT)) != pos)
    {
        fprintf(stderr, "Error: synt event failed (err %lu)\n", GetLastError());
        return RETURN_MSG_SYNT_EVENT_FAILED;
    }
    return RETURN_MSG_SYNT_EVENT;
} 

ReturnMsg sendUnicodeEvent(KeyEvent* event)
{   
    if (!event->keyDown)
        return RETURN_MSG_KEY_UP;
        
    INPUT inputs[2] = {0};
    UINT pos = 0;

    DWORD flags = KEYEVENTF_UNICODE |
                 (!event->keyDown ? KEYEVENTF_KEYUP : 0);

    DWORD character = event->uniCode;

    if (character >= 0xD800 && character <= 0xDFFF)
        return RETURN_MSG_INVALID_UNICODE;

    inputs[pos] = (INPUT){
        .type = INPUT_KEYBOARD,
        .ki.wVk = 0,
        .ki.dwFlags = flags,
        .ki.dwExtraInfo = INFO_EVENT_INJECTED
    };

    if (character <= 0xFFFF)
    {
        inputs[pos++].ki.wScan = (WORD)character;
    }
    else
    {
        character -= 0x10000;

        WORD high = 0xD800 + (character >> 10);
        WORD low  = 0xDC00 + (character & 0x3FF);

        inputs[pos++].ki.wScan = high;

        inputs[pos++] = (INPUT){
            .type = INPUT_KEYBOARD,
            .ki.wVk = 0,
            .ki.wScan = low,
            .ki.dwFlags = flags,
            .ki.dwExtraInfo = INFO_EVENT_INJECTED
        };
    }

    if (SendInput(pos, inputs, sizeof(INPUT)) != pos)
    {
        printLastError();
        return RETURN_MSG_SYNT_EVENT_FAILED;
    }
    return RETURN_MSG_SYNT_EVENT;
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
        if (KeyMapStatus[vkCode].isActive && !isModifier(vkCode))
        {
            for (size_t i = 0; i < KeyMapStatus[vkCode].count; i++)
            {
                input[pos++] = (INPUT){
                    .type = INPUT_KEYBOARD,
                    .ki.wVk = vkCode,
                    .ki.wScan = MapVirtualKey(vkCode, MAPVK_VK_TO_VSC),
                    .ki.dwFlags = KEYEVENTF_KEYUP,
                    .ki.dwExtraInfo = INFO_EVENT_INJECTED
                };
            }
        }
    }
    for (size_t i = 0; i < MODIFIERKEY_COUNT; i++)
    {
        WORD vkCode = ModifierEvents[i].vkCode;
        KeyStatus modStatus = KeyMapStatus[vkCode];
        if (modStatus.isActive)
        {
            for (size_t j = 0; j < KeyMapStatus[vkCode].count; j++)
            {
                input[pos++] = (INPUT){
                    .type = INPUT_KEYBOARD,
                    .ki.wVk = vkCode,
                    .ki.wScan = MapVirtualKey(vkCode, MAPVK_VK_TO_VSC),
                    .ki.dwFlags = KEYEVENTF_KEYUP,
                    .ki.dwExtraInfo = INFO_EVENT_INJECTED
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