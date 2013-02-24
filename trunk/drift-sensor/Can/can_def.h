/*
 * $Id$
 */

#ifndef __CAN_DEF_H
#define __CAN_DEF_H


#define CI_WRITE_TIMEOUT_DEF     10
#define CI_WRITE_TIMEOUT_MAX     300

#define CAN_INIT                 0
#define CAN_RUNNING              1

// Error codes
#define ECIOK                    0            /* success */
#define ECIGEN                   1            /* generic (not specified) error */
#define ECIBUSY                  2            /* device or resourse busy */
#define ECIMFAULT                3            /* memory fault */
#define ECISTATE                 4            /* function can't be called for chip in current state */
#define ECIINCALL                5            /* invalid call, function can't be called for this object */
#define ECIINVAL                 6            /* invalid parameter */
#define ECIACCES                 7            /* can not access resource */
#define ECINOSYS                 8            /* function or feature not implemented */
#define ECIIO                    9            /* input/output error */
#define ECINODEV                 10           /* no such device or object */
#define ECIINTR                  11           /* call was interrupted by event */
#define ECINORES                 12           /* no resources */
#define ECITOUT                  13           /* time out occured */

// Possible events
#define CIEV_RC                  1
#define CIEV_TR                  2
#define CIEV_CANERR              6

#define CIEV_EWL                 3
#define CIEV_BOFF                4
#define CIEV_HOVR                5
#define CIEV_WTOUT               7
#define CIEV_SOVR                8

// Possible flags for ci_open
#define CIO_BLOCK                0x1          // ignored, block mode was removed in CHAI 2.x
#define CIO_CAN11                0x2
#define CIO_CAN29                0x4

//Possible commands for ci_set_lom
#define CI_LOM_OFF               0x0
#define CI_LOM_ON                0x1

// Possible bit numbers in canmsg_t.flags field
#define MSG_RTR                  0
#define MSG_FF                   2           // if is set extended frame format is used

#define FRAME_RTR                0x1
#define FRAME_EFF                0x4



#endif