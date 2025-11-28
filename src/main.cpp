#include <Arduino.h>
#include <FS.h>
#include <config/configuration.h>
#include <EEPROM.h>

#include <lib/logging.h>

#include <lib/EspNowSensor.h>
#include <lib/ota.h>

#ifdef ESP32
  #include <esp_wifi.h>
  #include <ESP32WebServer.h>
  ESP32WebServer server(80);
#endif
#ifdef ESP8266
  #include <ESP8266WebServer.h>
  ESP8266WebServer server(80);
#endif

//=============================Creditials
const char* wifiSSID = WIFI_SSID;
const char* wifiPassword = WIFI_PASSWORD;

String DeviceName;
//=============================Settings + Config
bool      configmode = false;
unsigned long configModeTime;

//=============================ESP!Now
    EspNowSensorClass EspNowSensor;

//=============================Prototypes
void webserverSetup();

//=============================ESP!Now
void OnDataSent(
    #ifdef ESP8266 
    uint8_t *mac, uint8_t sendStatus
    #endif
    #ifdef ESP32
    const uint8_t *mac_addr, esp_now_send_status_t status
    #endif
)
{
    #ifdef ESP32
    uint8_t sendStatus = (uint8_t)status;
    uint8_t mac[6];
    mac[0] = mac_addr[0];
    mac[1] = mac_addr[1];
    mac[2] = mac_addr[2];
    mac[3] = mac_addr[3];
    mac[4] = mac_addr[4];
    mac[5] = mac_addr[5];
    #endif

    EspNowSensor.OnDataSent(mac,sendStatus);
}
void OnDataRecv(
                #ifdef ESP8266
                uint8_t * mac, uint8_t *incomingData, uint8_t len
                #endif
                #ifdef ESP32
                const esp_now_recv_info *esp_now_info, const uint8_t *incomingData, int len
                #endif
)
{
  #ifdef ESP32
  uint8_t mac[6];
  mac[0] = esp_now_info->src_addr[0];
  mac[1] = esp_now_info->src_addr[1];
  mac[2] = esp_now_info->src_addr[2];
  mac[3] = esp_now_info->src_addr[3];
  mac[4] = esp_now_info->src_addr[4];
  mac[5] = esp_now_info->src_addr[5];
  #endif
      
  EspNowSensor.OnDataRecv(mac,incomingData,len);
}

//=============================Webserver
String webserverGetPageRoot(){
  char buffer[100];
  //String page = "<html lang=fr-FR><head><meta http-equiv='refresh' content='10'/>";
  String page = "<html lang=en-US><head>";
  page += "<title>";
  page += DeviceName.c_str();
  page += "</title>";
  page += "<style> body { background-color: #fffff; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }</style>";
  page += "</head><body><h1>";
  page += PRODUCT;
  page += "</h1>";
  page += "<h2>";
  page += DeviceName.c_str();
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
  page += "<form action='/submit' method='POST'>"; 
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
  #ifdef POWER_OFF_DEEPSLEEP
  page += "<label for='deepsleepTime'>Deep sleep time [s]: </label>";
  sprintf(buffer,"<INPUT type='number' name='deepsleepTime' id='deepsleepTime' value='%d' min='0' max='252000'><br>" , EspNowSensor.settings.deepsleepTime);
  page += buffer;
  #endif
  page += "<INPUT type='submit' value='Submit'>";
  page += "</form>";

  page += "<form action='/seqnum' method='POST'>"; 
  page += "<label for='espnowseq'>Sequence number: </label>";
  sprintf(buffer,"<INPUT type='number' name='espnowSeq' id='espnowseq' value='%d' min='0' max='4294967295'>" , EspNowSensor.getSequenceNumber());
  page += buffer;
  page += "<INPUT type='submit' value='Set'>";
  page += "</form>";
  
  page += "<br>";
  page += "<form action='/reboot' method='POST'>"; 
  page += "<INPUT type='submit' value='Reboot'>";
  page += "</form>";

  page += "</body></html>";
  return page;
}

void webserverHandleSubmit(){
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
  }
}

void webserverSetup(){

  server.onNotFound([](){
    server.send(404, "text/plain", "page is missing");  
    printLogMsgTime("Info: Webserver: Page is missing\n");
  });
  server.on("/", []() {
    server.send(200, "text/html", webserverGetPageRoot());
    printLogMsgTime("Info: Webserver: root page\n");
  });
  server.on("/submit", []() {
    printLogMsgTime("Info: Webserver: Settings ..\n");
    webserverHandleSubmit();
    EspNowSensor.saveSettings();
    server.send ( 200, "text/html", webserverGetPageRoot() );
  });
  server.on("/seqnum", []() {
    printLogMsgTime("Info: Webserver: ESP!Now sequence number submit\n");
    webserverHandleSubmit();
    server.send ( 200, "text/html", webserverGetPageRoot() );
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
  server.begin();
  delay(1000);
  printLogMsgTime("Info: Webserver: started.\n");
}

//=============================Misc
void VersionInfo(){
  printLogMsg("%s\n",DeviceName.c_str());
  printLogMsg("%s V%X.%X.%X (%s) - %s\n",PRODUCT, (VERSION >> 16) & 0xFF, (VERSION >> 8) & 0xFF, VERSION & 0xFF, OWNER, DESCRIPTION);
  printLogMsg("%s V%X.%X.%X\n",FRAMEWORK, (FRAMEWORK_VERSION >> 16) & 0xFF, (FRAMEWORK_VERSION >> 8) & 0xFF, FRAMEWORK_VERSION & 0xFF);
  printLogMsg("MAC: %s\n", WiFi.macAddress().c_str());
  printLogMsg("MCU: %s\n", MCU_TYPE);
  printLogMsg("BUILD: %X\n", BUILD);

  printLogMsg("Authentification Key: %s\n", AUTHENTIFICATION_KEY);  
  printLogMsg("\n");
}

void powerOff() {

    #ifdef ACTIVE_PIN
    digitalWrite(ACTIVE_PIN, !ACTIVE_PIN_POLARITY);
    #endif

    #ifdef VOLTAGE_REGULATOR_PIN
      printLogMsgTime("PowerOff: Voltage regulator shutdown" );
      delay(100);
      digitalWrite(VOLTAGE_REGULATOR_PIN, !VOLTAGE_REGULATOR_POLARITY);
    #endif

    #ifdef POWER_OFF_DEEPSLEEP
      uint64_t duration = EspNowSensor.settings.deepsleepTime * 1000000;
      if (duration>ESP.deepSleepMax()) duration=ESP.deepSleepMax();
      String durationStr;
      if (EspNowSensor.settings.deepsleepTime==0)  durationStr += "Wakeup: Reset";
      else {
        durationStr += "Wakeup: ";
        durationStr += (int32_t)(duration / 1000000);
        durationStr += "s";
      }
      printLogMsgTime("PowerOff: Deepsleep: %s", durationStr.c_str());
      delay(100);
      ESP.deepSleep(duration);
    #endif
}

void setupCustomMAC()
{
  #ifdef CUSTOM_MAC_ADDRESS
  printLogMsgTime("Info: Setting custom MAC address: %s\n" ,CUSTOM_MAC_ADDRESS);
  uint8_t mac[6];
  const char cutomMac[18] = {CUSTOM_MAC_ADDRESS};
  char str[2];
  str[0] = cutomMac[0];
  str[1] = cutomMac[1];
  mac[0] = (uint8_t) strtol(str, 0, 16);
  str[0] = cutomMac[3];
  str[1] = cutomMac[4];
  mac[1] = (uint8_t) strtol(str, 0, 16);     
  str[0] = cutomMac[6];
  str[1] = cutomMac[7];
  mac[2] = (uint8_t) strtol(str, 0, 16);
  str[0] = cutomMac[9];
  str[1] = cutomMac[10];
  mac[3] = (uint8_t) strtol(str, 0, 16);                        
  str[0] = cutomMac[12];
  str[1] = cutomMac[13];
  mac[4] = (uint8_t) strtol(str, 0, 16);
  str[0] = cutomMac[15];
  str[1] = cutomMac[16];
  mac[5] = (uint8_t) strtol(str, 0, 16);
  wifi_set_macaddr(0, const_cast<uint8*>(mac));   //This line changes MAC adderss of ESP8266
  #endif
}

void setupDeviceName(){
  char buffer[100];
  uint8_t mac[6];
  WiFi.macAddress(mac);
  sprintf(buffer,"%s_%02X%02X%02X%02X%02X%02X",PRODUCT_ID, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  DeviceName = buffer;
}

void setup_wifi() {
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
  WiFi.hostname(DeviceName.c_str());
  #endif
  #ifdef ESP32
  const char* hostname = DeviceName.c_str();
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

//=============================Main
void setup() {
  
  unsigned long startTime = millis();

  LoggingBegin();

  if (WiFi.mode(WIFI_STA) != true) {
      printLogMsg("setting Wi-Fi mode failed\n");
  }
  printLogMsg("\n\n");
  setupDeviceName();

  VersionInfo();

  printLogMsg("%ld  Info: Startup\n", startTime);

  setupCustomMAC();

    #ifdef ACTIVE_PIN
    pinMode(ACTIVE_PIN, OUTPUT);
    digitalWrite(ACTIVE_PIN, ACTIVE_PIN_POLARITY);
    #endif

    #ifdef VOLTAGE_REGULATOR_PIN
    // Set the voltage regulator pin as an output pin
    pinMode(VOLTAGE_REGULATOR_PIN, OUTPUT);
    // Enable the voltage regulator, so the remote stays awake
    digitalWrite(VOLTAGE_REGULATOR_PIN, VOLTAGE_REGULATOR_POLARITY);
    #endif


  EspNowSensor.begin();

  EspNowSensor.registerSendCallback(OnDataSent);
  EspNowSensor.registerRecvCallback(OnDataRecv);

  #ifdef SETUP_PIN
    pinMode(SETUP_PIN, INPUT_PULLUP);
  #endif
  #ifdef SHUTDOWN_PIN
    pinMode(SHUTDOWN_PIN, INPUT_PULLUP);
  #endif
}

void loop() {

  #ifdef SETUP_PIN
    if (digitalRead(SETUP_PIN)==SETUP_PIN_POLARITY) {
      while (digitalRead(SETUP_PIN)==SETUP_PIN_POLARITY) ;// wait for Button released.
      if (!configmode){
        printLogMsgTime("Info: Button: Config mode enter\n" );
        setup_wifi();
        configmode = true;
        EspNowSensor.espnowMessageConfig();
      }
      else {
        printLogMsgTime("Info: Button: Config mode leave\n"); 
        configmode = false;  
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
    if( ((millis()-configModeTime)>CONFIG_MODE_TIMEOUT) && (CONFIG_MODE_TIMEOUT>0) ) {
      printLogMsgTime("Info: Config: Time out\n"); 
      configmode = false;  
    }
  }

  EspNowSensor.espnowAuthCheck();

  printLogMsgIdle();


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
      if ( (millis()>SHUTDOWN_TIMER) ){
          printLogMsgTime("Info: Timer: Shutdown request\n" );
          delay(100);
          powerOff();
      }
    #endif
  }

  delay(10);    // Add a small delay to avoid overwhelming the CPU
}
