/*
 * MCP2515_REG.h
 *
 *  Created on: Aug 4, 2026
 *      Author: MadaYaswanth
 */

#ifndef INC_MCP2515_REG_H_
#define INC_MCP2515_REG_H_

/* ========================================================================= */
/*                          REGISTER ADDRESS MAP                             */
/* ========================================================================= */

/* Configuration & Control Registers */
#define MCP_REG_CANCTRL         0x0F
#define MCP_REG_CANSTAT         0x0E
#define MCP_REG_CNF1            0x2A
#define MCP_REG_CNF2            0x29
#define MCP_REG_CNF3            0x28
#define MCP_REG_TEC             0x1C
#define MCP_REG_REC             0x1D
#define MCP_REG_EFLG            0x2D
#define MCP_REG_CANINTE         0x2B
#define MCP_REG_CANINTF         0x2C

/* Transmit Buffer Base Registers */
#define MCP_REG_TXB0CTRL        0x30
#define MCP_REG_TXB0SIDH        0x31
#define MCP_REG_TXB1CTRL        0x40
#define MCP_REG_TXB1SIDH        0x41
#define MCP_REG_TXB2CTRL        0x50
#define MCP_REG_TXB2SIDH        0x51

/* Receive Buffer Base Registers */
#define MCP_REG_RXB0CTRL        0x60
#define MCP_REG_RXB0SIDH        0x61
#define MCP_REG_RXB1CTRL        0x70
#define MCP_REG_RXB1SIDH        0x71

/* Filter Register Base Addresses */
#define MCP_REG_RXF0SIDH        0x00
#define MCP_REG_RXF1SIDH        0x04
#define MCP_REG_RXF2SIDH        0x08
#define MCP_REG_RXF3SIDH        0x10
#define MCP_REG_RXF4SIDH        0x14
#define MCP_REG_RXF5SIDH        0x18

/* Mask Register Base Addresses */
#define MCP_REG_RXM0SIDH        0x20
#define MCP_REG_RXM1SIDH        0x24

/* Buffer Register Stride Offsets */
#define MCP_OFFSET_SIDH         0
#define MCP_OFFSET_SIDL         1
#define MCP_OFFSET_EID8         2
#define MCP_OFFSET_EID0         3
#define MCP_OFFSET_DLC          4
#define MCP_OFFSET_DATA         5

/* ========================================================================= */
/*                          SPI INSTRUCTION OPCODES                         */
/* ========================================================================= */

#define MCP_CMD_RESET           0xC0
#define MCP_CMD_READ            0x03
#define MCP_CMD_WRITE           0x02
#define MCP_CMD_RTS             0x80
#define MCP_CMD_READ_STATUS     0xA0
#define MCP_CMD_BITMOD          0x05
#define MCP_CMD_READ_RX_BUF0    0x90
#define MCP_CMD_READ_RX_BUF1    0x94

/* ========================================================================= */
/*                          REGISTER BIT DEFINITIONS                         */
/* ========================================================================= */

#define MCP_MASK_MODE           0xE0
#define MCP_BIT_OSM             0x08

/* Interrupt Flags & Enables (CANINTF / CANINTE) */
#define MCP_BIT_RX0IF           0x01
#define MCP_BIT_RX1IF           0x02
#define MCP_BIT_TX0IF           0x04
#define MCP_BIT_TX1IF           0x08
#define MCP_BIT_TX2IF           0x10
#define MCP_BIT_ERRIF           0x20
#define MCP_BIT_WAKIF           0x40
#define MCP_MASK_RXIF           (MCP_BIT_RX0IF | MCP_BIT_RX1IF)

#define MCP_BIT_TXREQ           0x08
#define MCP_MASK_TXPRI          0x03

/* Receive Control (RXBnCTRL) */
#define MCP_MASK_RXB0_RXM       0x60
#define MCP_BIT_RX0_BUKT        0x04
#define MCP_MASK_RXB1_RXM       0x60

/* ID Register Bit Flags */
#define MCP_BIT_EXIDE           0x08

/* DLC Register Flags */
#define MCP_MASK_DLC            0x0F
#define MCP_BIT_RTR             0x40

/* SPI Transfer Helper */
#define MCP_SPI_DUMMY           0xFF

#endif /* INC_MCP2515_REG_H_ */
