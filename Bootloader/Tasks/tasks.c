/*
 * $Id$
 */


#include "data_types.h"
#include "Core\core.h"
#include "Uart\v_printf.h"
#include "Protothread\pt.h"
#include "Led\led.h"
#include "Updater\updater.h"
#include "Bootloader\bootloader.h"
#include "Tasks\tasks.h"





#define UPDATE_TASK           1



static uint32_t flag_run_enabled = 0;
static uint32_t enabled_tasks = 0x000000;
static int16_t upd_state;

static struct pt first_pt;
static struct pt upd_pt;
static struct pt led_pt;



/*
 * First task
 *
 */
static int first_task(struct pt * pt) {
    static uint32_t ticks = 0;
    PT_BEGIN(pt);

    while(1) {
        PT_WAIT_UNTIL(pt, !(get_sys_tick() % 4) && ticks != get_sys_tick());
        ticks = get_sys_tick();
        flag_run_enabled = 1;
    }

    PT_END(pt);
}


/*
 * Updating task
 *
 */
static int update_task(struct pt * pt) {
    PT_BEGIN(pt);

    while(1) {
        PT_WAIT_UNTIL(pt, flag_run_enabled);
        {
            switch (upd_state) {
                //--------------------------------------------------------------
                case 0:      // init + erase mcu flash
                    DEBUG_PRINTF("BL:Start update task\n\r");
                    reset_update_state();
                    if (erase_main_fw_area_upd()) {
                        upd_state++;
                    } else {
                        DEBUG_PRINTF("BL:updt err - 1\n\r");
                        upd_state = -1;
                    }
                    break;

                //--------------------------------------------------------------
                case 1:      // read data block, decode
                    if (read_block_from_flash_upd()) {
                        // decode_block_upd();
                        upd_state++;
                    } else {
                        DEBUG_PRINTF("BL:updt err - 2\n\r");
                        upd_state = -1;
                    }
                    break;

                //--------------------------------------------------------------
                case 2:      // write block in mcu flash
                    if (write_block_in_main_fw_upd()) {
                        if (is_update_end()) {
                            upd_state++;
                        } else {
                            upd_state--;
                        }
                    } else {
                        DEBUG_PRINTF("BL:updt err - 3\n\r");
                        upd_state = -1;
                    }
                    break;

                //--------------------------------------------------------------
                case 3:
                    erase_flg_need_update();
                    if (finalize_update()) {
                        DEBUG_PRINTF("BL:Update successfully\r\n");
                        reset_device();
                    }
                    DEBUG_PRINTF("BL:updt err - 4\n\r");
                    disable_update_task();
                    break;

                //--------------------------------------------------------------
                default:
                    erase_flg_need_update();
                    disable_update_task();
                    break;
            }
        }
        PT_WAIT_UNTIL(pt, !flag_run_enabled);
    }

    PT_END(pt);
}


/*
 * Led task
 *
 */
static int led_task(struct pt * pt) {
    static uint32_t time_count = 0;
    PT_BEGIN(pt);

    while(1) {
        PT_WAIT_UNTIL(pt, flag_run_enabled);
        {

            switch (get_led1_state()) {
                //--------------------------------------------------------------
                case 0:      // off state
                    {
                        uint32_t tick = get_sys_tick();
                        if (tick > time_count) {
                            time_count = tick + TIMER0_TICK / 50;
                            led1_on();
                        }
                    }
                    break;

                //--------------------------------------------------------------
                case 1:      // on state
                    {
                        uint32_t tick = get_sys_tick();
                        if (tick > time_count) {
                            time_count = tick + TIMER0_TICK;
                            led1_off();
                        }
                    }
                    break;

                //--------------------------------------------------------------
                default:
                    led1_off();
                    break;
            }
        }
        PT_WAIT_UNTIL(pt, !flag_run_enabled);
    }

    PT_END(pt);
}


/*
 * Test task
 *
 */
static bool_t test_task(const uint32_t __task_num) {
    return BIT_TEST(enabled_tasks, __task_num) ? TRUE_T : FALSE_T;
}


/*
 * Enable update task
 *
 */
void enable_update_task() {
    BIT_SET(enabled_tasks, UPDATE_TASK);
    upd_state = 0;
}


/*
 * Disable update task
 *
 */
void disable_update_task() {
    BIT_CLEAR(enabled_tasks, UPDATE_TASK);
}


/*
 * Init tasks
 *
 */
void init_tasks() {
    PT_INIT(&first_pt);
    PT_INIT(&led_pt);
    PT_INIT(&upd_pt);

    DEBUG_PRINTF("\r\nTasks: 0x%8x\r\n", enabled_tasks);
}


/*
 * Get enabled tasks
 *
 */
uint32_t get_en_tasks() {
    return enabled_tasks;
}


/*
 * Run tasks
 *
 */
void run_tasks() {
    first_task(&first_pt);
    led_task(&led_pt);

    if (test_task(UPDATE_TASK)) {
        update_task(&upd_pt);
    }

    flag_run_enabled = 0;
}
