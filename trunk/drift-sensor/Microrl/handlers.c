/*
 * $Id$
 */

#include "data_types.h"
#include "Core\core.h"
#include "Lib\bin_to_bcd.h"
#include "Led\led.h"
#include "Uart\v_printf.h"
#include "Ringbuff\ring_buffer.h"
#include "Sett\settings.h"
#include "Drift\drift.h"
#include "Compass\compass.h"
#include "Tasks\tasks.h"
#include "Microrl\microrl.h"
#include "Microrl\handlers.h"


// definition commands word
#define _CMD_HELP            "help"
#define _CMD_RESET           "reset"
#define _CMD_CLEAR           "cls"
#define _CMD_TIME            "time"
#define _CMD_TRACE           "trace"
#define _CMD_LED             "led"
#define _CMD_ACC_TASK        "acc"
#define _CMD_ACC_MATRIX_TASK "mtr"
#define _CMD_ACC_XX          "ax?"
#define _CMD_ACC_YY          "ay?"
#define _CMD_ACC_ZZ          "az?"
#define _CMD_ACC_ZZ_PIT      "ap?"
#define _CMD_ACC_ZZ_MOD      "am?"
#define _CMD_ACC_X           "ax"
#define _CMD_ACC_Y           "ay"
#define _CMD_ACC_Z           "az"
#define _CMD_ACC_Z_PIT       "ap"
#define _CMD_ACC_Z_MOD       "am"
#define _CMD_CALB            "calb"
#define _CMD_SW_PROTO        "proto"
#define _CMD_AXIS_INV        "invert"


// available  commands
int8_t * keyword [] = {
    _CMD_HELP,
    _CMD_TIME,
    _CMD_RESET,
    _CMD_TRACE,
    _CMD_LED,
    _CMD_ACC_TASK,
    _CMD_ACC_MATRIX_TASK,
    _CMD_ACC_XX,
    _CMD_ACC_YY,
    _CMD_ACC_ZZ,
    _CMD_ACC_ZZ_PIT,
    _CMD_ACC_ZZ_MOD,
    _CMD_ACC_X,
    _CMD_ACC_Y,
    _CMD_ACC_Z,
    _CMD_ACC_Z_PIT,
    _CMD_ACC_Z_MOD,
    _CMD_CALB,
    _CMD_SW_PROTO,
    _CMD_AXIS_INV,
    _CMD_CLEAR
};

#define _NUM_OF_CMD    21


// array for comletion
int8_t * compl_word [_NUM_OF_CMD + 22];




/*
 * Print help
 *
 */
void print_help_cmd () {
    (*get_microrl_printf (pointerMicrorl)) ("Use TAB key for completion\n\rCommand:\n\r");
    (*get_microrl_printf (pointerMicrorl)) ("\tctrl+C      - en/dis console\n\r");
    (*get_microrl_printf (pointerMicrorl)) ("\thelp        - print it info\n\r");
    (*get_microrl_printf (pointerMicrorl)) ("\tcls         - clear screen\n\r");
    (*get_microrl_printf (pointerMicrorl)) ("\ttime        - time in sec continius from start\n\r");
    (*get_microrl_printf (pointerMicrorl)) ("\treset       - reset device\n\r");
    (*get_microrl_printf (pointerMicrorl)) ("\ttrace       - on/off debug trace\n\r");
    (*get_microrl_printf (pointerMicrorl)) ("\tled         - on/off led\n\r");
    (*get_microrl_printf (pointerMicrorl)) ("\tcalb        - on/off run calibrating\n\r");
    (*get_microrl_printf (pointerMicrorl)) ("\tmtr         - on/off acc matrix task\n\r");
    (*get_microrl_printf (pointerMicrorl)) ("\tacc         - on/off acc task\n\r");
    (*get_microrl_printf (pointerMicrorl)) ("\tax?         - get acc X sett (med, up, down, gist, wind, average)\n\r");
    (*get_microrl_printf (pointerMicrorl)) ("\tay?         - get acc Y sett\n\r");
    (*get_microrl_printf (pointerMicrorl)) ("\taz?         - get acc Z sett\n\r");
    (*get_microrl_printf (pointerMicrorl)) ("\tap?         - get acc Z_pit sett\n\r");
    (*get_microrl_printf (pointerMicrorl)) ("\tam?         - get acc Z_mod sett\n\r");
    (*get_microrl_printf (pointerMicrorl)) ("\tax          - set acc X sett (med, up, down, gist, wind, average)\n\r");
    (*get_microrl_printf (pointerMicrorl)) ("\tay          - set acc Y sett\n\r");
    (*get_microrl_printf (pointerMicrorl)) ("\taz          - set acc Z sett\n\r");
    (*get_microrl_printf (pointerMicrorl)) ("\tap          - set acc Z_pit sett\n\r");
    (*get_microrl_printf (pointerMicrorl)) ("\tam          - set acc Z_mod sett\n\r");
    (*get_microrl_printf (pointerMicrorl)) ("\tproto       - switch protocol to sdp\n\r");
    (*get_microrl_printf (pointerMicrorl)) ("\tinvert      - inverting axises X, Y on/off\n\r");
}


/*
 * execute callback for microrl library
 * do what you want here, but don't write to argv!!! read only!!
 *
 */
int32_t execute (const int32_t argc, const int8_t * const * argv) {
    int32_t i = 0;
    __FMEM_DATA fdata;

    // just iterate through argv word and compare it with your commands
    while (i < argc) {
        if (strcmp (argv[i], _CMD_HELP) == 0) {
            print_help_cmd();
        }
        else if (strcmp (argv[i], _CMD_TIME) == 0) {
            (*get_microrl_printf (pointerMicrorl)) ("time alive = %u sec\n\r", get_time());
        }
        else if (strcmp (argv[i], _CMD_RESET) == 0) {
            reset_device();
        }
        else if (strcmp (argv[i], _CMD_SW_PROTO) == 0) {
            set_proto_type(PROTO_TYPE_SDP);
        }
        else if (strcmp (argv[i], _CMD_AXIS_INV) == 0) {
            set_proto_type(PROTO_TYPE_SDP);

            if (++i < argc) {
                uint32_t flg = 0;
                fdata.addr = ACC_INV_AXIS_FLG_ADDR;
                fdata.pBuff = (uint8_t *) &flg;
                fdata.len = 4;

                if (strcmp (argv[i], "on") == 0) {
                    set_axis_inv_flg(AXIS_INV_FLAG);

                    flg = AXIS_INV_FLAG;
                    write_app_settings(&fdata);
                }
                else if (strcmp (argv[i], "off") == 0) {
                    set_axis_inv_flg(0);
                    (*get_microrl_printf (pointerMicrorl)) ("\n\r");

                    write_app_settings(&fdata);
                }
                else {
                    (*get_microrl_printf (pointerMicrorl)) ("invalid cmd argument");
                }

                (*get_microrl_printf (pointerMicrorl)) ("\n\r");
            }
        }
        else if (strcmp (argv[i], _CMD_TRACE) == 0) {
            if (++i < argc) {
                uint32_t flg;
                fdata.addr = DEV_EN_TRACE_ADDR;
                fdata.pBuff = (uint8_t *) &flg;
                fdata.len = 4;

                if (strcmp (argv[i], "on") == 0) {
                    set_trace_func(&(*get_microrl_printf (pointerMicrorl)));

                    flg = 0xAA55AA55;
                    write_app_settings(&fdata);
                }
                else if (strcmp (argv[i], "off") == 0) {
                    set_trace_func(0);

                    flg = 0;
                    write_app_settings(&fdata);
                }
                else {
                    (*get_microrl_printf (pointerMicrorl)) ("invalid cmd argument");
                }

                (*get_microrl_printf (pointerMicrorl)) ("\n\r");
            }
        }
        else if (strcmp (argv[i], _CMD_CALB) == 0) {
            if (++i < argc) {
                void (* func) () = 0;

                if (strcmp (argv[i], "on") == 0) {
                    func = &enable_calibr_task;
                }
                else if (strcmp (argv[i], "off") == 0) {
                    func = &disable_calibr_task;
                }

                if (func) {
                    (*func)();
                    (*get_microrl_printf (pointerMicrorl)) ("\n\r");
                } else {
                    (*get_microrl_printf (pointerMicrorl)) ("invalid cmd argument\n\r");
                }
            }
        }
        else if (strcmp (argv[i], _CMD_LED) == 0) {
            if (++i < argc) {
                void (* func) () = 0;

                if (strcmp (argv[i], "on") == 0) {
                    func = &led1_on;
                }
                else if (strcmp (argv[i], "off") == 0) {
                    func = &led1_off;
                }
                else {
                    (*get_microrl_printf (pointerMicrorl)) ("invalid cmd argument\n\r");
                }

                if (func) (*func)();
                (*get_microrl_printf (pointerMicrorl)) ("\n\r");
            }
        }
        else if ((strcmp (argv[i], _CMD_ACC_TASK) == 0)
                    || (strcmp (argv[i], _CMD_ACC_MATRIX_TASK) == 0)) {
            if ((i + 1) < argc) {
                void (* pFunc) () = 0;
                uint16_t isEn = 0;

                if (strcmp (argv[i + 1], "on") == 0) {
                    isEn = 1;
                }
                else if (strcmp (argv[i + 1], "off") == 0) {
                    isEn = 2;
                }

                if (isEn) {
                    if (argv[i][1] == 'c') {
                        pFunc = isEn == 1 ? &enable_acc_task : &disable_acc_task;
                    }
                    else if (argv[i][1] == 't') {
                        pFunc = isEn == 1 ? &enable_acc_matrix_task : &disable_acc_matrix_task;
                    }
                }

                if (pFunc) {
                    (*pFunc)();
                    (*get_microrl_printf (pointerMicrorl)) ("\n\r");

                    const uint32_t tasks_reg = get_en_tasks();
                    fdata.addr = DEV_EN_TASKS_ADDR;
                    fdata.pBuff = (uint8_t *)&tasks_reg;
                    fdata.len = 4;
                    write_app_settings(&fdata);
                }
                else {
                    (*get_microrl_printf (pointerMicrorl)) ("invalid cmd argument\n\r");
                }
            }
        }
        else if ((strcmp (argv[i], _CMD_ACC_XX) == 0)
                    || (strcmp (argv[i], _CMD_ACC_YY) == 0)
                    || (strcmp (argv[i], _CMD_ACC_ZZ) == 0)
                    || (strcmp (argv[i], _CMD_ACC_ZZ_MOD) == 0)
                    || (strcmp (argv[i], _CMD_ACC_ZZ_PIT) == 0)) {
            drift_threshold * pThr = 0;
            const int8_t * pMsg = "\r\nmed=%d\n\rup=%d\n\rdown=%d\n\rgist=%d\r\nwind=%u\r\nav=%u\n\r";

            if (argv[i][1] == 'x') {
                pThr = get_x_acc_threshold();
            }
            else if (argv[i][1] == 'y') {
                pThr = get_y_acc_threshold();
            }
            else if (argv[i][1] == 'z') {
                pThr = get_z_acc_threshold();
            }
            else if (argv[i][1] == 'p') {
                pThr = get_pit_acc_threshold();
            }
            else if (argv[i][1] == 'm') {
                pThr = get_mod_acc_threshold();
            }

            if (pThr) {
                (*get_microrl_printf (pointerMicrorl))(pMsg, pThr->med, pThr->up_front, pThr->down_front, pThr->gist, pThr->wind_size, pThr->average_type);
            } else {
                (*get_microrl_printf (pointerMicrorl))("\n\r");
            }
        }
        else if ((strcmp (argv[i], _CMD_ACC_X) == 0)
                    || (strcmp (argv[i], _CMD_ACC_Y) == 0)
                    || (strcmp (argv[i], _CMD_ACC_Z) == 0)
                    || (strcmp (argv[i], _CMD_ACC_Z_PIT) == 0)
                    || (strcmp (argv[i], _CMD_ACC_Z_MOD) == 0)) {
            if ((i + 6) < argc) {
                drift_threshold * pThr = 0;
                average_buff * pFilter = 0;
                drift_threshold tr_data;

                if (argv[i][1] == 'x') {
                    pThr = get_x_acc_threshold();
                    pFilter = get_x_drift_filter();
                    fdata.addr = 0;
                }
                else if (argv[i][1] == 'y') {
                    pThr = get_y_acc_threshold();
                    pFilter = get_y_drift_filter();
                    fdata.addr = 1;
                }
                else if (argv[i][1] == 'z') {
                    pThr = get_z_acc_threshold();
                    pFilter = get_z_drift_filter();
                    fdata.addr = 2;
                }
                else if (argv[i][1] == 'p') {
                    pThr = get_pit_acc_threshold();
                    pFilter = get_pit_drift_filter();
                    fdata.addr = 3;
                }
                else if (argv[i][1] == 'm') {
                    pThr = get_mod_acc_threshold();
                    pFilter = get_mod_drift_filter();
                    fdata.addr = 4;
                }

                tr_data.med = txt_to_bin(argv[++i]);
                tr_data.up_front = txt_to_bin(argv[++i]);
                tr_data.down_front = txt_to_bin(argv[++i]);
                tr_data.gist = txt_to_bin(argv[++i]);
                tr_data.wind_size = txt_to_bin(argv[++i]);
                tr_data.average_type = txt_to_bin(argv[++i]);

                if (pThr && pFilter) {
                    set_acc_threshold(pThr, &tr_data);
                    set_wind_size_average_filter(pFilter, tr_data.wind_size);
                    set_average_type_filter(pFilter, tr_data.average_type);

                    fdata.addr = ACC_FILTR_DATA_ADDR + fdata.addr * sizeof(drift_threshold);
                    fdata.pBuff = (uint8_t *)&tr_data;
                    fdata.len = sizeof(drift_threshold);
                    write_app_settings(&fdata);

                    (*get_microrl_printf (pointerMicrorl))("\n\r");
                } else {
                    (*get_microrl_printf (pointerMicrorl))("invalid cmd arguments\n\r");
                }
            } else {
                (*get_microrl_printf (pointerMicrorl))("invalid cmd arguments\n\r");
            }
        }
        else if (strcmp (argv[i], _CMD_CLEAR) == 0) {
            (*get_microrl_printf (pointerMicrorl)) ("\033[2J");    // ESC seq for clear entire screen
            (*get_microrl_printf (pointerMicrorl)) ("\033[H");     // ESC seq for move cursor at left-top corner
        }
        else {
            (*get_microrl_printf (pointerMicrorl)) ("command: '");
            (*get_microrl_printf (pointerMicrorl)) ((int8_t *)argv[i]);
            (*get_microrl_printf (pointerMicrorl)) ("' Not found.\n\r");
        }

        break;
    }
    return 0;
}


/*
 * completion callback for microrl library
 *
 */
int8_t ** complet (int32_t argc, const int8_t * const * argv) {
    int32_t j = 0;
    compl_word [0] = NULL;

    // if there is token in cmdline
    if (argc == 1) {
        // get last entered token
        int8_t * bit = (int8_t *)argv [argc - 1];
        for (int32_t i = 0; i < _NUM_OF_CMD; i++) {
            if (strstr(keyword[i], bit) == keyword [i]) {
                compl_word[j++] = keyword[i];
            }
        }
    }
    else { // if there is no token in cmdline, just print all available token
        for (; j < _NUM_OF_CMD; j++) {
            compl_word[j] = keyword[j];
        }
    }

    // note! last ptr in array always must be NULL!!!
    compl_word [j] = NULL;
    return compl_word;
}


/*
 * Sigint
 *
 */
void sigint () {
    (*get_microrl_printf (pointerMicrorl))("^C\n\r");
    microrl_init_printf(pointerMicrorl,
        get_microrl_printf (pointerMicrorl) == &serprintf ? NULL : &serprintf);
}
