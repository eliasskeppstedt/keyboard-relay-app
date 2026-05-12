# Keyboard ReLay
Keyboard ReLay is a software which is used to modify conventional keyboards. The application is 
going to intercept keyboard inputs and simulate a new keystroke based on what modifications are
made. The purpose here is to enable easy modification through a easy-to-use web interface, which is 
used to create a mapping profile for your keyboard, which can then be interpreted by this program. 
The purpose of this is to abstract away the complexity of custom keyboard mappings and make keyboard 
customization accessible to everyone.

Website is managed in this repository: https://github.com/eliasskeppstedt/keyboard-relay-website

## Current state (v0.0.3)
Latest status of windows release [v0.0.2](https://github.com/eliasskeppstedt/keyboard-relay-app/tree/cd1107d5e421ce4ed343eb6afcdf4461eb883d3b).

v0.0.2 has macOS support, where both press and hold remaps are available to create. The website should be compliant
with this versions new json structure, however if it isnt this is the new structure of it:
```json
{
    "remaps": {
        "layers": [
            {
                "name": "Main",
                "id": 0,
                "config": {},
                "keys": [
                    {
                        "code": "KeyA",
                        "vkCode": 65,
                        "actions": [
                            {
                                "type": "press",
                                "outputType": "vkCode",
                                "codes": [
                                    [
                                        31
                                    ]
                                ]
                            }
                        ]
                    },
                    {
                        "code": "KeyH",
                        "vkCode": 4,
                        "actions": [
                            {
                                "type": "hold",
                                "outputType": "unicode",
                                "codes": [
                                    [
                                        55357, 56842
                                    ]
                                ]
                            }
                        ]
                    },
                ]
            }
        ],
        "config": {
            "os": "WINDOWS",
            "language": "swedish",
            "standard": "Show All",
            "keyboardName": "ISO 100 - Generic",
            "layout": "My Remap"
        },
        "extras": []
    }
}
```
where `a` is mapped to `o` when pressed and `h`mapped to `😊` when held.

## Run program

Download and unzip the `keyboard-relay` folder. The program will be in that folder, which is also
where you should put your mapping json files. Right click on an empty space in the folder and in 
the menu select `Open in Terminal`. From there you should write  
```
./keyboard-relay.exe <file-name>.json
```
where you replace `<file-name>.json` to the name of the mapping file you want to run.

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
