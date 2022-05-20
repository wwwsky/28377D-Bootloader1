//###########################################################################
//
// FILE:   emif1_16bit_sdram_far.c
//
// TITLE:  EMIF1 module accessing 32bit SDRAM using memcpy_fast_far()
//
//! \addtogroup cpu01_example_list
//! <h1> EMIF1 SDRAM Module (emif1_16bit_sdram_far)</h1>
//!
//! This example configures EMIF1 in 16bit SDRAM mode and uses
//! CS0 (SDRAM) as chip enable. It will first write to an array
//! in the SDRAM and then read it back using the FPU function,
//! memcpy_fast_far(), for both operations.
//! The buffer in SDRAM will be placed in the .farbss memory on
//! account of the fact that its assigned the attribute "far"
//! indicating it lies beyond the 22-bit program address space.
//! The compiler will take care to avoid using instructions such
//! as PREAD, which uses the Program Read Bus, or addressing
//! modes restricted to the lower 22-bit space when accessing
//! data with the attribute "far"
//! \note The memory space beyond 22-bits must be treated as data space
//! for load/store operations only. The user is cautioned against using
//! this space for either instructions or working memory.
//!
//! \b Watch \b Variables: \n
//! - \b TEST_STATUS - Equivalent to \b TEST_PASS if test finished correctly,
//!                    else the value is set to \b TEST_FAIL
//! - \b ErrCount - Error counter
//!
//
//
//###########################################################################
// $TI Release: F2837xD Support Library v3.06.00.00 $
// $Release Date: Mon May 27 06:48:24 CDT 2019 $
// $Copyright:
// Copyright (C) 2013-2019 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//   Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
//
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the
//   distribution.
//
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $
//###########################################################################

//
// Included Files
//

#include "F2837xD_device.h"

void setup_emif1_pinmux_sdram_32bit(Uint16 cpu_sel);


void Init_Sdram(void)
{
    Uint16 ErrCount = 0;
    //
    // Configure to run EMIF1 on half Rate (EMIF1CLK = CPU1SYSCLK/2)
    //
    EALLOW;
    ClkCfgRegs.PERCLKDIVSEL.bit.EMIF1CLKDIV = 0x1;
    EDIS;

    EALLOW;
    //
    // Grab EMIF1 For CPU1
    //
    Emif1ConfigRegs.EMIF1MSEL.all = 0x93A5CE71;
    if (Emif1ConfigRegs.EMIF1MSEL.all != 0x1)
    {
        ErrCount++;
    }

    //
    // Disable Access Protection (CPU_FETCH/CPU_WR/DMA_WR)
    //
    Emif1ConfigRegs.EMIF1ACCPROT0.all = 0x0;
    if (Emif1ConfigRegs.EMIF1ACCPROT0.all != 0x0)
    {
        ErrCount++;
    }

    //
    // Commit the configuration related to protection. Till this bit remains
    // set content of EMIF1ACCPROT0 register can't be changed.
    //
    Emif1ConfigRegs.EMIF1COMMIT.all = 0x1;
    if (Emif1ConfigRegs.EMIF1COMMIT.all != 0x1)
    {
        ErrCount++;
    }

    //
    // Lock the configuration so that EMIF1COMMIT register can't be changed
    // any more.
    //
    Emif1ConfigRegs.EMIF1LOCK.all = 0x1;
    if (Emif1ConfigRegs.EMIF1LOCK.all != 1)
    {
        ErrCount++;
    }

    EDIS;

    //
    // Configure GPIO pins for EMIF1
    //
    //    setup_emif1_pinmux_sdram_16bit(0); // 32bit used _ KYJ
    setup_emif1_pinmux_sdram_32bit(0);
    //
    // Configure SDRAM control registers
    //
    // Need to be programmed based on SDRAM Data-Sheet.
    //T_RFC = 60ns = 0x6
    //T_RP  = 18ns = 0x1
    //T_RCD = 18ns = 0x1
    //T_WR  = 1CLK + 6 ns = 0x1
    //T_RAS = 42ns = 0x4
    //T_RC  = 60ns = 0x6
    //T_RRD = 12ns = 0x1
    //
    Emif1Regs.SDRAM_TR.all = 0x31114610;

    //
    //Txsr = 70ns = 0x7
    //
    Emif1Regs.SDR_EXT_TMNG.all = 0x7;

    //
    //Tref = 64ms for 8192 ROW, RR = 64000*100(Tfrq)/8192 = 781.25 (0x30E)
    //
    //    Emif1Regs.SDRAM_RCR.all = 0x30E;
    Emif1Regs.SDRAM_RCR.all = 0x61B; // 32bit used _KYJ

    //
    //PAGESIZE=2 (1024 elements per ROW), IBANK = 2 (4 BANK), CL = 3,
    //NM = 1 (16bit)
    //
    //    Emif1Regs.SDRAM_CR.all = 0x00015622;
    Emif1Regs.SDRAM_CR.all = 0x00000721; // 32bit used _KYJ
}
