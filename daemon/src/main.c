// src/main.c

#include <stdio.h>
#include <stdlib.h>
#include "../header/eventHandler.h"
#include "../header/eventHandlerOs.h"
#include "../header/eventQueue.h"
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
    int msg = loadFile(path);
    if (msg != kRLErrorNone)
        return msg;

    KeyMapping* keyMapInfo = initKeyMapInfo();
    if (!keyMapInfo) 
        return 1;

    KeyStatus* keyMapStatus = initKeyMapStatus();
    if (!keyMapStatus)
    {
        free(keyMapInfo);
        return 1;
    }

    RLEventQueue* eventQueue = calloc(1, sizeof(RLEventQueue));
    if (!eventQueue)
    {
        printf("Error: calloc failed for eventQueue\n");
        free(keyMapInfo);
        free(keyMapStatus);
        return 1;
    }

    if (populateMappingTable(keyMapInfo))
    {
        free(keyMapInfo);
        free(keyMapStatus);
        free(eventQueue);
        return 1;
    }
    
    setMaps(keyMapInfo, keyMapStatus);
    setQueue(eventQueue);
    
    atexit(cleanup);

    msg = runEventLoop(keyMapStatus, keyMapInfo);

    free(keyMapStatus);
    free(keyMapInfo);
    free(eventQueue);

    return msg;
}

void cleanup()
{
    printLastError();
}
