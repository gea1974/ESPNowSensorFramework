# ESP!Now Sensor Framework

## Framework for ESP!Now Sensors and Remotes



Configuration                       |Default (Example)                      |Description                                                                                    |               |
------------------------------------|---------------------------------------|-----------------------------------------------------------------------------------------------|---------------|
                                    |                                       |Various Product / project information                                                          |               |
PRODUCT                             |"ESP!NOW Sensor"                       |                                                                                               |Coded          |
VERSION                             |0x000000                               |                                                                                               |Coded          |
PRODUCT_ID                          |"SENSOR"                               |                                                                                               |Coded          |
DESCRIPTION                         |"Generic ESP!Now Sensor"               |                                                                                               |Coded          |
PRODUCT_KEY                         |0x00                                   |                                                                                               |Coded          |
OWNER                               |"gea"                                  |                                                                                               |Coded          |
------------------------------------|---------------------------------------|-----------------------------------------------------------------------------------------------|---------------|
CUSTOM_MAC_ADDRESS                  |("00:00:00:00:00:00")                  |use this MAC address instead of ESP MAC address (WizMote compatibility)                        |Coded          |
------------------------------------|---------------------------------------|-----------------------------------------------------------------------------------------------|---------------|
Serial debug settings               |                                       |                                                                                               |               |
SERIAL_DEBUG_TX_PIN                 |MCU spezific                           |Serial debug TX pin                                                                            |Coded          |
SERIAL_DEBUG_RX_PIN                 |MCU spezific                           |Serial debug RX pin                                                                            |Coded          |
SERIAL_DEBUG_PORT                   |MCU spezific                           |Serial debug port 0=Hardware Serial 1=Software Serial                                          |Coded          |
BAUD_RATE_DEBUG                     |115200                                 |Serial debug baud                                                                              |Coded          |
------------------------------------|---------------------------------------|-----------------------------------------------------------------------------------------------|---------------|
ESP!Now Authentification  settings  |                                       |                                                                                               |               |               
AUTHENTIFICATION_KEY                |"1234567890ABCDEF"                     |ESP!Now Authentification Key                                                                   |Coded          |
AUTH_TOKEN_REQ                      |0                                      |Authentification Token needed                                                                  |Website        |
AUTH_TOKEN_REQUEST_TIMEOUT          |1000                                   |Timeout no Authentification token received, try next eabled channel                            |Coded          |
AUTH_TOKEN_REQUEST_RETRY            |9                                      |no of retries if no Authentification token received                                            |Coded          |




                                                                                    ESP!Now settings
ESPNOW_CHANNEL                      ((1<<1 | 1<<6 | 1<<11) >> 1)                    ESP!Nowenabled channel bitmask (Default 1, 6, 11)                                           Website
ESPNOW_REPEAT_SEND                  5                                               ESP!Now telegrams sent on each enabled channel                                              Website
ESPNOW_TELEGRAM_WIZMOTE             (#define)                                       WizMote compatible ESP!Now telegrams used                                                   Coded
ESPNOW_TELEGRAM_EXTENDED            (#define) ->default                             Extended ESP!Now telegrams used (WizMote + 4 uint32_t values)                               Coded
ESPNOW_ALIVE                        (#define)                                       Send ESP!Now alive telegram (program=AF)                                                    Coded
ESPNOW_SEND_DATA_COMPLETE           (#define)                                       Send ESP!Now data telegram, if all dpid added (program=DO)                                  Coded

                                                                                    Data point identifier inspired by TUYA telegrams (0=not used)
DPID_STATE                          0                                               Data point identifier for state value                                                       Coded
DPID_BATTERY                        0                                               Data point identifier for battery value                                                     Coded
DPID_VALUE1                         0                                               Data point identifier for process value 1                                                   Coded
DPID_VALUE2                         0                                               Data point identifier for process value 2                                                   Coded
DPID_VALUE3                         0                                               Data point identifier for process value 3                                                   Coded
DPID_VALUE4                         0                                               Data point identifier for process value 4                                                   Coded

DPID_STATE_POLARITY                 (value)                                         undefined:  State = data                                                                    Coded
                                                                                    defined:    State = data==DPID_STATE_POLARITY 

CONFIG_MODE_TIMEOUT                 120                                             Leave config mode after this time                                                           Coded                                    
SHUTDOWN_TIMER                      30                                              Shutdown after this time  if not in configuration mode                                      Coded

ACTIVE_PIN                          (pin)                                           Output pin active LED:                                                                      Coded
                                                                                        ON          =   Normal mode                                 
                                                                                        BLINKSLOW   =   Config mode / Connecting to wifi
                                                                                        BLINKFAST   =   Config mode / Connected to wifi, Ready for Config
                                                                                        FLICKER     =   Download in progress
ACTIVE_PIN_POLARITY                 LOW                                             Polarity of active LED                                                                      Coded

VOLTAGE_REGULATOR_PIN               (pin)                                           Output pin hold voltage regulator on                                                        Coded
                                                                                        Set at startup
                                                                                        Reset at poweroff
VOLTAGE_REGULATOR_POLARITY          HIGH                                            Polarity of voltage regulator on                                                            Coded

SETUP_PIN                           (pin)                                           Input pin for enter configuration mode                                                      Coded
SETUP_PIN_POLARITY                  LOW                                             Polarity of pin for enter configuration mode                                                Coded

SHUTDOWN_PIN                        (pin)                                           Input pin for shutdown                                                                      Coded
SHUTDOWN_PIN_POLARITY               LOW                                             Polarity of pin for shutdown                                                                Coded

POWER_OFF_DEEPSLEEP                 (#define)                                                 
DEEPSLEEP_WAKEUP_GPIO_PIN1          (1..5)                                          1. pin for wakeup from deepsleep (only ESP32C2 or ESP32C3)                                  Coded
DEEPSLEEP_WAKEUP_GPIO_PIN2          (1..5)                                          2. pin for wakeup from deepsleep (only ESP32C2 or ESP32C3)                                  Coded          
DEEPSLEEP_WAKEUP_GPIO_PIN3          (1..5)                                          3. pin for wakeup from deepsleep (only ESP32C2 or ESP32C3)                                  Coded
DEEPSLEEP_WAKEUP_GPIO_PIN4          (1..5)                                          4. pin for wakeup from deepsleep (only ESP32C2 or ESP32C3)                                  Coded
DEEPSLEEP_WAKEUP_GPIO_PIN_POLARITY  LOW                                             Polarity of pin for wakeup from deepsleep                                                   Coded
DEEPSLEEP_TIME                      0                                               Time for deepsleep (0=sleep unitl reset)                                                    Website (settings.deepsleepTime)

                                                                                    Settings for product configuration x=0..5 (free to use in product for configurations)
SETTINGS_CONFIGx                    (#define)                                       use configuration x                                                                         Website (settings.Config[x])
SETTINGS_CONFIGx_NAME               "Settings x"                                    Name for configuration x (Website)                                                          Coded
SETTINGS_CONFIGx_INIT               0                                               Initial value for configuration x                                                           Coded

                                                                                    Settings for product value x=0..3 (free to use in product for value storage)
VALUE_x                             (#define)                                       use value x                                                                                 User application
VALUE_x_NAME                        "Value x"                                       Name for value x (Website)                                                                  Coded
VALUE_x_INIT                        0                                               Initial value for value x                                                                   Coded