#ifndef __DHT22_H__
#define __DHT22_H__

/* Dependencies */
#include "../GPIO/GPIO.h"
#if defined(ARDUINO)
#include "Arduino.h"
#else
#include "../Time/Time.h"
#endif

#if defined(ARDUINO)
#define beginTime()       0
#define getMicroseconds() micros()
#else // Microchip Studio
#define beginTime()       Time.begin()
#define getMicroseconds() Time.microseconds()
#endif

#define DHT22_WAIT_LOW_INTERVAL   (const uint32_t)1000    // in microseconds
#define DHT22_WAIT_20_US_INTERVAL (const uint32_t)20      // in microseconds
#define DHT22_WAIT_40_US_INTERVAL (const uint32_t)40      // in microseconds
#define DHT22_WAIT_NEXT_PACKET_US (const uint32_t)2000000 // in microseconds

enum DHT22_State
{
    DHT22_REQUEST_LOW,
    DHT22_REQUEST_WAIT_LOW,
    DHT22_REQUEST_WAIT_20_US,
    DHT22_RESPONSE_WAIT_START,
    DHT22_RESPONSE_WAIT_HIGH,
    DHT22_RESPONSE_WAIT_LOW,
    DHT22_DATA_READ_START,
    DHT22_DATA_WAIT_HIGH,
    DHT22_DATA_WAIT_40_US,
    DHT22_DATA_WAIT_LOW,
    DHT22_WAIT_NEXT_PACKET
};

class DHT22
{
    public:
        DHT22(GPIO* dhtGPIO);
        ~DHT22();
        void        begin         (void);
        void        run           (void);
        const float getTemperature(void);
        const float getHumidity   (void);

    private:
        GPIO* dhtGPIO;
        uint32_t nextTime;
        float humidity;
        float temperature;
        uint8_t state;
        uint8_t data;
        uint8_t bitIndex;
        uint8_t byteIndex;
        uint8_t receivedData[5]; // 5 bytes: I_RH, D_RH, I_Temp, D_Temp, Checksum

        void requestLow();
        void waitLow();
        void wait20us();
        void waitStartResponse();
        void waitResponseHigh();
        void waitResponseLow();
        void startDataRead();
        void waitDataHigh();
        void waitData40us();
        void waitDataLow();
        void waitNextPacket();
};

#endif // DHT22_H
