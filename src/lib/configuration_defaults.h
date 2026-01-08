#ifndef PRODUCT
    #define PRODUCT                             "ESP!NOW Sensor"
    #define VERSION                             0x000000
    #define PRODUCT_ID                          "SENSOR"
    #define DESCRIPTION                         "Generic ESP!Now Sensor"
    #define PRODUCT_KEY                         0x00
    #define OWNER                               "gea"
#endif
#ifndef BUILD
    #define BUILD                               LAST_BUILD_TIME
#endif
#ifndef BUILDTIME
    #define BUILDTIME                           __DATE__ " "  __TIME__
#endif

#ifndef WIFI_SSID
    #define WIFI_SSID                               "mySSID"
#endif
#ifndef WIFI_PASSWORD
    #define WIFI_PASSWORD                           "mySecretWifiPassword"
#endif

#ifndef AUTHENTIFICATION_KEY
    #define AUTHENTIFICATION_KEY   "1234567890ABCDEF"
#endif
#define AUTH_KEY_LENGTH 16

#ifdef ESP12
    #define MCU_TYPE                    "ESP12 (ESP8266)"
#endif
#ifdef TYWE3S
    #define MCU_TYPE                    "TYWES3 (ESP8266)"
#endif
#ifdef ESP_WROOM_02
    #define MCU_TYPE                    "ESP-WROOM-02 (ESP8266)"
#endif
#ifdef ESP01F_CBU
    #define MCU_TYPE                    "ESP01F CBU-Replacement (ESP8285)"
#endif
#ifdef ESP32C3WROOM06
    #define MCU_TYPE                    "ESP32-C3 WROOM06"
#endif
#ifdef ESP32C2WROOM06
    #define MCU_TYPE                    "ESP32-C2 WROOM06"
#endif
#ifdef ESP32C2WROOM01C
    #define MCU_TYPE                    "ESP32-C2 WROOM01C"
#endif

#ifndef MCU_TYPE
    #ifdef ESP8266
        #define MCU_TYPE                "ESP8266"        
    #endif
    #if (defined ESP32 && defined ESP32C3)
        #define MCU_TYPE                "ESP32-C3"       
    #endif
    #if (defined ESP32 && defined ESP32C2)
        #define MCU_TYPE                "ESP32-C2"       
    #endif
    #ifndef MCU_TYPE
        #if (defined ESP32)
            #define MCU_TYPE            "ESP32"       
        #endif
    #endif
    #ifndef MCU_TYPE
        #define MCU_TYPE                "unspezified"        
    #endif
#endif

#ifndef ESPNOW_CHANNEL
    #define ESPNOW_CHANNEL              ((1<<1 | 1<<6 | 1<<11) >> 1)
#endif
 
#ifndef SERIAL_DEBUG_PORT
        #if  (defined TYWE3S || defined ESP12 || defined ESP_WROOM_02)
            #ifdef SERIAL_DEBUG_SW
                #define SERIAL_DEBUG_TX_PIN         12
                #define SERIAL_DEBUG_RX_PIN         13
                #define SERIAL_DEBUG_PORT           1                   //0=Hardware Serial 1=Software Serial
                #define BAUD_RATE_DEBUG 115200
            #else
                #define SERIAL_DEBUG_PORT           0                   //0=Hardware Serial 1=Software Serial
                #define BAUD_RATE_DEBUG 115200
            #endif
        #endif
        #ifdef ESP01F_CBU
            #ifdef SERIAL_DEBUG_SW
                #define SERIAL_DEBUG_TX_PIN         5
                #define SERIAL_DEBUG_RX_PIN         4
                #define SERIAL_DEBUG_PORT           1                   //0=Hardware Serial 1=Software Serial
                #define BAUD_RATE_DEBUG             115200
            #else
                #define SERIAL_DEBUG_PORT           0                   //0=Hardware Serial 1=Software Serial
                #define BAUD_RATE_DEBUG             115200
            #endif
        #endif
        #ifdef ESP32C3WROOM06
            #define SERIAL_DEBUG_TX_PIN         21
            #define SERIAL_DEBUG_RX_PIN         20
            #define SERIAL_DEBUG_PORT           0 
            #define BAUD_RATE_DEBUG             115200
        #endif
        #ifdef ESP32C2WROOM06
            #define SERIAL_DEBUG_TX_PIN         20
            #define SERIAL_DEBUG_RX_PIN         19
            #define SERIAL_DEBUG_PORT           0 
            #define BAUD_RATE_DEBUG             115200
        #endif
        #ifdef ESP32C2WROOM01C
            #define SERIAL_DEBUG_TX_PIN         20
            #define SERIAL_DEBUG_RX_PIN         19
            #define SERIAL_DEBUG_PORT           0 
            #define BAUD_RATE_DEBUG             115200
        #endif
#endif

#if (!defined POWER_OFF_EXTERNAL && !defined POWER_OFF_VOLTAGE_REGULATOR && !defined POWER_OFF_DEEPSLEEP)
    #define POWER_OFF_DEEPSLEEP
#endif

#ifndef DPID_STATE
    #define DPID_STATE                  0
#endif
#ifndef DPID_BATTERY
    #define DPID_BATTERY                0
#endif
#ifndef DPID_VALUE1
    #define DPID_VALUE1                 0
#endif
#ifndef DPID_VALUE2
    #define DPID_VALUE2                 0
#endif
#ifndef DPID_VALUE3
    #define DPID_VALUE3                 0
#endif
#ifndef DPID_VALUE4
    #define DPID_VALUE4                 0
#endif

#ifndef SETTINGS_CONFIG0_INIT
    #define SETTINGS_CONFIG0_INIT                 0
#endif
#ifndef SETTINGS_CONFIG1_INIT
    #define SETTINGS_CONFIG1_INIT                 0
#endif
#ifndef SETTINGS_CONFIG2_INIT
    #define SETTINGS_CONFIG2_INIT                 0
#endif
#ifndef SETTINGS_CONFIG3_INIT
    #define SETTINGS_CONFIG3_INIT                 0
#endif
#ifndef SETTINGS_CONFIG4_INIT
    #define SETTINGS_CONFIG4_INIT                 0
#endif
#ifndef SETTINGS_CONFIG5_INIT
    #define SETTINGS_CONFIG5_INIT                 0
#endif

#ifndef VALUE_0_INIT
    #define VALUE_0_INIT                 0
#endif
#ifndef VALUE_1_INIT
    #define VALUE_1_INIT                 0
#endif
#ifndef VALUE_2_INIT
    #define VALUE_2_INIT                 0
#endif
#ifndef VALUE_3_INIT
    #define VALUE_3_INIT                 0
#endif

#ifndef CONFIG_MODE_TIMEOUT
    #define CONFIG_MODE_TIMEOUT                 120
#endif

#ifndef SHUTDOWN_TIMER
    #define SHUTDOWN_TIMER                      30
#endif

#ifndef AUTH_TOKEN_REQ
    #define AUTH_TOKEN_REQ                      0
#endif
#ifndef AUTH_TOKEN_REQUEST_TIMEOUT
    #define AUTH_TOKEN_REQUEST_TIMEOUT          1000
#endif
#ifndef AUTH_TOKEN_REQUEST_RETRY
    #define AUTH_TOKEN_REQUEST_RETRY            9
#endif

#ifndef ESPNOW_REPEAT_SEND
    #define ESPNOW_REPEAT_SEND                  5
#endif
#if (!defined ESPNOW_TELEGRAM_WIZMOTE && !defined ESPNOW_TELEGRAM_EXTENDED) 
    #define ESPNOW_TELEGRAM_EXTENDED
#endif


#if (defined ACTIVE_PIN && !defined ACTIVE_PIN_POLARITY)
    #define ACTIVE_PIN_POLARITY                 LOW 
#endif
#if (defined VOLTAGE_REGULATOR_PIN && !defined VOLTAGE_REGULATOR_POLARITY)
    #define VOLTAGE_REGULATOR_POLARITY          HIGH 
#endif
#if (defined SETUP_PIN && !defined SETUP_PIN_POLARITY)
    #define SETUP_PIN_POLARITY                  LOW 
#endif
#if (defined SETUP_PIN && !defined SETUP_PIN_DELAY)
    #define SETUP_PIN_DELAY                     1000 
#endif
#if (defined SHUTDOWN_PIN && !defined SHUTDOWN_PIN_POLARITY)
    #define SHUTDOWN_PIN_POLARITY               LOW 
#endif
#if ((defined DEEPSLEEP_WAKEUP_GPIO_PIN1 || defined DEEPSLEEP_WAKEUP_GPIO_PIN2 || defined DEEPSLEEP_WAKEUP_GPIO_PIN3 || defined DEEPSLEEP_WAKEUP_GPIO_PIN4) && !defined DEEPSLEEP_WAKEUP_GPIO_PIN_POLARITY)
    #define DEEPSLEEP_WAKEUP_GPIO_PIN_POLARITY      LOW
#endif

#ifndef DEEPSLEEP_WAKEUP_GPIO_PIN_POLARITY
    #define DEEPSLEEP_WAKEUP_GPIO_PIN_POLARITY      LOW
#endif

#ifndef DEEPSLEEP_TIME
    #define DEEPSLEEP_TIME                              0
#endif