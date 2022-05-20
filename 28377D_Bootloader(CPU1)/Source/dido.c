//###############################################################################################################
// 파일명      : dido.c
// 회사명      : G-Philos
// 작성날짜  : 2020.12.17
// 작성자      : KYJ-PC
// 수정날짜  : 2021.01.04
// 수정자      : Lee Sang Cheol
// 설명         : GPIO Input/Output Define
//###############################################################################################################

#include "Define.h"

void Init_Dido()
{
    EALLOW;

    //DI
    GpioCtrlRegs.GPAGMUX1.bit.GPIO0 = 1;
    GpioCtrlRegs.GPAGMUX1.bit.GPIO1 = 1;
    GpioCtrlRegs.GPAGMUX1.bit.GPIO2 = 1;
    GpioCtrlRegs.GPAGMUX1.bit.GPIO3 = 1;
    GpioCtrlRegs.GPAGMUX1.bit.GPIO4 = 1;
    GpioCtrlRegs.GPDGMUX2.bit.GPIO126 = 1;
    GpioCtrlRegs.GPDGMUX2.bit.GPIO127 = 1;
    GpioCtrlRegs.GPEGMUX1.bit.GPIO128 = 1;
    GpioCtrlRegs.GPEGMUX1.bit.GPIO129 = 1;
    GpioCtrlRegs.GPEGMUX1.bit.GPIO130 = 1;
    GpioCtrlRegs.GPEGMUX1.bit.GPIO131 = 1;
    GpioCtrlRegs.GPEGMUX1.bit.GPIO132 = 1;

    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 0;
    GpioCtrlRegs.GPDMUX2.bit.GPIO126 = 0;
    GpioCtrlRegs.GPDMUX2.bit.GPIO127 = 0;
    GpioCtrlRegs.GPEMUX1.bit.GPIO128 = 0;
    GpioCtrlRegs.GPEMUX1.bit.GPIO129 = 0;
    GpioCtrlRegs.GPEMUX1.bit.GPIO130 = 0;
    GpioCtrlRegs.GPEMUX1.bit.GPIO131 = 0;
    GpioCtrlRegs.GPEMUX1.bit.GPIO132 = 0;

    GpioCtrlRegs.GPADIR.bit.GPIO0 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO1 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO2 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO3 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO4 = 0;

    GpioCtrlRegs.GPDDIR.bit.GPIO126 = 0;
    GpioCtrlRegs.GPDDIR.bit.GPIO127 = 0;
    GpioCtrlRegs.GPEDIR.bit.GPIO128 = 0;
    GpioCtrlRegs.GPEDIR.bit.GPIO129 = 0;
    GpioCtrlRegs.GPEDIR.bit.GPIO130 = 0;
    GpioCtrlRegs.GPEDIR.bit.GPIO131 = 0;
    GpioCtrlRegs.GPEDIR.bit.GPIO132 = 0;

    //Connector Connection check
    GpioCtrlRegs.GPAGMUX2.bit.GPIO25    = 0;
    GpioCtrlRegs.GPAMUX2.bit.GPIO25     = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO25      = 0;

    //DO
    GpioCtrlRegs.GPDGMUX1.bit.GPIO106 = 0;
    GpioCtrlRegs.GPDGMUX1.bit.GPIO107 = 0;
    GpioCtrlRegs.GPDGMUX1.bit.GPIO108 = 0;
    GpioCtrlRegs.GPDGMUX1.bit.GPIO109 = 0;
    GpioCtrlRegs.GPDGMUX1.bit.GPIO110 = 0;
    GpioCtrlRegs.GPDGMUX1.bit.GPIO111 = 0;

    GpioCtrlRegs.GPAGMUX1.bit.GPIO5 = 0;
    GpioCtrlRegs.GPAGMUX1.bit.GPIO7 = 0;
    GpioCtrlRegs.GPAGMUX1.bit.GPIO9 = 0;
    GpioCtrlRegs.GPAGMUX1.bit.GPIO11 = 0;
    GpioCtrlRegs.GPAGMUX1.bit.GPIO15 = 0;
    GpioCtrlRegs.GPAGMUX2.bit.GPIO24 = 0;


    GpioDataRegs.GPDDAT.bit.GPIO106 = 1;
    GpioDataRegs.GPDDAT.bit.GPIO107 = 1;
    GpioDataRegs.GPDDAT.bit.GPIO108 = 1;
    GpioDataRegs.GPDDAT.bit.GPIO109 = 1;
    GpioDataRegs.GPDDAT.bit.GPIO110 = 1;
    GpioDataRegs.GPDDAT.bit.GPIO111 = 1;
    GpioDataRegs.GPADAT.bit.GPIO5 = 1;
    GpioDataRegs.GPADAT.bit.GPIO7 = 1;
    GpioDataRegs.GPADAT.bit.GPIO9 = 1;
    GpioDataRegs.GPADAT.bit.GPIO11 = 1;
    GpioDataRegs.GPADAT.bit.GPIO15 = 1;
    GpioDataRegs.GPADAT.bit.GPIO24 = 1;

    GpioCtrlRegs.GPDDIR.bit.GPIO106 = 1;
    GpioCtrlRegs.GPDDIR.bit.GPIO107 = 1;
    GpioCtrlRegs.GPDDIR.bit.GPIO108 = 1;
    GpioCtrlRegs.GPDDIR.bit.GPIO109 = 1;
    GpioCtrlRegs.GPDDIR.bit.GPIO110 = 1;
    GpioCtrlRegs.GPDDIR.bit.GPIO111 = 1;
    GpioCtrlRegs.GPADIR.bit.GPIO5 = 1;
    GpioCtrlRegs.GPADIR.bit.GPIO7 = 1;
    GpioCtrlRegs.GPADIR.bit.GPIO9 = 1;
    GpioCtrlRegs.GPADIR.bit.GPIO11 = 1;
    GpioCtrlRegs.GPADIR.bit.GPIO15 = 1;
    GpioCtrlRegs.GPADIR.bit.GPIO24 = 1;

    // DO add
    GpioCtrlRegs.GPAGMUX2.bit.GPIO21 = 0;
    GpioCtrlRegs.GPAGMUX2.bit.GPIO22 = 0;
    GpioCtrlRegs.GPAGMUX2.bit.GPIO23 = 0;
    GpioCtrlRegs.GPBGMUX1.bit.GPIO33 = 0;

    GpioDataRegs.GPADAT.bit.GPIO21 = 1;
    GpioDataRegs.GPADAT.bit.GPIO22 = 1;
    GpioDataRegs.GPADAT.bit.GPIO23 = 1;
    GpioDataRegs.GPBDAT.bit.GPIO33 = 1;

    GpioCtrlRegs.GPADIR.bit.GPIO21 = 1;
    GpioCtrlRegs.GPADIR.bit.GPIO22 = 1;
    GpioCtrlRegs.GPADIR.bit.GPIO23 = 1;
    GpioCtrlRegs.GPBDIR.bit.GPIO33 = 1;

    // LED1, LED2
    GpioCtrlRegs.GPEGMUX1.bit.GPIO143 = 0;
    GpioCtrlRegs.GPEGMUX2.bit.GPIO144 = 0;

    GpioDataRegs.GPEDAT.bit.GPIO143 = 1;
    GpioDataRegs.GPEDAT.bit.GPIO144 = 1;

    GpioCtrlRegs.GPEDIR.bit.GPIO143 = 1;
    GpioCtrlRegs.GPEDIR.bit.GPIO144 = 1;

    EDIS;

    Do_off();
}

void Do_off()
{
    DO_1_OFF;
    DO_2_OFF;
    DO_3_OFF;
    DO_4_OFF;
    DO_5_OFF;
    DO_6_OFF;
    DO_7_OFF;
    DO_8_OFF;
    DO_9_OFF;
    DO_10_OFF;
    DO_11_OFF;
    DO_12_OFF;

    LED_1_OFF;
    LED_2_OFF;
}
