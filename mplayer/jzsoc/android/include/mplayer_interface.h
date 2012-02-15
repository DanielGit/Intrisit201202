
/*written by Cynthia */

#ifndef __MPLAYER_H__
#define __MPLAYER_H__

#ifdef __cplusplus
extern "C" {
#endif

	typedef enum
	{
		MP_STATE_UNKNOWN = -1,
		MP_STATE_IDLE=1,
		MP_STATE_INITIALIZED = 2,
		MP_STATE_PREPARED = 3,
		MP_STATE_STARTED=4,
		MP_STATE_STOPPED=5,
		MP_STATE_PAUSED=6,
		MP_STATE_RESTART=7,
		MP_STATE_ERROR=-2
	} MPlayerState;
	
	

	typedef struct {
		void   (*ConditionWait)  (void *datasource);
		void   (*ConditionSignal)  (void *datasource);
		int   (*SendEvent)  (void *datasource, int event_type);
		
		/* AudioSink interface */

		/* Surface, interface */

	} mplayer_callbacks;





typedef struct MPlayer_File {
	void            *datasource; /* Pointer to a FILE *, etc. */

	mplayer_callbacks callbacks;
} MPlayer_File;


inline void dump_MPlayerFile(MPlayer_File *f) 
{
	printf("MPlayer_File=%p\n", f);
	printf("datasource=%p\n", f->datasource);
	printf("callbacks.ConditionWait=%p\n", f->callbacks.ConditionWait);
	printf("callbacks.ConditionSignal=%p\n", f->callbacks.ConditionSignal);
	printf("callbacks.SendEvent=%p\n", f->callbacks.SendEvent);

}


	extern void exit_player(const char* how);
	extern void setPauseState();
	extern void setStartState();
	extern void setMPlayerStart();
	extern void setStopState();
	extern void setPlayFileFD(int fd);
	extern int  GetCurPlayTime();
	extern int  GetAudioCurPlayTime();
	extern int  GetDuration();
	extern void SetSeekTime(int f);
	extern void setlooptimes(int times);
	extern int  mplayer_main(MPlayer_File * mMPlayerFile, int argc,char* argv[]);//add
	extern int  mplayerIsPlayEnd(void);
	extern int  mplayerRestart(void);

	extern int mplayer_execute_command(int cmd, int ext1, int ext2);
	extern int mplayer_turnto_lcdc_original(void);//Emily 
#ifdef __cplusplus
} // extern "C"
#endif



#endif	/* __MPLAYER_H__ */
