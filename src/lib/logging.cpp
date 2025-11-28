#include <lib/logging.h>

#if (ESP8266 && SERIAL_DEBUG_PORT==1)
SoftwareSerial SerialDebug( SERIAL_DEBUG_RX_PIN, SERIAL_DEBUG_TX_PIN );
#endif //#ifdef ESP8266

#if ( (defined ESP32 && defined ESP32C3) || (defined ESP32 && defined ESP32C2))
HardwareSerial SerialDebug(SERIAL_DEBUG_PORT);
#endif

unsigned long logMessageIdleTime;
bool logMessageIdle = false;

void LoggingBegin(){
    #if (ESP8266 && SERIAL_DEBUG_PORT==0)
    Serial.begin(BAUD_RATE_DEBUG);
    #endif
    #if (ESP8266 && SERIAL_DEBUG_PORT==1)
    SerialDebug.begin(BAUD_RATE_DEBUG);
    #endif
    #if (defined ESP32C3 || defined ESP32C2)
    SerialDebug.begin(BAUD_RATE_DEBUG, SERIAL_8N1, SERIAL_DEBUG_RX_PIN, SERIAL_DEBUG_TX_PIN);
    #endif   
    logMessageIdleTime = millis();
}

void printLogMsgTime(const char *format, ...){
    va_list args;
    va_start(args, format);
        // Referenced from https://stackoverflow.com/questions/436367/best-way-to-safely-printf-to-a-string
        size_t sz = vsnprintf(nullptr, 0, format, args);
        size_t bufsize = sz + 1;
        char *buf = (char *)malloc(bufsize);
        vsnprintf(buf, bufsize, format, args);
        //buf[bufsize - 1] = '\0'; // This line is not necessary, check the official documentation of vsnprintf for proof.
    va_end(args);
    String message = buf;
    #if (ESP8266 && SERIAL_DEBUG_PORT==0)
    if (logMessageIdle)  Serial.printf("\n");
    Serial.printf("%ld  %s" , millis(), message.c_str());
    #else
    if (logMessageIdle)  SerialDebug.printf("\n");
    SerialDebug.printf("%ld  %s" , millis(), message.c_str());
    #endif
    logMessageIdleTime = millis();
    logMessageIdle = false;
}

void printLogMsg(const char *format, ...){
    va_list args;
    va_start(args, format);
        // Referenced from https://stackoverflow.com/questions/436367/best-way-to-safely-printf-to-a-string
        size_t sz = vsnprintf(nullptr, 0, format, args);
        size_t bufsize = sz + 1;
        char *buf = (char *)malloc(bufsize);
        vsnprintf(buf, bufsize, format, args);
        //buf[bufsize - 1] = '\0'; // This line is not necessary, check the official documentation of vsnprintf for proof.
    va_end(args);
    String message = buf;
    #if (ESP8266 && SERIAL_DEBUG_PORT==0)
    Serial.printf("%s", message.c_str());
    #else
    SerialDebug.printf("%s", message.c_str());
    #endif
    logMessageIdleTime = millis();
    logMessageIdle = false;
}

void printLogMsgIdle(){
  if ( ((millis()-logMessageIdleTime)>MESSAGE_IDLE_INTERVAL) && (MESSAGE_IDLE_INTERVAL>0) )
  {
    logMessageIdle = true;
    logMessageIdleTime = millis();
    #if (ESP8266 && SERIAL_DEBUG_PORT==0)
    Serial.printf(".");
    #else
    SerialDebug.printf(".");
    #endif
  }
}
