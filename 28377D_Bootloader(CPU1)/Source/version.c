/*
 * version.c
 *
 *  Created on: 2021. 4. 7.
 *      Author: j.g.choe
 *
 *  Software Version Manager 2021. 4. 7, (c) G-Philos
 *
 */
#include <stdlib.h>
#include "version.h"
#include "F28x_Project.h"
#include "fatfs/src/ff.h"
#include "utils/uartstdio.h"

extern FIL g_sFileObject;

extern unsigned short usBytesRead;
extern FRESULT fresult;

const __attribute__((section(".version"))) char g_version[9][32] =
    {
        "G-PHILOS",
        CORE_ID,
        FIRMWARE_ID,
        VERSION_MAJOR "." VERSION_MINOR,
        MODEL_ID,
        CUSTOMER_ID,
        CPU_ID,
        BOARD_ID,
        __DATE__ " " __TIME__,
};

char Get_Flash_version(char *version)
{
    Uint32 *start_addr = (Uint32 *)0x088000;
    Uint32 data = 0;
    char buffer[2];

    Uint16 state = 0;
    char fcpy = 0;
    int vcnt = 0;
    int ret = 0;

    if ( *start_addr != 0xFFFFFFFF)
    {
        do
        {
            data = *start_addr;

            buffer[0] = data & 0xFF;
            buffer[1] = (data >> 16) & 0xFF;

            switch (state)
            {
            case 0:
                if (buffer[0] == 'G' && buffer[1] == '-')
                    state = 1;
                else
                    state = 0;
                break;

            case 1:
                if (buffer[0] == 'P' && buffer[1] == 'H')
                    state = 2;
                else
                    state = 0;
                break;

            case 2:
                if (buffer[0] == 'I' && buffer[1] == 'L')
                    state = 3;
                else
                    state = 0;
                break;

            case 3:
                if (buffer[0] == 'O' && buffer[1] == 'S')
                    state = 4;
                else
                    state = 0;
                break;

            case 4:
                if (buffer[0] != 0x00 || buffer[1] != 0x00)
                    fcpy = 1;

                if (fcpy)
                {
                    *version++ = buffer[0];
                    *version++ = buffer[1];
                    vcnt++;
                    if (128 <= vcnt)
                        state = 5;
                }
                break;

            default:
                break;
            }

            if (state >= 5)
                break;
        }
        while (start_addr++ < (Uint32 *)0x0C0000);
    }

    f_lseek(&g_sFileObject, 0);

    if (state == 5)
        ret = 1;

    return ret;
}

char Get_File_version(char *version)
{
    unsigned long fcnt = 0, fsize = g_sFileObject.fsize;

    int state = 0, ret = 1, vcnt = 0;

    char readbyte[2];
    char fcpy = 0;


    while (fcnt <= fsize)
    {
        fresult = f_read(&g_sFileObject, readbyte, 2, &usBytesRead);

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

        case 8:
            if (readbyte[0] != 0x00)
                fcpy = 1;

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

    f_lseek(&g_sFileObject, 0);

    if (state != 9)
        ret = 0;

    return ret;
}
