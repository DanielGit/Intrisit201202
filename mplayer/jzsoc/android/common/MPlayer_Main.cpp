#define LOG_NDEBUG 0
#define LOG_TAG "mplayer"
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

#include <utils/ProcessState.h>
#include <utils/IPCThreadState.h>
#include <utils/IServiceManager.h>

#include <MPlayer.h>




using namespace android;


int main(int argc, char * argv[])
{

	sp<ProcessState> proc(ProcessState::self());

	if (proc->supportsProcesses()) {
		LOGV("App process: starting thread pool.\n");
		proc->startThreadPool();
	}

	MPlayer * mplayer = new MPlayer();

	mplayer->run();			// mplayer loop

//	delete mplayer;
	IPCThreadState::self()->flushCommands();

	return 0;
}

