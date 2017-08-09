# Arduino-network-status
Turns Arduino based systems into client that can recieve JSON messages about the networks
status. With that data the Arduino can drive an Ikea Dioder led strip (or any other) and 
represent the network status through LED color.

## Intention
Sometimes you just want to see a networks status represented by something visual.

## TO DO
[x] Figure out useful data structure for the data (structs, lists, maps, ...).  
[x] Figure out how to transelate the HTTP JSON response into a useful data structure.  
[x] Invent and impelemt functions for the data structures (fetch(), update(), trigger(), ...).  
[] Research good physical triggers for network status (light, sound, ...).  
[] Program service that pushes data to the Arduino.  
[] Build a nice enclosure.  
[] Schematics as in how I hacked the Arduino onto the control box.

## Notes
Pay attention to memory used and free. For example the JSON buffer can easily fill up all
causing the Arduino to freeze.  
This is specifically designed as to how I hacked the Arduino into the dioder control box,
so keep that in mind. Might need to adjust some things.

## Disclaimer
This project is finished as far as I care about it. Maybe I'll update it later, maybe I 
won't. If stuff breaks, you  get to keep the pieces.