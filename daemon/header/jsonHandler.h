#ifndef JSONHANDLER_H
#define JSONHANDLER_H

#include "../header/types.h"
#include "../header/constants.h"
#include "../lib/cJSON.h"

bool loadFile(char* path);
KeyMapping* initKeyMapInfo();
KeyStatus* initKeyMapStatus();
EventQueue* initEventQueue();
int populateMappingTable(KeyMapping* keyMapInfo) ;
void testWindows(KeyMapping* keyMapInfo);

#endif // JSONHANDLER_H