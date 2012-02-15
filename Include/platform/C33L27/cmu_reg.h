/****************************************************************************
 * 	Copyright (C) SEIKO EPSON CORP. 2010
 *
 * 	File Name   : cmu_reg.h
 * 	Description : S1C33L27 CMU register definition file
 *
 * 	Revision History:
 *  2010/01/13   Joan Lin     New creation
 ****************************************************************************/
#ifndef _CMU_REG_H
#define _CMU_REG_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 * 	Structures (typedef, struct, union)
 ****************************************************************************/

/********************************************************************/
/* [ sytem clock control reg ] CMU_CLKCNTL          0x0030_0101    */
/********************************************************************/
    union CMU_CLKCNTL_tag {
        volatile struct {
            unsigned int SOSC3:1;       //Low-speed OSC3 On/off
            unsigned int SOSC1:1;       //High-speed OSC1 On/off
            unsigned int DUMMY:2;
            unsigned int OSCTM:4;       //OSC3 stablization-wait time
        } bCTL;
        volatile unsigned char ucCTL;
    };

/********************************************************************/
/* [ PLL clock control reg ] CMU_PLL0               0x0030_0108    */
/********************************************************************/
    union CMU_PLL0_tag {
        volatile struct {
            unsigned int PLLPOWR:1;     //PLL on/off control
            unsigned int DUMMY:1;       //
            unsigned int PLLV:2;        //PLL V-divider setup
            unsigned int PLLN:4;        //PLL multiplictation rate setup
        } bCTL;
        volatile unsigned char ucCTL;
    };

/********************************************************************/
/* [ PLL clock control reg ] CMU_PLL1               0x0030_0109    */
/********************************************************************/
    union CMU_PLL1_tag {
        volatile struct {
            unsigned int PLLRS:4;       //PLLPF resistance setup
            unsigned int PLLVC:4;       //PLL VCO Kv setup
        } bCTL;
        volatile unsigned char ucCTL;
    };

/********************************************************************/
/* [ PLL clock control reg ] CMU_PLL3               0x0030_010a    */
/********************************************************************/
    union CMU_PLL3_tag {
        volatile struct {
            unsigned int PLLCP:5;       //PLL Charge Pump current setup
            unsigned int PLLBYP:1;      //PLL bypass mode setup
            unsigned int PLLCS:2;       //PLL LPF capacitance setup
        } bCTL;
        volatile unsigned char ucCTL;
    };

/********************************************************************/
/* [ SSCG macro control reg ] CMU_SSCG              0x0030_010d    */
/********************************************************************/
    union CMU_SSCG_tag {
        volatile struct {
            unsigned int SSMCIDT:4;     //SSCG macro maximum frequency change width
            unsigned int SSMCITM:4;     //SSCG macro interval timer setting
        } bCTL;
        volatile unsigned char ucCTL;
    };

/********************************************************************/
/* [ clock control reg 0 ] CMU CLOCK Register 0  0x0030_0114        */
/********************************************************************/
    union CMU_CLKCTL0_tag {
        volatile struct {
            unsigned int TCLK_SEL:2;    //T16A6 counter clock source select
            unsigned int TCLK_CKE:1;    //T16A6 counter clock enable
            unsigned int T16A_CKE:1;    //T16A6 register clock enable
        } bCTL;
        volatile unsigned char ucCTL;
    };

/********************************************************************/
/* [ clock control reg 1 ] CMU CLOCK Register 1 0x0030_0115         */
/********************************************************************/
    union CMU_CLKCTL1_tag {
        volatile struct {
            unsigned int DMAC_CKE:1;    //DMAC clock enable
            unsigned int ITC_CKE:1;     //ITC clock enable
            unsigned int I2S_CKE:1;     //I2S clock enable
            unsigned int SDMMC_CKE:1;   //SD_MMC clock enable
            unsigned int PORT_CKE:1;    //PORT clock enable
            unsigned int ADC_CKE:1;     //ADC clock enable
            unsigned int REMC_CKE:1;    //REMC clock enable
            unsigned int NANDIF_CKE:1;  //CARD clock enable
        } bCTL;
        volatile unsigned char ucCTL;
    };

/********************************************************************/
/* [ clock control reg 2 ] CMU CLOCK Register 2 0x0030_0116         */
/********************************************************************/
    union CMU_CLKCTL2_tag {
        volatile struct {
            unsigned int BCLK_CKE:1;     //BCLK clock enable
            unsigned int BCU_CKE:1;      //BCU clock enable
            unsigned int USB_CKE:1;      //USB clock enable
            unsigned int USBREG_CKE:1;   //USBREG_CKE clock enable
            unsigned int LCDC_CKE:1;     //LCDC_CKE clock enable
            unsigned int LCDC_AHB_CKE:1; //LCDC_AHB_CKE clock enable
            unsigned int LCDC_SAPB_CKE:1;//LCDC_SAPB_CKE clock enable
            unsigned int HIF_CKE:1;      //HIF_CKE clock enable
        } bCTL;
        volatile unsigned char ucCTL;
    };

/********************************************************************/
/* [ clock control reg 3 ] CMU CLOCK Register 3 0x0030_0117         */
/********************************************************************/
    union CMU_CLKCTL3_tag {
        volatile struct {
            unsigned int USIL_CKE:1;    //USIL_CKE clock enable
            unsigned int USI_CKE:1;     //USI_CKE clock enable
            unsigned int DUMMY:2;       //DUMMY clock enable
            unsigned int T16P_CKE:1;    //T16P clock enable
            unsigned int UART_CKE:1;    //UART clock enable
            unsigned int T8F_CKE:1;     //T8F clock enable
            unsigned int PSC1_2_CKE:1;  //PSC1_2 clock enable
        } bCTL;
        volatile unsigned char ucCTL;
    };

/********************************************************************/
/* [ clock control reg 4 ] CMU CLOCK Register 4 0x0030_0118         */
/********************************************************************/
    union CMU_CLKCTL4_tag {
        volatile struct {
            unsigned int WDT_CKE:1;       //WDT clock enable
            unsigned int RTC_SAPB_CKE:1;  //RTC_SAPB clock enable
            unsigned int A3RAM_CKE:1;     //A3RAM clock enable
            unsigned int IVRAM_ARB_CKE:1; //IVRAM_ARB clock enable
            unsigned int MISC_CKE:1;      //MISC clock enable
        } bCTL;
        volatile unsigned char ucCTL;
    };

//------------------------------
// Macro Define
#define CMU_BASE                  0x00300100

#define CMU_OSCSEL                (*(volatile unsigned char *)(CMU_BASE))

#define CMU_OSCCTL                (*(union CMU_CLKCNTL_tag *)(CMU_BASE+1)).ucCTL
#define CMU_OSC3EN                (*(union CMU_CLKCNTL_tag *)(CMU_BASE+1)).bCTL.SOSC3
#define CMU_OSC1EN                (*(union CMU_CLKCNTL_tag *)(CMU_BASE+1)).bCTL.SOSC1
#define CMU_OSC3WT                (*(union CMU_CLKCNTL_tag *)(CMU_BASE+1)).bCTL.OSCTM

#define CMU_LCLKDIV               (*(volatile unsigned char *)(CMU_BASE+3))

#define CMU_SYSCLKDIV             (*(volatile unsigned char *)(CMU_BASE+5))

#define CMU_CLKSEL                (*(volatile unsigned char *)(CMU_BASE+6))

#define CMU_PLLINDIV              (*(volatile unsigned char *)(CMU_BASE+7))

#define CMU_PLLCTL0               (*(union CMU_PLL0_tag *)(CMU_BASE+8)).ucCTL
#define CMU_PLLN                  (*(union CMU_PLL0_tag *)(CMU_BASE+8)).bCTL.PLLN
#define CMU_PLLV                  (*(union CMU_PLL0_tag *)(CMU_BASE+8)).bCTL.PLLV
#define CMU_PLLPOWR               (*(union CMU_PLL0_tag *)(CMU_BASE+8)).bCTL.PLLPOWR

#define CMU_PLLCTL1               (*(union CMU_PLL1_tag *)(CMU_BASE+9)).ucCTL
#define CMU_PLLVC                 (*(union CMU_PLL1_tag *)(CMU_BASE+9)).bCTL.PLLVC
#define CMU_PLLRS                 (*(union CMU_PLL1_tag *)(CMU_BASE+9)).bCTL.PLLRS

#define CMU_PLLCTL2               (*(union CMU_PLL3_tag *)(CMU_BASE+0xa)).ucCTL
#define CMU_PLLCS                 (*(union CMU_PLL3_tag *)(CMU_BASE+0xa)).bCTL.PLLCS
#define CMU_PLLBYP                (*(union CMU_PLL3_tag *)(CMU_BASE+0xa)).bCTL.PLLBYP
#define CMU_PLLCP                 (*(union CMU_PLL3_tag *)(CMU_BASE+0xa)).bCTL.PLLCP

#define CMU_SSMCON                (*(volatile unsigned char *)(CMU_BASE+0x0c))

#define CMU_SSCG1                 (*(union CMU_SSCG_tag *)(CMU_BASE+0x0d)).ucCTL
#define CMU_SSMCITM               (*(union CMU_SSCG_tag *)(CMU_BASE+0x0d)).bCTL.SSMCITM
#define CMU_SSMCIDT               (*(union CMU_SSCG_tag *)(CMU_BASE+0x0d)).bCTL.SSMCIDT

//#define CMU_USBCLKSEL             (*(volatile unsigned char *)(CMU_BASE+0x0f))

#define CMU_PROTECT               (*(volatile unsigned char *)(CMU_BASE+0x10))

#define CMU_CLKCTL0               (*(union CMU_CLKCTL0_tag *)(CMU_BASE+0x14)).ucCTL
#define CMU_TCLK_SEL              (*(union CMU_CLKCTL0_tag *)(CMU_BASE+0x14)).bCTL.TCLK_SEL
#define CMU_TCLK_CKE              (*(union CMU_CLKCTL0_tag *)(CMU_BASE+0x14)).bCTL.TCLK_CKE
#define CMU_T16A_CKE              (*(union CMU_CLKCTL0_tag *)(CMU_BASE+0x14)).bCTL.T16A_CKE

#define CMU_CLKCTL1               (*(union CMU_CLKCTL1_tag *)(CMU_BASE+0x15)).ucCTL
#define CMU_DMAC_CKE              (*(union CMU_CLKCTL1_tag *)(CMU_BASE+0x15)).bCTL.DMAC_CKE
#define CMU_ITC_CKE               (*(union CMU_CLKCTL1_tag *)(CMU_BASE+0x15)).bCTL.ITC_CKE
#define CMU_I2S_CKE               (*(union CMU_CLKCTL1_tag *)(CMU_BASE+0x15)).bCTL.I2S_CKE
#define CMU_SDMMC_CKE             (*(union CMU_CLKCTL1_tag *)(CMU_BASE+0x15)).bCTL.SDMMC_CKE
#define CMU_PORT_CKE              (*(union CMU_CLKCTL1_tag *)(CMU_BASE+0x15)).bCTL.PORT_CKE
#define CMU_ADC_CKE               (*(union CMU_CLKCTL1_tag *)(CMU_BASE+0x15)).bCTL.ADC_CKE
#define CMU_REMC_CKE              (*(union CMU_CLKCTL1_tag *)(CMU_BASE+0x15)).bCTL.REMC_CKE
#define CMU_NANDIF_CKE            (*(union CMU_CLKCTL1_tag *)(CMU_BASE+0x15)).bCTL.NANDIF_CKE

#define CMU_CLKCTL2               (*(union CMU_CLKCTL2_tag *)(CMU_BASE+0x16)).ucCTL
#define CMU_BCLK_CKE              (*(union CMU_CLKCTL2_tag *)(CMU_BASE+0x16)).bCTL.BCLK_CKE
#define CMU_BCU_CKE               (*(union CMU_CLKCTL2_tag *)(CMU_BASE+0x16)).bCTL.BCU_CKE
#define CMU_USB_CKE               (*(union CMU_CLKCTL2_tag *)(CMU_BASE+0x16)).bCTL.USB_CKE
#define CMU_USBREG_CKE            (*(union CMU_CLKCTL2_tag *)(CMU_BASE+0x16)).bCTL.USBREG_CKE
#define CMU_LCDC_CKE              (*(union CMU_CLKCTL2_tag *)(CMU_BASE+0x16)).bCTL.LCDC_CKE
#define CMU_LCDC_AHB_CKE          (*(union CMU_CLKCTL2_tag *)(CMU_BASE+0x16)).bCTL.LCDC_AHB_CKE
#define CMU_LCDC_SAPB_CKE         (*(union CMU_CLKCTL2_tag *)(CMU_BASE+0x16)).bCTL.LCDC_SAPB_CKE
#define CMU_HIF_CKE               (*(union CMU_CLKCTL2_tag *)(CMU_BASE+0x16)).bCTL.HIF_CKE

#define CMU_CLKCTL3               (*(union CMU_CLKCTL3_tag *)(CMU_BASE+0x17)).ucCTL
#define CMU_USIL_CKE              (*(union CMU_CLKCTL3_tag *)(CMU_BASE+0x17)).bCTL.BCLK_CKE
#define CMU_USI_CKE               (*(union CMU_CLKCTL3_tag *)(CMU_BASE+0x17)).bCTL.BCU_CKE
#define CMU_T16P_CKE              (*(union CMU_CLKCTL3_tag *)(CMU_BASE+0x17)).bCTL.USB_CKE
#define CMU_UART_CKE              (*(union CMU_CLKCTL3_tag *)(CMU_BASE+0x17)).bCTL.USBREG_CKE
#define CMU_T8F_CKE               (*(union CMU_CLKCTL3_tag *)(CMU_BASE+0x17)).bCTL.LCDC_CKE
#define CMU_PSC12_CKE             (*(union CMU_CLKCTL3_tag *)(CMU_BASE+0x17)).bCTL.LCDC_AHB_CKE

#define CMU_CLKCTL4               (*(union CMU_CLKCTL3_tag *)(CMU_BASE+0x18)).ucCTL
#define CMU_WDT_CKE               (*(union CMU_CLKCTL3_tag *)(CMU_BASE+0x18)).bCTL.WDT_CKE
#define CMU_RTC_SAPB_CKE          (*(union CMU_CLKCTL3_tag *)(CMU_BASE+0x18)).bCTL.RTC_SAPB_CKE
#define CMU_A3RAM_CKE             (*(union CMU_CLKCTL3_tag *)(CMU_BASE+0x18)).bCTL.A3RAM_CKE
#define CMU_IVRAM_ARB_CKE         (*(union CMU_CLKCTL3_tag *)(CMU_BASE+0x18)).bCTL.IVRAM_ARB_CKE
#define CMU_MISC_CKE              (*(union CMU_CLKCTL3_tag *)(CMU_BASE+0x18)).bCTL.MISC_CKE

#ifdef __cplusplus
}
#endif
#endif                          //_CMU_REG_H
