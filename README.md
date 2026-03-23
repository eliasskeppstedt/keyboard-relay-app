# Keyboard ReLay
Keyboard ReLay is a software which is used to modify conventional keyboards. The application is 
going to intercept keyboard inputs and simulate a new keystroke based on what modifications are
made. The purpose here is to enable easy modification through a easy-to-use web interface, which is 
used to create a mapping profile for your keyboard, which can then be interpreted by this program. 
The purpose of this is to abstract away the complexity of custom keyboard mappings and make keyboard 
customization accessible to everyone.

Website is managed in this repository: https://github.com/eliasskeppstedt/keyboard-relay-website

## Current state
Work on daemon is started and should now support press modifications with virtual key codes. 
Cant get alt + shift + tab to work, when both alt and shift are synthetic. When holding synthetic
alt and shift, tab is not even registred by the hook procedure, it is like windows is eating up
the tab event. So for now, this combo will not work unless at least native alt or native shift
is involved.

JSON integration is in place, such that the website mapping tool now can be used to generate
a functional mapping for the program. 

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
