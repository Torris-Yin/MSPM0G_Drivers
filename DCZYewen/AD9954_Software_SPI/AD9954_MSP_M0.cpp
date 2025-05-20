#include "AD9954_MSP_M0.h"
#include "ti/driverlib/dl_gpio.h"
#include <cstdint>

void SPI_Controller_transmitData(uint8_t *data, uint8_t dataLength){
    int i = 0;
    for (i = 0; i < dataLength; i++) {
        while (DL_SPI_isBusy(SPI_DDS_INST))
            ;
        DL_SPI_transmitData8(SPI_DDS_INST, data[i]);
    }
}
void SPI_Controller_transmitCommand(uint8_t *cmd, uint8_t cmdLength){
    /* SPI must be idle prior to setting a new value of CDMODE */
    while (DL_SPI_isBusy(SPI_DDS_INST))
        ;

    DL_SPI_setControllerCommandDataModeConfig(SPI_DDS_INST, cmdLength);

    int i = 0;
    for (i = 0; i < cmdLength; i++) {
        while (DL_SPI_isBusy(SPI_DDS_INST))
            ;
        DL_SPI_transmitData8(SPI_DDS_INST, cmd[i]);
    }
}
void SPITransfer(_byte data){
    SPI_Controller_transmitData(&data, (uint8_t)1);
}

void writeIO( bool IO_STATE ,GPIO_Regs *gpio, uint32_t pins){
    if (IO_STATE){
        DL_GPIO_setPins(gpio, pins);
    }else {
        DL_GPIO_clearPins(gpio, pins);
    }
}


AD9954::AD9954(Pin_t ssPin , Pin_t rstPin, Pin_t updPin, Pin_t ps0Pin, Pin_t ps1Pin, Pin_t oskPin)
{
    RESOLUTION  = 4294967296.0;

    _ssPin      = ssPin;
    _rstPin     = rstPin;
    _updPin     = updPin;
    _ps0Pin     = ps0Pin;
    _ps1Pin     = ps1Pin;
    _oskPin     = oskPin;


    // defaults for pin logic levels
    DL_GPIO_setPins(SS_PORT, _ssPin);
    DL_GPIO_clearPins(GPIO_DDS_PIN_RST_PORT, _rstPin);
    DL_GPIO_clearPins(GPIO_DDS_PIN_UPDATE_PORT, _updPin);
    DL_GPIO_clearPins(GPIO_DDS_PIN_PS0_PORT, _ps0Pin);
    DL_GPIO_clearPins(GPIO_DDS_PIN_PS1_PORT, _ps1Pin);
    DL_GPIO_clearPins(GPIO_DDS_PIN_OSK_PORT, _oskPin);

}

// initialize(refClk) - initializes DDS with reference clock frequency refClk
void AD9954::initialize(unsigned long refClk){
    _refIn = refClk;
    _refClk = refClk;

    AD9954::reset();

    _byte registerInfo[] = {0x00, 4};
    _byte data[] = {0x00, 0x00, 0x00, 0x00};
    AD9954::writeRegister(registerInfo, data);

}
// initialize(refClk, clkMult) -- initializes DDS with input refClk, and activates the
//      onboard PLL multiplier clkMult.
//      clkMult: must be integer between 4 and 20 inclusive.
void AD9954::initialize(unsigned long refClk, _byte clkMult){
    _refIn = refClk;
    _refClk = _refIn * clkMult;

    // resets DDS
    AD9954::reset();

    _byte multValue = clkMult;

    _byte registerInfo[] = {0x01, 3};
    _byte data[] = {0x18, 0x00, 0x00};

    // writes value for clock multiplier
    if (_refClk < 25000000){ 
        multValue <<= 1;
        //multValue = multValue << 1;
    }else{
        (multValue <<= 1)++;
        //multValue = (multValue << 1) + 1;

        multValue <<= 2;
        //multValue = multValue << 2;

        data[2] = lowByte(multValue);

        AD9954::writeRegister(registerInfo, data);
    }
}

// reset() - takes no arguments; resets DDS
void AD9954::reset(){
    writeIO(HIGH , RST_PORT , _rstPin);
    delay_ms(1);
    writeIO(LOW , RST_PORT , _rstPin);
}

// update() - sends a logic pulse to IO UPDATE pin on DDS; updates frequency output to 
//      newly set frequency (FTW0)
void AD9954::update(){
    writeIO(HIGH , UPDATE_PORT, _updPin);
    writeIO(LOW  , UPDATE_PORT, _updPin);
}

// setFreq(freq) -- writes freq to DDS board, in FTW0
void AD9954::setFreq(unsigned long freq){
    // set _freq and _ftw variables
    _freq = freq;
    _ftw = freq * RESOLUTION / _refClk ;

    // divide up ftw into four bytes
    _byte ftw[] = { lowByte(_ftw >> 24), lowByte(_ftw >> 16), lowByte(_ftw >> 8), lowByte(_ftw)};
    // register info -- writing four bytes to register 0x04, 
    _byte registerInfo[] = {0x04, 4};
    
	_byte CFR1[] = { 0x00, 0x00, 0x00, 0x00 };
    _byte CFR1Info[] = {0x00, 4};
    
	// actually writes to register
    AD9954::writeRegister(CFR1Info, CFR1);
	AD9954::writeRegister(registerInfo, ftw);
    
	// issues update command
    AD9954::update();
}

// getFreq() - returns current frequency
unsigned long AD9954::getFreq(){
    return _freq;
}

// getFTW() -- returns current FTW
unsigned long AD9954::getFTW(){
    return _ftw;
}

// Function setFTW -- accepts 32-bit frequency tuning word ftw;
//      updates instance variables for FTW and Frequency, and writes ftw to DDS.
void AD9954::setFTW(unsigned long ftw){
    // set freqency and ftw variables
    _ftw = ftw;
    _freq = ftw * _refClk / RESOLUTION;

    // divide up ftw into four bytes
    _byte data[] = { lowByte(_ftw >> 24), lowByte(_ftw >> 16), lowByte(_ftw >> 8), lowByte(_ftw)};
    // register info -- writing four bytes to register 0x04, 
    _byte registerInfo[] = {0x04, 4};

	
    _byte CFR1[] = { 0x00, 0x00, 0x00, 0x00 };
    _byte CFR1Info[] = {0x00, 4};
	
    AD9954::writeRegister(CFR1Info, CFR1);
    AD9954::writeRegister(registerInfo, data);
    AD9954::update();

}

// Function linearSweep -- places DDS in linear sweep mode.
//      Behavior is determined by two frequency tuning words, freq0 and freq1 (freq0 < freq1).
//      The PS0 pin HIGH will ramp towards freq1, PS0 LOW will ramp towards freq0.
//      The rate of the ramp is dictated by pos/negDF (positive/negative DeltaFreq), and pos/negRR (positive/negative RampRate).
//      
//      freq0: lower frequency bound (Hz)
//      freq1: upper frequency bound (Hz)
//      posDF: delta frequency for positive ramp (Hz)
//      negDF: delta frequency for negative ramp (Hz)
//      posRR: number between 0 and 255, indicating number of SYNC_CLK cycles spent at each
//              frequency value in the ramp. SYNC_CLK operates at 1/4 of the SYSCLK clock value. Typically SYNC_CLK = 100MHz.
//              Thus, the true "ramp rate" is, eg, posDF/(posRR*10 ns)
//      negRR: same as above, but for negative ramp.
//      
//      As a general rule, round up (not down) in calculating the delta frequency steps.
void AD9954::linearSweep(unsigned long freq0, unsigned long freq1, unsigned long posDF, _byte posRR, unsigned long negDF, _byte negRR){

    // calculate
    unsigned long ftw0 = freq0*RESOLUTION / _refClk;
    unsigned long ftw1 = freq1*RESOLUTION / _refClk;
    unsigned long posDFW = posDF*RESOLUTION / _refClk;
    unsigned long negDFW = negDF*RESOLUTION / _refClk;


    // construct register values
    _byte CFR1[] = { 0x00, 0x20, 0x00, 0x00 };
    _byte CFR1Info[] = {0x00, 4};

    _byte FTW0[] = {lowByte(ftw0 >> 24), lowByte(ftw0 >> 16), lowByte(ftw0 >> 8), lowByte(ftw0) };
    _byte FTW0Info[] = {0x04, 4};

    _byte FTW1[] = {lowByte(ftw1 >> 24), lowByte(ftw1 >> 16), lowByte(ftw1 >> 8), lowByte(ftw1) };
    _byte FTW1Info[] = {0x06, 4};

    _byte NLSCW[] = { negRR, lowByte(negDFW >> 24), lowByte(negDFW >> 16), lowByte(negDFW >> 8), lowByte(negDFW) };
    _byte NLSCWInfo[] = {0x07, 5};

    _byte PLSCW[] = { posRR, lowByte(posDFW >> 24), lowByte(posDFW >> 16), lowByte(posDFW >> 8), lowByte(posDFW) };
    _byte PLSCWInfo[] = {0x08, 5};
    
    AD9954::writeRegister(CFR1Info, CFR1);
    AD9954::writeRegister(FTW0Info, FTW0);
    AD9954::writeRegister(FTW1Info, FTW1);
    AD9954::writeRegister(NLSCWInfo, NLSCW);
    AD9954::writeRegister(PLSCWInfo, PLSCW);

    AD9954::update();
	writeIO(HIGH , PS0_PORT , _ps0Pin);


}


// Writes SPI to particular register.
//      registerInfo is a 2-element array which contains [register, number of bytes]
void AD9954::writeRegister(_byte registerInfo[], _byte data[]){

    writeIO(LOW, SS_PORT, _ssPin);

    // Writes the register value
    SPITransfer(registerInfo[0]);

    // Writes the data
    for(int i = 0; i < registerInfo[1]; i++){
        SPITransfer(data[i]);
    }

    writeIO(HIGH, SS_PORT, _ssPin);

}