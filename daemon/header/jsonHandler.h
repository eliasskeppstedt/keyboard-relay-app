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
ReturnMsg loadSettings(Settings* settings);
void deleteJson();

#endif // JSONHANDLER_H