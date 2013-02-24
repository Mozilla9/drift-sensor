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

/*
 * request
 * <$><get><CRLF>
 * replay
 * <$><lable = max 16 byte>:<data max 16 byte>:<CRC8 2 byte><CRLF>
 * <$><lable = max 16 byte>:<data max 16 byte>:<CRC8 2 byte><CRLF>
 * <$><lable = max 16 byte>:<data max 16 byte>:<CRC8 2 byte><CRLF>

 * <$><lable = max 16 byte>:<data max 16 byte>:<CRC8 2 byte><CRLF>
 * <$><lable = max 16 byte>:<data max 16 byte>:<CRC8 2 byte><CRLF>
 * <$><lable = max 16 byte>:<data max 16 byte>:<CRC8 2 byte><CRLF>
 *
 * MAX 100 parameters
 */


#define LISTEN_PARAM     22
#define CAN1_CHAN        0


static __can_param_t __can_params[LISTEN_PARAM];


/*
 * Init can params
 *
 */
static void init_params_data() {
    // 0
    // not used?

    //1
    __can_params[1].pgn = 61443; // spn91 � Accelerator Pedal Position
    __can_params[1].pos = 8;
    __can_params[1].len = 8;
    __can_params[1].pLabel = "SPN 0091";
    DEBUG_PRINTF("%s\r\n", __can_params[1].pLabel);

    //2
    __can_params[2].pgn = 65276; // spn96 � Fuel Level
    __can_params[2].pos = 8;
    __can_params[2].len = 8;
    __can_params[2].pLabel = "SPN 0096";
    DEBUG_PRINTF("%s\r\n", __can_params[2].pLabel);

    //3
    __can_params[3].pgn = 65262; // spn110 � Engine Coolant Temperature
    __can_params[3].pos = 0;
    __can_params[3].len = 8;
    __can_params[3].pLabel = "SPN 0110";
    DEBUG_PRINTF("%s\r\n", __can_params[3].pLabel);

    //4
    __can_params[4].pgn = 65269; // spn171 � Ambient Air Temperature
    __can_params[4].pos = 24;
    __can_params[4].len = 16;
    __can_params[4].pLabel = "SPN 0171";
    DEBUG_PRINTF("%s\r\n", __can_params[4].pLabel);

    //5
    __can_params[5].pgn = 65262; // spn174 � Fuel Temperature
    __can_params[5].pos = 8;
    __can_params[5].len = 8;
    __can_params[5].pLabel = "SPN 0174";
    DEBUG_PRINTF("%s\r\n", __can_params[5].pLabel);

    //6
    __can_params[6].pgn = 65266; // spn183 � Fuel Rate
    __can_params[6].pos = 0;
    __can_params[6].len = 16;
    __can_params[6].pLabel = "SPN 0183";
    DEBUG_PRINTF("%s\r\n", __can_params[6].pLabel);

    //7
    __can_params[7].pgn = 61444; // spn190 � Engine Speed
    __can_params[7].pos = 32;
    __can_params[7].len = 2;
    __can_params[7].pLabel = "SPN 0190";
    DEBUG_PRINTF("%s\r\n", __can_params[7].pLabel);

    //8
    __can_params[8].pgn = 61441; // spn521 � Brake Pedal Position
    __can_params[8].pos = 8;
    __can_params[8].len = 8;
    __can_params[8].pLabel = "SPN 0521";
    DEBUG_PRINTF("%s\r\n", __can_params[8].pLabel);

    //9
    __can_params[9].pgn = 61442; // spn522 � Percent Clutch Slip
    __can_params[9].pos = 32;
    __can_params[9].len = 8;
    __can_params[9].pLabel = "SPN 0522";
    DEBUG_PRINTF("%s\r\n", __can_params[9].pLabel);

    //10
    __can_params[10].pgn = 61445; // spn523 � Current Gear
    __can_params[10].pos = 32;
    __can_params[10].len = 8;
    __can_params[10].pLabel = "SPN 0523";
    DEBUG_PRINTF("%s\r\n", __can_params[10].pLabel);

    //11
    __can_params[11].pgn = 65217; // spn917 � High Resolution Total Vehicle Distance
    __can_params[11].pos = 0;
    __can_params[11].len = 32;
    __can_params[11].pLabel = "SPN 0917";
    DEBUG_PRINTF("%s\r\n", __can_params[11].pLabel);

    //12
    __can_params[12].pgn = 65132; // spn1614 � Overspeed
    __can_params[12].pos = 15;
    __can_params[12].len = 2;
    __can_params[12].pLabel = "SPN 1614";
    DEBUG_PRINTF("%s\r\n", __can_params[12].pLabel);

    //13
    __can_params[13].pgn = 65114; // spn1744 � Door Release
    __can_params[13].pos = 44;
    __can_params[13].len = 2;
    __can_params[13].pLabel = "SPN 1744";
    DEBUG_PRINTF("%s\r\n", __can_params[13].pLabel);

    //14
    __can_params[14].pgn = 65265; // spn70 � Parking Brake Switch
    __can_params[14].pos = 10;
    __can_params[14].len = 2;
    __can_params[14].pLabel = "SPN 0070";
    DEBUG_PRINTF("%s\r\n", __can_params[14].pLabel);

    //15
    __can_params[15].pgn = 65258; // spn181 � Cargo Weight
    __can_params[15].pos = 48;
    __can_params[15].len = 16;
    __can_params[15].pLabel = "SPN 0181";
    DEBUG_PRINTF("%s\r\n", __can_params[15].pLabel);

    //16
    __can_params[16].pgn = 65244; // spn235 � Total Idle Hours
    __can_params[16].pos = 32;
    __can_params[16].len = 32;
    __can_params[16].pLabel = "SPN 0235";
    DEBUG_PRINTF("%s\r\n", __can_params[16].pLabel);

    //17
    __can_params[17].pgn = 65244; // spn236 � Total Idle Fuel Used
    __can_params[17].pos = 0;
    __can_params[17].len = 32;
    __can_params[17].pLabel = "SPN 0236";
    DEBUG_PRINTF("%s\r\n", __can_params[17].pLabel);

    //18
    __can_params[18].pgn = 65257; // spn250 � Total Fuel Used
    __can_params[18].pos = 32;
    __can_params[18].len = 32;
    __can_params[18].pLabel = "SPN 0250";
    DEBUG_PRINTF("%s\r\n", __can_params[18].pLabel);

    //19
    __can_params[19].pgn = 65216; // spn914 � Service Distance
    __can_params[19].pos = 8;
    __can_params[19].len = 16;
    __can_params[19].pLabel = "SPN 0914";
    DEBUG_PRINTF("%s\r\n", __can_params[19].pLabel);

    //20
    __can_params[20].pgn = 65153; // spn1440 � Fuel Flow Rate 1
    __can_params[20].pos = 0;
    __can_params[20].len = 16;
    __can_params[20].pLabel = "SPN 1440";
    DEBUG_PRINTF("%s\r\n", __can_params[20].pLabel);

    //21
    __can_params[21].pgn = 65136; // spn1585 � Powered Vehicle Weight
    __can_params[21].pos = 0;
    __can_params[21].len = 16;
    __can_params[21].pLabel = "SPN 1585";
    DEBUG_PRINTF("%s\r\n", __can_params[21].pLabel);
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

    if (ci_read(CAN1_CHAN, &rx, 1) > 0) {
        pdu_f = (uint8_t) ((rx.id >> 16) & 0xff);

        if ((pdu_f & 0xff) < 0xf0) {
            pgn_no = (uint32_t) ((rx.id >> 8) & (uint32_t) 0x0003ff00);
        } else {
            pgn_no = (uint32_t) ((rx.id >> 8) & (uint32_t) 0x0003ffff);
        }

        for (uint16_t i = 1; i < LISTEN_PARAM; i++) {
            if (pgn_no == __can_params[i].pgn) {
                for (uint16_t cnt = 0; cnt < 8; cnt++) {
                    ((uint8_t *)&xtracted)[cnt] = rx.data[cnt];
                }

                xtracted = xtracted >> __can_params[i].pos;          // clr low sig bits
                xtracted = xtracted << (64 - __can_params[i].len);   // clr high sig bits
                xtracted = xtracted >> (64 - __can_params[i].len);

                __can_params[i].data = xtracted;
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


    /* uncomment if need listen only mode

    error = ci_set_lom(CAN1_CHAN, CI_LOM_ON)) < 0) {
    DEBUG_PRINTF("ci_set_lom(), %d\n\r", error);

    */

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
    if (idx == 0 || idx >= LISTEN_PARAM)
        return 0;

    return __can_params + idx;
}