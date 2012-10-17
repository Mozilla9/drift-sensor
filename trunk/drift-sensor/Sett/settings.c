/******************************************************************************/
/* settings.c                                                                 */
/******************************************************************************/

#include "data_types.h"
#include "Core\core.h"
#include "Uart\v_printf.h"
#include "At25df\at25df.h"
#include "Drift\drift.h"
#include "Lis3dh\lis3dh_def.h"
#include "Compass\compass.h"
#include "Tasks\tasks.h"
#include "Sett\settings.h"


static const __FMEM_SETT app_sett = {FMEM_PAGE_SIZE, FMEM_SECTOR_SIZE, GSETT_START_ADDR, GSETT_MEM_SIZE};
static const __FMEM_SETT app_swap_sett = {FMEM_PAGE_SIZE, FMEM_SECTOR_SIZE, GSETT_SWAP_START_ADDR, GSETT_MEM_SIZE};


/*
 * Write def sett
 *
 */
static void write_def_settings() {
    __FMEM_DATA data;
    uint32_t def_val;

    erase_memory_fmem(&app_sett);

    // def id
    def_val = 0;
    data.addr = DEV_ID_ADDR;
    data.pBuff = (uint8_t *) &def_val;
    data.len = 4;
    write_data_fmem(&app_sett, &data);

    // enabled tasks
    def_val = get_en_tasks();
    data.addr = DEV_EN_TASKS_ADDR;
    data.pBuff = (uint8_t *) &def_val;
    data.len = 4;
    write_data_fmem(&app_sett, &data);

    // enabled debug trace
    def_val = 0;
    data.addr = DEV_EN_TRACE_ADDR;
    data.pBuff = (uint8_t *) &def_val;
    data.len = 4;
    write_data_fmem(&app_sett, &data);

    // acc reg sett
    def_val = 0x00000000 | LIS3DH_TEMP_CFG_REG;
    data.addr = ACC_TEMP_CFG_REG_ADDR;
    data.pBuff = (uint8_t *) &def_val;
    data.len = 2;
    write_data_fmem(&app_sett, &data);

    def_val = 0x00005700 | LIS3DH_CTRL_REG1;
    data.addr = ACC_CTRL_REG1_ADDR;
    write_data_fmem(&app_sett, &data);

    def_val = 0x00001000 | LIS3DH_CTRL_REG4;
    data.addr = ACC_CTRL_REG4_ADDR;
    write_data_fmem(&app_sett, &data);

    // calb koeff
    data.addr = ACC_MATRIX_KOEFF_ADDR;
    data.pBuff = (uint8_t *) get_rotation_matrix();
    data.len = 36;
    write_data_fmem(&app_sett, &data);

    // axis indexes
    data.addr = ACC_AXIS_MAP_ADDR;
    data.pBuff = (uint8_t *) get_axis_data();
    data.len = 6;
    write_data_fmem(&app_sett, &data);

    // drift filtres
    data.addr = ACC_FILTR_DATA_ADDR;
    data.pBuff = (uint8_t *) get_x_acc_threshold();
    data.len = sizeof(drift_threshold);
    write_data_fmem(&app_sett, &data);

    data.addr += sizeof(drift_threshold);
    data.pBuff = (uint8_t *) get_y_acc_threshold();
    write_data_fmem(&app_sett, &data);

    data.addr += sizeof(drift_threshold);
    data.pBuff = (uint8_t *) get_z_acc_threshold();
    write_data_fmem(&app_sett, &data);

    data.addr += sizeof(drift_threshold);
    data.pBuff = (uint8_t *) get_pit_acc_threshold();
    write_data_fmem(&app_sett, &data);

    data.addr += sizeof(drift_threshold);
    data.pBuff = (uint8_t *) get_mod_acc_threshold();
    write_data_fmem(&app_sett, &data);

    // def sett flag
    def_val = DEF_SETT_FLAG;
    data.addr = DEF_SETT_FLAG_ADDR;
    data.pBuff = (uint8_t *) &def_val;
    data.len = 4;
    write_data_fmem(&app_sett, &data);
}


/*
 * Init sett, test for def sett
 *
 */
void init_app_settings() {
    __FMEM_DATA data;
    uint32_t def_flag;

    // read main flag
    def_flag = 0;
    data.addr = DEF_SETT_FLAG_ADDR;
    data.pBuff = (uint8_t *) (&def_flag);
    data.len = sizeof(def_flag);
    read_data_fmem(&app_sett, &data);

    if (def_flag != DEF_SETT_FLAG) {
        // read main flag from swap
        def_flag = 0;
        data.addr = DEF_SETT_FLAG_ADDR;
        data.pBuff = (uint8_t *) (&def_flag);
        data.len = sizeof(def_flag);
        read_data_fmem(&app_swap_sett, &data);

        if (def_flag == DEF_SETT_FLAG) {
            uint8_t buff[64];

            data.addr = 0;
            data.pBuff = buff;
            data.len = sizeof(buff);
            erase_memory_fmem(&app_sett);

            while (data.addr < GSETT_MEM_SIZE) {
                read_data_fmem(&app_swap_sett, &data);
                write_data_fmem(&app_sett, &data);

                data.addr += sizeof(buff);
            }
            erase_memory_fmem(&app_swap_sett);
        } else {
            write_def_settings();
            return;
        }
    }

    // device id
    data.addr = DEV_ID_ADDR;
    data.pBuff = (uint8_t *) &def_flag;
    data.len = sizeof(def_flag);
    read_app_settings(&data);
    set_device_id(def_flag);

    // trace func
    data.addr = DEV_EN_TRACE_ADDR;
    read_app_settings(&data);
    set_trace(def_flag ? &serprintf: 0);
}


/*
 * Write sett (max 256 bytes)
 *
 */
void write_app_settings(const __FMEM_DATA * const pWriteData) {
    // validating
    if (get_space_to_end_fmem(&app_sett, pWriteData->addr) >= pWriteData->len && pWriteData->len < 257) {
        __FMEM_DATA data;
        uint8_t buff[64];
        uint32_t def_flag;
        uint32_t wr_data_shift;

        // erase swap
        erase_memory_fmem(&app_swap_sett);

        // copy main to swap
        data.addr = 0;
        data.pBuff = buff;
        data.len = sizeof(buff);

        while (data.addr < GSETT_MEM_SIZE) {
            read_data_fmem(&app_sett, &data);
            write_data_fmem(&app_swap_sett, &data);

            data.addr += sizeof(buff);
        }

        // test flag
        data.addr = DEF_SETT_FLAG_ADDR;
        data.pBuff = (uint8_t *) &def_flag;
        data.len = 4;
        read_data_fmem(&app_swap_sett, &data);

        if (def_flag != DEF_SETT_FLAG) {
            return;
        }

        // erase main
        erase_memory_fmem(&app_sett);

        // copy from swap to main with new sett
        wr_data_shift = 0;
        data.addr = 0;
        data.pBuff = buff;
        data.len = sizeof(buff);

        while (data.addr < GSETT_MEM_SIZE) {
            read_data_fmem(&app_swap_sett, &data);

            if ((pWriteData->addr + wr_data_shift) >= data.addr
                && (pWriteData->addr + wr_data_shift) <= (data.addr + sizeof(buff))) {

                uint32_t index = (pWriteData->addr + wr_data_shift) - data.addr;
                while (wr_data_shift < pWriteData->len) {
                    if ((pWriteData->addr + wr_data_shift) < (data.addr + sizeof(buff))) {
                        data.pBuff[index++] = pWriteData->pBuff[wr_data_shift++];
                    } else {
                        break;
                    }
                }
            }
            write_data_fmem(&app_sett, &data);

            data.addr += sizeof(buff);
        }

        // test flag
        data.addr = DEF_SETT_FLAG_ADDR;
        data.pBuff = (uint8_t *) &def_flag;
        data.len = 4;
        read_data_fmem(&app_sett, &data);

        if (def_flag != DEF_SETT_FLAG) {
            reset_device();
        }

        // erase swap
        erase_memory_fmem(&app_swap_sett);
    }
}


/*
 * Read sett
 *
 */
uint32_t read_app_settings(const __FMEM_DATA * const pReadData) {
    return read_data_fmem(&app_sett, pReadData);
}
