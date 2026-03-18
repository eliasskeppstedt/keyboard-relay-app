# Keyboard ReLay
Keyboard ReLay is a software which is used to modify conventional keyboards. The application is 
going to intercept keyboard inputs and simulate a new keystroke based on what modifications are
made. The purpose here is to enable easy modification through a easy-to-use web interface, which is 
used to create a mapping profile for your keyboard, which can then be interpreted by this program. 
The purpose of this is to abstract away the complexity of custom keyboard mappings and make keyboard 
customization accessible to everyone.

Website is managed in this repository: https://github.com/eliasskeppstedt/keyboard-relay-website

## Current state
Work on daemon is started and should now support press modifications with virtual key codes and
unicodes. A problem i encountered that i did not (yet hopefully 😅) solve is the key combo
alt + shift + tab, or the combo used to tab over the windows backwards. It works when either alt
or shift is native (not remapped), but not when both are synthetic. Since this does not work, there
might be other shortcuts that does not work...

Json integration is now done, meaning that the program will read the json file at the path you
specify from your working dir. However I messed up the mapping on the website and it does not quite
produce the correct output for my program. Here is an example of how the key object should look 
like, mapping Q to Tab
```json
"keys": [
    {
        "code": "KeyQ",
        "vkCode": 81,
        "actions": [
            {
                "press": {
                    "type": "VKC",
                    "vkCode": 9
                }
            }
        ]
    }
]
```
For reference, here is how the website generate the key objects
```json
"keys": [
    {
        "code": "KeyQ",
        "action": {
            "press": {
                "type": "VKC",
                "vkCode": [
                    9
                ]
            }
        }
    }
]
```

## Future (goals)
The goal is to make this OS agnostic such that it can be used on whatever machine you are on and
with the same mapping configuration. It should support at least `press` and `hold` modifications, 
but the ambition is to include more over time, ex `double press`.

The program should consist of 2 parts. The heart of the program (a daemon) which is what is handling 
all the logic for intercepting and simulating keystrokes and a GUI, which is the only part the user 
should have contact with. In it the user should be able to change user configurations (such as time
for creating a `hold` action and settings for language input) and other stuff that'll come up along 
the way. 

Mapping is handled on the website and is downloaded as a json file. In order to use this mapping, 
the user should load it into the program (ig like "Choose mapping file" or alike) and then start 
the program, everything through the GUI

## Security
Only use configuration files that you trust. The website only generates the configuration (as a
json file), which when downloaded can be inspected as plain text before use. 
