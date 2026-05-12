#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "ctype.h"
#include "jsonHandler.h"
#include "../lib/cJSON.h"

cJSON* json;

// 10MB, crazy big file for this purpouse, maybe wrong file
#define MAX_JSON_SIZE (10 * 1024 * 1024) 

RLError loadFile(char* fileName)
{    
    if (!fileName)
    {
        printf("Error: parameter 'fileName' is NULL\n");
        return kRLErrorFile;
    }
    char* extension = ".json"; // inkludera andra filformat sen eller...
    size_t sizeExtension = strlen(extension);
    size_t sizeFileName = strlen(fileName);
    
    if (sizeFileName <= sizeExtension)
    {
        printf("Error: file name is too short, file name must be some name followed by %s\n", extension);
        return kRLErrorFile;
    }

    for (size_t i = 0; i < sizeFileName - sizeExtension; i++)
    {
        if (!(isalnum((unsigned char)fileName[i]) || fileName[i] == '-' || fileName[i] == '_')) {
            printf("Error: only letters, numbers, '-' and '_' allowed in file name\n");
            return kRLErrorFile;
        }
    }

    char pathExtension[6]; // hårdkådat, vet funkar för .json\0 ...
    for (int i = (int)sizeFileName, j = (int)sizeExtension; j >= 0; i--, j--)
    {
        pathExtension[j] = fileName[i];
    }

    if (strcmp(pathExtension, extension) != 0)
    {
        printf("Error: pathname must end with %s\n", extension);
        return kRLErrorFile;
    }

    char* destDir = "./";
    char *path = malloc(strlen(destDir) + strlen(fileName) + 1);
    if (!path)
    {
        printf("Error: malloc failed for path\n");
        return kRLErrorFile;
    }
    
    strcpy(path, destDir);
    strcat(path, fileName);

    FILE *f = fopen(path, "r");
    free(path);
    if (!f)
    {
        printf("Could not open file, \n - Is the file in keyboard-relay/mappings/?\n - Is it the correct file name?\n");
        return kRLErrorFile;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    if (size < 0) {
        printf("Error: failed read file size\n");
        fclose(f);
        return kRLErrorFile;
    }
    rewind(f);
    
    if (size > MAX_JSON_SIZE) {
        printf("Error: file is too large (%ld bytes)\n", size);
        fclose(f);
        return kRLErrorFile;
    }

    char *jsonString = malloc(size + 1);
    if (!jsonString) {
        printf("Error: mallock failed for jsonString\n");
        fclose(f);
        return kRLErrorFile;
    }

    size_t read = fread(jsonString, sizeof(char), size, f);
    if (read != (size_t)size) {
        printf("Error: failed to read file into jsonString\n");
        free(jsonString);
        fclose(f);
        return kRLErrorFile;
    }

    jsonString[size] = '\0';
    fclose(f);

    json = cJSON_Parse(jsonString);
    free(jsonString);
    if (!json)
        return kRLErrorFile;
    
    return kRLErrorNone;
}

KeyMapping* initKeyMapInfo()
{
    KeyMapping* keyMapInfo = malloc(sizeof(KeyMapping) * VKC_COUNT);
    if (!keyMapInfo)
    {
        printf("Error: malloc failed for keyMapInfo\n");
        return NULL;
    }
    

    for (size_t i = 0; i < VKC_COUNT; i++)
    {
        keyMapInfo[i].onPress = (KeyAction){
            .outputType = kRLOutputTypeNone,
        };

        keyMapInfo[i].onHold = (KeyAction){
            .outputType = kRLOutputTypeNone,
        };
    }

    return keyMapInfo;
}

KeyStatus* initKeyMapStatus()
{
    KeyStatus* keyMapStatus = malloc(sizeof(KeyStatus) * VKC_COUNT);
    if (!keyMapStatus)
    {
        printf("Error: malloc failed for keyMapStatus\n");
        return NULL;
    }
    
    for (size_t i = 0; i < VKC_COUNT; i++)
    {
        keyMapStatus[i] = (KeyStatus) {
            .keyDown = false,
        };
    }

    return keyMapStatus;
} 

RLError populateMappingTable(KeyMapping* keyMapInfo) 
{
    cJSON* remaps = cJSON_GetObjectItemCaseSensitive(json, "remaps");
    if (!remaps) {
        printf("Error: remaps could not be retrieved\n");
        return kRLErrorJson;
    }

    cJSON* layers = cJSON_GetObjectItemCaseSensitive(remaps, "layers");
    if (!layers) {
        printf("Error: layers could not be retrieved\n");
        return kRLErrorJson;
    }
    cJSON* layer;
    cJSON* keys;
    cJSON* key;
    cJSON* from;
    cJSON* actions;
    cJSON* action;
    cJSON* type;
    cJSON* codes;
    cJSON* code;
    cJSON* charArr;
    cJSON* codeUnit;

    int layerCount = 0;
    cJSON_ArrayForEach(layer, layers) {
        printf("Layer %d: \n", layerCount);
        if (!layer) {
            printf("Error: layer could not be retrieved\n");
            return kRLErrorJson;
        }

        keys = cJSON_GetObjectItemCaseSensitive(layer, "keys");
        if (!keys) {
            printf("Error: keys could not be retrieved\n");
            return kRLErrorJson;
        }

        int keyCount = 0;
        cJSON_ArrayForEach(key, keys) {
            printf("Layer %d: Key %d:\n", layerCount, keyCount);
            if (!key) {
                printf("Error: key could not be retrieved\n");
                return kRLErrorJson;
            }

            from = cJSON_GetObjectItemCaseSensitive(key, "vkCode");
            if (!from) {
                printf("Error: base key vkCode could not be retrieved\n");
                return kRLErrorJson;
            }
            if (!cJSON_IsNumber(from)) {
                printf("Error: base key vkCode not a number\n");
                return kRLErrorJson;
            }
            unsigned int fromVKCode = (unsigned int)cJSON_GetNumberValue(from);
            if (!(fromVKCode < VKC_COUNT)) {
                printf("Error: base vkCode too big, can be of max value %d\n", VKC_COUNT);
                return kRLErrorJson;
            }

            actions = cJSON_GetObjectItemCaseSensitive(key, "actions");
            if (!actions) {
                printf("Error: actions could not be retrieved\n");
                return kRLErrorJson;
            }
            int actionCount = 0;
            cJSON_ArrayForEach(action, actions) {
                type = cJSON_GetObjectItemCaseSensitive(action, "type");
                if (!type) {
                    printf("Error: type could not be retrieved\n");
                    return kRLErrorJson;
                }
                char* typeStr = cJSON_GetStringValue(type);
                if (!typeStr) {
                    printf("Error: type is not a string\n");
                    return kRLErrorJson;
                }

                KeyAction* keyAction = (strcmp(typeStr, "press") == 0)
                    ? &keyMapInfo[fromVKCode].onPress
                    : &keyMapInfo[fromVKCode].onHold;

                cJSON* outputType = cJSON_GetObjectItemCaseSensitive(action, "outputType");
                if (!outputType) {
                    printf("Error: outputType could not be retrieved\n");
                    return kRLErrorJson;
                }
                char* outputTypeStr = cJSON_GetStringValue(outputType);
                if (!outputTypeStr) {
                    printf("Error: outputType is not a string\n");
                    return kRLErrorJson;
                }

                int typeId = -1;
                if (strcmp(outputTypeStr, "vkCode") == 0)   typeId = kRLOutputTypeKeyCode;
                if (strcmp(outputTypeStr, "unicode") == 0)  typeId = kRLOutputTypeUnicode;
                if (typeId == -1) {
                    printf("Error: outputType mismatch\n");
                    return kRLErrorJson;
                }
                keyAction->outputType = typeId;

                codes = cJSON_GetObjectItemCaseSensitive(action, "codes");
                if (!codes) {
                    printf("Error: action code could not be retrieved\n");
                    return kRLErrorJson;
                }
                
                int arraySize = cJSON_GetArraySize(codes);
                if (arraySize < 1)
                {
                    printf("Error: cant be less than 1 code\n");
                    return kRLErrorJson;
                }

                int totalUnits = 0;
                cJSON_ArrayForEach(charArr, codes)
                {
                    if (!cJSON_IsArray(charArr)) {
                        printf("Error: code is not an array, should be array of codes (arrays)\n");
                        return kRLErrorJson;
                    }
                    cJSON_ArrayForEach(codeUnit, charArr)
                    {
                        if (!cJSON_IsNumber(codeUnit)) {
                            printf("Error: unit is not a number\n");
                            return kRLErrorJson;
                        }
                        if (typeId == kRLOutputTypeUnicode) {
                            RLUnicode toCode = (RLUnicode)cJSON_GetNumberValue(codeUnit);

                            if (toCode >= UNICODE_COUNT) {
                                printf("Error: unicode too big, max %d\n", UNICODE_COUNT);
                                return kRLErrorJson;
                            }
                            if (totalUnits < UNICODE_MAX_CODE_POINTS) {
                                keyAction->sendCode.unicode.chars[totalUnits++] = toCode;
                            } else {
                                printf("Warning: too many unicode points, capping at %d\n", UNICODE_MAX_CODE_POINTS);
                            }
                        } else if (typeId == kRLOutputTypeKeyCode) {
                            RLKeyCode toCode = (RLKeyCode)cJSON_GetNumberValue(codeUnit);
                            if (toCode >= VKC_COUNT) {
                                printf("Error: vkCode too big, max %d\n", VKC_COUNT);
                                return kRLErrorJson;
                            }
                            if (totalUnits == 0) {
                                keyAction->sendCode.keyCode = toCode;
                                totalUnits++;
                            }
                        }
                    }
                }
                if (typeId == kRLOutputTypeUnicode) {
                    keyAction->sendCode.unicode.length = totalUnits;
                }
                actionCount++;
            }
            keyCount++;
        }
        layerCount++;
    }

    return kRLErrorNone;
}

RLError loadSettings(Settings* settings)
{
    return kRLErrorNone;
}

void deleteJson() 
{
    cJSON_Delete(json);
}
