# Mimir
_Mímir (Old Norse "The rememberer, the wise one") or Mim is a figure in Norse mythology renowned for his knowledge and wisdom who is beheaded during the Æsir-Vanir War. Afterward, the god Odin carries around Mímir's head and it recites secret knowledge and counsel to him._

Mimir is a server-client application that enables microcontrollers to advise on the status of a network. It exists out of Mimir, a program that is capable of taking multiple inputs, and Muninn, microcontroller program that recieves messages from Mimir.

It arised out of the need of having a physical indication of where something goes wrong in a network that has devices spread throughout a location.


## TO DO
### Software

- [ ] Write a good README file explaining how to actually use this
- [ ] Implement querying to  the Prometheus API
- [ ] Replace Arduino with a more general term
- [ ] Research more things to implement

### Hardware

- [ ] Schematics as in how I hacked the Arduino onto the control box.
- [ ] Research good physical triggers for network status (light, sound, ...).
- [ ] Build a nice enclosure.

## Notes
Pay attention to memory used and free. For example the JSON buffer can easily fill up all
causing the Arduino to freeze.  
This is specifically designed as to how I hacked the Arduino into the dioder control box,
so keep that in mind. Might need to adjust some things.

## Disclaimer
This project is finished as far as I care about it. Maybe I'll update it later, maybe I 
won't. If stuff breaks, you  get to keep the pieces.