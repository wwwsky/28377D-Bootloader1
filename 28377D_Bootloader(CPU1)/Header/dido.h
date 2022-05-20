/*
 * dido.h
 *
 *  Created on: 2020. 8. 25.
 *      Author: GP01
 */

#ifndef HEADER_DIDO_H_
#define HEADER_DIDO_H_

#define DI_1    (GpioDataRegs.GPADAT.bit.GPIO0)
#define DI_2    (GpioDataRegs.GPADAT.bit.GPIO1)
#define DI_3    (GpioDataRegs.GPADAT.bit.GPIO2)
#define DI_4    (GpioDataRegs.GPADAT.bit.GPIO3)
#define DI_5    (GpioDataRegs.GPADAT.bit.GPIO4)
#define DI_6    (GpioDataRegs.GPDDAT.bit.GPIO126)

#define DI_7    (GpioDataRegs.GPDDAT.bit.GPIO127)
#define DI_8    (GpioDataRegs.GPEDAT.bit.GPIO128)
#define DI_9    (GpioDataRegs.GPEDAT.bit.GPIO129)
#define DI_10   (GpioDataRegs.GPEDAT.bit.GPIO130)
#define DI_11   (GpioDataRegs.GPEDAT.bit.GPIO131)
#define DI_12   (GpioDataRegs.GPEDAT.bit.GPIO132)


#define Conector_1_2_3   (GpioDataRegs.GPADAT.bit.GPIO25)

#define DO_1_ON         (GpioDataRegs.GPDCLEAR.bit.GPIO106  = 1)
#define DO_1_OFF        (GpioDataRegs.GPDSET.bit.GPIO106    = 1)
#define DO_1_TOGGLE     (GpioDataRegs.GPDTOGGLE.bit.GPIO106 = 1)

#define DO_2_ON         (GpioDataRegs.GPDCLEAR.bit.GPIO107  = 1)
#define DO_2_OFF        (GpioDataRegs.GPDSET.bit.GPIO107    = 1)
#define DO_2_TOGGLE     (GpioDataRegs.GPDTOGGLE.bit.GPIO107 = 1)

#define DO_3_ON         (GpioDataRegs.GPDCLEAR.bit.GPIO108  = 1)
#define DO_3_OFF        (GpioDataRegs.GPDSET.bit.GPIO108    = 1)
#define DO_3_TOGGLE     (GpioDataRegs.GPDTOGGLE.bit.GPIO108 = 1)

#define DO_4_ON         (GpioDataRegs.GPDCLEAR.bit.GPIO109  = 1)
#define DO_4_OFF        (GpioDataRegs.GPDSET.bit.GPIO109    = 1)
#define DO_4_TOGGLE     (GpioDataRegs.GPDTOGGLE.bit.GPIO109 = 1)

#define DO_5_ON         (GpioDataRegs.GPDCLEAR.bit.GPIO110  = 1)
#define DO_5_OFF        (GpioDataRegs.GPDSET.bit.GPIO110    = 1)
#define DO_5_TOGGLE     (GpioDataRegs.GPDTOGGLE.bit.GPIO110 = 1)

#define DO_6_ON         (GpioDataRegs.GPDCLEAR.bit.GPIO111  = 1)
#define DO_6_OFF        (GpioDataRegs.GPDSET.bit.GPIO111    = 1)
#define DO_6_TOGGLE     (GpioDataRegs.GPDTOGGLE.bit.GPIO111 = 1)

#define DO_7_ON         (GpioDataRegs.GPACLEAR.bit.GPIO5    = 1)
#define DO_7_OFF        (GpioDataRegs.GPASET.bit.GPIO5      = 1)
#define DO_7_TOGGLE     (GpioDataRegs.GPATOGGLE.bit.GPIO5   = 1)

#define DO_8_ON         (GpioDataRegs.GPACLEAR.bit.GPIO7    = 1)
#define DO_8_OFF        (GpioDataRegs.GPASET.bit.GPIO7      = 1)
#define DO_8_TOGGLE     (GpioDataRegs.GPATOGGLE.bit.GPIO7   = 1)

#define DO_9_ON         (GpioDataRegs.GPACLEAR.bit.GPIO9    = 1)
#define DO_9_OFF        (GpioDataRegs.GPASET.bit.GPIO9      = 1)
#define DO_9_TOGGLE     (GpioDataRegs.GPATOGGLE.bit.GPIO9   = 1)

#define DO_10_ON         (GpioDataRegs.GPACLEAR.bit.GPIO11  = 1)
#define DO_10_OFF        (GpioDataRegs.GPASET.bit.GPIO11    = 1)
#define DO_10_TOGGLE     (GpioDataRegs.GPATOGGLE.bit.GPIO11 = 1)

#define DO_11_ON         (GpioDataRegs.GPACLEAR.bit.GPIO15  = 1)
#define DO_11_OFF        (GpioDataRegs.GPASET.bit.GPIO15    = 1)
#define DO_11_TOGGLE     (GpioDataRegs.GPATOGGLE.bit.GPIO15 = 1)

#define DO_12_ON         (GpioDataRegs.GPACLEAR.bit.GPIO24  = 1)
#define DO_12_OFF        (GpioDataRegs.GPASET.bit.GPIO24    = 1)
#define DO_12_TOGGLE     (GpioDataRegs.GPATOGGLE.bit.GPIO24 = 1)

#define DO_13_ON         (GpioDataRegs.GPACLEAR.bit.GPIO21  = 1)
#define DO_13_OFF        (GpioDataRegs.GPASET.bit.GPIO21    = 1)
#define DO_13_TOGGLE     (GpioDataRegs.GPATOGGLE.bit.GPIO21 = 1)

#define DO_14_ON         (GpioDataRegs.GPACLEAR.bit.GPIO22  = 1)
#define DO_14_OFF        (GpioDataRegs.GPASET.bit.GPIO22    = 1)
#define DO_14_TOGGLE     (GpioDataRegs.GPATOGGLE.bit.GPIO22 = 1)

#define DO_15_ON         (GpioDataRegs.GPACLEAR.bit.GPIO23  = 1)
#define DO_15_OFF        (GpioDataRegs.GPASET.bit.GPIO23    = 1)
#define DO_15_TOGGLE     (GpioDataRegs.GPATOGGLE.bit.GPIO23 = 1)

#define DO_16_ON         (GpioDataRegs.GPBCLEAR.bit.GPIO33  = 1)
#define DO_16_OFF        (GpioDataRegs.GPBSET.bit.GPIO33    = 1)
#define DO_16_TOGGLE     (GpioDataRegs.GPBTOGGLE.bit.GPIO33 = 1)

#define LED_1_ON         (GpioDataRegs.GPECLEAR.bit.GPIO143  = 1)
#define LED_1_OFF        (GpioDataRegs.GPESET.bit.GPIO143    = 1)
#define LED_1_TOGGLE     (GpioDataRegs.GPETOGGLE.bit.GPIO143 = 1)


#define LED_2_ON         (GpioDataRegs.GPECLEAR.bit.GPIO144  = 1)
#define LED_2_OFF        (GpioDataRegs.GPESET.bit.GPIO144    = 1)
#define LED_2_TOGGLE     (GpioDataRegs.GPETOGGLE.bit.GPIO144 = 1)


void Init_Dido();
void Do_off();


#endif /* HEADER_DIDO_H_ */
