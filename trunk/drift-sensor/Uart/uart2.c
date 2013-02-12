/*
 * $Id$
 */


#include "data_types.h"
#include "Core\core.h"
#include "Uart\uart2.h"



/*
 * Init uart2
 *
 */
void init_uart2(const uint32_t _baudrate, const uint8_t _key) {
    U2IER = 0UL;                  // int dis

    PCONP_bit.PCUART2 = 1;

    PINSEL4_bit.P2_8 = 2;         // TXD
    PINSEL4_bit.P2_9 = 2;         // RXD

    FIO2DIR_bit.P2_8 = 1;         // TXD
    FIO2DIR_bit.P2_9 = 0;         // RXD

    PINMODE4_bit.P2_8 = 2;        // TXD PULL_RES - dis
    PINMODE4_bit.P2_9 = 2;        // RXD

    U2LCR = 0x80 | _key;          // 8 bits, no Parity, 1 Stop bit
    U2FDR_bit.DIVADDVAL = 0;
    U2FDR_bit.MULVAL = 0;

    uint32_t Br;

    if(_baudrate == 115200) {
        // ! for FPCLK=28800000
        U2DLM = 0;
        U2DLL = 13;
        U2FDR_bit.DIVADDVAL = 1;
        U2FDR_bit.MULVAL = 5;
    }
    else {
        Br = (FPCLK / 16) / _baudrate;
        U2DLM = Br / 256;
        U2DLL = Br % 256;
    }

    U2LCR = _key;                  // DLAB = 0
    U2FCR = 0x41;                  // Enable and reset TX and RX FIFO, size 4 bytes
}



/*
 * Uart2 interrupt handler
 *
 */
__irq __arm void __interrupt_handler_uart2() {
    uint8_t IIR_VALUE = U2IIR;
    const uint8_t LSR_VALUE = U2LSR;

    IIR_VALUE >>= 1;
    IIR_VALUE &= 0x07;

    switch(IIR_VALUE)
    {
        //----------------------------------------------------------------------
        case IIR_RLS:
          if (LSR_VALUE & LSR_RXFE) {
              U2SCR = U2RBR;            // bad data flush
          }
          break;

        //----------------------------------------------------------------------
        case IIR_RDA:
          for (uint32_t i = 0; i < UART2_FIFO_TRLEVEL; i++) {
              //add_byte(U2RBR);
          }
          break;

        //----------------------------------------------------------------------
        case IIR_CTI:
          do {
              //add_byte(U2RBR);
          } while (U2LSR & LSR_RDR);
          break;

        //----------------------------------------------------------------------
        case IIR_THRE:
          break;

        //----------------------------------------------------------------------
        default:
          break;
    }

    VICADDRESS = 0;
}