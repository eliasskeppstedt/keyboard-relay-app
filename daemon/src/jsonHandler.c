#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "../lib/cJSON.h"
#include "jsonHandler.h"

cJSON* json;

#define VKC_MAX 256

bool loadFile(char* path)
{    
    char* extension = ".json"; // inkludera andra filformat sen eller...
    const int sizeExtension = strlen(extension);
    int sizePath = strlen(path);
    if (sizePath <= sizeExtension)
    {
        printf("Error: pathname is too short, file name must be some name followed by %s\n", extension);
        return NULL;
    }
    
    char pathExtension[6]; // hårdkådat, vet funkar för .json\0 ...

    for (int i = sizePath, j = sizeExtension; j >= 0; i--, j--)
    {
        pathExtension[j] = path[i];
    }

    if (strcmp(pathExtension, extension) != 0)
    {
        printf("Error: pathname must end with %s\n", extension);
        return NULL;
    }

    FILE *f = fopen(path, "r");
    if (!f)
    {
        printf("Could not open file, is the path name correct?\n");
        return NULL;
    }

    uint32_t size = 0;
    while (fgetc(f) != EOF)
        size++;

    rewind(f);

    char *jsonString = malloc(size + 1);
    jsonString[size] = '\0';
    fread(jsonString, sizeof(char), size, f);
    fclose(f);

    json = cJSON_Parse(jsonString);
    free(jsonString);
    if (!json)
        return 1;
    return true;
}

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

int populateMappingTable(KeyMapping* keyMapInfo) 
{
    cJSON* remaps = cJSON_GetObjectItemCaseSensitive(json, "remaps");
    if (!remaps) {
        printf("Error: remaps could not be retrieved\n");
        return 1;
    }

    cJSON* layers = cJSON_GetObjectItemCaseSensitive(remaps, "layers");
    if (!layers) {
        printf("Error: layers could not be retrieved\n");
        return 1;
    }
    cJSON* layer;
    cJSON* keys;
    cJSON* key;
    cJSON* from;
    cJSON* actions;
    cJSON* action;
    cJSON* press;
    cJSON* type;
    cJSON* vkCode;

    int layerCount = -1;
    cJSON_ArrayForEach(layer, layers) {
        printf("Layer %d: \n", ++layerCount);
        if (!layer) {
            printf("Error: layer could not be retrieved\n");
            return 1;
        }

        keys = cJSON_GetObjectItemCaseSensitive(layer, "keys");
        if (!keys) {
            printf("Error: keys could not be retrieved\n");
            return 1;
        }

        int keyCount = -1;
        cJSON_ArrayForEach(key, keys) {
            printf("Layer %d: Key %d:\n", layerCount, ++keyCount);
            if (!key) {
                printf("Error: key could not be retrieved\n");
                return 1;
            }

            from = cJSON_GetObjectItemCaseSensitive(key, "vkCode");
            if (!from) {
                printf("Error: base key vkCode could not be retrieved\n");
                return 1;
            }
            if (!from) {
                printf("Error: base key vkCode could not be retrieved\n");
                return 1;
            }
            if (!cJSON_IsNumber(from)) {
                printf("Error: base key vkCode not a number\n");
                return 1;
            }

            actions = cJSON_GetObjectItemCaseSensitive(key, "actions");
            if (!actions) {
                printf("Error: actions could not be retrieved\n");
                return 1;
            }
            int actionCount = -1;
            cJSON_ArrayForEach(action, actions)
            {
                printf("Layer %d: Key %d: Action %d: \n", layerCount, keyCount, ++actionCount);
                printf("", actionCount++);
                press = cJSON_GetObjectItemCaseSensitive(action, "press");
                if (!press) {
                    printf("Error: press could not be retrieved\n");
                    return 1;
                }

                type = cJSON_GetObjectItemCaseSensitive(press, "type");
                if (!press) {
                    printf("Error: type could not be retrieved\n");
                    return 1;
                }

                // atm hårdkådat för vkc
                int isVKC;
                for (int i = 0; i < KEYTYPE_COUNT; i++)
                {
                    isVKC = strcmp(cJSON_GetStringValue(type), "VKC") == 0;
                    if (isVKC) {  
                        i++;
                        break;
                    }
                }
                if (!isVKC) {
                    printf("Error: type missmatch on type vkCode\n");
                    return 1;
                }

                vkCode = cJSON_GetObjectItemCaseSensitive(press, "vkCode");
                if (!vkCode) {
                    printf("Error: action vkCode could not be retrieved\n");
                    return 1;
                }
                if (!cJSON_IsNumber(vkCode)) {
                    printf("Error: action vkCode not a number\n");
                    return 1;
                }

                keyMapInfo[(int) cJSON_GetNumberValue(from)] = (KeyMapping){
                    .onPress.type = KEYTYPE_VIRTUAL_KEYCODE,
                    .onPress.code = cJSON_GetNumberValue(vkCode)
                };
            }
        }
    }

    return 0;
}

void testWindows(KeyMapping* keyMapInfo)
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