
#ifndef HEADER_EEPROM_H_
#define HEADER_EEPROM_H_

#include "Define.h"
#include "F28x_Project.h"

#define I2C_PWR_ON         (GpioDataRegs.GPESET.bit.GPIO133    = 1)
#define I2C_PWR_OFF        (GpioDataRegs.GPECLEAR.bit.GPIO133  = 1)

// Initialize I2C for serial eeprom(AT24C16) access
void Init_I2c_Eeprom();


void Write_EEPROM_Data(WORD addr, BYTE data);

// refer to single byte random read mode of AT24C16
void Read_EEPROM_Data(WORD addr, BYTE* pData);

#endif /* HEADER_EEPROM_H_ */
