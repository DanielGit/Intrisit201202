//[PROPERTY]===========================================[PROPERTY]
//            *****  诺亚神舟操作系统V2  *****
//        --------------------------------------
//    	            视频解压缩库依赖函数
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明		
//  V0.1    2009-6      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef _AVI_AUDIO_PORT_H__
#define _AVI_AUDIO_PORT_H__

#include <platform/platform.h>
#include <direct/audio.h>


/*
 * @brief Callback function when audio playing ends. audio_EndCallback(void) must be called in this function
 *
 * @author Feng_Yunqing
 * @return  void
 */
void VD_AudioEndCallback(T_U8 user_stop);

/*
 * @brief Stop playing audio
 *
 * @author Feng_Yunqing
 * @return  T_U16
 * @retval  0       success
 * @retval  other   refer to sdglobal.h
 */
T_U16 VD_AudioStop(void);


/*
 * @brief Stop recording audio
 *
 * @author Feng_Yunqing
 * @return  T_U16
 * @retval  0       success
 * @retval  other   refer to sdglobal.h
 */
T_U16 VD_AudioRecStop(void);


/*
 * @brief Initial audio playback
 *
 * @author Feng_Yunqing
 * @return  T_S16
 * @retval  0       success
 * @retval  -1      failure
 */
T_S16 VD_AudioInit(void);

/*
 * @brief Initial audio recording
 *
 * @author Feng_Yunqing
 * @return  T_S16
 * @retval  0       success
 * @retval  -1      failure
 */
T_S16 VD_AudioRecInit(void);

/*
 * @brief Add the first buffer data and start to play
 *
 * @author Feng_Yunqing
 * @param   data        audio data
 * @param   len         audio data len
 * @param   audio_type  audio data type
 * @return  T_S16
 * @retval  0       success
 * @retval  -1      failure
 */
T_S16 VD_AudioStartPlay(T_U8 *data, T_U32 len, T_U16 audio_type, T_AUDIO_PARAM* pParam);


/*
 * @brief Append data to play
 *
 * @author Feng_Yunqing
 * @return  T_BOOL
 * @retval  AK_TRUE     success
 * @retval  AK_FALSE    failure
 */
T_BOOL VD_AudioAppendBuffer(const T_U8 *data, T_U32 len);


/*
 * @brief Handle sound messages
 *
 * @author Feng_Yunqing
 * @return  T_S16
 * @retval  0       success
 * @retval  other   refer to sdglobal.h
 */
T_S16 VD_HandleSoundMessage(void);


/*
 * @brief Query whether there is a buffer is available to append data
 *
 * @author Feng_Yunqing
 * @return  T_BOOL
 * @retval  AK_TRUE     buffer available
 * @retval  AK_FALSE    buffer NOT available
 */
T_BOOL VD_IsNextBufferNull(void);


/*
 * @brief Query playback time from last _AKSD_Stop(void) operation
 *
 * @author Feng_Yunqing
 * @param   pTime       
 * @return  T_BOOL
 * @retval  AK_TRUE     success
 * @retval  AK_FALSE    failure
 */
T_U16 VD_AudioGetDuration(T_U32 *pTime);


/*
 * @brief Pause playing audio
 *
 * @author Feng_Yunqing
 * @return  T_U16
 * @retval  0       success
 * @retval  other   refer to sdglobal.h
 */
T_U16 VD_AudioPause(void);


/*
 * @brief Clear all buffer data which has not been played
 *
 * @author Feng_Yunqing
 * @return  T_U16
 * @retval  0       success
 * @retval  other   refer to sdglobal.h
 */
void VD_AudioClearBuffer(T_U16 audio_type);


/*
 * @brief Add the first buffer data to resume playing audio. After using _AKSD_Pause(void) and _AKSD_ClearAllData(void), this function must be called to resume playing, NOT _AKSD_AppendBuffer(void)
 *
 * @author Feng_Yunqing
 * @return  T_U16
 * @retval  0       success
 * @retval  other   refer to sdglobal.h
 */
T_U16 VD_AudioResumeAddBuffer(T_U8 *data, T_U32 len, T_U16 audio_type);


/*
 * @brief Add the first buffer data and start to play
 *
 * @author Feng_Yunqing
 * @return  T_U16
 * @retval  refer to audio manual
 */
T_U16 VD_AudioStartRecord(void);


/*
 * @brief Since audio lib V2.3.3 and video lib V2.9.0, the following two functions are needed!
 *        This two functions are called by the video lib, not by the video player.
 */
T_U32 VD_CheckBuffer(_SD_T_BUFFER_CONTROL *buffer_control);
T_U32 VD_UpdateBufferWpos(T_U32 len);


/*
 * @brief Since video lib V2.10.2 , the following functio is needed!
 *        This function is called by the video lib, not by the video player.
 */
T_U16  VD_Resume(void); 

#endif

