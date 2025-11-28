#include <lib/EspNowSensor.h>
#ifdef ESP32
  #include <esp_wifi.h>
#endif

uint8_t EspNowSensorClass::broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

EspNowSensorClass::EspNowSensorClass() {}

void EspNowSensorClass::begin() {
    // Prevent calling this method a second time
    if (initialized) {
        return;
    }

    EEPROM.begin(EEPROM_SIZE);

    loadSettings();
    loadValues();

    espnowinitialize();

    initialized = true;
}

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
        saveSettings();
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
      broadcast_data_to_send.dataState = b[3]==DPID_STATE_POLARITY;
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
        && (broadcast_data_to_send.dTypeData1==DPID_VALUE1)
        && (broadcast_data_to_send.dTypeData2==DPID_VALUE2)
        && (broadcast_data_to_send.dTypeData3==DPID_VALUE3)
        && (broadcast_data_to_send.dTypeData4==DPID_VALUE4) )  {
      printLogMsgTime("ESP!Now: Message: Data: all DPID values received, sending ESP!Now message.\n");
      espnowMessageDataSend();
    }
    #endif //ESPNOW_SEND_DATA_COMPLETE
}

void EspNowSensorClass::espnowMessageDataSend() {
  broadcast_data_to_send.program = 0xa0;
  broadcast_data = broadcast_data_to_send;
  espnowMessageClear();
  espnowMessageSend();
}

void EspNowSensorClass::espnowMessageAuthTokenRequest(){
  broadcast_data_to_send.program = 0xFA;
  #ifdef ESPNOW_TELEGRAM_EXTENDED
  broadcast_data_to_send.dTypeData1 =  PRODUCT_KEY;
  broadcast_data_to_send.data1 =  VERSION;
  broadcast_data_to_send.data3 =  FRAMEWORK_VERSION;
  #endif
  broadcast_data = broadcast_data_to_send;
  espnowMessageClear();
  espnowMessageSend();
}

void EspNowSensorClass::espnowMessageAlive(){
  broadcast_data_to_send.program = 0xAF;
  #ifdef ESPNOW_TELEGRAM_EXTENDED
  broadcast_data_to_send.dTypeData1 =  PRODUCT_KEY;
  broadcast_data_to_send.data1 =  VERSION;
  broadcast_data_to_send.data3 =  FRAMEWORK_VERSION;
  #endif
  broadcast_data = broadcast_data_to_send;
  espnowMessageClear();
  espnowMessageSend();
}

void EspNowSensorClass::espnowMessageConfig(){
  broadcast_data_to_send.program = 0xC0;
  #ifdef ESPNOW_TELEGRAM_EXTENDED
  broadcast_data_to_send.dTypeData1 =  PRODUCT_KEY;
  broadcast_data_to_send.data1 =  VERSION;
  broadcast_data_to_send.dTypeData2 =  settings.channel;
  broadcast_data_to_send.data2 =  WiFi.localIP();
  broadcast_data_to_send.data3 =  FRAMEWORK_VERSION;
  #endif
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
      else if (broadcast_data.program== 0xA0) messageTyp = F("Sensor data ");
      else if (broadcast_data.program== 0xA0) messageTyp = F("Configuration mode ");
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
void EspNowSensorClass::saveSettings(){
  printLogMsgTime("ESP!Now:  Settings: Save\n");
  EEPROM.put(EEPROM_CHANNEL , settings.channel);
  EEPROM.put(EEPROM_USEAUTHTOKEN , settings.useAuthToken);
  EEPROM.put(EEPROM_BROADCASTREPEAT , settings.broadcastRepeat);
  EEPROM.put(EEPROM_DEFAULTCHANNEL , settings.defaultChannel);
  EEPROM.put(EEPROM_DEEPSLEEP_TIME, settings.deepsleepTime);
  EEPROM.put(EEPROM_CONFIG0, settings.Config); 
  EEPROM.put(EEPROM_INITIALIZED , EEPROM_INITIALIZED_VALUE);

  EEPROM.commit();
}
void EspNowSensorClass::loadSettings(){
  uint16_t initialized = 0;
  EEPROM.get(EEPROM_INITIALIZED , initialized);
  if (initialized==EEPROM_INITIALIZED_VALUE) {
    EEPROM.get(EEPROM_CHANNEL , settings.channel);
    EEPROM.get(EEPROM_USEAUTHTOKEN , settings.useAuthToken);
    EEPROM.get(EEPROM_BROADCASTREPEAT , settings.broadcastRepeat);
    EEPROM.get(EEPROM_DEFAULTCHANNEL , settings.defaultChannel);
    EEPROM.get(EEPROM_DEEPSLEEP_TIME, settings.deepsleepTime);
    EEPROM.get(EEPROM_CONFIG0, settings.Config); 
    printLogMsgTime("ESP!Now: Settings: Load\n");
  }
  else {
    printLogMsgTime("ESP!Now: Settings: Initialize\n");
    EEPROM.put(EEPROM_SEQUENCE , 0x0000);
    saveSettings();
  }
  if ((settings.channel & 0x3FFF)==0) settings.channel = 1057; // 1 + 6 + 11
  if (settings.broadcastRepeat==0) settings.broadcastRepeat=ESPNOW_REPEAT_SEND;
  if ((settings.defaultChannel==0) || (settings.defaultChannel>14)) settings.defaultChannel=1;

  broadcastChannel = settings.defaultChannel;
}
void EspNowSensorClass::factorySettings(){
  printLogMsgTime("ESP!Now: Settings: Factory settings!!\n");
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
    printLogMsgTime("ESP!Now:  Values: Save: wrong address\n");
    break;
  }
  if (eepromAdr!=0) {
    printLogMsgTime("ESP!Now:  Values: Save: Value%d=%d\n",no,value);
    EEPROM.put(EEPROM_VALUE0 , value);
    EEPROM.commit();
  }
}
void EspNowSensorClass::loadValues(){
  printLogMsgTime("ESP!Now: Values: Load\n");
  EEPROM.get(EEPROM_CONFIG0, values.Value); 
}
void EspNowSensorClass::initValues(){
  uint32_t initValue[4] = {(uint32_t)VALUE0, (uint32_t)VALUE1, (uint32_t)VALUE2, (uint32_t)VALUE3};
  EEPROM.put(EEPROM_VALUE0 , initValue);
  EEPROM.commit();
}

//=============================Sequence number
void EspNowSensorClass::setSequenceNumber(uint32_t sequenceNumber) {
  printLogMsgTime("ESP!Now: Settings: Set sequence number: %d\n",sequenceNumber);
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