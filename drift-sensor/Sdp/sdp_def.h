/*
 * $Id$
 */

#ifndef __SDP_DEF_H
#define __SDP_DEF_H



/*
 * Packet = |header=07 0E| + |data len (cmd type incl)=max 30 bytes| +
 * |cmd type + data bytes| + |CRC8=summ data bytes|
 *
*/

/*********************************** Constants SDP ****************************/
#define SDP_UDATA_MAX_LEN    30                            // max data len 30 bytes (CMD_TYPE + DATA)
#define SDP_PDATA_MAX_LEN    (2 * SDP_UDATA_MAX_LEN)       // max data len packed (in hex)
#define SDP_BUFF_LEN         64

#define SDP_HEADER_x07       0x07                          // first header byte
#define SDP_HEADER_x0E       0x0E                          // second header byte

#define SDP_ACK_x06          0x06                          // ack
#define SDP_NACK_x07         0x07                          // negative ack

#define SDP_CMD_C            0x43                          // erase program-flash
#define SDP_CMD_A            0x41                          // erase sett-flash
#define SDP_CMD_W            0x57                          // write to program-flash
#define SDP_CMD_E            0x45                          // write to sett-flash
#define SDP_CMD_U            0x55                          // invoke code
#define SDP_CMD_Z            0x5A                          // invoke user-cmd

#define SDP_STATE_WAIT_HDR   0
#define SDP_STATE_WAIT_LEN   1
#define SDP_STATE_DATA_RCV   2


#endif