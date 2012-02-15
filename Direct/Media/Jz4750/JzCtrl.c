//[PROPERTY]===========================================[PROPERTY]
//            *****  ŵ�����۲���ϵͳV2  *****
//        --------------------------------------
//    	          ֱ����Ƶ���ſ��ƺ�������
//        --------------------------------------
//                 ��Ȩ: ��ŵ���ۿƼ�
//             ---------------------------
//                  ��   ��   ��   ʷ
//        --------------------------------------
//  �汾    ��ǰ		˵��
//  V0.1    2009-5      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#include <kernel/kernel.h>
#include <kernel/kernel.h>
#include <kernel/irq.h>
#include <kernel/device.h>
#include <kernel/karch.h>
#include <platform/platform.h>
#include <direct/media.h>

extern int DacInit(void);
extern int AdcInit(void);
extern int DacSetTempo(HANDLE hdac, int tempo);
extern int AdcSetEq(HANDLE hadc, int filter);
extern void SetMuteMode(char mode);
extern int MplayerResizeVideo(PAK_OBJECT obj,PMEDIA_TASK task);
extern void MillinsecoundDelay(unsigned int msec);
extern void SetPowerAmplifier(char io);
extern int GetDacChannel(void);
//int seek_flag = 0;
////////////////////////////////////////////////////
// ����:
// ����:
// ���:
// ����:
// ˵��:
////////////////////////////////////////////////////
int JzCtlInit(void)
{	
	// ADC & DAC�豸��ʼ��
	AdcInit();
	DacInit();
	return 0;
}


////////////////////////////////////////////////////
// ����:
// ����:
// ���:
// ����:
// ˵��:
////////////////////////////////////////////////////
int JzCtlVolumeSet(PAK_OBJECT obj, int volume)
{
	obj->Task.Volume = volume;
	if(obj->Task.Record)
		return AdcSetVolume(obj->hDac, volume);
	return DacSetVolume(obj->hDac, volume);
}


////////////////////////////////////////////////////
// ����:
// ����:
// ���:
// ����:
// ˵��:
////////////////////////////////////////////////////
int JzCtlPause(PAK_OBJECT obj)
{
	// ����Ƿ�Ϊ¼��״̬
	if(obj->Task.Record)
	{
		obj->bRecPause = 1;
		return 0;
	}

	MediaLibPause(obj);

	obj->bPlayerPause = 1;
	// ����ü�·�����ڲ���, ���ܾ���
	// ע��GetDacChannel()��������ʹ���˻�����,
	// ��JzCtlPause()������ʱҲ��ʹ�û�����, С������
	if(GetDacChannel() <= 1)
	{
		SetPowerAmplifier(0);
		MillinsecoundDelay(20);
		SetMuteMode(0);				//�ر�����
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
int JzCtlForward(PAK_OBJECT obj)
{
	return -1;
}


////////////////////////////////////////////////////
// ����:
// ����:
// ���:
// ����:
// ˵��:
////////////////////////////////////////////////////
int JzCtlBackward(PAK_OBJECT obj)
{
	return -1;
}


////////////////////////////////////////////////////
// ����:
// ����:
// ���:
// ����:
// ˵��:
////////////////////////////////////////////////////
int JzCtlResume(PAK_OBJECT obj)
{
	// ����Ƿ�Ϊ¼��״̬
	if(obj->Task.Record)
	{
		obj->bRecPause = 0;
		return 0;
	}

	MediaLibResume(obj);
	obj->bPlayerPause = 0;
	// �����ж���·�����ڲ���, ����Ҫ��������.
	SetMuteMode(1);		//������ͨ��
	MillinsecoundDelay(20);
	SetPowerAmplifier(1);
	return 0;
}


////////////////////////////////////////////////////
// ����:
// ����:
// ���:
// ����:
// ˵��:
////////////////////////////////////////////////////
int JzCtlSeek(PAK_OBJECT obj, DWORD milliSecond)
{
 	MEDIALIB_STATUS player_status;
	
	// ����Ƿ�Ϊ¼��״̬
	if(obj->Task.Record)
		return -1;
	
	// ����Ƿ�����seek����
	if(!obj->Info.bAllowSeek)
	{
		kdebug(mod_media, PRINT_ERROR, "the media unsupport seeking!\n");
		return -1;
	}
 
 	// ��ͣ����
 	player_status = MediaLibGetStatus(obj);
	if(player_status == MEDIALIB_SEEK)
		return 0;

	if((player_status == MEDIALIB_PLAYING) || (player_status == MEDIALIB_PAUSE))
	{
		MediaLibSeek(obj, milliSecond);
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
int JzCtlGetDuration(PAK_OBJECT obj, DWORD *duration)
{
	int ret;
	
	// ����Ƿ�Ϊ¼��״̬
	if(obj->Task.Record)
	{
		*duration = obj->RecDuration;
		return 0;
	}

	ret = obj->RecDuration;
	if(ret >= 0)
	{
		*duration = ret;
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
int JzCtlIsPlaying(PAK_OBJECT obj)
{
	// ����Ƿ�Ϊ¼��״̬
	if(obj->Task.Record)
	{
		if(obj->bRecPause)
			return 1;
		return 0;
	}

	if( obj->bPlayerPause )
		return 1;

	if( MediaLibGetStatus(obj) == MEDIALIB_PLAYING )
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
int JzCtlPlaySpeed(PAK_OBJECT obj, int tempo)
{
	if(obj->Task.Record)
		return -1;
	return DacSetTempo(obj->hDac, tempo);
}


////////////////////////////////////////////////////
// ����:
// ����:
// ���:
// ����:
// ˵��:
////////////////////////////////////////////////////
int JzCtlEq(PAK_OBJECT obj, int eq)
{
	if(obj->Task.Record)
		return AdcSetEq(obj->hDac, eq);
	return -1;
}

////////////////////////////////////////////////////
// ����:
// ����:
// ���:
// ����:
// ˵��:
////////////////////////////////////////////////////
int JzCtlResize(PAK_OBJECT obj)
{
	kdebug(mod_media, PRINT_INFO, "resize: x = %d, y = %d, w = %d, h = %d\n",
		obj->Task.ShowRect.x,obj->Task.ShowRect.y,obj->Task.ShowRect.w,obj->Task.ShowRect.h);
	return MplayerResizeVideo(obj,&obj->Task);
}

////////////////////////////////////////////////////
// ����:
// ����:
// ���:
// ����:
// ˵��:
////////////////////////////////////////////////////
int JzCtlGetTotalTime(PAK_OBJECT obj, DWORD* totaltime)
{
	*totaltime = obj->Info.TotalTime;
	kdebug(mod_media, PRINT_INFO, "get total time : %d\n",*totaltime);
	return 1;
}

////////////////////////////////////////////////////
// ����:
// ����:
// ���:
// ����:
// ˵��:
////////////////////////////////////////////////////
int JzCtlSetAB(PAK_OBJECT obj,DWORD start, DWORD end)
{
	int ret;

	kprintf("JzCtlSetAB: start = %d, end = %d\n",start,end);
	if( end <= start )
		return -1;

	ret = -1;
	ret = JzCtlSeek(obj,start);
	if(  ret < 0 )
	{
		kprintf("JzCtlSetAB: seek error,ret = %d\n",ret);
		return -1;
	}
	obj->EndTime = end;
	return ret;
}
