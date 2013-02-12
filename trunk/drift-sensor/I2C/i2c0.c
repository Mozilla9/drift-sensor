/*
 * $Id$
 */


#include "data_types.h"
#include "Core\core.h"
#include "Uart\v_printf.h"
#include "I2c\i2c0.h"
#include "Protothread\pt.h"

static i2c_data * pData;


/*
 * Init i2c0
 *
 */
void init_i2c0() {
    PCONP_bit.PCI2C0 = 1;

    I2C0CONCLR = I2C_EN;       // dis module

    // Init pins
    PINSEL1_bit.P0_27 = 1;     // SDA0
    PINSEL1_bit.P0_28 = 1;     // SCL0

    PINMODE1_bit.P0_27 = 2;    // PULL_RES
    PINMODE1_bit.P0_28 = 2;    //

    // Init module I2C0
    I2C0SCLH_bit.SCLH = 40;   // Freq 28,8 Mhz / (40 + 40)
    I2C0SCLL_bit.SCLL = 40;

    I2C0CONCLR = I2C_AA;       // master mode
    I2C0CONCLR = I2C_SI;
    I2C0CONCLR = I2C_STA;

    I2C0CONSET = I2C_EN;
}


/*
 * Start transaction i2c0
 *
 */
int start_i2c0(void * _pt, i2c_data * _pData) {
    // Thread
    PT_BEGIN((struct pt *)_pt);

    // Prepare
    pData = _pData;
    _pData->ind = 0UL;
    _pData->isEnd = FALSE_T;
    _pData->isOk = FALSE_T;

    I2C0CONSET = I2C_STA;

    PT_WAIT_UNTIL((struct pt *)_pt, _pData->isEnd);

    PT_END((struct pt *)_pt);
}


/*
 * Int handler
 *
 */
__irq __arm void __interrupt_handler_i2c0() {
    __eSTATUS_I2C status = (__eSTATUS_I2C)I2C0STAT;

    switch (status)
    {
        //----------------------------------------------------------------------
        case eI2C_08:   // Start tx
          //DEBUG_PRINTF("\r\n\r\n08_0");
          I2C0DAT = pData->addr;
          I2C0CONCLR = I2C_STA;

          if (pData->ind >= pData->len) {
              I2C0CONSET = I2C_STO;
              pData->isEnd = TRUE_T;
          };
        break;

        //----------------------------------------------------------------------
        case eI2C_10:   // Repeat start tx
          //DEBUG_PRINTF("\r\n10_0");
          pData->ind = 0;
          I2C0DAT = pData->addr;
          I2C0CONCLR = I2C_STA;
        break;

        //----------------------------------------------------------------------
        case eI2C_18:   // ADR+W tx
          //DEBUG_PRINTF("\r\n18_0");
          I2C0DAT = pData->buff[pData->ind++];
        break;

        //----------------------------------------------------------------------
        case eI2C_20:   // ADR+W tx, but not rx ACK
          //DEBUG_PRINTF("\r\n20_0");
          I2C0CONSET = I2C_STO;
          pData->isEnd = TRUE_T;
         break;

        //----------------------------------------------------------------------
        case eI2C_28:   // DATA TX
          //DEBUG_PRINTF("\r\n28_0");
          if (pData->ind == pData->len) {
              I2C0CONSET = I2C_STO;
              pData->isEnd = TRUE_T;
              pData->isOk = TRUE_T;
          }
          else {
              I2C0DAT = pData->buff[pData->ind++];
          };
        break;

        //----------------------------------------------------------------------
        case eI2C_30:   // DATA TX, but not rx ACK
          //DEBUG_PRINTF("\r\n30_0");
          I2C0CONSET = I2C_STO;
          pData->isEnd = TRUE_T;
        break;

        //----------------------------------------------------------------------
        case eI2C_38:   // Bus busy
          //DEBUG_PRINTF("\r\n38_0");
          I2C0CONSET = I2C_STA;
        break;

        //----------------------------------------------------------------------
        case eI2C_40:   // ADR+R tx
          //DEBUG_PRINTF("\r\n40_0");
          I2C0CONSET = I2C_AA;
        break;

        //----------------------------------------------------------------------
        case eI2C_48:   // ADR+R tx, but not rx ACK
          //DEBUG_PRINTF("\r\n48_0");
          I2C0CONSET = I2C_STO;
          pData->isEnd = TRUE_T;
        break;

        //----------------------------------------------------------------------
        case eI2C_50:   // DATA RX, ACK rx
          //DEBUG_PRINTF("\r\n50_0");
          pData->buff[pData->ind++] = I2C0DAT;
          if (pData->ind == pData->len) {
              I2C0CONCLR = I2C_AA;
          }
          else {
              I2C0CONSET = I2C_AA;
          };
        break;

        //----------------------------------------------------------------------
        case eI2C_58:   // DATA RX, NO ACK rx
          //DEBUG_PRINTF("\r\n58_0");
          I2C0CONSET = I2C_STO;
          pData->isEnd = TRUE_T;
          pData->isOk = TRUE_T;
        break;

        //----------------------------------------------------------------------
        default:
          I2C0CONSET = I2C_STO;
          pData->isEnd = TRUE_T;
        break;
    };

    I2C0CONCLR = I2C_SI;
    VICADDRESS = 0;
}


