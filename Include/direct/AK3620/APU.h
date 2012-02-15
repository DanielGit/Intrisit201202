/**
 * @file	SoundDevice.h
 * @brief	Anyka Sound Device Module interfaces header file.
 *
 * This file declare Anyka Sound Device Module interfaces.\n
 * Copyright (C) 2005 Anyka (GuangZhou) Software Technology Co., Ltd.
 * @author	Huang Lixin
 * @date	2005-01-24
 * @version 1.0
 * @ref
 */

#ifndef __SOUND_DEVICE_H__
#define __SOUND_DEVICE_H__

#include "APUGlobal.h"

/** @defgroup AUDIOLIB Audio library 
 * @ingroup ENG
 */
/*@{*/


/**
 * @brief Set anyka chip type,such as AK3223/AK3224/3610/3631/322L
 * Should be called before _AKSD_init()
 * functions will not work correctly.
 * @author 
 * @date 
 * @param _SD_CHIP_ID, chip id:_SD_CHIP_AK3224,_SD_CHIP_AK3610,_SD_CHIP_AK3631L,_SD_CHIP_AK322L
 * @return void
 * @retval Others: refer to Anyka sound device error code in sdglobal.h.
 */

void _AKSD_SetChipID(_SD_CHIP_ID id);

/**
 * @brief	Initialize A2 internal hardware device and resource.
 *
 * Perform all the necessary initializations.
 * Should be called after system boot-up and before any reproduction, otherwise, other
 * functions will not work correctly.
 * @author	Huang Lixin
 * @date	2004-09-14
 * @param	void
 * @return	_SD_T_U16
 * @retval	0x0000 Successful
 * @retval	Others refer to Anyka sound device error code in sdglobal.h.
 */
_SD_T_U16 _AKSD_Init(void);

/**
 * @brief	Release the hardware resource.
 *
 * Should be called before system terminates.
 * @author	Huang Lixin
 * @date	2004-09-14
 * @param	void
 * @return	_SD_T_U16
 * @retval	0x0000 Successful
 * @retval	Others refer Anyka sound device error code in sdglobal.h.
 */
_SD_T_U16 _AKSD_Free(void);



/**
 * @brief	Set sound player's volume.
 *
 * The default value is _SD_VOLUME_MAX/2.
 * This function is available when the music is playing.
 * @author	Huang Lixin
 * @date	2004-09-14
 * @param	[in] volume The volume value to be set. The value should between
 * _SD_VOLUME_MIN and _SD_VOLUME_MAX, which are defined as:
 * _SD_VOLUME_MIN 0
 * _SD_VOLUME_MAX 0x7fff
 * _SD_VOLUME_DEF _SD_VOLUME_MAX
 * The default volume is _SD_VOLUME_DEF.
 * @return	_SD_T_U16
 * @retval	0x0000 Successful
 * @retval	Others refer to Anyka sound device error code in sdglobal.h.
 */
_SD_T_U16  _AKSD_SetVolume(_SD_T_U16 volume);
_SD_T_U16  _AKSD_SetDigitalVolume(_SD_T_U16 volume);

/**
 * @brief	Set AD volume.
 *
 * The default value is 0x100.
 * @author	Huang Lixin
 * @date	2004-09-14
 * @param	[in] volume The volume value to be set. The value should between
 * _SD_VOLUME_MIN and _SD_VOLUME_MAX, which are defined as:
 * _SD_VOLUME_MIN 0
 * _SD_VOLUME_MAX 0x7fff
 * _SD_VOLUME_DEF _SD_VOLUME_MAX
 * The default volume is _SD_VOLUME_DEF.
 * @return	_SD_T_U16
 * @retval	0x0000 Successful
 * @retval	Others refer to Anyka sound device error code in sdglobal.h.
 */
_SD_T_U16  _AKSD_AD_SetDigitalVolume(_SD_T_U16 volume);
/**
 * @brief	Set the EQ mode in the MP3 playback.
 *
 * The default value is _SD_EQ_MODE_NORMAL.
 * This function is available when the music is playing.
 * @author	Huang Lixin
 * @date	2004-09-14
 * @param	[in] mode The EQ mode to be set up, the macro representation is defined as:
 * _SD_EQ_MODE_NORMAL normal mode (default)
 * _SD_EQ_MODE_CLASSIC classical music mode
 * _SD_EQ_MODE_ROCK rock music mode
 * _SD_EQ_MODE_POP pop music mode
 * _SD_EQ_MODE_JAZZ jazz music mode
 * @return	_SD_T_U16
 * @retval	0x0000 Successful
 * @retval	Others refer to Anyka sound device error code in sdglobal.h.
 */
_SD_T_U16 _AKSD_SetEQMode(_SD_T_U8 mode);

/**
 * @brief	Set the channel (left or right or stereo) in the MIDI reproduction.
 *
 * The default value is _SD_CHANNEL_STEREO.
 * This function is available when the music is playing.
 * This function has no effect in MP3 reproduction. In MP3 play back, both left and right channels
 * will always work simultaneously, no matter the MP3 sound data is stereophonic or monophonic.
 * For the monophonic MP3 reproduction, left and right channels produce same content.
 * @author	Huang Lixin
 * @date	2004-09-14
 * @param	[in] channel The audio output channel, defined as:
 * _SD_CHANNEL_RIGHT 0 right channel only
 * _SD_CHANNEL_LEFT 1 left channel only
 * _SD_CHANNEL_STEREO 2 stereo playback (default)
 * @return	_SD_T_U16
 * @retval	0x0000 Successful
 * @retval	Others refer to Anyka sound device error code in sdglobal.h.
 */
_SD_T_U16  _AKSD_SetChannel(_SD_T_U8 channel);

/**
 * @brief	Start speaker LINE-IN mode.
 *
 * If A22 is playing media, user should call function _AKSD_Stop first.
 * User can call function _AKSD_Stop to end LINE-IN mode.
 * @author	Huang Lixin
 * @date 2007-12-25
 * @param	[in] Line_in_Mode 0: line_in disabled 1:mono line_in 2: stereo line_in
 * @return	_SD_T_U16
 * @retval	0x0000 Successful
 * @retval	Others refer to Anyka sound device error code in sdglobal.h.
 */
_SD_T_U16  _AKSD_StartLineIn(_SD_T_U8 Line_in_Mode);

/**
 * @brief	Start reproduction from a sound data buffer.
 *
 * If play music failed, the "end callback function" will not be called.
 * If currently there's an ongoing sound reproduction, one should invoke _AKSD_STOP to stop it
 * before another reproduction can begin. Invoking _AKSD_PlayFromBuffer or
 * _AKSD_PlayFromFile before current reproduction is terminated will lead to an unsuccessful
 * operation, the return error code will be _SD_ERR_DEVICE_BUSY.
 * @author	Huang Lixin
 * @date	2004-09-14
 * @param	[in] Buff The pointer to the start address of the buffer to be played.
 * @param	[in] BuffLen The length of the buffer (in bytes).
 * @param	[in] times The reproduction count (repeat count).
 * @param	[in] MediaType The media type of reproduction
 * @return	_SD_T_U16
 * @retval	0x0000 Successful
 * @retval	Others refer to Anyka sound device error code in sdglobal.h.
 */
_SD_T_U16 _AKSD_PlayFromBuffer(const _SD_T_U8 *Buff, _SD_T_U32 BuffLen, _SD_T_U16 times, _SD_T_U8 MediaType);

/**
 * @brief	Start a music reproduction from a list of buffers.
 *
 * If play music failed, the "end callback function" will not be called.
 * If currently there's an ongoing sound reproduction, the _AKSD_STOP should be invoked to
 * stop it before _AKSD_MultiPlayFromBuffer can be invoked. Otherwise, operation will fail and
 * the error code will be _SD_ERR_DEVICE_BUSY.
 * If a call back function has been set by _AKSD_SetEndCallBack, this call back function will be
 * invoked after all the buffers have been played, not at the end of each buffer.
 * @author	Huang Lixin
 * @date	2004-09-14
 * @param	[in] Buffs The array of pointers to the start addresses of the buffers.
 * @param	[in] BuffLens The array of lengths of the buffers (in bytes).
 * @param	[in] BufCnts	The number of buffers to be played.
 * @param	[in] times The reproduction count (repeat count).
 * @param	MediaType The media type of reproduction
 * @return	_SD_T_U16
 * @retval	0x0000 Successful
 * @retval	Others refer to Anyka sound device error code in sdglobal.h.
 */
_SD_T_U16 _AKSD_MultiPlayFromBuffer(_SD_T_U8 **Buffs, _SD_T_U32 *BuffLens, _SD_T_U16 BufCnts, _SD_T_U16 times, _SD_T_U8 MediaType);

/**
 * @brief	Start a reproduction from a sound file. 
 *
 * Before invoking this function, one should open the target file and obtain its length.
 * Currently, only MP3 file reproduction is supported.
 * If the sound data is not locating at the beginning of the file, the file pointer should be set to the
 * actual start position of the sound data and FileLen should be set to the real length of the data.
 * The reproduction can also work correctly after those set-up.
 * If currently there's an ongoing sound reproduction, one should invoke _AKSD_STOP to stop it
 * before another reproduction can begin. Invoking _AKSD_PlayFromBuffer or
 * _AKSD_PlayFromFile before current reproduction is terminated will lead to an unsuccessful
 * operation , the return error code will be _SD_ERR_DEVICE_BUSY.
 * @author	Huang Lixin
 * @date	2004-09-14
 * @param	[in] pFile The file handle of sound data file to be played.
 * @param	[in] FileLen The length of the target file.
 * @param	[in] times The reproduction count (repeat count).
 * @param	[in] MediaType The media type of reproduction
 * @return	_SD_T_U16
 * @retval	0x0000 Successful
 * @retval	Others refer to Anyka sound device error code in sdglobal.h.
 */
#ifdef _SD_SUPPORT_FILE_SYSTEM
_SD_T_U16 _AKSD_PlayFromFile(_SD_T_FILE pFile, _SD_T_U32 FileLen, _SD_T_U16 times, _SD_T_U8 MediaType);
#endif //_SD_SUPPORT_FILE_SYSTEM
/**
 * @brief	Before invoking this function, one should open all target files and obtain their lengths.
 *
 * Currently, only MP3 file reproduction is supported.
 * For each file, if the sound data is not locating at the beginning of the file, the file pointer should
 * be set to the actual start position of the sound data and FileLen should be set to the real length
 * of the data. The reproduction can also work correctly after those set-up.
 * If currently there's an ongoing sound reproduction, the _AKSD_STOP should be invoked to
 * stop it before _AKSD_MultiPlayFromFile can be invoked. Otherwise, operation will fail and
 * the error code will be _SD_ERR_DEVICE_BUSY.
 * If a call back function has been set by _AKSD_SetEndCallBack, this call back function will be
 * invoked after all the files have been played, not at the end of each file.
 * @author	Huang Lixin
 * @date	2004-09-14
 * @param	[in] pFiles The array of file handles.
 * @param	[in] FileLens The array of length of the target file.
 * @param	[in] FileCnts	The number of files to be played.
 * @param	[in] times The reproduction count (repeat count).
 * @param	[in] MediaType The media type of reproduction.
 * @return	_SD_T_U16
 * @retval	0x0000 Successful
 * @retval	Others refer to Anyka sound device error code in sdglobal.h.
 */
#ifdef _SD_SUPPORT_FILE_SYSTEM
_SD_T_U16 _AKSD_MultiPlayFromFile(_SD_T_FILE *pFiles, _SD_T_U32 *FileLens, _SD_T_U16 FileCnts, _SD_T_U16 times, _SD_T_U8 MediaType);
#endif	//_SD_SUPPORT_FILE_SYSTEM

/**
 * @brief	Stop the current reproduction.
 *
 * There will be no effect if currently no sound data is being reproduced.
 * @author	Huang Lixin
 * @date	2004-09-14
 * @param	void
 * @return	_SD_T_U16
 * @retval	0x0000 Successful
 * @retval	Others refer to Anyka sound device error code in sdglobal.h.
 */
_SD_T_U16 _AKSD_Stop(void);

/**
 * @brief	Pause the current reproduction.
 *
 * There will be no effect if currently no sound data is being reproduced.
 * The paused reproduction can be resumed by calling _AKSD_Resume.
 * @author	Huang Lixin
 * @date	2004-09-14
 * @param	void
 * @return	_SD_T_U16
 * @retval	0x0000 Successful
 * @retval	Others refer to Anyka sound device error code in sdglobal.h.
 */
_SD_T_U16 _AKSD_Pause(void);

/**
 * @brief	Resume a paused reproduction.
 *
 * This function is available when the music is in pause state.
 * Please refer to _AKSD_Pause().
 * @author	Huang Lixin
 * @date	2004-09-14
 * @param	void
 * @return	_SD_T_U16
 * @retval	0x0000 Successful
 * @retval	Others refer to Anyka sound device error code in sdglobal.h.
 */
_SD_T_U16 _AKSD_Resume(void);

/**
 * @brief	Set (seek) the current reproduction to the time position.
 *
 * There will be no effect if currently no sound data is being reproduced or paused.
 * Currently, only MP3 file format is supported.
 * @author	Huang Lixin
 * @date	2004-09-14
 * @param	[in] milliSecond The time position to be set, in ms.
 * @return	_SD_T_U16
 * @retval	0x0000 Successful
 * @retval	Others refer to Anyka sound device error code in sdglobal.h.
 */
_SD_T_U16 _AKSD_Seek(_SD_T_U32 milliSecond);

/**
 * @brief	Get the elapsed time in current reproduction.
 *
 * If the operation fails (the function return an error code), the value of *duration is invalid.
 * Currently, only MP3 file format is supported.
 * @author	Huang Lixin
 * @date	2004-09-14
 * @param	[out] *duration The returned elapsed time, in ms.
 * @return	_SD_T_U16
 * @retval	0x0000 Successful
 * @retval	Others refer to Anyka sound device error code in sdglobal.h.
 */
_SD_T_U16 _AKSD_GetDuration(_SD_T_U32 *duration);

/**
 * @brief	Handle audio device's message and send data from file system to Host Audio
 * Data Buffer.
 *
 * This function should be only called in the main program. There are two situations here\n
 * 1. If the user has set up a message call back function with _AKSD_SetSendMsgCallBack.
 * The main program should only invoke this function (_AKSD_HandleSDMessage) after
 * the call back function is invoked.\n
 * 2. If the user hasn't set up a message call back function by ignoring
 * _AKSD_SetSendMsgCallBack, the main program should invoke
 * _AKSD_HandleSdMessage periodically. The example in Section 5.1illustrates this
 * operation.
 * @WARNING If the user fails to invoke _AKSD_HandleSdMessage in time to communicate with
 * the A2, the data will be lost and the music will be discontinued.
 * @author	Huang Lixin
 * @date	2004-09-14
 * @param	void
 * @return	_SD_T_U16
 * @retval	0x0000 Successful
 * @retval	Others refer to Anyka sound device error code in sdglobal.h.
 */
_SD_T_U16 _AKSD_HandleSdMessage(void);

/**
 * @brief	Handle interrupt and send data frame from Host Audio Data Buffer to A2 Audio Data
 * Buffer.
 *
 * This function is to handle the DSP interrupt, when interrupt occurs, this function should be
 * invoked within 20ms to handle it.
 * This function should be invoked only in the main program, there are 3 possible operations:
 * 1. When A2 is interrupted, this function is invoked directly to handle it.
 * 2. When A2 is interrupted, the interrupt handler just sets an interrupt flag. The user detects
 * this flag in the main loop, then invoke _AKSD_HandleInterrupt to handle it and clear that
 * flag as well.
 * 3. If polling method is used, this function should be invoked in timer or main program.
 * The user's platform determines which operations.
 * @author	Huang Lixin
 * @date	2004-09-14
 * @param	void
 * @return	_SD_T_U16
 * @retval	0x0000 Successful
 * @retval	Others refer to Anyka sound device error code in sdglobal.h.
 */
_SD_T_U16 _AKSD_HandleInterrupt(void);

/**
 * @brief	Get the media type of the sound data stored in buffer.
 *
 * If the operation fails (the function return an error code), the value of *MediaType is invalid.
 * If the program can not detect the media type, _SD_MEDIA_TYPE_UNKOWN will be returned.
 * This function could not be used to get the information of the buffer which is being played or
 * paused currently.
 * @author	Huang Lixin
 * @date	2004-09-14
 * @param	[in] Buff The pointer to the start address of the buffer.
 * @param	[in] BuffLen The length of the buffer (in bytes).
 * @param	[in/out] MediaType The returned media type (in and out parameter)
 * @return	_SD_T_U16
 * @retval	0x0000 Successful
 * @retval	Others refer to Anyka sound device error code in sdglobal.h.
 */
_SD_T_U16 _AKSD_GetMediaTypeFromBuffer(const _SD_T_U8 *Buff, _SD_T_U32 BuffLen, _SD_T_U8 *MediaType);

/**
 * @brief	Get the total time needed to play the specified sound data buffer once.
 *
 * If the operation fails (the function return an error code), the value of *totalTime is invalid.
 * This function could not be used to get the information of the buffer which is being played or
 * paused currently.
 * @author	Huang Lixin
 * @date	2004-09-14
 * @param	[in] Buff The pointer to the start address of the buffer.
 * @param	[in] BuffLen	The length of the buffer (in bytes).
 * @param	[in] MediaType The media type of reproduction, which is defined in Table 2-3.
 * @param	[out] totalTime The returned total time, in ms.
 * @return	_SD_T_U16
 * @retval	0x0000 Successful
 * @retval	Others refer to Anyka sound device error code in sdglobal.h.
 */
_SD_T_U16 _AKSD_GetTotalTimeFromBuffer(const _SD_T_U8 *Buff, _SD_T_U32 BuffLen, _SD_T_U8 MediaType, _SD_T_U32 *totalTime);

/**
 * @brief	Get the meta data information of a sound data buffer.
 *
 * If the operation fails (the function return an error code), the value of *pMetadataInf is invalid.
 * This function could not be used to get the information of the buffer which is being played or
 * paused currently.
 * @author	Huang Lixin
 * @date	2004-09-14
 * @param	[in] Buff	The pointer to the start address of the buffer.
 * @param	[in] BuffLen The length of the buffer (in bytes).
 * @param	[in] MediaType The media type of reproduction.
 * @param	[out] pMetadataInf The returned meta data info.
 * @return	_SD_T_U16
 * @retval	0x0000 Successful
 * @retval	Others refer to Anyka sound device error code in sdglobal.h.
 */
_SD_T_U16 _AKSD_GetMetaDataInfoFromBuffer(const _SD_T_U8 *Buff, _SD_T_U32 BuffLen, _SD_T_U8 MediaType, _SD_T_META_INFO* pMetadataInf);

/**
 * @brief	Get bit rate and sample rate of the media content from buffer.
 *
 * If the operation failed (the function return an error code), the value of *bitRate and *sampleRate
 * will be invalid.
 * This function could not be used to get the information of the buffer which is being played or
 * paused currently.
 * @author	Huang Lixin
 * @date	2004-09-14
 * @param	[in] Buff The pointer to the start address of the buffer.
 * @param	[in] BuffLen The length of the buffer (in bytes).
 * @param	[in] MediaType The media type of reproduction.
 * @param	[out] *bitRate The returned bit rate, in kbps.
 * @param	[out] *sampleRate The returned sample rate.
 * @return	_SD_T_U16
 * @retval	0x0000 Successful
 * @retval	Others refer to Anyka sound device error code in sdglobal.h.
 */
_SD_T_U16 _AKSD_GetBitRSmpRFromBuffer(const _SD_T_U8 *Buff, _SD_T_U32 BuffLen, _SD_T_U8 MediaType, _SD_T_U32 *bitRate, _SD_T_U32 *sampleRate);

/**
 * @brief	Get the media type of the sound data stored in a file.
 *
 * If the operation fails (the function return an error code), the value of *MediaType is invalid.
 * If the program can't detect the media type, _SD_MEDIA_TYPE_UNKOWN will be returned.
 * This function could not be used to get the information of the file which is being played or
 * paused currently.
 * If the sound data is not locating at the beginning of the file, the file pointer should be set to the
 * actual start position of the sound data and FileLen should be set to the real length of the data.
 * The returned value will be correct after those set up.
 * @author	Huang Lixin
 * @date	2004-09-14
 * @param	[in] pFile The target file handle.
 * @param	[in] FileLen The length of the file.
 * @param	[in/out] MediaType The returned media type of reproduction
 * @return	_SD_T_U16
 * @retval	0x0000 Successful
 * @retval	Others refer to Anyka sound device error code in sdglobal.h.
 */
#ifdef _SD_SUPPORT_FILE_SYSTEM
_SD_T_U16 _AKSD_GetMediaTypeFromFile(_SD_T_FILE pFile, _SD_T_U32 FileLen, _SD_T_U8 *MediaType);
#endif	//_SD_SUPPORT_FILE_SYSTEM
/**
 * @brief	Get the total time needed to play the specified sound data file once.
 *
 * If the operation fails (the function return an error code), the value of *totalTime is invalid.
 * This function could not be used to get the information of the file which is being played or
 * paused currently.
 * If the sound data is not locating at the beginning of the file, the file pointer should be set to the
 * actual start position of the sound data and FileLen should be set to the real length of the data.
 * The returned value will be correct after those set-up.
 * @author	Huang Lixin
 * @date	2004-09-14
 * @param	[in] pFile The target file handle.
 * @param	[in] FileLen	The length of the file.
 * @param	[in] MediaType The media type of reproduction.
 * @param	[out] totalTime The returned total time, in ms.
 * @return	_SD_T_U16
 * @retval	0x0000 Successful
 * @retval	Others refer to Anyka sound device error code in sdglobal.h.
 */
#ifdef _SD_SUPPORT_FILE_SYSTEM
_SD_T_U16 _AKSD_GetTotalTimeFromFile(_SD_T_FILE pFile, _SD_T_U32 FileLen, _SD_T_U8 MediaType, _SD_T_U32 *totalTime);
#endif	//_SD_SUPPORT_FILE_SYSTEM
/**
 * @brief	Get the meta data info from a sound data file.
 *
 * If operation fails (the function return an error code), the value of *pMetadataInf is invalid.
 * This function could not be used to get the information of the file which is being played or
 * paused currently.
 * If the sound data is not locating at the beginning of the file, the file pointer should be set to the
 * actual start position of the sound data and FileLen should be set to the real length of the data.
 * The returned value will be correct after those set-up.
 * If the operation fails (the function return an error code), the value of *totalTime is invalid.
 * This function could not be used to get the information of the file which is being played or
 * paused currently.
 * If the sound data is not locating at the beginning of the file, the file pointer should be set to the
 * actual start position of the sound data and FileLen should be set to the real length of the data.
 * The returned value will be correct after those set-up.
 * @author	Huang Lixin
 * @date	2004-09-14
 * @param	[in] pFile The target file handle.
 * @param	[in] FileLen The length of the file.
 * @param	[in] MediaType The media type of reproduction, which is defined in Table 2-3.
 * @param	[out] pMetadataInf The returned meta data info
 * @return	_SD_T_U16
 * @retval	0x0000 Successful
 * @retval	Others refer to Anyka sound device error code in sdglobal.h.
 */
#ifdef _SD_SUPPORT_FILE_SYSTEM
_SD_T_U16 _AKSD_GetMetaDataInfoFromFile(_SD_T_FILE pFile, _SD_T_U32 FileLen, _SD_T_U8 MediaType, _SD_T_META_INFO* pMetadataInf);
#endif	//_SD_SUPPORT_FILE_SYSTEM

/**
 * @brief	Get bit rate and sample rate of the media content from file.
 *
 * If the media content is not in the head of the file, then user must seek the file
 * pointer to the location of the media content before call this function, and FileLen
 * must be the real media content's length.
 * @author	Huang Lixin
 * @date	2004-09-14
 * @param	[in] pFile The target file handle.
 * @param	[in] BuffLen	The length of the file.
 * @param	[in] MediaType The media type of reproduction.
 * @param	[out] *bitRate The returned bit rate, in kbps.
 * @param	[out] *sampleRate The returned sample rate.
 * @return	_SD_T_U16
 * @retval	0x0000 Successful
 * @retval	Others refer to Anyka sound device error code in sdglobal.h.
 */
#ifdef _SD_SUPPORT_FILE_SYSTEM
_SD_T_U16 _AKSD_GetBitRSmpRFromFile(_SD_T_FILE pFile, _SD_T_U32 BuffLen, _SD_T_U8 MediaType, _SD_T_U32 *bitRate, _SD_T_U32 *sampleRate);
#endif	//_SD_SUPPORT_FILE_SYSTEM

/**
 * @brief	Get copyright and version information of the Anyka audio device library.
 *
 * @author	Huang Lixin
 * @date	2004-09-14
 * @param	void
 * @return	_SD_T_S8
 * @retval	Character string containing the copyright and version information of the AK3223 API.
 */
_SD_T_S8 *_AKSD_GetVersionInfo(void);

/**
 * @brief Get copyright and version information of the Anyka audio device library.
 * @author Deng Zhou
 * @date   2006-4-24		
 * @param void
 * @return _SD_T_U8
 * @retval Number containing the copyright and version information of the AK3223 API.
 *	for example: 0x00000407 is V0.4.7
 * @retval
 */
_SD_T_U32 _AKSD_GetVersionNum(void);

/**
 * @brief	Check the audio device's status to see whether it's playing music.
 *
 * @author	Huang Lixin
 * @date	2006-08-22
 * @param	void
 * @return	_SD_T_U8
 * @retval	_SD_FALSE the audio device is not playing music.
 * @retval  others: the audio type.
 */
_SD_T_U8 _AKSD_IsPlaying(void);

/**
* @brief	append another different buffer to continue play mp3.
*
* this function is provided to AK3220M.
* @author	JiLixiao
* @date		2005-07-08
* @param	[in] *buff buffer to be appended.
* @param	[in] len buffer length.
* @return	_SD_T_BOOL
* @retval	_SD_FALSE Append next buffer failed. May be the buffer is not NULL or input param invalid.
* @retval	_SD_TRUE Success.
*/
_SD_T_BOOL _AKSD_AppendBuffer(const _SD_T_U8 *buff,_SD_T_U32 len);

/**
* @brief	This function is provided to Get the next buffer status.
*
* If the next buffer is null return true,otherwise return false.
* @author	JiLixiao
* @date		2005-07-10
* @param	void
* @return	_SD_T_BOOL
* @retval	_SD_TRUE the next buffer is null.
* @retval	_SD_FALSE the next buffer is not null.
*/
_SD_T_BOOL _AKSD_ISNextBufferNULL(void);

/**
* @brief	this is function provided to Clear all data in Buffer.
*
* This function is provided for Clear audio data to match with video data.
* After this OP,"_AKSD_UpdateBufferData" function must be called to update
* the audio buffer data which is matched with video.
* this function is provided to implement functional SEEK  synchronization 
* between audio and video.
* @author	JiLixiao
* @date		2005-08-01
* @param	[in] MediaType Media type. Now we only Support MP3 Audio,
*	and it must be _SD_MEDIA_TYPE_MP3 now.
* @return	void
*/
void _AKSD_ClearAllData(_SD_T_U16 MediaType);

/**
* @brief	this is function provided to update data in Buffer.
*
* This function is provided to update audio data in buffer to 
* match with video data.
* this function is provided to implement functional SEEK  synchronization 
* between audio and video.
* @author	JiLixiao
* @date		2005-08-01
* @param	[in] buf data buffer address.
* @param	[in] len data buffer length.
* @param	[in] MediaType media type. this param must be the same with 
* the param in function _AKSD_ClearAllData you have been Called to clear data.
* @return	_SD_T_BOOL 
* @retval	_SD_TRUE  Success.
* @retval	_SD_FALSE  Failed.
*/
_SD_T_BOOL _AKSD_UpdateBufferData(_SD_T_U8 *buf,_SD_T_U32 len,_SD_T_U16 MediaType);

/**
* @brief	Enabled DA 
* @author	JiLixiao
* @date		2005-06-30
* @param	void
* @return	void
* @retval	No
*/
void _AKSD_EnableDA(void);

/**
* @brief	Disabled DA 
* @author	JiLixiao
* @date	2005-06-30
* @param	void
* @return	void
* @retval	No
*/
void _AKSD_DisableDA(void);

/**
* @brief	Set open headphone or open speaker
* @author	Guo Bochun
* @date		2006-4-5
* @param	[in] Headphone _SD_HEADPHONE_ON is open headphone, close speaker.
* _SD_HEADPHONE_OFF is close headphone
* @return	No
* @retval	No
*/
#ifndef _SD_HP_PIPA_CONTROL
void _AKSD_SetHeadPhone(_SD_T_U8 Headphone);
void _AKSD_SelectHeadPhoneOrSpeaker(_SD_T_U8 Headphone);
#else
void _AKSD_SetHeadPhone(_SD_T_U8 Headphone, _SD_T_U8 LinearMode);
void _AKSD_SelectHeadPhoneOrSpeaker(_SD_T_U8 Headphone, _SD_T_U8 LinearMode);
#endif
void _AKSD_SelectHeadPhoneOrSpeaker_ByMode(_SD_T_U8 Headphone, _SD_T_U8 LinearMode);


/**
* @brief	get headphone is open or not
* @author	Guo Bochun
* @date		2006-4-5
* @param	void
* @return	_SD_T_BOOL
* @retval	True is headphone open
*/
_SD_T_U8 _AKSD_GetHeadPhoneSta(void);

/**
* @brief	this function is added to control the analog volume 
* @author	Li Jun
* @date		2005-06-01
* @param	[in] IsHeadphone set headphone or speaker,1 is headphone ,0 is speaker
* @param	[in] Gain set analog volume gain.
*	headphone 0 to 3
*	speaker   0 to 5
* @return	void
* @retval	No
* @remark	if gain is not set right, it will be set 0.
*/
void _AKSD_SetAnalogVolume(_SD_T_U8 IsHeadphone,_SD_T_U8 Gain);

/**
* @brief	Set the current working frequency to 60 or 84 M
* @author	Guo Bochun
* @date		2006-4-5	
* @param	[in] freq	_SD_WORKING_IN_84M indicates 84M Hz;	
*	_SD_WORKING_IN_60M indicates 60M Hz
*	_SD_WORKING_IN_72M indicates 72M Hz

* @return	No
* @retval	No
*/
void _AKSD_SetCurrentFreq(_SD_T_U8 freq);
void _AKSD_InformCurrentFreq(_SD_T_U8 freq);

/**
* @brief	Enable dsp clock and open DA
* @author	Guo Bochun
* @date		2006-4-5
* @param	void
* @return	No
* @retval	No
*/
void _AKSD_EnableDspAndOpenDA(void);

/**
* @brief	Disable dsp clock and close DA
* @author	Guo Bochun
* @date		2006-4-5
* @param	void
* @return	No
* @retval	No
*/
void _AKSD_DisableDspAndCloseDA(void);

/**
* @brief	Disable dsp clock
* @author	Tang Xuechai
* @date		2007-10-22
* @param	void
* @return	No
* @retval	No
*/
void _AKSD_DisableDsp(void);


/**
* @brief	To initialize some global variables before record.
* @author	Guo Bochun
* @date		2006-4-5
* @param	void
* @return	No
* @retval	No
*/
void	_AKSD_Record_Init(_SD_REC_MEDIA_TYPE MediaType);

/**
* @brief Set the record volume rate .
* @author Guo bochun
* @date 2005-12-12
* @param gain:the record volume rate ,one of the follow four numbers: 1,2,4,8.
* @				for 3671, one of the follow eight numbers: 0,1,2,3,4,5,6,7  
* @return _SD_T_U16
* @retval 
*/
_SD_T_U16 _AKSD_SetRecordGain(_SD_T_U16 volume_rate);

/**
* @brief	Set the request buffer size of each time and the first time
* @author	Deng Zhou 
* @date		2006.4.5
* @param	[in] unit_buff_len the size of each time
* @param	[in] first_buff_len the size of the first time
* @return	No
* @retval	No
*/
void	_AKSD_SetBuffParam(_SD_T_U32 unit_buff_len,	_SD_T_U32 first_buff_len);

/**
* @brief	get the mp3 player pause or resume stadus
* @author	Guo Bochun
* @date		2006-4-5
* @param	NO
* @return	NO
* @retval	NO
*/
_SD_T_BOOL _AKSD_IsDataPause(void);

/**
* @brief	Get the current playing samples of player 
* @author	Guo Bochun
* @date		2006-4-5
* @param	[in] *sample the buff which samples store in 
* @param	[in] number  the number of the samples 
* @retval	_SD_OK Successfully 
* @retval	not _SD_OK Successfully 
*/
_SD_T_BOOL _AKSD_GetCurSample(_SD_T_S16* sample, _SD_T_U8 number);

/**
* @brief	Set the specified media type whether should be played with video
* @author	Deng Zhou
* @date		2006.4.5
* @param	[in] play_with_video_flag True if the sepcified media
*	 type should be played with vidio
* @param	[in] MediaType The Media Type of the current play media
* @return	No
* @retval	No
*/
void _AKSD_Set_PlayWithVideo(_SD_T_U8 play_with_video_flag, _SD_T_U8 MediaType);
void _AKSD_InformPlayWithVideo(_SD_T_U8 play_with_video_flag, _SD_T_U8 MediaType);

/**
* @brief	To set DA sample rate
* @author	Deng Zhou
* @date		2006-4-4
* @param	[in] sampleRate  one channel sample rate.
* @param	[in] MediaType  one channel sample rate.
* @return	void
* @retval	No.
*/
void _AKSD_SetDASampleRate(_SD_T_U32 sampleRate, _SD_T_U8 MediaType);

/**
* @brief	control the resilience of the VOD play 
* @author	Guo Bochun
* @date		2006-4-5
* @param	[in] control _SD_TRUE:open the resilience control of player
			_SD_FALSE: don't open the resilience control of player
* @param	[in] MediaType only for mp3
* @return	No
* @retval	No
*/
void _AKSD_ERResilienceContr(_SD_T_BOOL control,_SD_T_U8 MediaType);

/**
* @brief	Set the specified media type whether should be played with VOD
* @author	Guo Bochun
* @date		2006-4-5
* @param	[in] cntflag True if the sepcified media
*	 type should be played with VOD
* @param	[in] MediaType The Media Type of the current play media
* @return	No
* @retval	No
*/
void _AKSD_PlayForVOD(_SD_T_BOOL cntflag,_SD_T_U8 MediaType);

/**
* @brief	When play with video, if video data is over, call this function
* @author	Deng Zhou 
* @date		2006-4-5
* @param	[in]play_end_flag True if video data is over	
* @return	No
* @retval	No
*/
void _AKSD_Set_EndFlag(_SD_T_U8 play_end_flag);

/**
 * @brief	this function is null
 * @author	Huang Lixin
 * @date	2004-09-14
 * @param	[in] SDParam  a pointer point to the sound device parameters.
 * @return	_SD_T_U16
 * @retval	0x0000  Successful
 * @retval	Others  refer to Anyka sound device error code in sdglobal.h.
 */
_SD_T_U16 _AKSD_SetParam(_SD_T_PARAM *SDParam);

/**
 * @brief	get record current duration
 * @author	guo bochun
 * @date	2006-3-15
 * @param	void
 * @return	_SD_T_U32 current duration in MS 
 * @retval	NO
 * @retval	NO
 */
_SD_T_U32 _AKSD_GetRECCurrentDuration(void);

/**
 * @brief	get the stadu of recorder
 * @author	guo bochun
 * @date	2006-3-15
 * @param	none
 * @return	_SD_T_BOOL 
 * @retval	_SD_TRUE  the recorder is recording 
 * @retval	_SD_FALSE  the recorder isn't recording 
 */
_SD_T_BOOL _AKSD_GetRecordStadu(void);
_SD_T_BOOL _AKSD_GetRecordStatus(void);

/**
 * @brief	pre enable speaker or headphone
 * @author	guo bochun
 * @date	2006-3-15
 * @param	[in] headphonestadu  _SD_TRUE:pre enable headphone
 _SD_FALSE: pre enable speaker 
 * @return	NO
 * @retval	NO
 */
void _AKSD_PreEnableHPorSP(_SD_T_BOOL headphonestadu);

/**
 * @brief	enable speaker or headphone
 * @author	guo bochun
 * @date	2006-3-15
 * @param	[in] headphonestadu  _SD_TRUE: enable headphone
 _SD_FALSE: enable speaker 
 * @return	NO
 * @retval	NO
 */
void _AKSD_EnableHPorSP(_SD_T_BOOL headphonestadu);

/**
 * @brief	get	
 * @author	Deng Zhou
 * @date	2006-6-28
 * @param	NO
 * @return	_SD_T_U32
 * @retval	length of remained
 */
_SD_T_U32 _AKSD_GetRemainedLen(void);
/* end of sounddevice.h */
/*@}*/
_SD_T_U16  _AKSD_PreRecord(_AUDIO_REC_MODE Mode,_SD_T_BOOL DTX_Flag);
void _AKSD_StartRecord(void);
_SD_T_U16  _AKSD_DataSendCallBack(_SD_T_DATA_SEND_CB_PARA *cbParm);
void _AKSD_RecordFree(void);
void _AKSD_DisableAD_MIC(void);
_SD_T_U16  _AKSD_SetTempo(_SD_T_U16 Tempo);

#if (_SD_MODULE_FCSIM_SUPPORT == 1)
void _AKSD_SetDPCMData(unsigned short BufPtr, unsigned short dpcmdata);
void _AKSD_FCSetDPCMData(unsigned short BufPtr, unsigned short dpcmdata);

void _AKSD_FCSetApuReg(int addr, int data);
#endif

void _AKSD_SetCallbackFuns(const _SD_CB_FUNS *pCBFuns);
_SD_T_U16 _AKSD_CloseAudioDevice(void);

/**
 * @brief	set reord once callback time
 * @author	Deng Zhou
 * @date	2006-9-20
 * @param	[in] Once_record_time : once callback time, ms
 */
void _AKSD_Set_Once_RecTime(_SD_T_U32 Once_record_time);

/**
 * @brief	set reord ASLC 
 * @author	Deng Zhou
 * @date	2006-9-20
 * @param	[in] ASLC_Enable : open or close ASLC
 * @param	[in] ASLC_level : ASLC Param
 */
void _AKSD_Set_ASLC_Level(_SD_T_U8 ASLC_Enable, _SD_T_U16 ASLC_level);

/**
 * @brief	if recording, set the AD noise reduce enable or disable
 * @author	Huang Lixin
 * @date	2006-11-9
 * @param	_SD_T_U8 EnableFlag: 0, disable , 1 enable
 * @retval	No
  */
void _AKSD_Record_SetADNoiseReduce(_SD_T_U8 EnableFlag);

/**
 * @brief	if recording, get the record info, such as sample rate, channles.
 * @author	Huang Lixin
 * @date	2006-10-31
 * @param	[in/out] pRecordInfo: to save the info
 * @retval	No
  */
void _AKSD_GetRecordInfo(_SD_T_MEDIA_INFO *pRecordInfo);

void _AKSD_SetMediaInfo(_SD_T_MEDIA_INFO *pMediaInfo);
/**
 * @brief	Save ARM and DSP data when game to be paused
 * @author	Huang Lixin
 * @date	2006-11-16
 * @param	[in/out] pRecordInfo: to save the info
 * @retval	No
  */
_SD_T_U16 _AKSD_SaveAudioData(_SD_T_U8 *buff);

/**
 * @brief	Restore ARM and DSP data when game to be resumed  
 * @author	
 * @date	2006-11-16
 * @param	[in/out] pRecordInfo: to save the info
 * @retval	No
  */
_SD_T_U16 _AKSD_LoadAudioData(_SD_T_U8 *buff, _SD_T_U8 MediaType);


void _AKSD_SetEQGainFlag(_SD_T_BOOL gain);

/**
 * @brief	Get the length of the data that to be saved when NES/SNES game is to be interrupted 
 * @author	
 * @date	2006-11-16
 * @param	
 * @retval	
  */
_SD_T_U16 _AKSD_GetSaveDataLen(_SD_T_U32 *len);

/**
* @brief   create AMR index buffer for the given file . 
* @author  HuangLixin
* @date    2007-6-12
* @param   _SD_T_INDEX_INFO *amr_index_info : given file handle, time per piece
* @param   _SD_T_PNODE **pnode, AMRINDEX pointer, the memory will be allocated in 
*			this function.
* @return _SD_T_U16
* @retval _SD_OK: Successful
* @retval _SD_IS_AMR_SEEK_FILE: the file has had index
* @retval _SD_ERR_AMR_FORMAT : not a AMR file
* @retval _SD_FAIL : other error.
*/
_SD_T_U16 _AKSD_CreateAMRIndexBuff(_SD_T_INDEX_INFO *amr_index_info, _SD_T_PNODE **pnode);

/**
* @brief   free the index buffer
* @author  HuangLixin
* @date    2007-6-12
* @param   _SD_T_PNODE *pnode, AMR INDEX to be freed 
* @return _SD_T_U16
* @retval 0x0000: Successful
* @retval _SD_FAIL : other error.
*/
_SD_T_U16 _AKSD_FreeAMRIndexBuff(_SD_T_PNODE *pnode);


#ifdef _SD_CHANGE_WORKCLOCK
/*
*@brief  set callback function of DSP working clock setting
*@author Tang Xuechai
*@date  2007-6-27
*@param  _SD_SET_DSPFREQDIV_CALLBACK: callback function
*@retval  _SD_OK: succeed
*@retval  others: failed
*/
_SD_T_U16 _AKSD_SetDSPClockDiv_CallBack(_SD_SET_DSPFREQDIV_CALLBACK callback);

/*
*@brief  Change DSP working clock
*@author Tang Xuechai
*@date  2007-6-27
*@param  arm_freq_div, arm frequency div  (2/4/6/8/16/32)
*@param  dsp_freq_div, dsp frequency div  (2/4/6/8/16/32)
*@retval  _SD_OK: change clock succeed
*@retval  _SD_FAIL: change clock failed
*/
_SD_T_U16 _AKSD_SetDSPClockDiv(_SD_T_U32 arm_freq_div, _SD_T_U32 dsp_freq_div);
_SD_T_U16 _AKSD_SetDSPClockFinish(void);
#endif


#if (1 == _SD_MODULE_PCM_OUT_SUPPORT)
_SD_T_U32 _AKSD_RetrievePCMBuffer(_SD_T_U8 **Buff, _SD_T_U32 Len);
void _AKSD_ReleasePCMBuffer(_SD_T_U32 Len);
_SD_T_U32 _AKSD_PCMBufferStat(_SD_T_U32 reqLen);
void _AKSD_SetPCMSendOut(_SD_T_BOOL pcmOut);
#endif

#if (1 == _SD_MODULE_SFCSIM_SUPPORT)
void _AKSD_Snes_ResetApu(void);
void _AKSD_Snes_ApuExecuteNow(_SD_T_S32 CpuCycles,_SD_T_S32 scanlines);
void _AKSD_Snes_SetApuOutPort(_SD_T_S32 CpuCycles, _SD_T_S32 scanlines, 
									_SD_T_S16 Addr, _SD_T_S16 Byte);
void _AKSD_Snes_SetOneCycle(_SD_T_S16 Cycles);
_SD_T_U8 _AKSD_Snes_GetApuOutPort(_SD_T_S32 CpuCycles, _SD_T_S32 scanlines, 
								  _SD_T_S32 Addr);
void _AKSD_Snes_EnableEnv(void);
void _AKSD_Snes_SetHMax(_SD_T_S32 hmax);
#endif
_SD_T_U32 _AKSD_InformDatareqMode(_SD_T_U8 DatareqMode);
_SD_T_U32 _AKSD_CheckBuffer(_SD_T_BUFFER_CONTROL *buffer_control);
_SD_T_U32 _AKSD_UpdateBufferWpos(_SD_T_U32 len);

_SD_T_U16 _AKSD_SetVtimerDelayMSCallBack(_SD_VTIMERDELAYMS_CALLBACK SDCallBack);
_SD_T_U16 _AKSD_SetVtimerDelayMSForHpCallBack(_SD_VTIMERDELAYMS_CALLBACK SDCallBack);


/**
* @brief	Specify the call back function to be invoked at the end of the reproduction.
*
* This functionality is optional. If no call back function is needed, this function can be ignored.
* In sound file reproduction, it's recommended to close the file handle in the call back function.
* If a call back function bas been specified by _AKSD_SetEndCallBack, the specified function
* will be invoked automatically at the end of the reproduction, no matter whether the reproduction
* is terminated expressly by calling _AKSD_Stop, or it stops automatically when the end of sound
* data file (or buffer) is reached.
* When the reproduction is terminated by calling _AKSD_Stop, the returned value in call back
* function, user_stop will equal _SD_T_TRUE. If the reproduction stops automatic when the
* end of sound data file (or buffer) is reached, user_stop will equal _SD_T_FALSE.
* @author	Huang Lixin
* @date	2004-09-14
* @param	[in] SDCallBack the pointer to the call back function, the function pointer type,
* _SD_END_CALLBACK, is defined as:
* typedef void (*_SD_END_CALLBACK)(_SD_T_BOOL user_stop);
* @return	_SD_T_U16
* @retval	0x0000 Successful
* @retval	Others refer to Anyka sound device error code in sdglobal.h.
*/
_SD_T_U16 _AKSD_SetEndCallBack(_SD_END_CALLBACK SDCallBack);

/**
* @brief	Specify the call back function to be invoked when the device sends out a message to
*
* request next sound data frame.
* The call back function is used by the audio device to inform the application on higher
* level to prepare next sound data frame. However, this function is optional. The user can choose
* to set a call back function or not based on the target platform's requirement.
* If a call back function is set, in the call back function, the user needs to send out a message to
* system's message queue, and in the main program, simply invoke _AKSD_HandleSdMessage to
* handle this message. Please don't invoked _AKSD_HandleSdMessage directly in the call back
* function, since it will be invoked in the function handling DSP interrupt as well.
* If the user choose not to set up a call back function, he should invoke
* _AKSD_HandleSdMessage in the main program periodically.
* Please refer to the examples in Section 3 for the examples
* @author	Huang Lixin
* @date	2004-09-14
* @param	[in] SDCallBack The pointer to the call back function, the function pointer type,
* _SD_SEND_MSG_CALLBACK, is defined as
* typedef	void (*_SD_SEND_MSG_CALLBACK)(void);
* @return	_SD_T_U16
* @retval	0x0000 Successful
* @retval	Others refer to Anyka sound device error code in sdglobal.h.
*/
_SD_T_U16 _AKSD_SetSendMsgCallBack(_SD_SEND_MSG_CALLBACK SDCallBack);


/**
* @brief	To set some global variables when the system is power on.
* @author	Guo Bochun
* @date		2006-4-5
* @param	[in] RecordCallBack the call-back function which is called
*           when Audio Driver record data is ready.
* @param	[in] RecordEndCallBack the call-back function which is called when 
*           one sound such as MP3 has been played over.
* @return	_SD_T_U16
* @retval	_SD_OK Successfully initialize the audio driver.
* @retval	not _SD_OK Initializing the audio driver failed. Returned value
*           is the error code.
*/
_SD_T_U16 _AKSD_SetRecordCallBack(_SD_ADPCM_RECORD_CALLBACK RecordCallBack,_SD_END_CALLBACK	RecordEndCallBack);


#if (_SD_MODULE_RESAMPLE_SUPPORT==1)
/**
 * @brief	打开重采样设备.
 * @author	Deng Zhou
 * @date	2007-10-15
 * @param	[in] pRS_Init:
 * 需要重采样的数据信息初始结构
 * @return	ReSampleContext:
 * 重采样结构指针 
 * @retval	if ==_SD_NULL : 打开失败
 * @retval	else : 打开成功
 */
void* _AKSD_Resample_Open(_SD_T_AUDIO_RS_INIT *pRS_Init);

/**
 * @brief	对指定数据进行重采样.
 * @author	Deng Zhou
 * @date	2007-10-15
 * @param	[in] pRSContext:
 * 重采样结构指针
 * @param	[in] pRS_Strc:
 * 需要重采样的数据结构
 * @return	T_U32 
 * @retval	重采样返回的数据长度
 */
_SD_T_S32 _AKSD_Resample(void *pRSContext, _SD_T_AUDIO_RS_STRC *pRS_Strc);

/**
 * @brief	关闭重采样设备.
 * @author	Deng Zhou
 * @date	2007-10-15
 * @param	[in] pRSContext:
 * 重采样结构指针
 * @retval	void
 */
void _AKSD_Resample_Close(void *pRSContext);

#endif// RESAMPLE_SUPPORT

typedef enum
{
    META_VERSION = 0x1,			//metainfo version
    META_TITLE = 0x2   ,		//Title
    META_ARTIST=0x4 ,			//Artist
    META_ALBUM =0x8 ,			//Album
    META_YEAR =0x10 ,			//Year
    META_COMMENT = 0x20,		//Comment
    META_GENRE = 0x40,			//Genre
    META_TRACK = 0x80			//Track
}_SD_METAINFO_ID;
 
 
typedef struct
{
	_SD_T_U32   StrLen;
	_SD_T_U16*  content;
}_SD_META_INFO;
 
typedef struct meta_node
{
	_SD_METAINFO_ID    metatype; 
	_SD_META_INFO	   metainfo;
	struct meta_node*  next;
}_SD_META_Node;

/**
 * @brief Get the meta information of a media file as list.
 * If the operation fails (the function return an error code), the returned value is _SD_NULL.
 * @author 
 * @date 
 * @param pFile: The target file handle.
 * @param FileLen: The length of the file.
 * @param metaflag: flag of meta that indicate if you wanna get it. 
 * @return pointer of struct _SD_META_Node
 * @retval not _SD_NULL: Successful
 * @retval Others: refer to Anyka sound device error code in sdglobal.h.
 */
_SD_META_Node*  _AKSD_GetMETALinkList(_SD_T_FILE pFile, _SD_T_U32 FileLen, _SD_T_U32 metaflag);

/**
 * @brief Get the meta node from a meta-info list.
 * If the operation fails (the function return an error code), the returned value is _SD_NULL.
 * @author 
 * @date 
 * @param linklist: header of the meta-info list.
 * @param metatype: meta information type.
 * @return pointer of struct _SD_META_INFO
 * @retval not _SD_NULL: Successful
 * @retval Others: refer to Anyka sound device error code in sdglobal.h.
 */
_SD_META_INFO*  _AKSD_GetMETAInfo(_SD_META_Node* linklist, _SD_METAINFO_ID metatype);


/**
 * @brief Destroy a meta-info list and free memory used by the list.
 * If the operation fails (the function return an error code), the returned value is _SD_FALSE.
 * @author 
 * @date 
 * @param linklist: header of the meta-info list.
 * @return _SD_T_BOOL
 * @retval _SD_TRUE: Successful
 * @retval Others: refer to Anyka sound device error code in sdglobal.h.
 */
_SD_T_BOOL _AKSD_ReleaseMETALinList(_SD_META_Node* metanode);


/**
 * @brief Modify the meta information of a media file based on specified meta-info and meta-type.
 * If the operation fails (the function return an error code), the returned value is _SD_FALSE.
 * @author 
 * @date 
 * @param pFile: The target file handle.
 * @param metaptr: meta information specified.
 * @param metatype: meta type specified. 
 * @return _SD_T_BOOL
 * @retval _SD_TRUE: Successful
 * @retval Others: refer to Anyka sound device error code in sdglobal.h.
 */
_SD_T_BOOL _AKSD_SetMETAInfo(_SD_T_FILE pFile, _SD_META_INFO* metaptr, _SD_METAINFO_ID metatype);

/**
 * @brief provide a interface to set stop delay time outside, 
 * if this function is not called, the sound lib will use the default time.
 * @author	bruce chen (chen zhandong)
 * @date	2008-12-3
 * @param _SD_T_U16 delaytime (scale in milliseconds)
 * @return void
 */
void _AKSD_Set_StopDelayTime(_SD_T_U16 delaytime);

/**
 * @brief provide a interface to set sound total time and time boundary,
 * if totaltime > time_boundary, sound lib will do slow up and slow down, 
 * if totaltime < time_boundary, sound lib will not do slow up and slow down,
 * if this function is not called, the sound lib will will do slow up and slow down as default.
 * @author	bruce chen (chen zhandong)
 * @date	2008-12-3
 * @param _SD_T_U16 totaltime, _SD_T_U16 time_boundary (scale in milliseconds both)
 * @return void
 */
void _AKSD_Set_SoundTotalTime(_SD_T_U16 totaltime, _SD_T_U16 time_boundary);


/**
* @brief	provide a interface to set MIC input mode
* if use the differential input mode, use DIFFERENTIAL_INPUT_MODE as parameter
* if use the single ended input mode, use SINGLE_ENDED_INPUT_MODE as parameter
* @author	bruce chen (chen zhandong)
* @date	2009-12-21
* @param	_SD_T_U32 (only DIFFERENTIAL_INPUT_MODE or SINGLE_ENDED_INPUT_MODE is legal)
* @return	_SD_T_BOOL
*/
_SD_T_BOOL	_AKSD_Set_MIC_InputModule(_SD_T_U32	mode);



/************************************************************************/
/*below is added for seek mp3 to exact time                             */
/************************************************************************/
typedef struct node
{
	_SD_T_U32	time;
	_SD_T_U32	position;
	struct node * next;
}seek_table_node;

/**
 * @brief	to creat a list to store the exact position for the given array of time
 * @author	bruce chen (chen zhandong)
 * @date	2008-12-5
 * @param	_SD_T_FILE pfile, _SD_T_U32 * time, _SD_T_U32 num (number of the time's element)
 * @return	seek_table_node * , a pointer to the created list
 */
seek_table_node * _AKSD_CreatMP3SeekTable(_SD_T_FILE pfile, _SD_T_U32 * time, _SD_T_U32 num);

/**
 * @brief	to search the array to get the right position for the particular time
 * @author	bruce chen (chen zhandong)
 * @date	2008-12-5
 * @param	seek_table_node * nodelist
 * @return	_SD_T_U32, the return vaule of the function is the right position
 */
_SD_T_U32 _AKSD_GetPositionFromMP3SeekTable(seek_table_node * nodelist, _SD_T_U32 time);

/**
 * @brief	to destroy the list creat by _AKSD_CreatMP3SeekTable
 * @author	bruce chen (chen zhandong)
 * @date	2008-12-5
 * @param	seek_table_node * nodelist
 * @return	void
 */
void _AKSD_DestoryMP3SeekTable(seek_table_node * nodelist);
#endif

