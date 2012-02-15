/********************************************************
*
*Author:  <sftan@ingenic.cn>
*
*********************************************************/

#ifndef __JZ4760CODEC_H__
#define __JZ4760CODEC_H__


#define CODEC_SR     0 /* status register */
#define CODEC_AICR   1  /*audio interface control*/
#define CODEC_CR1    2  /* control register 1*/
#define CODEC_CR2    3  /* control register 2*/
#define CODEC_CR3    4	/* control register 3*/
#define CODEC_CR4    5	/* control register 4*/
#define CODEC_CCR1   6  /* control clock register 1*/
#define CODEC_CCR2   7  /* control clock register 2*/
#define CODEC_PMR1   8  /* power mode register 1 */
#define CODEC_PMR2   9  /* power mode register 2*/
#define CODEC_ICR    10  /* interrupt control register*/
#define CODEC_IFR    11 /* interrupt flag register */
#define CODEC_CGR1   12 /* control gain register 1*/
#define CODEC_CGR2   13 /* control gain register 2*/
#define CODEC_CGR3   14 /* control gain register 3*/
#define CODEC_CGR4   15 /* control gain register 4*/
#define CODEC_CGR5   16 /* control gain register 5*/
#define CODEC_CGR6   17 /* control gain register 6*/
#define CODEC_CGR7   18 /* control gain register 7*/
#define CODEC_CGR8   19 /* control gain register 8*/
#define CODEC_CGR9   20 /* control gain register 9*/
#define CODEC_AGC1   21 /* automatic gain control 1*/
#define CODEC_AGC2   22 /* automatic gain control 2*/
#define CODEC_AGC3   23 /* automatic gain control 3*/
#define CODEC_AGC4   24 /* automatic gain control 4*/
#define CODEC_AGC5   25 /* automatic gain control 5*/
#define CODEC_MIX1   26 /* digital mixer 1*/
#define CODEC_MIX2   27 /* digital mixer 2*/
#define CODEC_TR1    28 /* test register 1*/
#define CODEC_TR2    29 /* test register 2*/
#define CODEC_TR3    30 /* test register 3*/
#define CODEC_TR4    31 /* test register 4*/


/*SR  status register */

#define ACK_PON  ( 1<<7 )
#define ACK_IRQ  ( 1<<6 )   

/*AICR audio interface control register*/
#define DAC_ADWL(n) (n << 6)
#define ADC_ADWL(n) (n << 4)
#define DAC_SERIAL  (1 << 3)
#define ADC_SERIAL  (1 << 2)
#define DAC_I2S     (1 << 1)
#define ADC_I2S     (1 << 0)

/* CR1 control register 1*/
#define LOAD         (1 << 7)
#define HP_MUTE			 (1 << 5)
#define LINEOUT_MUTE (1 << 4)
#define BTL_MUTE		 (1 << 3)
#define OUTSEL(n)    (n << 0)


/*CR2 control register 2*/
#define MONO  				(1 << 7)
#define DAC_MUTE 			(1 << 5)
#define NOMAD  				(1 << 1)
#define DAC_R_ONLY 		(1 << 0)

/*CR3 control register 3*/
#define INSEL(n)       (n << 2)
#define MICSTEREO      (1 << 1)
#define MICDIFF        (1 << 0) 

/*CR4 control register 4 */
#define ADC_HPF        (1 << 7)
#define ADC_R_ONLY     (1 << 0)


/* CCR1 control clock register 1*/
#define CRYSTAL(n)      (n << 0)

/* CCR2 control clock register 2*/
#define DAC_FREQ(n)      (n << 4)
#define ADC_FREQ(n)			 (n << 0)

/*PMR1 power mode register 1*/
#define SB					(1 << 7)
#define SB_SLEEP		(1 << 6)
#define SB_AIP			(1 << 5)
#define SB_LINE			(1 << 4)
#define SB_MIC1     (1 << 3)
#define SB_MIC2			(1 << 2)
#define SB_BYPASS   (1 << 1)
#define SB_MICBIAS  (1 << 0)

/* PMR2 power mode register 2*/
#define SB_ADC			(1 << 4)
#define SB_HP 			(1 << 3)
#define SB_BTL			(1 << 2)
#define SB_LOUT 		(1 << 1)
#define SB_DAC			(1 << 0)

/*ICR interrupt control register */
#define INT_FORM(n)	(n << 6)
#define JACK_MASK 	(1 << 5)
#define SCMC_MASK 	(1 << 4)
#define RUP_MASK		(1 << 3)
#define RDO_MASK 		(1 << 2)
#define GUP_MASK		(1 << 1)
#define GDO_MASK 		(1 << 0)

/* IFR interrupt flag register */
#define JACK 				(1 << 6)
#define JACK_EVENT 	(1 << 5)
#define SCMC 				(1 << 4)
#define RUP					(1 << 3)
#define RDO					(1 << 2)
#define GUP					(1 << 1)
#define GDO					(1 << 0)

/*CGR1 control gain register 1*/
#define RLGO     		(1 << 7)
#define GOR(n)			(n << 0)

/*CGR2 control gain register 2*/
#define GOL(n)			(n << 0)

/* CGR3 control gain register 3*/
#define RLGI				(1 << 7)
#define GIR(n)			(n << 0)

/*CGR4 control gain register 4*/
#define GIL(n)			(n << 0)

/*CGR5 control gain register 5*/
#define RLGOD 			(1 << 7)
#define GODR(n)			(n << 0)

/*CGR6 control gain register 6*/
#define GODL(n)			(n << 0)


/*CGR7 control gain register 7*/
#define GIM1(n)			(n << 3)
#define GIM2(n)			(n << 0)

/*CGR8 control gain register 8*/
#define GLGID				(1 << 7)
#define GIDR(n)			(n << 0)

/* CGR9 control gain register 9*/
#define GIDL(n)			(n << 0)

/* AGC1 automatic gain control register 1*/
#define AGC_EN 			(1 << 7)
#define TARGET(n)		(n << 2)


/* AGC2 automatic gain control register 2*/
#define NG_EN 			(1 << 7)
#define NG_THR(n)		(n << 4)
#define HOLD(n)			(n << 0)

/* AGC3 automatic gain control register 3*/
#define ATK(n)			(n << 4)
#define DCY(n)			(n << 0)

/* AGC4 automatic gain control register 4*/		
#define AGC_MAX(n)	(n << 0)

/* AGC5 automatic gain control register 5*/	
#define AGC_MIN(n)	(n << 0)


/*MIX1 digital mixer 1*/
#define MIX_REC(n)	(n << 6)
#define GIMIX(n)		(n << 0)

/* MIX2 digital mixer 2*/	
#define DAC_MIX(n)	(n << 6)
#define GOMIX(n)		(n << 0)


/* Audio Sample Format */
#define	AFMT_U8			8
#define AFMT_S16_LE		16

/* PCM ioctl command */
#define PCM_SET_SAMPLE_RATE	  0
#define PCM_SET_CHANNEL		    1
#define PCM_SET_FORMAT		    2
#define PCM_SET_VOL		        3
#define PCM_GET_VOL		        4
#define PCM_GET_SPACE         5
#define PCM_SET_HP_VOL		    6
#define PCM_GET_HP_VOL		    7
#define PCM_SET_PAUSE		      8
#define PCM_SET_PLAY		      9
#define PCM_RESET		          10
#define PCM_SET_REPLAY		    11
#define PCM_SET_RECORD		    12
#define PCM_SET_MUTE		      13
#define PCM_SET_EQPLAY        14
#define PCM_SET_VOLFUNCTION   15
#define PCM_GET_SAMPLE_MAX    16
#define PCM_SET_RECORD_FM	    17 
#define PCM_REINIT            18
#define PCM_DATA_FINISH       19
#define PCM_POWER_ON          20
#define PCM_POWER_OFF         21

#define PCM_BYPASS_LH         22 /* bypass line to headphone*/
#define PCM_BYPASS_LL         23 /* bypass line to line out */
#define PCM_BYPASS_LD         24 /* bypass line to differential line out*/
#define PCM_BYPASS_LB         25 /* bypass line to BTL out */

#define PCM_SIDETONE_SR       26 /* stereo mic to 16Ohm headphone out mic1 to right channel */
#define PCM_SIDETONE_SB       27 /* stereo mic to BTL out mic2 to right channel */

/* replay set macro*/
#define DAC_TO_HP     1
#define DAC_TO_LOUT   2
#define DAC_TO_BTL    3 

/* record set macro*/
#define MONO_MIC1_IN  1
#define LINE_IN       2
#define STEREO_IN     3 


#endif 
