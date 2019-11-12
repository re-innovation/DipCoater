# DipCoater
A dip coater is a machine which has a controllable up and down movement rate which can be used for adding different thickness coatings to plate and tube materials. It also has a controllable dwell time, which is the length of time that the item stays fully dipped.
The specifications are: 
* 0 - 450 mm/s up and down speed maximum
* 0 - 999 seconds dwell time

This repository holds:
* Arduino based control program for a dip coating machine used at CREST, Loughborough University.
* Wiring diagram (svg and pdf)

## Operation 
There are two buttons, a touch screen display and an emergency stop.

There is an on/off switch next to the IEC socket under the unit.

Check that the E-stop is not triggered.

Switch on and wait at least 5 seconds for the unit to start up. The screen should start up and the backlight will be on.

There are four values on this screen:
* Up Speed
* Down Speed
* Dwell Time
* Movement distance (this is approximate within 2-5mm)

### Normal Operation
Pressing the green button will start the unit.

This will move down from its position (the top position is the normal reset position).

It will move at a rate controlled by "Down Speed".

It will stop either when it hits the lower limit switch (a magnetic switch) OR if the unit has moved the movement distance.

It will then wait for the dwell time.

It will then rise until it hits the upper limit switch.

### Stop and E-Stop
Pressing the red button will stop the unit. Pressing the red button again will then reset the unit back up to the upper limit switch.

Pressing the E-Stop will stop and cut power to the stepper motor. Reset the E-Stop and then press the red button to reset.

### Adjust Values
To adjust the various values the touch screen is used.

Press the purple circle/button to enter adjust mode. The value to be adjusted will have a green light next to it.

Use the two orange circles/buttons to adjust the value up or down.

Press the purple circle/button and the unit will cycle through the four parameters.

At the end of the four parameters, if any values have been changed then these will be stored in EEPROM and so will be the same after power off.

You cannot run the machine when in adjust mode. Likewise you cannot adjust the values when in run mode.

## Wiring Overview
A wiring diagram is included in this repository for reference.

It uses a GEN4-ULCD-35DT-AR resistive touch screen from 4D Labs.

It uses a stepper motor controller - 4A rated.

It has control over the down speed, the dwell time and the up speed.

These values are controllable and saved in EEPROM

Additional libraries Needed:
* AccelStepper
* https://www.airspayce.com/mikem/arduino/AccelStepper/
* Mutila
* https://github.com/matthewg42/Mutila
* 4D Systems ViSi Genie Arduino Library
* https://github.com/4dsystems/ViSi-Genie-Arduino-Library
