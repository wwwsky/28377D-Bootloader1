#include "F28x_Project.h"
#include "string.h"

#define BUFF_SIZE 1024 * 2

// CPU Main Clock
#define CPUCLK          200000000L

// Peripheral Low Speed Clock for SCI-C**
#define SCIA_LSPCLK     (CPUCLK / 4)

// SCI-A Baudrate
//#define SCIA_BAUDRATE 9600L
#define   SCIA_BAUDRATE   115200L
//#define   SCIA_BAUDRATE   230400L
//#define   SCIA_BAUDRATE   460800L
//#define   SCIA_BAUDRATE   57600L

#define SCIA_BRR_VAL (SCIA_LSPCLK / (8 * SCIA_BAUDRATE) - 1)

interrupt void scia_rx_isr(void);

void Init_Sci(void)
{
    EALLOW;
    GPIO_SetupPinMux(136, GPIO_MUX_CPU1, 6);              //SCI_A_RX (MUX6)**
    GPIO_SetupPinOptions(136, GPIO_INPUT, GPIO_PUSHPULL); //SCI_A_RX (MUX6)
    GPIO_SetupPinMux(135, GPIO_MUX_CPU1, 6);              //SCI_A_TX (MUX6)
    GPIO_SetupPinOptions(135, GPIO_OUTPUT, GPIO_ASYNC);   //SCI_A_TX (MUX6)
    EDIS;

    SciaRegs.SCICCR.bit.STOPBITS = 0;  //1 stop bit
    SciaRegs.SCICCR.bit.PARITY = 1;    //Even parity
    SciaRegs.SCICCR.bit.LOOPBKENA = 0; //No loopback
    SciaRegs.SCICCR.bit.SCICHAR = 7;   //8 char bits
    SciaRegs.SCICCR.bit.PARITYENA = 1; //parity enable

    SciaRegs.SCICTL1.bit.RXERRINTENA = 1;
    SciaRegs.SCICTL1.bit.RXENA = 1;
    SciaRegs.SCICTL1.bit.TXENA = 1;

    SciaRegs.SCICTL2.bit.TXINTENA = 0;
    SciaRegs.SCICTL2.bit.RXBKINTENA = 0;
    SciaRegs.SCIHBAUD.bit.BAUD = SCIA_BRR_VAL >> 8;
    SciaRegs.SCILBAUD.bit.BAUD = SCIA_BRR_VAL & 0xff;

    // SCI의 송신 FIFO 설정
    SciaRegs.SCIFFTX.bit.SCIFFENA = 1;    // SCI FIFO 사용 설정 Enable
    SciaRegs.SCIFFTX.bit.TXFFINTCLR = 1;  // SCI 송신 FIFO 인터럽트 플래그 클리어
    SciaRegs.SCIFFTX.bit.TXFIFORESET = 1; // SCI 송신 FIFO RE-enable
    SciaRegs.SCIFFTX.bit.TXFFIENA = 0;    // SCI 송신 FIFO 인터럽트 Enable
    SciaRegs.SCIFFTX.bit.TXFFIL = 0;      // SCI 수신 FIFO 인터럽트 레벨 설정

    // SCI의 수신 FIFO 설정
    SciaRegs.SCIFFRX.bit.RXFFINTCLR = 1;  // SCI 수신 FIFO 인터럽트 플래그 클리어
    SciaRegs.SCIFFRX.bit.RXFIFORESET = 1; // SCI 수신 FIFO RE-enable
    SciaRegs.SCIFFRX.bit.RXFFIENA = 1;    // SCI 수신 FIFO 인터럽트 Enable
    SciaRegs.SCIFFRX.bit.RXFFIL = 1;      // SCI 수신 FIFO 인터럽트 레벨 설정

    SciaRegs.SCICTL1.all = 0x0023; // Relinquish SCI from Reset

    EALLOW;
    PieVectTable.SCIA_RX_INT = &scia_rx_isr;
    EDIS;

    PieCtrlRegs.PIECTRL.bit.ENPIE = 1; // Enable the PIE block
    PieCtrlRegs.PIEIER9.bit.INTx1 = 1; // PIE Group 9, INT1
                                       //    PieCtrlRegs.PIEIER9.bit.INTx1 = 1;   // PIE Group 9, INT1
    IER = M_INT9;                      // Enable CPU INT;
}

char rx_buff[BUFF_SIZE];
char tx_buff[BUFF_SIZE];
Uint16 rx_r = 0, rx_w = 0, rx_full = 0;
Uint16 tx_r = 0, tx_w = 0, tx_full = 0;


interrupt void scia_rx_isr(void)
{
    register Uint16 new_w = rx_w + 1;

    if(new_w >= BUFF_SIZE)
        new_w = 0;

    if(new_w == rx_r)
    {
        char dummy = SciaRegs.SCIRXBUF.bit.SAR;
        rx_full++;
    }
    else
    {
        rx_buff[rx_w] = SciaRegs.SCIRXBUF.bit.SAR;
        rx_w = new_w;
    }

    SciaRegs.SCIFFRX.bit.RXFFOVRCLR = 1; // Clear Overflow flag
    SciaRegs.SCIFFRX.bit.RXFFINTCLR = 1; // Clear Interrupt flag
    PieCtrlRegs.PIEACK.all |= M_INT9;    // Issue PIE ack
}


int16 Get_Buff()
{
    int16 ret = 0;

    if(rx_r == rx_w)
        ret = -1;

    else
    {
        ret = rx_buff[rx_r++];

        if(rx_r >= BUFF_SIZE)
            rx_r = 0;
    }

    return ret;
}

