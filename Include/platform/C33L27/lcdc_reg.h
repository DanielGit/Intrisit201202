/****************************************************************************
 * 	Copyright (C) SEIKO EPSON CORP. 2010
 *
 * 	File Name   : reg_lcdc.h
 * 	Description : S1C33L27 LCDC register definition file
 *
 * 	Revision History:
 *  2009/01/12   Joan Lin   New creation
 *  2010/01/14   Jackie Zhu Modify according L27 manual
 *
 ****************************************************************************/
#ifndef _LCDC_REG_H
#define _LCDC_REG_H

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************/
/* [ Status & power save config reg ] LCDC_PSAVE    0x0030_2004    */
/********************************************************************/
    union LCDC_PSAVE_tag {
        volatile struct {
            unsigned int PSAVE:2;       //Power save mode
            unsigned int DUMMY1:5;      //
            unsigned int VNDPF:1;       //Vertical display status
            unsigned int DUMMY2:23;
            unsigned int FRINTF:1;      //Frame int flag
        } bCTL;
        volatile unsigned long ulCTL;
    };

/********************************************************************/
/* [ Horizontal display reg ] LCDC_HDISP             0x0030_2010    */
/********************************************************************/
    union LCDC_HDISP_tag {
        volatile struct {
            unsigned int HDPCNT:7;      //Horizontal dispaly period
            unsigned int DUMMY1:9;      //
            unsigned int HTCNT:7;       //Horizontal total period
        } bCTL;
        volatile unsigned long ulCTL;
    };

/********************************************************************/
/* [ Vertical display reg ] LCDC_VDISP              0x0030_2014    */
/********************************************************************/
    union LCDC_VDISP_tag {
        volatile struct {
            unsigned int VDPCNT:10;     //Vertical dispaly period
            unsigned int DUMMY1:6;      //
            unsigned int VTCNT:10;      //Vertical total period
        } bCTL;
        volatile unsigned long ulCTL;
    };

/********************************************************************/
/* [ FPLINE Pulse setup reg ] LCDC_FPLINE            0x0030_2028    */
/********************************************************************/
    union LCDC_FPLINE_tag {
        volatile struct {
            unsigned int FPLINE_WD:7;       //FPLINE pulse width
            unsigned int FPLINE_POL:1;      //FPLINE pulse polarity
            unsigned int DUMMY:8;
            unsigned int FPLINE_ST:10;      //FPLINE pulse start pos
        } bCTL;
        volatile unsigned long ulCTL;
    };

/********************************************************************/
/* [ FPFRAME Pulse setup reg ] LCDC_FPFR            0x0030_202c     */
/********************************************************************/
    union LCDC_FPFR_tag {
        volatile struct {
            unsigned int FPFRAME_WD:7;       //FPFRAME pulse width
            unsigned int FPFRAME_POL:1;      //FPFRAME pulse polarity
            unsigned int DUMMY2:8;
            unsigned int FPFRAME_ST:10;      //FPFRAME pulse start pos
        } bCTL;
        volatile unsigned long ulCTL;
    };

/********************************************************************/
/* [ FPFRAME Pulse offset reg ] LCDC_FPFROFS         0x0030_2030    */
/********************************************************************/
    union LCDC_FPFROFS_tag {
        volatile struct {
            unsigned int FPFRAME_STOFS:10;     //FPFRAME pulse start offset
            unsigned int DUMMY:6;
            unsigned int FPFRAME_STPOFS:10;    //FPFRAME pulse stop offset
        } bCTL;
        volatile unsigned long ulCTL;
    };

/********************************************************************/
/* [ HR-TFT special output reg ] LCDC_TFTSO          0x0030_2040    */
/********************************************************************/
    union LCDC_TFTSO_tag {
        volatile struct {
            unsigned int CTL01SWAP:1;       //TFT_CTL0/TFT_CTL1 swap
            unsigned int FPSHIFT_POL:1;     //FPSHIFT polarity
            unsigned int CTLCNT_RUN:1;      //TFT_CTL0-2 preset enable
            unsigned int CTL1CTL:1;         //TFT_CTL1 control
        } bCTL;
        volatile unsigned long ulCTL;
    };

/********************************************************************/
/* [ TFT_CTL1 pulse reg ] LCDC_TFT_CTL1              0x0030_2044    */
/********************************************************************/
    union LCDC_TFT_CTL1_tag {
        volatile struct {
            unsigned int CTL1ST:10;     //TFT_CTL1 pulse start offset
            unsigned int DUMMY:6;
            unsigned int CTL1STP:10;    //TFT_CTL1 pulse stop offset
        } bCTL;
        volatile unsigned long ulCTL;
    };

/********************************************************************/
/* [ TFT_CTL0 pulse reg ] LCDC_TFT_CTL0              0x0030_2048    */
/********************************************************************/
    union LCDC_TFT_CTL0_tag {
        volatile struct {
            unsigned int CTL0ST:10;     //TFT_CTL0 pulse start offset
            unsigned int DUMMY:6;
            unsigned int CTL0STP:10;    //TFT_CTL0 pulse stop offset
        } bCTL;
        volatile unsigned long ulCTL;
    };

/********************************************************************/
/* [ LCDC Reload Control Register ] LCDC_RLDTL       0x0030_2050    */
/********************************************************************/
    union LCDC_RLDTL_tag {
        volatile struct {
            unsigned int CTABRLD:1;     // Control table reload trigger
            unsigned int LUTRLD:1;      // LUT reload trigger
        } bCTL;
        volatile unsigned long ulCTL;
    };

/********************************************************************/
/* [ lcdc disp mode reg ] LCDC_DISPMOD               0x0030_2060    */
/********************************************************************/
    union LCDC_DISPMOD_tag {
        volatile struct {
            unsigned int BPP:3;             //Bit-per-pixel select
            unsigned int DUMMY1:1;
            unsigned int LUTPASS:1;         //LUT bypass mode select
            unsigned int DUMMY2:2;
            unsigned int FRMRPT:1;          //Frame repeat for EL panel
            unsigned int DUMMY3:16;
            unsigned int BLANK:1;           //Display blank enable
            unsigned int SWINV:1;           //Software video invert
            unsigned int DWD:2;             //LCD panel data width select
            unsigned int DUMMY4:1;
            unsigned int FPSHIFT_MSK:1;     //FPSHIFT mask enable
            unsigned int COLOR:1;           //Color/mono select
            unsigned int PANELSEL:1;        //Panel type select
        } bCTL;
        volatile unsigned long ulCTL;
    };

/********************************************************************/
/* [ Sub win start pos reg ] LCDC_SUBSP                0x0030_2088   */
/********************************************************************/
    union LCDC_SUBSP_tag {
        volatile struct {
            unsigned int PIP_XSTART:10;     //Sub wind horizontal start pos
            unsigned int DUMMY1:6;      //
            unsigned int PIP_YSTART:10;     //Sub wind vertical start pos
            unsigned int DUMMY2:5;      //
            unsigned int PIP_EN:1;       //PIP sub win enable
        } bCTL;
        volatile unsigned long ulCTL;
    };

/********************************************************************/
/* [ Sub win end pos reg ] LCDC_SUVEP               0x0030_208c     */
/********************************************************************/
    union LCDC_SUVEP_tag {
        volatile struct {
            unsigned int PIP_XEND:10;    //Sub wind horizontal end pos
            unsigned int DUMMY:6;       //
            unsigned int PIP_YEND:10;    //Sub wind vertical end pos
        } bCTL;
        volatile unsigned long ulCTL;
    };

/********************************************************************/
/* [ Mono Look-up Table reg ] LCDC_MLUT0             0x0030_2090     */
/********************************************************************/
    union LCDC_MLUT0_tag {
        volatile struct {
            unsigned int MLUTx0:4;   // Monochrome LUT enrty 0 data
            unsigned int MLUTx1:4;   // Monochrome LUT enrty 1 data
            unsigned int MLUTx2:4;   // Monochrome LUT enrty 2 data
            unsigned int MLUTx3:4;   // Monochrome LUT enrty 3 data
            unsigned int MLUTx4:4;   // Monochrome LUT enrty 4 data
            unsigned int MLUTx5:4;   // Monochrome LUT enrty 5 data
            unsigned int MLUTx6:4;   // Monochrome LUT enrty 6 data
            unsigned int MLUTx7:4;   // Monochrome LUT enrty 7 data
        } bCTL;
        volatile unsigned long ulCTL;
    };

/********************************************************************/
/* [ Mono Look-up Table reg ] LCDC_MLUT1             0x0030_2094     */
/********************************************************************/
    union LCDC_MLUT1_tag {
        volatile struct {
            unsigned int MLUTx0:4;   // Monochrome LUT enrty 8 data
            unsigned int MLUTx1:4;   // Monochrome LUT enrty 9 data
            unsigned int MLUTx2:4;   // Monochrome LUT enrty 10 data
            unsigned int MLUTx3:4;   // Monochrome LUT enrty 11 data
            unsigned int MLUTx4:4;   // Monochrome LUT enrty 12 data
            unsigned int MLUTx5:4;   // Monochrome LUT enrty 13 data
            unsigned int MLUTx6:4;   // Monochrome LUT enrty 14 data
            unsigned int MLUTx7:4;   // Monochrome LUT enrty 15 data
        } bCTL;
        volatile unsigned long ulCTL;
    };

//------------------------------
// Macro Def 32bit access only
#define LCDC_BASE   0x00302000

#define LCDC_INT                (*(volatile unsigned char *)(LCDC_BASE))

#define LCDC_PSAVE              (*(union LCDC_PSAVE_tag *)(LCDC_BASE+0x04)).ulCTL
#define LCDC_PS                 (*(union LCDC_PSAVE_tag *)(LCDC_BASE+0x04)).bCTL.PSAVE
#define LCDC_VNDPF              (*(union LCDC_PSAVE_tag *)(LCDC_BASE+0x04)).bCTL.VNDPF
#define LCDC_FRINTF             (*(union LCDC_PSAVE_tag *)(LCDC_BASE+0x04)).bCTL.FRINTF

#define LCDC_HDISP              (*(union LCDC_HDISP_tag *)(LCDC_BASE+0x10)).ulCTL
#define LCDC_HDPCNT             (*(union LCDC_HDISP_tag *)(LCDC_BASE+0x10)).bCTL.HDPCNT
#define LCDC_HTCNT              (*(union LCDC_HDISP_tag *)(LCDC_BASE+0x10)).bCTL.HTCNT

#define LCDC_VDISP              (*(union LCDC_VDISP_tag *)(LCDC_BASE+0x14)).ulCTL
#define LCDC_VDPCNT             (*(union LCDC_VDISP_tag *)(LCDC_BASE+0x14)).bCTL.VDPCNT
#define LCDC_VTCNT              (*(union LCDC_VDISP_tag *)(LCDC_BASE+0x14)).bCTL.VTCNT

#define LCDC_MODR               (*(volatile unsigned char *)(LCDC_BASE+0x18))
#define LCDC_HDPS               (*(volatile unsigned short *)(LCDC_BASE+0x20))
#define LCDC_VDPS               (*(volatile unsigned short *)(LCDC_BASE+0x24))

#define LCDC_FPLINE             (*(union LCDC_FPLINE_tag *)(LCDC_BASE+0x28)).ulCTL
#define LCDC_FPLINE_WD          (*(union LCDC_FPLINE_tag *)(LCDC_BASE+0x28)).bCTL.FPLINE_WD
#define LCDC_FPLINE_POL         (*(union LCDC_FPLINE_tag *)(LCDC_BASE+0x28)).bCTL.FPLINE_POL
#define LCDC_FPLINE_ST          (*(union LCDC_FPLINE_tag *)(LCDC_BASE+0x28)).bCTL.FPLINE_ST

#define LCDC_FPFR               (*(union LCDC_FPFR_tag *)(LCDC_BASE+0x2c)).ulCTL
#define LCDC_FPFRAME_WD         (*(union LCDC_FPFR_tag *)(LCDC_BASE+0x2c)).bCTL.FPFRAME_WD
#define LCDC_FPFRAME_POL        (*(union LCDC_FPFR_tag *)(LCDC_BASE+0x2c)).bCTL.FPFRAME_POL
#define LCDC_FPFRAME_ST         (*(union LCDC_FPFR_tag *)(LCDC_BASE+0x2c)).bCTL.FPFRAME_ST

#define LCDC_FPFROFS            (*(union LCDC_FPFROFS_tag *)(LCDC_BASE+0x30)).ulCTL
#define LCDC_FPFRAME_STOFS      (*(union LCDC_FPFROFS_tag *)(LCDC_BASE+0x30)).bCTL.FPFRAME_STOFS
#define LCDC_FPFRAME_STPOFS     (*(union LCDC_FPFROFS_tag *)(LCDC_BASE+0x30)).bCTL.FPFRAME_STPOFS

#define LCDC_TFTSO              (*(union LCDC_TFTSO_tag *)(LCDC_BASE+0x40)).ulCTL
#define LCDC_CTL01SWAP          (*(union LCDC_TFTSO_tag *)(LCDC_BASE+0x40)).bCTL.CTL01SWAP
#define LCDC_FPSHIFT_POL        (*(union LCDC_TFTSO_tag *)(LCDC_BASE+0x40)).bCTL.FPSHIFT_POL
#define LCDC_CTLCNT_RUN         (*(union LCDC_TFTSO_tag *)(LCDC_BASE+0x40)).bCTL.CTLCNT_RUN
#define LCDC_CTL1CTL            (*(union LCDC_TFTSO_tag *)(LCDC_BASE+0x40)).bCTL.CTL1CTL

#define LCDC_TFT_CTL1           (*(union LCDC_TFT_CTL1_tag *)(LCDC_BASE+0x44)).ulCTL
#define LCDC_CTL1ST             (*(union LCDC_TFT_CTL1_tag *)(LCDC_BASE+0x44)).bCTL.CTL1ST
#define LCDC_CTL1STP            (*(union LCDC_TFT_CTL1_tag *)(LCDC_BASE+0x44)).bCTL.CTL1STP

#define LCDC_TFT_CTL0           (*(union LCDC_TFT_CTL0_tag *)(LCDC_BASE+0x48)).ulCTL
#define LCDC_CTL0ST             (*(union LCDC_TFT_CTL0_tag *)(LCDC_BASE+0x48)).bCTL.CTL0ST
#define LCDC_CTL0STP            (*(union LCDC_TFT_CTL0_tag *)(LCDC_BASE+0x48)).bCTL.CTL0STP

#define LCDC_TFT_CTL2           (*(volatile unsigned short *)(LCDC_BASE+0x4c))

#define LCDC_RLDTL              (*(union LCDC_RLDTL_tag *)(LCDC_BASE+0x50)).ulCTL
#define LCDC_CTABRLD            (*(union LCDC_RLDTL_tag *)(LCDC_BASE+0x50)).bCTL.CTABRLD
#define LCDC_LUTRLD             (*(union LCDC_RLDTL_tag *)(LCDC_BASE+0x50)).bCTL.LUTRLD

#define LCDC_RLDADR             (*(volatile unsigned long *)(LCDC_BASE+0x54))

#define LCDC_DISPMOD            (*(union LCDC_DISPMOD_tag *)(LCDC_BASE+0x60)).ulCTL
#define LCDC_BPP                (*(union LCDC_DISPMOD_tag *)(LCDC_BASE+0x60)).bCTL.BPP
#define LCDC_LUTPASS            (*(union LCDC_DISPMOD_tag *)(LCDC_BASE+0x60)).bCTL.LUTPASS
#define LCDC_FRMRPT             (*(union LCDC_DISPMOD_tag *)(LCDC_BASE+0x60)).bCTL.FRMRPT
#define LCDC_BLANK              (*(union LCDC_DISPMOD_tag *)(LCDC_BASE+0x60)).bCTL.BLANK
#define LCDC_SWINV              (*(union LCDC_DISPMOD_tag *)(LCDC_BASE+0x60)).bCTL.SWINV
#define LCDC_DWD                (*(union LCDC_DISPMOD_tag *)(LCDC_BASE+0x60)).bCTL.DWD
#define LCDC_FPSHIFT_MSK        (*(union LCDC_DISPMOD_tag *)(LCDC_BASE+0x60)).bCTL.FPSHIFT_MSK
#define LCDC_COLOR              (*(union LCDC_DISPMOD_tag *)(LCDC_BASE+0x60)).bCTL.COLOR
#define LCDC_PANELSEL           (*(union LCDC_DISPMOD_tag *)(LCDC_BASE+0x60)).bCTL.PANELSEL

#define LCDC_MAINADR            (*(volatile unsigned long *)(LCDC_BASE+0x70))

#define LCDC_MAINOFS            (*(volatile unsigned short *)(LCDC_BASE+0x74))

#define LCDC_SUBADR             (*(volatile unsigned long *)(LCDC_BASE+0x80))

#define LCDC_SUBOFS             (*(volatile unsigned short *)(LCDC_BASE+0x84))

#define LCDC_SUBSP              (*(union LCDC_SUBSP_tag *)(LCDC_BASE+0x88)).ulCTL
#define LCDC_PIP_XSTART         (*(union LCDC_SUBSP_tag *)(LCDC_BASE+0x88)).bCTL.PIP_XSTART
#define LCDC_PIP_YSTART         (*(union LCDC_SUBSP_tag *)(LCDC_BASE+0x88)).bCTL.PIP_YSTART
#define LCDC_PIP_EN             (*(union LCDC_SUBSP_tag *)(LCDC_BASE+0x88)).bCTL.PIP_EN

#define LCDC_SUBEP              (*(union LCDC_SUVEP_tag *)(LCDC_BASE+0x8c)).ulCTL
#define LCDC_PIP_XEND           (*(union LCDC_SUVEP_tag *)(LCDC_BASE+0x8c)).bCTL.PIP_XEND
#define LCDC_PIP_YEND           (*(union LCDC_SUVEP_tag *)(LCDC_BASE+0x8c)).bCTL.PIP_YEND

#define LCDC_MLUT0              (*(union LCDC_MLUT0_tag *)(LCDC_BASE+0x90)).ulCTL
#define LCDC_MLUTx0(n)          (*(union LCDC_MLUT0_tag *)(LCDC_BASE+0x90)).bCTL.MLUTx##n

#define LCDC_MLUT1              (*(union LCDC_MLUT1_tag *)(LCDC_BASE+0x94)).ulCTL
#define LCDC_MLUTx1(n)          (*(union LCDC_MLUT1_tag *)(LCDC_BASE+0x94)).bCTL.MLUTx##n



#ifdef __cplusplus
}
#endif
#endif                          //_LCDC_REG_H
