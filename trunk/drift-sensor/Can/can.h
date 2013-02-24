/*
 * $Id$
 */

#ifndef __CAN_H
#define __CAN_H

#include "Can\can_sig.h"
#include "Can\can_def.h"


void init_can();
void ci_poll_sigs();
sint16_t ci_open(const uint8_t chan_idx, const uint8_t flags, const uint8_t bt0, const uint8_t bt1);
sint16_t ci_close(const uint8_t chan_idx);
sint16_t ci_start(const uint8_t chan_idx);
sint16_t ci_stop(const uint8_t chan_idx);
sint16_t ci_set_filter(const uint8_t chan_idx, const uint32_t acode, const uint32_t amask);
sint16_t ci_set_baud(const uint8_t chan_idx, const uint8_t bt0, const uint8_t bt1);
sint16_t ci_write(const uint8_t chan_idx, __canmsg_t * mbuf, const sint16_t cnt);
sint16_t ci_read(const uint8_t chan_idx, __canmsg_t * mbuf, const sint16_t cnt);
sint16_t ci_set_cb(const uint8_t chan_idx, const uint8_t event, void (* ci_handler) (sint16_t));
sint16_t ci_hw_reset(const uint8_t chan_idx);
sint16_t ci_set_lom(const uint8_t chan_idx, const uint8_t mode);
__irq __arm void __interrupt_handler_can12();



#endif