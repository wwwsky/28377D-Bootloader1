//###############################################################################################################
// 파일명      : eeprom.c
// 회사명      : G-Philos
// 작성날짜  : 2020.12.17
// 작성자      : KYJ-PC
// 수정날짜  : 2021.01.04
// 수정자      : Lee Sang Cheol
// 설명         : Serial EEPROM(AT24C16) 구동에 필요한 I2C 관련 함수 구현
//###############################################################################################################

//////////////////////////////////////////////////////////////////////////////////////////////////
// [ AT24C16 reference ]
// 1) 2048(0x800) * 8 Bytes(16K)
// 2) Two-wire serial interface
// 3) 100KHz(1.8V) and 400KHz(2.7V, 5V) in Clock Frequency, SCL
//    Clock Pulse Width Low  : 4.7us(1.8V), 1.2us(2.7V, 5V) - Minimum value
//    Clock Pulse Width High : 4.0us(1.8V), 0.6us(2.7V, 5V) - Minimum value
// 4) 16 Byte Page Write Mode
// 5) Self-timed Write Cycle(5ms max)
// 6) organized with 128 pages of 16 bytes each, requires an 11-bit data word address
//
// Serial Clock(SCL)    : is used to positive edge clock data into each EEPROM device
//                        and negative edge clock data out of each device
// Serial Data(SDA)     : is bidirectional for serial data transfer. This pin is open-drain driven.
// Address(A0, A1, A2)  : does not use the device address pins
//
// [ 2 Byte Address - A0, A1 ]
// 1) A0[D7 ~ D4] : high [4-7] 4bit = 1010
// 2) A0[D3 ~ D1] : high [1-3] 3bit = Address
// 3) A0[D0]      : 1 = Read, 0 =Write
// 4) A1[D7 ~ D0] : low  [0-7] 8bit = Address
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "eeprom.h"
int     Wait_cnt[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int     Err_EEPROM              = 0;
//int Wait_cnt[10], Err_EEPROM;
// Initialize I2C for serial eeprom(AT24C16) access
void Init_I2c_Eeprom()
{

    EALLOW;

    // I2C_PWR
    GpioCtrlRegs.GPEGMUX1.bit.GPIO133 = 0;
    GpioDataRegs.GPEDAT.bit.GPIO133 = 1;
    GpioCtrlRegs.GPEDIR.bit.GPIO133 = 1;

    I2C_PWR_ON;
    EDIS;

    // Setting GPIO for I2C
    I2cAGpioConfig(5); //I2C_A_GPIO104_GPIO105


    // Initialize I2C interface
    // Put I2C module in the reset state
    I2caRegs.I2CMDR.bit.IRS = 0;

    // Set Slave Address according to AT24C16 device
//  I2caRegs.I2CSAR.all = 0x0050;
//  I2caRegs.I2COAR.all = 0x002D;

    // Set I2C module clock input
//  I2caRegs.I2CPSC.all = 10; // need 7-12 Mhz on module clk (200/11 = 18MHz)
    I2caRegs.I2CPSC.all = 19; // need 7-12 Mhz on module clk (200/20 = 10MHz)

    // 400KHz clock speed in SCL for master mode(2.5us period)
    // Clock Pulse Width Low  : 1.5us
    // Clock Pulse Width High : 1.0us
    // Refer to the formula for obtaining the master clock cycle in the F28377's I2C User Guide.
    I2caRegs.I2CCLKL = 10;
    I2caRegs.I2CCLKH = 5;

    // Disable I2C interrupt
    I2caRegs.I2CIER.all = 0x0;

    // Enable TX and RX FIFO in I2C module
    I2caRegs.I2CFFTX.all = 0x6000; // Enable FIFO mode and TXFIFO
    I2caRegs.I2CFFRX.all = 0x2040; // Enable RXFIFO, clear RXFFINT

    // Enable I2C module
    I2caRegs.I2CMDR.all = 0x20;
}


void Write_EEPROM_Data(WORD addr, BYTE data)
{
    WORD slave_addr, data_addr;

    int temp=0;

    slave_addr = 0x50 | ((addr >> 8) & 0x7);
    data_addr = addr & 0xff;

    // Wait until the STP bit is cleared from any previous master communication.
    //while(I2caRegs.I2CMDR.bit.STP == 1);
    for(temp=0;temp<1000;temp++)
    {
        if(I2caRegs.I2CMDR.bit.STP == 0)    break;
        Wait_cnt[5]++;
        if(temp>=999)   Err_EEPROM++;
    }

    // Wait until bus-free status
    //while(I2caRegs.I2CSTR.bit.BB == 1);
    for(temp=0;temp<1000;temp++)
    {
        if(I2caRegs.I2CSTR.bit.BB == 0) break;
        Wait_cnt[6]++;
        if(temp>=999)   Err_EEPROM++;
    }

    // Set Device(Slave) Address
    I2caRegs.I2CSAR.all = slave_addr;

    // Setup number of bytes to send
    I2caRegs.I2CCNT = 2;

    // Setup data to send
    I2caRegs.I2CDXR.all = data_addr; // Data Address
    I2caRegs.I2CDXR.all = data;      // Write Data

    // Send start as master transmitter with STT(=1), STP(=1), XA(=0), RM(=0)
    I2caRegs.I2CMDR.all = 0x6E20;

    // Wait until STOP condition is detected and clear STOP condition bit
    //while(I2caRegs.I2CSTR.bit.SCD == 0);
    for(temp=0;temp<1000;temp++)
    {
        if(I2caRegs.I2CSTR.bit.SCD == 1)    break;
        Wait_cnt[7]++;
        if(temp>=999)   Err_EEPROM++;
    }
    I2caRegs.I2CSTR.bit.SCD = 1;

    // Wait the Write-Cycle Time for EEPROM
    // refer to AT29C16 datasheet
    //  delay_ms(5);
}

// refer to single byte random read mode of AT24C16
void Read_EEPROM_Data(WORD addr, BYTE* pData)
{
    WORD slave_addr, data_addr;
    int temp=0;

    slave_addr = 0x50 | ((addr >> 8) & 0x7);
    data_addr = addr & 0xff;

    // Wait until the STP bit is cleared from any previous master communication.
    //while(I2caRegs.I2CMDR.bit.STP == 1);
    for(temp=0;temp<1000;temp++)
    {
        if(I2caRegs.I2CMDR.bit.STP == 0)    break;
        Wait_cnt[0]++;
        if(temp>=999)   Err_EEPROM++;
    }

    // Wait until bus-free status
    //while(I2caRegs.I2CSTR.bit.BB == 1);
    for(temp=0;temp<1000;temp++)
    {
        if(I2caRegs.I2CSTR.bit.BB == 0) break;
        Wait_cnt[1]++;
        if(temp>=999)   Err_EEPROM++;
    }

    // Set Device(Slave) Address
    I2caRegs.I2CSAR.all = slave_addr;

    // Setup number of bytes to send
    I2caRegs.I2CCNT = 1;

    // Setup data to send
    I2caRegs.I2CDXR.all = data_addr; // Data Address

    // Send start as master transmitter with STT(=1), STP(=0), XA(=0), RM(=0)
    I2caRegs.I2CMDR.all = 0x2620;

    // Wait until ARDY status bit is set
    //while(I2caRegs.I2CSTR.bit.ARDY == 0);
    for(temp=0;temp<1000;temp++)
    {
        if(I2caRegs.I2CSTR.bit.ARDY == 1)   break;
        Wait_cnt[2]++;
        if(temp>=999)   Err_EEPROM++;
    }

    // Wait until the STP bit is cleared
    //while(I2caRegs.I2CMDR.bit.STP == 1);
    for(temp=0;temp<1000;temp++)
    {
        if(I2caRegs.I2CMDR.bit.STP == 0)    break;
        Wait_cnt[3]++;
        if(temp>=999)   Err_EEPROM++;
    }

    // Set Device(Slave) Address
    I2caRegs.I2CSAR.all = slave_addr;

    // Setup number of bytes to read
    I2caRegs.I2CCNT = 1;

    // Send start as master receiver with STT(=1), STP(=1), XA(=0), RM(=0)
    I2caRegs.I2CMDR.all = 0x2C20;

    // Wait until STOP condition is detected and clear STOP condition bit
    //while(I2caRegs.I2CSTR.bit.SCD == 0);
    for(temp=0;temp<1000;temp++)
    {
        if(I2caRegs.I2CSTR.bit.SCD == 1)    break;
        Wait_cnt[4]++;
        if(temp>=999)   Err_EEPROM++;
    }
    I2caRegs.I2CSTR.bit.SCD = 1;

    *pData = (BYTE)(I2caRegs.I2CDRR.all & 0xff);
}

