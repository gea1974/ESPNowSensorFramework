#include <lib/EspNowSensor.h>
#ifdef ESP32
  #include <esp_wifi.h>
#endif

uint8_t EspNowSensorClass::broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

EspNowSensorClass EspNowSensor;

#ifdef ESP32
  #include <esp_wifi.h>
  #include <ESP32WebServer.h>
  ESP32WebServer server(80);
#endif
#ifdef ESP8266
  #include <ESP8266WebServer.h>
  ESP8266WebServer server(80);
#endif

EspNowSensorClass::EspNowSensorClass() {}

//=============================Setup
void EspNowSensorClass::begin() {
    // Prevent calling this method a second time
    if (initialized) {
        return;
    }
    startTime = millis();
    LoggingBegin();
    printLogMsg("\n\n");
    if (WiFi.mode(WIFI_STA) != true) {
      printLogMsgTime("Setting Wi-Fi mode failed!\n");
    }
    setupPin();

    setupDeviceName();
    VersionInfo();
    setupCustomMAC();
    wakeUpReason();  


    EEPROM.begin(EEPROM_SIZE);

    loadSettings();
    loadValues();

    espnowinitialize();

    initialized = true;
}

void EspNowSensorClass::VersionInfo(){
  printLogMsg("%s\n",EspNowSensor.DeviceName.c_str());
  printLogMsg("%s V%X.%X.%X (%s) - %s\n",PRODUCT, (VERSION >> 16) & 0xFF, (VERSION >> 8) & 0xFF, VERSION & 0xFF, OWNER, DESCRIPTION);
  printLogMsg("%s V%X.%X.%X\n",FRAMEWORK, (FRAMEWORK_VERSION >> 16) & 0xFF, (FRAMEWORK_VERSION >> 8) & 0xFF, FRAMEWORK_VERSION & 0xFF);
  printLogMsg("MAC: %s\n", WiFi.macAddress().c_str());
  printLogMsg("MCU: %s\n", MCU_TYPE);
  printLogMsg("BUILD: %X\n", BUILD);

  printLogMsg("Authentification Key: %s\n", AUTHENTIFICATION_KEY);  
  printLogMsg("\n");
}
void EspNowSensorClass::setupCustomMAC()
{
  #ifdef CUSTOM_MAC_ADDRESS
  printLogMsgTime("Info: Setting custom MAC address: %s\n" ,CUSTOM_MAC_ADDRESS);
  uint8_t mac[6];
  const char customMac[18] = {CUSTOM_MAC_ADDRESS};
  char str[2];
  str[0] = customMac[0];
  str[1] = customMac[1];
  mac[0] = (uint8_t) strtol(str, 0, 16);
  str[0] = customMac[3];
  str[1] = customMac[4];
  mac[1] = (uint8_t) strtol(str, 0, 16);     
  str[0] = customMac[6];
  str[1] = customMac[7];
  mac[2] = (uint8_t) strtol(str, 0, 16);
  str[0] = customMac[9];
  str[1] = customMac[10];
  mac[3] = (uint8_t) strtol(str, 0, 16);                        
  str[0] = customMac[12];
  str[1] = customMac[13];
  mac[4] = (uint8_t) strtol(str, 0, 16);
  str[0] = customMac[15];
  str[1] = customMac[16];
  mac[5] = (uint8_t) strtol(str, 0, 16);
  wifi_set_macaddr(0, const_cast<uint8*>(mac));   //This line changes MAC adderss of ESP8266
  #endif
}
void EspNowSensorClass::setupDeviceName(){
  char buffer[100];
  uint8_t mac[6];
  WiFi.macAddress(mac);
  #ifdef CUSTOM_MAC_ADDRESS
  const char customMac[18] = {CUSTOM_MAC_ADDRESS};
  char str[2];
  str[0] = customMac[0];
  str[1] = customMac[1];
  mac[0] = (uint8_t) strtol(str, 0, 16);
  str[0] = customMac[3];
  str[1] = customMac[4];
  mac[1] = (uint8_t) strtol(str, 0, 16);     
  str[0] = customMac[6];
  str[1] = customMac[7];
  mac[2] = (uint8_t) strtol(str, 0, 16);
  str[0] = customMac[9];
  str[1] = customMac[10];
  mac[3] = (uint8_t) strtol(str, 0, 16);                        
  str[0] = customMac[12];
  str[1] = customMac[13];
  mac[4] = (uint8_t) strtol(str, 0, 16);
  str[0] = customMac[15];
  str[1] = customMac[16];
  mac[5] = (uint8_t) strtol(str, 0, 16);
  #endif
  sprintf(buffer,"%s_%02X%02X%02X%02X%02X%02X",PRODUCT_ID, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  DeviceName = buffer;
}
void EspNowSensorClass::setupPin(){
   #ifdef VOLTAGE_REGULATOR_PIN
      pinMode(VOLTAGE_REGULATOR_PIN, OUTPUT);
      digitalWrite(VOLTAGE_REGULATOR_PIN, VOLTAGE_REGULATOR_POLARITY);
    #endif
    #ifdef ACTIVE_PIN
      pinMode(ACTIVE_PIN, OUTPUT);
      digitalWrite(ACTIVE_PIN, ACTIVE_PIN_POLARITY);
    #endif
    #ifdef SETUP_PIN
      pinMode(SETUP_PIN, INPUT_PULLUP);
    #endif
    #ifdef SHUTDOWN_PIN
      pinMode(SHUTDOWN_PIN, INPUT_PULLUP);
    #endif
}
void EspNowSensorClass::wakeUpReason(){
String reason;
  #ifdef ESP8266
    String resetReasonStr = ESP.getResetReason();
    printLogMsgTime("Info: Reset: Reason: %s\n", resetReasonStr.c_str());
  #endif
  #ifdef ESP32
    String wakeupReasonStr;
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
    switch(wakeup_reason)
    {
      case ESP_SLEEP_WAKEUP_EXT0 : wakeupReasonStr = "Wakeup EXT0 RTC_IO"; break;
      case ESP_SLEEP_WAKEUP_EXT1 : wakeupReasonStr = "Wakeup EXT1 RTC_CNTL"; break;
      case ESP_SLEEP_WAKEUP_TIMER : wakeupReasonStr = "Wakeup Timer"; break;
      case ESP_SLEEP_WAKEUP_TOUCHPAD : wakeupReasonStr = "Wakeup Touchpad"; break;
      case ESP_SLEEP_WAKEUP_ULP : wakeupReasonStr = "Wakeup ULP program"; break;
      case ESP_SLEEP_WAKEUP_GPIO: wakeupReasonStr = "Wakeup GPIO"; break;
      default: {
          wakeupReasonStr = "Wakeup unknown (";
          wakeupReasonStr += wakeup_reason;
          wakeupReasonStr += ")";
          break;
      }
    }
    String resetReasonStr;
    esp_reset_reason_t reset_reason =  esp_reset_reason();
    switch(reset_reason)
    {
      case ESP_RST_UNKNOWN : resetReasonStr = "Reset reason can not be determined"; break;
      case ESP_RST_POWERON : resetReasonStr = "Reset due to power-on event"; break;
      case ESP_RST_EXT : resetReasonStr = "Reset by external pin"; break;
      case ESP_RST_SW : resetReasonStr = "Software reset via esp_restart"; break;
      case ESP_RST_PANIC : resetReasonStr = "Software reset due to exception/panic"; break;
      case ESP_RST_INT_WDT: resetReasonStr = "Reset (software or hardware) due to interrupt watchdog"; break;
      case ESP_RST_TASK_WDT: resetReasonStr = "Reset due to task watchdog"; break;
      case ESP_RST_WDT: resetReasonStr = "Reset due to other watchdogs"; break;
      case ESP_RST_DEEPSLEEP: {
                                resetReasonStr = "Reset after exiting deep sleep mode ("; 
                                resetReasonStr += wakeupReasonStr.c_str();
                                resetReasonStr += ")";
                                break;
                              }
      case ESP_RST_BROWNOUT: resetReasonStr = "Brownout reset (software or hardware)"; break;
      case ESP_RST_SDIO: resetReasonStr = "Reset over SDIO"; break;
      default: {
          resetReasonStr = "Reset reason unknown (";
          resetReasonStr += reset_reason;
          resetReasonStr += ")";
          break;
      }
    }
    printLogMsgTime("Info: Reset: Reason: %s\n", resetReasonStr.c_str());
  #endif



  }

//=============================Config mode
void EspNowSensorClass::configmodeHandle(){
    #ifdef SETUP_PIN
    if (digitalRead(SETUP_PIN)==SETUP_PIN_POLARITY) {
      unsigned long setupDelay = millis();
      while (digitalRead(SETUP_PIN)==SETUP_PIN_POLARITY) { // wait for Button released.
        if ((millis()-setupDelay)>SETUP_PIN_DELAY) digitalWrite(ACTIVE_PIN, millis() % 1000 > 250);         //blink led fast while we are in config mode
      }
      if (!configmode && ((millis()-setupDelay)>SETUP_PIN_DELAY)){
        printLogMsgTime("Info: Button: Config mode enter\n" );
        configmodeEnter();
      }
      else if (configmode) {
        printLogMsgTime("Info: Button: Config mode leave\n"); 
        configmodeLeave();  
      }
    }
  #endif

  if (configmode) {
    if (configModeTime==0) configModeTime = millis();
    if (WiFi.status() == WL_CONNECTED){

      #ifdef ACTIVE_PIN
        digitalWrite(ACTIVE_PIN, millis() % 500 > 250);         //blink led fast while we are in config mode
      #endif
      server.handleClient();  
      ArduinoOTA.handle();
      yield();
    }
    if( ((millis()-configModeTime)>(CONFIG_MODE_TIMEOUT*1000)) && (CONFIG_MODE_TIMEOUT>0) ) {
      printLogMsgTime("Info: Config: Time out\n"); 
      configmodeLeave();  
    }
  }
}
void EspNowSensorClass::configmodeEnter(){
  setupConfigMode();
  configmode = true;
  espnowMessageConfig();
}
void EspNowSensorClass::configmodeLeave(){
  configmode = false;  
  startTime = millis();
  #ifdef ACTIVE_PIN
    pinMode(ACTIVE_PIN, OUTPUT);
    digitalWrite(ACTIVE_PIN, ACTIVE_PIN_POLARITY);
  #endif
}
void EspNowSensorClass::setupConfigMode() {
  int wifiCounter = 0;

  #ifdef ACTIVE_PIN
    digitalWrite(ACTIVE_PIN, !ACTIVE_PIN_POLARITY);      
    delay(100);
    digitalWrite(ACTIVE_PIN, ACTIVE_PIN_POLARITY);  
    delay(100);
    digitalWrite(ACTIVE_PIN, !ACTIVE_PIN_POLARITY);  
    delay(100);
    digitalWrite(ACTIVE_PIN, ACTIVE_PIN_POLARITY);  
  #endif
  
  // We start by connecting to a WiFi network

  #ifdef ESP8266
  WiFi.hostname(EspNowSensor.DeviceName.c_str());
  #endif
  #ifdef ESP32
  const char* hostname = EspNowSensor.DeviceName.c_str();
  WiFi.mode(WIFI_STA);
  printLogMsgTime("Wifi: Default hostname %s \n" , WiFi.getHostname());
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.setHostname(hostname); //define hostname
  #endif

  printLogMsgTime("Wifi: Connecting to %s " , wifiSSID);
  WiFi.begin(wifiSSID, wifiPassword);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if(wifiCounter < 10){
      #ifdef ACTIVE_PIN
        digitalWrite(ACTIVE_PIN, ((wifiCounter%2)!=ACTIVE_PIN_POLARITY));  
      #endif
//      delay(500);
      printLogMsg(".");
    }
    else{
      printLogMsg("\n");
      printLogMsgTime("Wifi: Connecting failed .. power off\n" );
      powerOff();
    }
    wifiCounter++;
  }
  if ((WiFi.status() == WL_CONNECTED))
  {
    printLogMsg("\n");
    String ipaddress = WiFi.localIP().toString();
    printLogMsgTime("Wifi: connected to %s at channel %d\n" ,WiFi.SSID().c_str(), WiFi.channel());
    #ifdef ESP32
    printLogMsgTime("Wifi: Custom hostname %s \n" , WiFi.getHostname());
    #endif
//    printLogMsgTime("MAC: %s\n", WiFi.macAddress().c_str());    
    printLogMsgTime("IP: %s\n", ipaddress.c_str());
    ArduinoOtaStart();
    webserverSetup();
    printLogMsgTime("Info Configuration mode: Ready for upload!\n" );
  }
}
void EspNowSensorClass::powerOff() {
    if(broadcastSending) printLogMsgTime("PowerOff: Waiting for ESP!Now message sent\n" );
    while (broadcastSending) delay(50);

    #ifdef ACTIVE_PIN
    digitalWrite(ACTIVE_PIN, !ACTIVE_PIN_POLARITY);
    #endif

    #ifdef POWER_OFF_EXTERNAL
      printLogMsgTime("PowerOff: Waiting for external shutdown\n" );
      while(true) {
        printLogMsg(".");
        delay(100);
      }
    #endif

    #ifdef POWER_OFF_VOLTAGE_REGULATOR
      printLogMsgTime("PowerOff: Voltage regulator shutdown\n" );
      delay(100);
      digitalWrite(VOLTAGE_REGULATOR_PIN, !VOLTAGE_REGULATOR_POLARITY);
      while(true) {
        printLogMsg(".");
        delay(100);
      }
    #endif

    #ifdef POWER_OFF_DEEPSLEEP
      uint64_t duration = settings.deepsleepTime * 1000000;
      #ifdef ESP8266
        if (duration>ESP.deepSleepMax()) duration=ESP.deepSleepMax();
      #endif
      String durationStr;
      if (settings.deepsleepTime==0)  durationStr += "Reset";
      else {
        durationStr += "Timer=";
        durationStr += (int32_t)(duration / 1000000);
        durationStr += "s";
      }
      printLogMsgTime("PowerOff: Deepsleep: Wakeup: %s\n", durationStr.c_str());
      delay(100);
      #ifdef ESP8266
        ESP.deepSleep(duration);
      #endif

      #ifdef ESP32
        #if (defined ESP32C3 || defined ESP32C2)
          uint64_t gpioWakeupPins = 0;
          #ifdef DEEPSLEEP_WAKEUP_GPIO_PIN1
            if(esp_sleep_is_valid_wakeup_gpio((gpio_num_t)DEEPSLEEP_WAKEUP_GPIO_PIN1)) {
              printLogMsgTime("PowerOff: Deepsleep: Wakeup: GPIO=%d\n", DEEPSLEEP_WAKEUP_GPIO_PIN1);
              gpioWakeupPins += (1 << DEEPSLEEP_WAKEUP_GPIO_PIN1);
              pinMode(DEEPSLEEP_WAKEUP_GPIO_PIN1, DEEPSLEEP_WAKEUP_GPIO_PIN_POLARITY==LOW ? INPUT_PULLUP : INPUT);
            }
            else {
              printLogMsgTime("PowerOff: Deepsleep: GPIO pin %d is invalid\n", DEEPSLEEP_WAKEUP_GPIO_PIN1);
            }
          #endif
          #ifdef DEEPSLEEP_WAKEUP_GPIO_PIN2
            if(esp_sleep_is_valid_wakeup_gpio((gpio_num_t)DEEPSLEEP_WAKEUP_GPIO_PIN2)) {
              printLogMsgTime("PowerOff: Deepsleep: Wakeup: GPIO=%d\n", DEEPSLEEP_WAKEUP_GPIO_PIN2);
              gpioWakeupPins += (1 << DEEPSLEEP_WAKEUP_GPIO_PIN2);
              pinMode(DEEPSLEEP_WAKEUP_GPIO_PIN2, DEEPSLEEP_WAKEUP_GPIO_PIN_POLARITY==LOW ? INPUT_PULLUP : INPUT);
            }
            else {
              printLogMsgTime("PowerOff: Deepsleep: GPIO pin %d is invalid\n", DEEPSLEEP_WAKEUP_GPIO_PIN2);
            }
          #endif
          #ifdef DEEPSLEEP_WAKEUP_GPIO_PIN3
            if(esp_sleep_is_valid_wakeup_gpio((gpio_num_t)DEEPSLEEP_WAKEUP_GPIO_PIN3)) {
              printLogMsgTime("PowerOff: Deepsleep: Wakeup: GPIO=%d\n", DEEPSLEEP_WAKEUP_GPIO_PIN3);
              gpioWakeupPins += (1 << DEEPSLEEP_WAKEUP_GPIO_PIN3);
              pinMode(DEEPSLEEP_WAKEUP_GPIO_PIN3, DEEPSLEEP_WAKEUP_GPIO_PIN_POLARITY==LOW ? INPUT_PULLUP : INPUT);
            }
            else {
              printLogMsgTime("PowerOff: Deepsleep: GPIO pin %d is invalid\n", DEEPSLEEP_WAKEUP_GPIO_PIN3);
            }
          #endif
          #ifdef DEEPSLEEP_WAKEUP_GPIO_PIN4
            if(esp_sleep_is_valid_wakeup_gpio((gpio_num_t)DEEPSLEEP_WAKEUP_GPIO_PIN4)) {
              printLogMsgTime("PowerOff: Deepsleep: Wakeup: GPIO=%d\n", DEEPSLEEP_WAKEUP_GPIO_PIN4);
              gpioWakeupPins += (1 << DEEPSLEEP_WAKEUP_GPIO_PIN4);
              pinMode(DEEPSLEEP_WAKEUP_GPIO_PIN4, DEEPSLEEP_WAKEUP_GPIO_PIN_POLARITY==LOW ? INPUT_PULLUP : INPUT);
            }
            else {
              printLogMsgTime("PowerOff: Deepsleep: GPIO pin %d is invalid\n", DEEPSLEEP_WAKEUP_GPIO_PIN4);
            }
          #endif
          if (gpioWakeupPins!=0) {
            esp_deep_sleep_enable_gpio_wakeup(gpioWakeupPins, (esp_deepsleep_gpio_wake_up_mode_t)DEEPSLEEP_WAKEUP_GPIO_PIN_POLARITY);
          }
        #endif
        if (duration>0) esp_sleep_enable_timer_wakeup(duration); // 10.000.000 Mikrosekunden = 10 Sekunden
        esp_deep_sleep_start();
      #endif
    #endif
}
void EspNowSensorClass::shutDownCheck(){
  #ifdef SHUTDOWN_PIN
    if (digitalRead(SHUTDOWN_PIN)==SHUTDOWN_PIN_POLARITY) {
        printLogMsgTime("Info: Button: Shutdown request\n" );
        configmode = false;  
        delay(100);
        powerOff();
    }
  #endif

  if (!configmode) {
    #ifdef SHUTDOWN_TIMER
      if ( (millis()>(startTime + (SHUTDOWN_TIMER*1000))) ){
          printLogMsgTime("Info: Timer: Shutdown request\n" );
          delay(100);
          powerOff();
      }
    #endif
  }
}

//=============================ESP!Now System
void EspNowSensorClass::espnowinitialize() {
    // Set device as a Wi-Fi Station
    if (WiFi.mode(WIFI_STA) != true) {
        printException("setting Wi-Fi mode failed");
    }

    // Immediately disconnect from any networks
    if (WiFi.disconnect() != true) {
        printException("disconnecting Wi-Fi failed");
    }

    // Initialize ESP-NOW
    if (esp_now_init() != OK) {
        printException("initializing ESP-NOW failed");
    }

    #ifdef ESP8266
    // Set this device's role to CONTROLLER
    if (esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER) != OK) {
        printException("setting ESP-NOW role failed");
    }
    #endif
        // printLogMsgTime("ESP-NOW initialized\n" );
}
void EspNowSensorClass::espnowSetChannel(uint8_t ch) {
    #ifdef ESP8266
    esp_now_del_peer(EspNowSensorClass::broadcastAddress);
    wifi_set_channel(ch);
    esp_now_add_peer(EspNowSensorClass::broadcastAddress, ESP_NOW_ROLE_SLAVE, ch, NULL, 0);
    #endif

    #ifdef ESP32
    // Register peer
    esp_now_peer_info_t peerInfo;
    memset(&peerInfo, 0, sizeof(peerInfo));
    memcpy(peerInfo.peer_addr, EspNowSensorClass::broadcastAddress, 6);
    peerInfo.channel = ch;
    peerInfo.encrypt = false;

    if (esp_now_is_peer_exist(peerInfo.peer_addr)) {
        esp_err_t delStatus = esp_now_del_peer(peerInfo.peer_addr);
//        printLogMsgTime("ESP!Now: delete peer: %d\n" ,delStatus);
    }
    
    esp_wifi_set_channel(ch, WIFI_SECOND_CHAN_NONE);

    if (!esp_now_is_peer_exist(peerInfo.peer_addr)) {
        esp_err_t addStatus = esp_now_add_peer(&peerInfo);
//        printLogMsgTime("ESP!Now: add peer: %d\n" ,addStatus);
    }

    #endif
}
void EspNowSensorClass::espnowBroadcast(uint8_t *data, size_t data_size) {
    if (esp_now_send(EspNowSensorClass::broadcastAddress, data, data_size) != OK) {
        printException("sending ESP-NOW message failed");
    }
}
void EspNowSensorClass::registerSendCallback(esp_now_send_cb_t cb) {
    if (esp_now_register_send_cb(cb) != OK) {
        printException("registering ESP-NOW send callback failed");
    }
}
void EspNowSensorClass::registerRecvCallback(esp_now_recv_cb_t cb){
    if (esp_now_register_recv_cb(cb) != OK) {
        printException("registering ESP-NOW receive callback failed");
    }
}
void EspNowSensorClass::OnDataSent(uint8_t *mac, uint8_t sendStatus)
{
     espnowBroadcastMessage();
}
void EspNowSensorClass::OnDataRecv(uint8_t * mac, const uint8_t *incomingData, uint8_t len)
{
  uint32_t curRecvSeq = calcCRC32((uint8_t *) &incomingData, len);
  String rawMessage = "";
  for (int i = 0; i < len; i++)
  {
    char buffer[3];
    sprintf(buffer,"%02X", incomingData[i]);
    rawMessage += buffer;
  }

    if (len != sizeof(broadcast_data)) {
      printLogMsgTime("ESP!Now: Receive: incoming message wrong length: %d\n" , len);
      return;
    }
    espnow_message_structure_t received_data;
    memcpy(&(received_data.program), incomingData, len);
    
    curRecvSeq = calcCRC32((uint8_t *) &received_data, len);
    printLogMsgTime("ESP!Now: Receive: incoming message: Source: %02X:%02X:%02X:%02X:%02X:%02X, Len: %d, CRC: %08X Last: %08X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], len, curRecvSeq, lastRecvSeq);

  if (curRecvSeq!=lastRecvSeq) {

  printLogMsgTime("ESP!Now: Receive: incoming message: RAW: %s\n", rawMessage.c_str());

    if ( (received_data.program==0xfb) && !authTokenReceived ) {
      authToken[0] = received_data.mac[0];  
      authToken[1] = received_data.mac[1];   
      authToken[2] = received_data.mac[2];   
      authToken[3] = received_data.mac[3]; 
      printLogMsgTime("ESP!Now: Receive: Authentification token from %02X:%02X:%02X:%02X:%02X:%02X: %02X%02X%02X%02X \n" , mac [0], mac [1], mac [2], mac [3], mac [4], mac [5], authToken [0], authToken [1], authToken [2], authToken [3]);
      authTokenReceived = true;
      if (broadcastChannel!=settings.defaultChannel){
        settings.defaultChannel = broadcastChannel;
        saveEspNowSettings();
      }
    }
    else if ( (received_data.program==0xfb) && authTokenReceived ) ;
    else {
      printLogMsgTime("ESP!Now: Receive: wrong message program type: %d\n" , broadcast_data.program);
    }
    lastRecvSeq = curRecvSeq;
  }
}

void EspNowSensorClass::printException(const char *message) {
    printLogMsgTime("ESP!Now Exception: %s -> System will restart in 5 seconds...\n" ,message);
    
    delay(5000);
    #ifdef ESP8266
    system_restart(); 
    #endif 
    #ifdef ESP32 
    esp_restart(); 
    #endif
}

//=============================ESP!Now Message
void EspNowSensorClass::espnowAuthCheck()
{
  if (!authTokenReceived){
    if (!authTokenReqSent) {
      if (settings.useAuthToken) {
          printLogMsgTime("ESP!Now: Authentification: token required\n");
          espnowMessageAuthTokenRequest();
          authTokenRequestRetry++;
          authTokenRequestedTime = millis();
      }
      else {
          printLogMsgTime("ESP!Now: Authentification: token not required\n");
          #ifdef ESPNOW_ALIVE
            espnowMessageAlive();
          #endif
          authTokenReceived = true;
      }
      authTokenReqSent = true;
    }

    if (authTokenReqSent && !authTokenReceived){
      if ((millis()-authTokenRequestedTime) > AUTH_TOKEN_REQUEST_TIMEOUT){
        printLogMsgTime("ESP!Now: Authentification: authentification token request timeout\n");
        broadcastChannel++;
        authTokenReqSent = false;
        if (authTokenRequestRetry>=AUTH_TOKEN_REQUEST_RETRY) {
          printLogMsgTime("ESP!Now: Authentification: authentification token request no response .. power off\n");
          powerOff();
        }
      }
    }
  }

  readyToSend = authTokenReceived;
}
void EspNowSensorClass::espnowMessageClear(){
  broadcast_data_to_send.program = 0x00;
  broadcast_data_to_send.dTypeState = 0x00;
  broadcast_data_to_send.dataState = 0x00;
  broadcast_data_to_send.dTypeBattery = 0x00;
  broadcast_data_to_send.dataBattery = 0x00;
  #ifdef ESPNOW_TELEGRAM_EXTENDED
  broadcast_data_to_send.dTypeData1 = 0x00;
  broadcast_data_to_send.data1 = 0x00;
  broadcast_data_to_send.dTypeData2 = 0x00;
  broadcast_data_to_send.data2 = 0x00;
  broadcast_data_to_send.dTypeData2 = 0x00;
  broadcast_data_to_send.data2 = 0x00;
  #endif
}
void EspNowSensorClass::espnowMessageSetupAuthCode(){
    // init Message Authentification Code
    broadcast_data.mac[0] = authToken[0];  
    broadcast_data.mac[1] = authToken[1];   
    broadcast_data.mac[2] = authToken[2];   
    broadcast_data.mac[3] = authToken[3];   
    // calculate Message Authentification Code
    uint8_t mac[4]; 
    char myKey[17] = {AUTHENTIFICATION_KEY};
    calculateMAC((uint8_t *) &broadcast_data, sizeof(broadcast_data), mac, (uint8_t *) &myKey);
//    printLogMsgTime("ESP!Now: Authentification: Token: %02X%02X%02X%02X MAC: %02X%02X%02X%02X \n" , broadcast_data.mac [0], broadcast_data.mac [1], broadcast_data.mac [2], broadcast_data.mac [3],mac[0],mac[1],mac[2],mac[3]);
    broadcast_data.mac[0] = mac[0];  
    broadcast_data.mac[1] = mac[1];  
    broadcast_data.mac[2] = mac[2];  
    broadcast_data.mac[3] = mac[3]; 
}
void EspNowSensorClass::espnowMessageDataAddSensorValue(uint8_t dpid, uint32_t value) {
  uint8_t b[4];
  u8from32 (b,value);
    if      (dpid==0){
      printLogMsgTime("ESP!Now: Message: Data: DPID: null\n");
    }
    else if (dpid==DPID_STATE) {
      broadcast_data_to_send.dTypeState = dpid;
      #ifdef DPID_STATE_POLARITY
      broadcast_data_to_send.dataState = b[3]==DPID_STATE_POLARITY;
      #endif
      #ifndef DPID_STATE_POLARITY
      broadcast_data_to_send.dataState = b[3];
      #endif
      printLogMsgTime("ESP!Now: Message: Data: DPID: %d, State = %d\n",dpid, broadcast_data_to_send.dataState);
    }
    else if (dpid==DPID_BATTERY) {
      switch (b[3]){
        case 1:{
          b[3] = 50;
          break;
        }
        case 2:{
          b[3] = 100;
          break;
        }
      }
      broadcast_data_to_send.dTypeBattery = dpid;
      broadcast_data_to_send.dataBattery = b[3];
      printLogMsgTime("ESP!Now: Message: Data: DPID: %d, Battery = %d\n",dpid,broadcast_data_to_send.dataBattery);
    }
    #ifdef ESPNOW_TELEGRAM_EXTENDED
    else if (dpid==DPID_VALUE1) {
      broadcast_data_to_send.dTypeData1 = dpid;
      broadcast_data_to_send.data1 = value;
      printLogMsgTime("ESP!Now: Message: Data: DPID: %d, Value1 = %d\n",dpid,broadcast_data_to_send.data1);
    }
    else if (dpid==DPID_VALUE2) {
      broadcast_data_to_send.dTypeData2 = dpid;
      broadcast_data_to_send.data2 = value;
      printLogMsgTime("ESP!Now: Message: Data: DPID: %d, Value2 = %d\n",dpid,broadcast_data_to_send.data2);
    }
    else if (dpid==DPID_VALUE3) {
      broadcast_data_to_send.dTypeData3 = dpid;
      broadcast_data_to_send.data3 = value;
      printLogMsgTime("ESP!Now: Message: Data: DPID: %d, Value3 = %d\n",dpid,broadcast_data_to_send.data3);
    }
    else if (dpid==DPID_VALUE4) {
      broadcast_data_to_send.dTypeData4 = dpid;
      broadcast_data_to_send.data4= value;
      printLogMsgTime("ESP!Now: Message: Data: DPID: %d, Value4 = %d\n",dpid,broadcast_data_to_send.data4);
    }
    #endif
    else {
      printLogMsgTime("ESP!Now: Message: Data: DPID: %d: ID not implemented\n",dpid);
    }

    #ifdef ESPNOW_SEND_DATA_COMPLETE
    if (   (broadcast_data_to_send.dTypeState==DPID_STATE) 
        && (broadcast_data_to_send.dTypeBattery==DPID_BATTERY) 
        #ifdef ESPNOW_TELEGRAM_EXTENDED
        && (broadcast_data_to_send.dTypeData1==DPID_VALUE1)
        && (broadcast_data_to_send.dTypeData2==DPID_VALUE2)
        && (broadcast_data_to_send.dTypeData3==DPID_VALUE3)
        && (broadcast_data_to_send.dTypeData4==DPID_VALUE4) 
        #endif
      )  {
      printLogMsgTime("ESP!Now: Message: Data: all DPID values received, sending ESP!Now message.\n");
      espnowMessageDataSend();
    }
    #endif //ESPNOW_SEND_DATA_COMPLETE
}
void EspNowSensorClass::espnowMessageDataSetProgram(uint8_t prog) {
  broadcast_data_to_send.program = prog;
}

void EspNowSensorClass::espnowMessageProductInfo() {
  broadcast_data_to_send.dTypeState =   PRODUCT_FAMILY_KEY;
  broadcast_data_to_send.dataState =    PRODUCT_KEY;
  broadcast_data_to_send.dTypeBattery = 0x00;
  broadcast_data_to_send.dataBattery =  0x00;
  #ifdef ESPNOW_TELEGRAM_EXTENDED
  broadcast_data_to_send.dTypeData1 =   settings.useAuthToken;
  broadcast_data_to_send.data1 =        VERSION;
  broadcast_data_to_send.dTypeData2 =   0x00;
  broadcast_data_to_send.data2 =        FRAMEWORK_VERSION;
  broadcast_data_to_send.dTypeData3 =   0x00;
  broadcast_data_to_send.data3 =        WiFi.localIP();
  broadcast_data_to_send.dTypeData4 =   0x00;
  broadcast_data_to_send.data4 =        settings.channel;
  #endif
}

void EspNowSensorClass::espnowMessageDataSend() {
  if (broadcast_data_to_send.program==0x00) broadcast_data_to_send.program = ESPNOW_TELEGRAM_PROGRAM;
  broadcast_data = broadcast_data_to_send;
  espnowMessageClear();
  espnowMessageSend();
}
void EspNowSensorClass::espnowMessageAuthTokenRequest(){
  broadcast_data_to_send.program = 0xFA;
  espnowMessageProductInfo();
  broadcast_data = broadcast_data_to_send;
  espnowMessageClear();
  espnowMessageSend();
}
void EspNowSensorClass::espnowMessageAlive(){
  broadcast_data_to_send.program = 0xAF;
  espnowMessageProductInfo();
  broadcast_data = broadcast_data_to_send;
  espnowMessageClear();
  espnowMessageSend();
}
void EspNowSensorClass::espnowMessageConfig(){
  broadcast_data_to_send.program = 0xC0;
  espnowMessageProductInfo();
  broadcast_data = broadcast_data_to_send;
  espnowMessageClear();
  espnowMessageSend();
}
void EspNowSensorClass::espnowBroadcastMessage(){
  if (broadcastSending) ;
  else if (WiFi.status() == WL_CONNECTED) {
    broadcastSending = true;
    broadcastAllChannels = false;
    broadcastChannel = WiFi.channel();
    broadcastRepeat = settings.broadcastRepeat;
  }
  else if (settings.useAuthToken) {
    broadcastSending = true;
    broadcastAllChannels = false;
    broadcastRepeat = settings.broadcastRepeat;
    if ((settings.channel & 0x3FFF)>0) {
        while (true) {
          if (broadcastChannel>14) broadcastChannel=1;
          int mask = 1<<(broadcastChannel-1);
          if (settings.channel & mask) break;
          else broadcastChannel++;
        }
    }
    else broadcastChannel = settings.defaultChannel;
  }
  else {
    broadcastSending = true;
    broadcastAllChannels = true;
    broadcastChannel = 0;
    broadcastRepeat = 0;
  }

  if ((broadcastRepeat==0) && broadcastAllChannels) {
    broadcastChannel++;
    for (int i = broadcastChannel; i <= 15; i++) {
      int mask = 1<<(i-1);
      if (settings.channel & mask) {
          broadcastChannel = i;
          broadcastRepeat = settings.broadcastRepeat;
          break;
      }
    }
  }

  delayMicroseconds(5000);  
  if (broadcastRepeat>0) {
    if (broadcastRepeat==settings.broadcastRepeat) {
      printLogMsgTime("ESP!Now: Broadcast: Channel: %d , Repeating: %d\n" , broadcastChannel, broadcastRepeat);
      espnowSetChannel(broadcastChannel);
    }
    broadcastRepeat--;
    espnowBroadcast((uint8_t *) &broadcast_data, sizeof(espnow_message_structure_t));
  }
  else {
    delayMicroseconds(10000);
    printLogMsgTime("ESP!Now: Broadcast: Message: Finished\n");
    broadcastSending = false;
  }

}
void EspNowSensorClass::espnowMessageSend(){

      uint32_t seq = nextSequenceNumber();
      u8from32(broadcast_data.seq,seq);
  
      espnowMessageSetupAuthCode();
      String messageTyp;
      if (broadcast_data.program== 0xFA) messageTyp = F("Authentifcation request ");
      else if (broadcast_data.program== 0xAF) messageTyp = F("Sensor alive ");
      else if (broadcast_data.program== 0xA0) messageTyp = F("Sensor data (Tuya) ");
      else if (broadcast_data.program== 0xD0) messageTyp = F("Sensor data (Data) ");
      else if (broadcast_data.program== 0xC0) messageTyp = F("Configuration mode ");
      else if (broadcast_data.program== 0x91) messageTyp = F("WizMote ON ");
      else if (broadcast_data.program== 0x81) messageTyp = F("WizMote key ");
      else if (broadcast_data.program== ESPNOW_TELEGRAM_PROGRAM) messageTyp = F("Sensor data ");
      else messageTyp = F("Unspecified ");
      char dataChar[2];
      sprintf (dataChar, "%02X", broadcast_data.program);
      messageTyp += F("(");
      messageTyp += dataChar;
      messageTyp += F(")");

      printLogMsgTime("ESP!Now: Broadcast: Message: %s , Sequence number: %ld , ", messageTyp.c_str(), seq);
      printLogMsg("MAC: %02X%02X%02X%02X , Token: %02X%02X%02X%02X\n" , broadcast_data.mac [0], broadcast_data.mac [1], broadcast_data.mac [2], broadcast_data.mac [3],authToken[0],authToken[1],authToken[2],authToken[3]);

      espnowBroadcastMessage();
}

//=============================settings
void EspNowSensorClass::initSettings(){
  printLogMsgTime("Info: Settings: Init\n");
  EEPROM.put(EEPROM_CHANNEL , ESPNOW_CHANNEL);
  EEPROM.put(EEPROM_USEAUTHTOKEN , AUTH_TOKEN_REQ);
  EEPROM.put(EEPROM_BROADCASTREPEAT , ESPNOW_REPEAT_SEND);
  EEPROM.put(EEPROM_DEFAULTCHANNEL , 1);
  EEPROM.put(EEPROM_DEEPSLEEP_TIME, DEEPSLEEP_TIME);
  EEPROM.put(EEPROM_CONFIG0, (uint32_t)SETTINGS_CONFIG0_INIT); 
  EEPROM.put(EEPROM_CONFIG1, (uint32_t)SETTINGS_CONFIG1_INIT); 
  EEPROM.put(EEPROM_CONFIG2, (uint32_t)SETTINGS_CONFIG2_INIT); 
  EEPROM.put(EEPROM_CONFIG3, (uint32_t)SETTINGS_CONFIG3_INIT); 
  EEPROM.put(EEPROM_CONFIG4, (uint32_t)SETTINGS_CONFIG4_INIT); 
  EEPROM.put(EEPROM_CONFIG5, (uint32_t)SETTINGS_CONFIG5_INIT); 

  EEPROM.put(EEPROM_INITIALIZED , EEPROM_INITIALIZED_VALUE);

  EEPROM.commit();
}
void EspNowSensorClass::saveEspNowSettings(){
  printLogMsgTime("Info: Settings: ESP!Now Save\n");
  EEPROM.put(EEPROM_CHANNEL , settings.channel);
  EEPROM.put(EEPROM_USEAUTHTOKEN , settings.useAuthToken);
  EEPROM.put(EEPROM_BROADCASTREPEAT , settings.broadcastRepeat);
  EEPROM.put(EEPROM_DEFAULTCHANNEL , settings.defaultChannel);

  EEPROM.commit();
}
void EspNowSensorClass::saveSensorSettings(){
  printLogMsgTime("Info: Settings: Sensor Save\n");
  EEPROM.put(EEPROM_DEEPSLEEP_TIME, settings.deepsleepTime);
  EEPROM.put(EEPROM_CONFIG0, settings.Config); 

  EEPROM.commit();
}
void EspNowSensorClass::loadSettings(){
  uint32_t initialized = 0;
  EEPROM.get(EEPROM_INITIALIZED , initialized);
  if (initialized==EEPROM_INITIALIZED_VALUE) {
    EEPROM.get(EEPROM_CHANNEL , settings.channel);
    EEPROM.get(EEPROM_USEAUTHTOKEN , settings.useAuthToken);
    EEPROM.get(EEPROM_BROADCASTREPEAT , settings.broadcastRepeat);
    EEPROM.get(EEPROM_DEFAULTCHANNEL , settings.defaultChannel);
    EEPROM.get(EEPROM_DEEPSLEEP_TIME, settings.deepsleepTime);
    EEPROM.get(EEPROM_CONFIG0, settings.Config); 
    printLogMsgTime("Info: Settings: Load\n");
  }
  else {
    printLogMsgTime("Info: Settings: Initialize needed\n");
    EEPROM.put(EEPROM_SEQUENCE , 0x0000);
    initSettings();
  }
  if ((settings.channel & 0x3FFF)==0) settings.channel = 1057; // 1 + 6 + 11
  if (settings.broadcastRepeat==0) settings.broadcastRepeat=ESPNOW_REPEAT_SEND;
  if ((settings.defaultChannel==0) || (settings.defaultChannel>14)) settings.defaultChannel=1;

  broadcastChannel = settings.defaultChannel;
}
void EspNowSensorClass::factorySettings(){
  printLogMsgTime("Info: Settings: Factory settings!!\n");
  EEPROM.put(EEPROM_SEQUENCE , 0x0000);
  EEPROM.put(EEPROM_INITIALIZED , 0x0000);
  EEPROM.commit();
  delay(500);
  printLogMsgTime("ESP!Now: Settings: Reboot.\n");
  delay(500);
  #ifdef ESP8266
  system_restart(); 
  #endif 
  #ifdef ESP32 
  esp_restart(); 
  #endif
}

//=============================Value storage
void EspNowSensorClass::storeValue(uint8_t no, uint32_t value){

  int eepromAdr = 0;
  switch (no)
  {
  case 0:
    eepromAdr = EEPROM_VALUE0;
    break;
  case 1:
    eepromAdr = EEPROM_VALUE1;
    break;
  case 2:
    eepromAdr = EEPROM_VALUE2;
    break;
  case 3:
    eepromAdr = EEPROM_VALUE3;
    break;
  default:
    eepromAdr = 0;
    printLogMsgTime("Info: Values: Save: wrong address\n");
    break;
  }
  if (eepromAdr!=0) {
    printLogMsgTime("Info: Values: Save: Value%d=%d\n",no,value);
    EEPROM.put(eepromAdr , value);
    EEPROM.commit();
  }
}
void EspNowSensorClass::loadValues(){
  printLogMsgTime("Info: Values: Load\n");
  EEPROM.get(EEPROM_CONFIG0, values.Value); 
}
void EspNowSensorClass::initValues(){
  uint32_t initValue[4] = {(uint32_t)VALUE_0_INIT, (uint32_t)VALUE_1_INIT, (uint32_t)VALUE_2_INIT, (uint32_t)VALUE_3_INIT};
  EEPROM.put(EEPROM_VALUE0 , initValue);
  EEPROM.commit();
}

//=============================Sequence number
void EspNowSensorClass::setSequenceNumber(uint32_t sequenceNumber) {
  printLogMsgTime("Info: Settings: Set sequence number: %d\n",sequenceNumber);
  EEPROM.put(EEPROM_SEQUENCE , sequenceNumber);
  EEPROM.commit();
}
uint32_t EspNowSensorClass::getSequenceNumber() {
  uint32_t sequenceNumber=0;
  EEPROM.get(EEPROM_SEQUENCE , sequenceNumber);
  return sequenceNumber;
}
uint32_t EspNowSensorClass::nextSequenceNumber() {
  uint32_t sequenceNumber=0;
  EEPROM.get(EEPROM_SEQUENCE , sequenceNumber);

  sequenceNumber++;

  EEPROM.put(EEPROM_SEQUENCE , sequenceNumber);
  EEPROM.commit();

  return sequenceNumber;
}

//=============================Battery level
uint8_t EspNowSensorClass::batteryLevel(){
  return getBatteryLevel();
}

//=============================Webserver
String EspNowSensorClass::webserverGetPageRoot(){
  char buffer[100];
  //String page = "<html lang=fr-FR><head><meta http-equiv='refresh' content='10'/>";
  String page = "<html lang=en-US><head>";
  page += "<title>";
  page += EspNowSensor.DeviceName.c_str();
  page += "</title>";
  page += "<style> body { background-color: #fffff; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }</style>";
  page += "</head><body><h1>";
  page += PRODUCT;
  page += "</h1>";
  page += "<h2>";
  page += EspNowSensor.DeviceName.c_str();
  page += "</h2>";
  sprintf(buffer,"Version V%X.%X.%X (%s)<br>", (VERSION >> 16) & 0xFF, (VERSION >> 8) & 0xFF, VERSION & 0xFF, OWNER);
  page += buffer;
  sprintf(buffer,"%s V%X.%X.%X<br>", FRAMEWORK,(FRAMEWORK_VERSION >> 16) & 0xFF, (FRAMEWORK_VERSION >> 8) & 0xFF, FRAMEWORK_VERSION & 0xFF);
  page += buffer;
  sprintf(buffer,"%s<br>", DESCRIPTION);
  page += buffer;
  sprintf(buffer,"MAC: %s<br>", WiFi.macAddress().c_str());
  page += buffer;
  sprintf(buffer,"MCU: %s<br>", MCU_TYPE);
  page += buffer;

  page += "<h3>Network:</h3>";
  String ipaddress = WiFi.localIP().toString();
  sprintf(buffer,"SSID: %s<br>" , WiFi.SSID().c_str());
  page += buffer;
  sprintf(buffer,"IP: %s<br>", ipaddress.c_str());
  page += buffer;
  sprintf(buffer,"Wifi channel: %d<br>", WiFi.channel());
  page += buffer;

  page += "<h3>ESP!Now:</h3>";
  sprintf(buffer,"Authentification Key: %s<br><br>", AUTHENTIFICATION_KEY);  
  page += buffer;
  page += "<form action='/espnowsettings' method='POST'>"; 
  page += "<label for='espnowCh'>Wifi channels (enabled): </label>";
  sprintf(buffer,"<INPUT type='number' name='espnowCh' id='espnowCh' value='%d' min='1' max='65535'><br>" , EspNowSensor.settings.channel);
  page += buffer;
  page += "<label for='espnowChDefault'>Wifi channel (default): </label>";
  sprintf(buffer,"<INPUT type='number' name='espnowChDefault' id='espnowChDefault' value='%d' min='1' max='14'><br>" , EspNowSensor.settings.defaultChannel);
  page += buffer;
  page += "<label for='espnowAuth'>Use authenification token: </label>";
  sprintf(buffer,"<INPUT type='number' name='espnowAuth' id='espnowAuth' value='%d' min='0' max='1'><br>" , EspNowSensor.settings.useAuthToken);
  page += buffer;
  page += "<label for='espnowRepeat'>Broadcast repeat: </label>";
  sprintf(buffer,"<INPUT type='number' name='espnowRepeat' id='espnowRepeat' value='%d' min='1' max='10'><br>" , EspNowSensor.settings.broadcastRepeat);
  page += buffer;
  page += "<INPUT type='submit' value='Submit'>";
  page += "</form>";

  page += "<form action='/seqnum' method='POST'>"; 
  page += "<label for='espnowseq'>Sequence number: </label>";
  sprintf(buffer,"<INPUT type='number' name='espnowSeq' id='espnowseq' value='%d' min='0' max='4294967295'>" , EspNowSensor.getSequenceNumber());
  page += buffer;
  page += "<INPUT type='submit' value='Set'>";
  page += "</form>";

  #if (defined POWER_OFF_DEEPSLEEP || defined SETTINGS_CONFIG0 || defined SETTINGS_CONFIG1 || defined SETTINGS_CONFIG2 || defined SETTINGS_CONFIG3 || defined SETTINGS_CONFIG4 || defined SETTINGS_CONFIG5)
    page += "<h3>Settings:</h3>";
    page += "<form action='/sensorsettings' method='POST'>"; 
    #ifdef POWER_OFF_DEEPSLEEP
      page += "<label for='deepsleepTime'>Deep sleep time [s]: </label>";
      sprintf(buffer,"<INPUT type='number' name='deepsleepTime' id='deepsleepTime' value='%d' min='0' max='252000'><br>" , EspNowSensor.settings.deepsleepTime);
      page += buffer;
    #endif
    #ifdef SETTINGS_CONFIG0
      page += "<label for='settings0'>";
      page += SETTINGS_CONFIG0_NAME;
      page += ":</label>";
      sprintf(buffer,"<INPUT type='number' name='settings0' id='settings0' value='%d'><br>" , EspNowSensor.settings.Config[0]);
      page += buffer;
    #endif
    #ifdef SETTINGS_CONFIG1
      page += "<label for='settings1'>";
      page += SETTINGS_CONFIG1_NAME;
      page += ":</label>";
      sprintf(buffer,"<INPUT type='number' name='settings1' id='settings1' value='%d'><br>" , EspNowSensor.settings.Config[1]);
      page += buffer;
    #endif
    #ifdef SETTINGS_CONFIG2
      page += "<label for='settings2'>";
      page += SETTINGS_CONFIG2_NAME;
      page += ":</label>";
      sprintf(buffer,"<INPUT type='number' name='settings2' id='settings2' value='%d'><br>" , EspNowSensor.settings.Config[2]);
      page += buffer;
    #endif
    #ifdef SETTINGS_CONFIG3
      page += "<label for='settings3'>";
      page += SETTINGS_CONFIG3_NAME;
      page += ":</label>";
      sprintf(buffer,"<INPUT type='number' name='settings3' id='settings3' value='%d'><br>" , EspNowSensor.settings.Config[3]);
      page += buffer;
    #endif
    #ifdef SETTINGS_CONFIG4
      page += "<label for='settings4'>";
      page += SETTINGS_CONFIG4_NAME;
      page += ":</label>";
      sprintf(buffer,"<INPUT type='number' name='settings4' id='settings4' value='%d'><br>" , EspNowSensor.settings.Config[4]);
      page += buffer;
    #endif
    #ifdef SETTINGS_CONFIG5
      page += "<label for='settings5'>";
      page += SETTINGS_CONFIG5_NAME;
      page += ":</label>";
      sprintf(buffer,"<INPUT type='number' name='settings5' id='settings5' value='%d'><br>" , EspNowSensor.settings.Config[5]);
      page += buffer;
    #endif
    page += "<INPUT type='submit' value='Submit'>";
    page += "</form>";
  #endif
  page += "<br>";
  page += "<form action='/reboot' method='POST'>"; 
  page += "<INPUT type='submit' value='Reboot'>";
  page += "</form>";
  page += "<form action='/exit' method='POST'>"; 
  page += "<INPUT type='submit' value='Exit'>";
  page += "</form>";
  page += "<form action='/poweroff' method='POST'>"; 
  page += "<INPUT type='submit' value='Power off'>";
  page += "</form>";

  page += "</body></html>";
  return page;
}
void EspNowSensorClass::webserverHandleSubmit(){
  String Tsetarduino = server.arg(0);
  String message = "URI: ";
  message += server.uri();
  message += ", Method: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += ", Arguments: ";
  message += server.args();
  message += ": ";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + ", ";
  }
  printLogMsgTime("Info: Webserver: Submit: %s\n", message.c_str());

  for (uint8_t i=0; i<server.args(); i++){
    if (server.argName(i)=="espnowCh") EspNowSensor.settings.channel = server.arg(i).toInt();
    else if (server.argName(i)=="espnowChDefault") EspNowSensor.settings.defaultChannel = server.arg(i).toInt();
    else if (server.argName(i)=="espnowAuth") EspNowSensor.settings.useAuthToken = (uint8_t)(server.arg(i).toInt()!=0);
    else if (server.argName(i)=="espnowRepeat") EspNowSensor.settings.broadcastRepeat = server.arg(i).toInt();
    else if (server.argName(i)=="espnowSeq") EspNowSensor.setSequenceNumber(server.arg(i).toInt());
    else if (server.argName(i)=="deepsleepTime") EspNowSensor.settings.deepsleepTime = (uint32_t)server.arg(i).toInt();
    else if (server.argName(i)=="settings0") EspNowSensor.settings.Config[0] = (uint32_t)server.arg(i).toInt();
    else if (server.argName(i)=="settings1") EspNowSensor.settings.Config[1] = (uint32_t)server.arg(i).toInt();
    else if (server.argName(i)=="settings2") EspNowSensor.settings.Config[2] = (uint32_t)server.arg(i).toInt();
    else if (server.argName(i)=="settings3") EspNowSensor.settings.Config[3] = (uint32_t)server.arg(i).toInt();
    else if (server.argName(i)=="settings4") EspNowSensor.settings.Config[4] = (uint32_t)server.arg(i).toInt();
    else if (server.argName(i)=="settings5") EspNowSensor.settings.Config[5] = (uint32_t)server.arg(i).toInt();
  }
}
void EspNowSensorClass::webserverSetup(){
  server.onNotFound([](){
    server.send(404, "text/plain", "page is missing");  
    printLogMsgTime("Info: Webserver: Page is missing\n");
  });
  server.on("/", []() {
    server.send(200, "text/html", EspNowSensor.webserverGetPageRoot());
    printLogMsgTime("Info: Webserver: root page\n");
  });
  server.on("/espnowsettings", []() {
    printLogMsgTime("Info: Webserver: ESP!Now settings\n");
    EspNowSensor.webserverHandleSubmit();
    EspNowSensor.saveEspNowSettings();
    server.send ( 200, "text/html", EspNowSensor.webserverGetPageRoot() );
  });
  server.on("/sensorsettings", []() {
    printLogMsgTime("Info: Webserver: Sensor settings\n");
    EspNowSensor.webserverHandleSubmit();
    EspNowSensor.saveSensorSettings();
    server.send ( 200, "text/html", EspNowSensor.webserverGetPageRoot() );
  });
  server.on("/seqnum", []() {
    printLogMsgTime("Info: Webserver: ESP!Now sequence number\n");
    EspNowSensor.webserverHandleSubmit();
    server.send ( 200, "text/html", EspNowSensor.webserverGetPageRoot() );
  });
  server.on("/reboot", []() {
    printLogMsgTime("Info: Webserver: Reboot\n");
    #ifdef ESP8266
    system_restart(); 
    #endif 
    #ifdef ESP32 
    esp_restart(); 
    #endif
    delay(500);
  });
  server.on("/exit", []() {
    printLogMsgTime("Info: Webserver: Exit configuration mode\n");
    EspNowSensor.configmodeLeave();
  });
  server.on("/poweroff", []() {
    printLogMsgTime("Info: Webserver: Power off\n");
    EspNowSensor.powerOff();
  });

  server.begin();
  delay(1000);
  printLogMsgTime("Info: Webserver: started.\n");
}