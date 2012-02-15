/****************************************************************************
 * 	Copyright (C) SEIKO EPSON CORP. 2010
 *
 * 	File Name   : uart_reg.h
 * 	Description : S1C33L27 UART register definition file
 *
 * 	Revision History:
 *  2010/01/06   Jackie Zhu   New creation
 ****************************************************************************/

#ifndef _UART_REG_H
#define _UART_REG_H

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************/
/* [ UART Status Register ]  UART_ST                           0x00 */
/********************************************************************/
    union UART_ST_tag {
        volatile struct {
            unsigned int TDBE:1;          // Transmit data buffer empty flag
            unsigned int RDRY:1;          // Receive data ready flag
            unsigned int TRBS:1;          // Transmit busy flag
            unsigned int RD2B:1;          // Second byte receive flag
            unsigned int OER:1;           // Overrun error flag
            unsigned int PER:1;           // Parity error flag
            unsigned int FER:1;           // Framing error flag
        } bCTL;
        volatile unsigned char ucCTL;
    };

/********************************************************************/
/* [ UART Mode Register ]  UART_MOD                           0x03 */
/********************************************************************/
    union UART_MOD_tag {
        volatile struct {
            unsigned int SSCK:1;                // Input clock select
            unsigned int STPB:1;                // Stop bit select
            unsigned int PMD:1;                 // Parity mode select
            unsigned int PREN:1;                // Parity enable
            unsigned int CHLN:1;                // Character length select
        } bCTL;
        volatile unsigned char ucCTL;
    };

/********************************************************************/
/* [ UART Control Register ]  UART_CTL                         0x04 */
/********************************************************************/
    union UART_CTL_tag {
        volatile struct {
            unsigned int RXEN:1;                // UART enable
            unsigned int RBFI:1;                // Receive buffer full int. cndition setup
            unsigned int DUMMY:2;
            unsigned int TIEN:1;                // Transmit buffer empty int. enable
            unsigned int RIEN:1;                // Receive buffer full int. enable
            unsigned int REIEN:1;               // Receive error int. enable
        } bCTL;
        volatile unsigned char ucCTL;
    };

/********************************************************************/
/* [ UART Expansion Register ]  UART_EXP                       0x05 */
/********************************************************************/
    union UART_EXP_tag {
        volatile struct {
            unsigned int IRMD:1;                // IrDA mode select
            unsigned int DUMMY:3;
            unsigned int IRCLK:3;               // IrDA receive detection clock select
        } bCTL;
        volatile unsigned char ucCTL;
    };
/****************************************************************************
 * 	Macros (#define)
 ****************************************************************************/
#define UART_BASE               (0x00300e10)

#define UART_ST                 (*(union UART_ST_tag *)(UART_BASE + 0x00)).ucCTL
#define UART_TDBE               (*(union UART_ST_tag *)(UART_BASE + 0x00)).bCTL.TDBE
#define UART_RDRY               (*(union UART_ST_tag *)(UART_BASE + 0x00)).bCTL.RDRY
#define UART_TRBS               (*(union UART_ST_tag *)(UART_BASE + 0x00)).bCTL.TRBS
#define UART_RD2B               (*(union UART_ST_tag *)(UART_BASE + 0x00)).bCTL.RD2B
#define UART_OER                (*(union UART_ST_tag *)(UART_BASE + 0x00)).bCTL.OER
#define UART_PER                (*(union UART_ST_tag *)(UART_BASE + 0x00)).bCTL.PER
#define UART_FER                (*(union UART_ST_tag *)(UART_BASE + 0x00)).bCTL.FER

#define UART_TXD                (*(volatile unsigned char *)(UART_BASE+0x01))
#define UART_RXD                (*(const volatile unsigned char *)(UART_BASE+0x02))

#define UART_MOD                (*(union UART_MOD_tag *)(UART_BASE + 0x03)).ucCTL
#define UART_SSCK               (*(union UART_MOD_tag *)(UART_BASE + 0x03)).bCTL.SSCK
#define UART_STPB               (*(union UART_MOD_tag *)(UART_BASE + 0x03)).bCTL.STPB
#define UART_PMD                (*(union UART_MOD_tag *)(UART_BASE + 0x03)).bCTL.PMD
#define UART_PREN               (*(union UART_MOD_tag *)(UART_BASE + 0x03)).bCTL.PREN
#define UART_CHLN               (*(union UART_MOD_tag *)(UART_BASE + 0x03)).bCTL.CHLN

#define UART_CTL                (*(union UART_CTL_tag *)(UART_BASE + 0x04)).ucCTL
#define UART_RXEN               (*(union UART_CTL_tag *)(UART_BASE + 0x04)).bCTL.RXEN
#define UART_RBFI               (*(union UART_CTL_tag *)(UART_BASE + 0x04)).bCTL.RBFI
#define UART_TIEN               (*(union UART_CTL_tag *)(UART_BASE + 0x04)).bCTL.TIEN
#define UART_RIEN               (*(union UART_CTL_tag *)(UART_BASE + 0x04)).bCTL.RIEN
#define UART_REIEN              (*(union UART_CTL_tag *)(UART_BASE + 0x04)).bCTL.REIEN

#define UART_EXP                (*(union UART_EXP_tag *)(UART_BASE + 0x05)).ucCTL
#define UART_IRMD               (*(union UART_EXP_tag *)(UART_BASE + 0x05)).bCTL.IRMD
#define UART_IRCLK              (*(union UART_EXP_tag *)(UART_BASE + 0x05)).bCTL.IRCLK


#ifdef __cplusplus
}
#endif
#endif                          /* _UART_REG_H */

