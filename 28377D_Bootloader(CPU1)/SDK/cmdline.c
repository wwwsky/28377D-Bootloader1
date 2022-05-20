//
// Included Files
//
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "utils/cmdline.h"
#include "utils/uartstdio.h"
#include "fatfs/src/ff.h"
#include "log.h"

//
// Defines the maximum number of arguments that can be parsed.
//
#ifndef CMDLINE_MAX_ARGS
#define CMDLINE_MAX_ARGS        8
#endif

//
// An array to hold the pointers to the command line arguments.
//
static char *g_ppcArgv[CMDLINE_MAX_ARGS + 1];


int CmdLineProcess(char *pcCmdLine)
{
    int CStatus;
    char *pcChar;
    uint_fast8_t ui8Argc;
    bool bFindArg = true;
    tCmdLineEntry *psCmdEntry;
    //
    // Initialize the argument counter, and point to the beginning of the
    // command line string.
    //
    ui8Argc = 0;
    pcChar = pcCmdLine;

    //
    // Advance through the command line until a zero character is found.
    //
    while(*pcChar)
    {
        //
        // If there is a space, then replace it with a zero, and set the flag
        // to search for the next argument.
        //
        if(*pcChar == ' ' || *pcChar == 0)
        {
            *pcChar = 0;
            bFindArg = true;
        }

        //
        // Otherwise it is not a space, so it must be a character that is part
        // of an argument.
        //
        else
        {
            //
            // If bFindArg is set, then that means we are looking for the start
            // of the next argument.
            //
            if(bFindArg)
            {
                //
                // As long as the maximum number of arguments has not been
                // reached, then save the pointer to the start of this new arg
                // in the argv array, and increment the count of args, argc.
                //
                if(ui8Argc < CMDLINE_MAX_ARGS)
                {
                    g_ppcArgv[ui8Argc] = pcChar;
                    ui8Argc++;
                    bFindArg = false;
                }

                //
                // The maximum number of arguments has been reached so return
                // the error.
                //
                else
                {
                    return(CMDLINE_TOO_MANY_ARGS);
                }
            }
        }

        //
        // Advance to the next character in the command line.
        //
        pcChar++;
    }

    //
    // If one or more arguments was found, then process the command.
    //
    CStatus = CMDLINE_BAD_CMD;
    if(ui8Argc)
    {
        //
        // Start at the beginning of the command table, to look for a matching
        // command.
        //
        psCmdEntry = &g_psCmdTable[0];

        //
        // Search through the command table until a null command string is
        // found, which marks the end of the table.
        //
        while(psCmdEntry->pcCmd)
        {
            //
            // If this command entry command string matches argv[0], then call
            // the function for this command, passing the command line
            // arguments.
            //
            if(!strcmp(g_ppcArgv[0], psCmdEntry->pcCmd))
            {
                Lprintf(0, arg_cat(ui8Argc, g_ppcArgv));
                CStatus = psCmdEntry->pfnCmd(ui8Argc, g_ppcArgv);

                break;
            }

            //
            // Not found, so advance to the next entry.
            //
            psCmdEntry++;
        }
    }



    UARTprintf("#");
    //
    // Fall through to here means that no matching command was found, so return
    // an error.
    //
    return(CStatus);
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

//
// End of file
//
