/**
 * @file SdConfig.h
 *
 * @brief Anyka Sound Device third part interfaces definition header file.
 *
 * This file define Anyka Sound Device data type and macro. 
 * Copyright (C) 2005 Anyka (GuangZhou) Software Technology Co., Ltd.
 * @author 
 * @date 2005-01-24
 * @version 1.0
 * @ref
 */

#ifndef __SOUND_DEVICE_CONFIG_H__
#define	__SOUND_DEVICE_CONFIG_H__

/**
 * The following is configure definition
 */

/** @{@name Define ADS compile interfaces */

//#define _SD_ADS_COMPILE
/** @} */

//////////////////////////////////////////////////////////////////////
//  Define implemented media type: MIDI, MP3, AMR, WAVE, WMA or PCM 
//  1 = support
//  0 = not support
/////////////////////////////////////////////////////////////////////

/** @{@name Type definition */
/**	Unsigned 8 bits integer */
typedef unsigned char				_SD_T_U8;	 
/**	Unsigned 16 bits integer */
typedef unsigned short				_SD_T_U16;	
/**	Unsigned 32 bits integer */
typedef unsigned long				_SD_T_U32;	 
/**	Signed 8 bits integer */
typedef signed char					_SD_T_S8;	
/**	Signed 16 bits integer */
typedef signed short				_SD_T_S16;	 
/**	Signed 32 bits integer */
typedef signed long					_SD_T_S32;	
/**	The "void" type in ANSI c */
//typedef void						_SD_T_VOID;	
/**	Boolean value, define as unsigned 8 bits integer */
typedef unsigned char				_SD_T_BOOL;	 
/**	The "char *" type in ANSI c for string */
typedef char *                      _SD_T_pCSTR;

/**	Define inline */
#define _SD_INLINE					__inline	
/** @} */

/** @{@name Define the Sound Data Buffer */
/**	Use this flag to identicate The total length of Sound Device Buffer */
#define _SD_DATA_BUFF_LEN			65536	
/**	Use this flag to identicate The minimum length when read Sound Data */
#define _SD_DATA_UNIT_BUFF_LEN		8192
/**	Use this flag to identicate TThe data length read in the first time */
#define _SD_DATA_FIRST_BUFF_LEN		32768
/**	Use this flag to identicate The left data length when end */
#define _SD_DATA_END_BUFF_LEN		8	
/**	Use this flag to identicate Temporary buffer length */ 
#define _SD_DATA_TEMP_BUFF_LEN		8192	
/** @remark _SD_DATA_BUFF_LEN must be a multiple of _SD_DATA_UNIT_BUFF_LEN.
 * _SD_DATA_FIRST_BUFF_LEN must be a multiple of _SD_DATA_UNIT_BUFF_LEN and less than _SD_DATA_BUFF_LEN.
 * _SD_DATA_END_BUFF_LEN must less than _SD_DATA_BUFF_LEN.
 * _SD_DATA_TEMP_BUFF_LEN must less than _SD_DATA_FIRST_BUFF_LEN.
 * _SD_MAX_AUDIO_NUM used to limit the multi-play file counts
* @} */ 

#define	_SD_REC_DATA_BUFF_LEN	16384
#define	_SD_REC_ONCE_DATA_LEN	320
/** Define maximum audio number for multi-play */
#define _SD_MAX_AUDIO_NUM			20		
/*updata pcm len*/
#define WAIT_OUTPCM_LEN         10240//	20k byte 

/**for MIDI debug, if do not debug, please comment it off. */
//#define __INT15_DEBUG_SUPPORT__

/** @{@name Set interrupt mode */
/**	Use this flag to identicate DSP cause a HOST interrupt 
* when DSP needs HOST to do something. Host call interrupt handle function in loop. */
#define _SD_USE_INTR_POLLING_MODE	0	
/**	Use this flag to identicate DSP cause a HOST interrupt
* when DSP needs HOST to do something. Host call interrupt handle function in interrupt. */
#define _SD_USE_INTR_ONLY_MODE		1	// 
/**	Use this flag to identicate DSP set a flag in DSP DM 
* when DSP needs HOST to do something, HOST need to polling the flag. 
* When find the flag is set, the HOST transmits data to DSP. */
#define _SD_USE_POLLING_MODE		0	
/** @} */

/** if dsp debug, can check sample is right or not*/
//#define	_MP3_DSP_CHECKSUM          

/** @{@name The following is specially for AK3223M platform */
/**	Use this flag to identicate Support file system or not */
	#define _SD_SUPPORT_FILE_SYSTEM			
/**	Use this flag to define choose register */
	#define	_SD_ARM_REGISTER			*(volatile unsigned  long *)
/**	Use this flag to define host side working clock */
	#define	_SD_HOST_WORKING_CLOCK		60000000	
/** @} */

/* @{@name Enable / disable interrupt. int_bits: AK3210M Interrupt-Mask-Register bits */
/**	Enable  interrupt*/
#define _SD_INTR_ENABLE(int_bits)	*(volatile unsigned long *)_SD_INTR_MASK_REG &= ~(int_bits)
/**	disable interrupt */
#define _SD_INTR_DISABLE(int_bits)	*(volatile unsigned long *)_SD_INTR_MASK_REG |= (int_bits)
/**	Use this flag to define Interrupt Mask Register*/

#ifdef _SD_RVDS_COMPILE
#define _SD_INTR_MASK_REG			0xb0000018
#else
#define _SD_INTR_MASK_REG			0x20000018
#endif

/**	Use this flag to define APX DSP */
#define _SD_INT_APX_BIT				0x0020
/** @} */

/** @{@name Define file system interfaces */
#ifdef _SD_SUPPORT_FILE_SYSTEM
/**	Use this flag to define file handle */
	#define _SD_T_FILE					_SD_T_U32	
/**	Use this flag to define file seek origin */
	#define _SD_FSEEK_SET				0			 
/**	Use this flag to define file seek current position of file pointer */
	#define _SD_FSEEK_CUR				1
/**	Use this flag to define file seek end */
	#define _SD_FSEEK_END				2
#endif	
/** @} */
/** @{@name Define ADS compile interfaces */
#define DEBUG_AUDIO
#define _SD_DEBUG_OUTPUT					\
	if(_SD_CB_Funtions.printf != _SD_NULL)	\
		_SD_CB_Funtions.printf

//#define ASSIGN_CHIP_TYPE //if assign CHIP type before compile, open this macro

#ifdef ASSIGN_CHIP_TYPE
/** Use this flag to partition AK3223 */
//#define _SD_CHIPTYPE_AK3223
/** Use this flag to partition AK3224 */
//#define _SD_CHIPTYPE_AK3224
/** Use this flag to partition AK322L */
//#define _SD_CHIPTYPE_AK322L
#ifdef _SD_CHIPTYPE_AK322L
    #define _SD_CHIPTYPE_AK3224
#endif
/** Use this flag to support AK3610, AK3620 */
#define _SD_CHIPTYPE_AK3610
/** Use this flag to support AK3631L, also for sundance2 chip */
//#define _SD_CHIPTYPE_AK3631

#endif //ASSIGN_CHIP_TYPE

/* To avoid headphone pi_pa noise */
#define _SD_HP_PIPA_CONTROL

/** Use external DA/AD chip thu I2S */
//#define _SD_USE_I2S_DA
//#define _SD_USE_I2S_AD

/* Do change work clock or not */
#define _SD_CHANGE_WORKCLOCK

/* @{@name Define audio version*/
/**	Use this to define version string */	
#define AUDIO_VERSION_STRING		(_SD_T_S8 *)"Audio Version V2.4.4bch3v4_svn2176"
/**	Use this to define version number */
#define AUDIO_VERSION_NUMBER		0x020404303
/** @} */

#endif  
