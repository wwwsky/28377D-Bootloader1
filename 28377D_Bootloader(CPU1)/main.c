#include "F28x_Project.h"
#include <string.h>
#include "utils/cmdline.h"
#include "utils/uartstdio.h"
#include "Shared_Boot.h"
#include "common.h"
#include "eeprom.h"
#include "sci.h"
#include "cmd.h"
#include "rtc.h"
#include "log.h"

#define EXIT_KEY_ESC    0x7F

//LED1
#define LED1_ON         GpioDataRegs.GPEDAT.bit.GPIO143 = 0
#define LED1_OFF        GpioDataRegs.GPEDAT.bit.GPIO143 = 1
#define LED1_TOGGLE     GpioDataRegs.GPETOGGLE.bit.GPIO143 = 1


extern char g_cCmdBuf[CMD_BUF_SIZE];

char g_cTmpBuf[20];

char r_start = 0;
char w_start = 0;

void Init_Sdram(void);
void Init_CpuTimer();
void Check_Shutdown();
void Read_Rtc0_start(Read_Time *data);
void Write_Rtc0_start(Read_Time *data);
void Read_Rtc0(void);
void Write_Rtc0(void);
void Ctrl_Led(void);

Read_Time test_r;
Read_Time test_w;

int main(void)
{
    EALLOW;
    DevCfgRegs.CPUSEL5.bit.SCI_A = 0;
    DevCfgRegs.CPUSEL6.bit.SPI_C = 0;
    EDIS;

    //Initialize system.
    InitSysCtrl();
    InitGpio();
    InitPieCtrl();
    InitPieVectTable();
    EnableInterrupts();

    Init_Sdram();

    /* where boot from */
    Cmd_bootflagGet();

    Init_Flash_Sectors();

    Init_Dido();
    Init_CpuTimer();
    Init_Sci();
    Init_Rtc();
    Init_I2c_Eeprom();


    //Check whether the power was suddenly off.
    Check_Shutdown();

    int iStatus = 0;
    r_start = 1;

    while(1)
    {
        //1.1 Check and get buffer from SCI comm .
        iStatus = ReadLine();

        //Check and write log buffer to 'log' file.
        log_write();

        if(r_start)
        {
            Read_Rtc0_start(&test_r);
            r_start = 0;
        }

        else if(w_start)
        {
            Write_Rtc0_start(&test_w);
            w_start = 0;
        }

        Read_Rtc0();
//        Write_Rtc0();

        //LED process.
        Ctrl_Led();

        //1.2 Check if a command is ready to run by 'ReadLine' function.
        if(iStatus != 1)
            continue;

        //1.3 Carry out the command.
        iStatus = CmdLineProcess(g_cCmdBuf);

        //1.4 Clear command buffer.
        memset(g_cCmdBuf, 0, sizeof(g_cCmdBuf));
        memset(g_cTmpBuf, 0, sizeof(g_cTmpBuf));

    }
}


void delay_us(long _cnt)
{
    CpuTimer0Regs.TIM.all = 0xFFFFFFFF;
    while ((0xFFFFFFFF - CpuTimer0Regs.TIM.all) <= _cnt)
        ;
}

void Init_CpuTimer()
{
    EALLOW;
    CpuTimer0Regs.PRD.all = 0xFFFFFFFF;
    CpuTimer0Regs.TPR.bit.TDDR = 0xd0;
    CpuTimer0Regs.TPRH.bit.TDDRH = 0x07;

    CpuTimer1Regs.PRD.all = 0xFFFFFFFF;
    CpuTimer1Regs.TPR.bit.TDDR = 200 - 1; // 200 * 5ns = 1us
    CpuTimer1Regs.TPRH.bit.TDDRH = 0;
    EDIS;
}


void Waiting_Bl()
{
    int  ch = 0;
    unsigned long timer = 0;

    //Exit key : [0x7F]
    //If the bootloader entry command is detected while the timer is running,
    //it exits the function without executing a jump command.
    while(timer < 4000000)
    {
        ch = Get_Buff();

        if(ch == 0x7F)
        {
            return;
        }

        timer++;
    }

    //Time out. Move to main F/W.
    Cmd_jump(0, 0);
}

void Check_Shutdown()
{
    memset(g_cCmdBuf, 0, sizeof(g_cCmdBuf));
    f_mount(0, &g_sFatFs);

    //Check 'update2.ini' exists.
    //If Power was suddenly off while it update, 'update2.ini' file will be still remain in SD_Card.
    if(FR_OK == f_open(&g_sFileObject, "update1.ini", FA_READ))
    {
        UARTprintf("\r\nE : Power removed during update.");
        UARTprintf("\r\nTry the update again.\r\n");

        //Read the latest flash command in 'update.ini' file.
        fresult = f_read(&g_sFileObject, g_cTmpBuf, sizeof(g_cTmpBuf) - 1, &usBytesRead);
        fresult = f_close(&g_sFileObject);

        //Add 'SP' command to add option.
        g_cTmpBuf[2] = 32;

        //Add option 'e' for flash command.
        strcat(g_cTmpBuf, " e");

        CmdLineProcess(g_cTmpBuf);
    }
    else
    {
        f_mount(0, NULL);

        //Check jumping to main F/W or starting boot loader.
        Waiting_Bl();

        //Boot loader is started.
        UARTprintf("\r\nG-Philos 28377D CPU1 BootLoader v1.0\r\n");
        UARTprintf("#");
    }
}

int led_status = 1;

void Ctrl_Led(void)
{
    static unsigned long pre_time0 = 0xFFFFFFFF;
    static unsigned long pre_time = 0xFFFFFFFF;
    static unsigned long period = 250000;
    static unsigned long led_count = 0;
    static unsigned long one = 1;

    if(pre_time - CpuTimer1Regs.TIM.all > period)  // 500000 == 500ms
    {
        pre_time = CpuTimer1Regs.TIM.all;
        LED1_TOGGLE;

        led_count++;

        if(led_count >= led_status)
        {
            period = 2000000;
            led_count = 0;
        }
        else
            period = 250000;
    }

    if(led_status != 1)
    {
        if(one)
        {
            pre_time0 = CpuTimer1Regs.TIM.all;
            one = 0;
        }
        if(pre_time0 - CpuTimer1Regs.TIM.all > 30000000)  // 30s
        {
            pre_time0 = CpuTimer1Regs.TIM.all;
            led_status = 1;
            one = 1;
        }
    }
}
