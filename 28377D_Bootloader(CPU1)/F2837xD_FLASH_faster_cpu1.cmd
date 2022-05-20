MEMORY
{
PAGE 0:    /* Program Memory */
          /* Memory (RAM/FLASH) blocks can be moved to PAGE1 for data allocation */
          /* BEGIN is used for the "boot to Flash" bootloader mode   */

   BEGIN           	: origin = 0x080000, length = 0x000002   // shin
   RAMM0           	: origin = 0x000122, length = 0x0002DE
   RAMD0           	: origin = 0x00B000, length = 0x000800

   RESET           	: origin = 0x3FFFC0, length = 0x000002

   RAMLS4_5      	: origin = 0x00A000, length = 0x001000
//   RAMLS5      	: origin = 0x00A800, length = 0x000800



	/* Flash sectors */
   FLASHA           : origin = 0x080002, length = 0x001FFE	/* on-chip Flash */
   FLASHB           : origin = 0x082000, length = 0x002000	/* on-chip Flash */
   FLASHC           : origin = 0x084000, length = 0x002000	/* on-chip Flash */
   FLASHD           : origin = 0x086000, length = 0x002000	/* on-chip Flash */
   FLASHE           : origin = 0x088000, length = 0x008000	/* on-chip Flash */
   FLASHF           : origin = 0x090000, length = 0x008000	/* on-chip Flash */
   FLASHG           : origin = 0x098000, length = 0x008000	/* on-chip Flash */
   FLASHH           : origin = 0x0A0000, length = 0x008000	/* on-chip Flash */
   FLASHI           : origin = 0x0A8000, length = 0x008000	/* on-chip Flash */
   FLASHJ           : origin = 0x0B0000, length = 0x008000	/* on-chip Flash */
   FLASHK           : origin = 0x0B8000, length = 0x002000	/* on-chip Flash */
   FLASHL           : origin = 0x0BA000, length = 0x002000	/* on-chip Flash */
   FLASHM           : origin = 0x0BC000, length = 0x002000	/* on-chip Flash */
   FLASHN           : origin = 0x0BE000, length = 0x002000	/* on-chip Flash */

PAGE 1 :   /* Data Memory */
         /* Memory (RAM/FLASH) blocks can be moved to PAGE0 for program allocation */

    BOOT_RSVD       : origin = 0x000002, length = 0x000120     /* Part of M0, BOOT rom will use this for stack */
	RAMM1           : origin = 0x000400, length = 0x000400     /* on-chip RAM block M1 */
    RAMD1           : origin = 0x00B800, length = 0x000800

   RAMLS0_1 	    : origin = 0x008000, length = 0x001000
//   RAMLS1         : origin = 0x008800, length = 0x000800
   RAMLS2_3         : origin = 0x009000, length = 0x001000
//   RAMLS3         : origin = 0x009800, length = 0x000800



   CPU2TOCPU1RAM   : origin = 0x03F800, length = 0x000400
   CPU1TOCPU2RAM   : origin = 0x03FC00, length = 0x000400
}


SECTIONS
{

   /* Allocate program areas: */
   codestart           : > BEGIN	   					PAGE = 0, 			ALIGN(8)
   .cinit              : > FLASHA      					PAGE = 0, 			ALIGN(8)
   .pinit              : > FLASHA     					PAGE = 0, 			ALIGN(8)
   .text               : >> FLASHA | FLASHB | FLASHC | FLASHD   	PAGE = 0,			ALIGN(8)


   /* Allocate uninitalized data sections: */
   .stack              : > RAMLS0_1   		    		PAGE = 1
   .ebss               : >> RAMLS2_3 | RAMD1 	        PAGE = 1
   .esysmem            : > RAMLS0_1       				PAGE = 1

   /* Initalized sections go in Flash */
   .econst             : > FLASHC     			PAGE = 0   // shin
   .switch             : > FLASHC     			PAGE = 0   // shin

   .reset              : > RESET                PAGE = 0, TYPE = DSECT /* not used, */
   .version			   : > FLASHC     			PAGE = 0




#ifdef __TI_COMPILER_VERSION__
    #if __TI_COMPILER_VERSION__ >= 15009000
        GROUP
        {
            .TI.ramfunc
            { -l F021_API_F2837xD_FPU32.lib}
            { Shared_Boot.obj (.text) }
            { uartstdio.obj (.text) }

        } LOAD = FLASHD,
          RUN  = RAMLS4_5,

          LOAD_START(_RamfuncsLoadStart),
          LOAD_SIZE(_RamfuncsLoadSize),
          LOAD_END(_RamfuncsLoadEnd),
          RUN_START(_RamfuncsRunStart),
          RUN_SIZE(_RamfuncsRunSize),
          RUN_END(_RamfuncsRunEnd),
          PAGE = 0
    #else
        GROUP
        {
            ramfuncs
            { -l F021_API_F2837xD_FPU32.lib}
            { Shared_Boot.obj (.text) }
            { uartstdio.obj (.text) }

        } LOAD = FLASHD,
          RUN  = RAMLS4_5,

          LOAD_START(_RamfuncsLoadStart),
          LOAD_SIZE(_RamfuncsLoadSize),
          LOAD_END(_RamfuncsLoadEnd),
          RUN_START(_RamfuncsRunStart),
          RUN_SIZE(_RamfuncsRunSize),
          RUN_END(_RamfuncsRunEnd),
          PAGE = 0
    #endif
#endif
   /* The following section definitions are required when using the IPC API Drivers */
    GROUP : > CPU1TOCPU2RAM, PAGE = 1
    {
        PUTBUFFER
        PUTWRITEIDX
        GETREADIDX
    }

    GROUP : > CPU2TOCPU1RAM, PAGE = 1
    {
        GETBUFFER :    TYPE = DSECT
        GETWRITEIDX :  TYPE = DSECT
        PUTREADIDX :   TYPE = DSECT
    }

}

/*
//===========================================================================
// End of file.
//===========================================================================
*/




