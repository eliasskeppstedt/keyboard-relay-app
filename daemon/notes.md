# Notes

## [Unicode](https://learn.microsoft.com/en-us/windows/win32/intl/unicode)
Encoded in UTF-16 (wide character)  
Types of UTF-16 encodings:
1. code values in range U+0000 to U+D7FF and U+E000 to U+FFFF, support for the Basic Multilingual Plane (BMP).
2. surrogate code value in range U+D800 to U+DFFF. Used to form surrogate pairs for supplementary characters.
Supplementary character are characters with their code points in range U+10000 to U+10FFFF

## Implementations

### Windows

Method of intercepting events: [hooks](https://learn.microsoft.com/en-us/windows/win32/winmsg/about-hooks)

Hook used: [SetWindowsHookExW](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowshookexw)  
\* However `SetWindowsHookEx` is used, which is an alias which based on a processor constant chooses the ANSI or Unicode version of the function. Umm,,, when would the ANSI version be necessary tho

Hook procedure - this is the callback function used to decide what type of messages the hook should monitor. The callback func is put first in the hook chain, and depending on which procedure is used view messages or even remove them from ever reaching the next hook procedure. To monitor and modify keyboard events, we can use [LowLevelKeyboardProc](https://learn.microsoft.com/en-us/windows/win32/winmsg/lowlevelkeyboardproc).
```c
LRESULT CALLBACK LowLevelKeyboardProc(
  _In_ int    nCode, 
  _In_ WPARAM wParam,
  _In_ LPARAM lParam   
);
```
`nCode`: code for the function to know how to process the message, must be `HC_ACTION` (0) in order to proceed with 
key modification.

`wParam`: msg identifier, can be `WM_KEYDOWN`, `WM_KEYUP`, `WM_SYSKEYDOWN`, `WM_SYSKEYUP`. A sys key occures when Alt is pressed or held, when menu bar (by f10?) is activated, or when no window has the keyboard focus.

`lParam`: pointer to a [KBDLLHOOKSTRUCT](https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-kbdllhookstruct): 
```c
typedef struct tagKBDLLHOOKSTRUCT {
  DWORD     vkCode;
  DWORD     scanCode;
  DWORD     flags;
  DWORD     time;
  ULONG_PTR dwExtraInfo;
} KBDLLHOOKSTRUCT, *LPKBDLLHOOKSTRUCT, *PKBDLLHOOKSTRUCT;
```

[`vkCode`](https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes): codes produced by a key on layout basis

[`scanCode`](https://learn.microsoft.com/en-us/windows/win32/inputdev/about-keyboard-input#scan-codes): codes produced from the physical key location, regardless of layout

`flags`: extended, injected, context code, transition-state

`time`: time stamp for message, same type as [GetMessageTime](https://learn.microsoft.com/en-us/windows/desktop/api/winuser/nf-winuser-getmessagetime) would return

`dwExtraInfo`: uhm yeah extra info about the msg

#### Unicode
UTF-16 uses code values, however older unicode implementations use code pages... **relevant?**

#### Input methods

[LoadKeyboardLayoutW](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-loadkeyboardlayoutw)

[Language identifier](learn.microsoft.com/en-us/windows/win32/intl/language-identifiers)

[Keyboard identifiers](https://learn.microsoft.com/en-us/windows-hardware/manufacture/desktop/windows-language-pack-default-values?view=windows-11)

### Mac
Will be developed with [Quartz Event Services](https://leopard-adc.pepas.com/documentation/Carbon/Reference/QuartzEventServicesRef/QuartzEventServicesRef.pdf) in the [Core Graphics](https://developer.apple.com/documentation/coregraphics) framework.
Dont know, but is a future goal