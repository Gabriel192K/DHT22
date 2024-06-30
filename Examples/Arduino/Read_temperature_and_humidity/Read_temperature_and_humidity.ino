/* Dependencies */
#include <DHT22.h>
#include <GPIO.h>

/* Macros */
#define DHT22_DDR DDRD
#define DHT22_DOR PORTD
#define DHT22_DIR PIND
#define DHT22_BIT (const uint8_t)2

/* Objects */
GPIO dhtGPIO(&DHT22_DDR, &DHT22_DOR, &DHT22_DIR, DHT22_BIT);
DHT22 dht22(&dhtGPIO);

void setup(void)
{
    Serial.begin(115200);
    Serial.print(F("\t-- DHT22 Reading Temperature & Humidity--\n"));
    dht22.begin();
}

void loop(void)
{
    dht22.run();
    static uint32_t nextTime = 1000;
    if (millis() < nextTime)
        return;

    const float temperature = dht22.getTemperature();
    const float humidity = dht22.getHumidity();
    Serial.print(F("Temperature: ")); Serial.print(temperature); Serial.print(F(" *C\n"));
    Serial.print(F("Humidity   : ")); Serial.print(humidity); Serial.print(F(" %\n"));
    nextTime = millis() + 1000;
}

