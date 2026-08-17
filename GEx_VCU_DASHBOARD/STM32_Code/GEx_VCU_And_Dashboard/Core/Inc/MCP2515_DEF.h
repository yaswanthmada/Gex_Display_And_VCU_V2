/*
 * MCP2515_DEF.h
 *
 *  Created on: Aug 4, 2026
 *      Author: MadaYaswanth
 */

#ifndef INC_MCP2515_DEF_H_
#define INC_MCP2515_DEF_H_

/* --- CAN Message Frame Structure --- */
typedef struct {
    uint32_t id;          /* 11-bit Standard or 29-bit Extended ID */
    uint8_t  dlc;         /* Data Length Code (0 to 8 bytes) */
    uint8_t  data[8];     /* Payload Bytes */
    bool     isExtended;  /* true = Extended ID (29-bit), false = Standard ID (11-bit) */
    bool     isRTR;       /* Remote Transmission Request flag */
} CAN_Message_t;
typedef enum
{
    MCP_MODE_NORMAL   = 0x00,
    MCP_MODE_SLEEP    = 0x20,
    MCP_MODE_LOOPBACK = 0x40,
    MCP_MODE_LISTEN   = 0x60,
    MCP_MODE_CONFIG   = 0x80
} MCP2515_Mode_t;

typedef enum
{
    CAN_SPEED_125KBPS = 0,
    CAN_SPEED_250KBPS,
    CAN_SPEED_500KBPS,
    CAN_SPEED_1000KBPS
} MCP2515_BaudRate_t;
typedef enum
{
    MCP_CLOCK_8MHZ = 0,
    MCP_CLOCK_16MHZ,
    MCP_CLOCK_20MHZ
} MCP2515_Clock_t;
typedef enum
{
    MCP_TXB0 = 0x01,
    MCP_TXB1 = 0x02,
    MCP_TXB2 = 0x04,
    MCP_TXB_ALL = 0x07
} MCP2515_TxBuffer_t;

typedef enum
{
    MCP_FILTER_RXF0 = 0,
    MCP_FILTER_RXF1,
    MCP_FILTER_RXF2,
    MCP_FILTER_RXF3,
    MCP_FILTER_RXF4,
    MCP_FILTER_RXF5
} MCP2515_Filter_t;
typedef enum
{
    MCP_MASK_RXM0 = 0,
    MCP_MASK_RXM1
} MCP2515_Mask_t;

#endif
/* INC_MCP2515_DEF_H_ */
