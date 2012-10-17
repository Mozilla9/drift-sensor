/******************************************************************************/
/* lis3dh.c                                                                   */
/******************************************************************************/


#include "data_types.h"
#include <math.h>
#include "Protothread\pt.h"
#include "Core\core.h"
#include "I2c\i2c0.h"
#include "Sett\settings.h"
#include "Lis3dh\lis3dh.h"

#include "Uart\v_printf.h"




static vector a;              // accelerometer readings
static i2c_data a_data;
static struct pt a_pt;

#define ACC_ADDRESS_W (ACC_ADDRESS & I2C_WRITE)
#define ACC_ADDRESS_R (ACC_ADDRESS | I2C_READ)




/*
 * Writes an accelerometer register
 *
 */
static void write_acc_reg(const uint8_t reg, const uint8_t value) {
    a_data.addr = ACC_ADDRESS_W;
    a_data.buff[0] = reg;
    a_data.buff[1] = value;
    a_data.len = 2;

    PT_INIT(&a_pt);
    while (1) {
        uint32_t status = start_i2c0(&a_pt, &a_data);

        if (status == PT_ENDED || status == PT_EXITED) {
            break;
        }
    }
}


/*
 * Init interrupt pins
 *
 */
static void init_interrupt_pins() {
    // Init int
    PINSEL4_bit.P2_11 = 0;     // int1
    PINSEL4_bit.P2_12 = 0;     // int2

    FIO2DIR_bit.P2_11 = 0;     // input
    FIO2DIR_bit.P2_12 = 0;     // input

    PINMODE4_bit.P2_11 = 3;    // PULL_RES
    PINMODE4_bit.P2_12 = 3;    //

}

/*
 * Init CS
 *
 */
static void init_cs() {
    // Init int
    PINSEL7_bit.P3_25 = 0;    // cs
    FIO3DIR_bit.P3_25 = 1;    // output
    PINMODE7_bit.P3_25 = 3;   // PULL_RES
}


/*
 * Init SA0
 *
 */
static void init_addr() {
    // Init int
    PINSEL7_bit.P3_26 = 0;    // sa0
    FIO3DIR_bit.P3_26 = 1;    // output
    PINMODE7_bit.P3_26 = 3;   // PULL_RES
}


/*
 * get int1
 *
 */
bool_t get_int1_lis3dh() {
    return (FIO2PIN_bit.P2_11) ? TRUE_T : FALSE_T;
}


/*
 * get int2
 *
 */
bool_t get_int2_lis3dh() {
    return (FIO2PIN_bit.P2_12) ? TRUE_T : FALSE_T;
}


/*
 * Set SA0
 *
 */
void set_sa0_lis3dh() {
    FIO3SET_bit.P3_26 = 1;
}


/*
 * Clear SA0
 *
 */
void clear_sa0_lis3dh() {
    FIO3CLR_bit.P3_26 = 1;
}


/*
 * Set cs
 *
 */
void set_cs_lis3dh() {
    FIO3SET_bit.P3_25 = 1;
}


/*
 * Clear cs
 *
 */
void clear_cs_lis3dh() {
    FIO3CLR_bit.P3_25 = 1;
}


/*
 * Init lis3dh
 *
 */
void init_lis3dh() {
    init_i2c0();
    init_interrupt_pins();
    init_addr();
    init_cs();

    set_sa0_lis3dh();  // slave addr 0x0011001
    set_cs_lis3dh();   // sel i2c
}


/*
 * Turns on the lis3dh's accelerometer and place
 * it in normal mode.
 *
 */
void enable_default_lis3dh() {
    uint8_t reg[2];
    __FMEM_DATA data;

    // temp cfg
    data.addr = ACC_TEMP_CFG_REG_ADDR;
    data.pBuff = reg;
    data.len = 2;
    read_app_settings(&data);
    
    if (reg[0] == LIS3DH_TEMP_CFG_REG) {
        write_acc_reg(LIS3DH_TEMP_CFG_REG, reg[1]);
    } else {
        write_acc_reg(LIS3DH_TEMP_CFG_REG, 0x00);
    }

    // reg1
    data.addr = ACC_CTRL_REG1_ADDR;
    read_app_settings(&data);

    if (reg[0] == LIS3DH_CTRL_REG1) {
        write_acc_reg(LIS3DH_CTRL_REG1, reg[1]);
    } else {
        write_acc_reg(LIS3DH_CTRL_REG1, 0x57);
    }

    // reg2
    data.addr = ACC_CTRL_REG2_ADDR;
    read_app_settings(&data);

    if (reg[0] == LIS3DH_CTRL_REG2) {
        write_acc_reg(LIS3DH_CTRL_REG2, reg[1]);
    }

    // reg3
    data.addr = ACC_CTRL_REG3_ADDR;
    read_app_settings(&data);

    if (reg[0] == LIS3DH_CTRL_REG3) {
        write_acc_reg(LIS3DH_CTRL_REG3, reg[1]);
    }

    // reg4
    data.addr = ACC_CTRL_REG4_ADDR;
    read_app_settings(&data);

    if (reg[0] == LIS3DH_CTRL_REG4) {
        write_acc_reg(LIS3DH_CTRL_REG4, reg[1]);
    } else {
        write_acc_reg(LIS3DH_CTRL_REG4, 0x10);
    }

    // reg5
    data.addr = ACC_CTRL_REG5_ADDR;
    read_app_settings(&data);

    if (reg[0] == LIS3DH_CTRL_REG5) {
        write_acc_reg(LIS3DH_CTRL_REG5, reg[1]);
    }

    // reg6
    data.addr = ACC_CTRL_REG6_ADDR;
    read_app_settings(&data);

    if (reg[0] == LIS3DH_CTRL_REG6) {
        write_acc_reg(LIS3DH_CTRL_REG6, reg[1]);
    }

    // preference
    data.addr = ACC_REFERENCE_ADDR;
    read_app_settings(&data);

    if (reg[0] == LIS3DH_REFERENCE) {
        write_acc_reg(LIS3DH_REFERENCE, reg[1]);
    }

    // fifo ctrl reg
    data.addr = ACC_FIFO_CTRL_REG_ADDR;
    read_app_settings(&data);

    if (reg[0] == LIS3DH_FIFO_CTRL_REG) {
        write_acc_reg(LIS3DH_FIFO_CTRL_REG, reg[1]);
    }

    // int1 cfg reg
    data.addr = ACC_INT1_CFG_ADDR;
    read_app_settings(&data);

    if (reg[0] == LIS3DH_INT1_CFG) {
        write_acc_reg(LIS3DH_INT1_CFG, reg[1]);
    }

    // int1 ths reg
    data.addr = ACC_INT1_THS_ADDR;
    read_app_settings(&data);

    if (reg[0] == LIS3DH_INT1_THS) {
        write_acc_reg(LIS3DH_INT1_THS, reg[1]);
    }

    // int1 duration reg
    data.addr = ACC_INT1_DURATION_ADDR;
    read_app_settings(&data);

    if (reg[0] == LIS3DH_INT1_DURATION) {
        write_acc_reg(LIS3DH_INT1_DURATION, reg[1]);
    }

    // click cfg reg
    data.addr = ACC_CLICK_CFG_ADDR;
    read_app_settings(&data);

    if (reg[0] == LIS3DH_CLICK_CFG) {
        write_acc_reg(LIS3DH_CLICK_CFG, reg[1]);
    }

    // click ths reg
    data.addr = ACC_CLICK_THS_ADDR;
    read_app_settings(&data);

    if (reg[0] == LIS3DH_CLICK_THS) {
        write_acc_reg(LIS3DH_CLICK_THS, reg[1]);
    }

    // time limit reg
    data.addr = ACC_TIME_LIMIT_ADDR;
    read_app_settings(&data);

    if (reg[0] == LIS3DH_TIME_LIMIT) {
        write_acc_reg(LIS3DH_TIME_LIMIT, reg[1]);
    }

    // time latency reg
    data.addr = ACC_TIME_LATENCY_ADDR;
    read_app_settings(&data);

    if (reg[0] == LIS3DH_TIME_LATENCY) {
        write_acc_reg(LIS3DH_TIME_LATENCY, reg[1]);
    }

    // time window reg
    data.addr = ACC_TIME_WINDOW_ADDR;
    read_app_settings(&data);

    if (reg[0] == LIS3DH_TIME_WINDOW) {
        write_acc_reg(LIS3DH_TIME_WINDOW, reg[1]);
    }
}


/*
 * Reads the 3 accelerometer channels and stores them in vector a
 *
 */
void read_acc_lis3dh() {
    // Write reg addr
    a_data.addr = ACC_ADDRESS_W;
    a_data.buff[0] = (LIS3DH_OUT_X_L | (1 << 7));
    a_data.len = 1;

    PT_INIT(&a_pt);
    while (1) {
        uint32_t status = start_i2c0(&a_pt, &a_data);

        if (status == PT_ENDED || status == PT_EXITED) {
            break;
        }
    }

    // Read
    a_data.addr = ACC_ADDRESS_R;
    a_data.len = 6;

    PT_INIT(&a_pt);
    while (1) {
        uint32_t status = start_i2c0(&a_pt, &a_data);

        if (status == PT_ENDED || status == PT_EXITED) {
            break;
        }
    }

    uint8_t xla = a_data.buff[0];
    uint8_t xha = a_data.buff[1];
    uint8_t yla = a_data.buff[2];
    uint8_t yha = a_data.buff[3];
    uint8_t zla = a_data.buff[4];
    uint8_t zha = a_data.buff[5];

    a.x = (sint16_t)((xha << 8) | (xla & 0xFF));
    a.y = (sint16_t)((yha << 8) | (yla & 0xFF));
    a.z = (sint16_t)((zha << 8) | (zla & 0xFF));

    a.status = a_data.isOk;
}


/*
 * Get acc vector
 *
 */
vector * get_acc_lis3dh() {
    return &a;
}
