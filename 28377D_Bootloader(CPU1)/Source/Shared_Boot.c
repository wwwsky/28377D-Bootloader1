#include "F28x_Project.h"
#include "fatfs/src/ff.h"
#include "F021_F2837xD_C28x.h"
#include "flash_programming_c28.h" //Include Flash API example header file
#include "utils/uartstdio.h"
#include "common.h"
#include "string.h"

//
// Globals
//

typedef struct
{
   Uint16 status;
   Uint32 address;
}  StatusCode;

StatusCode statusCode;


unsigned char erasedAlready[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static unsigned short crc16_tab[256] = {0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241, 0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440, 0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40, 0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841, 0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40, 0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41, 0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641, 0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040, 0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240, 0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441, 0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41, 0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840, 0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41, 0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40, 0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640, 0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041, 0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240, 0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441, 0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41, 0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840, 0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41, 0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40, 0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640, 0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041, 0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241, 0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440, 0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40, 0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841, 0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40, 0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41, 0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641, 0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040};

//
// Function Prototypes
//

Uint16 *sdram_data = (Uint16 *)0x80000000;
Uint16 DFU_CRC = 0xFFFF;
Uint16 flash_crc();
Uint16 FindSize(Uint32 address);
Uint32 FindSector(Uint32 address);
Uint16 File_GetWordData(void);
Uint16 GetByteData(void);
Uint32 GetLongData(void);

void CopyData(Uint32 EntryAddr);
void Example_Error(Fapi_StatusType status);
void ReadReservedFn(void);
unsigned short crc16(unsigned short crc, const void *buf, unsigned int size);


Uint32 SCI_Boot(Uint32 BootMode)
{
    Uint32 EntryAddr;

    statusCode.status = NO_ERROR;
    statusCode.address = 0x12346578;

    //
    // If the KeyValue was invalid, abort the load
    // and return the flash entry point.
    //
    if (File_GetWordData() != 0x08AA)
    {
        statusCode.status = VERIFY_ERROR;
        statusCode.address = 0x088000;
        UARTprintf("It is not firmware file. \r\n");
    }

    ReadReservedFn(); //reads and discards 8 reserved words
    {
        Uint16 x = 0;
        for(x = 0; x < 32676; x++)
            asm(" NOP");
    }
    EntryAddr = GetLongData();
    DINT;
    CopyData(EntryAddr);
    EINT;
    {
        Uint16 x = 0;
        for(x = 0; x < 32676; x++)
            asm(" NOP");
    }

    return EntryAddr;
}


void CopyData(Uint32 EntryAddr)
{
    struct HEADER {
     Uint16 BlockSize;
     Uint32 DestAddr;
    } BlockHeader;

    Uint16 wordData;
    Uint16 i;
    Uint16 j;
    static Uint16 Buffer[8], EntryBuffer[8];
    Uint32 sectorAddress;
    Uint16 sectorSize;


    for(i = 0; i < 14; i++)
    {
        erasedAlready[i] = 0;
    }

    BlockHeader.BlockSize = File_GetWordData();

    EALLOW;
    while(BlockHeader.BlockSize != (Uint16)0x0000)
    {
       Fapi_StatusType oReturnCheck;
       Fapi_FlashStatusWordType oFlashStatusWord;
       volatile Fapi_FlashStatusType oFlashStatus;
       BlockHeader.DestAddr = GetLongData();

       for(i = 0; i < BlockHeader.BlockSize; i += 0)
       {
            for(j = 0; j < 8; j++)
            {
               if(i == BlockHeader.BlockSize)
               {
                   Buffer[j] = 0xFFFF;
               }
               else
               {
                   wordData = File_GetWordData();
                   Buffer[j] = wordData;
                   i++;
               }
            }

            if(EntryAddr == BlockHeader.DestAddr)
            {
                for(j = 0 ; j < 8; j++)
                    EntryBuffer[j] = Buffer[j];
                break;
            }

           //
           //check that Buffer is not already all erased data
           //
           if(!((Buffer[0] == 0xFFFF) && (Buffer[1] == 0xFFFF) &&
                (Buffer[2] == 0xFFFF) && (Buffer[3] == 0xFFFF) &&
                (Buffer[4] == 0xFFFF) && (Buffer[5] == 0xFFFF) &&
                (Buffer[6] == 0xFFFF) && (Buffer[7] == 0xFFFF)))
           {
               //
               //clean out flash banks if needed
               //
               sectorAddress = FindSector(BlockHeader.DestAddr);
               if(sectorAddress != 0xdeadbeef)
               {
                    sectorSize = FindSize(sectorAddress);
                    oReturnCheck = Fapi_issueAsyncCommandWithAddress(Fapi_EraseSector,
                            (uint32 *)sectorAddress);

                    while(Fapi_checkFsmForReady() == Fapi_Status_FsmBusy);
                    oReturnCheck = Fapi_doBlankCheck((uint32 *)sectorAddress,
                            sectorSize,
                            &oFlashStatusWord);

                    if(oReturnCheck != Fapi_Status_Success)
                    {
                        statusCode.status = BLANK_ERROR;
                        statusCode.address = oFlashStatusWord.au32StatusWord[0];
                        Example_Error(oReturnCheck);
                    }
               }
               //
               //program 8 words at once, 128-bits
               //

               oReturnCheck = Fapi_issueProgrammingCommand((uint32 *)BlockHeader.DestAddr,
                                                       Buffer,
                                                       8,
                                                       0,
                                                       0,
                                                       Fapi_AutoEccGeneration);

               while(Fapi_checkFsmForReady() == Fapi_Status_FsmBusy);


                if(oReturnCheck != Fapi_Status_Success)
                {
                    statusCode.status = PROGRAM_ERROR;
                    statusCode.address = EntryAddr;
                    Example_Error(oReturnCheck);
                }

               oFlashStatus = Fapi_getFsmStatus();

               for(j = 0; j < 8; j += 2)
               {
                  Uint32 toVerify = Buffer[j+1];
                  toVerify = toVerify << 16;
                  toVerify |= Buffer[j];
                  oReturnCheck = Fapi_doVerify((uint32 *)(BlockHeader.DestAddr+j),
                                               1,
                                               (uint32 *)(&toVerify),
                                               &oFlashStatusWord);
                  if(oReturnCheck != Fapi_Status_Success)
                  {
                     statusCode.status = VERIFY_ERROR;
                     statusCode.address = oFlashStatusWord.au32StatusWord[0];
                     Example_Error(oReturnCheck);
                  }
               }
           }
           BlockHeader.DestAddr += 0x8;
       }
       //
       //get the size of the next block
       //
       BlockHeader.BlockSize = File_GetWordData();
    }

    EntryBuffer[2] = flash_crc();

    //EntryAddr DATA Flash Write
    {
        Fapi_StatusType oReturnCheck;
        Fapi_FlashStatusWordType oFlashStatusWord;

        oReturnCheck = Fapi_issueProgrammingCommand((uint32 *)EntryAddr, EntryBuffer, 8, 0, 0, Fapi_AutoEccGeneration);

        if(oReturnCheck != Fapi_Status_Success)
        {
            statusCode.status = PROGRAM_ERROR;
            statusCode.address = EntryAddr;
        }

        for(j = 0; j < 8; j += 2)
        {
           Uint32 toVerify = EntryBuffer[j+1];
           toVerify = toVerify << 16;
           toVerify |= EntryBuffer[j];
           oReturnCheck = Fapi_doVerify((uint32 *)(EntryAddr+j),
                                        1,
                                        (uint32 *)(&toVerify),
                                        &oFlashStatusWord);
            if(oReturnCheck != Fapi_Status_Success)
            {
                statusCode.status = VERIFY_ERROR;
                statusCode.address = oFlashStatusWord.au32StatusWord[0];
            }
        }
    }
    EDIS;
}

Uint16 flash_crc()
{
   register unsigned short *r_crc16_table = crc16_tab;
   Uint16 *Addr = (Uint16 *)0x88008;
   Uint16 CRC = 0xFFFF;
   for( ; Addr < (Uint16 *)Bzero_SectorN_End; Addr++)
   {
       CRC = r_crc16_table[(CRC ^ (*Addr & 0xFF)) & 0xFF] ^ (CRC >> 8);
       CRC = r_crc16_table[(CRC ^ (*Addr >> 8)) & 0xFF] ^ (CRC >> 8);
   }

   return CRC;
}


Uint32 FindSector(Uint32 address)
{
    if((address >= Bzero_SectorA_start) && (address <= Bzero_SectorA_End) &&
       (erasedAlready[0] == 0))
    {
        erasedAlready[0] = 1;
        return (Uint32)Bzero_SectorA_start;
    }
    else if((address >= Bzero_SectorB_start) &&
            (address <= Bzero_SectorB_End) && (erasedAlready[1] == 0))
    {
        erasedAlready[1] = 1;
        return (Uint32)Bzero_SectorB_start;
    }
    else if((address >= Bzero_SectorC_start) &&
            (address <= Bzero_SectorC_End) && (erasedAlready[2] == 0))
    {
        erasedAlready[2] = 1;
        return (Uint32)Bzero_SectorC_start;
    }
    else if((address >= Bzero_SectorD_start) &&
            (address <= Bzero_SectorD_End) && (erasedAlready[3] == 0))
    {
        erasedAlready[3] = 1;
        return (Uint32)Bzero_SectorD_start;
    }
    else if((address >= Bzero_SectorE_start) &&
            (address <= Bzero_SectorE_End) && (erasedAlready[4] == 0))
    {
        erasedAlready[4] = 1;
        return (Uint32)Bzero_SectorE_start;
    }
    else if((address >= Bzero_SectorF_start) &&
            (address <= Bzero_SectorF_End) && (erasedAlready[5] == 0))
    {
        erasedAlready[5] = 1;
        return (Uint32)Bzero_SectorF_start;
    }
    else if((address >= Bzero_SectorG_start) &&
            (address <= Bzero_SectorG_End) && (erasedAlready[6] == 0))
    {
        erasedAlready[6] = 1;
        return (Uint32)Bzero_SectorG_start;
    }
    else if((address >= Bzero_SectorH_start) &&
            (address <= Bzero_SectorH_End) && (erasedAlready[7] == 0))
    {
        erasedAlready[7] = 1;
        return (Uint32)Bzero_SectorH_start;
    }
    else if((address >= Bzero_SectorI_start) &&
            (address <= Bzero_SectorI_End) && (erasedAlready[8] == 0))
    {
        erasedAlready[8] = 1;
        return (Uint32)Bzero_SectorI_start;
    }
    else if((address >= Bzero_SectorJ_start) &&
            (address <= Bzero_SectorJ_End) && (erasedAlready[9] == 0))
    {
        erasedAlready[9] = 1;
        return (Uint32)Bzero_SectorJ_start;
    }
    else if((address >= Bzero_SectorK_start) &&
            (address <= Bzero_SectorK_End) && (erasedAlready[10] == 0))
    {
        erasedAlready[10] = 1;
        return (Uint32)Bzero_SectorK_start;
    }
    else if((address >= Bzero_SectorL_start) &&
            (address <= Bzero_SectorL_End) && (erasedAlready[11] == 0))
    {
        erasedAlready[11] = 1;
        return (Uint32)Bzero_SectorL_start;
    }
    else if((address >= Bzero_SectorM_start) &&
            (address <= Bzero_SectorM_End) && (erasedAlready[12] == 0))
    {
        erasedAlready[12] = 1;
        return (Uint32)Bzero_SectorM_start;
    }
    else if((address >= Bzero_SectorN_start) &&
            (address <= Bzero_SectorN_End) && (erasedAlready[13] == 0))
    {
        erasedAlready[13] = 1;
        return (Uint32)Bzero_SectorN_start;
    }
    else
    {
        return 0xdeadbeef; // a proxy address to signify that it is not
                           // a flash sector.
    }
}

//
// FindSector - This routine finds the size of the sector under use.
//
Uint16 FindSize(Uint32 address)
{
    if(address == Bzero_SectorA_start)
    {
        return Bzero_16KSector_u32length;
    }
    else if(address == Bzero_SectorB_start)
    {
        return Bzero_16KSector_u32length;
    }
    else if(address == Bzero_SectorC_start)
    {
        return Bzero_16KSector_u32length;
    }
    else if(address == Bzero_SectorD_start)
    {
        return Bzero_16KSector_u32length;
    }
    else if(address == Bzero_SectorE_start)
    {
        return Bzero_64KSector_u32length;
    }
    else if(address == Bzero_SectorF_start)
    {
        return Bzero_64KSector_u32length;
    }
    else if(address == Bzero_SectorG_start)
    {
        return Bzero_64KSector_u32length;
    }
    else if(address == Bzero_SectorH_start)
    {
        return Bzero_64KSector_u32length;
    }
    else if(address == Bzero_SectorI_start)
    {
        return Bzero_64KSector_u32length;
    }
    else if(address == Bzero_SectorJ_start)
    {
        return Bzero_64KSector_u32length;
    }
    else if(address == Bzero_SectorK_start)
    {
        return Bzero_16KSector_u32length;
    }
    else if(address == Bzero_SectorL_start)
    {
        return Bzero_16KSector_u32length;
    }
    else if(address == Bzero_SectorM_start)
    {
        return Bzero_16KSector_u32length;
    }
    else if(address == Bzero_SectorN_start)
    {
        return Bzero_16KSector_u32length;
    }

    //
    // no other possible case
    //
    return 0xbeef;
}

//
// GetLongData - This routine fetches a 32-bit value from the peripheral
//               input stream.
//
Uint32 GetLongData()
{
    Uint32 longData;

    //
    // Fetch the upper 1/2 of the 32-bit value
    //
    longData = (Uint32)File_GetWordData() << 16;

    //
    // Fetch the lower 1/2 of the 32-bit value
    //
    longData |= (Uint32)File_GetWordData();


    return longData;
}

//
// Read_ReservedFn - This function reads 8 reserved words in the header.
//                   None of these reserved words are used by the
//                   this boot loader at this time, they may be used in
//                   future devices for enhancements.  Loaders that use
//                   these words use their own read function.
//
void ReadReservedFn()
{
    Uint16 i;
    //
    // Read and discard the 8 reserved words.
    //
    for(i = 1; i <= 8; i++)
    {
        File_GetWordData();
    }
    return;
}


unsigned short crc16(unsigned short crc, const void *buf, unsigned int size)
{
    const unsigned char *p = buf;

    while (size--)
    {
        crc = crc16_tab[(crc ^ (*p++)) & 0xFF] ^ (crc >> 8);
    }
    return crc;
}


Uint16 File_GetWordData(void)
{
   Uint16 wordData = 0x0000;
   Uint16 byteData = 0x0000;

   wordData = *sdram_data++;

   byteData = *sdram_data++;

   wordData |= (byteData << 8);

   return wordData;
}

void Example_Error(Fapi_StatusType status)
{
    //
    // Error code will be in the status parameter
    //
    __asm("    ESTOP0");
}

void Init_Flash_Sectors(void)
{
    EALLOW;
    Flash0EccRegs.ECC_ENABLE.bit.ENABLE = 0x0;
    Fapi_StatusType oReturnCheck;

    oReturnCheck = Fapi_initializeAPI(F021_CPU0_BASE_ADDRESS, 150);
    if(oReturnCheck != Fapi_Status_Success)
    {
        Example_Error(oReturnCheck);
    }

    oReturnCheck = Fapi_setActiveFlashBank(Fapi_FlashBank0);
    if(oReturnCheck != Fapi_Status_Success)
    {
        Example_Error(oReturnCheck);
    }

    Flash0EccRegs.ECC_ENABLE.bit.ENABLE = 0xA;
    EDIS;
}

void File_Verify(void)
{
    Uint32 DestAddr = 0;
    Uint16 BlockSize = 0;
    Uint16 i;
    Uint16 j;
    Uint16 Buffer[8];

    GetLongData(); //EntryAddr

    BlockSize = File_GetWordData();

    while(BlockSize != (Uint16)0x0000)
    {
        Fapi_StatusType oReturnCheck;
        Fapi_FlashStatusWordType oFlashStatusWord;
        volatile Fapi_FlashStatusType oFlashStatus;
        DestAddr = GetLongData();

        for(i = 0; i < BlockSize; i += 0)
        {
            for(j = 0; j < 8; j++)
            {
                if(i == BlockSize)
                {
                    Buffer[j] = 0xFFFF;
                }
                else
                {
                    Buffer[j] = File_GetWordData();
                    i++;
                }
            }

            //
            //check that Buffer is not already all erased data
            //
            if(!((Buffer[0] == 0xFFFF) && (Buffer[1] == 0xFFFF) &&
            (Buffer[2] == 0xFFFF) && (Buffer[3] == 0xFFFF) &&
            (Buffer[4] == 0xFFFF) && (Buffer[5] == 0xFFFF) &&
            (Buffer[6] == 0xFFFF) && (Buffer[7] == 0xFFFF)))
            {


                for(j = 0; j < 8; j += 2)
                {
                    Uint32 toVerify = 0;
                    Uint16 low = Buffer[j];
                    Uint16 high = Buffer[j+1];
                    toVerify |= low;
                    toVerify |= (Uint32)high << 16;
                    oReturnCheck = Fapi_doVerify((uint32 *)(DestAddr+j),
                               1,
                               (uint32 *)(&toVerify),
                               &oFlashStatusWord);
                    if(oReturnCheck != Fapi_Status_Success)
                    {
                        if(oFlashStatusWord.au32StatusWord[0] == 0x88002)
                        {

                        }
                        else
                        {
                            statusCode.status = VERIFY_ERROR;
                            statusCode.address = oFlashStatusWord.au32StatusWord[0];
                        }
                        UARTprintf("addr : %X, flash : %08X, file : %08X\r\n", oFlashStatusWord.au32StatusWord[0], oFlashStatusWord.au32StatusWord[1], oFlashStatusWord.au32StatusWord[2]);
                    }
                }
            }
            DestAddr += 0x8;
        }
        BlockSize = File_GetWordData();
    }
}

int Flash_Erase(Uint32 Addr)
{
    int i = 0;
    Uint32 sectorAddress;
    Uint16 sectorSize;

    Fapi_StatusType oReturnCheck;
    Fapi_FlashStatusWordType oFlashStatusWord;

    for(i = 0; i < 14; i++)
    {
        erasedAlready[i] = 0;
    }
    sectorAddress = FindSector(Addr);
    if(sectorAddress != 0xdeadbeef)
    {
        sectorSize = FindSize(sectorAddress);
        oReturnCheck = Fapi_issueAsyncCommandWithAddress(Fapi_EraseSector, (uint32 *)sectorAddress);

        while(Fapi_checkFsmForReady() == Fapi_Status_FsmBusy);
        oReturnCheck = Fapi_doBlankCheck((uint32 *)sectorAddress, sectorSize, &oFlashStatusWord);

        if(oReturnCheck != Fapi_Status_Success)
        {
            statusCode.status = BLANK_ERROR;
            statusCode.address = oFlashStatusWord.au32StatusWord[0];
            Example_Error(oReturnCheck);
        }
    }
    else
    {
        return 1; // Out of flash zone.
    }

    return 0;
}


//
// End of file
//
