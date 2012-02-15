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
#include <platform/platform.h>
#include <direct/media.h>

typedef struct _CALLBACK_LINK
{
	LIST Link;
	int Type;
	DWORD Device;
	MEDIA_CALLBACK Callback;
}CALLBACK_LINK;
typedef CALLBACK_LINK *PCALLBACK_LINK;


static LIST MediaObjList;
static LIST MediaCallbackList;
static HANDLE hMediaMutex;

extern int DacSetGloalVolume(int vol, int speak);
extern int DacGetGloalVolume(int speak);
extern int DacIsHeadphoneIn(void);
extern int DacGetOwners(void);
extern void SetMidiWtdDirect(BYTE *param, int size);
extern int JzSrvUseMplayer(void);

extern void SaveMplayerVar();
////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
void MediaSrvInit(void)
{
	ListInit(&MediaObjList);
	ListInit(&MediaCallbackList);
	hMediaMutex = kMutexCreate();
	JzSrvInit();
	SaveMplayerVar();
}


////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
void MediaSrvLock(void)
{
	kMutexWait(hMediaMutex);
}

////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
void MediaSrvUnlock(void)
{
	kMutexRelease(hMediaMutex);
}


////////////////////////////////////////////////////
// 功能: 音频任务关机处理
// 输入: 
// 输出:
// 返回: 
// 说明:
////////////////////////////////////////////////////
int MediaSrvPowerOff(int mode)
{
	JzSrvPowerOff(mode);
	return 0;
}


////////////////////////////////////////////////////
// 功能: 检查音频设备是否正在运行
// 输入: 
// 输出:
// 返回: 
// 说明: 用于CPU检查是否进行频率切换
////////////////////////////////////////////////////
int MediaSrvMode(void)
{
	if(JzSrvMode())
		return 1;
	return 0;
}


////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
int MediaSrvRegistCallback(int type, DWORD device, PMEDIA_CALLBACK callback)
{
	PLIST n;
	PLIST head;
	PCALLBACK_LINK check;
	PCALLBACK_LINK link;

	// 申请节点，并初始化
	link = kmalloc(sizeof(CALLBACK_LINK));
	if(link == NULL)
		return -1;
	kmemcpy(&link->Callback, callback, sizeof(MEDIA_CALLBACK));
	link->Type = type;
	link->Device = device;
	ListInit(&link->Link);
	
	// 检查设备是否已经注册
	head = &MediaCallbackList;
	for(n=ListFirst(head); n!=head; n=ListNext(n))
	{
		check = ListEntry(n, CALLBACK_LINK, Link);
		if(&check->Callback == callback)
		{
			kfree(link);
			return -1;
		}
	}
	ListInsert(&MediaCallbackList, &link->Link);		
	return 0;
}


////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
int MediaSrvDestroyNotify(void *media)
{
	PLIST head;
	PLIST n;
	PMEDIA_OBJECT obj;
	
//	kMutexWait(hMediaMutex);
	head = &MediaObjList;
	
	// 搜索指定音频任务
	for(n = ListFirst(head); n != head; n = ListNext(n))
	{
		obj = ListEntry(n, MEDIA_OBJECT, Link);
		if(obj->Media == media)
		{
			// 释放当前节点
			kdebug(mod_media, PRINT_INFO, "CLOSE Notify: 0x%x\n", obj->Header.Handle);
			ListRemove(&obj->Link);
			HandleDestroy(obj->Header.Handle, MEDIA_MAGIC);			
			if(obj->MediaInfo)
				kfree(obj->MediaInfo);
			kfree(obj);
			
			// 启动下一个等待任务
			n = ListFirst(head);
			if(n != head)
			{
				obj = ListEntry(n, MEDIA_OBJECT, Link);
				if(obj->Mode == MEDIA_MODE_WAIT)
				{
					if(obj->Cb.MediaOpen(obj->Media) < 0)
					{
						ListRemove(&obj->Link);
						HandleDestroy(obj->Header.Handle, MEDIA_MAGIC);
						if(obj->MediaInfo)
							kfree(obj->MediaInfo);
						kfree(obj);
					}
				}
			}
//			kMutexRelease(hMediaMutex);
			return 0;
		}
	}
//	kMutexRelease(hMediaMutex);
	return -1;	
}


////////////////////////////////////////////////////
// 功能: 根据媒体类型，获取媒体回调函数
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
static int MediaSrvGetCallback(int type, PMEDIA_CALLBACK callback)
{
	PLIST n;
	PLIST head;
	PCALLBACK_LINK link;

	// 检查设备是否已经注册
	head = &MediaCallbackList;
	for(n=ListFirst(head); n!=head; n=ListNext(n))
	{
		link = ListEntry(n, CALLBACK_LINK, Link);
		if((link->Type & type) == type)
		{
			kmemcpy(callback, &link->Callback, sizeof(MEDIA_CALLBACK));
			return 0;
		}
	}
	return -1;
}



////////////////////////////////////////////////////
// 功能: 为音频任务申请并初始化节点
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
static PMEDIA_OBJECT MediaSrvObjCreate(void *media, char *name, int type)
{
	PMEDIA_OBJECT obj;
	
	// 输入参数检查
	if(!media)
		return NULL;

	// 申请节点，并初始化
	obj = kmalloc(sizeof(MEDIA_OBJECT));
	if(obj == NULL)
		return NULL;
	kmemset(obj, 0x00, sizeof(MEDIA_OBJECT));
	ListInit(&obj->Link);
	if(MediaSrvGetCallback(type, &obj->Cb) < 0)
	{
		kdebug(mod_media, PRINT_ERROR, "MediaSrvGetCallback error\n");
		kfree(obj);
		return NULL;
	}

	if(((PMEDIA_TASK)media)->ExterdType == 1 && JzSrvUseMplayer() < 0)
	{
		kdebug(mod_media, PRINT_ERROR, "MediaSrvGetCallback error: mplayer already exist\n");
		kfree(obj);
		return NULL;
	}
	
	// 创建媒体任务
	obj->Media = obj->Cb.MediaCreate(media);
	if(obj->Media == NULL)
	{
		kfree(obj);
		return NULL;
	}
	
	// 保存媒体名称
	if(name)
	{
		obj->MediaInfo = kmalloc(kstrlen(name)+1);
		if(obj->MediaInfo)
			kstrcpy(obj->MediaInfo, name);
	}
	
	// 设置媒体任务
#if defined(STC_EXP)
	obj->hTask = sTaskSelf();
#else
	obj->hTask = TaskSelf();
#endif	
	// 返回媒体对象
	return obj;
}


////////////////////////////////////////////////////
// 功能: 增加一个音频任务
// 输入: amedia: 任务结构体
//       preempt: 0-不抢占， 1-抢占; 2-等待
// 输出:
// 返回: 0: 操作成功 <0: 操作错误
// 说明:
////////////////////////////////////////////////////
#if defined(STC_EXP)
HANDLE sMediaSrvCreate(void *media, char *name, int type, int preempt)
#else
HANDLE MediaSrvCreate(void *media, char *name, int type, int preempt)
#endif
{
	PLIST head;
	PLIST n;
	PMEDIA_OBJECT obj;

	// 参数检查
	if(media == NULL)
		return NULL;

	// 检查当前是否存在录放任务
	kMutexWait(hMediaMutex);
	head = &MediaObjList;
	if(!ListEmpty(head) && (preempt != MEDIA_MODE_NORMAL))
	{
		// 检查是否强行结束正在录放的音频
		if(preempt == MEDIA_MODE_PREEMPTIVE)
		{
			// 获取正在音频任务节点
			n = ListFirst(head);
			obj = ListEntry(n, MEDIA_OBJECT, Link);

			// 结束当前正在录放的音频任务
			obj->Cb.MediaClose(obj->Media, 1);
			obj->Cb.MediaDestroy(obj->Media);

			// 释放当前节点
			ListRemove(&obj->Link);
			HandleDestroy(obj->Header.Handle, MEDIA_MAGIC);
			if(obj->MediaInfo)
				kfree(obj->MediaInfo);
			kfree(obj);

			// 申请并初始化节点
			obj = MediaSrvObjCreate(media, name, type);
			if(obj == NULL)
			{
				kMutexRelease(hMediaMutex);
				return NULL;
			}

			// 启动新的播放任务
			if(obj->Cb.MediaOpen(obj->Media) < 0)
			{
				if(obj->MediaInfo)
					kfree(obj->MediaInfo);
				kfree(obj);
				kMutexRelease(hMediaMutex);
				return NULL;
			}

			// 把节点插入任务队列
			ListInsert(head, &obj->Link);
		}
		else
		{
			// 申请并初始化节点
			obj = MediaSrvObjCreate(media, name, type);
			if(obj == NULL)
			{
				kMutexRelease(hMediaMutex);
				return NULL;
			}

			// 把当前音频插入等待队列尾部
			ListInsert(ListPrev(head), &obj->Link);
		}
	}
	else
	{
		// 申请并初始化节点
		obj = MediaSrvObjCreate(media, name, type);
		if(obj == NULL)
		{
			kMutexRelease(hMediaMutex);
			return NULL;
		}

		// 启动新的录放任务
		if(obj->Cb.MediaOpen(obj->Media) < 0)
		{
			if(obj->MediaInfo)
				kfree(obj->MediaInfo);
			kfree(obj);
			kMutexRelease(hMediaMutex);
			return NULL;
		}
		
		// 把当前音频插入等待队列尾部
		ListInsert(head, &obj->Link);
	}
	
	kMutexRelease(hMediaMutex);
	{
		HANDLE hret = HandleSet(obj, MEDIA_MAGIC, sMediaSrvDestroy);
		kdebug(mod_media, PRINT_INFO, "OPEN: 0x%x\n", hret);
		return hret;
	}
}



////////////////////////////////////////////////////
// 功能: 销毁一个音频任务
// 输入: 
// 输出:
// 返回: 
// 说明:
////////////////////////////////////////////////////
#if defined(STC_EXP)
int sMediaSrvDestroy(HANDLE hmedia)
#else
int MediaSrvDestroy(HANDLE hmedia)
#endif
{
	PMEDIA_OBJECT obj;
	int playback;

	kMutexWait(hMediaMutex);
	
	// 获取音频对象
	obj = (PMEDIA_OBJECT)HandleGet(hmedia, MEDIA_MAGIC);
	if(obj == NULL)
	{
		kdebug(mod_media, PRINT_ERROR, "CLOSE NULL: 0x%x\n", hmedia);
		kMutexRelease(hMediaMutex);
		return -1;
	}
	kdebug(mod_media, PRINT_INFO, "CLOSE: 0x%x\n", hmedia);
	
	// 结束当前正在录放的音频任务
	playback = obj->Cb.MediaClose(obj->Media, 1);
	obj = (PMEDIA_OBJECT)HandleGet(hmedia, MEDIA_MAGIC);
	if(obj == NULL)
	{
		kdebug(mod_media, PRINT_ERROR, "CLOSE NULL NULL: 0x%x\n", hmedia);
		kMutexRelease(hMediaMutex);
		return -1;
	}
	obj->Cb.MediaDestroy(obj->Media);
	
	// 释放当前节点
	ListRemove(&obj->Link);
	HandleDestroy(hmedia, MEDIA_MAGIC);
	if(obj->MediaInfo)
		kfree(obj->MediaInfo);
	kfree(obj);
	
	// 启动下一个等待任务
	if(!ListEmpty(&MediaObjList))
	{
		obj = ListEntry(ListFirst(&MediaObjList), MEDIA_OBJECT, Link);
		if(playback && (obj->Mode == MEDIA_MODE_WAIT))
		{
			if(obj->Cb.MediaOpen(obj->Media) < 0)
			{
				HandleDestroy(obj->Header.Handle, MEDIA_MAGIC);
				if(obj->MediaInfo)
					kfree(obj->MediaInfo);
				kfree(obj);
			}
		}
	}
	kMutexRelease(hMediaMutex);
	return 0;
}


////////////////////////////////////////////////////
// 功能: 获取当前音频任务信息
// 输入:
// 输出: process：正在播放任务信息
// 返回: 正在录放和等待录放任务数量
// 说明:
////////////////////////////////////////////////////
#if defined(STC_EXP)
int sMediaSrvMaster(char *process, int max, int type)
#else
int MediaSrvMaster(char *process, int max, int type)
#endif
{
	PMEDIA_OBJECT obj;
	PLIST head;
	PLIST n;
	int tasks;
	int owners;
	int len;

	// 获取媒体使用的设备
	kMutexWait(hMediaMutex);
//	if(ListEmpty(&MediaObjList))
//	{
//		kMutexRelease(hMediaMutex);
//		return 0;
//	}
	
	// 循环获取所有正在播放的音频任务名称，并用分隔符返回
	if(process)
		kstrcpy(process, "");
	tasks = 0;
	len = 0;
	head = &MediaObjList;
	for(n=ListFirst(head); n!=head; n=ListNext(n))
	{
		// 获取打开设备对象
		obj = ListEntry(n, MEDIA_OBJECT, Link);
		if(process)
		{
			if(tasks)
			{
				len++;
				if(len < max)
					kstrcat(process, ";");
			}
			if(obj->MediaInfo)
			{
				len += kstrlen(obj->MediaInfo);
				if(len < max)
					kstrcat(process, obj->MediaInfo);
			}
			else
			{
				len += 6;
				if(len < max)
					kstrcat(process, "<NULL>");
			}
		}
		tasks++;
	}
	
	// 获取打开DAC设备情况
	owners = DacGetOwners();
	if(owners > tasks)
	{
		if(process)
		{
			if(tasks)
			{
				len++;
				if(len < max)
					kstrcat(process, ";");
			}
			len += 8;
			if(len < max)
				kstrcat(process, "<DIRECT>");
		}
		tasks = owners;	
	}
	kMutexRelease(hMediaMutex);
	return tasks;
}


////////////////////////////////////////////////////
// 功能: 音频设备控制
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
#if defined(STC_EXP)
int sMediaSrvCtrl(HANDLE hmedia, int ctl, DWORD *param, int *size)
#else
int MediaSrvCtrl(HANDLE hmedia, int ctl, DWORD *param, int *size)
#endif
{
	PMEDIA_OBJECT obj;
	int ret;
	
	// 检查是否设置全局音量
	if((hmedia == NULL) && (param != NULL))
	{
//		kprintf("MediaSrvCtrl: ctl = %x\n",ctl);
		switch(ctl & 0xf00)
		{
		case MEDIA_CTRL_OVOLUME_SET:
			DacSetGloalVolume((int)*param, ctl & 0xff);
			break;
		case MEDIA_CTRL_OVOLUME_GET:
			*param = DacGetGloalVolume(ctl & 0xff);
			break;
		case MEDIA_CTRL_HEADPHONE_IN:
			*param = DacIsHeadphoneIn();
			break;
#ifdef CONFIG_DECODE_MIDI_ENABLE
		case MEDIA_CTRL_MIDI_DIRECT:
			kdebug(mod_media, PRINT_NOTICE, "MediaSrvCtrl: param = %s, size = %d\n",(BYTE*)param,*size);
			SetMidiWtdDirect((BYTE*)param,*size);
			break;
#endif
		default:
			return -1;
		}
		return 0;
	}
	
	kMutexWait(hMediaMutex);
	obj = (PMEDIA_OBJECT)HandleGet(hmedia, MEDIA_MAGIC);
	if(obj == NULL)
	{
		kMutexRelease(hMediaMutex);
		return -1;
	}
	ret = obj->Cb.MediaCtrl(obj->Media, ctl, param, size);
	kMutexRelease(hMediaMutex);
	return ret;
}


////////////////////////////////////////////////////
// 功能: 音频设备控制
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
#if defined(STC_EXP)
int sMediaSrvGetName(HANDLE hmedia, char *name, int max)
#else
int MediaSrvGetName(HANDLE hmedia, char *name, int max)
#endif
{
	PMEDIA_OBJECT obj;
	int ret;
	
	kMutexWait(hMediaMutex);
	obj = (PMEDIA_OBJECT)HandleGet(hmedia, MEDIA_MAGIC);
	if(obj == NULL)
	{
		kMutexRelease(hMediaMutex);
		return -1;
	}
	ret = kstrlen(obj->MediaInfo);
	if(ret <= max)
	{
		kstrcpy(name, obj->MediaInfo);
	}
	else
	{
		kmemcpy(name, obj->MediaInfo, max);
		ret = max;
	}
	kMutexRelease(hMediaMutex);
	return ret;
}

////////////////////////////////////////////////////
// 功能: 音频设备写
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
#if defined(STC_EXP)
int sMediaSrvInfo(void *media, void *info, int type)
#else
int MediaSrvInfo(void *media, void *info, int type)
#endif
{
	int ret;
	kMutexWait(hMediaMutex);
	ret = JzSrvInfo(media, info, type);
	kMutexRelease(hMediaMutex);
	return ret;
}




////////////////////////////////////////////////////
// 功能: 结束某一任务的媒体播放，并锁定媒体播放任务
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
void MediaTerminateLock(HANDLE htask)
{
	PMEDIA_OBJECT obj;
	PLIST list;
	
	kMutexWait(hMediaMutex);
	list = &MediaObjList;
	if(!ListEmpty(list))
	{
		// 获取正在音频任务节点
		obj = ListEntry(ListFirst(list), MEDIA_OBJECT, Link);
		
		if(obj->hTask == htask)
		{
			// 结束当前正在录放的音频任务
			obj->Cb.MediaClose(obj->Media, 1);
			obj->Cb.MediaDestroy(obj->Media);
			
			ListRemove(&obj->Link);
			HandleDestroy(obj->Header.Handle, MEDIA_MAGIC);
			if(obj->MediaInfo)
				kfree(obj->MediaInfo);
			kfree(obj);
		}
	}
}


////////////////////////////////////////////////////
// 功能: 取消媒体播放锁定
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
void MediaTerminateUnlock(void)
{
	kMutexRelease(hMediaMutex);
}

