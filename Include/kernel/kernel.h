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


#ifndef _KERNEL_H
#define _KERNEL_H

#include <config.h>
#include <sys/errno.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <arch/arch.h>
#include <kernel/list.h>
#include <kernel/bootinfo.h>
#include <kernel/handle.h>
#include <filesys/fs.h>
#include <libc/setjmp.h>

typedef struct _PERIPHERAL
{
	int IntSet;
	int Vector;
	int Prio;
	int Mode;
	int (*Isr)(int);
	int (*Ist)(int);
}PERIPHERAL;
typedef PERIPHERAL *PPERIPHERAL;

#ifndef _STRUCT_TM_
#define _STRUCT_TM_
struct tm
{
	int tm_sec;     // seconds after the minute - [0,59]
	int tm_min;     // minutes after the hour - [0,59]
	int tm_hour;    // hours since midnight - [0,23]
	int tm_mday;    // day of the month - [1,31]
	int tm_mon;     // months since January - [0,11]
	int tm_year;    // years since 1900
	int tm_wday;    // days since Sunday - [0,6]
	int tm_yday;    // days since January 1 - [0,365]
	int tm_isdst;   // daylight savings time flag
};
#endif

#define SEEK_CUR				1
#define SEEK_END				2
#define SEEK_SET				0

// 内核全局变量
extern struct _KERNEL_THREAD *gCurThread;	// 当前运行线程指针
extern struct _KERNEL_TASK gKernTask;		// 内核任务
extern struct _BOOT_INFO gNoahOsBootInfo;	// 启动信息指针
extern volatile int	gIrqLevel;				// 当前中断级别

void *kmalloc(size_t);
void *krealloc(void *mem, size_t size);
void kfree(void *);

int kmemcmp(const void *dst, const void *src, size_t count);
void *kmemset(void *dest, int ch, size_t count);
void *kmemcpy(void *dest, const void *src, size_t count);
void *kmemmove(void * dst,const void * src,size_t count);
int kstrncmp(const char *src, const char *tgt, size_t count);
size_t kstrnlen(const char *str, size_t count);
size_t kstrlen(const char *s);
char *kstrcpy (char *s1, const char *s2);
char *kstrcat(char *s, const char *append);
char *kstrrchr(const char *t, int c);
char *kstrncpy(char *dest, const char *src, size_t n);
char *kstrchr(register const char *t, int c);
int kstrcmp(const char *s1, const char *s2);
size_t kstrlcpy(char *dest, const char *src, size_t count);
char * kstrstr(const char *s, const char *find);
int kstricmp(const char *s1, const char *s2);
int ktolower(char c);
int ktoupper(char c);

struct tm *klocaltime(const time_t *timer);
struct tm *kgmtime(const time_t *t);
time_t kmktime(struct tm *t);
time_t ktime(time_t *t);

void kpanic(const char *fmt, ...);
void kassert(const char *file, int line, const char *exp);
int kprintf(const char *fmt, ...);

void kdebug(int module, int level, char * fmt, ...);
void kkdebug(int level, char * fmt, ...);
#ifndef WIN32
void syslog(char * module, int level, char *filename, int lineno, char * fmt, ...);
void _syslog(int module, int level, char *filename, int lineno, char * fmt, ...);

#define ksyslog(module, level, fmt, arg...)	_syslog(module, level, __FILE__, __LINE__, fmt, ##arg)
#define kksyslog(level, fmt, arg...)		_syslog(54, level, __FILE__, __LINE__, fmt, ##arg)

#else
void ksyslog(int module, int level, char * fmt, ...);
void kksyslog(int level, char * fmt, ...);
#endif

int ksprintf(char *buf, const char *fmt, ...);
int kfprintf(FILE *fh, const char *fmt, ...);

int krand(void);
DWORD kclock(void);

int ksetjmp(jmp_buf buf);
void klongjmp(jmp_buf buf, int ret_val);

FILE *kfopen(const char *, const char *);
int kfseek(FILE *, long, int);
long kftell(FILE *);
long kfsize(FILE *);
size_t kfwrite(const void *, size_t, size_t, FILE *);
size_t kfread(void *, size_t, size_t, FILE *);
int kferror(FILE *hfile);
int kfremove(const char *pname);
int kfrename(const char *oldname, const char *newname);
int kfmove(const char *oldname, const char *newname);
int kfproperty(const char *pname, FILE_ENTRY *pentry);
int kftrim(const char *pname, long size);

int kfclose(FILE *);
HANDLE kfdopen(const char *pname);
int kfdread(HANDLE hdir, DIR_ENTRY *pentry);
int kfdclose(HANDLE hdir);
int kfdmake(const char *pname);
int kfdremove(const char *pname);
int kfdrename(const char *oldname, const char *newname);
int kfdmove(const char *oldname, const char *newname);

DWORD NameHash(const char *name);
DWORD Crc32(const BYTE* s, DWORD len);

WORD GbkToUnicode(BYTE *s, int *len);
int UnicodeToGbk(WORD ucode, BYTE *s);

WORD Big5ToUnicode(BYTE *s, int *len);
int UnicodeToBig5(WORD ucode, BYTE *s);

WORD KscToUnicode(BYTE *s, int *len);
int UnicodeToKsc(WORD ucode, BYTE *s);

WORD SjisToUnicode(BYTE *s, int *len);
int UnicodeToSjis(WORD ucode, BYTE *s);

WORD Iso1ToUnicode(BYTE code);
BYTE UnicodeToIso1(WORD code);

WORD Iso2ToUnicode(BYTE code);
BYTE UnicodeToIso2(WORD code);

WORD Iso5ToUnicode(BYTE code);
BYTE UnicodeToIso5(WORD code);

WORD Iso7ToUnicode(BYTE code);
BYTE UnicodeToIso7(WORD code);

WORD Iso9ToUnicode(BYTE code);
BYTE UnicodeToIso9(WORD code);

WORD VietnamToUnicode(BYTE code);
BYTE UnicodeToVietnam(WORD code);

int UmemCopyin(void *, void *, size_t);
int UmemCopyout(void *, void *, size_t);
int UmemStrnlen(const char *, size_t, size_t *);

int KernelBootMode(void);

#define PRINT_DEBUG		5
#define PRINT_INFO		4
#define PRINT_NOTICE		3
#define PRINT_WARNING		2
#define PRINT_ERROR		1
#define PRINT_PANIC		0

#define	mask_buzzer		(1<<0)
#define mask_clock		(1<<1)
#define	mask_codec		(1<<2)
#define mask_grid		(1<<3)
#define	mask_inand		(1<<4)
#define mask_indicator		(1<<5)
#define mask_key		(1<<6)
#define mask_lcdc		(1<<7)
#define mask_memory		(1<<8)
#define mask_multiio		(1<<9)
#define mask_nand		(1<<10)
#define mask_powman		(1<<11)
#define mask_rtc		(1<<12)
#define mask_sd			(1<<13)
#define mask_touch		(1<<14)
#define mask_uart		(1<<15)
#define mask_usb		(1<<16)
#define mask_watchdog		(1<<17)

#define	dev_buzzer		0
#define dev_clock		1
#define	dev_codec		2
#define dev_grid		3
#define	dev_inand		4
#define dev_indicator		5
#define dev_key			6
#define dev_lcdc		7
#define dev_memory		8
#define dev_multiio		9
#define dev_nand		10
#define dev_powman		11
#define dev_rtc			12
#define dev_sd			13
#define dev_touch		14
#define dev_uart		15
#define dev_usb			16
#define dev_watchdog		17

#define mod_boot		(32+0)
#define mod_audio		(32+1)
#define mod_video		(32+2)
#define mod_media		(32+3)
#define mod_fs			(32+4)
#define mod_font		(32+5)
#define mod_gui			(32+6)
#define mod_kmem		(32+7)
#define mod_kcore		(32+8)
#define mod_karch		(32+9)
#define mod_kernel		(32+10)
#define mod_mplayer		(32+11)
#define mod_device		(32+12)
#define mod_reg			(32+13)
#define mod_simulator		(32+14)
#define mod_elf			(32+15)
#define mod_devcheck		(32+16)
#define mod_syslib		(32+17)
#define mod_sleep		(32+18)
#define mod_usrlib		(32+19)
#define mod_handwrite		(32+20)
#define mod_burn		(32+21)
#define mod_misc		(32+22)
#define mod_mouse		(32+23)

#define mask_boot		(1<<(32+0))
#define mask_audio		(1<<(32+1))
#define mask_video		(1<<(32+2))
#define mask_media		(1<<(32+3))
#define mask_fs			(1<<(32+4))
#define mask_font		(1<<(32+5))
#define mask_gui		(1<<(32+6))
#define mask_kmem		(1<<(32+7))
#define mask_kcore		(1<<(32+8))
#define mask_karch		(1<<(32+9))
#define mask_kernel		(1<<(32+10))
#define mask_mplayer		(1<<(32+11))
#define mask_device		(1<<(32+12))
#define mask_reg		(1<<(32+13))
#define mask_simulator		(1<<(32+14))
#define mask_elf		(1<<(32+15))
#define mask_devcheck		(1<<(32+16))
#define mask_syslib		(1<<(32+17))
#define mask_sleep		(1<<(32+18))
#define mask_usrlib		(1<<(32+19))
#define mask_handwrite		(1<<(32+20))
#define mask_burn		(1<<(32+21))
#define mask_misc		(1<<(32+22))
#define mask_mouse		(1<<(32+23))

#define BOOT_FROM_NAND	0
#define BOOT_FROM_SPI	1
#define BOOT_FROM_INAND	2
#define BOOT_FROM_USB	3

#define ASSERT(exp)	do { if (!(exp)) \
	kassert(__FILE__, __LINE__, #exp); } while (0)

#endif // !_KERNEL_H
