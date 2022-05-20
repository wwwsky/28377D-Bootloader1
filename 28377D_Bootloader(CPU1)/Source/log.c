/*
 * log.c
 *
 *  Created on: 2021. 5. 28.
 *      Author: K.J.G
 */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "fatfs/src/ff.h"
#include "utils/uartstdio.h"
#include "F2837xD_device.h"
#include "rtc.h"


#define MAX_LOG 10

extern unsigned short usBytesWritten;
extern FRESULT fresult;

static char que_log[MAX_LOG][96];
static char arg_buff[32];
static char log_w = 0;
static char log_r = 0;
static char cpu_t = 0;
static volatile unsigned long pre_cpu_t = 0;



void L_printf(const char* func, char line, char _lev, const char *fmt, ...)
{
    Uint16 new_w = log_w + 1;

    if(new_w >= MAX_LOG)
            new_w = 0;

    if(new_w == log_r)
    {
        UARTprintf("log buff full.\r\n");
    }
    else
    {
        unsigned long crt_cpu_t = CpuTimer1Regs.TIM.all;
        char str_buff[64];

        memset(str_buff, 0, sizeof(str_buff));
        va_list ap;
        va_start(ap, fmt);
        vsprintf(str_buff, fmt, ap);
        va_end(vaArgP);

        r_time = Read_Rtc();

        if(cpu_t == 0)
            pre_cpu_t = crt_cpu_t;

        sprintf(que_log[new_w],"%2d-%02d-%02d %02d:%02d:%02d, %s:%d, %d, %ld, ", r_time.Read_year - 2000, r_time.Read_mon, r_time.Read_day
                                                    , r_time.Read_hour, r_time.Read_min, r_time.Read_sec, func, line, _lev, pre_cpu_t - crt_cpu_t);
        strcat(que_log[new_w], str_buff);

        char *ptr0 = strstr(que_log[new_w], "\r\n");
        char *ptr1 = strstr(que_log[new_w], "\n\r");

        if(ptr0 == NULL && ptr1 == NULL)
            strcat(que_log[new_w], "\r\n");

        log_w = new_w;
    }
    cpu_t = 1;
    pre_cpu_t = CpuTimer1Regs.TIM.all;
}

void log_write(void)
{
    FIL LogObject;

    fresult = f_open(&LogObject, "log", FA_OPEN_ALWAYS | FA_WRITE);

    fresult = f_lseek(&LogObject, LogObject.fsize);

    while(1)
    {
        int new_log_r = log_r + 1;

        if(new_log_r >= MAX_LOG)
            new_log_r = 0;

        if(log_r == log_w)
        {
            break;
        }
        else
        {
            fresult = f_write(&LogObject, que_log[new_log_r], strlen(que_log[new_log_r]) , &usBytesWritten);
            log_r = new_log_r;
        }
    }
    cpu_t = 0;
    pre_cpu_t = CpuTimer1Regs.TIM.all;
    fresult = f_close(&LogObject);
}


char* arg_cat(int argc, char *argv[])
{
    int i = 0;

    memset(arg_buff, 0, sizeof(arg_buff));

    for(i = 0; i < argc; i++)
    {
        strcat(arg_buff, argv[i]);
        strcat(arg_buff, " ");
    }

    if(argc == 0)
        memcpy(arg_buff, "None", sizeof("None"));

    return arg_buff;
}

extern char r_start;
extern char w_start;

/***********************R E A D********************************/

void Read_Rtc0_start(Read_Time *data);
void Read_Rtc0_state1(void);
void Read_Rtc0_state2(void);
void Read_Rtc0_state3(void);
void Read_Rtc0_state4(void);
void Read_Rtc0_state5(void);
void Read_Rtc0_state6(void);
void Read_Rtc0_state7(void);
void Read_Rtc0_state8(void);

static Read_Time *g_data;
static void (*Read_Rtc0_table[])(void) =
{
     Read_Rtc0_state1,
     Read_Rtc0_state2,
     Read_Rtc0_state3,
     Read_Rtc0_state4,
     Read_Rtc0_state5,
     Read_Rtc0_state6,
     Read_Rtc0_state7,
     Read_Rtc0_state8
};

static char state = 0;
static short unsigned int i2c_state_r = 0;
static unsigned char data_addr = 0;

#define LED2_ON         GpioDataRegs.GPEDAT.bit.GPIO144 = 0;
#define LED2_OFF        GpioDataRegs.GPEDAT.bit.GPIO144 = 1;
#define LED2_TOGGLE     GpioDataRegs.GPETOGGLE.bit.GPIO144 = 1;

void Read_Rtc0(void)
{
    if(i2c_state_r & 0x10)
    {
       // LED2_ON
        Read_Rtc0_table[state]();
        //LED2_OFF
    }
    else if(i2c_state_r)
    {
        if(i2c_state_r & 0x01)
        {
            I2cbRegs.I2CSAR.all = 0x32;

            I2cbRegs.I2CCNT = 1;
            // Setup data to send
            I2cbRegs.I2CDXR.all = data_addr << 4; // Data Address

            I2cbRegs.I2CMDR.all = 0x2620;
            i2c_state_r <<= 2;
        }
        else if((i2c_state_r & 0x04) !=0)
        {
            if (!I2cbRegs.I2CMDR.bit.STP)
            {
                I2cbRegs.I2CMDR.all = 0x2C20;

                i2c_state_r <<= 1;
            }
        }
        else if(i2c_state_r & 0x08)
        {
            if (I2cbRegs.I2CSTR.bit.SCD)
            {
                I2cbRegs.I2CSTR.bit.SCD = 1;

                eep_read_data = (BYTE)(I2cbRegs.I2CDRR.all & 0xff);
                i2c_state_r <<= 1;
            }
        }
        else
        {
            // no work
        }
    }
}

void Read_Rtc0_start(Read_Time *data)
{
    g_data = data;

    data_addr = 0x00;
    i2c_state_r = 1;
}


void Read_Rtc0_state1(void)
{
    rtc_read_data[0] = eep_read_data - 6 * (eep_read_data >> 4);
    data_addr = 0x01;
    i2c_state_r = 1;
    state = 1;
}

void Read_Rtc0_state2(void)
{
    rtc_read_data[1] = eep_read_data - 6 * (eep_read_data >> 4);
    data_addr = 0x02;
    i2c_state_r = 1;
    state = 2;
}

void Read_Rtc0_state3(void)
{
    rtc_read_data[2] = eep_read_data - 6 * (eep_read_data >> 4);
    data_addr = 0x04;
    i2c_state_r = 1;
    state = 3;
}

void Read_Rtc0_state4(void)
{
    rtc_read_data[3] = eep_read_data - 6 * (eep_read_data >> 4);
    data_addr = 0x05;
    i2c_state_r = 1;
    state = 4;
}

void Read_Rtc0_state5(void)
{
    rtc_read_data[4] = eep_read_data - 6 * (eep_read_data >> 4);
    data_addr = 0x06;
    i2c_state_r = 1;
    state = 5;
}

void Read_Rtc0_state6(void)
{
    rtc_read_data[5] = eep_read_data - 6 * (eep_read_data >> 4);
    state = 6;
}


void Read_Rtc0_state7(void)
{
    register Read_Time *data = g_data;
    register int *r_rtc = rtc_read_data;

    data->Read_sec = *r_rtc++;
    data->Read_min = *r_rtc++;
    data->Read_hour = *r_rtc++;
    data->Read_day = *r_rtc++;
    data->Read_mon = *r_rtc++;
    data->Read_year = *r_rtc++;

    state = 7;
}

void Read_Rtc0_state8(void)
{
    register Read_Time *data = g_data;

    data->Read_hour_min = data->Read_hour * 100 + data->Read_min;
    data->Read_mon_day = data->Read_mon * 100 + data->Read_day;
    data->Read_year = data->Read_year + 2000;

    i2c_state_r = 0;
    state = 0;
    r_start = 1;
}



/*********************** W R I T E ********************************/

static unsigned char write_data = 0;
static short unsigned int i2c_state_w = 0x00;

void Write_Rtc0_state1(void);
void Write_Rtc0_state2(void);
void Write_Rtc0_state3(void);
void Write_Rtc0_state4(void);
void Write_Rtc0_state5(void);
void Write_Rtc0_state6(void);

static void (*Write_Rtc0_table[])(void) =
{
     Write_Rtc0_state1,
     Write_Rtc0_state2,
     Write_Rtc0_state3,
     Write_Rtc0_state4,
     Write_Rtc0_state5,
     Write_Rtc0_state6
};

void Write_Rtc0(void)
{
    if(i2c_state_w & 0x02)
    {
        Write_Rtc0_table[state]();
    }
    else if(i2c_state_w)
    {
        if(I2cbRegs.I2CMDR.bit.STP == 1)   return ;
        if(I2cbRegs.I2CSTR.bit.BB == 1)    return ;

        // Set Device(Slave) Address
        I2cbRegs.I2CSAR.all = 0x32;

        // Setup number of bytes to send
        I2cbRegs.I2CCNT = 2;

        // Setup data to send
        I2cbRegs.I2CDXR.all = data_addr << 4; // Data Address
        I2cbRegs.I2CDXR.all = write_data;      // Write Data

        // Send start as master transmitter with STT(=1), STP(=1), XA(=0), RM(=0)
        I2cbRegs.I2CMDR.all = 0x6E20;
        i2c_state_w <<= 1;
    }
}

void Write_Rtc0_start(Read_Time *data)
{
    g_data = data;

    write_data = ((int)(g_data->Read_sec * 0.1) << 4) + (g_data->Read_sec % 10);  // 652ns

    data_addr = 0x00;
    i2c_state_w = 1;
    state = 0;
}

void Write_Rtc0_state1(void)
{
    register unsigned char data = g_data->Read_min;
    write_data = ((int)(data * 0.1) << 4) + (data % 10);  // 652ns
    data_addr = 0x01;
    i2c_state_w = 1;
    state = 1;
}

void Write_Rtc0_state2(void)
{
    register unsigned char data = g_data->Read_hour;
    write_data = ((int)(data * 0.1) << 4) + (data % 10);  // 652ns
    data_addr = 0x02;
    i2c_state_w = 1;
    state = 2;
}

void Write_Rtc0_state3(void)
{
    register unsigned char data = g_data->Read_day;
    write_data = ((int)(data * 0.1) << 4) + (data % 10);  // 652ns
    data_addr = 0x04;
    i2c_state_w = 1;
    state = 3;
}

void Write_Rtc0_state4(void)
{
    register unsigned char data = g_data->Read_mon;
    write_data = ((int)(data * 0.1) << 4) + (data % 10);  // 652ns
    data_addr = 0x05;
    i2c_state_w = 1;
    state = 4;
}

void Write_Rtc0_state5(void)
{
    register unsigned char data = g_data->Read_year - 2000;
    write_data = ((char)(data * 0.1) << 4) + (data % 10);
    data_addr = 0x06;
    i2c_state_w = 1;
    state = 5;
}

void Write_Rtc0_state6(void)
{
    i2c_state_w = 0;
    state = 0;
}
