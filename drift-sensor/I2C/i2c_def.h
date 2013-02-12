/*
 * $Id$
 */

#ifndef __I2C_DEF_H
#define __I2C_DEF_H


/***************************** Constants I2C **********************************/
#define   I2C_BUFF_SIZE   (32)
#define   I2C_WRITE       (0xFE)
#define   I2C_READ        (0x01)

#define   I2C_AA          (0x04)
#define   I2C_SI          (0x08)
#define   I2C_STO         (0x10)
#define   I2C_STA         (0x20)
#define   I2C_EN          (0x40)


/************************** Prototypes struct for I2C *************************/
typedef enum
{
    // Master Tx
    eI2C_08 = 0x08,            // Start tx
    eI2C_10 = 0x10,            // Repeat start tx
    eI2C_18 = 0x18,            // ADR+W tx
    eI2C_20 = 0x20,            // ADR+W tx, but not rx ACK
    eI2C_28 = 0x28,            // DATA TX
    eI2C_30 = 0x30,            // DATA TX, but not rx ACK
    eI2C_38 = 0x38,            // Bus busy

    // Master Rx
    eI2C_40 = 0x40,            // ADR+R tx
    eI2C_48 = 0x48,            // ADR+R tx, but not rx ACK
    eI2C_50 = 0x50,            // DATA RX, ACK rx
    eI2C_58 = 0x58,            // DATA RX, NO ACK rx
    eI2C_F8 = 0xF8             //
} __eSTATUS_I2C;               // Type Status I2C


typedef struct i2c_data
{
    uint8_t buff[I2C_BUFF_SIZE];
    uint8_t addr;
    uint32_t len;
    uint32_t ind;
    volatile bool_t isEnd;
    volatile bool_t isOk;
} i2c_data;

#endif