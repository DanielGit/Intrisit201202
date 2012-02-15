
#ifndef __MPLAYER_AUDIO_OUTPUT_H__
#define __MPLAYER_AUDIO_OUTPUT_H__
#include "mplayer_interface.h"
#include "config.h"
#include "mplayer_audio_output.h"
#include "utils/IServiceManager.h"
#include "libaf/af_format.h"
#include <media/IMPlayer.h>
#include <media/IMediaPlayerService.h>
#include <mplayer_interface.h>

/* C code interface */
#ifdef __cplusplus
extern "C" {
#endif

	typedef struct audioSink_info {
		int bufferSize;
		int frameCount;
		int channelCount;
		int frameSize;
		unsigned int latency;
		float msecsPerFrame;
	} audioSink_info_t;

	int MplayerAudioOutput_Control(int cmd, void *arg);
	int MplayerAudioOutput_Init(int rate,int channels,int format,int flags);
	int MplayerAudioOutput_Uninit(int immed);
	int MplayerAudioOutput_GetAudioSinkInfo(audioSink_info_t * mAudoSink_info);
	int MplayerAudioOutput_GetBufferSize(void);
	int MplayerAudioOutput_GetFrameSize(void);
	int MplayerAudioOutput_GetSpace(void);
	int MplayerAudioOutput_GetDelay(void);
	int MplayerAudioOutput_FreeShm(void);
	int MplayerAudioOutput_ObtainShm(void);
	int MplayerAudioOutput_NotifyPlay(int len);
	int MplayerAudioOutput_Play(int addr, int len);
	int MplayerAudioOutput_PlayChunk(int len, void* addr);
	int MplayerAudioOutput_Trans(int cnt, int samval);
	void MplayerAudioOutput_Pause(void);
	void MplayerAudioOutput_Start(void);
	void MplayerAudioOutput_Resume(void);

extern void MplayerAudioOutput_Exit(void);

#ifdef __cplusplus
} // extern "C"
#endif



#ifdef __cplusplus

// 
#include <media/MediaPlayerInterface.h>
#include <MPlayer.h>

#include <utils.h>

using namespace android;

struct ao_data_buffer {
	void * data;
	int capacity;
	int size;
	int flags;
};

namespace android {

class MplayerAudioOutput 
{
public:
	enum MAUDIO_COMMANDS {
		AUDIO_CMD_UNKNOWN = -1,
		AUDIO_CMD_NONE = 0,
		AUDIO_CMD_START,
		AUDIO_CMD_EXIT,
		AUDIO_CMD_STOP,
		AUDIO_CMD_PAUSE,
		AUDIO_CMD_RESUME,
	};

	enum MAUDIO_STATES {
		AUDIO_STATE_UNKNOWN = -1,
		AUDIO_STATE_NONE = 0,
		AUDIO_STATE_STARTED,
		AUDIO_STATE_EXITED,
		AUDIO_STATE_PLAYING,
		AUDIO_STATE_WAITING,
		AUDIO_STATE_STOPED,
		AUDIO_STATE_PAUSED,
		AUDIO_STATE_RESUMED,
	};


	~MplayerAudioOutput();
	MplayerAudioOutput();
	MplayerAudioOutput(sp<MPlayer> player);

	void SetAudioSink(sp<MediaPlayerBase::AudioSink>&       iAudioSink);
	bool AudioSinkInited();

	int Init(int rate,int channels,int format,int flags);
	int Uninit();
	int GetAudioSinkInfo(audioSink_info_t * as_i);
	int GetFrameSize(void);
	int GetBufferSize(void);
	int GetSpace(void);
	int GetDelay(void);
	int Play(const void* data,int len,int flags);
	void Pause(void);
	void Resume(void);

static  sp<IMediaPlayerService>         sMediaPlayerService;

private:
	void Init_Var(void);
	static int audio_output_thread(void* p);
	int audio_output_thread_func();



	sp<MediaPlayerBase::AudioSink>       mAudioSink;
	sp<MPlayer>         mMPlayer;

	List<struct ao_data_buffer*> ao_data_list;
	List<struct ao_data_buffer*> ao_data_empty_list;
	List<struct ao_data_buffer*>::iterator buffer_list_iter;
	struct ao_data_buffer *ao_data_buf_head;
	Mutex                        mMutexBuffer;
	Condition                    mConditionBuffer;

	bool audioStarted;
	bool mAudioSinkInited;
	int mPlayCount;
	int mCommand;
	int mState;
	Mutex                        mMutex;
	Condition                    mCondition;


	int bufferSize;
	int frameSize;
	int frameCount;
	int channelCount;
	int latency;
	float msecsPerFrame;

};

}; // namespace android

//extern class MplayerAudioOutput * gAudioOutput;
extern class MplayerAudioOutput * MplayerAudioOutput_GetDefaultAudioOutPut();

#endif	/* define __cplusplus */

#endif	/* __MPLAYER_AUDIO_OUTPUT_H__ */
