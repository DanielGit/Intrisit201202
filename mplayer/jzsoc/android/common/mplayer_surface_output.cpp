#define LOG_NDEBUG 0
#define LOG_TAG "MPlayerSurfaceOut"
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
#include <media/MPlayer.h>
#include "mplayer_interface.h"
#include "config.h"
#include "mplayer_surface_output.h"


namespace android {

	
	class MplayerSurfaceOutput * MplayerSurfaceOutput_instance = NULL;
	sp<ISurface>                MplayerSurfaceOutput::mSurface = NULL;
	
	
	MplayerSurfaceOutput::MplayerSurfaceOutput()
	{
		LOGV("MplayerSurfaceOutput constructor\n");
		iScreenWidth = 480;
		iScreenBytesPerPixel = 16/8;
		iVideoDisplayWidth = 320;
		iVideoDisplayHeight = 240;
		iVideoWidth = 320;
		iVideoHeight = 240;
	
		mInitialized = false;
		MPLAYER_DBG();
	}
	
	
	MplayerSurfaceOutput::~MplayerSurfaceOutput()
	{
		LOGV("MplayerSurfaceOutput %s\n", __FUNCTION__);
	
		/* free memory ... */
	    if (mSurface.get()) {
	        LOGV("unregisterBuffers");
	        mSurface->unregisterBuffers();
	        mSurface.clear();
	    }
	
	
	}
	
	void MplayerSurfaceOutput::setVideoSurface(const sp<ISurface>& surface)
	{
		LOGV("MplayerSurfaceOutput setVideoSurface\n");
		LOGV("surface.get() = (%p)", surface.get());
	//	LOGV("mSurface.get()=(%p)", mSurface.get());
	
		MPLAYER_PRINTF("Build Date: %s %s\n",  __DATE__, __TIME__);
	
		mSurface = surface;
	
	}
	
	
	void MplayerSurfaceOutput::init()
	{
		LOGV("MplayerSurfaceOutput init\n");
	    int displayWidth = iVideoDisplayWidth;
	    int displayHeight = iVideoDisplayHeight;
	    int frameWidth = iVideoWidth;
	    int frameHeight = iVideoHeight;
	    int frameSize;
	
		MPLAYER_DBG(); 
		// RGB-565 frames are 2 bytes/pixel
		displayWidth = (displayWidth + 1) & -2;
		displayHeight = (displayHeight + 1) & -2;
		frameWidth = (frameWidth + 1) & -2;
		frameHeight = (frameHeight + 1) & -2;
		frameSize = frameWidth * frameHeight * iScreenBytesPerPixel;
	
		MPLAYER_PRINTF("frameSize=%d\n", frameSize);
		LOGV("frameSize=%d\n", frameSize);
		
		// create frame buffer heap and register with surfaceflinger
		mFrameHeap = new MemoryHeapBase(frameSize);
		if (mFrameHeap->heapID() < 0) {
			LOGE("Error creating frame buffer heap");
			return;
		}
	
		MPLAYER_DBG();
		MPLAYER_ERR("*********** skiped registerBuffers ****");
//		mSurface->registerBuffers(displayWidth, displayHeight, frameWidth, frameHeight, PIXEL_FORMAT_RGB_565, mFrameHeap);
	
		mInitialized = true;
	
	    // update app
	    // mPvPlayer->sendEvent(MEDIA_SET_VIDEO_SIZE, displayWidth, displayHeight);
	
	
	}
	
	
		static void inline change_buffer_color(unsigned char * buff, int size)
		{
			static int flag = 0;
			unsigned short color;
			unsigned short *buff_16 = (unsigned short *)buff;
	
			switch ( flag++ ) {
			case 0:
				color = 0xF800;
				break;
			case 1:
				color = 0x03E0;
				break;
			case 2:
				color = 0x001F;
				break;
			default:
				color = 0x0000;
				break;
			}
	
			if ( flag > 5 ) flag = 0;
	
	//		size /= 2;
			int cnt = (size/4)/2;	// 1/4 of buff size
			static int area1_flag = 1;
			if ( area1_flag ) {
				while (cnt--) {
					*buff_16++ = 0xFFFF;
				}
			}
	
			area1_flag = 0;
	
			buff_16 = (unsigned short *)(buff + size/4);
			cnt = ((size/4)*3)/2;
			while (cnt--) {
				*buff_16++ = color;
			}
		}
	
	
	static void* wolfgang_start_mplayer(void *surface)
		{
			MPLAYER_DBG();
	
			sp<MemoryHeapBase>          mFrameHeap;
	
	//		int displayWidth = 320;
	//		int displayHeight = 240;
			int displayWidth = 320;
			int displayHeight = 240;
			int frameWidth = 320;
			int frameHeight = 240;
			int frameSize;
	
			frameSize = frameWidth * frameHeight * 2;
	
			mFrameHeap = new MemoryHeapBase(frameSize);
			if (mFrameHeap->heapID() < 0) {
				LOGE("Error creating frame buffer heap");
				return NULL;
			}
			MPLAYER_DBG();
	
			MPLAYER_ERR("*********** Skipped registerBuffers ****");
//			int ret = 	MplayerSurfaceOutput::mSurface->registerBuffers(displayWidth, displayHeight, frameWidth, frameHeight, PIXEL_FORMAT_RGB_565, mFrameHeap);
	
			MPLAYER_PRINTF("registerbuffers ret=%d\n", ret);
			
			while (1) {
				LOGV("mSurface->postBuffer() dispalyer: %dx%d <<-- frame:%dx%d\n", displayWidth, displayHeight, frameWidth, frameHeight);
				MPLAYER_PRINTF("mSurface->postBuffer() dispalyer: %dx%d <<-- frame:%dx%d\n", displayWidth, displayHeight, frameWidth, frameHeight);
	//			MPLAYER_PRINTF("mSurface->postBuffer(),  320x240-->320x240, mFrameHeap->getSize()=%d, mFrameHeap->virtualSize()=%d, mFrameHeap->heapID()=%d\n", mFrameHeap->getSize(), mFrameHeap->virtualSize(), mFrameHeap->heapID());
	
				sleep(1);
				change_buffer_color((unsigned char *)mFrameHeap->base(), frameSize);
				MplayerSurfaceOutput::mSurface->postBuffer(0);
			}
	
		}
	
	void MplayerSurfaceOutput::surfaceTest( )
	{
		LOGV("MplayerSurfaceOutput %s\n", __FUNCTION__);
		MPLAYER_DBG();
	
		pthread_t ntid;
		pthread_create(&ntid , NULL , wolfgang_start_mplayer, (void*)NULL);  
	}
	
	void MplayerSurfaceOutput::writeFrameBuffer(unsigned char * img_buf, int size)
	{
		LOGV("MplayerSurfaceOutput %s\n", __FUNCTION__);
		if ( !mInitialized ) {
			LOGD("MplayerSurfaceOutput not init");
			MPLAYER_PRINTF("MplayerSurfaceOutput not init\n");
			return;
		}
	
		MPLAYER_PRINTF("update video surface...\n");
#if 0
		change_buffer_color((unsigned char *)mFrameHeap->base(), 320*240*2);
#else
		{
			MPLAYER_PRINTF("img_buf addr=%p, &mInitialized=%p\n", img_buf, &mInitialized);
//			memcpy ((void *)mFrameHeap->base(), (void *)img_buf, size);  
			int w, h, dst_stride, src_stride;
			unsigned char * src_addr = (unsigned char  *)img_buf;
			unsigned char  * dst_addr = (unsigned char  *)mFrameHeap->base();

			h = iVideoHeight;
			dst_stride = iVideoWidth*iScreenBytesPerPixel;
			src_stride = iScreenWidth*iScreenBytesPerPixel;

			while (h--) {
				memcpy ((void *)dst_addr, (void *)src_addr, dst_stride);	// 2bytes/pixel
				dst_addr += dst_stride;
				src_addr += src_stride;
			}
		}
#endif
	
		mSurface->postBuffer(0);
	
	}
	
	class MplayerSurfaceOutput * MplayerSurfaceOutput_getSurfaceOutput( void )
	{
		LOGV("MplayerSurfaceOutput %s\n", __FUNCTION__);
	
		if ( MplayerSurfaceOutput_instance )
			return MplayerSurfaceOutput_instance;
	
		MplayerSurfaceOutput_instance = new MplayerSurfaceOutput();
	
		return MplayerSurfaceOutput_instance;
	}
	
	

} 
// end namespace android


void MplayerSurfaceOutput_writeFrameBuffer(unsigned char * img_buf, int size)
{
	MPLAYER_PRINTF("\n******* MplayerSurfaceOutput_writeFrameBuffer \n");

#if 0
	class MplayerSurfaceOutput * surface_output;
	surface_output = MplayerSurfaceOutput_getSurfaceOutput();
	surface_output->writeFrameBuffer();
#elif 1
	if ( !MplayerSurfaceOutput_instance )
		return ;
	
//	MplayerSurfaceOutput_instance->writeFrameBuffer(img_buf, size);
	MplayerSurfaceOutput_instance->writeFrameBuffer(img_buf, 320*240*2);

#endif
}

