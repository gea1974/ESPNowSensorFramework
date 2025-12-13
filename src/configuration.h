/*
#define PRODUCT                             "ESP!NOW Sensor"
#define VERSION                             0x000000
#define PRODUCT_ID                          "SENSOR"
#define DESCRIPTION                         "Generic ESP!Now Sensor"
#define PRODUCT_KEY                         0x00
#define OWNER                               "gea"
*/

/*
**Outputs
    #define ACTIVE_PIN                      2
    #define VOLTAGE_REGULATOR_PIN           15
**Inputs
#define SETUP_PIN                           0
#define SHUTDOWN_PIN                        16
#define DEEPSLEEP_INTERUPT_PIN              4
#define DEEPSLEEP_INTERUPT_PIN_POLARITY      LOW // ESP_GPIO_WAKEUP_GPIO_HIGH
*/

/*
#define POWER_OFF_DEEPSLEEP
#define SHUTDOWN_TIMER                      60000           //Shutdown ESP after ms
#define ESPNOW_ALIVE                                        //Send alive message to ESP!Now on power on (if no Authentification token is required)
#define ESPNOW_SEND_DATA_COMPLETE                           //Send data message to ESP!Now if all dpid recweived
*/

/*
#define SETTINGS_CONFIG0
#define SETTINGS_CONFIG0_NAME               "Settings 0"
#define SETTINGS_CONFIG1
#define SETTINGS_CONFIG1_NAME               "Settings 1"
#define SETTINGS_CONFIG2
#define SETTINGS_CONFIG2_NAME               "Settings 2"
#define SETTINGS_CONFIG3
#define SETTINGS_CONFIG3_NAME               "Settings 3"
*/
#define SETTINGS_CONFIG4
#define SETTINGS_CONFIG4_NAME               "Settings 4"
#define SETTINGS_CONFIG5
#define SETTINGS_CONFIG5_NAME               "Settings 5"

#define VALUE_0
#define VALUE_0_NAME                        "Value 0"
#define VALUE_1
#define VALUE_1_NAME                        "Value 1"
#define VALUE_2
#define VALUE_2_NAME                        "Value 2"
#define VALUE_3
#define VALUE_3_NAME                        "Value 3"


// Enter Project configuration above this line
#include <lib/configuration_defaults.h>