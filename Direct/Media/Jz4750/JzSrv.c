//[PROPERTY]===========================================[PROPERTY]
//            *****  ŵ�����۲���ϵͳV2  *****
//        --------------------------------------
//    	             ֱ����Ƶ���Ų���
//        --------------------------------------
//                 ��Ȩ: ��ŵ���ۿƼ�
//             ---------------------------
//                  ��   ��   ��   ʷ
//        --------------------------------------
//  �汾    ��ǰ		˵��
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
// ����:
// ����:
// ���:
// ����:
// ˵��:
////////////////////////////////////////////////////
void JzSrvInit(void)
{
	JzCtlInit();
	ListInit(&JzObjList);
	MediaSrvRegistCallback(MEDIA_TYPE_AUDIO, MEDIA_DEVICE_AUDIO|MEDIA_DEVICE_VIDEO, &JzCallback);
	MediaSrvRegistCallback(MEDIA_TYPE_VIDEO, MEDIA_DEVICE_AUDIO|MEDIA_DEVICE_VIDEO, &JzCallback);
}



////////////////////////////////////////////////////
// ����:
// ����:
// ���:
// ����:
// ˵��:
////////////////////////////////////////////////////
int JzSrvDestroyNotify(PMEDIA_TASK task)
{
	PLIST head;
	PLIST n;
	PAK_OBJECT obj;
	
	JZ_LOCK();
	head = &JzObjList;
	
	// ����ָ����Ƶ����
	for(n = ListFirst(head); n != head; n = ListNext(n))
	{
		obj = ListEntry(n, AK_OBJECT, Link);
		if(&obj->Task == task)
		{
			// �ͷ���Ϣ�ṹ��
			JzSrvCtrlDeinit(&obj->Ctrl);
			
			// �Ƿ������ļ��ṹ��
			JzSrvDataFileDeinit(&obj->File);	
			
			// ���ϴ���NOTIFY�¼�
			MediaSrvDestroyNotify(obj->Header.Handle);

			// �ͷŵ�ǰ�ڵ�
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
// ����: �����Ƶ�豸�Ƿ���������
// ����: 
// ���:
// ����: 
// ˵��: ����CPU����Ƿ����Ƶ���л�
////////////////////////////////////////////////////
int JzSrvMode(void)
{
	return 1;
}


////////////////////////////////////////////////////
// ����: �����Ƶ�豸�Ƿ���������
// ����: 
// ���:
// ����: 
// ˵��: ����CPU����Ƿ����Ƶ���л�
////////////////////////////////////////////////////
int JzSrvPowerOff(int mode)
{
	return 0;
}

//void SetSeekTime( int f);
////////////////////////////////////////////////////
// ����: 
// ����:
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
static int JzSrvDataFileInit(PAK_VFILE vfile, PMEDIA_TASK task)
{
	int len,dot_pos,i;
	// ��ȡ����ASID
#if defined(CONFIG_MMU_ENABLE)
	vfile->Asid = VPageGetAsid();
#endif
	if( task->FileName )
	{	//�����ļ���չ��
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
	// ����Ƿ�¼��Ϊ�ļ�
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
	
	// ¼�������������߲����ļ�
	if(task->FileName)
	{	
		int fsize;
		
		// ���ļ�
		vfile->File = kfopen(task->FileName, "rb");
		if(vfile->File == NULL)
			return -1;
		
		// ��ȡ�ļ���С
		fsize = kfsize(vfile->File);
		if(task->PlayOffset >= fsize)
			return -1;
		kfseek(vfile->File, task->PlayOffset, SEEK_SET);
		
		// ��ʼ���������
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
	else if(task->DataBuf && (task->BufSize > 0))		// ���Ż������е�����
	{
		// ��ʼ������/¼�Ƴ���
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
		
		// ��ʼ����������
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
// ����: 
// ����:
// ���:
// ����: 
// ˵��: 
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
// ����: 
// ����:
// ���:
// ����: 
// ˵��: 
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
// ����: 
// ����:
// ���:
// ����: 
// ˵��: 
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
// ����: ��ʽ���������
// ����:
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
static int JzSrvTaskInit(PMEDIA_TASK dst, PMEDIA_TASK src)
{
	// ���ƽṹ��
	kmemcpy(dst, src, sizeof(MEDIA_TASK));

	// 1. ý���ļ����͸�ʽ��
	if(!src->Record)
		dst->MediaType = 0;
	
	return 0;
}


////////////////////////////////////////////////////
// ����: ���벢��ʼ���ڵ�
// ����:
// ���:
// ����:
// ˵��:
////////////////////////////////////////////////////
static HANDLE JzSrvCreate(void *media)
{
	PMEDIA_TASK task;
	PAK_OBJECT obj;
	HANDLE ret;

	// ����ڵ�
	task = (PMEDIA_TASK)media;
	obj = kmalloc(sizeof(AK_OBJECT));
	if(obj == NULL)
		return NULL;
	kmemset(obj, 0x00, sizeof(AK_OBJECT));
	
	// ��ʼ������ṹ��
	JZ_LOCK();	
	if(JzSrvTaskInit(&obj->Task, task) < 0)
	{
		kfree(obj);
		JZ_UNLOCK();
		return NULL;
	}

	// ��ʼ��Ϣ�ṹ��
	if(JzSrvCtrlInit(&obj->Ctrl) < 0)
	{
		kfree(obj);
		JZ_UNLOCK();
		return NULL;
	}
		
	// ��ʼ�������ļ��ṹ��
	if(JzSrvDataFileInit(&obj->File, task) < 0)
	{
		JzSrvCtrlDeinit(&obj->Ctrl);
		kfree(obj);
		JZ_UNLOCK();
		return NULL;
	}
	
	// �Ѵ����Ķ�����������
	ListInsert(&JzObjList, &obj->Link);
	ret = HandleSet(obj, VIDEO_MAGIC, JzSrvDestroy);
	JZ_UNLOCK();
	return ret;
}


////////////////////////////////////////////////////
// ����: ����һ����Ƶ����
// ����: 
// ���:
// ����: 
// ˵��:
////////////////////////////////////////////////////
static int JzSrvDestroy(HANDLE hvedio)
{
	PAK_OBJECT obj;

	JZ_LOCK();
	
	// ��ȡ��Ƶ����
	obj = (PAK_OBJECT)HandleGet(hvedio, VIDEO_MAGIC);
	if(obj == NULL)
	{
		JZ_UNLOCK();
		return -1;
	}
	
	// �ͷ���Ϣ�ṹ��
	JzSrvCtrlDeinit(&obj->Ctrl);
	
	// �Ƿ������ļ��ṹ��
	JzSrvDataFileDeinit(&obj->File);	

	// �ͷŵ�ǰ�ڵ�
	ListRemove(&obj->Link);
	HandleDestroy(hvedio, VIDEO_MAGIC);
	kfree(obj);	
	JZ_UNLOCK();
	return 0;
}

		
////////////////////////////////////////////////////
// ����: ���������Ϣ������
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
int JzSrvProcMsg(PAK_OBJECT obj)
{
	int ret;
	RECT cr;
	
	// �����߽�
	kMutexWait(obj->Ctrl.hMutex);
	if(obj->Ctrl.Code < 0)
	{
		kMutexRelease(obj->Ctrl.hMutex);
		return -1;
	}
	
	// ������Ϣ
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
	
	// ɾ����Ϣ	
	kMutexRelease(obj->Ctrl.hMutex);
	return ret;
}



////////////////////////////////////////////////////
// ����: ��Ƶ�豸����
// ����:
// ���:
// ����:
// ˵��:
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

	// ��ȡ��Ƶ����
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
	
	// �����߽�
	kMutexWaitNoLock(hmutex);
	
	// ��ʼ�����ƽṹ��
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
	else if(size > 0)	// ���ٴ���
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

	// �ȴ����ƴ���	
	hsema = obj->Ctrl.hSema;
	kMutexRelease(hmutex);

	//���»�ȡ������жϵ�ǰ�����Ƿ�ɾ��
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

	// ��ȡ���Ʒ���ֵ
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
// ����: 
// ����:
// ���:
// ����:
// ˵��:
////////////////////////////////////////////////////
static int JzSrvOpen(HANDLE hvedio)
{
	PAK_OBJECT obj;
	int ret = 0;
	
	// ��ȡ��Ƶ����
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
// ����: 
// ����:
// ���:
// ����:
// ˵��:
////////////////////////////////////////////////////
static int JzSrvClose(HANDLE hvedio, int code)
{
	PAK_OBJECT obj;
	
	// ��ȡ��Ƶ����
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
// ����: 
// ����:
// ���:
// ����:
// ˵��:
////////////////////////////////////////////////////
int JzSrvInfo(void *media, void *info, int type)
{
	PMEDIA_TASK task;
	PAK_OBJECT obj;
	int ret;

	// ���DSP�Ƿ�ռ��
	JZ_LOCK();
	
	// ����ڵ�
	task = (PMEDIA_TASK)media;
	obj = kmalloc(sizeof(AK_OBJECT));
	if(obj == NULL)
		return -1;
	kmemset(obj, 0x00, sizeof(AK_OBJECT));
	
	// ��ʼ������ṹ��
	JZ_LOCK();	
	if(JzSrvTaskInit(&obj->Task, task) < 0)
	{
		kfree(obj);
		JZ_UNLOCK();
		return -1;
	}

	// ��ʼ�������ļ��ṹ��
	if(JzSrvDataFileInit(&obj->File, task) < 0)
	{
		JzSrvCtrlDeinit(&obj->Ctrl);
		kfree(obj);
		JZ_UNLOCK();
		return -1;
	}
	
	// ��ʼ����Ϣ
	obj->bGetInfo = 1;
	
	// �����߳�
	ret = JzThreadOpen(obj);
	
	// ���ز���
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
	
	// �Ƿ������ļ��ṹ��
	JzSrvDataFileDeinit(&obj->File);	

	// ���ٶ���
	kfree(obj);
	JZ_UNLOCK();
	return ret;
}

////////////////////////////////////////////////////
// ����: 
// ����:
// ���:
// ����:
// ˵��:
////////////////////////////////////////////////////
int JzSrvUseMplayer(void)
{
	PLIST head;
	PLIST n;
	PMEDIA_TASK task;
	PAK_OBJECT obj;

	JZ_LOCK();
	head = &JzObjList;
	// ����ָ����Ƶ����
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
