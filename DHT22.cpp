#include "DHT22.h"

/*!
 * @brief  DHT22 constructor
 * @param  dhtGPIO
 *         The GPIO of DHT22
 */
DHT22::DHT22(GPIO* dhtGPIO)
{
    this->dhtGPIO = dhtGPIO;
}

/*!
 * @brief  DHT22 destructor
 */
DHT22::~DHT22()
{
    this->dhtGPIO = NULL;
}

/*!
 * @brief  Begins the DHT22 implementation
 */
void DHT22::begin(void)
{
    beginTime();
    this->state = DHT22_REQUEST_LOW;
}

/*!
 * @brief  DHT22 main loop
 */
void DHT22::run(void)
{
    switch (this->state)
    {
        case DHT22_REQUEST_LOW:
            requestLow();
            break;
        case DHT22_REQUEST_WAIT_LOW:
            waitLow();
            break;
        case DHT22_REQUEST_WAIT_20_US:
            wait20us();
            break;
        case DHT22_RESPONSE_WAIT_START:
            waitStartResponse();
            break;
        case DHT22_RESPONSE_WAIT_HIGH:
            waitResponseHigh();
            break;
        case DHT22_RESPONSE_WAIT_LOW:
            waitResponseLow();
            break;
        case DHT22_DATA_READ_START:
            startDataRead();
            break;
        case DHT22_DATA_WAIT_HIGH:
            waitDataHigh();
            break;
        case DHT22_DATA_WAIT_40_US:
            waitData40us();
            break;
        case DHT22_DATA_WAIT_LOW:
            waitDataLow();
            break;
        case DHT22_WAIT_NEXT_PACKET:
            waitNextPacket();
            break;
    }
}

/*!
 * @brief  Getting the value of temperature
 * @return Value of temperature as a floating point number
 */
const float DHT22::getTemperature(void)
{
    return this->temperature;
}

/*!
 * @brief  Getting the value of humidity
 * @return Value of humidity as a floating point number
 */
const float DHT22::getHumidity(void) 
{
    return this->humidity;
}

void DHT22::requestLow(void)
{
    this->dhtGPIO->setMode(OUTPUT);
    this->dhtGPIO->write(LOW);
    this->nextTime = getMicroseconds() + DHT22_WAIT_LOW_INTERVAL;
    this->state = DHT22_REQUEST_WAIT_LOW;
}

void DHT22::waitLow(void)
{
    if (getMicroseconds() < this->nextTime)
        return;
    
    this->dhtGPIO->write(HIGH);
    this->nextTime = getMicroseconds() + DHT22_WAIT_20_US_INTERVAL;
    this->state = DHT22_REQUEST_WAIT_20_US;
}

void DHT22::wait20us(void)
{
    if (getMicroseconds() < this->nextTime)
        return;

    this->dhtGPIO->setMode(INPUT);
    this->state = DHT22_RESPONSE_WAIT_START;
}

void DHT22::waitStartResponse(void)
{
    if (this->dhtGPIO->read())
        return;
    
    this->state = DHT22_RESPONSE_WAIT_HIGH;
}

void DHT22::waitResponseHigh(void)
{
    if (!this->dhtGPIO->read())
        return;
    
    this->state = DHT22_RESPONSE_WAIT_LOW;
}

void DHT22::waitResponseLow(void)
{
    if (this->dhtGPIO->read())
        return;
    
    this->state = DHT22_DATA_READ_START;
}

void DHT22::startDataRead(void)
{
    this->data = 0;
    this->bitIndex = 0;
    this->byteIndex = 0;
    this->state = DHT22_DATA_WAIT_HIGH;
}

void DHT22::waitDataHigh(void)
{
    if (!this->dhtGPIO->read())
        return;
    
    this->nextTime = getMicroseconds() + DHT22_WAIT_40_US_INTERVAL;
    this->state = DHT22_DATA_WAIT_40_US;
}

void DHT22::waitData40us(void)
{
    if (getMicroseconds() < this->nextTime)
        return;
    
    // if pin is still high after 40us, it's a '1' else its a '0'
    if (this->dhtGPIO->read()) 
        this->data = (this->data << 1) | 1;
    else
        this->data = (this->data << 1);

    this->state = DHT22_DATA_WAIT_LOW;
}

void DHT22::waitDataLow(void)
{
    if (this->dhtGPIO->read())
        return;

    this->bitIndex++;
    if (this->bitIndex >= 8)
    {
        this->receivedData[this->byteIndex++] = this->data;
        if (this->byteIndex >= 5)
        {
            // All 5 bytes received
            const uint8_t I_RH = this->receivedData[0];     // integer RH
            const uint8_t D_RH = this->receivedData[1];     // decimal RH
            const uint8_t I_Temp = this->receivedData[2];   // integer Temp
            const uint8_t D_Temp = this->receivedData[3];   // decimal Temp
            const uint8_t checksum = this->receivedData[4]; // checksum

            if ((I_RH + D_RH + I_Temp + D_Temp) == checksum)
            {
                this->humidity = ((I_RH << 8) + D_RH) / 10.0;
                this->temperature = (((I_Temp & 0x7F) << 8) + D_Temp) / 10.0;
                if (I_Temp & 0x80) this->temperature *= -1;
            }
            this->nextTime = getMicroseconds() + DHT22_WAIT_NEXT_PACKET_US;
            this->state = DHT22_WAIT_NEXT_PACKET;
            return;
        }
        this->data = 0;
        this->bitIndex = 0;
    }

    this->state = DHT22_DATA_WAIT_HIGH;
}

void DHT22::waitNextPacket(void)
{
    if (getMicroseconds() < this->nextTime)
        return;

    this->state = DHT22_REQUEST_LOW;
}
