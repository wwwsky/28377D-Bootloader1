#include "F28x_Project.h"
#include "utils/cmdline.h"
#include "utils/uartstdio.h"
#include "fatfs/src/ff.h"
#include "eeprom.h"
#include "cmd.h"
#include "rtc.h"
#include "log.h"
#include "sci.h"
#include "Shared_Boot.h"
#include "version.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define led_on2 led_status = 4
#define led_on3 led_status = 6

Uint32 entryAddress = 0x88000; //bank 1 sector 0
Read_Time r_time;

FIL g_sFileObject;
FATFS g_sFatFs;
unsigned short usBytesRead;
unsigned short usBytesWritten;
FRESULT fresult;

DIR g_sDirObject;
FILINFO g_sFileInfo;
char g_cCmdBuf[CMD_BUF_SIZE];

char g_cCwdBuf[PATH_BUF_SIZE] = "/";

int  ch = 0;
unsigned long ulIdx = 0;

typedef struct
{
   Uint16 status;
   Uint32 address;
}  StatusCode;
extern StatusCode statusCode;

FRESULT Make_Sd_enoughspace();
extern char Get_File_version(char *version);
extern char Get_Flash_version(char *version);
extern char g_version[8][32];
extern char g_cTmpBuf[20];
extern int led_status;
extern Uint16 *sdram_data;

tCmdLineEntry g_psCmdTable[] =
{
    { "??",     Cmd_help,         "  : Display list of commands"},
    { "ls",     Cmd_ls,           "  : Display list of files"},
    { "ct",     Cmd_cat,          "  : Show contents of a text file"},
    { "rm",     Cmd_rm,           "  : Removes a file or directory"},
    { "fh",     Cmd_flash,        "  : Update DSP flash"},
    { "jp",     Cmd_jump,         "  : Jump to the Application"},
    { "dw",     Cmd_download,     "  : Download a file from the server"},
    { "up",     Cmd_upload,       "  : Upload file to server"},
    { "sd",     Cmd_sd,           "  : Display SD_CARD information"},
    { "if",     Cmd_info,         "  : Get a f/w version"},
    { "vr",     Cmd_filev,        "  : Get a file version"},
    { "fy",     Cmd_Verify,       "  : Compare a file to flash"},
    { "er",     Cmd_Erase,        "  : Clear the flash"},
    { "ep",     Cmd_Eeprom,       "  : R/W EEPROM"},
    { 0, 0, 0 }
};


int ReadLine()
{
    int ret = 0;
    ch = Get_Buff();

    if(ch != -1)
    {

        if((ch == '\b') && (ulIdx != 0))
        {

            UARTprintf("\b \b");
            ulIdx--;
            g_cCmdBuf[ulIdx] = '\0';
        }
        else if((ch == '\r') || (ch == '\n')) // 0x0D('\r') == terminal / 0x10('\n') == HMI
        {
            UARTprintf("\r\n");
            ulIdx = 0;

            ret = 1;
        }
        else if((ch == 0x1b) || (ch == 0x15))
        {
            while(ulIdx)
            {
                UARTprintf("\b \b");
                ulIdx--;
            }
            g_cCmdBuf[0] = '\0';
        }
        else if((ch >= ' ') && (ch <= '~') &&
                (ulIdx < (sizeof(g_cCmdBuf) - 1)))
        {
            g_cCmdBuf[ulIdx++] = ch;
            g_cCmdBuf[ulIdx] = '\0';
            UARTprintf(&g_cCmdBuf[ulIdx -1]);
        }

    }
    return ret;
}

//Check a F/W File version and send it to user.
int Cmd_filev(int argc, char *argv[])
{
    unsigned long fcnt = 0, fsize = 0;
    char readbyte[2];
    char fcpy = 0;
    char file_version[8][32];
    char *version = *file_version;
    int state = 0;
    int vcnt = 0;

    //Check the requested file name is over 12(8.3 format) characters.
    if (strlen(argv[1]) > 12)
    {
        UARTprintf("E : The path name is too long.\r\n");
        return 1;
    }

    //Mount
    fresult = f_mount(0, &g_sFatFs);

    //Check the version of the f/w file that user request.
    //open -> read & check -> send a result.
    fresult = f_open(&g_sFileObject, argv[1], FA_READ);

    if(fresult != FR_OK)
    {
        UARTprintf("E : Failed to open the file. Check the file or SD card exist.\r\n");
        return 1;
    }

    //Check the Size.
    fsize = g_sFileObject.fsize;

    //Find the location f/w file version exist.
    while (fcnt <= fsize)
    {
        fresult = f_read(&g_sFileObject, readbyte, 2, &usBytesRead);

        //version information is started from 'G-PHILOS'.
        switch (state)
        {
        case 0:
            if (readbyte[0] == 'G' && readbyte[1] == 0x00)
                state = 1;
            else
                state = 0;
            break;

        case 1:
            if (readbyte[0] == '-' && readbyte[1] == 0x00)
                state = 2;
            else
                state = 0;
            break;

        case 2:
            if (readbyte[0] == 'P' && readbyte[1] == 0x00)
                state = 3;
            else
                state = 0;
            break;

        case 3:
            if (readbyte[0] == 'H' && readbyte[1] == 0x00)
                state = 4;
            else
                state = 0;
            break;

        case 4:
            if (readbyte[0] == 'I' && readbyte[1] == 0x00)
                state = 5;
            else
                state = 0;
            break;

        case 5:
            if (readbyte[0] == 'L' && readbyte[1] == 0x00)
                state = 6;
            else
                state = 0;
            break;

        case 6:
            if (readbyte[0] == 'O' && readbyte[1] == 0x00)
                state = 7;
            else
                state = 0;
            break;

        case 7:
            if (readbyte[0] == 'S' && readbyte[1] == 0x00)
                state = 8;
            else
                state = 0;
            break;

        //Finding Location is successful.
        case 8:
            //Make sure the location is correct.
            if (readbyte[0] != 0x00)
                fcpy = 1;

            //Get the version information and copy it.
            if (fcpy)
            {
                *version++ = readbyte[0];
                vcnt++;
                if (256 <= vcnt)
                {
                    state = 9;
                }
            }
            break;

        default:
            break;
        }

        fcnt += 2;
    }

    //it has successfully received the version information and send the result.
    if(state == 9)
    {
        UARTprintf("%s, %s, %s, %s, %s, %s, %s, %s\r\n",file_version[0] ,file_version[1] ,file_version[2] ,file_version[3]
                                                     ,file_version[4] ,file_version[5] ,file_version[6] ,file_version[7] );
    }

    else
    {
        UARTprintf("E : Can't find the version information.\r\n" );
    }

    return 0;
}

//Check a F/W version in Flash memory and send it to user.
int Cmd_info(int argc, char *argv[])
{
    char flash_version[8][32];

    //Get version from f/w which is locate in current flash memory.
    if(*argv[1] == 'm' || *argv[1] == 'm')
    {
        switch(Get_Flash_version((char *)flash_version))
        {

        //successful.
        case 1:
            UARTprintf("Flash : %s, %s, %s, %s, %s, %s, %s, %s\r\n",flash_version[0] ,flash_version[1] ,flash_version[2] ,flash_version[3]
                                                                     ,flash_version[4] ,flash_version[5] ,flash_version[6] ,flash_version[7] );
            break;
        //fail
        case 0:
            UARTprintf("The firmware version cannot be found.\r\n" );
            break;
        }
    }
    //Get version from current f/w.
    else
    {
        UARTprintf("%s, %s, %s, %s, %s, %s, %s\r\n",g_version[0] ,g_version[1] ,g_version[2] ,g_version[3]
                                                   ,g_version[6] ,g_version[7], g_version[8]);
    }


    return 0;
}

//Show all kind of command and explanation.
int Cmd_help(int argc, char *argv[])
{
    tCmdLineEntry *pEntry;

    UARTprintf("Available commands\r\n");
    UARTprintf("------------------\r\n");

    // Point at the beginning of the command table.
    pEntry = &g_psCmdTable[0];

    while(pEntry->pcCmd)
    {

        // Print the command name and the brief description.
        UARTprintf("%s %s\r\n", pEntry->pcCmd, pEntry->pcHelp);

        // Advance to the next entry in the table.
        pEntry++;
    }

    // Return success.
    return(0);
}

//Get a file from user.
int Cmd_download(int argc, char *argv[])
{
    static unsigned long end_count = 0, length = 0;
    unsigned int len_cnt = 0, state = 0;
    char f_buff[S_MAX_SIZ];

    end_count = 0;
    length = 0;

    //Check the requested file name is over 12(8.3 format) characters.
    if(strlen(argv[1]) > 12)
    {
        UARTprintf("E : The path name is too long.\r\n");
        return(fresult);
    }
    //Mount
    fresult = f_mount(0, &g_sFatFs);

    if(fresult != FR_OK)
    {
        UARTprintf("E : Failed to open the file. Check the file or SD card exist.\r\n");
        return 1;
    }


    //r_time = Read_Rtc();

    fresult = Make_Sd_enoughspace();

    if(fresult != FR_OK)
    {
        UARTprintf("E : Failed to check SD card free space.\r\n");
        return 1;
    }
    fresult = f_open(&g_sFileObject, argv[1], FA_WRITE | FA_CREATE_ALWAYS);

    if(fresult != FR_OK)
    {
        UARTprintf("E : Failed to open the file. Check the file or SD card exist.\r\n");
        return 1;
    }

    TIME_RESET;

    //2. Get contents of file.
    while(1)
    {
        ch = Get_Buff();

        if(ch != -1)
        {
            //2.2 Get file contents as much as size that we received before.
            if(len_cnt >= 4)
            {
                //Get contents
                f_buff[end_count % S_MAX_SIZ] = ch;

                //Whenever the maximum buffer size is full, it is saved to the SD card.
                if((end_count % S_MAX_SIZ) == S_MAX_SIZ - 1)
                    fresult = f_write(&g_sFileObject, f_buff, sizeof(f_buff), &usBytesWritten);

                state = 2;
                end_count++;

                //Get
                if(end_count >= length)
                {
                    fresult = f_write(&g_sFileObject, f_buff, (int)(end_count % S_MAX_SIZ), &usBytesWritten);
                    state = 3;
                    break;
                }
            }

            //2.1 Get file contents size through 4 byte.
            else
            {
                length >>= 8;
                length |= (unsigned long)ch << 24;

                len_cnt++;
                state = 1;
            }
            TIME_RESET;
        }

        if(TIME >= 1000000)  // 1sec     1  =  10us
            break;

    }

    //2.3 Check the status.

    //Over file size Error
    if(state == CMD_DW_ERR_FILESIZE)
    {
        UARTprintf("E : Check file size.\r\n");
        Lprintf(0, "E : Check file size.\r\n");
    }
    //Time out Error.
    else if(state == CMD_DW_ERR_TIMEOUT)
    {
        UARTprintf("E : Download timeout.\r\n");
        Lprintf(0, "E : Download timeout.\r\n");
    }
    else if(state == CMD_DW_NORMAL)    // 정상
    {
        fresult = f_close(&g_sFileObject);
        UARTprintf("Download complete.\r\n");
    }
    else
    {

    }
    return(0);
}

//Copy the f/w to flash memory.
int Cmd_flash(int argc, char *argv[])
{
    FIL logFileObject;
    Uint32 fsize = 0;
    Uint16* sdram = (Uint16 *)0x80000000;
    sdram_data = sdram;

    //0. Read the f/w file user want to update.
    //Mount
    fresult = f_mount(0, &g_sFatFs);

    //Read the f/w file user requested.
    fresult = f_open(&g_sFileObject, argv[1], FA_READ);

    //Check Opening the file is failed.
    if(fresult != FR_OK)
    {
        UARTprintf("E : Failed to open the file. Check the file or SD card exist.\r\n");
        Lprintf(0, "E : Failed to open the file. Check the file or SD card exist.\r\n");
        led_on2;
        return 1;
    }


    //1. Check CRC.
    if(Check_File_Crc())
    {
        led_on2;
        Lprintf(0, "E : CRC error.\r\n");
        return 1;
    }

    //2. Check and compare the file version between selected file and flash.
    if(Version_Check(argc, argv))
    {
        led_on2;
        Lprintf(0, "E : Version error.\r\n");
        return 1;
    }

    //3. Make a file to leave a record about current update information.
    fresult = f_open(&logFileObject, "update1.ini", FA_WRITE | FA_CREATE_ALWAYS);

    fresult = f_write(&logFileObject, argv[0], strlen(argv[0]) + strlen(argv[1]) + 1, &usBytesWritten);

    fresult = f_close(&logFileObject);

    fsize = g_sFileObject.fsize;

    //4. Copy the f/w file from sd_card to sd_ram.
    do
    {
        if(fsize > 0xFFFF)
        {
            fresult = f_read(&g_sFileObject, sdram, 0xFFFF, &usBytesRead);
            fsize -= 0xFFFF;
            sdram += 0xFFFF;
        }
        else
        {
            fresult = f_read(&g_sFileObject, sdram, fsize, &usBytesRead);
            fsize = 0;
        }
    }
    while(fsize > 0);

    //initialize CRC.
    DFU_CRC = 0xFFFF;

    SeizeFlashPump();

    //5. Copy the file from sd_ram to flash.
    entryAddress = SCI_Boot(1);         //loads application into CPU1 FLASH

    ReleaseFlashPump();

    //6. Remove the current update record because update was successfully completed.
    fresult = f_unlink("update1.ini");

    fresult = f_close(&g_sFileObject);

    //7. Jump to main f/w.
    if(statusCode.status == NO_ERROR)
    {
        UARTprintf("Flash finished.\r\n");
        Cmd_jump(0, 0);
    }

    else
    {
        led_on2;
        UARTprintf("E : Failed to copy F/W to FLASH memory.\r\n");
        Lprintf(0, "E : Failed to copy F/W to FLASH memory.\r\n");
        return 1;
    }


    return 0;
}

Uint16 flash_crc();

static int cpu = 0, fw = 0;

void Cmd_bootflagGet(void)
{
    char* sdram = (char *)0x80000000;

    sdram++;

    if( strcmp(sdram, "bootloader1") == 0 )
    {
        cpu = 1;
        fw = 0;
        UARTprintf("bootloader1.\r\n");
    }
    else if( strcmp(sdram, "controller") == 0 )
    {
        cpu = 1;
        fw = 1;
        UARTprintf("controller.\r\n");
    }
    else if( strcmp(sdram, "bootloader2") == 0 )
    {
        cpu = 2;
        fw = 0;
        UARTprintf("bootloader2.\r\n");
    }
    else if( strcmp(sdram, "blackbox") == 0 )
    {
        cpu = 2;
        fw = 1;
        UARTprintf("blackbox.\r\n");
    }

    strcpy(sdram, "01234567890123456789");
}

void Cmd_bootflagSet(void)
{
    char* sdram = (char *)0x80000000;

    UARTprintf("boot=%d,%d.\r\n", cpu, fw);

    if(cpu == 1)
    {
        if( fw == 0 )
        {
            strcpy(sdram, " bootloader1");
        }
        else
        {
            strcpy(sdram, " controller");
        }
    }
    else if(cpu == 2)
    {
        if( fw == 0 )
        {
            strcpy(sdram, " bootloader2");
        }
        else
        {
            strcpy(sdram, " blackbox");
        }
    }
}

//Jump to specific address depend on entryAddress value.
int Cmd_jump(int argc, char *argv[])
{
    Uint32 Address = entryAddress;

    f_mount(0, NULL);

    Cmd_bootflagSet();

    //Check the location of entryAddress is empty.
    if( *(Uint32 *)Address != 0xFFFFFFFF )
    {
        Address += 2;

        UARTprintf("CPU1 entry address = %08X(%08X).\r\n", Address, *((Uint32 *)Address));

        //Check CRC code exist.
        if( *(Uint16 *)Address != 0xFFFF )
        {
            Uint16 crc = flash_crc();

            if( crc == *(Uint16 *)Address )
            {
                UARTprintf("Jump to CPU1 entry address.\r\n");
                DINT;
                ((void (*)(void))entryAddress)();
            }
            else
            {
                led_on3;
                Lprintf(0, "E : Failed to jump to main F/W. CRC doesn't match.\r\n");
            }
        }
        else
        {
            UARTprintf("Jump to CPU1 entry address(0xFFFF).\r\n");
            DINT;
            ((void (*)(void))entryAddress)();
        }

        //If Debug mode is activated, it can jump without CRC Check.
        if( CpuSysRegs.RESC.bit.TRSTn_pin_status == 1 )
        {
            DINT;
            ((void (*)(void))entryAddress)();
        }
    }
    UARTprintf("E : F/W is empty.\r\n");

    return 0;
}

int Cmd_sd(int argc, char *argv[])
{
    unsigned long ulTotalSize;
    unsigned long ulFileCount;
    unsigned long ulDirCount;
    FATFS *pFatFs;

    fresult = f_mount(0, &g_sFatFs);
    //
    // Open the current directory for access.
    //
    fresult = f_opendir(&g_sDirObject, g_cCwdBuf);

    //
    // Check for error and return if there is a problem.
    //
    if(fresult != FR_OK)
    {
        UARTprintf("E : Failed to read SD card. Check the SD card exists.\r\n");
        return(fresult);
    }

    ulTotalSize = 0;
    ulFileCount = 0;
    ulDirCount = 0;

    //
    // Enter loop to enumerate through all directory entries.
    //
    while(1)
    {
        //
        // Read an entry from the directory.
        //
        fresult = f_readdir(&g_sDirObject, &g_sFileInfo);

        //
        // Check for error and return if there is a problem.
        //
        if(fresult != FR_OK)
        {
            UARTprintf("E : Failed to read the directory from SD Card.\r\n");
            return(fresult);
        }

        //
        // If the file name is blank, then this is the end of the listing.
        //
        if(!g_sFileInfo.fname[0])
        {
            break;
        }

        //
        // If the attribute is directory, then increment the directory count.
        //
        if(g_sFileInfo.fattrib & AM_DIR)
        {
            ulDirCount++;
        }

        //
        // Otherwise, it is a file.  Increment the file count, and add in the
        // file size to the total.
        //
        else
        {
            ulFileCount++;
            ulTotalSize += g_sFileInfo.fsize;
        }
    }

    //
    // Print summary lines showing the file, dir, and size totals.
    //
    UARTprintf("%4u File(s),%10u bytes total\r\n%4u Dir(s)",
               ulFileCount, ulTotalSize, ulDirCount);

    //
    // Get the free space.
    //
    fresult = f_getfree("/", &ulTotalSize, &pFatFs);

    //
    // Check for error and return if there is a problem.
    //
    if(fresult != FR_OK)
    {
        UARTprintf("E : Failed to get the free spaces from SD card.\r\n");
        return(fresult);
    }

    //
    // Display the amount of free space that was calculated.
    //
    UARTprintf(", %10uK bytes free\r\n", ulTotalSize * pFatFs->sects_clust / 2);

    //
    // Made it to here, return with no errors.
    //
    return(0);
}


int Cmd_ls(int argc, char *argv[])
{
    unsigned long ulTotalSize;
    unsigned long ulFileCount;
    unsigned long ulDirCount;


    fresult = f_mount(0, &g_sFatFs);
    //
    // Open the current directory for access.
    //
    fresult = f_opendir(&g_sDirObject, g_cCwdBuf);


    //
    // Check for error and return if there is a problem.
    //
    if(fresult != FR_OK)
    {
        UARTprintf("E : Fail to read SD card. Check the SD card exists.\r\n");
        return(fresult);
    }

    ulTotalSize = 0;
    ulFileCount = 0;
    ulDirCount = 0;

    //
    // Enter loop to enumerate through all directory entries.
    //
    while(1)
    {
        //
        // Read an entry from the directory.
        //
        fresult = f_readdir(&g_sDirObject, &g_sFileInfo);

        //
        // Check for error and return if there is a problem.
        //
        if(fresult != FR_OK)
        {
            UARTprintf("E : Failed to read the directory from SD card.\r\n");
            return(fresult);
        }

        //
        // If the file name is blank, then this is the end of the listing.
        //
        if(!g_sFileInfo.fname[0])
        {
            break;
        }

        //
        // If the attribute is directory, then increment the directory count.
        //
        if(g_sFileInfo.fattrib & AM_DIR)
        {
            ulDirCount++;
        }

        //
        // Otherwise, it is a file.  Increment the file count, and add in the
        // file size to the total.
        //
        else
        {
            ulFileCount++;
            ulTotalSize += g_sFileInfo.fsize;
        }

        //
        // Print the entry information on a single line with formatting to show
        // the attributes, date, time, size, and name.
        //
        UARTprintf("%u/%u/%u %u:%u,%u,%s\r\n",
                           (uint32_t)((g_sFileInfo.fdate >> 9) + 1980),
                           (uint32_t)((g_sFileInfo.fdate >> 5) & 15),
                           (uint32_t)(g_sFileInfo.fdate & 31),
                           (uint32_t)((g_sFileInfo.ftime >> 11)),
                           (uint32_t)((g_sFileInfo.ftime >> 5) & 63),
                           (uint32_t)(g_sFileInfo.fsize),
                                   g_sFileInfo.fname);
    }

    //
    // Made it to here, return with no errors.
    //
    return(0);
}

int Cmd_cat(int argc, char *argv[])
{
    FRESULT fresult;
    unsigned short usBytesRead;

    //
    // First, check to make sure that the current path (CWD), plus the file
    // name, plus a separator and trailing null, will all fit in the temporary
    // buffer that will be used to hold the file name.  The file name must be
    // fully specified, with path, to FatFs.
    //
    if(strlen(g_cCwdBuf) + strlen(argv[1]) + 1 + 1 > sizeof(g_cTmpBuf))
    {
        UARTprintf("E : The path name is too long.\r\n");
        return(0);
    }

    //
    // Copy the current path to the temporary buffer so it can be manipulated.
    //
    strcpy(g_cTmpBuf, g_cCwdBuf);

    //
    // If not already at the root level, then append a separator.
    //
    if(strcmp("/", g_cCwdBuf))
    {
        strcat(g_cTmpBuf, "/");
    }

    //
    // Now finally, append the file name to result in a fully specified file.
    //
    strcat(g_cTmpBuf, argv[1]);

    //
    // Open the file for reading.
    //
    fresult = f_open(&g_sFileObject, g_cTmpBuf, FA_READ);

    //
    // If there was some problem opening the file, then return an error.
    //
    if(fresult != FR_OK)
    {
        UARTprintf("E : Failed to open the file. Check the file or SD card exist.\r\n");
        return(fresult);
    }

    //
    // Enter a loop to repeatedly read data from the file and display it, until
    // the end of the file is reached.
    //
    do
    {
        //
        // Read a block of data from the file.  Read as much as can fit in the
        // temporary buffer, including a space for the trailing null.
        //
        fresult = f_read(&g_sFileObject, g_cTmpBuf, sizeof(g_cTmpBuf) - 1,
                         &usBytesRead);

        //
        // If there was an error reading, then print a newline and return the
        // error to the user.
        //
        if(fresult != FR_OK)
        {
            UARTprintf("E : Failed to read the file.\r\n");
            return(fresult);
        }

        //
        // Null terminate the last block that was read to make it a null
        // terminated string that can be used with printf.
        //
        g_cTmpBuf[usBytesRead] = 0;

        //
        // Print the last chunk of the file that was received.
        //
        UARTprintf("%s", g_cTmpBuf);

        //
        // Continue reading until less than the full number of bytes are read.
        // That means the end of the buffer was reached.
        //
    }
    while(usBytesRead == sizeof(g_cTmpBuf) - 1);

    //
    // Close the file after done with it.
    //
    f_close(&g_sFileObject);
    UARTprintf("\r\n");
    //
    // Return success.
    //
    return(0);
}

int Cmd_rm(int argc, char *argv[])
{
//    fresult = f_mount(0, &g_sFatFs);
    //
    // First, check to make sure that the current path (CWD), plus the file
    // name, plus a separator and trailing null, will all fit in the temporary
    // buffer that will be used to hold the file name.  The file name must be
    // fully specified, with path, to FatFs.
    //
    if(strlen(g_cCwdBuf) + strlen(argv[1]) + 1 + 1 > sizeof(g_cTmpBuf))
    {
        UARTprintf("E : The path name is too long\n");
        return(0);
    }

    //
    // Copy the current path to the temporary buffer so it can be manipulated.
    //
    strcpy(g_cTmpBuf, g_cCwdBuf);

    //
    // If not already at the root level, then append a separator.
    //
    if(strcmp("/", g_cCwdBuf))
    {
        strcat(g_cTmpBuf, "/");
    }

    //
    // Append the file/sub-directory name to result in a fully specified path.
    //
    strcat(g_cTmpBuf, argv[1]);

    //
    // Unlink the file or sub-directory to be removed.
    //
    fresult = f_unlink(g_cTmpBuf);

    //
    // Check for error and return if there is a problem.
    //
    if(fresult != FR_OK)
    {
        UARTprintf("E : Failed to remove the file.\r\n");
        return(fresult);
    }

    //
    // Return success.
    //
    return(0);
}

//Send a file to user.
int Cmd_upload(int argc, char *argv[])
{
    static Uint32 data_cnt = 0;
    unsigned short usBytesRead;

    data_cnt = 0;

//    BYTE multi_data[512];
    BYTE one_data;

    fresult = f_mount(0, &g_sFatFs);

    fresult = f_open(&g_sFileObject, argv[1], FA_READ);

    if(fresult != FR_OK)
    {
        UARTprintf("E : Failed to open the file. Check the file or SD card exist.\r\n");
        return 1; //Failed to open file.
    }

    while (SciaRegs.SCICTL2.bit.TXRDY != 1);
    SciaRegs.SCITXBUF.bit.TXDT = (g_sFileObject.fsize >> 24) & 0xff;

    while (SciaRegs.SCICTL2.bit.TXRDY != 1);
    SciaRegs.SCITXBUF.bit.TXDT = (g_sFileObject.fsize >> 16) & 0xff;

    while (SciaRegs.SCICTL2.bit.TXRDY != 1);
    SciaRegs.SCITXBUF.bit.TXDT = (g_sFileObject.fsize >> 8) & 0xff;

    while (SciaRegs.SCICTL2.bit.TXRDY != 1);
    SciaRegs.SCITXBUF.bit.TXDT = (g_sFileObject.fsize) & 0xff;

    //Send one byte at a time
    while(data_cnt <= g_sFileObject.fsize)
    {
        fresult = f_read(&g_sFileObject, &one_data, 1, &usBytesRead);


        //
        // Check for error and return if there is a problem.
        //
        if(fresult != FR_OK)
        {
            UARTprintf("E : Failed to read the file.\r\n");
            return(fresult);
        }

        while (SciaRegs.SCICTL2.bit.TXRDY != 1);
        SciaRegs.SCITXBUF.bit.TXDT = one_data;
        data_cnt++;
    }


    fresult = f_close(&g_sFileObject);

    UARTprintf("\r\n");

    return 0;
}

int Cmd_Verify(int argc, char *argv[])
{
    Uint32 fsize = 0;
    Uint16 *sdram = (Uint16 *)0x80000000;
    Uint32 *start_addr = (Uint32 *)entryAddress;
    sdram_data = sdram;

    fresult = f_mount(0, &g_sFatFs);

    fresult = f_open(&g_sFileObject, argv[1], FA_READ);

    if(fresult != FR_OK)
    {
        UARTprintf("E : Failed to open the file. Check the file or SD card exist.\r\n");
        return 1; //Failed to open file.
    }

    if(*start_addr == 0xFFFFFFFF)
    {
        UARTprintf("E : Flash is empty.\r\n");
        return 1;
    }

    statusCode.status = NO_ERROR;
    statusCode.address = 0x12346578;

    fsize = g_sFileObject.fsize;

    do
    {
        if(fsize > 0xFFFF)
        {
            fresult = f_read(&g_sFileObject, sdram, 0xFFFF, &usBytesRead);
            fsize -= 0xFFFF;
            sdram += 0xFFFF;
        }
        else
        {
            fresult = f_read(&g_sFileObject, sdram, fsize, &usBytesRead);
            fsize = 0;
        }
    }
    while(fsize > 0);

    if(File_GetWordData() != 0x08AA)
    {
        UARTprintf("E : This is not a firmware file.\r\n");
        return 1;
    }

    ReadReservedFn();

    File_Verify();

    if(statusCode.status == 0x1000)
        UARTprintf("Match.\r\n");
    else
        UARTprintf("Does not match.\r\n");

    return 0;
}

extern int Flash_Erase(Uint32 Addr);

int Cmd_Eeprom(int argc, char *argv[])
{
    WORD addr;
    BYTE data = 0;
    if(*argv[1] != 'r' && *argv[1] != 'R' && *argv[1] != 'w' && *argv[1] != 'W')
    {
        UARTprintf("E : R/W mode is inserted incorrectly .\r\n");
        return(fresult);
    }

    if(strlen(argv[2]) > 4)
    {
        UARTprintf("E : Address is too long.\r\n");
        return(fresult);
    }

    if(strlen(argv[3]) > 2)
    {
        UARTprintf("E : data content is too long.\r\n");
        return(fresult);
    }

    if(*argv[1] == 'r' || *argv[1] == 'R')
    {
        addr =  strtol(argv[2], NULL, 16);
        Read_EEPROM_Data(addr, &data);
        UARTprintf("%X.\r\n", (Uint32)data);
    }

    else if(*argv[1] == 'w' || *argv[1] == 'W')
    {
        addr = strtol(argv[2], NULL, 16);
        data = strtol(argv[3], NULL, 16);
        Write_EEPROM_Data(addr, data);
    }

    UARTprintf("Process finished.\r\n");
    return 0;
}
int Cmd_Erase(int argc, char *argv[])
{

    Uint32 addr = strtol(argv[1], NULL, 16);
    if(addr < entryAddress)
        return 1;
    SeizeFlashPump();
    EALLOW;
    Flash_Erase(addr);
    EDIS;
    ReleaseFlashPump();
    return 0;
}


/************************* 함 수 ****************************/


int Check_File_Crc()
{
    unsigned long fcnt = 0, fsize = 0;
    int ret = 0;
    Uint16 crc_temp = 0;
    char readbyte[2];

    // 4 = FILE CRC OFFSET

    if(g_sFileObject.fsize <= 4)
    {
        UARTprintf("No file data.\r\n" );
        return 1;
    }
    else
        fsize = g_sFileObject.fsize - 4;

    // INIT CRC
    DFU_CRC = 0xFFFF;


    while(fcnt <= fsize)
    {
        fresult = f_read(&g_sFileObject, readbyte, 2, &usBytesRead);

        DFU_CRC = crc16(DFU_CRC, readbyte, 2);

        fcnt += 2;
    }


    // Extract CRC
    fresult = f_read(&g_sFileObject, g_cTmpBuf, 2, &usBytesRead);

    crc_temp = g_cTmpBuf[1] << 8 | g_cTmpBuf[0];

    if(crc_temp != DFU_CRC)
    {
        UARTprintf("FILE : 0x%X, CAL : 0x%X CRC FAIL.\r\n", (Uint32)crc_temp, (Uint32)DFU_CRC);
        ret = 1;
    }
    else
        UARTprintf("FILE : 0x%X, CAL : 0x%X CRC OK.\r\n", (Uint32)crc_temp, (Uint32)DFU_CRC);


    f_lseek(&g_sFileObject, 0);

    return ret;
}

int Version_Check(int argc, char *argv[])
{
    int ret = 0;

    char file_version[8][32]= {0, };
    char flash_version[8][32] = {0, };
    char is_file = 0, is_flash = 0;

    memset(file_version, 0, sizeof(file_version));
    memset(flash_version, 0, sizeof(flash_version));

    is_file = Get_File_version((char *)file_version);
    is_flash = Get_Flash_version((char *)flash_version);

    //1. In case of file version was found successfully.
    if(is_file)
    {
        UARTprintf("File  : %s, %s, %s, %s, %s, %s, %s, %s\r\n",file_version[0] ,file_version[1] ,file_version[2] ,file_version[3]
                                                             ,file_version[4] ,file_version[5] ,file_version[6] ,file_version[7] );
        //1.1 In case of flash version was found successfully.
        if(is_flash)
        {
            UARTprintf("Flash : %s, %s, %s, %s, %s, %s, %s, %s\r\n",flash_version[0] ,flash_version[1] ,flash_version[2] ,flash_version[3]
                                                                   ,flash_version[4] ,flash_version[5] ,flash_version[6] ,flash_version[7] );
        }
        //1.2 fail
        else
            UARTprintf("Flash : No.\r\n");

        int pass = 0;
        int caution = 0;
        {
            int p = 0;
            int result = 0;

            //1.3 Compare version between flash and file.
            for(p = 0; p < 6; p++)
            {
                //1.3.1 Compare each other.
                result = strcmp(file_version[p], flash_version[p]);

                //1.3.2 Both versions match each other.
                if(result == 0)
                {
    //                if(i == INDEX_VERSION - 1)
    //                    caution += 1 << p;
                }

                //1.3.3 There is difference.
                else
                {
                    if(p == INDEX_VERSION - 1)
                    {
                        //1.3.3.1 In case of ' file version > flash version'
                        if(result > 0)   continue;

                        //1.3.3.2 In case of ' file version < flash version'
                        else             caution += 1 << p;
                    }
                    else
                    {
                        pass += 1 << p;
                    }
                }
            }
        }

        int i = 0;
        for(i = 2; i < argc; i++)
        {
            //If the f/w version user requested is older than current one, it need argument 'y' to copy it to flash.
            if(*argv[i] == 'y' || *argv[i] == 'Y')
            {
               pass &= ~(1 << (INDEX_VERSION - 1));
               pass &= ~(1 << (INDEX_BOARD_ID - 1));
               pass &= ~(1 << (INDEX_CUSTOMER_ID - 1));
               caution = -1;
            }
            //Free pass.
            else if(*argv[i] == 'e' || *argv[i] == 'E')
            {
                is_file = 2;
                is_flash = 1;
                pass = 0;
            }
        }


        if(is_flash == 0)
        {
            UARTprintf("F/W is empty.\r\n");
            ret = 1;
        }
        else if(is_file == 1)
        {
            if(pass == 0)
            {
                if(caution > 0)
                {
                    UARTprintf("This file is old version.\r\n");
                    ret = 1;
                }
                else if(caution == -1)
                {
                    //y cmd
                }
                else
                {
                    UARTprintf("Version match.\r\n");
                }
            }
            else
            {
                UARTprintf("This is a different program.\r\n");
                ret = 1;
            }
        }
        else
        {
            //  e cmd
        }

        if(strcmp(g_version[1], file_version[0]))
            ret = 1;

    }
    else
        ret = 1;
    f_lseek(&g_sFileObject, 0);


    return ret;
}

FRESULT Make_Sd_enoughspace()
{
        unsigned long free_byte;
        unsigned long ulTotalSize;
        unsigned long ulFileCount;
        unsigned long ulDirCount;
        char oldest_name[13], file_start;
        char* cmp_name;
        uint32_t oldest_time;
        uint32_t cur_time;
        FATFS *pFatFs;

        fresult = f_mount(0, &g_sFatFs);
        //
        // Open the current directory for access.
        //
        fresult = f_opendir(&g_sDirObject, g_cCwdBuf);

        //
        // Check for error and return if there is a problem.
        //
        if(fresult != FR_OK)
        {
            UARTprintf("E : Failed to read SD card. Check the SD card exists.\r\n");
            return(fresult);
        }


        ulTotalSize = 0;
        ulFileCount = 0;
        ulDirCount = 0;

        //
        // Enter loop to enumerate through all directory entries.
        //
        while(1)
        {
            //
            // Read an entry from the directory.
            //
            fresult = f_readdir(&g_sDirObject, &g_sFileInfo);

            //
            // Check for error and return if there is a problem.
            //
            if(fresult != FR_OK)
            {
                UARTprintf("E : Failed to read the directory from SD Card.\r\n");
                return(fresult);
            }

            //
            // If the file name is blank, then this is the end of the listing.
            //
            if(!g_sFileInfo.fname[0])
            {
                break;
            }

            //
            // If the attribute is directory, then increment the directory count.
            //
            if(g_sFileInfo.fattrib & AM_DIR)
            {
                ulDirCount++;
            }

            //
            // Otherwise, it is a file.  Increment the file count, and add in the
            // file size to the total.
            //
            else
            {
                ulFileCount++;
                ulTotalSize += g_sFileInfo.fsize;
            }
        }

        //
        // Get the free space.
        //
        fresult = f_getfree("/", &ulTotalSize, &pFatFs);

        //
        // Check for error and return if there is a problem.
        //
        if(fresult != FR_OK)
        {
            UARTprintf("E : Failed to get the free spaces from SD card.\r\n");
            return(fresult);
        }

        //
        // Display the amount of free space that was calculated.
        //
        free_byte = (ulTotalSize * pFatFs->sects_clust / 2);


        // Check less than 10MB
        while(free_byte < 10000)
        {
            UARTprintf("Not enough space to add file in sd_card.\r\n There must be more than 10MB\r\n");

            fresult = f_mount(0, &g_sFatFs);
            //
            // Open the current directory for access.
            //
            fresult = f_opendir(&g_sDirObject, g_cCwdBuf);

            //
            // Check for error and return if there is a problem.
            //
            if(fresult != FR_OK)
            {
                UARTprintf("E : Failed to read SD card. Check the SD card exists.\r\n");
                return(fresult);
            }

            ulTotalSize = 0;
            ulFileCount = 0;
            ulDirCount = 0;
            cur_time = 0;
            oldest_time = 0;
            file_start = 0;
            //
            // Enter loop to enumerate through all directory entries.
            //
            UARTprintf("Remove the oldest file to secure 10MB at least.\r\n");
            while(1)
            {
                //
                // Read an entry from the directory.
                //
                fresult = f_readdir(&g_sDirObject, &g_sFileInfo);

                //
                // Check for error and return if there is a problem.
                //
                if(fresult != FR_OK)
                {
                    UARTprintf("E : Failed to read the directory from SD Card.\r\n");
                    return(fresult);
                }

                //
                // If the file name is blank, then this is the end of the listing.
                //
                if(!g_sFileInfo.fname[0])
                {
                    break;
                }

                //
                // If the attribute is directory, then increment the directory count.
                //
                if(g_sFileInfo.fattrib & AM_DIR)
                {
                    ulDirCount++;
                }

                //
                // Otherwise, it is a file.  Increment the file count, and add in the
                // file size to the total.
                //
                else
                {
                    ulFileCount++;
                    ulTotalSize += g_sFileInfo.fsize;
                }

                cmp_name = strtok(g_sFileInfo.fname, 0);

                if(!file_start)
                {
                    if(strcmp(cmp_name,"SYSTEM~1") && strcmp(cmp_name,"log") && strcmp(cmp_name,"bootflag.ini"))
                    {
                        oldest_time = (((uint32_t)((g_sFileInfo.fdate >> 9) + 1980) - 2000) * 100000000)+
                        ((uint32_t)((g_sFileInfo.fdate >> 5) & 15) * 1000000)+
                        ((uint32_t)(g_sFileInfo.fdate & 31) * 10000)+
                        ((uint32_t)((g_sFileInfo.ftime >> 11)) * 100)+
                        ((uint32_t)((g_sFileInfo.ftime >> 5) & 63));

                        memcpy(oldest_name,g_sFileInfo.fname, sizeof(g_sFileInfo.fname));

                        file_start = 1;
                    }
                }
                else
                {
                    if(strcmp(cmp_name,"SYSTEM~1") && strcmp(cmp_name,"log") && strcmp(cmp_name,"bootflag.ini"))
                    {
                        cur_time = (((uint32_t)((g_sFileInfo.fdate >> 9) + 1980) - 2000) * 100000000)+
                        ((uint32_t)((g_sFileInfo.fdate >> 5) & 15) * 1000000)+
                        ((uint32_t)(g_sFileInfo.fdate & 31) * 10000)+
                        ((uint32_t)((g_sFileInfo.ftime >> 11)) * 100)+
                        ((uint32_t)((g_sFileInfo.ftime >> 5) & 63));

                        if(cur_time < oldest_time)
                        {
                            memcpy(oldest_name,g_sFileInfo.fname, sizeof(g_sFileInfo.fname));
                            oldest_time = cur_time;
                        }
                    }
                }

            }
            UARTprintf("Remove %s.\r\n",oldest_name);
            fresult = f_unlink(oldest_name);

            if(fresult != FR_OK)
            {
                UARTprintf("E : Failed to remove the file.\r\n");
                return(fresult);
            }
            //
            // Get the free space.
            //
            fresult = f_getfree("/", &ulTotalSize, &pFatFs);

            //
            // Check for error and return if there is a problem.
            //
            if(fresult != FR_OK)
            {
                UARTprintf("E : Failed to get the free spaces from SD card.\r\n");
                return(fresult);
            }

            //
            // Display the amount of free space that was calculated.
            //
            free_byte = (ulTotalSize * pFatFs->sects_clust / 2);

        }
        UARTprintf("There is free space more than 10MB in SD card.\r\n");

        //
        // Made it to here, return with no errors.
        //
        return(FR_OK);
}
