#ifndef HEADER_RTC_H_
#define HEADER_RTC_H_

#include "F28x_Project.h"
#include "Define.h"

#define SEC_FLAG    0x01
#define HHMM_FLAG   0x02
#define MMDD_FLAG   0x04
#define YYYY_FLAG   0x08

typedef struct
{
    char         ch[4];
    char         ch_on_off[4];
    int          ofs[4];
    int          max[4];
    float        data[4];
    char         module_on_off;
}
Spi_Info;

typedef struct
{
    int Read_sec;
    int Read_min;
    int Read_hour;
    int Read_hour_min;
    int Read_day;
    int Read_mon;
    int Read_mon_day;
    int Read_year;
}
Read_Time;


typedef struct
{
    int     Set_sec;
    int     Set_hour_min;
    int     Set_mon_day;
    int     Set_year;
    int     Wr_flag;
    int     Sel_Wr;
}
Write_Time;


extern Spi_Info spi_info;
extern Read_Time time;
extern Write_Time wr_time;


extern int rtc_read_data[6];
extern int Read_sec;
extern int Read_min;
extern int Read_hour;
extern int Read_hour_min;
extern int Read_day;
extern int Read_mon;
extern int Read_mon_day;
extern int Read_year;

extern WORD eep_addr;
extern BYTE eep_write_data;
extern BYTE eep_read_data;

extern Read_Time r_time;

void Send_Dac_Data(char _ch, float _data, char _on_off, int _ofs, int _max);
void Init_Rtc();
void Read_RX8025_Data(BYTE addr, BYTE *pData);
void I2C_RTC_init(Write_Time _data);
Read_Time Read_Rtc();

#endif /* HEADER_RTC_H_ */
