#include <Arduino.h>
#include <configuration.h>
#include <lib/logging.h>

#include <Crypto.h>
#include <CRC.h>

uint32_t u8tou32(uint8_t b[4]);

void u8from32 (uint8_t b[4], uint32_t u32);

void calculateMAC(byte * msg, int len, uint8_t mac[4], byte key[AUTH_KEY_LENGTH]);