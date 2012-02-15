
/*
  Android audio flinger output plugin for MPlayer

  (C) Ingenic Semicondutor Inc.
  
  Author:      Wolfgang <lgwang@ingenic.cn>, 05-12-2009
  Modified:    
  
  Any bugreports regarding to this driver are welcome.
*/

//#define LOG_NDEBUG 0
#define LOG_TAG "MPlayer_AudioFlinger_Interface"
#include <utils/Log.h>
#include <pthread.h>

#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <utils/threads.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <MPlayer.h>
#include "mplayer_interface.h"
#include "config.h"
#include "mplayer_audio_output.h"
#include "utils/IServiceManager.h"
#include "libaf/af_format.h"
#include <media/IMPlayer.h>
#include <media/IMediaPlayerService.h>
#include <mplayer_interface.h>

#if 1
#define MPLAYER_DBG()							\
	do {								\
		LOGD("%s[%d], %s() *** DBG", __FILE__, __LINE__, __FUNCTION__); \
		printf("%s[%d], %s() *** DBG\n", __FILE__, __LINE__, __FUNCTION__); \
	} while (0)

#define MPLAYER_PRINTF(fff, aaa...)					\
	do {								\
		LOGD("%s[%d], %s()::" fff, __FILE__, __LINE__, __FUNCTION__, ##aaa); \
		printf("%s[%d], %s()::\n" fff"\n", __FILE__, __LINE__, __FUNCTION__, ##aaa); \
	} while (0)

#define MY_PRINTF(fff, aaa...)			\
	do {					\
		LOGD(fff, ##aaa);		\
		printf(fff"\n", ##aaa);		\
	} while (0)

#define please_use_av_log printf
#else
#define MY_PRINTF(fff, aaa...) do {} while (0)
#endif
#define MY_PRINTF(fff, aaa...) do {} while (0)
#define MY_MIN(a, b) ((a) < (b)) ? (a) : (b)


#define AO_DATA_BUFFER_NUM 10
//#define AO_DATA_BUFFER_SIZE 0x10000 // 64K default
#define AO_DATA_BUFFER_SIZE 0x4000 // 16K default

namespace android {
sp<IMPlayerClient>  gMPlayerClient = NULL;

static class MplayerAudioOutput * gAudioOutput = NULL;


MplayerAudioOutput::MplayerAudioOutput()
{
	LOGV("%s, Enter...", __FUNCTION__);

	Init_Var();
}

MplayerAudioOutput::MplayerAudioOutput(sp<MPlayer> player)
{
	LOGV("%s, Enter...", __FUNCTION__);

//	mAudioSink = NULL;
	mMPlayer = player;
	Init_Var();
}


void MplayerAudioOutput::Init_Var()
{
	mMPlayer = NULL;
	audioStarted = false;
	mAudioSinkInited = false;
	mPlayCount = 0;
	ao_data_buf_head = NULL;
}


MplayerAudioOutput::~MplayerAudioOutput()
{
	LOGV("%s, Enter...", __FUNCTION__);
	MPLAYER_PRINTF("***   Enter...\n");
	mAudioSinkInited = false;
	audioStarted = false;
	mPlayCount = 0;
//	Uninit();
}

void MplayerAudioOutput::SetAudioSink(sp<MediaPlayerBase::AudioSink>&       iAudioSink)
{
	LOGV("%s, Enter...", __FUNCTION__);
	MPLAYER_PRINTF("***   Enter...\n");

	if ( true == AudioSinkInited() ) {
		LOGD("Re Set mAudioSink");
		MPLAYER_PRINTF("*** Re Set mAudioSink\n");
//		mAudioSink->stop();
		MPLAYER_DBG();
//		mAudioSink.clear();

	}

	mAudioSink = iAudioSink;
	mAudioSinkInited = true;
	LOGV("my mAudioSink.get()=%p", mAudioSink.get());
	MPLAYER_PRINTF("my mAudioSink.get()=%p\n", mAudioSink.get());

	return ;
}

bool MplayerAudioOutput::AudioSinkInited()
{
//	LOGV("%s, Enter...", __FUNCTION__);
//	MPLAYER_PRINTF("***   Enter...\n");

	return mAudioSinkInited;

}

/*
    return: 1=success 0=fail
*/

int MplayerAudioOutput::Uninit()
{
	LOGV("%s, Enter...", __FUNCTION__);
	MPLAYER_PRINTF("***   Enter...\n");

 	if ( AudioSinkInited() == false ) {
		LOGV("line %d", __LINE__);
		MPLAYER_DBG();
		return 0;
	}

//	return 0;

	mAudioSinkInited = false;

//	MPLAYER_PRINTF(" mAudioSink->stop()");
//	mAudioSink->stop();
	LOGV("line %d", __LINE__);
	MPLAYER_DBG();
	mCommand = AUDIO_CMD_EXIT;
	audioStarted = false;


	mCondition.signal();

	LOGV("line %d", __LINE__);
	MPLAYER_DBG();

	if ( mState != AUDIO_STATE_EXITED ) {
		MPLAYER_PRINTF("wait audio thread exit...");
		mCondition.wait(mMutex);
	}

	MPLAYER_DBG();


	// Free audio buffers
//	int i, n;
	printf("222 ao_data_buf_head=%p ???????????????????\n", ao_data_buf_head);

	List<struct ao_data_buffer*>::iterator iter;
	struct ao_data_buffer *ao_data_buf;

	// release list
	while ( ao_data_empty_list.size() ) {
		iter = ao_data_empty_list.begin();
		ao_data_buf = *iter;
//		if ( ao_data_buf->data ) {
		{
			MPLAYER_PRINTF("ao_data_buf->data=%p", ao_data_buf->data);
			free(ao_data_buf->data); // free data
//			MPLAYER_PRINTF("ao_data_buf=%p", ao_data_buf);
//			free(ao_data_buf); // free buffer struct 
		}
		ao_data_empty_list.erase(iter);
	}

	while ( ao_data_list.size() ) {
		iter = ao_data_list.begin();
		ao_data_buf = *iter;
//		if ( ao_data_buf->data ) {
		{
			MPLAYER_PRINTF("ao_data_buf->data=%p", ao_data_buf->data);
			free(ao_data_buf->data); // free data
//			free(ao_data_buf); // free buffer struct 
		}
		ao_data_list.erase(iter);
	}

	/* === Why ao_data_buf_head = 0 === */
	printf("333 ao_data_buf_head=%p ?????????????????????\n", ao_data_buf_head);
//	free(ao_data_buf_head);
	MPLAYER_DBG();

	return 0;

}

int MplayerAudioOutput::Init(int rate,int channels,int format,int flags)
{
	LOGV("%s, Enter...", __FUNCTION__);
	MPLAYER_PRINTF("***   Enter...\n");

	if ( true  == audioStarted ) {
		return 0;
	}

	if ( false == AudioSinkInited() )
		return 0;

	MPLAYER_PRINTF("&mAudioSink=%p\n", &mAudioSink);//sleep(1);
//	MPLAYER_PRINTF("mAudioSink=%p\n", mAudioSink); sleep(1);
	MPLAYER_PRINTF("mAudioSink.get()=%p\n", mAudioSink.get());//sleep(1);

	int audio_system_format;
	switch (format)
	{
	case AF_FORMAT_S8:
	case AF_FORMAT_U8:
		audio_system_format = AudioSystem::PCM_8_BIT;
		break;
	case AF_FORMAT_U16_LE:
	case AF_FORMAT_U16_BE:
#ifndef WORDS_BEGINDIAN
	case AF_FORMAT_AC3:
#endif
	case AF_FORMAT_S16_LE:
#ifdef WORDS_BEGINDIAN
	case AF_FORMAT_AC3:
#endif
	case AF_FORMAT_S16_BE:
	default:
		audio_system_format = AudioSystem::PCM_16_BIT; //? default should be -1
		break;
	}
	
	// audiosink open
	LOGV("mAudioSink->open(%d, %d, PCM_16_BIT, 2)", rate, channels);
	printf("mAudioSink->open(%d, %d, PCM_16_BIT(%d), 4)\n", rate, channels, audio_system_format);
	if (mAudioSink->open(rate, channels, audio_system_format, DEFAULT_AUDIOSINK_BUFFERCOUNT) != NO_ERROR) {
		LOGE("mAudioSink open failed");
		MPLAYER_DBG();
		return 0;
	}

	MPLAYER_DBG();

	bufferSize = mAudioSink->bufferSize();
	frameSize = mAudioSink->frameSize();
	frameCount = mAudioSink->frameCount();
	channelCount = mAudioSink->channelCount();
	latency = mAudioSink->latency();
	msecsPerFrame = mAudioSink->msecsPerFrame();

	// if ( 0 )
	{
		printf("mAudioSink->bufferSize()=%d\n", mAudioSink->bufferSize());
		printf("mAudioSink->frameSize()=%d\n", mAudioSink->frameSize());
		printf("mAudioSink->frameCount()=%d\n", mAudioSink->frameCount());
		printf("mAudioSink->channelCount()=%d\n", mAudioSink->channelCount());
		printf("mAudioSink->latency()=%d\n", mAudioSink->latency());
		printf("mAudioSink->msecsPerFrame()=%f\n", mAudioSink->msecsPerFrame());
	}

	int i;
	int n = AO_DATA_BUFFER_NUM;
	struct ao_data_buffer *ao_data_buf_head = 
		(struct ao_data_buffer *)malloc(AO_DATA_BUFFER_NUM*sizeof(struct ao_data_buffer));

	printf("111 ao_data_buf_head=%p\n", ao_data_buf_head);

	struct ao_data_buffer * ao_data_buf = ao_data_buf_head;
	printf("AO_DATA_BUFFER_NUM=%d, AO_DATA_BUFFER_SIZE=%d\n", AO_DATA_BUFFER_NUM, AO_DATA_BUFFER_SIZE);

	for (i=0; i< n; i++) {
		ao_data_buf[i].data = (void*) malloc(AO_DATA_BUFFER_SIZE);
//		MPLAYER_MSG("ao_data_buf=%p\n", &ao_data_buf[i]);
//		MPLAYER_MSG("ao_data_buf[%d].data=%p\n", i, ao_data_buf[i].data);
		if ( !ao_data_buf[i].data ) {
			printf("no memory available.");
			goto failed; // audio sink init failed.
		}
		ao_data_buf[i].capacity = AO_DATA_BUFFER_SIZE;
		ao_data_buf[i].size = AO_DATA_BUFFER_SIZE;
		ao_data_empty_list.push_back(&ao_data_buf[i]);
	}

	MPLAYER_PRINTF("create audio_output_thread");
        createThreadEtc(audio_output_thread, this, "mp_ao_thread");

	LOGV("Audiosink Init OK...");
	MPLAYER_PRINTF("Init OK...\n");
	return 1;					// 1, success

failed:
	n = AO_DATA_BUFFER_NUM;
	if ( ao_data_buf = ao_data_buf_head ) {
		for (i=0; i< n; i++) {
			if ( ao_data_buf[i].data ) {
				free(ao_data_buf[i].data);
			}
		}
		free(ao_data_buf_head);	
	}


	LOGV("AudioSink Init failed...");
	MPLAYER_PRINTF("Init failed...\n");
	return 0;					// 0, failed
}

int MplayerAudioOutput::GetAudioSinkInfo(audioSink_info_t * as_i)
{
	// if ( 0 )
	{
		printf("mAudioSink->bufferSize()=%d\n", mAudioSink->bufferSize());
		printf("mAudioSink->frameSize()=%d\n", mAudioSink->frameSize());
		printf("mAudioSink->frameCount()=%d\n", mAudioSink->frameCount());
		printf("mAudioSink->channelCount()=%d\n", mAudioSink->channelCount());
		printf("mAudioSink->latency()=%d\n", mAudioSink->latency());
		printf("mAudioSink->msecsPerFrame()=%f\n", mAudioSink->msecsPerFrame());
	}

	if ( NULL == as_i ) return 1;

	as_i->bufferSize = mAudioSink->bufferSize();
	as_i->frameSize = mAudioSink->frameSize();
	as_i->frameCount = mAudioSink->frameCount();
	as_i->channelCount = mAudioSink->channelCount();
	as_i->latency = mAudioSink->latency();
	as_i->msecsPerFrame = mAudioSink->msecsPerFrame();

	return 0;
}

int MplayerAudioOutput::GetBufferSize(void)
{
	LOGV("%s, Enter...", __FUNCTION__);
	MPLAYER_PRINTF("***   Enter...\n");

	if ( false == AudioSinkInited() ) {
		LOGW("Warning, mAudioSink deleted...");
		return -1;
	}

//	return mAudioSink->bufferSize();
	printf("bufferSize=%d\n", AO_DATA_BUFFER_NUM*AO_DATA_BUFFER_SIZE);
	return AO_DATA_BUFFER_NUM*AO_DATA_BUFFER_SIZE;
}

int MplayerAudioOutput::GetFrameSize(void)
{
	LOGV("%s, Enter...", __FUNCTION__);
	MPLAYER_PRINTF("***   Enter...\n");

	if ( false == AudioSinkInited() ) {
		LOGW("Warning, mAudioSink deleted...");
		return -1;
	}

	return mAudioSink->frameSize();
}

int MplayerAudioOutput::GetSpace(void)
{
	Mutex::Autolock lock(mMutexBuffer);

	LOGV("%s, Enter...", __FUNCTION__);
	MPLAYER_PRINTF("***   Enter...\n");

	if ( false == AudioSinkInited() ) {
		LOGW("Warning, mAudioSink deleted...");
		return -1;
	}

	int space_available = 0;

	int n = ao_data_empty_list.size();
//	printf("GetSpace ao_data_empty_list.size()=%d\n", n);

	if ( n == 0 ) {
		return 0;
	}
	
	buffer_list_iter = ao_data_empty_list.begin();

//	for (i=0; i < n; i++) {

	space_available += (*buffer_list_iter)->capacity;
	return space_available;

	while( *buffer_list_iter ) {
		space_available += (*buffer_list_iter)->capacity;
		buffer_list_iter++;
	}

	return space_available;
}

int MplayerAudioOutput::GetDelay(void)
{
	LOGV("%s, Enter...", __FUNCTION__);
	MPLAYER_PRINTF("***   Enter...\n");
	Mutex::Autolock lock(mMutexBuffer);

	if ( false == AudioSinkInited() ) {
		LOGW("Warning, mAudioSink deleted...");
		return -1;
	}

	int byte_wait_to_play = 0;

	int n = ao_data_list.size();

//	printf("GetDelay ao_data_list.size()=%d\n", n);

	if ( n == 0 )
		return 0;
	
	buffer_list_iter = ao_data_list.begin();

	byte_wait_to_play += (*buffer_list_iter)->capacity;

	return byte_wait_to_play;

	while( *buffer_list_iter ) {
		byte_wait_to_play += (*buffer_list_iter)->capacity;
		buffer_list_iter++;
	}
	return byte_wait_to_play;


	float delay = (byte_wait_to_play/frameSize)*msecsPerFrame;
	return 0;
}

int MplayerAudioOutput::Play(const void* data,int len,int flags)
{
//	LOGV("%s, Enter...", __FUNCTION__);
//	MPLAYER_PRINTF("***   Enter...\n");

	Mutex::Autolock lock(mMutexBuffer);

	ao_data_buffer * ao_buffer_tmp;

	LOGV("Play() -- write data=%p, len=%d", data, len);
	++mPlayCount;
//	LOGV("Play count(%d)", mPlayCount);
	MPLAYER_PRINTF("Play count(%d)\n", mPlayCount);

	if ( false == AudioSinkInited() ) {
		LOGW("Warning, mAudioSink deleted...");
		return 0;
	}
	
	if ( false == audioStarted ) {
		LOGV("mAudioSink->start()");
		mAudioSink->start();
		audioStarted = true;
	}
	int n = ao_data_empty_list.size();
	

	if ( n == 0 ) {
		return 0;	// 
	}


	buffer_list_iter = ao_data_empty_list.begin();

	ao_buffer_tmp = * buffer_list_iter;


	ao_data_empty_list.erase(buffer_list_iter);

	ao_buffer_tmp->size = MY_MIN(len, ao_buffer_tmp->capacity);
	memcpy((void *)ao_buffer_tmp->data, (void*)data, ao_buffer_tmp->size);

	ao_data_list.push_back(ao_buffer_tmp);

	mConditionBuffer.signal();

	return ao_buffer_tmp->size;

}

void MplayerAudioOutput::Pause(void)
{
	LOGV("%s, Enter...", __FUNCTION__);
	MPLAYER_PRINTF("***   Enter...\n");
	printf(" this function waiting implemented..\n");

 	if ( AudioSinkInited() == false ) {
		LOGV("line %d", __LINE__);
		MPLAYER_DBG();
		return ;
	}

	mCommand = AUDIO_CMD_PAUSE;


	return ;
}

void MplayerAudioOutput::Resume(void)
{
	LOGV("%s, Enter...", __FUNCTION__);
	MPLAYER_PRINTF("***   Enter...\n");

	printf(" this function waiting implemented..\n");


 	if ( AudioSinkInited() == false ) {
		LOGV("line %d", __LINE__);
		MPLAYER_DBG();
		return ;
	}

	mCommand = AUDIO_CMD_RESUME;
	mCondition.signal(); // wakeup Pause();


	return ;
}


int MplayerAudioOutput::audio_output_thread(void* p)
{
	return ((MplayerAudioOutput*)p)->audio_output_thread_func();
}


int MplayerAudioOutput::audio_output_thread_func( )
{

	pthread_cond_t cond;
	pthread_mutex_t test_lock = PTHREAD_MUTEX_INITIALIZER;

	pthread_cond_init(&cond, NULL);

	unsigned char * data = 0;
	struct timeval now_tv, now_tv2;
	ao_data_buffer * ao_buffer_tmp;

	List<struct ao_data_buffer*>::iterator buffer_list_iter_tmp;

	int byteToPlay;
	int bytePlayed;
	MPLAYER_PRINTF("audio_output_thread_func starting...");

#define USEC_PER_SEC 1000000
#define NSEC_PER_SEC 1000000000

#define NSEC_DELAY 20000000 // 20ms
//#define NSEC_DELAY 100000000 // 100ms

	while ( 1 )
	{
		
		int iBytePlayed = 0;
		struct timespec abstime;

		// PAUSE
		if ( mCommand == AUDIO_CMD_PAUSE ) {
			mCommand = AUDIO_CMD_NONE;
			mAudioSink->pause();
			//mAudioSink = NULL;
			mState = AUDIO_STATE_PAUSED;
			MPLAYER_PRINTF("audio thread paused...");
			// wait resume
			mCondition.wait(mMutex);

			MPLAYER_PRINTF("audio thread paused...");

		}

		// STOP
		if ( mCommand == AUDIO_CMD_EXIT ) {
			mCommand = AUDIO_CMD_NONE;
			mAudioSink->flush();
			mAudioSink->stop();

			mAudioSink.clear();
			//mAudioSink = NULL;
			mState = AUDIO_STATE_EXITED;
			MPLAYER_PRINTF("audio thread exit...");

			mCondition.signal(); // wakeup Uninit();

			break;
		}

		// Resume
		if ( mCommand == AUDIO_CMD_RESUME ) {
			mCommand = AUDIO_CMD_NONE;
			mAudioSink->start();
			//mAudioSink = NULL;
			mState = AUDIO_STATE_RESUMED;
		}

		// lock(mMute);
		mMutexBuffer.lock();
		MPLAYER_PRINTF("audio thread: ao_data_list.size()=%d", ao_data_list.size());
		int n = ao_data_list.size();
		// unlock 
		mMutexBuffer.unlock();

		if ( n == 0 ) {
			// wait...
#if 0
			MPLAYER_PRINTF("audio sink underrun, wait 10ms");
			usleep(100000);
#elif 1
			mMutexBuffer.lock();
			mConditionBuffer.wait(mMutexBuffer); // wait Play() wakeup me ...
			mMutexBuffer.unlock();
#else
			clock_gettime(CLOCK_REALTIME, &abstime);
			if ( abstime.tv_nsec <  NSEC_PER_SEC - NSEC_DELAY ) {
				abstime.tv_nsec += NSEC_DELAY;
				
			}
			else {
				abstime.tv_sec += 1;
//				abstime.tv_nsec = abstime.tv_nsec +  NSEC_DELAY - NSEC_PER_SEC;
				abstime.tv_nsec -= NSEC_PER_SEC - NSEC_DELAY;

			}

			pthread_mutex_lock(&test_lock);
			pthread_cond_timedwait(&cond, &test_lock, &abstime);
			pthread_mutex_unlock(&test_lock);
#endif

			if ( mCommand == AUDIO_CMD_EXIT ) {
				mCommand = AUDIO_CMD_NONE;
				mAudioSink.clear();
				//mAudioSink = NULL;
				break;
			}
			continue;
		}


		mMutexBuffer.lock();
		buffer_list_iter_tmp = ao_data_list.begin();
		ao_buffer_tmp = * buffer_list_iter_tmp;

		// unlock 
		mMutexBuffer.unlock();


		MPLAYER_DBG();


		data = (unsigned char *)ao_buffer_tmp->data;
		MPLAYER_DBG();
		while ( ao_buffer_tmp->size > 0 ) {
			mState = AUDIO_STATE_PLAYING;

			byteToPlay = ao_buffer_tmp->size;
//			gettimeofday(&now_tv, 0);
			if ((bytePlayed = mAudioSink->write(data, byteToPlay)) < 0) {
				LOGE("Error in writing:%d", byteToPlay);
				MPLAYER_PRINTF("Error in writing:%d", byteToPlay);
				mMutexBuffer.unlock();
				return 1;
			}
/*			gettimeofday(&now_tv2, 0);
			printf("Before AudioSink->write() time: %d.%06d, byteToPlay=%d\n", now_tv.tv_sec, now_tv.tv_usec, byteToPlay);
			printf("After  AudioSink->write() time: %d.%06d, bytePlayed=%d\n", now_tv2.tv_sec, now_tv2.tv_usec, bytePlayed); 
*/
			ao_buffer_tmp->size -= bytePlayed;
			data += bytePlayed;
			iBytePlayed += bytePlayed;

		}
		MPLAYER_DBG();

		// lock 
		mMutexBuffer.lock();
		ao_data_list.erase(buffer_list_iter_tmp);
		ao_data_empty_list.push_back(ao_buffer_tmp);
		// unlock(mMute);
		mMutexBuffer.unlock();

		if ( iBytePlayed > 1000 ) {
			mState = AUDIO_STATE_WAITING;
			// wait iBytePlayed data playend
			// timeout ....
			/* =================== */
			MPLAYER_PRINTF("Wait audio play a little time");

/*			int delay_usec;
			iBytePlayed = ((iBytePlayed * 80) / 100);
			MPLAYER_PRINTF("iBytePlayed=%d", iBytePlayed);
			delay_usec = (1000*(iBytePlayed)/frameSize)*msecsPerFrame;
			MPLAYER_PRINTF("frameSize=%d, msecsPerFrame=%f, delay_usec=%d", frameSize, msecsPerFrame, delay_usec);
			usleep(delay_usec);
*/

			clock_gettime(CLOCK_REALTIME, &abstime);
			if ( abstime.tv_nsec <  NSEC_PER_SEC - NSEC_DELAY ) {
				abstime.tv_nsec += NSEC_DELAY;

			}
			else {
				abstime.tv_sec += 1;
//				abstime.tv_nsec = abstime.tv_nsec +  NSEC_DELAY - NSEC_PER_SEC;
				abstime.tv_nsec -= NSEC_PER_SEC - NSEC_DELAY;

			}

//			gettimeofday(&now_tv, 0);
			pthread_mutex_lock(&test_lock);
			pthread_cond_timedwait(&cond, &test_lock, &abstime);
			pthread_mutex_unlock(&test_lock);
/*
			gettimeofday(&now_tv2, 0);
			printf("Before pthread_cond_timedwait(%d ns) time: %d.%06d\n",  NSEC_DELAY, now_tv.tv_sec, now_tv.tv_usec);
			printf("After  pthread_cond_timedwait(%d ns) time: %d.%06d\n",  NSEC_DELAY, now_tv2.tv_sec, now_tv2.tv_usec);
*/
		}
		

	}

	MPLAYER_PRINTF(" LEAVE()");
		
}
}// end namespace android


int MplayerAudioOutput_Control(int cmd, void *arg)
{
	return 1;
}

/*
    return: 1=success 0=fail
*/

int MplayerAudioOutput_Init(int rate,int channels,int format,int flags)
{
	printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	if ( gMPlayerClient != 0 ) {
		printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
		gMPlayerClient->audioInit(rate, channels, format);
	}

	return 0;
}


int MplayerAudioOutput_Uninit(int immed)
{
	if ( gMPlayerClient != 0 ) {
		gMPlayerClient->audioUninit(immed);
	}
	
	return 1;
}

int MplayerAudioOutput_GetBufferSize(void)
{
	int ret = 0;
	if (gMPlayerClient != 0) {
		ret = gMPlayerClient->audioGetBufferSize();
	}

	return ret;
}

int MplayerAudioOutput_GetFrameSize(void)
{
//	LOGV("%s, Enter...", __FUNCTION__);
//	MPLAYER_PRINTF("***   Enter...\n");
	if (gAudioOutput)
		return gAudioOutput->GetFrameSize();

	return 32000;
}

int MplayerAudioOutput_GetDelay(void)
{
	int ret = 0;
	if (gMPlayerClient != 0) {
		ret = gMPlayerClient->audioGetDelay();
	}

	return ret;
}

int MplayerAudioOutput_Trans(int cnt, int samval)
{
	int ret = 0;
	if ( gMPlayerClient != 0 ) {
		ret = gMPlayerClient->audioTrans(cnt, samval);
	}

	return ret;
}

int MplayerAudioOutput_GetSpace(void)
{
	int ret = 0;
	if (gMPlayerClient != 0) {
		ret = gMPlayerClient->audioGetSpace();
	}

	return ret;
}

int MplayerAudioOutput_NotifyPlay(int len)
{
	int ret = 0;
	//printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	if ( gMPlayerClient != 0 ) {
		//printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
		ret = gMPlayerClient->audioNotifyPlay(len);
		//printf("ret = %d\n",ret);
	}
	//printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	return ret;
}

int MplayerAudioOutput_ObtainShm(void)
{
	int ret = 0;
	if ( gMPlayerClient != 0 ) {
		ret = gMPlayerClient->audioObtainShm();
	}

	return ret;
}

int MplayerAudioOutput_FreeShm(void)
{
	int ret = 0;
	if ( gMPlayerClient != 0 ) {
		ret = gMPlayerClient->audioFreeShm();
	}

	return ret;
}


int MplayerAudioOutput_Play(int addr,int len)
{
	int ret = 0;
	if ( gMPlayerClient != 0 ) {
		ret = gMPlayerClient->audioPlay(addr, len);
	}

	return ret;
}

int MplayerAudioOutput_PlayChunk(int len, void* addr)
{
	int ret = 0;
	if ( gMPlayerClient != 0 ) {
		ret = gMPlayerClient->audioPlayChunk(len, addr);
	}

	return ret;
}

void MplayerAudioOutput_Pause(void)
{
	if ( gMPlayerClient != 0 ) {
		gMPlayerClient->audioPause();
		gMPlayerClient->audioStop();
	}
}

void MplayerAudioOutput_Start(void)
{
	if (gMPlayerClient != 0 ) {
		gMPlayerClient->audioStart();
	}

	return ;
}

void MplayerAudioOutput_Resume(void)
{
	MplayerAudioOutput_Start();
}

int MplayerAudioOutput_GetAudioSinkInfo(audioSink_info_t * as_i)
{
	LOGV("%s, Enter...", __FUNCTION__);
	MPLAYER_PRINTF("***   Enter...\n");
	if (gAudioOutput)
		gAudioOutput->GetAudioSinkInfo(as_i);
	return 0;
}

class MplayerAudioOutput * MplayerAudioOutput_GetDefaultAudioOutPut()
{
	LOGV("%s, Enter...", __FUNCTION__);
	MPLAYER_PRINTF("***   Enter...\n");

	if ( gAudioOutput != NULL )
		return gAudioOutput;

	LOGV("create MplayerAudioOutput()");
	MPLAYER_PRINTF("create MplayerAudioOutput()\n");
	gAudioOutput = new MplayerAudioOutput();
	return gAudioOutput;
}

void MplayerAudioOutput_Exit(void)
{
	LOGV("%s, Enter...", __FUNCTION__);
	MPLAYER_PRINTF("***   Enter...\n");
	if ( gAudioOutput == NULL )
		return ;

//	gAudioOutput->Uninit();
//	MPLAYER_PRINTF("**************   return....   ***************\n");
//	return ;

	LOGV("delete gAudioOutput");
	MPLAYER_PRINTF("delete gAudioOutput\n");
	delete gAudioOutput;
	gAudioOutput = NULL;
	MPLAYER_DBG();

}


