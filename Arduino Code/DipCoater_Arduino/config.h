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
#define MICRO_STEPS         8.0     // Micro stepping from controller
#define GEARBOX_RATIO       100.0   // 100:1 gearbox ratio
#define TEETH_STEPPER       27.0    // Number of teeth on the stepper end drive wheel
#define TEETH_DRIVE         48.0    // Number of teeth of the belt end drive wheel
#define TEETH_BELT          45.0    // Number of teeth on the main belt drive toother wheel
#define BELT_MM_TOOTH       5.0     // 5 mm per tooth movement
#define MOVEMENT_DISTANCE   1000.0  // mm movement of unit
#define MAX_STEPPER_SPEED   500000  // Max pulses/sec

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
