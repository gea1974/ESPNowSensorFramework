#define PRODUCT                             "ESP!NOW Sensor"
#define VERSION                             0x000000
#define BUILD                               LAST_BUILD_TIME
#define BUILDTIME                           __DATE__ " "  __TIME__
#define OWNER                               "gea"

/*
#define ACTIVE_PIN                          2
#define VOLTAGE_REGULATOR_PIN               15
#define SETUP_PIN                           0
#define SHUTDOWN_PIN                        16
*/

#define POWER_OFF_DEEPSLEEP
#define SHUTDOWN_TIMER                      60000           //Shutdown ESP after ms


#define SETUP_PIN                           14
#define SHUTDOWN_PIN                        12
#define ACTIVE_PIN                          2

#define SERIAL_DEBUG_PORT           0                       //0=Hardware Serial 1=Software Serial
#define BAUD_RATE_DEBUG 115200

/*
#define IO_BUTTON_POLARITY          LOW
#define IO_BUTTON_MODE              INPUT_PULLUP    
#define IO_BUTTON_ON_PIN            14
#define IO_BUTTON_OFF_PIN           12
#define IO_BUTTON_1_PIN             13
#define IO_BUTTON_2_PIN             4
#define ACTIVE_PIN                  2
#define VOLTAGE_REGULATOR_PIN       15
*/
#include <config/configuration_defaults.h>