#include <configuration.h>

#include <lib/logging.h>

#include <lib/EspNowSensor.h>

uint8_t   dataBatteryLevel = 0;

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

//=============================Main
void setup() {
  EspNowSensor.begin();
  EspNowSensor.registerSendCallback(OnDataSent);
  EspNowSensor.registerRecvCallback(OnDataRecv);

  dataBatteryLevel = EspNowSensor.batteryLevel();
}

void loop() {
  EspNowSensor.configmodeHandle();
  if (!EspNowSensor.configmode) EspNowSensor.espnowAuthCheck();

  //Sensor user application

  EspNowSensor.shutDownCheck();
  printLogMsgIdle();
  delay(10);    // Add a small delay to avoid overwhelming the CPU
}
