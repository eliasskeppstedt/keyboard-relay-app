#ifndef JSONHANDLER_H
#define JSONHANDLER_H

#include "types.h"
#include "constants.h"
#include "eventQueue.h"
#include "../lib/cJSON.h"

int loadFile(char* path);
KeyMapping* initKeyMapInfo();
KeyStatus* initKeyMapStatus();
RLEventQueue initEventQueue();
int populateMappingTable(KeyMapping* keyMapInfo) ;
int loadSettings(Settings* settings);
void deleteJson();

#endif // JSONHANDLER_H
