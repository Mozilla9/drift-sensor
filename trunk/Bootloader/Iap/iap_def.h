/*
 * $Id$
 */


#ifndef __IAP_DEF_H
#define __IAP_DEF_H


/**************************** Constants for IAP *******************************/
#define IAP_ADR_THUMB             0x7FFFFFF0      // Entry point in sys bl IAP Thumb
#define IAP_ADR_ARM               0x7FFFFFF1      // Entry point in sys bl IAP ARM

#define IAP_PREPARE_SECT          50
#define IAP_COPY_RAM_TO_FLASH     51
#define IAP_ERASE_SECT            52
#define IAP_BLANK_CHECK_SECT      53
#define IAP_READ_PART_ID          54
#define IAP_READ_BOOT_VERS        55
#define IAP_COMPARE               56

#define IAP_CMD_SUCCESS           0
#define IAP_INVALID_COMMAND       1
#define IAP_SRC_ADDR_ERROR        2
#define IAP_DST_ADDR_ERROR        3
#define IAP_SRC_ADDR_NOT_MAPPED   4
#define IAP_DST_ADDR_NOT_MAPPED   5
#define IAP_COUNT_ERROR           6
#define IAP_INVALID_SECTOR        7
#define IAP_SECTOR_NOT_BLANK      8
#define IAP_SECTOR_NOT_PREPARED   9
#define IAP_COMPARE_ERROR         10
#define IAP_BUSY                  11

#define CRP_ADR                   0x000001FC      // CRP addr
#define CRP1                      0x12345678
#define CRP2                      0x87654321
#define CRP3                      0x43218765

#define SECTOR0                   0x00000000
#define SECTOR1                   0x00001000
#define SECTOR2                   0x00002000
#define SECTOR3                   0x00003000
#define SECTOR4                   0x00004000
#define SECTOR5                   0x00005000
#define SECTOR6                   0x00006000
#define SECTOR7                   0x00007000
#define SECTOR8                   0x00008000
#define SECTOR9                   0x00010000
#define SECTOR10                  0x00018000
#define SECTOR11                  0x00020000
#define SECTOR12                  0x00028000
#define SECTOR13                  0x00030000
#define SECTOR14                  0x00038000
#define SECTOR15                  0x00040000
#define SECTOR16                  0x00048000
#define SECTOR17                  0x00050000
#define SECTOR18                  0x00058000
#define SECTOR19                  0x00060000
#define SECTOR20                  0x00068000
#define SECTOR21                  0x00070000
#define SECTOR22                  0x00078000
#define SECTOR23                  0x00079000
#define SECTOR24                  0x0007A000
#define SECTOR25                  0x0007B000
#define SECTOR26                  0x0007C000
#define SECTOR27                  0x0007D000

#define SECTOR0_SIZE              4
#define SECTOR1_SIZE              4
#define SECTOR2_SIZE              4
#define SECTOR3_SIZE              4
#define SECTOR4_SIZE              4
#define SECTOR5_SIZE              4
#define SECTOR6_SIZE              4
#define SECTOR7_SIZE              4
#define SECTOR8_SIZE              32
#define SECTOR9_SIZE              32
#define SECTOR10_SIZE             32
#define SECTOR11_SIZE             32
#define SECTOR12_SIZE             32
#define SECTOR13_SIZE             32
#define SECTOR14_SIZE             32
#define SECTOR15_SIZE             32
#define SECTOR16_SIZE             32
#define SECTOR17_SIZE             32
#define SECTOR18_SIZE             32
#define SECTOR19_SIZE             32
#define SECTOR20_SIZE             32
#define SECTOR21_SIZE             32
#define SECTOR22_SIZE             4
#define SECTOR23_SIZE             4
#define SECTOR24_SIZE             4
#define SECTOR25_SIZE             4
#define SECTOR26_SIZE             4
#define SECTOR27_SIZE             4




#endif