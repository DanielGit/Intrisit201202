/****************************************************************************
 * 	Copyright (C) SEIKO EPSON CORP. 2010
 *
 * 	File Name   : adc_reg.h
 * 	Description : S1C33L27 ADC register definition file
 *
 * 	Revision History:
 *  2010/01/28   Joan Lin     New creation
 ****************************************************************************/
#ifndef _ADC_REG_H
#define _ADC_REG_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 * 	Structures (typedef, struct, union)
 ****************************************************************************/
/********************************************************************/
/* [ A/D Trigger/Channel Select Register ] ADC10_TRG 0x0030_1302    */
/********************************************************************/
    union ADC10_TRG_tag {
        volatile struct {
            unsigned int ADST:3;        //Sampling time setting
            unsigned int DUMMY1:1;
            unsigned int ADTS:2;        //Conversion trigger select
            unsigned int ADMS:1;        //Conversion mode select
            unsigned int STMD:1;        //Conversion result storing mode
            unsigned int ADCS:3;        //Start channel select
            unsigned int ADCE:3;        //End channel select
            unsigned int DUMMY2:2;
        } bCTL;
        volatile unsigned short ucCTL;
    };

/********************************************************************/
/* [ A/D Control/Status Register ] ADC10_CTL         0x0030_1304    */
/********************************************************************/
    union ADC10_CTL_tag {
        volatile struct {
            unsigned int ADEN:1;        //ADC10 enable
            unsigned int ADCTL:1;       //A/D conversion control
            unsigned int DUMMY1:2;
            unsigned int ADCIE:1;       //Conversion completion int. enable
            unsigned int ADOIE:1;       //Overwrite error interrupt enable
            unsigned int DUMMY2:2;
            unsigned int ADCF:1;        //Conversion completion flag
            unsigned int ADOWE:1;       //Overwrite error flag
            unsigned int ADIBS:1;       //ADC10 status
            unsigned int DUMMY3:1;
            unsigned int ADICH:3;       //Conversion channel indicator
            unsigned int DUMMY4:1;
        } bCTL;
        volatile unsigned short ucCTL;
    };

//------------------------------
// Macro Define
//------------------------------
#define ADC_BASE                    0x00301300

#define ADC10_ADD                   (*(volatile unsigned short *)(ADC_BASE))

#define ADC10_TRG                   (*(union ADC10_TRG_tag *)(ADC_BASE+2)).ucCTL
#define ADC10_ADST                  (*(union ADC10_TRG_tag *)(ADC_BASE+2)).bCTL.ADST
#define ADC10_ADTS                  (*(union ADC10_TRG_tag *)(ADC_BASE+2)).bCTL.ADTS
#define ADC10_ADMS                  (*(union ADC10_TRG_tag *)(ADC_BASE+2)).bCTL.ADMS
#define ADC10_STMD                  (*(union ADC10_TRG_tag *)(ADC_BASE+2)).bCTL.STMD
#define ADC10_ADCS                  (*(union ADC10_TRG_tag *)(ADC_BASE+2)).bCTL.ADCS
#define ADC10_ADCE                  (*(union ADC10_TRG_tag *)(ADC_BASE+2)).bCTL.ADCE

#define ADC10_CTL                   (*(union ADC10_CTL_tag *)(ADC_BASE+4)).ucCTL
#define ADC10_ADEN                  (*(union ADC10_CTL_tag *)(ADC_BASE+4)).bCTL.ADEN
#define ADC10_ADCTL                 (*(union ADC10_CTL_tag *)(ADC_BASE+4)).bCTL.ADCTL
#define ADC10_ADCIE                 (*(union ADC10_CTL_tag *)(ADC_BASE+4)).bCTL.ADCIE
#define ADC10_ADOIE                 (*(union ADC10_CTL_tag *)(ADC_BASE+4)).bCTL.ADOIE
#define ADC10_ADCF                  (*(union ADC10_CTL_tag *)(ADC_BASE+4)).bCTL.ADCF
#define ADC10_ADOWE                 (*(union ADC10_CTL_tag *)(ADC_BASE+4)).bCTL.ADOWE
#define ADC10_ADIBS                 (*(union ADC10_CTL_tag *)(ADC_BASE+4)).bCTL.ADIBS
#define ADC10_ADICH                 (*(union ADC10_CTL_tag *)(ADC_BASE+4)).bCTL.ADICH

#define ADC10_CLK                   (*(volatile unsigned short *)(ADC_BASE+6))

#ifdef __cplusplus
}
#endif
#endif                          //_ADC_REG_H
