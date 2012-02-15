#ifndef __MPLAYER_SURFACE_OUTPUT_H__
#define __MPLAYER_SURFACE_OUTPUT_H__


// Linux and Kernel Includes for Frame Buffer
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/time.h>
//#include <linux/fb.h>
//#include <linux/videodev.h>


/* C code interface */
#ifdef __cplusplus
extern "C" {
#endif

//extern void MplayerSurfaceOutput_writeFrameBuffer(void);
	void MplayerSurfaceOutput_writeFrameBuffer(unsigned char * img_buf, int size);

#ifdef __cplusplus
} // extern "C"
#endif


// C++ 
#ifdef __cplusplus

// SurfaceFlinger
#include <ui/ISurface.h>

// pmem interprocess shared memory support
#include <utils/MemoryBase.h>
#include <utils/MemoryHeapBase.h>



using namespace android;

namespace android {


//	static class MplayerSurfaceOutput * mSurfaceOutput;
	extern class MplayerSurfaceOutput * MplayerSurfaceOutput_instance;
	extern class MplayerSurfaceOutput * MplayerSurfaceOutput_getSurfaceOutput();

	class MplayerSurfaceOutput
	{
	public:
	
		MplayerSurfaceOutput();
	
		static sp<ISurface>                mSurface;
	
	
		void writeFrameBuffer(unsigned char * img_buf, int size);
		void init();
		void setVideoSurface(const sp<ISurface>& surface);
	//	sp<ISurface> *  getVideoSurface();
		void surfaceTest();
	
	private:
	
		~MplayerSurfaceOutput();
	
	//	void* wolfgang_start_mplayer(void *surface);
		int iScreenWidth;
		int iScreenBytesPerPixel;
		int iVideoDisplayWidth;
		int iVideoDisplayHeight;
		int iVideoWidth;
		int iVideoHeight;
	
	//	sp<ISurface>                mSurface;
		sp<MemoryHeapBase>          mFrameHeap;
	
		bool                        mInitialized;
	
	
	
	};
	
}; // namespace android

#endif	/* define __cplusplus */

#endif	/* __MPLAYER_SURFACE_OUTPUT_H__ */
