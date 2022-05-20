#ifndef DEFINE_H
#define DEFINE_H

#include "fatfs/src/integer.h"
#include "F2837xD_device.h"
#include "F2837xD_Examples.h"
#include "dido.h"
#define US  (1e-6)
#define MS  (1e-3)
#define S  (1)

#define SYSCLK              200E6
#define TXCOUNT             100
#define MSG_DATA_LENGTH     2

#define NOP				asm("	NOP")

#define HIGH			1
#define LOW				0

// Bit 연산시 일반적으로 쓰이는 부분을 매크로 함수로 정의함
#define BIT_MASK(bit)				(1 << (bit))
#define GetBit(val, bit)			(((val) & BIT_MASK(bit)) >> (bit))
#define SetBit(val, bit)			(val |= BIT_MASK(bit))
#define ClearBit(val, bit)			(val &= ~BIT_MASK(bit))
#define ToggleBit(val, bit)			(val ^= BIT_MASK(bit))
#define bit_is_set(val, bit)		(val & BIT_MASK(bit))
#define bit_is_clear(val, bit)		(~val & BIT_MASK(bit))

/* Bit 위치 정의(주로 위에서 정의한 매크로 함수에서 사용하기 위해 정의함) */
#define BIT0_POS    	0
#define BIT1_POS    	1
#define BIT2_POS    	2
#define BIT3_POS    	3
#define BIT4_POS    	4
#define BIT5_POS    	5
#define BIT6_POS    	6
#define BIT7_POS    	7
#define BIT8_POS    	8
#define BIT9_POS    	9
#define BIT10_POS   	10
#define BIT11_POS   	11
#define BIT12_POS   	12
#define BIT13_POS   	13
#define BIT14_POS   	14
#define BIT15_POS   	15

/* Bit Mask Data define */
#define BIT0_MASK    	0x0001
#define BIT1_MASK    	0x0002
#define BIT2_MASK    	0x0004
#define BIT3_MASK    	0x0008
#define BIT4_MASK    	0x0010
#define BIT5_MASK    	0x0020
#define BIT6_MASK    	0x0040
#define BIT7_MASK    	0x0080
#define BIT8_MASK    	0x0100
#define BIT9_MASK    	0x0200
#define BIT10_MASK   	0x0400
#define BIT11_MASK   	0x0800
#define BIT12_MASK   	0x1000
#define BIT13_MASK   	0x2000
#define BIT14_MASK   	0x4000
#define BIT15_MASK   	0x8000


#endif // end of DEFINE_H definition
