//[PROPERTY]===========================================[PROPERTY]
//            *****  ŵ�����۲���ϵͳV2  *****
//        --------------------------------------
//    	            ��Ƶ��ѹ������������
//        --------------------------------------
//                 ��Ȩ: ��ŵ���ۿƼ�
//             ---------------------------
//                  ��   ��   ��   ʷ
//        --------------------------------------
//  �汾    ��ǰ		˵��		
//  V0.1    2009-6      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef _MOVIE_FS_PORT_H__
#define _MOVIE_FS_PORT_H__

#include <platform/platform.h>

typedef     T_S32   T_hFILE;

#define     VD_FS_SEEK_SET              0
#define     VD_FS_SEEK_CUR              1
#define     VD_FS_SEEK_END              2


T_hFILE  VD_FileOpen(T_pCSTR path, T_U8 Mode);
T_BOOL      VD_FileClose(T_hFILE aviFile);
T_S32       VD_FileRead(T_hFILE aviFile , T_pVOID buffer, T_U32 count);
T_S32       VD_FileSeek(T_hFILE aviFile , T_S32 offset, T_U16 origin);
T_S32       VD_FileWrite(T_hFILE aviFile , T_pVOID buffer, T_U32 count);
T_BOOL      VD_FileDelete(T_pCSTR path);
T_S32       VD_FileGetCurPos(T_hFILE aviFile);
T_U32       VD_FileGetLen(T_hFILE aviFile);

#endif

