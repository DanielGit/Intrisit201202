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
 **  ==========================================================================
 **  Modified:
 **      2009-07-21, Change to MPlayer process from thread mode. -- Wolfgang
 **
 **/

#ifndef ANDROID_MPLAYER_H
#define ANDROID_MPLAYER_H

#include <utils/threads.h> //include mutex\condition
#include <utils/Errors.h>

#include <media/IMPlayer.h>
#include <media/IMediaPlayerService.h>
//#include <media/MediaPlayerInterface.h>
#include <mplayer_interface.h>

namespace android {


	typedef enum
	{
		
		MP_STATE_IDLE=1,
		MP_STATE_INITIALIZED	= 2,
		MP_STATE_PREPARED = 3,
		MP_STATE_STARTED=4,
		MP_STATE_STOPPED=5,
		MP_STATE_PAUSED=6,
		MP_STATE_ERROR=7
	} MPlayerState;
	
	
	struct MyData {
		char * filename;
		int fd;
		int64_t offset;
		int64_t length;
	};

	
	class MPlayer : public BnMPlayer {
	public:
		
		MPlayer();
		~MPlayer();
		
		void                    run();


	private:

		/* *IMPlayer interface */
		virtual void            disconnect();
		
		virtual status_t        initCheck();
		virtual status_t        setDataSource(const char *url);
		virtual status_t        setDataSource(int fd, int64_t offset, int64_t length);
		
		virtual status_t        setVideoSurface(const sp<ISurface>& surface); /* Maybe not used in MPlayer Process */
		virtual status_t        prepareAsync();
		virtual status_t        start();
		virtual status_t        stop();
		virtual status_t        pause();
		virtual status_t        isPlaying(bool* state);
		virtual status_t        seekTo(int msec);
		virtual status_t        getCurrentPosition(int* msec);
		virtual status_t        getDuration(int* msec);
		virtual status_t        reset();
		virtual status_t        setAudioStreamType(int type);
		virtual status_t        setLooping(int loop);
		virtual status_t        setVolume(float leftVolume, float rightVolume);
		virtual status_t        IngenicPlayerInterface(int cmd,int ext1,int ext2);


		
	private:
		class DeathNotifier: public IBinder::DeathRecipient
		{
		public:
			DeathNotifier() {}
			virtual ~DeathNotifier();
			
			virtual void binderDied(const wp<IBinder>& who);
		};

		static void                  MP_ConditionWait (void * me);
		static void                  MP_ConditionSiganl (void * me);
		static int                   MP_SendEvent (void * me, int event_type);

		void                    sendEvent(int msg, int ext1=0, int ext2=0);
		int                     render();
		
		friend class DeathNotifier;
		static const sp<IMediaPlayerService>& getMediaPlayerService();
		static  sp<IMediaPlayerService>         sMediaPlayerService;
		static  sp<DeathNotifier>               sDeathNotifier;


		sp<IMPlayerClient>      mMPlayerClient;


		pid_t                        mTid;
		int                          mLoop;
		sp<ISurface>                 mSurface;
		status_t                     mInit;
		int                          mDuration;
		int                          mPlaytime;	
		char *                       datasource;
		MPlayerState                 mState;
		Mutex                        mMutex;
		Condition                    mCondition;

		bool 		                 dataSourceSeted;
		bool                         mInitFailed;
		bool                         released;
		bool                         mExit;
		bool                         mRender;
		int                          mSeekmode;
		MPlayer_File                 mMPlayerFile;
		struct MyData                mData;
		bool                         initialized;
		bool                         connected;
		bool                         reseted;

	};
		
}; // namespace android

#endif // ANDROID_MIDIFILE_H

	
