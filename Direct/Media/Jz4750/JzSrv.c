//[PROPERTY]===========================================[PROPERTY]
//            *****  诺亚神舟操作系统V2  *****
//        --------------------------------------
//    	             直接音频播放部分
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明
//  V0.1    2009-5      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]


#include <kernel/kernel.h>
#include <kernel/irq.h>
#include <kernel/device.h>
#include <kernel/karch.h>
#include <kernel/callback.h>
#include <kernel/mmu.h>
#include <kernel/sched.h>
#include <platform/platform.h>
#include <direct/media.h>

static int JzSrvDataFileInit(PAK_VFILE vfile, PMEDIA_TASK task);
static void JzSrvDataFileDeinit(PAK_VFILE vfile);
static int JzSrvCtrlInit(PAK_CTRL ctrl);
static void JzSrvCtrlDeinit(PAK_CTRL ctrl);
static int JzSrvTaskInit(PMEDIA_TASK dst, PMEDIA_TASK src);

static HANDLE JzSrvCreate(void *media);
static int JzSrvDestroy(HANDLE haudio);
static int JzSrvOpen(HANDLE haudio);
static int JzSrvClose(HANDLE haudio, int code);
static int JzSrvCtrl(HANDLE haudio, int ctl, DWORD *param, int *size);

int kMutexWaitNoLock(HANDLE hmtx);

static LIST JzObjList;
static MEDIA_CALLBACK JzCallback =
{
	JzSrvCreate,
	JzSrvDestroy,
	JzSrvOpen,
	JzSrvClose,
	JzSrvCtrl
};


////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
void JzSrvInit(void)
{
	JzCtlInit();
	ListInit(&JzObjList);
	MediaSrvRegistCallback(MEDIA_TYPE_AUDIO, MEDIA_DEVICE_AUDIO|MEDIA_DEVICE_VIDEO, &JzCallback);
	MediaSrvRegistCallback(MEDIA_TYPE_VIDEO, MEDIA_DEVICE_AUDIO|MEDIA_DEVICE_VIDEO, &JzCallback);
}



////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
int JzSrvDestroyNotify(PMEDIA_TASK task)
{
	PLIST head;
	PLIST n;
	PAK_OBJECT obj;
	
	JZ_LOCK();
	head = &JzObjList;
	
	// 搜索指定音频任务
	for(n = ListFirst(head); n != head; n = ListNext(n))
	{
		obj = ListEntry(n, AK_OBJECT, Link);
		if(&obj->Task == task)
		{
			// 释放消息结构体
			JzSrvCtrlDeinit(&obj->Ctrl);
			
			// 是否数据文件结构体
			JzSrvDataFileDeinit(&obj->File);	
			
			// 向上传递NOTIFY事件
			MediaSrvDestroyNotify(obj->Header.Handle);

			// 释放当前节点
			ListRemove(&obj->Link);
			HandleDestroy(obj->Header.Handle, VIDEO_MAGIC);
			kfree(obj);
			JZ_UNLOCK();
			return 0;
		}
	}
	kdebug(mod_audio, PRINT_INFO, "Destroy notify end\n");
	JZ_UNLOCK();
	return -1;	
}



////////////////////////////////////////////////////
// 功能: 检查音频设备是否正在运行
// 输入: 
// 输出:
// 返回: 
// 说明: 用于CPU检查是否进行频率切换
////////////////////////////////////////////////////
int JzSrvMode(void)
{
	return 1;
}


////////////////////////////////////////////////////
// 功能: 检查音频设备是否正在运行
// 输入: 
// 输出:
// 返回: 
// 说明: 用于CPU检查是否进行频率切换
////////////////////////////////////////////////////
int JzSrvPowerOff(int mode)
{
	return 0;
}

//void SetSeekTime( int f);
////////////////////////////////////////////////////
// 功能: 
// 输入:
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static int JzSrvDataFileInit(PAK_VFILE vfile, PMEDIA_TASK task)
{
	int len,dot_pos,i;
	// 获取进程ASID
#if defined(CONFIG_MMU_ENABLE)
	vfile->Asid = VPageGetAsid();
#endif
	if( task->FileName )
	{	//保存文件扩展名
		kmemset(vfile->FileType,0,4);
		for(len = 0,dot_pos = 0 ; task->FileName[len] != 0 ; len++ )
		{
			if( task->FileName[len] == '.' )
				dot_pos = len;
		}

		for( i = 0 ; i < 4 ; i++ )
		{
			if( (dot_pos + i + 1) < len )
				vfile->FileType[i] = task->FileName[dot_pos + i + 1];
		}
	}
	// 检查是否录音为文件
	if(task->Record && task->FileName)
	{
		vfile->File = kfopen(task->FileName, "wb");
		if(vfile->File == NULL)
			return -1;
		
		vfile->PlayLength = 0x7fffffff;	
		vfile->PlayOffset = 0;
		vfile->vOffset = 0;
		return 0;
	}		
	
	// 录音到缓冲区或者播放文件
	if(task->FileName)
	{	
		int fsize;
		
		// 打开文件
		vfile->File = kfopen(task->FileName, "rb");
		if(vfile->File == NULL)
			return -1;
		
		// 获取文件大小
		fsize = kfsize(vfile->File);
		if(task->PlayOffset >= fsize)
			return -1;
		kfseek(vfile->File, task->PlayOffset, SEEK_SET);
		
		// 初始化缓冲参数
		if(!task->PlayLength)
			vfile->PlayLength = fsize - task->PlayOffset;
		else if((fsize - task->PlayOffset) >= task->PlayLength)
			vfile->PlayLength = task->PlayLength;
		else
			vfile->PlayLength = (fsize - task->PlayOffset);
		vfile->PlayOffset = task->PlayOffset;
		vfile->vOffset = 0;
		return 0;
	}
	else if(task->DataBuf && (task->BufSize > 0))		// 播放缓冲区中的声音
	{
		// 初始化播放/录制长度
		if(!task->PlayLength)
		{
			if(task->hCallback)
				vfile->PlayLength = 0x7fffffff;
			else
				vfile->PlayLength = task->BufSize;
		}
		else
		{
			if(!task->hCallback && (task->PlayLength > task->BufSize))
				vfile->PlayLength = task->BufSize;
			else
				vfile->PlayLength = task->PlayLength;
		}
		
		// 初始化其他变量
		vfile->PlayOffset = task->PlayOffset;
		vfile->vOffset = 0;
		vfile->uBuf = (BYTE*)task->DataBuf;
		vfile->uSize = task->BufSize;
		vfile->uOffset = 0;
		vfile->uCb = task->hCallback;
		return 0;
	}
	return -1;
}



////////////////////////////////////////////////////
// 功能: 
// 输入:
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static void JzSrvDataFileDeinit(PAK_VFILE vfile)
{
	if(vfile->File)
		kfclose(vfile->File);
	if(vfile->uCb)
		sKernCallbackDestroy(vfile->uCb);
	kmemset(vfile, 0x00, sizeof(AK_VFILE));
}


////////////////////////////////////////////////////
// 功能: 
// 输入:
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static int JzSrvCtrlInit(PAK_CTRL ctrl)
{
	ctrl->Code = -1;
	ctrl->Size = 0;
	ctrl->Param = NULL;
	ctrl->ParamData = 0;
	ctrl->hSema = kSemaCreate(0);
	if(ctrl->hSema == NULL)
		return -1;
		
	ctrl->hMutex = kMutexCreate();
	if(ctrl->hMutex == NULL)
	{
		kSemaDestroy(ctrl->hSema);
		return -1;
	}
	return 0;
}


////////////////////////////////////////////////////
// 功能: 
// 输入:
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static void JzSrvCtrlDeinit(PAK_CTRL ctrl)
{
	if(ctrl->Size > sizeof(DWORD))
		kfree(ctrl->Param);
	if(ctrl->hMutex)
		kMutexDestroy(ctrl->hMutex);
	if(ctrl->hSema)
		kSemaDestroy(ctrl->hSema);
	kmemset(ctrl, 0x00, sizeof(AK_CTRL));
}


////////////////////////////////////////////////////
// 功能: 格式化输入参数
// 输入:
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static int JzSrvTaskInit(PMEDIA_TASK dst, PMEDIA_TASK src)
{
	// 复制结构体
	kmemcpy(dst, src, sizeof(MEDIA_TASK));

	// 1. 媒体文件类型格式化
	if(!src->Record)
		dst->MediaType = 0;
	
	return 0;
}


////////////////////////////////////////////////////
// 功能: 申请并初始化节点
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
static HANDLE JzSrvCreate(void *media)
{
	PMEDIA_TASK task;
	PAK_OBJECT obj;
	HANDLE ret;

	// 申请节点
	task = (PMEDIA_TASK)media;
	obj = kmalloc(sizeof(AK_OBJECT));
	if(obj == NULL)
		return NULL;
	kmemset(obj, 0x00, sizeof(AK_OBJECT));
	
	// 初始化任务结构体
	JZ_LOCK();	
	if(JzSrvTaskInit(&obj->Task, task) < 0)
	{
		kfree(obj);
		JZ_UNLOCK();
		return NULL;
	}

	// 初始消息结构体
	if(JzSrvCtrlInit(&obj->Ctrl) < 0)
	{
		kfree(obj);
		JZ_UNLOCK();
		return NULL;
	}
		
	// 初始化数据文件结构体
	if(JzSrvDataFileInit(&obj->File, task) < 0)
	{
		JzSrvCtrlDeinit(&obj->Ctrl);
		kfree(obj);
		JZ_UNLOCK();
		return NULL;
	}
	
	// 把创建的对象加入对象链
	ListInsert(&JzObjList, &obj->Link);
	ret = HandleSet(obj, VIDEO_MAGIC, JzSrvDestroy);
	JZ_UNLOCK();
	return ret;
}


////////////////////////////////////////////////////
// 功能: 销毁一个音频任务
// 输入: 
// 输出:
// 返回: 
// 说明:
////////////////////////////////////////////////////
static int JzSrvDestroy(HANDLE hvedio)
{
	PAK_OBJECT obj;

	JZ_LOCK();
	
	// 获取音频对象
	obj = (PAK_OBJECT)HandleGet(hvedio, VIDEO_MAGIC);
	if(obj == NULL)
	{
		JZ_UNLOCK();
		return -1;
	}
	
	// 释放消息结构体
	JzSrvCtrlDeinit(&obj->Ctrl);
	
	// 是否数据文件结构体
	JzSrvDataFileDeinit(&obj->File);	

	// 释放当前节点
	ListRemove(&obj->Link);
	HandleDestroy(hvedio, VIDEO_MAGIC);
	kfree(obj);	
	JZ_UNLOCK();
	return 0;
}

		
////////////////////////////////////////////////////
// 功能: 加入控制信息到队列
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int JzSrvProcMsg(PAK_OBJECT obj)
{
	int ret;
	RECT cr;
	
	// 锁定边界
	kMutexWait(obj->Ctrl.hMutex);
	if(obj->Ctrl.Code < 0)
	{
		kMutexRelease(obj->Ctrl.hMutex);
		return -1;
	}
	
	// 处理消息
	ret = -1;
	switch(obj->Ctrl.Code)
	{
	case MEDIA_CTRL_PAUSE:
		obj->PauseRect.x = 0;
		obj->PauseRect.y = 0;
		obj->PauseRect.w = 0;
		obj->PauseRect.h = 0;
		if(obj->Ctrl.Param && (obj->Ctrl.Size == sizeof(RECT)))
			obj->PauseRect = *(PRECT)obj->Ctrl.Param;

		ret = JzCtlPause(obj);
		break;
		
	case MEDIA_CTRL_RESUME:
		ret = JzCtlResume(obj);
		break;
		
	case MEDIA_CTRL_SEEK:
		if(obj->Ctrl.Param)
			ret = JzCtlSeek(obj, *obj->Ctrl.Param);
		break;
		
	case MEDIA_CTRL_VOLUME:
		if(obj->Ctrl.Param)
			ret = JzCtlVolumeSet(obj, *obj->Ctrl.Param);
		break;
		
	case MEDIA_CTRL_TEMPO:
		if(obj->Ctrl.Param)
			ret = JzCtlPlaySpeed(obj, *obj->Ctrl.Param);
		break;
	
	case MEDIA_CTRL_DURATION:
		if(obj->Ctrl.Param)
			ret = JzCtlGetDuration(obj, obj->Ctrl.Param);
		break;		
	case MEDIA_CTRL_FORWARD:
		ret = JzCtlForward(obj);
		break;
		
	case MEDIA_CTRL_BACKWARD:
		ret = JzCtlBackward(obj);
		break;
		
	case MEDIA_CTRL_ISPLAY:
		ret = JzCtlIsPlaying(obj);
		break;
		
	case MEDIA_CTRL_SETEQ:
		ret = JzCtlEq(obj, (int)*obj->Ctrl.Param);
		break;

	case MEDIA_CTRL_RECT:
		if(obj->Ctrl.Param && (obj->Ctrl.Size == sizeof(RECT)))
		{
			obj->Task.ShowRect = *(PRECT)obj->Ctrl.Param;
			JzCtlResize(obj);
			ret = 0;
		}
		break;	
	case MEDIA_CTRL_TOTALTIME:
		if( obj->Ctrl.Size == sizeof(DWORD) )
			ret = JzCtlGetTotalTime(obj,obj->Ctrl.Param);
 		else
		{
			kdebug(mod_audio, PRINT_ERROR, "MEDIA_CTRL_TOTALTIME: input para is not DWORD\n");
			ret = -1;
		}
		break;
	case MEDIA_CTRL_SETAB:
		if(obj->Ctrl.Param && (obj->Ctrl.Size == sizeof(RECT)))
		{
			cr  = *(PRECT)obj->Ctrl.Param;
			ret = JzCtlSetAB(obj,cr.x, cr.y);
		}
		break;
	}
	ret = obj->Ctrl.Code;
	obj->Ctrl.Return = ret;
	obj->Ctrl.Code = -1;
	kSemaRelease(obj->Ctrl.hSema);
	
	// 删除消息	
	kMutexRelease(obj->Ctrl.hMutex);
	return ret;
}



////////////////////////////////////////////////////
// 功能: 音频设备控制
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
static int JzSrvCtrl(HANDLE hvedio, int ctl, DWORD *param, int *param_size)
{
	PAK_OBJECT obj;
	DWORD* param_back;
	int ret;
	int size;
	HANDLE hsema;
	HANDLE hmutex;
	
	if(param_size)
		size = *param_size;
	else
		size = 0;

	// 获取音频对象
	JZ_LOCK();
	SchedLock();
	obj = (PAK_OBJECT)HandleGet(hvedio, VIDEO_MAGIC);
	if(obj == NULL)
	{
		SchedUnlock();
		JZ_UNLOCK();
		return -1;
	}
	hmutex = obj->Ctrl.hMutex;
	
	// 锁定边界
	kMutexWaitNoLock(hmutex);
	
	// 初始化控制结构体
	param_back = NULL;
	if(size > sizeof(DWORD))
	{
		param_back = (DWORD*)kmalloc(size);
		obj->Ctrl.Param = param_back;
		if(obj->Ctrl.Param == NULL)
		{
			kMutexRelease(hmutex);
			JZ_UNLOCK();
			return -1;
		}
		kmemcpy(obj->Ctrl.Param, param, size);
	}
	else if(size > 0)	// 加速处理
	{
		obj->Ctrl.Param = &obj->Ctrl.ParamData;
		kmemcpy(obj->Ctrl.Param, param, size);
	}
	else
	{
		obj->Ctrl.Param = NULL;
	}
	obj->Ctrl.Code = ctl;
	obj->Ctrl.Size = size;

	// 等待控制处理	
	hsema = obj->Ctrl.hSema;
	kMutexRelease(hmutex);

	//重新获取句柄，判断当前声音是否被删除
	kSemaWait(hsema, 0);
	SchedLock();
	obj = (PAK_OBJECT)HandleGet(hvedio, VIDEO_MAGIC);
	if(obj == NULL)
	{
		kdebug(mod_audio, PRINT_ERROR, "JzSrvCtrl delete\n");
		SchedUnlock();
		JZ_UNLOCK();
		return -1;
	}

	// 获取控制返回值
	kMutexWaitNoLock(hmutex);
	if(obj->Ctrl.Size )
	{
		kmemcpy(param, obj->Ctrl.Param, obj->Ctrl.Size);
		if(obj->Ctrl.Size > sizeof(DWORD))
		{
			kfree(param_back);
		}
	}
	obj->Ctrl.Param = NULL;
	obj->Ctrl.Size = 0;
	ret = obj->Ctrl.Return;
	kMutexRelease(hmutex);
	JZ_UNLOCK();
	return ret;
}


////////////////////////////////////////////////////
// 功能: 
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
static int JzSrvOpen(HANDLE hvedio)
{
	PAK_OBJECT obj;
	int ret = 0;
	
	// 获取音频对象
	JZ_LOCK();
	obj = (PAK_OBJECT)HandleGet(hvedio, VIDEO_MAGIC);
	if(obj == NULL)
	{
		JZ_UNLOCK();
		return -1;
	}
	ret = JzThreadOpen(obj);
	JZ_UNLOCK();
	return ret;
}




////////////////////////////////////////////////////
// 功能: 
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
static int JzSrvClose(HANDLE hvedio, int code)
{
	PAK_OBJECT obj;
	
	// 获取音频对象
	JZ_LOCK();
	obj = (PAK_OBJECT)HandleGet(hvedio, VIDEO_MAGIC);
	if(obj == NULL)
	{
		JZ_UNLOCK();
		return -1;
	}
	JzThreadClose(obj, code);
	JZ_UNLOCK();
	return 0;
}


////////////////////////////////////////////////////
// 功能: 
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
int JzSrvInfo(void *media, void *info, int type)
{
	PMEDIA_TASK task;
	PAK_OBJECT obj;
	int ret;

	// 检查DSP是否被占用
	JZ_LOCK();
	
	// 申请节点
	task = (PMEDIA_TASK)media;
	obj = kmalloc(sizeof(AK_OBJECT));
	if(obj == NULL)
		return -1;
	kmemset(obj, 0x00, sizeof(AK_OBJECT));
	
	// 初始化任务结构体
	JZ_LOCK();	
	if(JzSrvTaskInit(&obj->Task, task) < 0)
	{
		kfree(obj);
		JZ_UNLOCK();
		return -1;
	}

	// 初始化数据文件结构体
	if(JzSrvDataFileInit(&obj->File, task) < 0)
	{
		JzSrvCtrlDeinit(&obj->Ctrl);
		kfree(obj);
		JZ_UNLOCK();
		return -1;
	}
	
	// 初始化信息
	obj->bGetInfo = 1;
	
	// 启动线程
	ret = JzThreadOpen(obj);
	
	// 返回参数
	if(info && (ret >= 0))
	{	
		switch(type)
		{
			case MEDIA_TYPE_VIDEO:
			{
				PVIDEO_INFO vinfo;
				
				vinfo = (PVIDEO_INFO)info;
				vinfo->Type = obj->Info.MediaType;
				vinfo->MovieWidth = obj->Info.VideoWidth;
				vinfo->MovieHeight = obj->Info.VideoHeight;
				vinfo->FPS = obj->Info.VideoFps;
				vinfo->TotalTime = obj->Info.TotalTime;
				vinfo->VideoBitrate = obj->Info.VideoBitrate;
				vinfo->AudioBitrate = obj->Info.AudioBitrate;
			}
			break;
			
			case MEDIA_TYPE_AUDIO:
			{
				PAUDIO_INFO ainfo;
				
				ainfo = (PAUDIO_INFO)info;
				ainfo->Type = obj->Info.MediaType;
				ainfo->TotalTime = obj->Info.TotalTime;
				ainfo->Bitrate = obj->Info.AudioBitrate;
				ainfo->Samplerate = obj->Info.AudioSamplerate;
			}
			break;
			
			default:
				kmemcpy(info, &obj->Info, sizeof(MEDIA_INFO));
				break;
		}
	}
	
	// 是否数据文件结构体
	JzSrvDataFileDeinit(&obj->File);	

	// 销毁对象
	kfree(obj);
	JZ_UNLOCK();
	return ret;
}

////////////////////////////////////////////////////
// 功能: 
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
int JzSrvUseMplayer(void)
{
	PLIST head;
	PLIST n;
	PMEDIA_TASK task;
	PAK_OBJECT obj;

	JZ_LOCK();
	head = &JzObjList;
	// 搜索指定音频任务
	for(n = ListFirst(head); n != head; n = ListNext(n))
	{
		obj = ListEntry(n, AK_OBJECT, Link);
		task = &obj->Task;
		if(task->ExterdType == 1)
		{
			JZ_UNLOCK();
			return -1;
		}
	}

	JZ_UNLOCK();
	return 1;
}
