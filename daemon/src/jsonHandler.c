#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "ctype.h"
#include "jsonHandler.h"
#include "../lib/cJSON.h"

cJSON* json;

// 10MB, crazy big file for this purpouse, maybe wrong file
#define MAX_JSON_SIZE (10 * 1024 * 1024) 

int loadFile(char* fileName)
{    
    if (!fileName)
    {
        printf("Error: parameter 'fileName' is NULL\n");
        return ERR_FILE;
    }
    char* extension = ".json"; // inkludera andra filformat sen eller...
    size_t sizeExtension = strlen(extension);
    size_t sizeFileName = strlen(fileName);
    
    if (sizeFileName <= sizeExtension)
    {
        printf("Error: file name is too short, file name must be some name followed by %s\n", extension);
        return ERR_FILE;
    }

    for (size_t i = 0; i < sizeFileName - sizeExtension; i++)
    {
        if (!(isalnum((unsigned char)fileName[i]) || fileName[i] == '-' || fileName[i] == '_')) {
            printf("Error: only letters, numbers, '-' and '_' allowed in file name\n");
            return ERR_FILE;
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
        return ERR_FILE;
    }

    char* destDir = "./";
    char *path = malloc(strlen(destDir) + strlen(fileName) + 1);
    if (!path)
    {
        printf("Error: malloc failed for path\n");
        return ERR_FILE;
    }
    
    strcpy(path, destDir);
    strcat(path, fileName);

    FILE *f = fopen(path, "r");
    free(path);
    if (!f)
    {
        printf("Could not open file, \n - Is the file in keyboard-relay/mappings/?\n - Is it the correct file name?\n");
        return ERR_FILE;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    if (size < 0) {
        printf("Error: failed read file size\n");
        fclose(f);
        return ERR_FILE;
    }
    rewind(f);
    
    if (size > MAX_JSON_SIZE) {
        printf("Error: file is too large (%ld bytes)\n", size);
        fclose(f);
        return ERR_FILE;
    }

    char *jsonString = malloc(size + 1);
    if (!jsonString) {
        printf("Error: mallock failed for jsonString\n");
        fclose(f);
        return ERR_FILE;
    }

    size_t read = fread(jsonString, sizeof(char), size, f);
    if (read != (size_t)size) {
        printf("Error: failed to read file into jsonString\n");
        free(jsonString);
        fclose(f);
        return ERR_FILE;
    }

    jsonString[size] = '\0';
    fclose(f);

    json = cJSON_Parse(jsonString);
    free(jsonString);
    if (!json)
        return ERR_FILE;
    
    return ERR_NIL;
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
            .type = KEYTYPE_SRC_EVENT,
            .code[0] = i,
            .size = 1
        };

        keyMapInfo[i].onHold = (KeyAction){
            .type = KEYTYPE_SRC_EVENT,
            .code[0] = i,
            .size = 1
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
            .keyType = RL_KEY_SRC
        };
    }

    return keyMapStatus;
} 

int populateMappingTable(KeyMapping* keyMapInfo) 
{
    cJSON* remaps = cJSON_GetObjectItemCaseSensitive(json, "remaps");
    if (!remaps) {
        printf("Error: remaps could not be retrieved\n");
        return ERR_JSON;
    }

    cJSON* layers = cJSON_GetObjectItemCaseSensitive(remaps, "layers");
    if (!layers) {
        printf("Error: layers could not be retrieved\n");
        return ERR_JSON;
    }
    cJSON* layer;
    cJSON* keys;
    cJSON* key;
    cJSON* from;
    cJSON* actions;
    cJSON* action;
    cJSON* press;
    cJSON* type;
    cJSON* codes;
    cJSON* code;

    int layerCount = 0;
    cJSON_ArrayForEach(layer, layers) 
    {
        printf("Layer %d: \n", layerCount);
        if (!layer) {
            printf("Error: layer could not be retrieved\n");
            return ERR_JSON;
        }

        keys = cJSON_GetObjectItemCaseSensitive(layer, "keys");
        if (!keys) {
            printf("Error: keys could not be retrieved\n");
            return ERR_JSON;
        }

        int keyCount = 0;
        cJSON_ArrayForEach(key, keys) 
        {
            printf("Layer %d: Key %d:\n", layerCount, keyCount);
            if (!key) {
                printf("Error: key could not be retrieved\n");
                return ERR_JSON;
            }

            from = cJSON_GetObjectItemCaseSensitive(key, "vkCode");
            if (!from) {
                printf("Error: base key vkCode could not be retrieved\n");
                return ERR_JSON;
            }
            if (!cJSON_IsNumber(from)) {
                printf("Error: base key vkCode not a number\n");
                return ERR_JSON;
            }
            unsigned int fromVKCode = (unsigned int)cJSON_GetNumberValue(from);
            if (!(fromVKCode < VKC_COUNT)) {
                printf("Error: base vkCode too big, can be of max value %d\n", VKC_COUNT);
                return ERR_JSON;
            }

            actions = cJSON_GetObjectItemCaseSensitive(key, "actions");
            if (!actions) {
                printf("Error: actions could not be retrieved\n");
                return ERR_JSON;
            }
            int actionCount = 0;
            cJSON_ArrayForEach(action, actions)
            {
                printf("Layer %d: Key %d: Action %d: \n", layerCount, keyCount, actionCount);
                press = cJSON_GetObjectItemCaseSensitive(action, "press");
                if (!press) {
                    printf("Error: press could not be retrieved\n");
                    return ERR_JSON;
                }
                type = cJSON_GetObjectItemCaseSensitive(press, "type");
                if (!type) {
                    printf("Error: type could not be retrieved\n");
                    return ERR_JSON;
                }
                char* typeStr = cJSON_GetStringValue(type);
                if (!typeStr)
                {
                    printf("Error: type is not a string\n");
                    return ERR_JSON;
                }
                int typeId = -1;
                for (int i = 0; i < KEYTYPE_COUNT; i++)
                {
                    if (strcmp(typeStr, "vkCode") == 0) {
                        typeId = KEYTYPE_VIRTUAL_KEYCODE;
                    }
                    if (strcmp(typeStr, "unicode") == 0) {
                        typeId = KEYTYPE_UNICODE;
                    }
                }
                if (typeId == -1) {
                    printf("Error: type missmatch on code type\n");
                    return ERR_JSON;
                }

                codes = cJSON_GetObjectItemCaseSensitive(press, "codes");
                if (!codes) {
                    printf("Error: action code could not be retrieved\n");
                    return ERR_JSON;
                }
                int size = cJSON_GetArraySize(codes);
                if (size > UNICODE_MAX_CODE_POINTS)
                {
                    printf("Error: to many codes\n");
                    return ERR_JSON;
                }
                
                keyMapInfo[fromVKCode].onPress.size = cJSON_GetArraySize(codes);
                keyMapInfo[fromVKCode].onPress.type = typeId;
                int codeIdx = 0;

                cJSON_ArrayForEach(code, codes)
                {                    
                    printf("Layer %d: Key %d: Action %d: Code %d:\n", layerCount, keyCount, actionCount, codeIdx);

                    unsigned long toCodeOnPress = (unsigned long)cJSON_GetNumberValue(code);

                    if (typeId == KEYTYPE_UNICODE) {
                        if (!(toCodeOnPress < UNICODE_COUNT)) {
                            printf("Error: unicode on press too big, can be of max value %d\n", UNICODE_COUNT);
                            return ERR_JSON;
                        }
                    }
                    else if (!(toCodeOnPress < VKC_COUNT)) {
                        printf("Error: vkCode on press too big, can be of max value %d\n", VKC_COUNT);
                        return ERR_JSON;
                    }
                    else if (codeIdx == 1)
                    {
                        printf("Error: multiple virtual key codes are unsupported atm\n");
                        return ERR_JSON;
                    }
                    

                    keyMapInfo[fromVKCode].onPress.code[codeIdx++] = toCodeOnPress;
                }
                actionCount++;
            }
            keyCount++;
        }
        layerCount++;
    }

    return ERR_NIL;
}

int loadSettings(Settings* settings)
{
    return ERR_NIL;
}

void deleteJson() 
{
    cJSON_Delete(json);
}
