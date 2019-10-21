#pragma once

#include <Arduino.h>

// General config
#define RESETLINE 4  // For the 4D Display Unit

// Stepper config
#define STEP_DIR          9     // Direction pin
#define STEP_PUL          8     // Pulses pin
#define STEP_ENA          10    // Enable pin

// Distance Conversion Parameters
// These do not need to be changed, unles design changed
#define STEP_DEGREES        1.8   // Stepper motor parameter
#define MICRO_STEPS         4.0     // Micro stepping from controller
#define GEARBOX_RATIO       100.0   // 100:1 gearbox ratio
#define TEETH_STEPPER       40.0    // Number of teeth on the stepper end drive wheel
#define TEETH_DRIVE         40.0    // Number of teeth of the belt end drive wheel
#define TEETH_BELT          20.0    // Number of teeth on the main belt drive toothed wheel
#define BELT_MM_TOOTH       8.0     // 8 mm per tooth movement
#define MOVEMENT_DISTANCE   3000.0  // Max mm movement of unit
#define MAX_STEPPER_SPEED   5000    // Max pulses/sec
#define MAX_MM_M            450     // Max speed up or down


// Button feel settings
#define DEBOUNCED_BUTTON_THRESHOLD      5
#define DEBOUNCED_BUTTON_DELAY          5
#define DEBOUNCED_BUTTON_HELD_MS        800
#define DEBOUNCED_BUTTON_RPT_INITIAL_MS 1000
#define DEBOUNCED_BUTTON_RPT_MS         300

// Button parameters
#define START_BUTTON_PIN                5
#define STOP_BUTTON_PIN                 6
#define ESTOP_BUTTON_PIN                7
#define UP_LIMIT                        2
#define DOWN_LIMIT                      3

#define DISPLAYUPDATEMS                 100 // mS between each display and button press check update
