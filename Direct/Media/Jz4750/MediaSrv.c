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
// ����:
// ����:
// ���:
// ����:
// ˵��:
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
// ����:
// ����:
// ���:
// ����:
// ˵��:
////////////////////////////////////////////////////
void MediaSrvLock(void)
{
	kMutexWait(hMediaMutex);
}

////////////////////////////////////////////////////
// ����:
// ����:
// ���:
// ����:
// ˵��:
////////////////////////////////////////////////////
void MediaSrvUnlock(void)
{
	kMutexRelease(hMediaMutex);
}


////////////////////////////////////////////////////
// ����: ��Ƶ����ػ�����
// ����: 
// ���:
// ����: 
// ˵��:
////////////////////////////////////////////////////
int MediaSrvPowerOff(int mode)
{
	JzSrvPowerOff(mode);
	return 0;
}


////////////////////////////////////////////////////
// ����: �����Ƶ�豸�Ƿ���������
// ����: 
// ���:
// ����: 
// ˵��: ����CPU����Ƿ����Ƶ���л�
////////////////////////////////////////////////////
int MediaSrvMode(void)
{
	if(JzSrvMode())
		return 1;
	return 0;
}


////////////////////////////////////////////////////
// ����:
// ����:
// ���:
// ����:
// ˵��:
////////////////////////////////////////////////////
int MediaSrvRegistCallback(int type, DWORD device, PMEDIA_CALLBACK callback)
{
	PLIST n;
	PLIST head;
	PCALLBACK_LINK check;
	PCALLBACK_LINK link;

	// ����ڵ㣬����ʼ��
	link = kmalloc(sizeof(CALLBACK_LINK));
	if(link == NULL)
		return -1;
	kmemcpy(&link->Callback, callback, sizeof(MEDIA_CALLBACK));
	link->Type = type;
	link->Device = device;
	ListInit(&link->Link);
	
	// ����豸�Ƿ��Ѿ�ע��
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
// ����:
// ����:
// ���:
// ����:
// ˵��:
////////////////////////////////////////////////////
int MediaSrvDestroyNotify(void *media)
{
	PLIST head;
	PLIST n;
	PMEDIA_OBJECT obj;
	
//	kMutexWait(hMediaMutex);
	head = &MediaObjList;
	
	// ����ָ����Ƶ����
	for(n = ListFirst(head); n != head; n = ListNext(n))
	{
		obj = ListEntry(n, MEDIA_OBJECT, Link);
		if(obj->Media == media)
		{
			// �ͷŵ�ǰ�ڵ�
			kdebug(mod_media, PRINT_INFO, "CLOSE Notify: 0x%x\n", obj->Header.Handle);
			ListRemove(&obj->Link);
			HandleDestroy(obj->Header.Handle, MEDIA_MAGIC);			
			if(obj->MediaInfo)
				kfree(obj->MediaInfo);
			kfree(obj);
			
			// ������һ���ȴ�����
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
// ����: ����ý�����ͣ���ȡý��ص�����
// ����:
// ���:
// ����:
// ˵��:
////////////////////////////////////////////////////
static int MediaSrvGetCallback(int type, PMEDIA_CALLBACK callback)
{
	PLIST n;
	PLIST head;
	PCALLBACK_LINK link;

	// ����豸�Ƿ��Ѿ�ע��
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
// ����: Ϊ��Ƶ�������벢��ʼ���ڵ�
// ����:
// ���:
// ����:
// ˵��:
////////////////////////////////////////////////////
static PMEDIA_OBJECT MediaSrvObjCreate(void *media, char *name, int type)
{
	PMEDIA_OBJECT obj;
	
	// ����������
	if(!media)
		return NULL;

	// ����ڵ㣬����ʼ��
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
	
	// ����ý������
	obj->Media = obj->Cb.MediaCreate(media);
	if(obj->Media == NULL)
	{
		kfree(obj);
		return NULL;
	}
	
	// ����ý������
	if(name)
	{
		obj->MediaInfo = kmalloc(kstrlen(name)+1);
		if(obj->MediaInfo)
			kstrcpy(obj->MediaInfo, name);
	}
	
	// ����ý������
#if defined(STC_EXP)
	obj->hTask = sTaskSelf();
#else
	obj->hTask = TaskSelf();
#endif	
	// ����ý�����
	return obj;
}


////////////////////////////////////////////////////
// ����: ����һ����Ƶ����
// ����: amedia: ����ṹ��
//       preempt: 0-����ռ�� 1-��ռ; 2-�ȴ�
// ���:
// ����: 0: �����ɹ� <0: ��������
// ˵��:
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

	// �������
	if(media == NULL)
		return NULL;

	// ��鵱ǰ�Ƿ����¼������
	kMutexWait(hMediaMutex);
	head = &MediaObjList;
	if(!ListEmpty(head) && (preempt != MEDIA_MODE_NORMAL))
	{
		// ����Ƿ�ǿ�н�������¼�ŵ���Ƶ
		if(preempt == MEDIA_MODE_PREEMPTIVE)
		{
			// ��ȡ������Ƶ����ڵ�
			n = ListFirst(head);
			obj = ListEntry(n, MEDIA_OBJECT, Link);

			// ������ǰ����¼�ŵ���Ƶ����
			obj->Cb.MediaClose(obj->Media, 1);
			obj->Cb.MediaDestroy(obj->Media);

			// �ͷŵ�ǰ�ڵ�
			ListRemove(&obj->Link);
			HandleDestroy(obj->Header.Handle, MEDIA_MAGIC);
			if(obj->MediaInfo)
				kfree(obj->MediaInfo);
			kfree(obj);

			// ���벢��ʼ���ڵ�
			obj = MediaSrvObjCreate(media, name, type);
			if(obj == NULL)
			{
				kMutexRelease(hMediaMutex);
				return NULL;
			}

			// �����µĲ�������
			if(obj->Cb.MediaOpen(obj->Media) < 0)
			{
				if(obj->MediaInfo)
					kfree(obj->MediaInfo);
				kfree(obj);
				kMutexRelease(hMediaMutex);
				return NULL;
			}

			// �ѽڵ�����������
			ListInsert(head, &obj->Link);
		}
		else
		{
			// ���벢��ʼ���ڵ�
			obj = MediaSrvObjCreate(media, name, type);
			if(obj == NULL)
			{
				kMutexRelease(hMediaMutex);
				return NULL;
			}

			// �ѵ�ǰ��Ƶ����ȴ�����β��
			ListInsert(ListPrev(head), &obj->Link);
		}
	}
	else
	{
		// ���벢��ʼ���ڵ�
		obj = MediaSrvObjCreate(media, name, type);
		if(obj == NULL)
		{
			kMutexRelease(hMediaMutex);
			return NULL;
		}

		// �����µ�¼������
		if(obj->Cb.MediaOpen(obj->Media) < 0)
		{
			if(obj->MediaInfo)
				kfree(obj->MediaInfo);
			kfree(obj);
			kMutexRelease(hMediaMutex);
			return NULL;
		}
		
		// �ѵ�ǰ��Ƶ����ȴ�����β��
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
// ����: ����һ����Ƶ����
// ����: 
// ���:
// ����: 
// ˵��:
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
	
	// ��ȡ��Ƶ����
	obj = (PMEDIA_OBJECT)HandleGet(hmedia, MEDIA_MAGIC);
	if(obj == NULL)
	{
		kdebug(mod_media, PRINT_ERROR, "CLOSE NULL: 0x%x\n", hmedia);
		kMutexRelease(hMediaMutex);
		return -1;
	}
	kdebug(mod_media, PRINT_INFO, "CLOSE: 0x%x\n", hmedia);
	
	// ������ǰ����¼�ŵ���Ƶ����
	playback = obj->Cb.MediaClose(obj->Media, 1);
	obj = (PMEDIA_OBJECT)HandleGet(hmedia, MEDIA_MAGIC);
	if(obj == NULL)
	{
		kdebug(mod_media, PRINT_ERROR, "CLOSE NULL NULL: 0x%x\n", hmedia);
		kMutexRelease(hMediaMutex);
		return -1;
	}
	obj->Cb.MediaDestroy(obj->Media);
	
	// �ͷŵ�ǰ�ڵ�
	ListRemove(&obj->Link);
	HandleDestroy(hmedia, MEDIA_MAGIC);
	if(obj->MediaInfo)
		kfree(obj->MediaInfo);
	kfree(obj);
	
	// ������һ���ȴ�����
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
// ����: ��ȡ��ǰ��Ƶ������Ϣ
// ����:
// ���: process�����ڲ���������Ϣ
// ����: ����¼�ź͵ȴ�¼����������
// ˵��:
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

	// ��ȡý��ʹ�õ��豸
	kMutexWait(hMediaMutex);
//	if(ListEmpty(&MediaObjList))
//	{
//		kMutexRelease(hMediaMutex);
//		return 0;
//	}
	
	// ѭ����ȡ�������ڲ��ŵ���Ƶ�������ƣ����÷ָ�������
	if(process)
		kstrcpy(process, "");
	tasks = 0;
	len = 0;
	head = &MediaObjList;
	for(n=ListFirst(head); n!=head; n=ListNext(n))
	{
		// ��ȡ���豸����
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
	
	// ��ȡ��DAC�豸���
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
// ����: ��Ƶ�豸����
// ����:
// ���:
// ����:
// ˵��:
////////////////////////////////////////////////////
#if defined(STC_EXP)
int sMediaSrvCtrl(HANDLE hmedia, int ctl, DWORD *param, int *size)
#else
int MediaSrvCtrl(HANDLE hmedia, int ctl, DWORD *param, int *size)
#endif
{
	PMEDIA_OBJECT obj;
	int ret;
	
	// ����Ƿ�����ȫ������
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
// ����: ��Ƶ�豸����
// ����:
// ���:
// ����:
// ˵��:
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
// ����: ��Ƶ�豸д
// ����:
// ���:
// ����:
// ˵��:
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
// ����: ����ĳһ�����ý�岥�ţ�������ý�岥������
// ����:
// ���:
// ����:
// ˵��:
////////////////////////////////////////////////////
void MediaTerminateLock(HANDLE htask)
{
	PMEDIA_OBJECT obj;
	PLIST list;
	
	kMutexWait(hMediaMutex);
	list = &MediaObjList;
	if(!ListEmpty(list))
	{
		// ��ȡ������Ƶ����ڵ�
		obj = ListEntry(ListFirst(list), MEDIA_OBJECT, Link);
		
		if(obj->hTask == htask)
		{
			// ������ǰ����¼�ŵ���Ƶ����
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
// ����: ȡ��ý�岥������
// ����:
// ���:
// ����:
// ˵��:
////////////////////////////////////////////////////
void MediaTerminateUnlock(void)
{
	kMutexRelease(hMediaMutex);
}

