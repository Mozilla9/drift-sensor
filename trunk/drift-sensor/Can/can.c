/*
 * $Id$
 */


#include "data_types.h"
#include "Can\can.h"



// Def settings
#define CI_SIGS_CALLFROMISR      0
#define CI_SIGNUM                7
#define CI_QUEUE_DEFSIZE_RC      32
#define CI_CHAN_NUMS             1

#define CI_CONTEXT_PROCESS       0
#define CI_CONTEXT_IRQ           1

#define msg_zero(pmsg)    ci_memset((pmsg), 0, sizeof(__canmsg_t))
#define msg_isrtr(pmsg)   ((pmsg)->flags & FRAME_RTR)
#define msg_setrtr(pmsg)  do { (pmsg)->flags = (pmsg)->flags | FRAME_RTR; } while (0)
#define msg_iseff(pmsg)   ((pmsg)->flags & FRAME_EFF)
#define msg_seteff(pmsg)  do { (pmsg)->flags = (pmsg)->flags | FRAME_EFF; } while (0)

// Data
static uint16_t __ci_context = CI_CONTEXT_PROCESS;
static __canmsg_t __can_msg[CI_QUEUE_DEFSIZE_RC];
static __ci_sigslot_t __sig_slots[CI_CHAN_NUMS][CI_SIGNUM];
static __ci_can_dev_t __cans_dev[CI_CHAN_NUMS];



/*
 * Enable int for can 1,2
 *
 */
inline static void __enable_can_int() {
    VICINTENABLE_bit.CAN12 = 1;
}


/*
 * Disable int for can 1,2
 *
 */
inline static void __disable_can_int() {
    VICINTENCLEAR_bit.CAN12 = 1;
}


/*
 * Memset
 *
 */
inline static void ci_memset(uint8_t * __addr, const uint8_t __value, const uint32_t __size) {
    for (uint32_t i = 0; i < __size; i++)
        *__addr++ = __value;
}


/*
 * Sleep 100 mks
 *
 */
inline static void sleep_100mks() {
    uint32_t i;
    for (i = 0; i < 2700; i++);
}


/*
 * Sleep 10 mks
 *
 */
inline static void sleep_10mks() {
    uint32_t i;
    for (i = 0; i < 270; i++);
}


/*
 * Init data
 *
 */
void init_can_dev_data() {
    for (uint16_t i = 0; i < CI_CHAN_NUMS; i++) {
        ci_memset((uint8_t *)(__cans_dev + i), 0, sizeof(__ci_can_dev_t));
        __cans_dev[i].sslot = __sig_slots[i];
        __cans_dev[i].sslot_size = CI_SIGNUM;
    }
}


/*
 * Init sig queue
 *
 */
static sint16_t cansig_que_init(__canque_t * const q) {
    q->d = __can_msg;
    q->size = CI_QUEUE_DEFSIZE_RC;
    q->sloc = 0;
    q->rloc = 0;
    q->count = 0;
    return 0;
}


/*
 * Get msg from queue
 *
 */
static sint16_t cansig_que_retr(__canque_t * const q, __canmsg_t * const  msg, const uint16_t num) {
    sint16_t ret;

    if (q->count == 0) {
        return 0;            // queue is empty
    }

    ret = (q->count > num) ? num : q->count;
    for (sint16_t i = 0; i < ret; i++) {
        msg[i] = q->d[q->rloc];
        q->rloc += 1;

        if ( q->rloc >= q->size )
            q->rloc = 0;

        q->count--;
    }

    return ret;
}


/*
 * Init sig
 *
 */
static void ci_sigs_init(const __ci_can_dev_t * const can_dev) {
    for (sint16_t i = 0; i < can_dev->sslot_size; i++) {
        can_dev->sslot[i].sigcnt = 0;
        can_dev->sslot[i].hdl = (void (*) (const uint8_t)) 0;
    }
}


/*
 * Set handler for sig
 *
 */
static void ci_signal(const __ci_can_dev_t * const can_dev, const uint8_t sig, void (* hdl) (const uint8_t)) {
    can_dev->sslot[sig].hdl = hdl;
}


/*
 * Handle sign
 *
 */
static void ci_send_signal(const __ci_can_dev_t * const can_dev, const uint8_t sig) {
    if (can_dev->sslot[sig].hdl) {
        #if CI_SIGS_CALLFROMISR == 1
        can_dev->sslot[sig].hdl(sig);
        #else
        can_dev->sslot[sig].sigcnt += 1;
        #endif
    }
}


/*
 * Init can
 *
 */
void init_can() {
    PCONP_bit.PCAN1 = 1;

    // Conf pins
    PINSEL1_bit.P0_21 = 3;                // RD1
    FIO0DIR_bit.P0_21 = 0;                // input
    PINMODE1_bit.P0_21 = 2;               // pulls dis

    PINSEL1_bit.P0_22 = 3;                // TD1
    FIO0DIR_bit.P0_22 = 1;                // output
    PINMODE1_bit.P0_22 = 2;               // pulls dis

    // Clear can module
    AFMR = 0x3;       // hw-filter - ignore all frames

    CAN1MOD = 0x1;    // Enter Reset Mode
    CAN1IER = 0;      // Disable All Interrupts
    CAN1GSR = 0;      // Clear Status register

    AFMR = 0x2;       // Disable h/w filtering (accept all frames)

    // Init data
    init_can_dev_data();
}


/*
 * Open can
 *
 */
sint16_t ci_open(const uint8_t chan_idx, const uint8_t flags, const uint8_t bt0, const uint8_t bt1) {
    if (chan_idx >= CI_CHAN_NUMS)
        return -ECINODEV;

    if (__cans_dev[chan_idx].opened)
        return -ECIBUSY;

    CAN1MOD_bit.RM = 1;      // Enter Reset Mode
    CAN1MOD_bit.TPM = 0;     // Priority of the 3 TR Buffers depends on their CAN IDs

    CAN1IER = 0;             // Disable All Interrupts
    CAN1GSR = 0;             // Clear Status register

    __cans_dev[chan_idx].state = CAN_INIT;
    __cans_dev[chan_idx].wtout = CI_WRITE_TIMEOUT_DEF;
    __cans_dev[chan_idx].hovr = 0;
    __cans_dev[chan_idx].sovr = 0;
    __cans_dev[chan_idx].rxcnt = 0;
    __cans_dev[chan_idx].amask11 = 0;
    __cans_dev[chan_idx].acode11 = 0;
    __cans_dev[chan_idx].amask29 = 0;
    __cans_dev[chan_idx].acode29 = 0;
    __cans_dev[chan_idx].signals = 0;
    __cans_dev[chan_idx].flags = flags;

    // Set can id len (11 or 29 bits)
    if (!(__cans_dev[chan_idx].flags & CIO_CAN29) && !(__cans_dev[chan_idx].flags & CIO_CAN11)) {
        __cans_dev[chan_idx].flags |= CIO_CAN11;
    }

    // Init queue
    cansig_que_init(&__cans_dev[chan_idx].rcq);
    ci_sigs_init(&__cans_dev[chan_idx]);

    __cans_dev[chan_idx].cur_ewl = __cans_dev[chan_idx].cur_boff = __cans_dev[chan_idx].cur_hovr = 0;
    __cans_dev[chan_idx].opened = 1;

    ci_stop(chan_idx);
    ci_set_baud(chan_idx, bt0, bt1);

    return 0;
}


/*
 * Close can
 *
 */
sint16_t ci_close(const uint8_t chan_idx) {
    if (chan_idx >= CI_CHAN_NUMS)
        return -ECINODEV;

    if (!__cans_dev[chan_idx].opened)
        return -ECIINVAL;

    ci_stop(chan_idx);
    __cans_dev[chan_idx].opened = 0;

    return 0;
}


/*
 * Start can
 *
 */
sint16_t ci_start(const uint8_t chan_idx) {
    if (chan_idx >= CI_CHAN_NUMS)
        return -ECINODEV;

    if (!__cans_dev[chan_idx].opened)
        return -ECIINVAL;

    CAN1IER_bit.RIE = 1;
    CAN1IER_bit.EIE = 1;
    CAN1IER_bit.DOIE = 1;
    CAN1MOD_bit.RM = 0;

    __cans_dev[chan_idx].state = CAN_RUNNING;
    return 0;
}


/*
 * Stop can
 *
 */
sint16_t ci_stop(const uint8_t chan_idx) {
    if (chan_idx >= CI_CHAN_NUMS)
        return -ECINODEV;

    if (!__cans_dev[chan_idx].opened)
        return -ECIINVAL;

    CAN1IER = 0;
    CAN1MOD_bit.RM = 1;
    __cans_dev[chan_idx].state = CAN_INIT;
    return 0;
}


/*
 * Set can filter
 *
 */
sint16_t ci_set_filter(const uint8_t chan_idx, const uint32_t acode, const uint32_t amask) {
    if (chan_idx >= CI_CHAN_NUMS)
        return -ECINODEV;

    if (!__cans_dev[chan_idx].opened)
        return -ECIINVAL;

    if (__cans_dev[chan_idx].state != CAN_INIT)
        return -ECISTATE;

    if ((__cans_dev[chan_idx].flags & CIO_CAN11) && (__cans_dev[chan_idx].flags & CIO_CAN29)) {
        __cans_dev[chan_idx].amask11 = amask;
        __cans_dev[chan_idx].acode11 = acode;
        __cans_dev[chan_idx].amask29 = amask;
        __cans_dev[chan_idx].acode29 = acode;
    } else if (__cans_dev[chan_idx].flags & CIO_CAN11) {
        __cans_dev[chan_idx].amask11 = amask;
        __cans_dev[chan_idx].acode11 = acode;
        __cans_dev[chan_idx].amask29 = 0xFFFFFFFF;
        __cans_dev[chan_idx].acode29 = 0;
    } else if (__cans_dev[chan_idx].flags & CIO_CAN29) {
        __cans_dev[chan_idx].amask11 = 0xFFFFFFFF;
        __cans_dev[chan_idx].acode11 = 0;
        __cans_dev[chan_idx].amask29 = amask;
        __cans_dev[chan_idx].acode29 = acode;
    }

    return 0;
}



/*
 * Set can baud
 *
 * CAN timings. Refer to Configuration of the CAN Bit Timing
 * http://www.port.de/pdf/CAN_Bit_Timing.pdf
 * Parameters are in Philips SJA1000 controller format (BTR0 and BTR1
 * registers). The parameter values are for 8 MHz VPB clock;
 */
sint16_t ci_set_baud(const uint8_t chan_idx, const uint8_t bt0, const uint8_t bt1) {
    if (chan_idx >= CI_CHAN_NUMS)
        return -ECINODEV;

    if (!__cans_dev[chan_idx].opened)
        return -ECIINVAL;

    if (__cans_dev[chan_idx].state != CAN_INIT)
        return -ECISTATE;

    uint8_t brp, sjw, sam, tseg2, tseg1;  // CAN timing parameters
    uint32_t reg = 0;

    brp = bt0 & 0x3f;
    sjw = (bt0 & 0xc0) >> 6;
    sam = (bt1 & 0x80) >> 7;
    tseg1 = (bt1 & 0xf);
    tseg2 = (bt1 & 0x70) >> 4;
    reg = brp;
    reg |= (sjw << 14);
    reg |= (tseg1 << 16);
    reg |= (tseg2 << 20);
    reg |= (sam << 23);

    CAN1BTR = reg;
    __cans_dev[chan_idx].bt0 = bt0;
    __cans_dev[chan_idx].bt1 = bt1;

    return 0;
}


/*
 * Read can
 *
 */
sint16_t ci_read(const uint8_t chan_idx, __canmsg_t * mbuf, const sint16_t cnt) {
    if (chan_idx >= CI_CHAN_NUMS)
        return -ECINODEV;

    if (!__cans_dev[chan_idx].opened)
        return -ECIINVAL;

    if (__cans_dev[chan_idx].state != CAN_RUNNING)
        return -ECISTATE;

    if (__ci_context == CI_CONTEXT_PROCESS) {
        __disable_can_int();
    }

    sint16_t ret = cansig_que_retr(&__cans_dev[chan_idx].rcq, mbuf, cnt);
    if (__ci_context == CI_CONTEXT_PROCESS) {
        __enable_can_int();
    }

    return ret;
}


/*
 * Set callback for can
 *
 */
sint16_t ci_set_cb(const uint8_t chan_idx, const uint8_t event, void (*ci_handler) (sint16_t)) {
    if (chan_idx >= CI_CHAN_NUMS)
        return -ECINODEV;

    if (event != CIEV_RC && event != CIEV_TR && event != CIEV_CANERR)
        return -ECIINVAL;

    if (!__cans_dev[chan_idx].opened)
        return -ECIINVAL;

    if (__cans_dev[chan_idx].state != CAN_INIT)
        return -ECISTATE;

    sint16_t ret = 0;
    switch (event) {
        case CIEV_RC:
          __cans_dev[chan_idx].signals &= ~CAN_SIG_RX_ENABLE;
          ci_signal(&__cans_dev[chan_idx], CIEV_RC, (void (*) (const uint8_t)) ci_handler);

          if (ci_handler) {
              __cans_dev[chan_idx].signals |= CAN_SIG_RX_ENABLE;
          } else {
              __cans_dev[chan_idx].signals &= ~CAN_SIG_RX_ENABLE;
          }
          break;

        case CIEV_CANERR:
          __cans_dev[chan_idx].signals &= ~CAN_SIG_ERR_ENABLE;
          ci_signal(&__cans_dev[chan_idx], CIEV_BOFF, (void (*) (const uint8_t)) ci_handler);
          ci_signal(&__cans_dev[chan_idx], CIEV_EWL, (void (*) (const uint8_t)) ci_handler);
          ci_signal(&__cans_dev[chan_idx], CIEV_WTOUT, (void (*) (const uint8_t)) ci_handler);
          ci_signal(&__cans_dev[chan_idx], CIEV_SOVR, (void (*) (const uint8_t)) ci_handler);
          ci_signal(&__cans_dev[chan_idx], CIEV_HOVR, (void (*) (const uint8_t)) ci_handler);

          if (ci_handler) {
              __cans_dev[chan_idx].signals |= CAN_SIG_ERR_ENABLE;
          } else {
              __cans_dev[chan_idx].signals &= ~CAN_SIG_ERR_ENABLE;
          }
          break;

        default:
          ret = -ECIINVAL;
        break;
    }

    return ret;
}


/*
 * Write can
 *
 */
sint16_t ci_write(const uint8_t chan_idx, __canmsg_t * mbuf, const sint16_t cnt) {
    if (chan_idx >= CI_CHAN_NUMS)
        return -ECINODEV;

    if (!__cans_dev[chan_idx].opened)
        return -ECIINVAL;

    if (__cans_dev[chan_idx].state != CAN_RUNNING)
        return -ECISTATE;

    if (!CAN1GSR_bit.TBS)
        return -ECITOUT;

    if (msg_iseff(mbuf)) {
        if (!(__cans_dev[chan_idx].flags & CIO_CAN29)) {
            return -ECIIO;
        }

        CAN1TFI1_bit.DLC = mbuf->len & 0x0F;
        CAN1TFI1_bit.FF = 1;
        CAN1TID1_bit.ID29_0 = mbuf->id;
    } else {
        if (!(__cans_dev[chan_idx].flags & CIO_CAN11)) {
            return -ECIIO;
        }

        CAN1TFI1_bit.DLC = mbuf->len & 0x0F;
        CAN1TFI1_bit.FF = 0;
        CAN1TID1_bit.ID10_0 = mbuf->id;
    }

    if (msg_isrtr(mbuf)) {
        CAN1TFI1_bit.RTR = 1;
    } else {
        CAN1TFI1_bit.RTR = 0;
        CAN1TDA1_bit.DATA1 = mbuf->data[0];
        CAN1TDA1_bit.DATA2 = mbuf->data[1];
        CAN1TDA1_bit.DATA3 = mbuf->data[2];
        CAN1TDA1_bit.DATA4 = mbuf->data[3];
        CAN1TDB1_bit.DATA5 = mbuf->data[4];
        CAN1TDB1_bit.DATA6 = mbuf->data[5];
        CAN1TDB1_bit.DATA7 = mbuf->data[6];
        CAN1TDB1_bit.DATA8 = mbuf->data[7];
    }

    CAN1CMR_bit.STB1 = 1;    // command send tx buff1
    CAN1CMR_bit.TR = 1;

    if (__cans_dev[chan_idx].wtout == 0)
        return 1;

    if (CAN1GSR_bit.TCS)
        return 1;            // success

    sleep_10mks();

    if (CAN1GSR_bit.TCS)
        return 1;            // success

    // wait transmition
    uint32_t waittimes = __cans_dev[chan_idx].wtout * 10; // in 100 mks fractions
    while (waittimes) {
        if (CAN1GSR_bit.TCS)
            return 1;        //success

        waittimes--;
        sleep_100mks();
    }

    CAN1CMR_bit.AT = 1;      // abort transmission
    return -ECITOUT;
}


/*
 * Can hw reset
 *
 */
sint16_t ci_hw_reset(const uint8_t chan_idx) {
    if (chan_idx >= CI_CHAN_NUMS)
        return -ECINODEV;

    if (!__cans_dev[chan_idx].opened)
        return -ECIINVAL;

    // Abort pending transmition
    CAN1CMR_bit.AT = 1;

    // Clear error counters
    CAN1MOD_bit.RM = 1;
    __cans_dev[chan_idx].hovr = 0;
    __cans_dev[chan_idx].sovr = 0;
    CAN1MOD_bit.RM = 0;

    return 0;
}


/*
 * Can set write timeout
 *
 */
sint16_t ci_set_write_tout(const uint8_t chan_idx, const uint16_t msec) {
    if (chan_idx >= CI_CHAN_NUMS)
        return -ECINODEV;

    if (!__cans_dev[chan_idx].opened)
        return -ECIINVAL;

    if (__cans_dev[chan_idx].state != CAN_INIT)
        return -ECISTATE;

    __cans_dev[chan_idx].wtout = msec;
    return 0;
}


/*
 * Can set lom
 *
 */
sint16_t ci_set_lom(const uint8_t chan_idx, const uint8_t mode) {
    if (chan_idx >= CI_CHAN_NUMS)
        return -ECINODEV;

    if (!__cans_dev[chan_idx].opened)
        return -ECIINVAL;

    if (__cans_dev[chan_idx].state != CAN_INIT)
        return -ECISTATE;

    if (mode)
        CAN1MOD_bit.LOM = 1;
    else
        CAN1MOD_bit.LOM = 0;

    return 0;
}


/*
 * Poll new signals
 *
 */
void ci_poll_sigs() {
    for (uint16_t chan_idx = 0; chan_idx < CI_CHAN_NUMS; chan_idx++) {
        if (__cans_dev[chan_idx].opened == 0)
            continue;

        for (uint16_t i = 0; i < __cans_dev[chan_idx].sslot_size; i++) {
            if (__cans_dev[chan_idx].sslot[i].hdl) {
                while (__cans_dev[chan_idx].sslot[i].sigcnt > 0) {
                    __disable_can_int();
                    __cans_dev[chan_idx].sslot[i].sigcnt--;;
                    __enable_can_int();
                    __cans_dev[chan_idx].sslot[i].hdl(i);
                }
            }
        }
    }
}


/*
 * Can interrupt func
 *
 */
static __arm void __can_interrupt(const uint8_t chan_idx, const uint32_t canicr_val) {
    uint32_t id;
    uint16_t flags = 0;   /* bit 0 - RTR, 2 - EFF */
    __canmsg_t * frame;
    __canque_t * q;

    if (chan_idx >= CI_CHAN_NUMS)
        return;

    if (CAN1GSR_bit.RBS) {   //RX
        if (CAN1RFS_bit.FF) { // EFF 29 bits
            if (!(__cans_dev[chan_idx].flags & CIO_CAN29) ) {
                CAN1CMR_bit.RRB = 1;
                goto nextirq;
            }

            id = CAN1RID_bit.ID29_0;
            if ((id & __cans_dev[chan_idx].amask29) != (__cans_dev[chan_idx].acode29 & __cans_dev[chan_idx].amask29)) {
                // not passed sw filtering
                CAN1CMR_bit.RRB = 1;
                goto nextirq;
            }
            flags |= FRAME_EFF;
        } else { // 11 bits
            if (!(__cans_dev[chan_idx].flags & CIO_CAN11)) {
                CAN1CMR_bit.RRB = 1;
                goto nextirq;
            }

            id = CAN1RID_bit.ID10_0;
            if ((id & __cans_dev[chan_idx].amask11) != (__cans_dev[chan_idx].acode11 & __cans_dev[chan_idx].amask11)) {
                // not passed sw filtering
                CAN1CMR_bit.RRB = 1;
                goto nextirq;
            }
        }

        q = &__cans_dev[chan_idx].rcq;
        if (q->count == q->size) {
            ci_send_signal(&__cans_dev[chan_idx], CIEV_SOVR);  // queue is full
            CAN1CMR_bit.RRB = 1;
            goto nextirq;
        }

        frame = &q->d[q->sloc];
        frame->id = id;
        frame->flags = flags;
        frame->len = CAN1RFS_bit.DLC;

        if (CAN1RFS_bit.RTR) { // RTR frame
            frame->flags |= FRAME_RTR;
        } else {               // Data frame
            frame->data[0] = CAN1RDA_bit.DATA1;
            frame->data[1] = CAN1RDA_bit.DATA2;
            frame->data[2] = CAN1RDA_bit.DATA3;
            frame->data[3] = CAN1RDA_bit.DATA4;
            frame->data[4] = CAN1RDB_bit.DATA5;
            frame->data[5] = CAN1RDB_bit.DATA6;
            frame->data[6] = CAN1RDB_bit.DATA7;
            frame->data[7] = CAN1RDB_bit.DATA8;
        }

        CAN1CMR_bit.RRB = 1; // Release rcv buffer

        q->sloc++;
        if (q->sloc >= q->size)
            q->sloc=0;

        q->count++;
        ci_send_signal(&__cans_dev[chan_idx], CIEV_RC);
        __cans_dev[chan_idx].rxcnt++;
    }

nextirq:

    // check hw data overrun
    if (CAN1GSR_bit.DOS) {
        CAN1CMR_bit.CDO = 1;
        __cans_dev[chan_idx].hovr++;
        __cans_dev[chan_idx].cur_hovr++;
        ci_send_signal(&__cans_dev[chan_idx], CIEV_HOVR);
    }

    // CAN errors (except data overrun)
    if (canicr_val & 0x4) {    // Handle errors here
        if (CAN1GSR_bit.ES) {  // Error warning
            __cans_dev[chan_idx].cur_ewl++;
            ci_send_signal(&__cans_dev[chan_idx], CIEV_EWL);
        }

        if (CAN1GSR_bit.BS) { // Bus off
            __cans_dev[chan_idx].cur_boff++;
            ci_send_signal(&__cans_dev[chan_idx], CIEV_BOFF);
        }
    }
}


/*
 * Int handler for CAN1-2
 *
 */
__irq __arm void __interrupt_handler_can12() {
    uint32_t canicr_val;
    canicr_val = CAN1ICR;   // clear all interrupt flags here
                            // (except data overrun and rcv)

    __ci_context = CI_CONTEXT_IRQ;
    if (canicr_val & 0x0D) {
        __can_interrupt(0, canicr_val);
    }
    __ci_context = CI_CONTEXT_PROCESS;

    VICADDRESS = 0;
}
