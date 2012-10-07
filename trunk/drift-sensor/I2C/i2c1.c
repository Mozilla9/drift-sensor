/******************************************************************************/
/* i2c1.c                                                                     */
/******************************************************************************/


#include "data_types.h"
#include "Core\core.h"
#include "Uart\v_printf.h"
#include "I2c\i2c1.h"
#include "Protothread\pt.h"

static i2c_data * pData;


/*
 * Init i2c1
 *
 */
void init_i2c1() {
    PCONP_bit.PCI2C1 = 1;

    I2C1CONCLR = I2C_EN;       // dis module

    // Init pins
    PINSEL1_bit.P0_19 = 3;     // SDA0
    PINSEL1_bit.P0_20 = 3;     // SCL0

    PINMODE1_bit.P0_19 = 2;    // PULL_RES
    PINMODE1_bit.P0_20 = 2;    //

    // Init module I2C1
    I2C1SCLH_bit.SCLH = 80;   // Freq 28,8 Mhz / (40 + 40)
    I2C1SCLL_bit.SCLL = 80;

    I2C1CONCLR = I2C_AA;       // master mode
    I2C1CONCLR = I2C_SI;
    I2C1CONCLR = I2C_STA;

    I2C1CONSET = I2C_EN;
}


/*
 * Start transaction i2c1
 *
 */
int start_i2c1(void * _pt, i2c_data * _pData) {
    // Thread
    PT_BEGIN((struct pt *)_pt);

    // Prepare
    pData = _pData;
    _pData->ind = 0UL;
    _pData->isEnd = FALSE_T;
    _pData->isOk = FALSE_T;

    I2C1CONSET = I2C_STA;

    PT_WAIT_UNTIL((struct pt *)_pt, _pData->isEnd);

    PT_END((struct pt *)_pt);
}


/*
 * Int handler
 *
 */
__irq __arm void __interrupt_handler_i2c1() {
    __eSTATUS_I2C status = (__eSTATUS_I2C)I2C1STAT;

    switch (status)
    {
        //----------------------------------------------------------------------
        case eI2C_08:   // Start tx
          //DEBUG_PRINTF("\r\n\r\n08_1");
          I2C1DAT = pData->addr;
          I2C1CONCLR = I2C_STA;

          if (pData->ind >= pData->len) {
              I2C1CONSET = I2C_STO;
              pData->isEnd = TRUE_T;
          };
        break;

        //----------------------------------------------------------------------
        case eI2C_10:   // Repeat start tx
          //DEBUG_PRINTF("\r\n10_1");
          pData->ind = 0;
          I2C1DAT = pData->addr;
          I2C1CONCLR = I2C_STA;
        break;

        //----------------------------------------------------------------------
        case eI2C_18:   // ADR+W tx
          //DEBUG_PRINTF("\r\n18_1");
          I2C1DAT = pData->buff[pData->ind++];
        break;

        //----------------------------------------------------------------------
        case eI2C_20:   // ADR+W tx, but not rx ACK
          //DEBUG_PRINTF("\r\n20_1");
          I2C1CONSET = I2C_STO;
          pData->isEnd = TRUE_T;
         break;

        //----------------------------------------------------------------------
        case eI2C_28:   // DATA TX
          //DEBUG_PRINTF("\r\n28_1");
          if (pData->ind == pData->len) {
              I2C1CONSET = I2C_STO;
              pData->isEnd = TRUE_T;
              pData->isOk = TRUE_T;
          }
          else {
              I2C1DAT = pData->buff[pData->ind++];
          };
        break;

        //----------------------------------------------------------------------
        case eI2C_30:   // DATA TX, but not rx ACK
          //DEBUG_PRINTF("\r\n30_1");
          I2C1CONSET = I2C_STO;
          pData->isEnd = TRUE_T;
        break;

        //----------------------------------------------------------------------
        case eI2C_38:   // Bus busy
          //DEBUG_PRINTF("\r\n38_1");
          I2C1CONSET = I2C_STA;
        break;

        //----------------------------------------------------------------------
        case eI2C_40:   // ADR+R tx
          //DEBUG_PRINTF("\r\n40_1");
          I2C1CONSET = I2C_AA;
        break;

        //----------------------------------------------------------------------
        case eI2C_48:   // ADR+R tx, but not rx ACK
          //DEBUG_PRINTF("\r\n48_1");
          I2C1CONSET = I2C_STO;
          pData->isEnd = TRUE_T;
        break;

        //----------------------------------------------------------------------
        case eI2C_50:   // DATA RX, ACK rx
          //DEBUG_PRINTF("\r\n50_1");
          pData->buff[pData->ind++] = I2C1DAT;
          if (pData->ind == pData->len) {
              I2C1CONCLR = I2C_AA;
          }
          else {
              I2C1CONSET = I2C_AA;
          };
        break;

        //----------------------------------------------------------------------
        case eI2C_58:   // DATA RX, NO ACK rx
          //DEBUG_PRINTF("\r\n58_1");
          I2C1CONSET = I2C_STO;
          pData->isEnd = TRUE_T;
          pData->isOk = TRUE_T;
        break;

        //----------------------------------------------------------------------
        default:
          I2C1CONSET = I2C_STO;
          pData->isEnd = TRUE_T;
        break;
    };

    I2C1CONCLR = I2C_SI;
    VICADDRESS = 0;
}


