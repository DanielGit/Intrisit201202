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


/***************************************************
    The following is an example to use memory porting functions

    void *pBuffer1;
    void *pBuffer2;
    
    pBuffer1 = VD_Malloc(128);
    VD_ZeroMemory(pBuffer1, 128);
    pBuffer2 = VD_Malloc(256);
    VD_ReMalloc(pBuffer2, 128);
    
    VD_Free(pBuffer1);
    VD_Free(pBuffer2);
 ***************************************************/


#ifndef __VIDEO_MEM_PORT_H__
#define __VIDEO_MEM_PORT_H__

#include <platform/platform.h>


/*
 * @brief Allocate memory
 *
 * @author Feng_Yunqing
 * @param   size        memory allocation size
 * @return  memory pointer
 */
void *VD_Malloc(T_U32 size);


/*
 * @brief Free memory
 *
 * @author Feng_Yunqing
 * @param   mem     memory pointer
 * @return  void
 */
void VD_Free(void *mem);


/*
 * @brief Free memory and allocate with new size
 *
 * @author Feng_Yunqing
 * @param   mem         original memory memory pointer
 * @param   size        new memory size
 * @return  memory pointer
 */
void *VD_ReMalloc(void *mem, T_U32 size);


/*
 * @brief Set memory with 0x00
 *
 * @author Feng_Yunqing
 * @param   mem         memory pointer
 * @param   size        memory size
 * @return  memory pointer
 */
void *VD_ZeroMemory(void *mem, T_U32 size);

/*
 * @brief Copy Mem data through DMA
 *
 * @author Liu_Zhenwu
 * @param   dst         destination addr
 * @param   src         source addr
 * @param   count       copy length
 * @return  memory pointer
 */
T_pVOID VD_DMAMemcpy(T_pVOID dst, T_pVOID src, unsigned int count);


/*
 * @��Ƶ��Ӧ�ֻ�ƽ̨��Ҫ���Ӵ˺������ã������ϲ�Ҫ����ҲҪ�ṩ�������
 * @��Ƶ��V2.9.1��ʼ��Ҫ��
 */
void  VD_MMUCleanCache(void);

#endif

