#ifndef HEADER_COMMON_H_
#define HEADER_COMMON_H_

#define NO_ERROR                    0x1000
#define BLANK_ERROR                 0x2000
#define VERIFY_ERROR                0x3000
#define PROGRAM_ERROR               0x4000
#define COMMAND_ERROR               0x5000
#define UNLOCK_ERROR                0x6000
#define CRC_ERROR                   0x7000


#define TIME (0xFFFFFFFF - CpuTimer0Regs.TIM.all)
#define TIME_RESET CpuTimer0Regs.TIM.all = 0xFFFFFFFF


void delay_us(long _cnt);



#endif /* HEADER_COMMON_H_ */
