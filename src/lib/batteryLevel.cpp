#include <configuration.h>
#include <lib/batteryLevel.h>
#include <lib/logging.h>

uint8_t getBatteryLevel()
{
    uint8_t batteryLevel = 100;
//    printLogMsgTime("Info: Battery level: starting..\n");
    #ifdef BATTERY_VOLTAGE_ADC_PIN
        #ifdef BATTERY_VOLTAGE_ON_PIN
            pinMode(BATTERY_VOLTAGE_ON_PIN, OUTPUT);
            digitalWrite(BATTERY_VOLTAGE_ON_PIN, BATTERY_VOLTAGE_ON_POLARITY);
        #endif

        delay(BATTERY_MEASURE_DELAY);

        #ifdef ESP8266
        int adc_value = (BATTERY_VOLTAGE_ADC_PIN > 0) ? (analogRead(BATTERY_VOLTAGE_ADC_PIN)) : 0;
        #endif
        #ifdef ESP32
        int adc_value = analogRead(BATTERY_VOLTAGE_ADC_PIN);
        #endif
//        printLogMsgTime("Info: Battery level: Digit: %d\n", adc_value);

        float batteryVoltage = getBatteryVoltage(adc_value);
//        printLogMsgTime("Info: Battery level: Voltage: %.3f V, Digit: %d\n", batteryVoltage, adc_value);
        batteryLevel = calcBatteryLevel(batteryVoltage);
        printLogMsgTime("Info: Battery level: %d %%, Voltage: %.3f V, Digit: %d\n", batteryLevel, batteryVoltage, adc_value);
    #endif

    return batteryLevel;
}

float getBatteryVoltage(int adc_value)
{
    float battery_voltage = BATTERY_ADC_VREF / BATTERY_ADC_DIGIT_RANGE * adc_value * BATTERY_VOLTAGE_DIVIDER;

    return  battery_voltage;
}

uint8_t calcBatteryLevel(float voltage)
{ 
    const float VoltageMatrix[22][2] = BATTERY_LEVEL;
    
    int perc = 100;
    
    for(int i=20; i>=0; i--) {
        if (VoltageMatrix[i][0] >= voltage) {
            perc = (VoltageMatrix[i + 1][1]);
        break;
        }
    }
    return  (uint8_t) perc;
}