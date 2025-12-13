// This is the baud rate the serial interface will use for communication.
#ifdef ESP8266
#include <SoftwareSerial.h>
#endif
#ifdef ESP32
#include <HardwareSerial.h>
#endif

#define MESSAGE_IDLE_INTERVAL       1000

#include <configuration.h>

void LoggingBegin();

void printLogMsgTime(const char *format, ...);
void printLogMsg(const char *format, ...);

void printLogMsgIdle();