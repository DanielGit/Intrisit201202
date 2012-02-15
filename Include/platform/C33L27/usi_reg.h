/****************************************************************************
 * 	Copyright (C) SEIKO EPSON CORP. 2010
 *
 * 	File Name   : usi_reg.h
 * 	Description : S1C33L27 USI register definition file
 *
 * 	Revision History:
 *  2010/01/12   Joan Lin     New creation
 *  2010/01/27   Jackie Zhu   Modify according L27 manual
 *                            Shift USIL to usil_reg.h file
 ****************************************************************************/
#ifndef _USI_REG_H
#define _USI_REG_H

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************/
/* [ Global Configure Register ]  USI_GCFG                  0x00    */
/********************************************************************/
    union USI_GCFG_tag {
        volatile struct {
            unsigned int USIMOD:3;  // USI Mode Configure
            unsigned int LSBFST:1;  // MSB/LSB Select
            unsigned int DUMMY:4;
        } bCTL;
        volatile unsigned char ucCTL;
    };
/********************************************************************/
/* [ UART Configure Register ]  USI_UCFG                   0x40    */
/********************************************************************/
    union USI_UCFG_tag {
        volatile struct {
            unsigned int UPREN:1;   // Parity Enable
            unsigned int UPMD:1;    // Parity Mode Select
            unsigned int USTPB:1;   // Stop Bit Select
            unsigned int UCHLN:1;   // Character length
            unsigned int DUMMY:4;
        } bCTL;
        volatile unsigned char ucCTL;
    };
/********************************************************************/
/* [ UART IE Register ]  USI_UIE                            0x41    */
/********************************************************************/
    union USI_UIE_tag {
        volatile struct {
            unsigned int UTDIE:1;   // Tx Interrupt Enable
            unsigned int URDIE:1;   // Rx Interrupt Enable
            unsigned int UEIE:1;    // Error Interrupt Enable
            unsigned int DUMMY:5;
        } bCTL;
        volatile unsigned char ucCTL;
    };
/********************************************************************/
/* [ UART IF Register ]  USI_UIF                            0x42    */
/********************************************************************/
    union USI_UIF_tag {
        volatile struct {
            unsigned int UTDIF:1;   // Trans-buffer Empty Flag
            unsigned int URDIF:1;   // Receive-buffer Full Flag
            unsigned int UOEIF:1;   // Overrun Error Flag
            unsigned int USEIF:1;   // Frame Error Flag
            unsigned int UPEIF:1;   // Parity Error Flag
            unsigned int UTBSY:1;   // UART Tx Busy Flag
            unsigned int URBSY:1;   // UART Rx Busy Flag
            unsigned int DUMMY:1;
        } bCTL;
        volatile unsigned char ucCTL;
    };
/********************************************************************/
/* [ SPI Configure Register ]  USI_SCFG                     0x50    */
/********************************************************************/
    union USI_SCFG_tag {
        volatile struct {
            unsigned int SFSTMOD:1; // Fast Mode
            unsigned int SMSKEN:1;  // Receive Data Mask and Re-transreceive Enable
            unsigned int SCPOL:1;   // Clock polarity select
            unsigned int SCPHA:1;   // Clock phase select
            unsigned int SCHLN:1;   // Character length
            unsigned int SCMD:1;    // Command Bit (For 9 bit)
            unsigned int DUMMY:2;
        } bCTL;
        volatile unsigned char ucCTL;
    };
/********************************************************************/
/* [ SPI IE Register ]  USI_SIE                             0x51    */
/********************************************************************/
    union USI_SIE_tag {
        volatile struct {
            unsigned int STDIE:1;   // Tx Interrupt Enable
            unsigned int SRDIE:1;   // Rx Interrupt Enable
            unsigned int SEIE:1;    // Error Interrupt Enable
            unsigned int DUMMY:5;
        } bCTL;
        volatile unsigned char ucCTL;
    };
/********************************************************************/
/* [ SPI IF Register ]  USI_SIF                             0x52    */
/********************************************************************/
    union USI_SIF_tag {
        volatile struct {
            unsigned int STDIF:1;   // Trans-buffer Empty Flag
            unsigned int SRDIF:1;   // Receive-buffer Full Flag
            unsigned int SEIF:1;    // Overrun Error Flag
            unsigned int SSIF:1;    // Status Flag
            unsigned int DUMMY:4;
        } bCTL;
        volatile unsigned char ucCTL;
    };
/********************************************************************/
/* [ I2C Master Trigger Register ]  USI_IMTG                0x60    */
/********************************************************************/
    union USI_IMSTG_tag {
        volatile struct {
            unsigned int IMTGMOD:3; // Master Trigger Mode
            unsigned int DUMMY1:1;  //
            unsigned int IMTG:1;    // I2C master operation trigger
            unsigned int DUMMY2:3;
        } bCTL;
        volatile unsigned char ucCTL;
    };
/********************************************************************/
/* [ I2C Master IE Register ]  USI_IMIE                     0x61    */
/********************************************************************/
    union USI_IMSIE_tag {
        volatile struct {
            unsigned int IMIE:1;    // Master Interrupt Enable
            unsigned int IMEIE:1;   // Error Interrupt Enable
            unsigned int DUMMY:6;
        } bCTL;
        volatile unsigned char ucCTL;
    };
/********************************************************************/
/* [ I2C Master IF Register ]  USI_IMIF                     0x62    */
/********************************************************************/
    union USI_IMSIF_tag {
        volatile struct {
            unsigned int IMIF:1;    // Master Interrupt Flag
            unsigned int IMEIF:1;   // Overrun Error Flag
            unsigned int IMSTA:3;   // Master IF State
            unsigned int IMBSY:1;   // Master Busy Flag
            unsigned int DUMMY:2;
        } bCTL;
        volatile unsigned char ucCTL;
    };
/********************************************************************/
/* [ I2C Slave Trigger Register ]  USI_ISTG                 0x70    */
/********************************************************************/
    union USI_ISVTG_tag {
        volatile struct {
            unsigned int ISTGMOD:3; // Slave Trigger Mode
            unsigned int DUMMY1:1;
            unsigned int ISTG:1;    // Slave Trigger
            unsigned int DUMMY2:3;
        } bCTL;
        volatile unsigned char ucCTL;
    };
/********************************************************************/
/* [ I2C Slave IE Register ]  USI_ISIE                      0x71    */
/********************************************************************/
    union USI_ISVIE_tag {
        volatile struct {
            unsigned int ISIE:1;    // Slave Interrupt Enable
            unsigned int ISEIE:1;   // Error Interrupt Enable
            unsigned int DUMMY2:6;
        } bCTL;
        volatile unsigned char ucCTL;
    };
/********************************************************************/
/* [ I2C Slave IF Register ]  USI_ISIF                      0x72    */
/********************************************************************/
    union USI_ISVIF_tag {
        volatile struct {
            unsigned int ISIF:1;    // Slave Interrupt Flag
            unsigned int ISEIF:1;   // Overrun Error Flag
            unsigned int ISSTA:3;   // Slave IF State
            unsigned int ISBSY:1;   // Slave Busy Flag
            unsigned int DUMMY2:2;
        } bCTL;
        volatile unsigned char ucCTL;
    };

/****************************************************************************
 * 	Macros (#define)
 ****************************************************************************/
#define USI_BASE                (0x00300400)

#define USI_GCFG(n)             (*(union USI_GCFG_tag *)(USI_BASE+(n)*0x100)).ucCTL
#define USI_USIMOD(n)           (*(union USI_GCFG_tag *)(USI_BASE+(n)*0x100)).bCTL.USIMOD
#define USI_LSBFST(n)           (*(union USI_GCFG_tag *)(USI_BASE+(n)*0x100)).bCTL.LSBFST

#define USI_TD(n)               (*(volatile unsigned char *)(USI_BASE+(n)*0x100+0x1))
#define USI_RD(n)               (*(volatile const unsigned char *)(USI_BASE+(n)*0x100+0x2))

#define USI_UCFG(n)             (*(union USI_UCFG_tag *)(USI_BASE+(n)*0x100+0x40)).ucCTL
#define USI_UPREN(n)            (*(union USI_UCFG_tag *)(USI_BASE+(n)*0x100+0x40)).bCTL.UPREN
#define USI_UPMD(n)             (*(union USI_UCFG_tag *)(USI_BASE+(n)*0x100+0x40)).bCTL.UPMD
#define USI_USTPB(n)            (*(union USI_UCFG_tag *)(USI_BASE+(n)*0x100+0x40)).bCTL.USTPB
#define USI_UCHLN(n)            (*(union USI_UCFG_tag *)(USI_BASE+(n)*0x100+0x40)).bCTL.UCHLN

#define USI_UIE(n)              (*(union USI_UIE_tag *)(USI_BASE+(n)*0x100+0x41)).ucCTL
#define USI_UTDIE(n)            (*(union USI_UIE_tag *)(USI_BASE+(n)*0x100+0x41)).bCTL.UTDIE
#define USI_URDIE(n)            (*(union USI_UIE_tag *)(USI_BASE+(n)*0x100+0x41)).bCTL.URDIE
#define USI_UEIE(n)             (*(union USI_UIE_tag *)(USI_BASE+(n)*0x100+0x41)).bCTL.UEIE

#define USI_UIF(n)              (*(union USI_UIF_tag *)(USI_BASE+(n)*0x100+0x42)).ucCTL
#define USI_UTDIF(n)            (*(union USI_UIF_tag *)(USI_BASE+(n)*0x100+0x42)).bCTL.UTDIF
#define USI_URDIF(n)            (*(union USI_UIF_tag *)(USI_BASE+(n)*0x100+0x42)).bCTL.URDIF
#define USI_UOEIF(n)            (*(union USI_UIF_tag *)(USI_BASE+(n)*0x100+0x42)).bCTL.UOEIF
#define USI_USEIF(n)            (*(union USI_UIF_tag *)(USI_BASE+(n)*0x100+0x42)).bCTL.USEIF
#define USI_UPEIF(n)            (*(union USI_UIF_tag *)(USI_BASE+(n)*0x100+0x42)).bCTL.UPEIF
#define USI_UTBSY(n)            (*(union USI_UIF_tag *)(USI_BASE+(n)*0x100+0x42)).bCTL.UTBSY
#define USI_URBSY(n)            (*(union USI_UIF_tag *)(USI_BASE+(n)*0x100+0x42)).bCTL.URBSY

#define USI_SCFG(n)             (*(union USI_SCFG_tag *)(USI_BASE+(n)*0x100+0x50)).ucCTL
#define USI_SFSTMOD(n)          (*(union USI_SCFG_tag *)(USI_BASE+(n)*0x100+0x50)).bCTL.SFSTMOD
#define USI_SMSKEN(n)           (*(union USI_SCFG_tag *)(USI_BASE+(n)*0x100+0x50)).bCTL.SMSKEN
#define USI_SCPOL(n)            (*(union USI_SCFG_tag *)(USI_BASE+(n)*0x100+0x50)).bCTL.SCPOL
#define USI_SCPHA(n)            (*(union USI_SCFG_tag *)(USI_BASE+(n)*0x100+0x50)).bCTL.SCPHA
#define USI_SCHLN(n)            (*(union USI_SCFG_tag *)(USI_BASE+(n)*0x100+0x50)).bCTL.SCHLN
#define USI_SCMD(n)             (*(union USI_SCFG_tag *)(USI_BASE+(n)*0x100+0x50)).bCTL.SCMD

#define USI_SIE(n)              (*(union USI_SIE_tag *)(USI_BASE+(n)*0x100+0x51)).ucCTL
#define USI_STDIE(n)            (*(union USI_SIE_tag *)(USI_BASE+(n)*0x100+0x51)).bCTL.STDIE
#define USI_SRDIE(n)            (*(union USI_SIE_tag *)(USI_BASE+(n)*0x100+0x51)).bCTL.SRDIE
#define USI_SEIE(n)             (*(union USI_SIE_tag *)(USI_BASE+(n)*0x100+0x51)).bCTL.SEIE

#define USI_SIF(n)              (*(union USI_SIF_tag *)(USI_BASE+(n)*0x100+0x52)).ucCTL
#define USI_STDIF(n)            (*(union USI_SIF_tag *)(USI_BASE+(n)*0x100+0x52)).bCTL.STDIF
#define USI_SRDIF(n)            (*(union USI_SIF_tag *)(USI_BASE+(n)*0x100+0x52)).bCTL.SRDIF
#define USI_SEIF(n)             (*(union USI_SIF_tag *)(USI_BASE+(n)*0x100+0x52)).bCTL.SEIF
#define USI_SSIF(n)             (*(union USI_SIF_tag *)(USI_BASE+(n)*0x100+0x52)).bCTL.SSIF

#define USI_SMSK(n)             (*(volatile unsigned char *)(USI_BASE+(n)*0x100+0x5f))

#define USI_IMSTG(n)            (*(union USI_IMSTG_tag *)(USI_BASE+(n)*0x100+0x60)).ucCTL
#define USI_IMTGMOD(n)          (*(union USI_IMSTG_tag *)(USI_BASE+(n)*0x100+0x60)).bCTL.IMTGMOD
#define USI_IMTG(n)             (*(union USI_IMSTG_tag *)(USI_BASE+(n)*0x100+0x60)).bCTL.IMTG

#define USI_IMSIE(n)            (*(union USI_IMSIE_tag *)(USI_BASE+(n)*0x100+0x61).ucCTL
#define USI_IMIE(n)             (*(union USI_IMSIE_tag *)(USI_BASE+(n)*0x100+0x61).bCTL.IMIE
#define USI_IMEIE(n)            (*(union USI_IMSIE_tag *)(USI_BASE+(n)*0x100+0x61).bCTL.IMEIE

#define USI_IMSIF(n)            (*(union USI_IMSIF_tag *)(USI_BASE+(n)*0x100+0x62)).ucCTL
#define USI_IMIF(n)             (*(union USI_IMSIF_tag *)(USI_BASE+(n)*0x100+0x62)).bCTL.IMIF
#define USI_IMEIF(n)            (*(union USI_IMSIF_tag *)(USI_BASE+(n)*0x100+0x62)).bCTL.IMEIF
#define USI_IMSTA(n)            (*(union USI_IMSIF_tag *)(USI_BASE+(n)*0x100+0x62)).bCTL.IMSTA
#define USI_IMBSY(n)            (*(union USI_IMSIF_tag *)(USI_BASE+(n)*0x100+0x62)).bCTL.IMBSY

#define USI_ISVTG(n)            (*(union USI_ISVTG_tag *)(USI_BASE+(n)*0x100+0x70)).ucCTL
#define USI_ISTGMOD(n)          (*(union USI_ISVTG_tag *)(USI_BASE+(n)*0x100+0x70)).bCTL.ISTGMOD
#define USI_ISTG(n)             (*(union USI_ISVTG_tag *)(USI_BASE+(n)*0x100+0x70)).bCTL.ISTG

#define USI_ISVIE(n)            (*(union USI_ISVIE_tag *)(USI_BASE+(n)*0x100+0x71)).ucCTL
#define USI_ISIE(n)             (*(union USI_ISVIE_tag *)(USI_BASE+(n)*0x100+0x71)).bCTL.ISIE
#define USI_ISEIE(n)            (*(union USI_ISVIE_tag *)(USI_BASE+(n)*0x100+0x71)).bCTL.ISEIE

#define USI_ISVIF(n)            (*(union USI_ISVIF_tag *)(USI_BASE+(n)*0x100+0x72)).ucCTL
#define USI_ISIF(n)             (*(union USI_ISVIF_tag *)(USI_BASE+(n)*0x100+0x72)).bCTL.ISIF
#define USI_ISEIF(n)            (*(union USI_ISVIF_tag *)(USI_BASE+(n)*0x100+0x72)).bCTL.ISEIF
#define USI_ISSTA(n)            (*(union USI_ISVIF_tag *)(USI_BASE+(n)*0x100+0x72)).bCTL.ISSTA
#define USI_ISBSY(n)            (*(union USI_ISVIF_tag *)(USI_BASE+(n)*0x100+0x72)).bCTL.ISBSY

/****************************************************************************
 * 	Function prototypes
 ****************************************************************************/

#ifdef __cplusplus
}
#endif
#endif                          /* _USI_REG_H */
