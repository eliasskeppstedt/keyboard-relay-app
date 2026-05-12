#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdint.h>

#define PRINT_BOOL(str, bool) printf("%s: %s\n", str, bool ? "true" : "false");

#define VKC_COUNT 256
#define UNICODE_COUNT 0x10FFFF
#define NO_CODE 300
#define UNICODE_MAX_CODE_POINTS 4

#ifdef _WIN32

#define Esc = 0x1B; // 

#define LSHIFT          0xa0 // VK_LSHIFT
#define RSHIFT          0xa1 // VK_RSHIFT
#define LCTRL           0xa2 // VK_LCONTROL
#define RCTRL           0xa3 // VK_RCONTROL
#define LALT            0xa4 // VK_LMENU
#define RALT            0xa5 // VK_RMENU

#elif defined __APPLE__

#define Esc 0x35 // change when gui is implemented (Qt)

#define Autorepeat 8 // kCGEventKeyboardAutorepeat
#define NullEvent 0 // kcgeventnull

#endif

#define MAX_QUEUE_SIZE 101

typedef enum RLOutputType{
    kRLOutputTypeKeyCode = 0,
    kRLOutputTypeUnicode = 1,
    kRLOutputTypeNone = 2,
} RLOutputType;

typedef enum RLInputEventKind {
    kRLInputEventKindKeyboard = 0,
    kRLInputEventKindHoldTimer = 1,
} RLInputEventKind;

typedef enum RLActionType {
    kRLActionTypePress = 0,
    kRLActionTypePendingHold = 1,
    kRLActionTypeHold = 2,
} RLActionType;

typedef enum RLHandleResult {
    kRLHandleResultHandled = 0,
    kRLHandleResultPendingHold = 1,
    kRLHandleResultPassThrough = 2,
    kRLHandleResultError = 3,
} RLHandleResult;

typedef enum RLUserData {
    kRLUserDataNone = 0,
    kRLUserDataEventInjected = 1,
    kRLUserDataToggleKey = 2,
} RLUserData;

typedef enum RLError {
    kRLErrorNone = 0,
    kRLErrorBadHook = -1,
    kRLErrorQuitByUser = -2,
    kRLErrorSyntheticEventFailed = -3,
    kRLErrorQueueFull = -4,
    //kRLErrorQueueEmpty = -5,
    kRLErrorEventNotFound = -6,
    kRLErrorInvalidUnicode = -7,
    kRLErrorFile = -8,
    kRLErrorJson = -9,
    kRLErrorRunDaemon = -10,
    kRLErrorMallocFailed = -11,
    kRLErrorTimerFailed = -12,
} RLError;

#endif // CONSTANTS_H
