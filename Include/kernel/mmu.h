//[PROPERTY]===========================================[PROPERTY]
//            *****   电子词典文件系统  *****
//        --------------------------------------
//                    内核相关头文件
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明
//  V0.1    2007-4      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#include <kernel/kernel.h>

#ifndef _KERNEL_MMU_H
#define _KERNEL_MMU_H

int VPageCreate(void);
void VPageDestroy(int asid);
int VPageAlloc(int asid, int vpage, int pages);
void VPageFree(int asid, int vpage, int pages);
int VPageProperty(int asid, int vpage, int pages, int prop);
int VPageCheck(int asid, int vpage);
int VPagePageSize(void);
int VPageSetAsid(int asid);
int VPageGetAsid(void);
int VPageMapDestroy(int asid);
int VPageMapCreate(int asid, DWORD pstart, DWORD vstart, DWORD size, int cache);

void PPageInit(void);
int PPageAlloc(DWORD *entry, int pages, DWORD prop);
void PPageFree(DWORD *entry, int pages);
void *PPageAllocTlb(void);
void PPageFreeTlb(void *tlb);
int PPageInfo(DWORD *frees, DWORD *totals);

void MmuInit(void);
int MmuGetPageSize(void);
int MmuGetTlbPages(void);
DWORD *MmuGetEntryAddr(int asid, DWORD vpage);
DWORD *MmuSetTlbPageAddr(int asid, DWORD vpage, DWORD addr);
void MmuVpageFree(int asid, DWORD vpage, int pages);
int MmuAsidNew(void);
void MmuAsidDelete(int asid);
int MmuGetAsid(void);
int MmuSetAsid(int asid);
int MmuEntryReload(void);

#endif // _KERNEL_MMU_H
