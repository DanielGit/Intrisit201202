/**  MPlayer interface
 ** 
 **   Android audio flinger output plugin for MPlayer
 ** 
 **  (C) Ingenic Semicondutor Inc.
 **  
 **  Author:  Cynthia Zhao <zhzhao@ingenic.cn>
 **      
 **  Any bugreports regarding to this code are welcome.
 **
 **/

#define LOG_NDEBUG 0
#define LOG_TAG "MPlayer.cpp"
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
#include <mplayer_interface.h>
#include "utils/IServiceManager.h"
#include <media/mediaplayer.h>
#include "mplayer_debug.h"



#ifdef HAVE_GETTID
static pid_t myTid() { return gettid(); }
#else
static pid_t myTid() { return getpid(); }
#endif



#if 0
#define MPLAYER_DBG()							\
	do {								\
		LOGV("%s[%d], %s()::", __FILE__, __LINE__, __FUNCTION__); \
		printf("%s[%d], %s()::\n", __FILE__, __LINE__, __FUNCTION__); \
	}while(0)

#define MPLAYER_PRINTF(fff, aaa...)					\
	do {								\
		LOGV("%s[%d], %s()::" fff, __FILE__, __LINE__, __FUNCTION__, ##aaa); \
		printf("%s[%d], %s()::\n" fff "\n", __FILE__, __LINE__, __FUNCTION__, ##aaa); \
	} while (0)

#define ENTER()								\
	do {								\
		LOGV("%s *** ENTER ***", __FUNCTION__);			\
		printf("%s[%d], %s():: *** ENTER ***\n", __FILE__, __LINE__, __FUNCTION__); \
	} while (0)


#define LEAVE()								\
	do {								\
		LOGV("%s *** LEAVE ***", __FUNCTION__);			\
		printf("%s[%d], %s():: *** LEAVE ***\n", __FILE__, __LINE__, __FUNCTION__); \
	} while (0)

#define MY_LOG(fff, aaa...)											\
	do {															\
		LOGD("%s(%d), " fff , __FUNCTION__, __LINE__, ##aaa);		\
		printf("%s(%d), " fff "\n", __FUNCTION__, __LINE__, ##aaa);	\
	} while (0)

#else
#define ENTER()	do {} while (0)
#define LEAVE()	do {} while (0)
#define MPLAYER_DBG()	do {} while (0)
#define MPLAYER_PRINTF(fff, aaa...)	do {} while (0)
#define MY_LOG(fff, aaa...)	do {} while (0)
#endif

namespace android {
	extern sp<IMPlayerClient>  gMPlayerClient;

	sp<IMediaPlayerService> MPlayer::sMediaPlayerService;
	sp<MPlayer::DeathNotifier> MPlayer::sDeathNotifier;


// establish binder interface to service
	const sp<IMediaPlayerService>& MPlayer::getMediaPlayerService()
	{
		MPLAYER_DBG();
//    Mutex::Autolock _l(sServiceLock);
		if (sMediaPlayerService.get() == 0) {
			sp<IServiceManager> sm = defaultServiceManager();
			sp<IBinder> binder;
			do {
				binder = sm->getService(String16("media.player"));
				if (binder != 0)
					break;
				LOGW("MediaPlayerService not published, waiting...");
				usleep(500000); // 0.5 s
			} while(true);
			
/*
  if (sDeathNotifier == NULL) {
  sDeathNotifier = new DeathNotifier();
  }
  MPLAYER_DBG();
  binder->linkToDeath(sDeathNotifier);
*/
			
			MPLAYER_DBG();
			sMediaPlayerService = interface_cast<IMediaPlayerService>(binder);
		}
		MPLAYER_DBG();
		LOGE_IF(sMediaPlayerService==0, "no MediaPlayerService!?");
		MPLAYER_DBG();
		return sMediaPlayerService;
	}
	

	MPlayer::MPlayer(): mPlaytime(-1),mDuration(-1),mTid(-1),mRender(false),mExit(false), released(false), mInitFailed(false)
	{
		ENTER();

		LOGV("MPlayer Constructor");
		datasource = NULL;
		mLoop = -1;
		dataSourceSeted = false;

		mState = MP_STATE_ERROR;
		mSeekmode = 0;
		memset(&mMPlayerFile, 0, sizeof mMPlayerFile);

		initialized = false;
		reseted = false;
		memset((void*)&mData, 0, sizeof(mData));
		mMPlayerClient = 0;

		mMPlayerFile.datasource = (void *)this;

		mplayer_callbacks callbacks = {
			(void (*)(void *)) MP_ConditionWait,
			(void (*)(void *)) MP_ConditionSiganl,
			(int (*)(void *,int)) MP_SendEvent,
		};

		mMPlayerFile.callbacks = callbacks;

		LEAVE();
	}
		
	
	MPlayer::~MPlayer() {
 
		LOGV("MPlayer destructor");

		if ( mInitFailed ) {
			return ;
		} 
//		mplayer_turnto_lcdc_original();//Emily
		printf("MPlayer.cpp: %d\n", __LINE__);
		if ( mData.filename ) {
		printf("MPlayer.cpp: %d\n", __LINE__);
			free(mData.filename );
			mData.filename = NULL;
		}

		if ( mData.fd ) {
			close(mData.fd);
		}

		printf("MPlayer.cpp: %d\n", __LINE__);
		LEAVE();
	}

	void MPlayer::run()
	{
		ENTER();

		// connect to MediaPlayerService
//		MPLAYER_PRINTF(" *********** sleep(1) **************");
//		sleep(1);

		const sp<IMediaPlayerService>& service(getMediaPlayerService());

		MPLAYER_PRINTF("service.get()=%p", service.get());

		MPLAYER_DBG();

		if (service != 0) {
			int timeout = 300;	// 3 sec
			while ( mMPlayerClient == 0 && timeout ) 
			{
				MPLAYER_PRINTF("*** Warnning check mMPlayerClient !!! ***");
				MPLAYER_PRINTF("=== MPlayer::this=%p", this);

				mMPlayerClient = service->connectMPlayerClient(getpid(), this);
//				mMPlayerClient = service->connectMPlayerClient(player_pid, this);
				gMPlayerClient = mMPlayerClient;
				timeout--;
				usleep(10000);	// sleep 10ms
			}

			if ( timeout == 0 ) {
				MPLAYER_PRINTF("connectMPlayerClient failed....");
				return;
			}
		}
		else {
			initialized = false;
			MPLAYER_PRINTF("*** Failed connect to media service, exit....");
			return ; // exit MPlayer process
		}

		initialized = true;
		render();
		LEAVE();
	}


	void MPlayer::sendEvent(int msg, int ext1, int ext2)
	{
		ENTER();
		if ( mMPlayerClient != 0 )
			mMPlayerClient->sendEvent(msg, ext1, ext2);
		LEAVE();
//		return NO_ERROR;
	}

	void MPlayer::disconnect()
	{
		ENTER();

		LEAVE();
	}

	status_t MPlayer::initCheck()
	{
		LOGV("MPlayer initcheck");
		ENTER();



		LEAVE();

		return NO_ERROR;//if return 1, ~MPlayer will be called.
	}

	status_t MPlayer::setDataSource(const char* url)
	{
		LOGV("MPlayer::setDataSource url=%s", url);

		Mutex::Autolock lock(mMutex);
		if ( mData.filename ) {
			free(mData.filename);
			mData.filename = NULL;
		}

		if(NULL == url )
			return UNKNOWN_ERROR;
		mData.filename= strdup(url);
		mData.fd = 0;

		MPLAYER_PRINTF("wake up reader thread, mCondition.signal");
		dataSourceSeted = true;
		mCondition.signal();
	
		return NO_ERROR;
	}
	
	status_t MPlayer::setDataSource(int fd, int64_t offset, int64_t length)
	{
		LOGV("MPlayer::setDataSource fd=%d,offset=%lld,length=%lld", fd, offset, length);
		Mutex::Autolock lock(mMutex);


		if ( mData.filename ) {
			free(mData.filename);
			mData.filename = NULL;
		}

//		mData.fd = dup(fd);
		lseek(fd,0,SEEK_SET);	// reset offset to 0
		mData.fd = fd;
		mData.offset = offset;
		mData.length = length;

		setPlayFileFD(fd);

		MPLAYER_PRINTF("wake up reader thread, mCondition.signal");
		dataSourceSeted = true;
		mCondition.signal();


		return NO_ERROR;
	}

	status_t MPlayer::setVideoSurface(const sp<ISurface>& surface)
	{
		LOGV("setVideoSurface(%p)", surface.get());
//		mSurface = surface;
		return NO_ERROR;
		
	}
	

	status_t MPlayer::prepareAsync()
	{
		LOGV("MPlayer::prepareAsync");

		return NO_ERROR;
	}

	
	status_t MPlayer::start()
	{
		LOGV("MPlayer::start");
//		Mutex::Autolock lock(mMutex);

		if (mState == MP_STATE_PAUSED){
			MPLAYER_DBG();
			mState=MP_STATE_STARTED;  
			setStartState();
			MPLAYER_DBG();
//			return NO_ERROR;	
		}

		if ( mplayerIsPlayEnd() == 1 ) {
			MPLAYER_PRINTF("mplayerIsPlayEnd(), mplayerRestart\n");
			mplayerRestart();
		}


		MPLAYER_DBG();
		mState = MP_STATE_STARTED;  

		MPLAYER_PRINTF("wake up reader thread, mCondition.signal\n");
		setMPlayerStart();
		mCondition.signal();	// wake up prepare, pause loop

		MPLAYER_DBG();

		return NO_ERROR;

	}
	
	status_t MPlayer::stop()
	{
		LOGV("MPlayer::stop");

		setStopState();

		MPLAYER_PRINTF("mState=%d\n", mState);

/*
		// paused, pre-started
		if ( mState != MP_STATE_STARTED ) {
			mCondition.signal();	// wake up prepare, pause loop
		}
*/

		LOGV("wakeup mplayer thread...");
		MPLAYER_PRINTF("wakeup mplayer thread...\n");
		mCondition.signal();	// wake up prepare, pause loop

		mState=MP_STATE_STOPPED;
//		printf("wait mplayer thread, sleep 1sec\n");		sleep(1);

		return NO_ERROR;

	}
	
	
	status_t MPlayer::seekTo(int msec)
	{
		LOGV("MPlayer::seekTo %d", msec);
		if ((msec == mDuration) && (mDuration > 0)) {
			msec--;
			LOGV("MPlayer::Seek adjusted 1 msec from end");
		}
		if( mSeekmode == 0) {
			mSeekmode = 1;
		}
		SetSeekTime(msec);
		mPlaytime = msec;
		return NO_ERROR;
	}
	
	status_t MPlayer::pause()
	{
		LOGV("MPlayer::pause");
		if ( mState == MP_STATE_STARTED ) {
			mState=MP_STATE_PAUSED;
			setPauseState();
		}
	
		return NO_ERROR;
		

	}
	
	status_t MPlayer::isPlaying(bool * state)
	{
		LOGV("MPlayer::isPlaying, mState=%d", int(mState));

		if ( mplayerIsPlayEnd() ) {
			* state = false;
			return NO_ERROR;
		}

		if( mState == MP_STATE_STARTED ) {
			*state = true;
			return NO_ERROR;
		}

		* state = false;

		return NO_ERROR;
	}
	
	status_t MPlayer::getCurrentPosition(int* msec)
	{
	

		LOGV("getCurrentPosition(): mPlayTime = %d", mPlaytime );

		if ( mplayerIsPlayEnd() ) {
			LOGV("mplayer play end, curPosition=duration");
			MPLAYER_PRINTF("mplayer play end, curPosition=duration\n");
			* msec = mDuration;
			return NO_ERROR;
		}

		if ( mSeekmode == 0 ) {
			MPLAYER_DBG();
			mPlaytime = GetCurPlayTime();
		}

		if( mPlaytime < 0 ) {
			LOGD("%s mPlaytime(%d) error.", __FUNCTION__, mPlaytime);
			MPLAYER_MSG("mPlaytime(%d) error.\n", mPlaytime);
//			mState = MP_STATE_ERROR;
//			return MP_STATE_ERROR;
			mPlaytime = 0 ;
		}
		

		* msec = mPlaytime;
		
		MPLAYER_PRINTF("getCurrentPosition(): mPlayTime = %d\n", mPlaytime );
		
	
		LOGV("getCurrentPosition(): mPlayTime = %d", mPlaytime );

		return NO_ERROR;

	}
	
	status_t MPlayer::getDuration(int* msec)
	{
		
		LOGV("MPlayer::getDuration");
			
		mDuration = GetDuration();

		*msec = mDuration;
		LOGV("MPlayer::getDuration  %d",mDuration);
		MPLAYER_PRINTF("MPlayer::getDuration  %d \n",mDuration);

		return NO_ERROR;
	}

	status_t MPlayer::reset()
	{
		LOGV("MPlayer::reset");
		ENTER();
		if ( reseted ) 
			return NO_ERROR;

		stop();
//		mSurface.clear();
		mPlaytime= -1;
		mDuration= -1;
		mLoop = -1;

		mMPlayerClient.clear();
		reseted = true;
		LEAVE();

		return NO_ERROR;
	
	}
	
	status_t MPlayer::setLooping(int loop)
	{
		LOGV("MPlayer::setLooping %d",loop);
		Mutex::Autolock lock(mMutex);
		mLoop = loop ? -1:0;   //if mLoop=-1, the music will repeat;if 0,the music will run only once. 
		MPLAYER_PRINTF("MPlayer :: mLoop = %d \n",mLoop);
//		setlooptimes(mLoop);
		return NO_ERROR;

	}

	status_t MPlayer::setAudioStreamType(int type)
	{
		ENTER();

		LEAVE();
		return NO_ERROR;
	}

	status_t MPlayer::setVolume(float leftVolume, float rightVolume)
	{
		ENTER();

		LEAVE();
		return NO_ERROR;
	}


	status_t MPlayer:: IngenicPlayerInterface(int cmd,int ext1,int ext2)
	{
		LOGV("###  MPlayer ::IngenicPlayerInterface is called %d(cmd) ",cmd);
		return mplayer_execute_command(cmd, ext1, ext2);
	}



	void MPlayer::MP_ConditionWait(void * me) {
		MPlayer * self = (MPlayer *) me;
		ENTER();
//		Mutex::Autolock lock(self->mMutex);
		MPLAYER_DBG();

		self->mCondition.wait(self->mMutex);

		LEAVE();


	}


	void MPlayer::MP_ConditionSiganl (void * me) {
		MPlayer * self = (MPlayer *) me;
		ENTER();

//		Mutex::Autolock lock(self->mMutex);
		MPLAYER_DBG();

		self->mCondition.signal();

		LEAVE();


	}


	int MPlayer::MP_SendEvent(void * me, int event_type) {
		MPlayer * self = (MPlayer *) me;
		ENTER();

		switch ( event_type ) {
		case MEDIA_SEEK_COMPLETE:
			self->mSeekmode = 0;
			MPLAYER_PRINTF("sendevent seek complete...\n");
			break;
		}

		self->sendEvent(event_type,0,0);

		LEAVE();
		return 0;
	}


	int  MPlayer::render() {
		int ret;
		
		LOGV("create start_mplayer pthread");

		mMutex.lock();
		if ( !dataSourceSeted )
		{
			MPLAYER_PRINTF("signal wait -- wait setdatasource command\n");
//			printf("signal wait -- wait setdatasource command\n");
			mCondition.wait(mMutex);
			MPLAYER_PRINTF("signal wakuped by setdatasource command\n");
//			printf("signal wakuped by setdatasource command\n");
		}

		mMutex.unlock();
		


		int argc;
		char * argv[] = {
			"mplayer_main",
			"use_opened_fd",
			NULL
		};

		argc = sizeof(argv)/sizeof(char*);

		if( mData.fd <= 0 )
			argv[1] = mData.filename;
		

		ret = mplayer_main(&mMPlayerFile, argc, argv);

		MPLAYER_DBG();

		mRender = false;
		
		LEAVE();
		return ret;
	}

	void MPlayer::DeathNotifier::binderDied(const wp<IBinder>& who) {
		ENTER();
		LOGW("MediaPlayer server died!");
#if 0
/*
		// Need to do this with the lock held
		SortedVector< wp<MPlayer> > list;
		{
			Mutex::Autolock _l(MPlayer::sServiceLock);
			MediaPlayer::sMediaPlayerService.clear();
			list = sObitRecipients;
		}

		// Notify application when media server dies.
		// Don't hold the static lock during callback in case app
		// makes a call that needs the lock.
		size_t count = list.size();
		for (size_t iter = 0; iter < count; ++iter) {
			sp<MediaPlayer> player = list[iter].promote();
			if ((player != 0) && (player->mPlayer != 0)) {
				player->notify(MEDIA_ERROR, MEDIA_ERROR_SERVER_DIED, 0);
			}
		}
*/
#endif
	}

	MPlayer::DeathNotifier::~DeathNotifier()
	{
		ENTER();
#if 0
/*
		Mutex::Autolock _l(sServiceLock);
		sObitRecipients.clear();
		if (sMediaPlayerService != 0) {
			sMediaPlayerService->asBinder()->unlinkToDeath(this);
		}
*/
#endif
	}


} // end namespace android
