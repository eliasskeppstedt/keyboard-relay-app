#include <stdio.h>
#include <stdlib.h>
#include "../header/eventHandler.h"
#include "../header/jsonHandler.h"
#include "../lib/cJSON.h"

void cleanup(void);

void test(KeyMapping* keyMapInfo);

int main(int argc, char* argv[]) {
    if (argc < 2)
    {
        perror("Error: filepath not specified, eg. keyboard-relay.exe my-remap.json\n");
        return 1;
    }
    else if (argc > 2)
    {
        perror("Error: program only takes 1 argument\n");
        return 1;
    }
    
    char* path = argv[1];
    if (!loadFile(path))
        return 1;

    KeyMapping* keyMapInfo = initKeyMapInfo();
    KeyStatus* keyMapStatus = initKeyMapStatus();
    EventQueue* eventQueue = calloc(1, sizeof(EventQueue));

    if (!keyMapInfo) 
        return 1;
    if (!keyMapStatus)
        return 1;
    if (populateMappingTable(keyMapInfo))
        return 1;
    
    setMaps(keyMapInfo, keyMapStatus);
    setQueue(eventQueue);
    
    atexit(cleanup);

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