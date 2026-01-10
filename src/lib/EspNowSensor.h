#ifndef EspNowSensor_h
#define EspNowSensor_h

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <user_interface.h>
#include <espnow.h>
#endif

#ifdef ESP32
#include <WiFi.h>
#include <esp_now.h>
#endif

#include <Arduino.h>
#include <EEPROM.h>

#include <configuration.h>

#include <lib/logging.h>
#include <lib/authMac.h>
#include <lib/ota.h>
#include <lib/batteryLevel.h>

#define FRAMEWORK                             "ESP!NOW Sensor Framework"
#define FRAMEWORK_VERSION                     0x000205

// This is the space in bytes that will be reserved in EEPROM for storing data that should be persisted.
#define EEPROM_SIZE 512
// This is the offset at which the sequence number will be stored inside the EEPROM.
#define EEPROM_INITIALIZED                  0  

#define EEPROM_SEQUENCE                     10                 
#define EEPROM_CHANNEL                      104
#define EEPROM_USEAUTHTOKEN                 106
#define EEPROM_BROADCASTREPEAT              107
#define EEPROM_DEFAULTCHANNEL               108

#define EEPROM_DEEPSLEEP_TIME               200
#define EEPROM_CONFIG0                      204
#define EEPROM_CONFIG1                      208
#define EEPROM_CONFIG2                      212
#define EEPROM_CONFIG3                      216
#define EEPROM_CONFIG4                      220
#define EEPROM_CONFIG5                      224

#define EEPROM_VALUE0                       400
#define EEPROM_VALUE1                       404
#define EEPROM_VALUE2                       408
#define EEPROM_VALUE3                       412

#define EEPROM_INITIALIZED_VALUE            FRAMEWORK_VERSION

typedef struct EspNowMessageStructure {
  uint8_t program = 0x0;  // 0x91 for ON button, 0x81 for all others
  uint8_t seq[4];         // Incremental sequence number 32-bit unsigned integer LSB first
  uint8_t dTypeState = 0x00;     // Data type: button (32)
  uint8_t dataState = 0x00;;          // Identifies which button is being pressed
  uint8_t dTypeBattery = 0x00;;     // Data type: batterylevel (1)
  uint8_t dataBattery = 0x00;;          // EspNowSensor batterylevel (out of 100)
  #ifdef ESPNOW_TELEGRAM_EXTENDED
  uint8_t dTypeData1 = 0x00;;
  uint32_t data1 = 0x00;;
  uint8_t dTypeData2 = 0x00;;
  uint32_t data2 = 0x00;;
  uint8_t dTypeData3 = 0x00;;
  uint32_t data3 = 0x00;;
  uint8_t dTypeData4 = 0x00;;
  uint32_t data4 = 0x00;;
  #endif
  uint8_t mac[4];         // CCM MAC (Message Authentication Code)
} espnow_message_structure_t;

typedef struct SettingsStructure{
  uint16_t channel = ESPNOW_CHANNEL;
  uint8_t useAuthToken = AUTH_TOKEN_REQ;
  uint8_t broadcastRepeat = ESPNOW_REPEAT_SEND;
  uint8_t defaultChannel = 1;
  uint32_t deepsleepTime = DEEPSLEEP_TIME;
  uint32_t Config[6] = {(uint32_t)SETTINGS_CONFIG0_INIT, (uint32_t)SETTINGS_CONFIG1_INIT, (uint32_t)SETTINGS_CONFIG2_INIT, (uint32_t)SETTINGS_CONFIG3_INIT, (uint32_t)SETTINGS_CONFIG4_INIT, (uint32_t)SETTINGS_CONFIG5_INIT};
} settings_structure_t;

typedef struct ValueStructure{
  uint32_t Value[4] = {(uint32_t)VALUE_0_INIT, (uint32_t)VALUE_1_INIT, (uint32_t)VALUE_2_INIT, (uint32_t)VALUE_3_INIT};
} value_structure_t;

class EspNowSensorClass {

public:

    EspNowSensorClass();

    void begin();

    void espnowMessageDataSetProgram(uint8_t prog);
    void espnowMessageDataAddSensorValue(uint8_t dpid, uint32_t value);
    void espnowMessageDataSend();
    void espnowMessageAuthTokenRequest();
    void espnowMessageAlive();
    void espnowMessageConfig();

    bool broadcastSending = false;
    bool readyToSend = false;

    void espnowAuthCheck();

    void registerSendCallback(esp_now_send_cb_t cb);
    void registerRecvCallback(esp_now_recv_cb_t cb);
    void OnDataSent(uint8_t *mac, uint8_t sendStatus );
    void OnDataRecv(uint8_t *mac, const uint8_t *incomingData, uint8_t len);

    void configmodeHandle();
    void configmodeEnter();
    void configmodeLeave();
    void shutDownCheck();
    void powerOff();

    void initSettings();
    void saveEspNowSettings();
    void saveSensorSettings();
    void loadSettings();
    void factorySettings();

    void storeValue(uint8_t no, uint32_t value);
    void loadValues();
    void initValues();

    uint8_t batteryLevel();

    void setSequenceNumber(uint32_t sequenceNumber);
    uint32_t getSequenceNumber();
    uint32_t nextSequenceNumber();

    bool      configmode = false;

    String DeviceName;
    value_structure_t values;
    settings_structure_t settings;
private:

    bool initialized = false;
    static uint8_t broadcastAddress[];

    void VersionInfo();
    void setupCustomMAC();
    void setupDeviceName();
    void setupPin();
    void wakeUpReason();
    void printException(const char *message);

    void espnowinitialize();
    void espnowSetChannel(uint8_t ch);
    void espnowBroadcast(uint8_t *data, size_t data_size);
    void espnowMessageClear();
    void espnowMessageSetupAuthCode();
    void espnowBroadcastMessage();
    void espnowMessageSend();
    uint8_t broadcastChannel = 0;

    void setupConfigMode();
    unsigned long configModeTime;
    unsigned long startTime;

    //Authentification
    bool      authTokenReqSent = false;
    bool      authTokenReceived = false;
    unsigned long authTokenRequestedTime;
    uint8_t   authTokenRequestRetry = 0;
    uint8_t   authToken[4] = {0x0, 0x0, 0x0, 0x0};           // CCM MAC (Message Authentication Code)

    //ESP!Now Broadcast
    espnow_message_structure_t broadcast_data_to_send;
    espnow_message_structure_t broadcast_data;
    uint8_t broadcastRepeat = 0;
    bool broadcastAllChannels = false;

    uint32_t  lastRecvSeq = 0;
    //=============================Creditials
    const char* wifiSSID = WIFI_SSID;
    const char* wifiPassword = WIFI_PASSWORD;
    //=============================Webserver
    String webserverGetPageRoot();
    void webserverHandleSubmit();
    void webserverSetup();

};
    extern EspNowSensorClass EspNowSensor;
#endif