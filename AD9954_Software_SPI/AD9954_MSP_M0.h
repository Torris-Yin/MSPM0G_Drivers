#ifndef AD9959_MSP_M0
#define AD9959_MSP_M0

#include <cstdint>
#include "ti_msp_dl_config.h"
#include "delay.h"


typedef     uint8_t     _byte;
// pin short names
typedef     uint32_t    Pin_t;
//provide Arduio like definations
#define     HIGH    1
#define     LOW     0

//short to looooooong Definations of GPIO bank
#define    SS_PORT      GPIO_SPI_DDS_CS0_PORT
#define    RST_PORT     GPIO_DDS_PIN_RST_PORT
#define    UPDATE_PORT  GPIO_DDS_PIN_UPDATE_PORT
#define    PS0_PORT     GPIO_DDS_PIN_PS0_PORT
#define    PS1_PORT     GPIO_DDS_PIN_PS1_PORT
#define    OSK_PORT     GPIO_DDS_PIN_OSK_PORT

// helpler funcs to transmit in SPI bus
void SPI_Controller_transmitData(uint8_t *data, uint8_t dataLength);
void SPI_Controller_transmitCommand(uint8_t *cmd, uint8_t cmdLength);
void SPITransfer(_byte data);

//Arduino function decarations (marco)
#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)


class AD9954
{
    public: 
        // Constructor function. 
        AD9954(Pin_t , Pin_t, Pin_t, Pin_t, Pin_t, Pin_t);

        // Initialize with refClk frequency
        void initialize(unsigned long);

        // Initialize with refIn frequency, and clock multiplier value
        void initialize(unsigned long, _byte);

        // Reset the DDS
        void reset();

        // Update the new frequency tuning word
        void update();
        

        // Gets current frequency
        unsigned long getFreq();
        // Sets frequency
        void setFreq(unsigned long);

        // Gets current frequency tuning word
        unsigned long getFTW();
        // Sets frequency tuning word
        void setFTW(unsigned long);

        // places DDS in linear sweep mode
        void linearSweep(unsigned long, unsigned long, unsigned long, _byte, unsigned long, _byte);





    private:
        // Instance variables that hold pinout mapping
        // from arduino to DDS pins.
        Pin_t _ssPin, _rstPin, _updPin, _ps0Pin, _ps1Pin, _oskPin;

        // Instance variables for frequency _freq, frequency tuning word _ftw,
        // reference clock frequency _refClk, etc.
        unsigned long _freq, _ftw, _refClk, _refIn;


        // function to write data to register. 
        void writeRegister(_byte[2], _byte[1024]);


        // DDS frequency resolution
         double RESOLUTION;// = 4294967296; // sets resolution to 2^32 = 32 bits. Using type double to avoid confusion with integer division...



};
#endif