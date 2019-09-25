#pragma once

#include <Arduino.h>

// General config
#define RESETLINE 4  // For the 4D Display Unit

// Stepper config
#define STEP_DIR          9   // Direction pin
#define STEP_PUL          8  // Pulses pin
#define STEP_ENA          10   // Enable pin
#define CONVERSION_UP     10 // This is a conversion factor form converting mm/s into speed pulses
#define CONVERSION_DOWN   -10 // This is a conversion factor form converting mm/s into speed pulses

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
