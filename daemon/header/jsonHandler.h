#ifndef JSONHANDLER_H
#define JSONHANDLER_H

#include "../header/types.h"
#include "../header/constants.h"
#include "../lib/cJSON.h"

ReturnMsg loadFile(char* path);
KeyMapping* initKeyMapInfo();
KeyStatus* initKeyMapStatus();
EventQueue* initEventQueue();
ReturnMsg populateMappingTable(KeyMapping* keyMapInfo) ;
void testWindows(KeyMapping* keyMapInfo);

#endif // JSONHANDLER_H