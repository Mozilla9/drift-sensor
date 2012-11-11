/******************************************************************************/
/*  Flash memory map for Compass                                              */
/*                                                                            */
/******************************************************************************/

#ifndef __FMEM_MAP_H
#define __FMEM_MAP_H

#define FMEMORY_VOLUME             (0x200000)


/******************************* Partition flash ******************************/
#define GSETT_START_ADDR           (0)
#define GSETT_SWAP_START_ADDR      (4096)
#define GSETT_MEM_SIZE             (4096)

#define FW_START_ADDR              (GSETT_SWAP_START_ADDR + GSETT_MEM_SIZE)
#define FW_MEM_SIZE                (31 * 4096)



/******************************** Generic settings ****************************/
#define DEV_ID_ADDR                (0)                            // device id (4 b)
#define DEV_EN_TASKS_ADDR          (DEV_ID_ADDR + 4)              // enabled tasks (4 b)
#define DEV_EN_TRACE_ADDR          (DEV_EN_TASKS_ADDR + 4)        // trace enable (4 b)

#define DEV_RESERVED_ADDR_ST       (DEV_EN_TRACE_ADDR + 4)        // 
#define DEV_RESERVED_ADDR_ED       (255)                          // 

#define ACC_TEMP_CFG_REG_ADDR      (DEV_RESERVED_ADDR_ED + 1)     // acc lis3dh sett (40 b)
#define ACC_CTRL_REG1_ADDR         (ACC_TEMP_CFG_REG_ADDR + 2)    //
#define ACC_CTRL_REG2_ADDR         (ACC_CTRL_REG1_ADDR + 2)       //
#define ACC_CTRL_REG3_ADDR         (ACC_CTRL_REG2_ADDR + 2)       //
#define ACC_CTRL_REG4_ADDR         (ACC_CTRL_REG3_ADDR + 2)       //
#define ACC_CTRL_REG5_ADDR         (ACC_CTRL_REG4_ADDR + 2)       //
#define ACC_CTRL_REG6_ADDR         (ACC_CTRL_REG5_ADDR + 2)       //
#define ACC_REFERENCE_ADDR         (ACC_CTRL_REG6_ADDR + 2)       //
#define ACC_FIFO_CTRL_REG_ADDR     (ACC_REFERENCE_ADDR + 2)       //
#define ACC_INT1_CFG_ADDR          (ACC_FIFO_CTRL_REG_ADDR + 2)   //
#define ACC_INT1_THS_ADDR          (ACC_INT1_CFG_ADDR + 2)        //
#define ACC_INT1_DURATION_ADDR     (ACC_INT1_THS_ADDR + 2)        //

#define ACC_CLICK_CFG_ADDR         (ACC_INT1_DURATION_ADDR + 2)   //
#define ACC_CLICK_THS_ADDR         (ACC_CLICK_CFG_ADDR + 2)       //
#define ACC_TIME_LIMIT_ADDR        (ACC_CLICK_THS_ADDR + 2)       //
#define ACC_TIME_LATENCY_ADDR      (ACC_TIME_LIMIT_ADDR + 2)      //
#define ACC_TIME_WINDOW_ADDR       (ACC_TIME_LATENCY_ADDR + 2)    //

#define ACC_MATRIX_KOEFF_ADDR      (ACC_TEMP_CFG_REG_ADDR + 40)   // matrix koeff 9 float (36 b)
#define ACC_AXIS_MAP_ADDR          (ACC_MATRIX_KOEFF_ADDR + 36)   // axis mapping indx (6 b)
#define ACC_FILTR_DATA_ADDR        (ACC_AXIS_MAP_ADDR + 6)        // filter sett 10 x 12 (120 b)


#define DEF_SETT_FLAG_ADDR         (GSETT_MEM_SIZE - 4)           // default sett flag (4 b  0xaa55aa55)


/*********************************** Firmware flash ***************************/
#define FW_STAFF_MEM_ADDR          (FW_MEM_SIZE - 256)









#endif