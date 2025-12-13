#include <lib/ota.h>


    unsigned int OtaProgress=0;
    bool OtaStarted=false;

    void ArduinoOtaStart() {
        // setup for OTA
        const String myhostname = F("EspNowSensor");
        ArduinoOTA.setHostname(myhostname.c_str()); // weird: this decides the mDNS name
        ArduinoOTA.onStart([]()
        {
            // printLogMsgTime("OTA: Start\n" );
        });
        ArduinoOTA.onEnd([]()
        {
            // printLogMsg("\n%ld  OTA: End\n" );
        });
        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
        {
            unsigned int progressCalc = (progress / (total / 100));
            if (!OtaStarted) printLogMsgTime("OTA: Update Progress: ");
            if ( (progressCalc!=OtaProgress) && ((progressCalc % 5)==0) ) printLogMsg("%u%% " , (progress / (total / 100)));

            OtaStarted = true; 
            OtaProgress = progressCalc;

            #ifdef ACTIVE_PIN
                digitalWrite(ACTIVE_PIN, ( (progressCalc % 2)==0) );         //blink led very while ota in progress
            #endif
        });
        ArduinoOTA.onError([](ota_error_t error)
        {
            printLogMsgTime("OTA:  Error[%u]: " , error);
            String error_message;
            switch (error)
            {
                case OTA_AUTH_ERROR:
                    error_message = "OTA auth failed";
                    break;

                case OTA_BEGIN_ERROR:
                    error_message = "OTA begin failed";
                    break;

                case OTA_CONNECT_ERROR:
                    error_message = "OTA connect failed";
                    break;

                case OTA_RECEIVE_ERROR:
                    error_message = "OTA receive failed";
                    break;

                case OTA_END_ERROR:
                    error_message = "OTA end failed";
                    break;

                default:
                    error_message = "Unknown OTA error";
                    break;
            }
            printLogMsg("%s\n",error_message.c_str());

        });
        ArduinoOTA.setPassword((const char *)AUTHENTIFICATION_KEY);
        ArduinoOTA.begin();
    }