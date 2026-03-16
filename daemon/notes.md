# Notes

## Flow



## Reading kbr event

### Windows - [`Hooks`](https://learn.microsoft.com/en-us/windows/win32/winmsg/about-hooks)

Hook used: [`SetWindowsHookExW`](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowshookexw) (`SetWindowsHookEx` for compatibility i think)

Hook procedure - this is the callback function used to decide what type of messages the hook should monitor. The callback func is put first in the hook chain, and depending on which procedure is used view messages or even remove them from ever reaching the next hook procedure. To monitor and modify keyboard events, we can use [`LowLevelKeyboardProc`](https://learn.microsoft.com/en-us/windows/win32/winmsg/lowlevelkeyboardproc).
```c
LRESULT CALLBACK LowLevelKeyboardProc(
  _In_ int    nCode, 
  _In_ WPARAM wParam,
  _In_ LPARAM lParam   
);
```
`nCode`: procedure type, in this case `WH_KEYBOARD_LL` (double check)

`wParam`: msg identifier, can be `WM_KEYDOWN`, `WM_KEYUP`, `WM_SYSKEYDOWN`, `WM_SYSKEYUP`. A sys key occures when Alt is pressed or held, when menu bar (by f10?) is activated, or when no window has the keyboard focus.

`lParam`: pointer to a [`KBDLLHOOKSTRUCT`](https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-kbdllhookstruct): 
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

`time`: time stamp for message, same type as [`GetMessageTime`](https://learn.microsoft.com/en-us/windows/desktop/api/winuser/nf-winuser-getmessagetime) would return

`dwExtraInfo`: uhm yeah extra info about the msg


## Loading new kbr layout

### Windows

[`LoadKeyboardLayoutW`](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-loadkeyboardlayoutw)

[`Language identifier`](learn.microsoft.com/en-us/windows/win32/intl/language-identifiers)

[`Keyboard identifiers`](https://learn.microsoft.com/en-us/windows-hardware/manufacture/desktop/windows-language-pack-default-values?view=windows-11)