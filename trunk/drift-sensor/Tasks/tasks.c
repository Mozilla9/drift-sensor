/*
 * $Id$
 */


#include "data_types.h"
#include "Core\core.h"
#include "Uart\v_printf.h"
#include "Protothread\pt.h"
#include "Sett\settings.h"
#include "Led\led.h"
#include "Lis3dh\lis3dh.h"
#include "Compass\compass.h"
#include "Drift\drift.h"
#include "Can\can_handlers.h"
#include "Tasks\tasks.h"


#define ACC_TASK          1
#define ACC_MATRIX_TASK   2
#define CAN_TASK          3
#define CALIBR_TASK       6



static uint32_t flag_run_enabled = 0;
static uint32_t enabled_tasks = 0x00000C;  // ACC_MATRIX_TASK, CAN_TASK enabled by default
static uint16_t calibrating_state = 0;

static struct pt first_pt;
static struct pt acc_pt;
static struct pt acc_matrix_pt;
static struct pt calibr_pt;
static struct pt led_pt;
static struct pt can_pt;


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
 * Calibrating task
 *
 */
static int calibr_task(struct pt * pt) {
    static uint32_t time_count = 0;
    PT_BEGIN(pt);

    while(1) {
        PT_WAIT_UNTIL(pt, flag_run_enabled);
        {
            read_acc_lis3dh();
            vector * pA = get_acc_lis3dh();

            switch (calibrating_state) {
                //--------------------------------------------------------------
                case 0:      // initial state
                    init_calibrating_data();
                    time_count = get_sys_tick() + 3 * TIMER0_TICK;
                    serprintf("$DRIFT,STAGE1\r\n");
                    calibrating_state++;

                    disable_acc_matrix_task();
                    break;

                //--------------------------------------------------------------
                case 1:      // accumulation quiet
                    {
                        add_acc_samples_in_calibr(pA->x, pA->y, pA->z);
                        uint32_t tick = get_sys_tick();
                        if (tick > time_count) {
                            calibrating_state++;

                            commit_quiet_calibr_data();
                            time_count = tick + 70 * TIMER0_TICK;
                        }
                    }
                    break;

                //--------------------------------------------------------------
                case 2:     // acceleration
                    {
                        add_acc_samples_in_calibr(pA->x, pA->y, pA->z);
                        handle_acc_samples_in_calibr();
                        uint32_t tick = get_sys_tick();
                        if (tick > time_count) {
                            serprintf("$DRIFT,STAGE2\r\n");
                            calibrating_state++;

                            if (commit_motion_calibr_data() == 0) {
                                commit_axis_calibr_data();
                            }

                            print_calibr_results();
                            disable_calibr_task();
                            enable_acc_matrix_task();
                        }
                    }
                    break;

                //--------------------------------------------------------------
                default:
                    break;
            }
        }
        PT_WAIT_UNTIL(pt, !flag_run_enabled);
    }

    PT_END(pt);
}


/*
 * Accelerometer task
 *
 */
static int acc_task(struct pt * pt) {

    PT_BEGIN(pt);

    while(1) {
        PT_WAIT_UNTIL(pt, flag_run_enabled);
        {
            read_acc_lis3dh();
            vector * pA = get_acc_lis3dh();

            DEBUG_PRINTF("acc=%d,%d,%d,%d\r\n", pA->x, pA->y, pA->z, pA->status);

            if (!get_trace())
                add_acc_samples_in_reco_drift(pA->x, pA->y, pA->z);
        }
        PT_WAIT_UNTIL(pt, !flag_run_enabled);
    }

    PT_END(pt);
}


/*
 * Can task
 *
 */
static int can_task(struct pt * pt) {
    PT_BEGIN(pt);

    while(1) {
        PT_WAIT_UNTIL(pt, flag_run_enabled);
        {
            __can_param_t * param;
            int8_t * pPattern;
            uint32_t res;
            uint16_t idx;

            idx = 1;
            while (idx < CAN_LISTEN_PARAM) {
                param = get_next_can_data(idx++);
                
                if (param) {
                    res = (uint32_t)(param->data >> param->pos);

                    switch (param->len) {
                      case 2:
                        res &= 0x00000003;
                        pPattern = "$CAN,%2X,%2X\r\n";
                        break;

                      case 8:
                        res &= 0x000000FF;
                        pPattern = "$CAN,%2X,%2X\r\n";
                        break;

                      case 16:
                        res &= 0x0000FFFF;
                        pPattern = "$CAN,%2X,%4X\r\n";
                        break;

                      case 32:
                        pPattern = "$CAN,%2X,%8X\r\n";
                        break;

                      default:
                        continue;
                    }

                    serprintf(pPattern, param->label, res);

                    param->data = 0;
                    param->len = 0;
                    param->pos = 0;
                }
            }
        }
        PT_WAIT_UNTIL(pt, !flag_run_enabled);
    }

    PT_END(pt);
}


/*
 * Accelerometer matrix task
 *
 */
static int acc_matrix_task(struct pt * pt) {

    PT_BEGIN(pt);

    while(1) {
        PT_WAIT_UNTIL(pt, flag_run_enabled);
        {
            read_acc_lis3dh();
            vector * pA = get_acc_lis3dh();
            add_acc_matrix_samples_in_reco_drift(pA->x, pA->y, pA->z);
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
                            time_count = tick + TIMER0_TICK * 3;
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
 * Test acc task
 *
 */
static bool_t test_acc_task() {
    return BIT_TEST(enabled_tasks, ACC_TASK) ? TRUE_T : FALSE_T;
}


/*
 * Enable acc task
 *
 */
void enable_acc_task() {
    BIT_SET(enabled_tasks, ACC_TASK);
}


/*
 * Disable acc task
 *
 */
void disable_acc_task() {
    BIT_CLEAR(enabled_tasks, ACC_TASK);
}


/*
 * Test can task
 *
 */
static bool_t test_can_task() {
    return BIT_TEST(enabled_tasks, CAN_TASK) ? TRUE_T : FALSE_T;
}


/*
 * Enable can task
 *
 */
void enable_can_task() {
    BIT_SET(enabled_tasks, CAN_TASK);
}


/*
 * Disable can task
 *
 */
void disable_can_task() {
    BIT_CLEAR(enabled_tasks, CAN_TASK);
}


/*
 * Test acc matrix task
 *
 */
static bool_t test_acc_matrix_task() {
    return BIT_TEST(enabled_tasks, ACC_MATRIX_TASK) ? TRUE_T : FALSE_T;
}


/*
 * Enable acc matrix task
 *
 */
void enable_acc_matrix_task() {
    BIT_SET(enabled_tasks, ACC_MATRIX_TASK);
}


/*
 * Disable acc matrix task
 *
 */
void disable_acc_matrix_task() {
    BIT_CLEAR(enabled_tasks, ACC_MATRIX_TASK);
}


/*
 * Test calibr task
 *
 */
static bool_t test_calibr_task() {
    return BIT_TEST(enabled_tasks, CALIBR_TASK) ? TRUE_T : FALSE_T;
}


/*
 * Enable calibr task
 *
 */
void enable_calibr_task() {
    BIT_SET(enabled_tasks, CALIBR_TASK);
    calibrating_state = 0;
}


/*
 * Disable calibr task
 *
 */
void disable_calibr_task() {
    BIT_CLEAR(enabled_tasks, CALIBR_TASK);
}


/*
 * Init tasks
 *
 */
void init_tasks() {
    __FMEM_DATA data;

    PT_INIT(&first_pt);
    PT_INIT(&led_pt);
    PT_INIT(&calibr_pt);
    PT_INIT(&acc_pt);
    PT_INIT(&acc_matrix_pt);
    PT_INIT(&can_pt);

    // read sett
    data.addr = DEV_EN_TASKS_ADDR;
    data.pBuff = (uint8_t *) &enabled_tasks;
    data.len = 4;
    read_app_settings(&data);

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

    if (test_calibr_task()) {
        calibr_task(&calibr_pt);
        flag_run_enabled = 0;
        return;
    }

    if (test_acc_matrix_task()) {
        acc_matrix_task(&acc_matrix_pt);
    }

    if (test_acc_task()) {
        acc_task(&acc_pt);
    }

    if (test_can_task()) {
        can_task(&can_pt);
    }

    flag_run_enabled = 0;
}
