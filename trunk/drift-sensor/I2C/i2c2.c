/******************************************************************************/
/* i2c2.c                                                                     */
/******************************************************************************/


#include "data_types.h"
#include "Core\core.h"
#include "Uart\v_printf.h"
#include "I2c\i2c2.h"
#include "Protothread\pt.h"

static i2c_data * pData;


/*
 * Init i2c2
 *
 */
void init_i2c2() {
    PCONP_bit.PCI2C2 = 1;

    I2C2CONCLR = I2C_EN;       // dis module

    // Init pins
    PINSEL0_bit.P0_10 = 2;     // SDA0
    PINSEL0_bit.P0_11 = 2;     // SCL0

    PINMODE0_bit.P0_10 = 2;    // PULL_RES
    PINMODE0_bit.P0_11 = 2;    //

    // Init module I2C2
    I2C2SCLH_bit.SCLH = 80;   // Freq 28,8 Mhz / (40 + 40)
    I2C2SCLL_bit.SCLL = 80;

    I2C2CONCLR = I2C_AA;       // master mode
    I2C2CONCLR = I2C_SI;
    I2C2CONCLR = I2C_STA;

    I2C2CONSET = I2C_EN;
}


/*
 * Start transaction i2c2
 *
 */
int start_i2c2(void * _pt, i2c_data * _pData) {
    // Thread
    PT_BEGIN((struct pt *)_pt);

    // Prepare
    pData = _pData;
    _pData->ind = 0UL;
    _pData->isEnd = FALSE_T;
    _pData->isOk = FALSE_T;

    I2C2CONSET = I2C_STA;

    PT_WAIT_UNTIL((struct pt *)_pt, _pData->isEnd);

    PT_END((struct pt *)_pt);
}




/*
 * Int handler
 *
 */
__irq __arm void __interrupt_handler_i2c2() {
    __eSTATUS_I2C status = (__eSTATUS_I2C)I2C2STAT;

    switch (status)
    {
        //----------------------------------------------------------------------
        case eI2C_08:
          //DEBUG_PRINTF("\r\n\r\n08_2");
          I2C2DAT = pData->addr;
          I2C2CONCLR = I2C_STA;

          if (pData->ind >= pData->len) {
              I2C2CONSET = I2C_STO;
              pData->isEnd = TRUE_T;
          };
        break;

        //----------------------------------------------------------------------
        case eI2C_10:
          //DEBUG_PRINTF("\r\n10_2");
          pData->ind = 0;
          I2C2DAT = pData->addr;
          I2C2CONCLR = I2C_STA;
        break;

        //----------------------------------------------------------------------
        case eI2C_18:
          //DEBUG_PRINTF("\r\n18_2");
          I2C2DAT = pData->buff[pData->ind++];
        break;

        //----------------------------------------------------------------------
        case eI2C_20:
          //DEBUG_PRINTF("\r\n20_2");
          I2C2CONSET = I2C_STO;
          pData->isEnd = TRUE_T;
         break;

        //----------------------------------------------------------------------
        case eI2C_28:
          //DEBUG_PRINTF("\r\n28_2");
          if (pData->ind == pData->len) {
              I2C2CONSET = I2C_STO;
              pData->isEnd = TRUE_T;
              pData->isOk = TRUE_T;
          }
          else {
              I2C2DAT = pData->buff[pData->ind++];
          };
        break;

        //----------------------------------------------------------------------
        case eI2C_30:
          //DEBUG_PRINTF("\r\n30_2");
          I2C2CONSET = I2C_STO;
          pData->isEnd = TRUE_T;
        break;

        //----------------------------------------------------------------------
        case eI2C_38:
          //DEBUG_PRINTF("\r\n38_2");
          I2C2CONSET = I2C_STA;
        break;

        //----------------------------------------------------------------------
        case eI2C_40:
          //DEBUG_PRINTF("\r\n40_2");
          I2C2CONSET = I2C_AA;
        break;

        //----------------------------------------------------------------------
        case eI2C_48:
          //DEBUG_PRINTF("\r\n48_2");
          I2C2CONSET = I2C_STO;
          pData->isEnd = TRUE_T;
        break;

        //----------------------------------------------------------------------
        case eI2C_50:
          //DEBUG_PRINTF("\r\n50_2");
          pData->buff[pData->ind++] = I2C2DAT;
          if (pData->ind == pData->len) {
              I2C2CONCLR = I2C_AA;
          }
          else {
              I2C2CONSET = I2C_AA;
          };
        break;

        //----------------------------------------------------------------------
        case eI2C_58:
          //DEBUG_PRINTF("\r\n58_2");
          I2C2CONSET = I2C_STO;
          pData->isEnd = TRUE_T;
          pData->isOk = TRUE_T;
        break;

        //----------------------------------------------------------------------
        default:
          I2C2CONSET = I2C_STO;
          pData->isEnd = TRUE_T;
        break;
    };

    I2C2CONCLR = I2C_SI;
    VICADDRESS = 0;
}
