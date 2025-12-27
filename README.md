# ESP!Now Sensor Framework

## Framework for ESP!Now Sensors and Remotes


## Configuration options
Configuration                       |Default (Example)                      |Description                                                                                    |               |
------------------------------------|---------------------------------------|-----------------------------------------------------------------------------------------------|---------------|
**Various Product information**     |                                       |                                                                                               |               |
PRODUCT                             |"ESP!NOW Sensor"                       |                                                                                               |Coded          |
VERSION                             |0x000000                               |                                                                                               |Coded          |
PRODUCT_ID                          |"SENSOR"                               |                                                                                               |Coded          |
DESCRIPTION                         |"Generic ESP!Now Sensor"               |                                                                                               |Coded          |
PRODUCT_KEY                         |0x00                                   |                                                                                               |Coded          |
OWNER                               |"gea"                                  |                                                                                               |Coded          |
**Networking**                      |                                       |                                                                                               |               |
WIFI_SSID                           |"mySSID"                               |Wifi SSID for configuration mode                                                               |Platformio     |
WIFI_PASSWORD                       |"mySecretWifiPassword"                 |Wifi password for WIFI_SSID                                                                    |Platformio     |
CUSTOM_MAC_ADDRESS                  |("00:00:00:00:00:00")                  |use this MAC address instead of ESP MAC address (WizMote compatibility)                        |Coded          |
**Serial debug settings**           |                                       |                                                                                               |               |
SERIAL_DEBUG_TX_PIN                 |MCU spezific                           |Serial debug TX pin                                                                            |Coded          |
SERIAL_DEBUG_RX_PIN                 |MCU spezific                           |Serial debug RX pin                                                                            |Coded          |
SERIAL_DEBUG_PORT                   |MCU spezific                           |Serial debug port<br>0 = Hardware Serial<br> 1 = Software Serial                               |Coded          |
BAUD_RATE_DEBUG                     |115200                                 |Serial debug baud                                                                              |Coded          |
**ESP!Now Authentification  settings**  |                                   |                                                                                               |               |               
AUTHENTIFICATION_KEY                |"1234567890ABCDEF"                     |ESP!Now Authentification Key                                                                   |Coded          |
AUTH_TOKEN_REQ                      |0                                      |Authentification Token needed                                                                  |Website<br>settings.useAuthToken|
AUTH_TOKEN_REQUEST_TIMEOUT          |1000                                   |Timeout no Authentification token received, try next eabled channel                            |Coded          |
AUTH_TOKEN_REQUEST_RETRY            |9                                      |no of retries if no Authentification token received                                            |Coded          |
**ESP!Now settings**                |                                       |                                                                                               |               |  
ESPNOW_CHANNEL                      |1057 (Channel 1, 6, 11)                 |ESP!Now enabled channel bitmask                                                               |Website<br>settings.channel|
ESPNOW_REPEAT_SEND                  |5                                      |ESP!Now telegrams sent on each enabled channel                                                 |Website<br>settings.broadcastRepeat|
ESPNOW_TELEGRAM_WIZMOTE             |(#define)                              |WizMote compatible ESP!Now telegrams used                                                      |Coded          |
ESPNOW_TELEGRAM_EXTENDED            |(#define) ->default                    |Extended ESP!Now telegrams used (WizMote + 4 uint32_t values)                                  |Coded          |
ESPNOW_ALIVE                        |(#define)                              |Send ESP!Now alive telegram (program=AF)                                                       |Coded          |
ESPNOW_SEND_DATA_COMPLETE           |(#define)                              |Send ESP!Now data telegram, if all dpid added (program=DO)                                     |Coded          |
**Data point identifier**           |                                       |inspired by TUYA telegrams (0=not used)                                                        |               |
DPID_STATE                          |0                                      |Data point identifier for state value                                                          |Coded          |
DPID_BATTERY                        |0                                      |Data point identifier for battery value                                                        |Coded          |
DPID_VALUE1                         |0                                      |Data point identifier for process value 1                                                      |Coded          |
DPID_VALUE2                         |0                                      |Data point identifier for process value 2                                                      |Coded          |
DPID_VALUE3                         |0                                      |Data point identifier for process value 3                                                      |Coded          |
DPID_VALUE4                         |0                                      |Data point identifier for process value 4                                                      |Coded          |
DPID_STATE_POLARITY                 |(value)                                |undefined: State = data<br>defined: State = data==DPID_STATE_POLARITY                          |Coded          |
**Output pin**                      |                                       |                                                                                               |               |
ACTIVE_PIN                          |(pin)                                  |Output pin active LED<br>ON          =   Normal mode<br>BLINKSLOW   =   Config mode / Connecting to wifi<br>BLINKFAST   =   Config mode / Connected to wifi, Ready for Config<br>FLICKER     =   Download in progress|Coded          |
ACTIVE_PIN_POLARITY                 |LOW                                    |Polarity of active LED                                                                         |Coded          |
VOLTAGE_REGULATOR_PIN               |(pin)                                  |Output pin voltage regulator on<br>Set at startup<br>Reset at poweroff                         |Coded          |
VOLTAGE_REGULATOR_POLARITY          |HIGH                                   |Polarity of voltage regulator on                                                               |Coded          |
**Input pin**                       |                                       |                                                                                               |               |
SETUP_PIN                           |(pin)                                  |Input pin for enter configuration mode                                                         |Coded          |
SETUP_PIN_POLARITY                  |LOW                                    |Polarity of pin for enter configuration mode                                                   |Coded          |
SHUTDOWN_PIN                        |(pin)                                  |Input pin for shutdown                                                                         |Coded          |
SHUTDOWN_PIN_POLARITY               |LOW                                    |Polarity of pin for shutdown                                                                   |Coded          |
**Time out**                        |                                       |                                                                                               |               |
CONFIG_MODE_TIMEOUT                 |120                                    |Leave config mode after this time                                                              |Coded          |                                    
SHUTDOWN_TIMER                      |30                                     |Shutdown after this time  if not in configuration mode                                         |Coded          |
**Power off**                       |                                       |recommended to use only one power off option                                                   |               |
POWER_OFF_EXTERNAL                  |(#define)                              |power off by external (only waiting for power off)                                             |               |
POWER_OFF_VOLTAGE_REGULATOR         |(#define)                              |power off by VOLTAGE_REGULATOR_PIN (need to be defined)                                        |               |
POWER_OFF_DEEPSLEEP                 |#define                                |power off by deep sleep                                                                        |               |   
**Deep sleep**                      |                                       |POWER_OFF_DEEPSLEEP is defined                                                                 |               |
DEEPSLEEP_TIME                      |0                                      |Time in sec. for deepsleep (0=sleep unitl reset)                                               |Website<br>settings.deepsleepTime|         
DEEPSLEEP_WAKEUP_GPIO_PIN1          |(1..5)                                 |1. pin for wakeup from deepsleep (only ESP32C2 or ESP32C3)                                     |Coded          |
DEEPSLEEP_WAKEUP_GPIO_PIN2          |(1..5)                                 |2. pin for wakeup from deepsleep (only ESP32C2 or ESP32C3)                                     |Coded          |          
DEEPSLEEP_WAKEUP_GPIO_PIN3          |(1..5)                                 |3. pin for wakeup from deepsleep (only ESP32C2 or ESP32C3)                                     |Coded          |
DEEPSLEEP_WAKEUP_GPIO_PIN4          |(1..5)                                 |4. pin for wakeup from deepsleep (only ESP32C2 or ESP32C3)                                     |Coded          |
DEEPSLEEP_WAKEUP_GPIO_PIN_POLARITY  |LOW                                    |Polarity of pin for wakeup from deepsleep                                                      |Coded          |
**Settings for product configuration**|                                     |x=0..5 (free to use in product for configurations)                                             |               |  
SETTINGS_CONFIGx                    (#define)                               |use configuration x                                                                            |Website<br>settings.Config[x]|
SETTINGS_CONFIGx_NAME               |"Settings x"                           |Name for configuration x (Website)                                                             |Coded          |
SETTINGS_CONFIGx_INIT               |0                                      |Initial value for configuration x                                                              |Coded          |
**Settings for product value storage**|                                     |x=0..3 (free to use in product for value storage)                                              |               | 
VALUE_x                             |(#define)                              |use value x                                                                                    |User application|
VALUE_x_NAME                        |"Value x"                              |Name for value x (Website)                                                                     |Coded          |
VALUE_x_INIT                        |0                                      |Initial value for value x                                                                      |Coded          |
**Battery level measurement**       |                                       |                                                                                               |               |
BATTERY_2AAA                        |(#define)                              |battery type: 2xAAA 1.5V Alkaline batteries                                                    |Coded          |
BATTERY_LIPO                        |(#define)                              |battery type: 3.7V LiPo battery                                                                |Coded          |
BATTERY_LEVEL                       |(BATTERY_2AAA_LEVEL<br>BATTERY_LIPO_LEVEL|Array of Battery voltage vs. level<br>default selected by battery type                       |Coded          |
BATTERY_VOLTAGE_DIVIDER             |2.5 (ESP32)<br>3.5 (ESP8266)           |use value x                                                                                    |Coded          |      
BATTERY_ADC_VREF                    |3.300 (ESP32)<br>1.000 (ESP8266)       |use value x                                                                                    |Coded          |
BATTERY_ADC_DIGIT_RANGE             |4095.0 (ESP32)<br>1024.0 (ESP8266)     |use value x                                                                                    |Coded          |
BATTERY_VOLTAGE_ON_PIN              |(pin)                                  |Output pin to activate adc voltage divider                                                     |Coded          |
BATTERY_VOLTAGE_ON_POLARITY         |HIGH                                   |Polarity of activate adc voltage divider                                                       |Coded          |