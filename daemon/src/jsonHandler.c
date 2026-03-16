#include "stdlib.h"
#include "../lib/cJSON.h"
#include "jsonHandler.h"

#define NO_CODE 300
//#define SC_MAX 57446 // 
#define VKC_MAX 256

KeyMapping* initKeyMapInfo()
{
    KeyMapping* keyMapInfo = malloc(sizeof(KeyMapping) * VKC_MAX);

    for (uint32_t i = 0; i < VKC_MAX; i++)
    {
        keyMapInfo[i].onPress = (KeyAction){
            .type = KEYTYPE_VIRTUAL_KEYCODE_PASSTHROW,
            .code = NO_CODE
        };

        keyMapInfo[i].onHold = (KeyAction){
            .type = KEYTYPE_VIRTUAL_KEYCODE_PASSTHROW,
            .code = NO_CODE
        };
    }

    return keyMapInfo;
}

KeyStatus* initKeyMapStatus()
{
    KeyStatus* keyMapStatus = malloc(sizeof(KeyStatus) * VKC_MAX);

    for (uint32_t i = 0; i < VKC_MAX; i++)
    {
        keyMapStatus[i] = (KeyStatus) {
            .isActive = false,
            .activeCode = NO_CODE
        };
    }

    return keyMapStatus;
}   

EventQueue* initEventQueue() 
{
    return calloc(1, sizeof(EventQueue));
}