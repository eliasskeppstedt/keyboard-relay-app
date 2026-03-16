#include <stdio.h>
#include <stdlib.h>
#include "../header/eventHandler.h"
#include "../header/eventQueue.h"
#include "../header/jsonHandler.h"

void cleanup(void);

void test(KeyMapping* keyMapInfo);

int main() {
    atexit(cleanup);
    KeyMapping* keyMapInfo = initKeyMapInfo();
    test(keyMapInfo);
    KeyStatus* keyMapStatus = initKeyMapStatus();
    EventQueue* eventQueue = initEventQueue();

    if (!keyMapInfo) 
    {
        perror("KeyMap malloc failed\n");
        return 1;
    }    
    if (!keyMapStatus)
    {
        perror("KeyStatusTable malloc failed\n");
        return 1;
    }

    setMaps(keyMapInfo, keyMapStatus);
    setQueue(eventQueue);

    ReturnMsg returnMsg = runEventLoop();

    free(keyMapStatus);
    free(keyMapInfo);
    free(eventQueue);

    return returnMsg;
}

void cleanup()
{
    printLastError();
}

void test(KeyMapping* keyMapInfo)
{
    keyMapInfo[66].onPress.type = KEYTYPE_UNICODE;
    keyMapInfo[66].onPress.code = 0x1F60A; // b -> 😊, 2 code point unicode 

    keyMapInfo[65].onPress.type = KEYTYPE_VIRTUAL_KEYCODE;
    keyMapInfo[65].onPress.code = 160; // a -> left shift

    keyMapInfo[68].onPress.type = KEYTYPE_VIRTUAL_KEYCODE;
    keyMapInfo[68].onPress.code = 162; // d -> left ctrl

    keyMapInfo[70].onPress.type = KEYTYPE_VIRTUAL_KEYCODE;
    keyMapInfo[70].onPress.code = 164; // a -> left alt
}