#include <Arduino.h>
#include <lib/authMac.h>

uint32_t u8tou32(uint8_t b[4]){
    uint32_t u;
    u = b[0];
    u = (u  << 8) + b[1];
    u = (u  << 8) + b[2];
    u = (u  << 8) + b[3];
    return u;
  }
  void u8from32 (uint8_t b[4], uint32_t u32) {
      b[3] = (uint8_t)u32;
      b[2] = (uint8_t)(u32>>=8);
      b[1] = (uint8_t)(u32>>=8);
      b[0] = (uint8_t)(u32>>=8);
  }
  void calculateMAC(byte * msg, int len, uint8_t mac[4], byte key[AUTH_KEY_LENGTH]) {                      
                      SHA256HMAC hmac(key, AUTH_KEY_LENGTH);
                      hmac.doUpdate(msg, len);          
                      byte authCode[SHA256HMAC_SIZE];
                      hmac.doFinal(authCode);      
  
                      u8from32(mac, calcCRC32(authCode, sizeof(authCode)));
  }