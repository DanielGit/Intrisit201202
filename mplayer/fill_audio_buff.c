#define PRIO_AUDIO_DECODER  79
#include "os_api.h" 
static volatile int Audio_Decode_Run = 0;
static int audio_pre_dec_count = 0;
static OS_EVENT *audio_decode_sem = 0;
static OS_EVENT *audio_locked = 0;
static int audio_decode_delay = 0;

#if MP_STATISTIC
static int free_audio_decode=0;
static int haveto_audio_decode=0;
#endif

#define OP_AU_LOCK() do{						\
							 char  err; \
						os_SemaphorePend(audio_locked, 0, &err);  \
						}while(0)
#define OP_AU_UNLOCK() do{ \
							os_SemaphorePost(audio_locked);  \
							}while(0)

static int dec_audio_prebuf_time = 0;
static int audio_buf_trig = 1;
static int audio_buf_fill_count = 1;

static unsigned int init_buf_fill_count = 0;

static void fill_audio_param_reset(){
    audio_buf_trig = 1;
    audio_buf_fill_count = 1;
    init_buf_fill_count = 0;
    
}
//#undef printf
static int fill_audio_out_buffers(int pre_dec_count)
{
    unsigned int t;
    int playsize;
    int playflags=0;
    int audio_eof=0;
    int bytes_to_write;
    int dec_audio_size;
    int init_dec_audio_prebuf_time = 0;
    sh_audio_t * const sh_audio = mpctx->sh_audio;
	  int dec_audio_time = GetTimer();	
		
    current_module="play_audio";
		if(dec_audio_prebuf_time == 0) init_dec_audio_prebuf_time = GetTimer();
    while (1) {
		// all the current uses of ao_data.pts seem to be in aos that handle
		// sync completely wrong; there should be no need to use ao_data.pts
		// in get_space()
		ao_data.pts = ((mpctx->sh_video?mpctx->sh_video->timer:0)+mpctx->delay)*90000.0;
		bytes_to_write = mpctx->audio_out->get_space();
        F("bytes_to_write = %d\n",bytes_to_write);
		if(pre_dec_count == 0)
		{
			if(bytes_to_write >= ao_data.buffersize - ao_data.outburst*audio_buf_trig)
			{
                if((bytes_to_write >= ao_data.buffersize - ao_data.outburst) && (init_buf_fill_count > 2 ))
                {
                    if((audio_buf_trig > 1)&&(audio_buf_fill_count < 3)) audio_buf_fill_count++;
                    
                    if(audio_buf_trig < 4)
                    {
                        audio_buf_trig++;
                        F("audio_buf will break audio_buf_trig = %d!\n",audio_buf_trig);
                        
                    }
                    bytes_to_write = ao_data.outburst * audio_buf_fill_count;
                }else
                    bytes_to_write = ao_data.outburst * audio_buf_fill_count;
                break;
			}else
				return 1; 
		}else
		{
			if(bytes_to_write > (int)ao_data.outburst * pre_dec_count)
			{
				bytes_to_write = (int)ao_data.outburst * pre_dec_count;
			}else
			{
				//bytes_to_write = bytes_to_write / ao_data.outburst * ao_data.outburst;
			}	 
			break;
	  }
       
		if (mpctx->sh_video || bytes_to_write >= ao_data.outburst)
			break;

		// handle audio-only case:
		// this is where mplayer sleeps during audio-only playback
		// to avoid 100% CPU use
		usec_sleep(10000); // Wait a tick before retry
    }
    OP_AU_LOCK();
    init_buf_fill_count++;
    dec_audio_size = bytes_to_write;
    while (bytes_to_write) {
		playsize = bytes_to_write;
		if (playsize > MAX_OUTBURST)
			playsize = MAX_OUTBURST;
		bytes_to_write -= playsize;

		//Fill buffer if needed:
		current_module="decode_audio";
		t = GetTimer();
		while (sh_audio->a_out_buffer_len < playsize) {
			int buflen = sh_audio->a_out_buffer_len;
			#if MP_STATISTIC
		  int av_dec_time;
			av_dec_time = GetTimer();
			#endif
			int ret = decode_audio(sh_audio, (unsigned char *)&sh_audio->a_out_buffer[buflen],
								   playsize - buflen, // min bytes
								   sh_audio->a_out_buffer_size - buflen // max
                                   );
      #if MP_STATISTIC
      if(ret > 0)
			{
				if(pre_dec_count > 0)
				{
					free_audio_decode += ret;
				}else
					haveto_audio_decode += ret;
			}
			if(ret > 0)
			{	
				av_dec_time = (GetTimer() - av_dec_time);
				if(av_dec_time > 0)
				{
					av_a_dec_count++;
					av_a_dec_time += av_dec_time;
				}
		  }
			#endif                             
			if (ret <= 0) { // EOF?
				if (mpctx->d_audio->eof) {
					audio_eof = 1;
					if (sh_audio->a_out_buffer_len == 0)
					{
						 OP_AU_UNLOCK();	
						 return 0;
				  }
				}
				break;
			}
	    
			sh_audio->a_out_buffer_len += ret;
	    
		}
		t = GetTimer() - t;
		audio_time_usage+=t;
		if (playsize > sh_audio->a_out_buffer_len) {
			playsize = sh_audio->a_out_buffer_len;
			if (audio_eof)
				playflags |= AOPLAY_FINAL_CHUNK;
			
		}
		if (!playsize)
			break;
   
		// play audio:  
		current_module="play_audio";

		// Is this pts value actually useful for the aos that access it?
		// They're obviously badly broken in the way they handle av sync;
		// would not having access to this make them more broken?
		ao_data.pts = ((mpctx->sh_video?mpctx->sh_video->timer:0)+mpctx->delay)*90000.0;
		playsize = mpctx->audio_out->play(sh_audio->a_out_buffer, playsize, playflags);

		if (playsize > 0) {
			sh_audio->a_out_buffer_len -= playsize;
			memmove(sh_audio->a_out_buffer, &sh_audio->a_out_buffer[playsize],
					sh_audio->a_out_buffer_len);
			//memmove(sh_audio->a_out_buffer, &sh_audio->a_out_buffer[playsize],
			//  sh_audio->a_out_buffer_len);
			mpctx->delay += playback_speed*playsize/(double)ao_data.bps;
		}
		else if (audio_eof && mpctx->audio_out->get_delay() < .04) {
			// Sanity check to avoid hanging in case current ao doesn't output
			// partial chunks and doesn't check for AOPLAY_FINAL_CHUNK
			mp_msg(MSGT_CPLAYER, MSGL_WARN, "Audio output truncated at end.\n");
			sh_audio->a_out_buffer_len = 0;
		}
    }
    if(dec_audio_prebuf_time == 0) 
    {
    		dec_audio_prebuf_time = GetTimer() - init_dec_audio_prebuf_time;
    		dec_audio_prebuf_time = dec_audio_prebuf_time / ( dec_audio_size / ao_data.outburst);
    		dec_audio_prebuf_time = (dec_audio_prebuf_time * 110) / 100;
    		//dec_audio_prebuf_time = 4000;
    		//printf("dec_audio_prebuf_time = %d\n",dec_audio_prebuf_time);
    		//printf("dec_audio_size = %d\n",dec_audio_size);
    		//printf("ao_data.outburst = %d\n",ao_data.outburst);
    		
    }
    dec_audio_time = GetTimer() - dec_audio_time;
   // if(audio_pre_dec_count && (dec_audio_time > audio_pre_dec_count * dec_audio_prebuf_time))
   // {
    	//printf("dec_audio_time = %d audio_pre_dec_count * dec_audio_prebuf_time = %d\n",dec_audio_time,audio_pre_dec_count * dec_audio_prebuf_time);
  //  }
    OP_AU_UNLOCK();
    return 1;
}
static int fill_audio_out_buffers_foraudio()
{
    int playsize;
    int playflags=0;
    int audio_eof=0;
    int bytes_to_write;
    sh_audio_t * const sh_audio = mpctx->sh_audio;

    current_module="play_audio";

    while (1) {
		// all the current uses of ao_data.pts seem to be in aos that handle
		// sync completely wrong; there should be no need to use ao_data.pts
		// in get_space()
		ao_data.pts = ((mpctx->sh_video?mpctx->sh_video->timer:0)+mpctx->delay)*90000.0;
		bytes_to_write = mpctx->audio_out->get_space();
		
		if (mpctx->sh_video || bytes_to_write >= ao_data.outburst)
			break;

		// handle audio-only case:
		// this is where mplayer sleeps during audio-only playback
		// to avoid 100% CPU use
		usec_sleep(10000); // Wait a tick before retry
    }
    while (bytes_to_write) {
		playsize = bytes_to_write;
		if (playsize > MAX_OUTBURST)
			playsize = MAX_OUTBURST;
		bytes_to_write -= playsize;

		//Fill buffer if needed:
		current_module="decode_audio";
			while (sh_audio->a_out_buffer_len < playsize) {
			int buflen = sh_audio->a_out_buffer_len;
			int ret = decode_audio(sh_audio, (unsigned char *)&sh_audio->a_out_buffer[buflen],
								   playsize - buflen, // min bytes
								   sh_audio->a_out_buffer_size - buflen // max
                                   );
      if (ret <= 0) { // EOF?
				
				if (mpctx->d_audio->eof) {
					audio_eof = 1;
					if (sh_audio->a_out_buffer_len == 0)
						return 0;
				}
				break;
			}
	    
			sh_audio->a_out_buffer_len += ret;
	    
		}
		if (playsize > sh_audio->a_out_buffer_len) {
			playsize = sh_audio->a_out_buffer_len;
			if (audio_eof)
				playflags |= AOPLAY_FINAL_CHUNK;
			
		}
		if (!playsize)
			break;
   
		// play audio:  
		current_module="play_audio";

		// Is this pts value actually useful for the aos that access it?
		// They're obviously badly broken in the way they handle av sync;
		// would not having access to this make them more broken?
		ao_data.pts = ((mpctx->sh_video?mpctx->sh_video->timer:0)+mpctx->delay)*90000.0;
		playsize = mpctx->audio_out->play(sh_audio->a_out_buffer, playsize, playflags);

		if (playsize > 0) {
			sh_audio->a_out_buffer_len -= playsize;
			memmove(sh_audio->a_out_buffer, &sh_audio->a_out_buffer[playsize],
					sh_audio->a_out_buffer_len);
			//memmove(sh_audio->a_out_buffer, &sh_audio->a_out_buffer[playsize],
			//  sh_audio->a_out_buffer_len);
			mpctx->delay += playback_speed*playsize/(double)ao_data.bps;
		}
		else if (audio_eof && mpctx->audio_out->get_delay() < .04) {
			// Sanity check to avoid hanging in case current ao doesn't output
			// partial chunks and doesn't check for AOPLAY_FINAL_CHUNK
			mp_msg(MSGT_CPLAYER, MSGL_WARN, "Audio output truncated at end.\n");
			sh_audio->a_out_buffer_len = 0;
		}
    }
    return 1;
}

static int fill_audiobuffers(int pre_dec_count)
{
	int ret;
	//printf("fill_audiobuffers\n");
	ret = fill_audio_out_buffers(pre_dec_count);
	return ret;
}
static void stop_audiothread();

static void sync_audio_thread(){
    //stop_audiothread();
    OP_AU_LOCK();
    if(mpctx->sh_audio && mpctx->sh_audio->ds){
        
        demuxer_free_packs_buffer(mpctx->sh_audio->ds);
    }
    OP_AU_UNLOCK();
}
static unsigned int audio_savereg[2];
static int audio_excptid = 0;
static void audio_thread_excpt(void);
//#undef printf
void audio_thread_run(){
    unsigned char err;
	int ret;
	int dec_count;
#if EXCEPTION_MPLAYER
	audio_excptid = AddExcept(audio_thread_excpt,audio_savereg);
    printf("audio_savereg = %x %x\n",	audio_savereg[0],audio_savereg[1]);
#endif	
    while(Audio_Decode_Run == 1)
	{	
		os_SemaphorePend(audio_decode_sem,audio_decode_delay,&err);	
		//printf("audio_decode_sem = %x audio_decode_delay = %d\n",audio_decode_sem,audio_decode_delay);
		if(mpctx->sh_audio)
		{	
		//printf("aaaa\n");
            dec_count = audio_pre_dec_count;
            ret = fill_audiobuffers(dec_count);
        }
        
	}
#if EXCEPTION_MPLAYER
	DecExcept(audio_excptid,audio_savereg);
	F("audio thread exit\n");
	excpt_exit(0,audio_savereg);
#endif

}



void audio_thread_excpt(void);
void c_audio_thread_run();
void (*c_audio_thread_run_fun)() = c_audio_thread_run;

static void audio_decode_function(void *pdata)
{
	pdata = pdata;
	//printf("ret = %x\n",&ret);
    c_audio_thread_run_fun();
	Audio_Decode_Run = 2;
	while(1) usec_sleep(10000);
}
void c_audio_thread_run(){
#if EXCEPTION_MPLAYER
	SAVE_PROCESS_REGISTER(audio_savereg);
#endif
    return audio_thread_run();
    
}
void audio_thread_excpt(void){
    char err;
    
    mpctx->eof = 1;
    exit_player_mode = 0;
    os_SemaphoreSet(audio_decode_sem,0,&err);
    os_SemaphoreSet(audio_locked,1,&err);
	Audio_Decode_Run = 2;
	while(1) usec_sleep(10000);
    excpt_exit(0,audio_savereg);
}
void* CreateAudioThread(int stksize)
{
	if(Audio_Decode_Run) return 0;
	OS_TASK_STACK   *audiostk;
	audiostk = (OS_TASK_STACK *)memalign(32,stksize);
	//printf("audiostk = %x\n",audiostk);
	if(audiostk == 0) return 0;
	Audio_Decode_Run = 1;
	audio_decode_sem = os_SemaphoreCreate(0);
	//printf("audio_decode_sem create = %x\n",audio_decode_sem);
	if(audio_decode_sem == 0) return 0;
	
	audio_locked = os_SemaphoreCreate(1);
	if(audio_locked == 0) return 0;
		
	if(os_TaskCreateEx(audio_decode_function,(void * )0,(audiostk + stksize / 4 - 1),
                  						PRIO_AUDIO_DECODER,
                  						PRIO_AUDIO_DECODER,
                  						audiostk,
                  						stksize/4,
                  						(void *)0,
                  						OS_TASK_CREATE_EX_STK_CHK | OS_TASK_CREATE_EX_STK_CLR) == 0)
  {
       printf("Create Audio Thread!\n");
       return audiostk;
  }
  return 0;

}
void FreeThread(void *stk)
{
	unsigned char err;
	if(Audio_Decode_Run == 1)
	{
		Audio_Decode_Run = 0;
		os_SemaphorePost(audio_decode_sem);
		while(Audio_Decode_Run != 2)
		{
			usec_sleep(10000);
		} 
	}
	os_TaskDelete(PRIO_AUDIO_DECODER);

	Audio_Decode_Run = 0;
	if(audio_decode_sem)
		os_SemaphoreDelete(audio_decode_sem,0,&err); 
	if(audio_locked)
		os_SemaphoreDelete(audio_locked,0,&err);
	if(stk)
		free(stk);
}
void Excpt_Audio()
{
	unsigned char err;
	audio_decode_delay = 0;
	if(Audio_Decode_Run)
		os_TaskDelete(PRIO_AUDIO_DECODER);
	Audio_Decode_Run = 0;
	if(audio_decode_sem)
		os_SemaphoreDelete(audio_decode_sem,0,&err);
	if(audio_locked)
		os_SemaphoreDelete(audio_locked,0,&err);
}
//因为线程优先级可以这样简单的做
static void start_audiothread()
{
	if(!mpctx->sh_audio) return;

	audio_pre_dec_count = 0;
	audio_decode_delay = 1;
	os_SemaphorePost(audio_decode_sem);
	//printf("ddddd\n");
}
static void stop_audiothread()
{
	if(!mpctx->sh_audio) return;
	audio_decode_delay = 0;
	os_SemaphorePost(audio_decode_sem);
}

typedef enum __CLOCK_DIV__
{
	CPM_CDIV = 0x00,
	CPM_H0DIV = 0x04,
	CPM_PDIV = 0x08,
	CPM_MDIV = 0x0C,
	CPM_H1DIV = 0x10
} CLKDIV, *PCLKDIV;

unsigned int GetCommonClock(CLKDIV clk);

void set_release_cycle(int cycle)
{
    //printf("cycle = %d\n",cycle);
	if(dec_audio_prebuf_time)
	{
       
        
		int cycletime = (cycle - 6000) / (GetCommonClock(CPM_CDIV) / 1000000); 
		int tmp  = cycletime / dec_audio_prebuf_time;
		audio_pre_dec_count = tmp;
  }
	//printf("cycle = %d audio_pre_dec_count = %d dec_audio_prebuf_time = %d\n",cycle,audio_pre_dec_count,dec_audio_prebuf_time);
}
