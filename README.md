# DipCoater

A dip coater is a machine which has a controllable up and down movement rate which can be used for adding different thickness coatings to plate and tube materials. It also has a controllable dwell time, which is the length of time that the item stays fully dipped.
The specifications are: 
* 0 - 450 mm/s up and down speed maximum
* 0 - 999 seconds dwell time

This repository holds:
* Arduino based control program for a dip coating machine used at CREST, Loughborough University.
* Wiring diagram (svg and pdf)

## Operation 



## Wiring Overview
It uses a GEN4-ULCD-35DT-AR resistive touch screen from 4D Labs
It uses a stepper motor controller - 4A rated
It has control over the down speed, the dwell time and the up speed.
These values are controllable and saved in EEPROM

Additional libraries Needed:
* AccelStepper
* https://www.airspayce.com/mikem/arduino/AccelStepper/
* Mutila
* https://github.com/matthewg42/Mutila
* 4D Systems ViSi Genie Arduino Library
* https://github.com/4dsystems/ViSi-Genie-Arduino-Library
