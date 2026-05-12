#ifndef JSONHANDLER_H
#define JSONHANDLER_H

#include "types.h"
#include "constants.h"
#include "eventQueue.h"
#include "../lib/cJSON.h"

RLError loadFile(char* path);
KeyMapping* initKeyMapInfo();
KeyStatus* initKeyMapStatus();
RLEventQueue initEventQueue();
RLError populateMappingTable(KeyMapping* keyMapInfo) ;
RLError loadSettings(Settings* settings);
void deleteJson();

#endif // JSONHANDLER_H
