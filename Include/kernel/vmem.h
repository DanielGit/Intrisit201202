//[PROPERTY]===========================================[PROPERTY]
//            *****   ���Ӵʵ��ļ�ϵͳ  *****
//        --------------------------------------
//                    �ں����ͷ�ļ�
//        --------------------------------------
//                 ��Ȩ: ��ŵ���ۿƼ�
//             ---------------------------
//                  ��   ��   ��   ʷ
//        --------------------------------------
//  �汾    ��ǰ		˵��
//  V0.1    2007-4      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef _VM_H
#define _VM_H

#include <platform/platform.h>
#include <kernel/kernel.h>

// �ڴ�ѷ�����ƿ�ṹ����(�ʺϲ���MMU�������ڴ��)
typedef struct
{
	DWORD		KPoolStart;		// �ڴ����ʼ��ַָ��
	DWORD		KPoolEnd;		// �ڴ�ѽ�����ַָ��
	DWORD		AllocSize;		// �ѷ����С(�ֽ�)
}MEM_KPOOL;
typedef MEM_KPOOL *PMEM_KPOOL;

// �ڴ�ѷ�����ƿ�ṹ����(��Ӧ��MMU�������ڴ��)
typedef struct
{
	int			Asid;			// �ڴ�ض�Ӧ��ASID��
	int			PageSize;		// �ڴ�ط�ҳ��С
	DWORD		UPoolStart;		// �ڴ����ʼ��ַָ��
	DWORD		UPoolEnd;		// �ڴ�ѽ�����ַָ��
	DWORD		AllocSize;		// �ѷ����С(�ֽ�)
#if defined(CONFIG_MMU_ENABLE)	
	DWORD		VMap[0x200];	// ����������ҳ��λͼ
	DWORD		VPages;			// ����������ҳ����
#endif
}MEM_UPOOL;
typedef MEM_UPOOL *PMEM_UPOOL;

// VM attributes
#define VMA_READ	0x01
#define VMA_WRITE	0x02
#define VMA_EXEC	0x04

int KPoolAlloc(void **pmem, DWORD size, DWORD regid);
int KPoolRealloc(void **pmem, DWORD size, DWORD regid);
int KPoolFree(void * pmem);
int KPoolInit(void);
int KPoolGetPoolStatus(DWORD *total, DWORD *free);

int UPoolAlloc(PMEM_UPOOL pool, void **pmem, DWORD size);
int UPoolRealloc(PMEM_UPOOL pool, void **pmem, DWORD size);
int UPoolFree(PMEM_UPOOL pool, void * pmem);
PMEM_UPOOL UPoolCreate(DWORD start, int id);
int UPoolDestroy(PMEM_UPOOL pool);
int UPoolGetPoolStatus(PMEM_UPOOL pool, DWORD *blks, DWORD *alcsz);
int UPoolCheckPoolLeak(MEM_UPOOL *pool);

void *VmCreate(DWORD start, int id);
int VmDestroy(void *pool);
int	 VmAccess(void *, size_t, int);
int VmAttribute(HANDLE htask, void *addr, int attr);

#if defined(STC_EXP)
int	sVmAlloc(HANDLE, void **, size_t, int);
int sVmRealloc(HANDLE, void **, size_t, int);
int sVmAllocStack(HANDLE, void **, size_t);
int	sVmFree(HANDLE, void *);
int	sVmFreeStack(HANDLE, void *);
int sVmAllocExec(HANDLE, void **, size_t);
int	sVmFreeExec(HANDLE, void *);
int	sVmAttribute(HANDLE, void *, int);
int	sVmMap(HANDLE, void *, size_t, void **);
int sVmInfo(HANDLE, DWORD *blks, DWORD *alcsz);
int sVmInfoAll(DWORD *blks, DWORD *alcsz);
#else
int	VmAlloc(HANDLE, void **, size_t, int);
int VmRealloc(HANDLE, void **, size_t, int);
int VmAllocStack(HANDLE, void **, size_t);
int	VmFree(HANDLE, void *);
int	VmFreeStack(HANDLE, void *);
int VmAllocExec(HANDLE, void **, size_t);
int	VmFreeExec(HANDLE, void *);
int	VmAttribute(HANDLE, void *, int);
int	VmMap(HANDLE, void *, size_t, void **);
int VmInfo(HANDLE, DWORD *blks, DWORD *alcsz);
int VmInfoAll(DWORD *blks, DWORD *alcsz);
#endif

#endif // !_VM_H
