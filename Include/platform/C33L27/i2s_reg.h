/****************************************************************************
 * 	Copyright (C) SEIKO EPSON CORP. 2010
 *
 * 	File Name   : i2s_reg.h
 * 	Description : S1C33L27 I2S register definition file
 *
 * 	Revision History:
 *  2009.12.31  Joan.Lin     New creation
 ****************************************************************************/

#ifndef _I2S_REG_H
#define _I2S_REG_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 * 	Structures (typedef, struct, union)
 ****************************************************************************/
/********************************************************************/
/* [ I2S output control reg  ] I2S_Out_CONTRL          REG[0x00]    */
/********************************************************************/
    union I2S_OUT_CONTRL_tag {
        volatile struct {
            unsigned int CHMD:2;        //Output channel mode
            unsigned int DTTMG:2;       //Output data timing
            unsigned int OUTEN:1;       //I2S Output Enable
            unsigned int DTFORM:1;      //Output data MSB/LSB format
            unsigned int BCLKPOL:1;     //Output bit clock polarity
            unsigned int WCLKMD:1;      //Output word clock mode
            unsigned int I2SEN:1;       //Output I2S enable
            unsigned int DATRES:1;      //Output data resolution
            unsigned int DTSIGN:1;      //Output signed/unsigned mode
        } bCTL;
        volatile struct {
            unsigned int OUTL:1;        //Output channel L (1: mute, 0: sound)
            unsigned int OUTR:1;        //Output channel R (1: mute, 0: sound)
        } bCTL_OUTMD;
        volatile unsigned short usCTL;
    };

/********************************************************************/
/* [ I2S input control reg  ] I2S_In_CONTRL            REG[0x04]    */
/********************************************************************/
    union I2S_IN_CONTRL_tag {
        volatile struct {
            unsigned int INEN:1;        //Input Enable
            unsigned int DATRES:1;      //Input data resolution
            unsigned int DTTMG:2;       //Input data timing
            unsigned int BCLKPOL:1;     //Input bit clock polarity
            unsigned int WCLKMD:1;      //Input word clock mode
            unsigned int INBYPASS:1;    //Input bypass mode
        } bCTL;
        volatile unsigned short usCTL;
    };

/********************************************************************/
/* [ I2S MCLK divide ratio reg  ] I2S_MCLK_DIV            REG[0x08] */
/********************************************************************/
    union I2S_MCLK_DIV_tag {
        volatile struct {
            unsigned int MCLKDIV:6;     //I2S MCLK divide ratio
            unsigned int DUMMY:9;       //
            unsigned int MCLKSEL:1;     //I2S MCLK source clock
        } bCTL;
        volatile unsigned short usCTL;
    };

/********************************************************************/
/* [ I2S Audio Clock divide ratio reg  ] I2S_AUDIO_DIV    REG[0x0c] */
/********************************************************************/
    union I2S_AUDIO_DIV_tag {
        volatile struct {
            unsigned int BCLKDIV:8;     //I2S output bit divide ratio
            unsigned int WSCLKCYCOUT:5; //I2S output WS clock cycle
            unsigned int DUMMY:3;       //
            unsigned int WSCLKCYCIN:5;  //I2S input WS clock cycle
        } bCTL;
        volatile unsigned long ulCTL;
    };

/********************************************************************/
/* [ I2S start reg ] I2S_START                         REG[0x10]    */
/********************************************************************/
    union I2S_START_tag {
        volatile struct {
            unsigned int OUTSTART:1;    //I2S output start
            unsigned int DUMMY:6;       //
            unsigned int BUSY:1;        //I2S busy flag
            unsigned int INSTART:1;     //I2S input start
        } bCTL;
        volatile unsigned short usCTL;
    };

/********************************************************************/
/* [ I2S FIFO Status reg ] I2S_FIFOSTATUS              REG[0x14]    */
/********************************************************************/
    union I2S_FIFOSTATUS_tag {
        volatile struct {
            unsigned int OUTFIFOEF:1;   //Output FIFO empty flag
            unsigned int OUTFIFOFF:1;   //Output FIFO full flag
            unsigned int OUTFSM:3;      //Output FIFO fsm
            unsigned int DUMMY:3;       //
            unsigned int INFIFOEF:1;    //Input FIFO empty flag
            unsigned int INFIFOFF:1;    //Input FIFO full flag
        } bCTL;
        volatile unsigned short usCTL;
    };

/********************************************************************/
/* [ I2S Interrupt mode reg ] I2S_INTMOD               REG[0x18]    */
/********************************************************************/
    union I2S_INTMOD_tag {
        volatile struct {
            unsigned int OUTOEINTEN:1;  //Output one-empty enable
            unsigned int OUTHEINTEN:1;  //Output half-empty enable
            unsigned int OUTWEINTEN:1;  //Output whole-empty enable
            unsigned int DUMMY0:1;      //
            unsigned int INOFINTEN:1;   //Input one-full enable
            unsigned int DUMMY3:1;      //
            unsigned int INWFINTEN:1;   //Input whole-full enable
            unsigned int DUMMY1:1;      //
            unsigned int OUTOEINTF:1;   //Output one-empty flag
            unsigned int OUTHEINTF:1;   //Output half-empty flag
            unsigned int OUTWEINTF:1;   //Output whole-empty flag
            unsigned int DUMMY2:1;      //
            unsigned int INOFINTF:1;    //Input one-full flag
            unsigned int DUMMY4:1;      //
            unsigned int INWFINTF:1;    //Input whole-full flag
        } bCTL;
        volatile unsigned short usCTL;
    };

/********************************************************************/
/* [ I2S Pin & DMA source reg ] I2S_PIN_DMA            REG[0x1c]    */
/********************************************************************/
    union I2S_PIN_DMA_tag {
        volatile struct {
            unsigned int PINSEL:1;      //I2S pin connection select
            unsigned int DUMMY:7;       //
            unsigned int OUTDMA:1;      //Output DMA mode select
            unsigned int INDMA:1;       //Input DMA mode seletct
        } bCTL;
        volatile unsigned short usCTL;
    };

//------------------------------
#define I2S_BASE    0x00302400

// 0x302400
#define I2S_OUT_CONTRL          (*(union I2S_OUT_CONTRL_tag *)(I2S_BASE)).usCTL
#define I2S_OUT_CHMD            (*(union I2S_OUT_CONTRL_tag *)(I2S_BASE)).bCTL.CHMD
#define I2S_OUT_DTTMG           (*(union I2S_OUT_CONTRL_tag *)(I2S_BASE)).bCTL.DTTMG
#define I2S_OUT_EN              (*(union I2S_OUT_CONTRL_tag *)(I2S_BASE)).bCTL.OUTEN
#define I2S_OUT_DTFORM          (*(union I2S_OUT_CONTRL_tag *)(I2S_BASE)).bCTL.DTFORM
#define I2S_OUT_BCLKPOL         (*(union I2S_OUT_CONTRL_tag *)(I2S_BASE)).bCTL.BCLKPOL
#define I2S_OUT_WCLKMD          (*(union I2S_OUT_CONTRL_tag *)(I2S_BASE)).bCTL.WCLKMD
#define I2S_OUT_I2SEN           (*(union I2S_OUT_CONTRL_tag *)(I2S_BASE)).bCTL.I2SEN
#define I2S_OUT_DATRES          (*(union I2S_OUT_CONTRL_tag *)(I2S_BASE)).bCTL.DATRES
#define I2S_OUT_DTSIGN          (*(union I2S_OUT_CONTRL_tag *)(I2S_BASE)).bCTL.DTSIGN

#define I2S_OUTMD_L             (*(union I2S_OUT_CONTRL_tag *)(I2S_BASE)).bCTL_OUTMD.OUTL
#define I2S_OUTMD_R             (*(union I2S_OUT_CONTRL_tag *)(I2S_BASE)).bCTL_OUTMD.OUTR

// 0x302404
#define I2S_IN_CONTRL           (*(union I2S_IN_CONTRL_tag *)(I2S_BASE+0x04)).usCTL
#define I2S_IN_EN               (*(union I2S_IN_CONTRL_tag *)(I2S_BASE+0x04)).bCTL.INEN
#define I2S_IN_DATRES           (*(union I2S_IN_CONTRL_tag *)(I2S_BASE+0x04)).bCTL.DATRES
#define I2S_IN_DTTMG            (*(union I2S_IN_CONTRL_tag *)(I2S_BASE+0x04)).bCTL.DTTMG
#define I2S_IN_BCLKPOL          (*(union I2S_IN_CONTRL_tag *)(I2S_BASE+0x04)).bCTL.BCLKPOL
#define I2S_IN_WCLKMD           (*(union I2S_IN_CONTRL_tag *)(I2S_BASE+0x04)).bCTL.WCLKMD
#define I2S_IN_BYPASS           (*(union I2S_IN_CONTRL_tag *)(I2S_BASE+0x04)).bCTL.INBYPASS

// 0x302408
#define I2S_DV_MCLK             (*(union I2S_MCLK_DIV_tag *)(I2S_BASE+0x08)).usCTL
#define I2S_MCLKDIV             (*(union I2S_MCLK_DIV_tag *)(I2S_BASE+0x08)).bCTL.MCLKDIV
#define I2S_MCLKSEL             (*(union I2S_MCLK_DIV_tag *)(I2S_BASE+0x08)).bCTL.MCLKSEL

// 0x30240c
#define I2S_DV_BCLK             (*(union I2S_AUDIO_DIV_tag *)(I2S_BASE+0x0c)).ulCTL
#define I2S_BCLKDIV             (*(union I2S_AUDIO_DIV_tag *)(I2S_BASE+0x0c)).bCTL.BCLKDIV
#define I2S_WSCLKCYCOUT         (*(union I2S_AUDIO_DIV_tag *)(I2S_BASE+0x0c)).bCTL.WSCLKCYCOUT
#define I2S_WSCLKCYCIN          (*(union I2S_AUDIO_DIV_tag *)(I2S_BASE+0x0c)).bCTL.WSCLKCYCIN

// 0x302410
#define I2S_START               (*(union I2S_START_tag *)(I2S_BASE+0x10)).usCTL
#define I2S_OUTSTART            (*(union I2S_START_tag *)(I2S_BASE+0x10)).bCTL.OUTSTART
#define I2S_BUSY                (*(union I2S_START_tag *)(I2S_BASE+0x10)).bCTL.BUSY
#define I2S_INSTART             (*(union I2S_START_tag *)(I2S_BASE+0x10)).bCTL.INSTART

// 0x302414
#define I2S_STATUS              (*(union I2S_FIFOSTATUS_tag *)(I2S_BASE+0x14)).usCTL
#define I2S_OUTFIFOEF           (*(union I2S_FIFOSTATUS_tag *)(I2S_BASE+0x14)).bCTL.OUTFIFOEF
#define I2S_OUTFIFOFF           (*(union I2S_FIFOSTATUS_tag *)(I2S_BASE+0x14)).bCTL.OUTFIFOFF
#define I2S_OUTFIFOSTAT         (*(union I2S_FIFOSTATUS_tag *)(I2S_BASE+0x14)).bCTL.OUTFSM
#define I2S_INFIFOEF            (*(union I2S_FIFOSTATUS_tag *)(I2S_BASE+0x14)).bCTL.INFIFOEF
#define I2S_INFIFOFF            (*(union I2S_FIFOSTATUS_tag *)(I2S_BASE+0x14)).bCTL.INFIFOFF

// 0x302418
#define I2S_INT                 (*(union I2S_INTMOD_tag *)(I2S_BASE+0x18)).usCTL
#define I2S_OUTOEIE             (*(union I2S_INTMOD_tag *)(I2S_BASE+0x18)).bCTL.OUTOEINTEN
#define I2S_OUTHEIE             (*(union I2S_INTMOD_tag *)(I2S_BASE+0x18)).bCTL.OUTHEINTEN
#define I2S_OUTWEIE             (*(union I2S_INTMOD_tag *)(I2S_BASE+0x18)).bCTL.OUTWEINTEN
#define I2S_INOFIE              (*(union I2S_INTMOD_tag *)(I2S_BASE+0x18)).bCTL.INOFINTEN
#define I2S_INWFIE              (*(union I2S_INTMOD_tag *)(I2S_BASE+0x18)).bCTL.INWFINTEN
#define I2S_OUTOEIF             (*(union I2S_INTMOD_tag *)(I2S_BASE+0x18)).bCTL.OUTOEINTF
#define I2S_OUTHEIF             (*(union I2S_INTMOD_tag *)(I2S_BASE+0x18)).bCTL.OUTHEINTF
#define I2S_OUTWEIF             (*(union I2S_INTMOD_tag *)(I2S_BASE+0x18)).bCTL.OUTWEINTF
#define I2S_INOFIF              (*(union I2S_INTMOD_tag *)(I2S_BASE+0x18)).bCTL.INOFINTF
#define I2S_INWFIF              (*(union I2S_INTMOD_tag *)(I2S_BASE+0x18)).bCTL.INWFINTF

// 0x30241c
#define I2S_MODE                (*(union I2S_PIN_DMA_tag *)(I2S_BASE+0x1c)).usCTL
#define I2S_PIN                 (*(union I2S_PIN_DMA_tag *)(I2S_BASE+0x1c)).bCTL.PINSEL
#define I2S_OUTDMA              (*(union I2S_PIN_DMA_tag *)(I2S_BASE+0x1c)).bCTL.OUTDMA
#define I2S_INDMA               (*(union I2S_PIN_DMA_tag *)(I2S_BASE+0x1c)).bCTL.INDMA

// 0x302420
#define I2S_OUT_FIFO_L          (I2S_BASE+0x20)  //16-bit FIFO address
#define I2S_OUT_FIFO_R          (I2S_BASE+0x22)
#define I2S_OUT_FIFO            (I2S_BASE+0x20)  //24-bit FIFO address

#define I2S_IN_FIFO_L           (I2S_BASE+0x30)  //16-bit FIFO address
#define I2S_IN_FIFO_R           (I2S_BASE+0x32)
#define I2S_IN_FIFO             (I2S_BASE+0x30)) //24-bit FIFO address

#ifdef __cplusplus
}
#endif
#endif                          //_I2S_REG_H
