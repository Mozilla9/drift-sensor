/*
 * $Id$
 */


#include "data_types.h"
#include "Core\core.h"
#include "Uart\v_printf.h"
#include "Can\can_sig.h"
#include "Can\can_baud_table.h"
#include "Can\can.h"
#include "Can\can_handlers.h"


static __can_param_t __can_params[CAN_LISTEN_PARAM];


/*
 * Init can params
 *
 */
static void init_params_data() {
    // 0
    // not used?

    //1
    __can_params[1].pgn = 61443; // spn91 – Accelerator Pedal Position
    __can_params[1].pos = 8;
    __can_params[1].len = 8;
    __can_params[1].label = 1;
    __can_params[1].ready = 0;
    DEBUG_PRINTF("SPN %4x\r\n", __can_params[1].label);

    //2
    __can_params[2].pgn = 65276; // spn96 – Fuel Level
    __can_params[2].pos = 8;
    __can_params[2].len = 8;
    __can_params[2].label = 2;
    __can_params[2].ready = 0;
    DEBUG_PRINTF("SPN %4x\r\n", __can_params[2].label);

    //3
    __can_params[3].pgn = 65262; // spn110 – Engine Coolant Temperature
    __can_params[3].pos = 0;
    __can_params[3].len = 8;
    __can_params[3].label = 3;
    __can_params[3].ready = 0;
    DEBUG_PRINTF("SPN %4x\r\n", __can_params[3].label);

    //4
    __can_params[4].pgn = 65269; // spn171 – Ambient Air Temperature
    __can_params[4].pos = 24;
    __can_params[4].len = 16;
    __can_params[4].label = 4;
    __can_params[4].ready = 0;
    DEBUG_PRINTF("SPN %4x\r\n", __can_params[4].label);

    //5
    __can_params[5].pgn = 65262; // spn174 – Fuel Temperature
    __can_params[5].pos = 8;
    __can_params[5].len = 8;
    __can_params[5].label = 5;
    __can_params[5].ready = 0;
    DEBUG_PRINTF("SPN %4x\r\n", __can_params[5].label);

    //6
    __can_params[6].pgn = 65266; // spn183 – Fuel Rate
    __can_params[6].pos = 0;
    __can_params[6].len = 16;
    __can_params[6].label = 6;
    __can_params[6].ready = 0;
    DEBUG_PRINTF("SPN %4x\r\n", __can_params[6].label);

    //7
    __can_params[7].pgn = 61444; // spn190 – Engine Speed
    __can_params[7].pos = 32;
    __can_params[7].len = 2;
    __can_params[7].label = 7;
    __can_params[7].ready = 0;
    DEBUG_PRINTF("SPN %4x\r\n", __can_params[7].label);

    //8
    __can_params[8].pgn = 61441; // spn521 – Brake Pedal Position
    __can_params[8].pos = 8;
    __can_params[8].len = 8;
    __can_params[8].label = 8;
    __can_params[8].ready = 0;
    DEBUG_PRINTF("SPN %4x\r\n", __can_params[8].label);

    //9
    __can_params[9].pgn = 61442; // spn522 – Percent Clutch Slip
    __can_params[9].pos = 32;
    __can_params[9].len = 8;
    __can_params[9].label = 9;
    __can_params[9].ready = 0;
    DEBUG_PRINTF("SPN %4x\r\n", __can_params[9].label);

    //10
    __can_params[10].pgn = 61445; // spn523 – Current Gear
    __can_params[10].pos = 32;
    __can_params[10].len = 8;
    __can_params[10].label = 10;
    __can_params[10].ready = 0;
    DEBUG_PRINTF("SPN %4x\r\n", __can_params[10].label);

    //11
    __can_params[11].pgn = 65217; // spn917 – High Resolution Total Vehicle Distance
    __can_params[11].pos = 0;
    __can_params[11].len = 32;
    __can_params[11].label = 11;
    __can_params[11].ready = 0;
    DEBUG_PRINTF("SPN %4x\r\n", __can_params[11].label);

    //12
    __can_params[12].pgn = 65132; // spn1614 – Overspeed
    __can_params[12].pos = 15;
    __can_params[12].len = 2;
    __can_params[12].label = 12;
    __can_params[12].ready = 0;
    DEBUG_PRINTF("SPN %4x\r\n", __can_params[12].label);

    //13
    __can_params[13].pgn = 65114; // spn1744 – Door Release
    __can_params[13].pos = 44;
    __can_params[13].len = 2;
    __can_params[13].label = 13;
    __can_params[13].ready = 0;
    DEBUG_PRINTF("SPN %4x\r\n", __can_params[13].label);

    //14
    __can_params[14].pgn = 65265; // spn70 – Parking Brake Switch
    __can_params[14].pos = 10;
    __can_params[14].len = 2;
    __can_params[14].label = 14;
    __can_params[14].ready = 0;
    DEBUG_PRINTF("SPN %4x\r\n", __can_params[14].label);

    //15
    __can_params[15].pgn = 65258; // spn181 – Cargo Weight
    __can_params[15].pos = 48;
    __can_params[15].len = 16;
    __can_params[15].label = 15;
    __can_params[15].ready = 0;
    DEBUG_PRINTF("SPN %4x\r\n", __can_params[15].label);

    //16
    __can_params[16].pgn = 65244; // spn235 – Total Idle Hours
    __can_params[16].pos = 32;
    __can_params[16].len = 32;
    __can_params[16].label = 16;
    __can_params[16].ready = 0;
    DEBUG_PRINTF("SPN %4x\r\n", __can_params[16].label);

    //17
    __can_params[17].pgn = 65244; // spn236 – Total Idle Fuel Used
    __can_params[17].pos = 0;
    __can_params[17].len = 32;
    __can_params[17].label = 17;
    __can_params[17].ready = 0;
    DEBUG_PRINTF("SPN %4x\r\n", __can_params[17].label);

    //18
    __can_params[18].pgn = 65257; // spn250 – Total Fuel Used
    __can_params[18].pos = 32;
    __can_params[18].len = 32;
    __can_params[18].label = 18;
    __can_params[18].ready = 0;
    DEBUG_PRINTF("SPN %4x\r\n", __can_params[18].label);

    //19
    __can_params[19].pgn = 65216; // spn914 – Service Distance
    __can_params[19].pos = 8;
    __can_params[19].len = 16;
    __can_params[19].label = 19;
    __can_params[19].ready = 0;
    DEBUG_PRINTF("SPN %4x\r\n", __can_params[19].label);

    //20
    __can_params[20].pgn = 65153; // spn1440 – Fuel Flow Rate 1
    __can_params[20].pos = 0;
    __can_params[20].len = 16;
    __can_params[20].label = 20;
    __can_params[20].ready = 0;
    DEBUG_PRINTF("SPN %4x\r\n", __can_params[20].label);

    //21
    __can_params[21].pgn = 65136; // spn1585 – Powered Vehicle Weight
    __can_params[21].pos = 0;
    __can_params[21].len = 16;
    __can_params[21].label = 21;
    __can_params[21].ready = 0;
    DEBUG_PRINTF("SPN %4x\r\n", __can_params[21].label);
}



/*
 * CAN errors handling callback function
 *
 */
static void cb_err_can1(const sint16_t sig) {
    if (sig == CIEV_WTOUT) {
        DEBUG_PRINTF("CAN: WTOUT occured\n\r");
    } else if (sig == CIEV_EWL) {
        DEBUG_PRINTF("CAN: EWL occured\n\r");
    } else if (sig == CIEV_BOFF) {
        DEBUG_PRINTF("CAN: Bus Off occured\n\r");
    } else if (sig == CIEV_HOVR) {
        DEBUG_PRINTF("CAN: HOVR occured\n\r");
    } else if (sig == CIEV_SOVR) {
        DEBUG_PRINTF("CAN: SOVR occured\n\r");
    }
}


/*
 * Can receive handling callback function
 *
 */
static void cb_rcv_can1(const sint16_t sig) {
    __canmsg_t rx;
    uint32_t pgn_no = 0x0;
    uint8_t pdu_f = 0;
    ulong64_t xtracted;
    __can_param_t * prm;

    if (ci_read(CAN1_CHAN, &rx, 1) > 0) {
        pdu_f = (uint8_t) ((rx.id >> 16) & 0xff);

        if ((pdu_f & 0xff) < 0xf0) {
            pgn_no = (uint32_t) ((rx.id >> 8) & (uint32_t) 0x0003ff00);
        } else {
            pgn_no = (uint32_t) ((rx.id >> 8) & (uint32_t) 0x0003ffff);
        }

        for (uint16_t i = 1; i < CAN_LISTEN_PARAM; i++) {
            prm = __can_params + i;
            if (pgn_no == prm->pgn) {
                for (uint16_t cnt = 0; cnt < 8; cnt++) {
                    ((uint8_t *)&xtracted)[cnt] = rx.data[cnt];
                }

                xtracted = xtracted >> prm->pos;          // clr low sig bits
                xtracted = xtracted << (64 - prm->len);   // clr high sig bits
                xtracted = xtracted >> (64 - prm->len);

                prm->ready = 1;
                prm->data = xtracted;
                break;
            }
        }
    }
}


/*
 * Poll new msg on can
 *
 */
void poll_can_msg() {
    ci_poll_sigs();
}


/*
 * Poll new msg on can
 *
 */
void init_can_j1939() {
    sint16_t error;

    DEBUG_PRINTF("\r\nInit J1939:\r\n");
    init_params_data();

    error = ci_open(CAN1_CHAN, CIO_CAN11 | CIO_CAN29, BCI_250K);
    DEBUG_PRINTF("\r\nci_open(BCI_250K), %d\r\n", error);

    // uncomment if need listen only mode
    //error = ci_set_lom(CAN1_CHAN, CI_LOM_ON);
    //DEBUG_PRINTF("ci_set_lom(), %d\n\r", error);

    error = ci_set_cb(CAN1_CHAN, CIEV_RC, cb_rcv_can1);
    DEBUG_PRINTF("ci_set_cb(rc), %d\n\r", error);

    error = ci_set_cb(CAN1_CHAN, CIEV_CANERR, cb_err_can1);
    DEBUG_PRINTF("ci_set_cb(er), %d\n\r", error);

    error = ci_start(CAN1_CHAN);
    DEBUG_PRINTF("ci_start(), %d\n\r", error);
}


/*
 * Send arrived data to uart
 *
 */
__can_param_t * get_next_can_data(const uint16_t idx) {
    if (idx == 0 || idx >= CAN_LISTEN_PARAM)
        return 0;

    if (__can_params[idx].ready > 0) {
        __can_params[idx].ready = 0;
        return __can_params + idx;
    } else {
        return 0;
    }
}
