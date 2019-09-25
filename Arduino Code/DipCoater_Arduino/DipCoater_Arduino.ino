// ************************************************ //
// ********** DIP COATER CONTROL UNIT ************* //
// ************************************************ //
// By M. Little (m.e.little@lboro.ac.uk)            //
// September 2019                                   //
// ************************************************ //
// Overview
// This program controls a Dip Coating machine.
// It uses a GEN4-ULCD-35DT-AR resistive touch screen from 4D Labs
// It uses a **** type stepper motor controller - 4A rated
// It has control over the down speed, the dwell time and the up speed.
// These values are controllable and saved in EEPROM

// Libraries Needed:
// AccelStepper
// https://www.airspayce.com/mikem/arduino/AccelStepper/
// Mutila
// https://github.com/matthewg42/Mutila
// 4D Systems ViSi Genie Arduino Library
// https://github.com/4dsystems/ViSi-Genie-Arduino-Library

// General configuration
#include "config.h"

// General Arduino features
#include <Arduino.h>
#include <MutilaDebug.h>
#include <Millis.h>
#include <DebouncedButton.h>
#include <DigitalInputButton.h>
#include <EEPROM.h>

// Sort out Stepper control
#include <AccelStepper.h>
AccelStepper stepper(1, STEP_PUL, STEP_DIR); // Stepper using a driver (1) with Pulses on pin D10 and Direction on pin D9. Enable is D8

// Sort out display control
#include <genieArduino.h>
Genie genie;

const uint16_t OutputPeriodMs = 150;
uint32_t LastOutputMs = 0;

DebouncedButton StartButton(START_BUTTON_PIN, true);
DebouncedButton StopButton(STOP_BUTTON_PIN, true);
DigitalInputButton EStopButton(ESTOP_BUTTON_PIN, true);
DebouncedButton UpLimit(UP_LIMIT, true);
DebouncedButton DownLimit(DOWN_LIMIT, true);

bool previousEStopButton = true;

int downSpeed;
int upSpeed;
int dwellTime;

int oldDownSpeed;
int oldUpSpeed;
int oldDwellTime;

String modeData;
int modeValue = 0;  // This stores the mode we are in
int modeMax = 4;   // Number of modes we can have

bool runModeFlag = false; // This puts us into run mode which stops value adjustment
int runMode = 0;  // This is for the running of the unit
uint32_t runModeTimer = 0;  // This holds the time take in runMode
uint32_t dwellTimer = 0;  // This holds the time take in runMode
bool resetFlag = false; // For the stop button

void setup()
{
  Serial.begin(9600);

  //Stepper setup
  pinMode(STEP_ENA, OUTPUT);
  digitalWrite(STEP_ENA, HIGH); // STEP_ENA is active LOW
  stepper.setMaxSpeed(10000);      // Adjust as required
  //End Stepper setup

  // Initialize button objects
  StartButton.begin();
  StopButton.begin();
  EStopButton.begin();
  UpLimit.begin();
  DownLimit.begin(); 
  
  // Set up display
  genie.Begin(Serial);   // Use Serial0 for talking to the Genie Library, and to the 4D Systems display
  genie.AttachEventHandler(myGenieEventHandler); // Attach the user function Event Handler for processing events
  // Reset the Display (change D4 to D2 if you have original 4D Arduino Adaptor)
  // THIS IS IMPORTANT AND CAN PREVENT OUT OF SYNC ISSUES, SLOW SPEED RESPONSE ETC
  // If NOT using a 4D Arduino Adaptor, digitalWrites must be reversed as Display Reset is Active Low, and
  // the 4D Arduino Adaptors invert this signal so must be Active High.
  pinMode(RESETLINE, OUTPUT);  // Set D4 on Arduino to Output (4D Arduino Adaptor V2 - Display Reset)
  digitalWrite(RESETLINE, 1);  // Reset the Display via D4
  delay(300);
  digitalWrite(RESETLINE, 0);  // unReset the Display via D4
  delay (3500); //let the display start up after the reset (This is important)
  // Set the brightness/Contrast of the Display - (Not needed but illustrates how)
  // Most Displays, 1 = Display ON, 0 = Display OFF. See below for exceptions and for DIABLO16 displays.
  // For uLCD-43, uLCD-220RD, uLCD-70DT, and uLCD-35DT, use 0-15 for Brightness Control, where 0 = Display OFF, though to 15 = Max Brightness ON.
  genie.WriteContrast(15);
  //Write a string to the Display to show the version of the library used
  genie.WriteStr(0, GENIE_VERSION);
  delay(1000);

  // Recover any stored values from EEPROM
  downSpeed = EEPROMReadInt(0);
  dwellTime = EEPROMReadInt(2);
  upSpeed = EEPROMReadInt(4);

  oldDownSpeed = downSpeed;
  oldUpSpeed = upSpeed;
  oldDwellTime = dwellTime;

  genie.WriteObject(GENIE_OBJ_LED_DIGITS, 0, downSpeed);
  genie.WriteObject(GENIE_OBJ_LED_DIGITS, 1, dwellTime);
  genie.WriteObject(GENIE_OBJ_LED_DIGITS, 2, upSpeed);

}

void loop()
{
  static long waitPeriod = millis();

  // Check Buttons
  StartButton.update();
  StopButton.update();
  UpLimit.update();
  DownLimit.update();  
  
  // Deal with display
  genie.DoEvents(); // This calls the library each loop to process the queued responses from the display

  // Deal with buttons as display as required
  if (millis() >= waitPeriod)
  {
    // Deal with button data
    if (StartButton.pushed() == true)
    {
      if (modeValue != 0)
      {
        genie.WriteStr(0, F("Cannot Run in Adjust Mode"));
      }
      else
      {
        genie.WriteStr(0, F("START!"));
        runModeFlag = true; // Start the unit running
        runMode = 1; // Put unit into running down mode
      }
    }
    if (StopButton.pushed() == true)
    {
      if(runModeFlag==false)
      {
        genie.WriteStr(0, F("STOP Pressed - Not Running!"));
      }
      else if(resetFlag == false)
      {
        genie.WriteStr(0, F("STOP Pressed \n - Press again to Reset"));
        runMode = 6;
        resetFlag = true; // means we need to reset the process
      }
      else
      {
        runMode = 4;
        runModeTimer = millis();
        genie.WriteStr(0, F("Moving UP \n - Resetting to top"));
        resetFlag = false; // means we need to reset the process
      }
    }

    if (previousEStopButton == true && EStopButton.on() == false)
    {
      runModeFlag==false;
      genie.WriteStr(0, F("E-STOP Actiavted"));
      runMode = 6;
      resetFlag = true; // means we need to reset the process      
      genie.WriteObject(GENIE_OBJ_USER_LED, 1, true);
      previousEStopButton = false;
    }
    else if (previousEStopButton == false && EStopButton.on() == true)
    {
      genie.WriteStr(0, "E-STOP Cleared \n - Press Stop to Reset");
      genie.WriteObject(GENIE_OBJ_USER_LED, 1, false);
      previousEStopButton = true;
    }
    else
    {
      previousEStopButton = EStopButton.on();
    }

    if (runModeFlag == true)
    {
      genie.WriteObject(GENIE_OBJ_USER_LED, 0, true); // Set running LED
    }
    else
    {
      genie.WriteObject(GENIE_OBJ_USER_LED, 0, false); // reset runnign LED
    }

    // The results of this call will be available to myGenieEventHandler() after the display has responded
    genie.ReadObject(GENIE_OBJ_USER_LED, 0); // Do a manual read from the UserLEd0 object

    waitPeriod = millis() + 50; // rerun this code in another 50ms time.
  }

  // Here we want to deal with the movement of the stepper
  // If started then do not allow any adjustment of the values (Works on flags and cannot adjust mode)

  // If running the unit will follow this pattern:
  // Move down at rate of downspeed until low sensor triggered
  // stay not moving for dwelltime
  // Move up at rate of upspeed until high sensor triggered
  switch (runMode)
  {
    case 0:
      // In this mode do nothing - waiting on start
      runModeTimer = millis();  // Keep the start time stored
      break;
    case 1:
      genie.WriteStr(0, F("Moving DOWN"));
      // Here we have a delay before moving down - adjust as required
      // Set Stepper enabled
      digitalWrite(STEP_ENA, LOW); 
        
      if (millis() >= runModeTimer + 1000)
      {
        runMode = 2;
        runModeTimer = millis();
      }      
      break;
    case 2:
      // In this mode we move down until the lower sensor is activated
      // **************** SET STEPPER - TO DO ***********************
      
      stepper.setSpeed(downSpeed*CONVERSION_DOWN); // Inverted for down!
      stepper.runSpeed();
       
      if(DownLimit.pushed() == true)
      {
        stepper.stop();
        runMode = 3;
        dwellTimer = millis();
        runModeTimer = millis();
        genie.WriteObject(GENIE_OBJ_SLIDER, 0, 0);  // Set Slider Value
      }
      break;
    case 3:
      // In this mode we dwell until dwell timer is over
      // Ensure Stepper is STOPPED ** TO DO *********
      
      if (millis() >= dwellTimer + 100)
      {
        //  This displays the dwell time as a counter
        float timer = dwellTime - ((millis() - runModeTimer) / 1000.0);
        modeData = "Dwell: " + (String)timer;
        genie.WriteStr(0, modeData);
        dwellTimer = millis();
      }

      if (millis() >= runModeTimer + (dwellTime * 1000))
      {
        runMode = 4;
        runModeTimer = millis();
        genie.WriteStr(0, F("Moving UP"));
      }
      break;
    case 4:
      // In this mode we move up until the upper sensor is activated
      // **************** SET STEPPER - TO DO ***********************
      // Set direction?
      stepper.setSpeed(upSpeed*CONVERSION_UP);
      stepper.runSpeed();
      
      if(UpLimit.pushed() == true)
      {
        stepper.stop();        
        runMode = 5;
        runModeTimer = millis();
        genie.WriteStr(0, F("At Top - Waiting"));
        genie.WriteObject(GENIE_OBJ_SLIDER, 0, 100);  // Set Slider Value
      }
      break;
    case 5:
      // Here we have a delay before stopping - adjust as required  
      if (millis() >= runModeTimer + 1000)
      {
         // Set Stepper disabled
        digitalWrite(STEP_ENA, HIGH); 
        runMode = 0;
        runModeTimer = Millis();
        runModeFlag = false;
      }
      break;
    case 6:
      // This case is used if stop or E Stop button pressed

      // Need to snesure the stepper is STOPPED here
      stepper.stop();   
             
      // Press stop again to reset back to top      
      break;
  }
}

/////////////////////////////////////////////////////////////////////
//
// This is the user's event handler. It is called by genieDoEvents()
// when the following conditions are true
//
//    The link is in an IDLE state, and
//    There is an event to handle
//
// The event can be either a REPORT_EVENT frame sent asynchronously
// from the display or a REPORT_OBJ frame sent by the display in
// response to a READ_OBJ (genie.ReadObject) request.
//

/* COMPACT VERSION HERE, LONGHAND VERSION BELOW WHICH MAY MAKE MORE SENSE
  void myGenieEventHandler(void)
  {
  genieFrame Event;
  genie.DequeueEvent(&Event);

  int slider_val = 0;

  //Filter Events from Slider0 (Index = 0) for a Reported Message from Display
  if (genie.EventIs(&Event, GENIE_REPORT_EVENT, GENIE_OBJ_SLIDER, 0))
  {
    slider_val = genie.GetEventData(&Event);  // Receive the event data from the Slider0
    genie.WriteObject(GENIE_OBJ_LED_DIGITS, 0, slider_val);     // Write Slider0 value to LED Digits 0
  }

  //Filter Events from UserLed0 (Index = 0) for a Reported Object from Display (triggered from genie.ReadObject in User Code)
  if (genie.EventIs(&Event, GENIE_REPORT_OBJ,   GENIE_OBJ_USER_LED, 0))
  {
    bool UserLed0_val = genie.GetEventData(&Event);               // Receive the event data from the UserLed0
    UserLed0_val = !UserLed0_val;                                 // Toggle the state of the User LED Variable
    genie.WriteObject(GENIE_OBJ_USER_LED, 0, UserLed0_val);       // Write UserLed0_val value back to UserLed0
  }
  } */

// LONG HAND VERSION, MAYBE MORE VISIBLE AND MORE LIKE VERSION 1 OF THE LIBRARY
void myGenieEventHandler(void)
{
  genieFrame Event;
  genie.DequeueEvent(&Event); // Remove the next queued event from the buffer, and process it below



  //If the cmd received is from a Reported Event (Events triggered from the Events tab of Workshop4 objects)
  if (Event.reportObject.cmd == GENIE_REPORT_EVENT)
  {

    if (Event.reportObject.object == GENIE_OBJ_SLIDER)                // If the Reported Message was from a Slider
    {
      //      if (Event.reportObject.index == 0)                              // If Slider0 (Index = 0)
      //      {
      //        downSpeed = genie.GetEventData(&Event);                      // Receive the event data from the Slider0
      //        genie.WriteObject(GENIE_OBJ_LED_DIGITS, 0, downSpeed);       // Write DownSpeed (slider0) value to LED Digits 0
      //      }
    }

    if (Event.reportObject.object == GENIE_OBJ_4DBUTTON)                // If the Reported Message was from a Button
    {
      if (Event.reportObject.index == 0)                              // This is button 0 - the Adjust UP button
      {
        switch (modeValue) {
          case 0:
            // This is the 'normal' mode - no adjustment made
            // This is the only mode where we can start/stop the unit
            break;
          case 1:
            downSpeed++;    // Increment the down speed
            if (downSpeed >= 250)
            {
              downSpeed = 250;
            }
            genie.WriteObject(GENIE_OBJ_LED_DIGITS, 0, downSpeed);
            break;
          case 2:
            dwellTime++;    // Increment the up speed
            if (dwellTime >= 999)
            {
              dwellTime = 0;
            }
            genie.WriteObject(GENIE_OBJ_LED_DIGITS, 1, dwellTime);
            break;
          case 3:
            upSpeed++;    // Increment the up speed
            if (upSpeed >= 250)
            {
              upSpeed = 250;
            }
            genie.WriteObject(GENIE_OBJ_LED_DIGITS, 2, upSpeed);
            break;
        }
      }

      if (Event.reportObject.index == 1)                              // This is Button1 - Adjust DOWN
      {
        switch (modeValue) {
          case 0:
            // This is the 'normal' mode - no adjustment made
            // This is the only mode where we can start/stop the unit
            break;
          case 1:
            downSpeed--;    // decrement the down speed
            if (downSpeed < 0)
            {
              downSpeed = 0;
            }
            genie.WriteObject(GENIE_OBJ_LED_DIGITS, 0, downSpeed);
            break;
          case 2:
            dwellTime--;    // decrement the up speed
            if (dwellTime < 0)
            {
              dwellTime = 999;
            }
            genie.WriteObject(GENIE_OBJ_LED_DIGITS, 1, dwellTime);
            break;
          case 3:
            upSpeed--;    // decrement the up speed
            if (upSpeed < 0)
            {
              upSpeed = 0;
            }
            genie.WriteObject(GENIE_OBJ_LED_DIGITS, 2, upSpeed);
            break;
        }
      }

      if (Event.reportObject.index == 2)                // This is button 2 - the MODE button
      {
        if (runModeFlag == true)
        {
          genie.WriteStr(0, F("Cannot Adjust in Run Mode"));
        }
        else
        {
          modeValue++;
          if (modeValue >= modeMax)
          {
            modeValue = 0;
          }

          switch (modeValue)
          {
            case 0:
              genie.WriteObject(GENIE_OBJ_USER_LED, 2, false);
              genie.WriteObject(GENIE_OBJ_USER_LED, 3, false);
              genie.WriteObject(GENIE_OBJ_USER_LED, 4, false);
              break;
            case 1:
              genie.WriteObject(GENIE_OBJ_USER_LED, 2, true);
              genie.WriteObject(GENIE_OBJ_USER_LED, 3, false);
              genie.WriteObject(GENIE_OBJ_USER_LED, 4, false);
              break;

            case 2:
              genie.WriteObject(GENIE_OBJ_USER_LED, 2, false);
              genie.WriteObject(GENIE_OBJ_USER_LED, 3, true);
              genie.WriteObject(GENIE_OBJ_USER_LED, 4, false);
              break;

            case 3:
              genie.WriteObject(GENIE_OBJ_USER_LED, 2, false);
              genie.WriteObject(GENIE_OBJ_USER_LED, 3, false);
              genie.WriteObject(GENIE_OBJ_USER_LED, 4, true);
              break;
          }

          // Here we change the data depending upon the mode
          switch (modeValue)
          {
            case 0:
              String EEPROMwritten = F("EEROM Written:\n");
              bool EEPROMchange = false;
              //If the values have changed then save them into EEPROM.
              //If values unchanged then do not
              if (downSpeed != oldDownSpeed)
              {
                EEPROMWriteInt(0, downSpeed);
                EEPROMwritten = EEPROMwritten + F("Down Speed ");
                EEPROMchange = true;
              }
              if (upSpeed != oldUpSpeed)
              {
                EEPROMWriteInt(4, upSpeed);
                EEPROMwritten = EEPROMwritten + F("Up Speed ");
                EEPROMchange = true;
              }
              if (dwellTime != oldDwellTime)
              {
                EEPROMWriteInt(2, dwellTime);
                EEPROMwritten = EEPROMwritten + F("Dwell Time");
                EEPROMchange = true;
              }
              if (EEPROMchange == false)
              {
                EEPROMwritten = F("No EEPROM Change");
              }
              genie.WriteStr(0, EEPROMwritten);
              oldDownSpeed = downSpeed;
              oldUpSpeed = upSpeed;
              oldDwellTime = dwellTime;
              break;

            case 1:
              break;

            case 2:
              break;

            case 3:
              break;
          }
        }
      }
    }
  }

  //  //If the cmd received is from a Reported Object, which occurs if a Read Object (genie.ReadOject) is requested in the main code, reply processed here.
  //  if (Event.reportObject.cmd == GENIE_REPORT_OBJ)
  //  {
  //    if (Event.reportObject.object == GENIE_OBJ_USER_LED)              // If the Reported Message was from a User LED
  //    {
  //      if (Event.reportObject.index == 0)                              // If UserLed0 (Index = 0)
  //      {
  //        bool UserLed0_val = genie.GetEventData(&Event);               // Receive the event data from the UserLed0
  //        UserLed0_val = !UserLed0_val;                                 // Toggle the state of the User LED Variable
  //        genie.WriteObject(GENIE_OBJ_USER_LED, 0, UserLed0_val);       // Write UserLed0_val value back to UserLed0
  //      }
  //    }
  //  }

  /********** This can be expanded as more objects are added that need to be captured *************
  *************************************************************************************************
    Event.reportObject.cmd is used to determine the command of that event, such as an reported event
    Event.reportObject.object is used to determine the object type, such as a Slider
    Event.reportObject.index is used to determine the index of the object, such as Slider0
    genie.GetEventData(&Event) us used to save the data from the Event, into a variable.
  *************************************************************************************************/
}

void EEPROMWriteInt(int address, int value)
{
  byte two = (value & 0xFF);
  byte one = ((value >> 8) & 0xFF);

  EEPROM.update(address, two);
  EEPROM.update(address + 1, one);
}

int EEPROMReadInt(int address)
{
  long two = EEPROM.read(address);
  long one = EEPROM.read(address + 1);

  return ((two << 0) & 0xFFFFFF) + ((one << 8) & 0xFFFFFFFF);
}