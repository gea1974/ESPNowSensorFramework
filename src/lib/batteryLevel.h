#include <Arduino.h>

#include <configuration.h>

#if (not defined BATTERY_LIPO || not defined BATTERY_2AAA)
    #define BATTERY_2AAA
#endif

#define BATTERY_LEVEL_2AAA { {3.20,100},{3.18,95},{3.16,90},{3.14,85},{3.12,80},{3.10,75},{3.08,70},{3.06,65},{3.04,60},{3.02,55},{3.00,50},{2.98,45},{2.96,40},{2.94,35},{2.92,30},{2.90,25},{2.88,20},{2.86,15},{2.84,10},{2.82,5},{2.80,0},{0,0} }
#define BATTERY_LEVEL_LIPO { {4.20,100},{4.15,95},{4.11,90},{4.08,85},{4.02,80},{3.98,75},{3.95,70},{3.91,65},{3.87,60},{3.85,55},{3.84,50},{3.82,45},{3.80,40},{3.79,35},{3.77,30},{3.75,25},{3.73,20},{3.71,15},{3.69,10},{3.61,5},{3.27,0},{0,0}}

#if (defined BATTERY_2AAA && not defined BATTERY_LEVEL)
        #define BATTERY_LEVEL BATTERY_LEVEL_2AAA
#endif
#if (defined BATTERY_LIPO && not defined BATTERY_LEVEL)
        #define BATTERY_LEVEL BATTERY_LEVEL_LIPO
#endif


#if (defined BATTERY_VOLTAGE_ON_PIN && not defined BATTERY_VOLTAGE_ON_POLARITY)
        #define BATTERY_VOLTAGE_ON_POLARITY HIGH
#endif

#ifndef BATTERY_MEASURE_DELAY
    #define BATTERY_MEASURE_DELAY       100
#endif

#ifdef ESP32
    #ifndef BATTERY_VOLTAGE_DIVIDER
        #define BATTERY_VOLTAGE_DIVIDER      2.5
    #endif
    #ifndef BATTERY_ADC_VREF
        #define BATTERY_ADC_VREF    3.300
    #endif
    #ifndef BATTERY_ADC_DIGIT_RANGE
        #define BATTERY_ADC_DIGIT_RANGE 4095.0
    #endif
#endif

#ifdef ESP8266
    #ifndef BATTERY_VOLTAGE_DIVIDER
        #define BATTERY_VOLTAGE_DIVIDER      3.5
    #endif
    #ifndef BATTERY_ADC_VREF
        #define BATTERY_ADC_VREF    1.000
    #endif
    #ifndef BATTERY_ADC_DIGIT_RANGE
        #define BATTERY_ADC_DIGIT_RANGE 1024.0
    #endif
#endif

uint8_t getBatteryLevel();
float getBatteryVoltage(int adc_value);
uint8_t calcBatteryLevel(float voltage);