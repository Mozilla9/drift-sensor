/*
 * $Id$
 */

#ifndef __CAN_SIG_H
#define __CAN_SIG_H


#define CAN_SIG_TX_ENABLE        0x1
#define CAN_SIG_RX_ENABLE        0x2
#define CAN_SIG_ERR_ENABLE       0x4


typedef struct {
    uint32_t id;
    uint32_t ts;
    uint16_t flags;                    // bit 0 - RTR, 2 - EFF
    uint8_t  data[8];
    uint8_t  len;
} __canmsg_t;


typedef struct {
    __canmsg_t * d;                    // data - array of can_msg_t type values
    uint16_t size;                     // size of data array
    uint16_t sloc;                     // next store element adress (index of array d)
    uint16_t rloc;                     // next retrieve element adress (index of array d)
    uint16_t count;                    // current number of elemnts in data array
} __canque_t;


typedef struct {
    uint16_t sigcnt;
    void (* hdl) (const uint8_t);
} __ci_sigslot_t;


typedef struct {
    uint8_t  opened;
    uint8_t  flags;
    uint8_t  state;
    uint16_t wtout;
    uint8_t  signals;                   // 0x1 - CAN_SIG_TX enable
                                        // 0x2 - CAN_SIG_RX enable
                                        // 0x4 - CAN_SIG_ERR enable: BOFF, HOVR, SOVR, EWL, WTOUT
    uint32_t tsec;
    uint32_t rxcnt;
    uint32_t sovr;
    uint32_t hovr;
    uint32_t amask11;
    uint32_t amask29;
    uint32_t acode11;
    uint32_t acode29;
    uint8_t  bt0;
    uint8_t  bt1;

    uint32_t cur_ewl;
    uint32_t cur_boff;
    uint32_t cur_hovr;
    __canque_t rcq;                     // rcv queue (fifo)
    __ci_sigslot_t * sslot;
    uint16_t sslot_size;
} __ci_can_dev_t;



#endif