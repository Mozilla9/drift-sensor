/******************************************************************************/
/* uart0.c                                                                    */
/******************************************************************************/


#include "data_types.h"
#include "Core\core.h"
#include "Ringbuff\ring_buffer.h"
#include "Uart\uart0.h"


extern __ring_buff * const pointerRingBuff;


/*
 * Init uart0
 *
 */
void init_uart0(const uint32_t _baudrate, const uint8_t _key) {
    U0IER = 0UL;                  // int dis

    PCONP_bit.PCUART0 = 1;

    PINSEL0_bit.P0_2 = 1;         // TXD
    PINSEL0_bit.P0_3 = 1;         // RXD

    FIO0DIR_bit.P0_2 = 1;         // TXD
    FIO0DIR_bit.P0_3 = 0;         // RXD

    PINMODE0_bit.P0_2 = 2;        // TXD PULL_RES - dis
    PINMODE0_bit.P0_3 = 2;        // RXD

    U0LCR = 0x80 | _key;          // 8 bits, no Parity, 1 Stop bit
    U0FDR_bit.DIVADDVAL = 0;
    U0FDR_bit.MULVAL = 0;

    uint32_t Br;

    if(_baudrate == 115200) {
        // ! for FPCLK=28800000
        U0DLM = 0;
        U0DLL = 13;
        U0FDR_bit.DIVADDVAL = 1;
        U0FDR_bit.MULVAL = 5;
    }
    else {
        Br = (FPCLK / 16) / _baudrate;
        U0DLM = Br / 256;
        U0DLL = Br % 256;
    }

    U0LCR = _key;                  // DLAB = 0
    U0FCR = 0x41;                  // Enable and reset TX and RX FIFO, size 4 bytes
}


/*
 * Uart0 interrupt handler
 *
 */
__irq __arm void __interrupt_handler_uart0() {
    uint8_t IIR_VALUE = U0IIR;
    const uint8_t LSR_VALUE = U0LSR;

    IIR_VALUE >>= 1;
    IIR_VALUE &= 0x07;

    switch(IIR_VALUE)
    {
        //----------------------------------------------------------------------
        case IIR_RLS:
          if (LSR_VALUE & LSR_RXFE) {
              U0SCR = U0RBR;            // Bad data flush
          }
          break;

        //----------------------------------------------------------------------
        case IIR_RDA:
          for (uint32_t i = 0; i < UART0_FIFO_TRLEVEL; i++) {
              put_in_ring_buff(pointerRingBuff, U0RBR);
          }
          break;

        //----------------------------------------------------------------------
        case IIR_CTI:
          do {
              put_in_ring_buff(pointerRingBuff, U0RBR);
          } while (U0LSR & LSR_RDR);
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
