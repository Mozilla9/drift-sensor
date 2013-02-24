/*
 * $Id$
 */

#ifndef __CAN_BAUD_TABLE_H
#define __CAN_BAUD_TABLE_H

// Table for PCLK = 8MHZ
/*#define BCI_1M_BT0               0x00
#define BCI_1M_BT1               0x14

#define BCI_800K_BT0             0x00
#define BCI_800K_BT1             0x16

#define BCI_500K_BT0             0x00
#define BCI_500K_BT1             0x1c

#define BCI_250K_BT0             0x01
#define BCI_250K_BT1             0x1c

#define BCI_125K_BT0             0x03
#define BCI_125K_BT1             0x1c

#define BCI_50K_BT0              0x09
#define BCI_50K_BT1              0x1c
*/

// Table for PCLK = 24MHZ
#define BCI_1M_BT0               0x02
#define BCI_1M_BT1               0x05

#define BCI_800K_BT0             0x02
#define BCI_800K_BT1             0x07

#define BCI_500K_BT0             0x02
#define BCI_500K_BT1             0x1c

#define BCI_250K_BT0             0x05
#define BCI_250K_BT1             0x1c

#define BCI_125K_BT0             0x0b
#define BCI_125K_BT1             0x1c

#define BCI_100K_BT0             0x0e
#define BCI_100K_BT1             0x1c

#define BCI_50K_BT0              0x1d
#define BCI_50K_BT1              0x1c



//
#define BCI_1M                   BCI_1M_BT0,   BCI_1M_BT1
#define BCI_800K                 BCI_800K_BT0, BCI_800K_BT1
#define BCI_500K                 BCI_500K_BT0, BCI_500K_BT1
#define BCI_250K                 BCI_250K_BT0, BCI_250K_BT1
#define BCI_125K                 BCI_125K_BT0, BCI_125K_BT1
#define BCI_100K                 BCI_100K_BT0, BCI_100K_BT1
#define BCI_50K                  BCI_50K_BT0,  BCI_50K_BT1


#endif