/*
 * Shared_Boot.h
 *
 *  Created on: 2021. 6. 1.
 *      Author: GP01
 */

#ifndef HEADER_SHARED_BOOT_H_
#define HEADER_SHARED_BOOT_H_

extern Uint16 DFU_CRC;


extern Uint16 File_GetWordData(void);
extern void ReadReservedFn();
extern void File_Verify(void);
extern void Init_Flash_Sectors(void);
extern int Version_Check(int argc, char *argv[]);
extern int Check_File_Crc();
extern Uint32 SCI_Boot(Uint32 BootMode);
extern Uint32 Crc_Check(Uint32 EntryAddr);
extern Uint32 MarginRead(Uint32 entryAddr);
extern unsigned short crc16(unsigned short crc, const void *buf, unsigned int size);


#endif /* HEADER_SHARED_BOOT_H_ */
