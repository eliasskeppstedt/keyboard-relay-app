# Changelog

## v0.0.1 (2026-03-23)
Features:
- windows release only
- remap press actions
- only virtual key codes (unicode support not included)
- all keys are not tested, however should support basic keys defined by the website

Limitations
- `Esc` is hardcoded as the exit key. Since the program does not have a GUI yet, the program can 
be terminated by pressing `ctrl+c` when inside the terminal.
- bugs related to key states can still occur
- `alt + shift + tab` when `alt` and `shift` are syntethic does not work, possibly more system level combos

If keys start to behave weird, it is because the state of a key is not correct. You can reset them by using 
one of these combos
- `shift + ctrl + win + alt`
- `ctrl + alt + delete`. then press `esc`
