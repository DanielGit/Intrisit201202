//[PROPERTY]===========================================[PROPERTY]
//            *****   电子词典文件系统  *****
//        --------------------------------------
//                    处理器相关头文件
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明		
//  V0.1    2007-4      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef _KERNEL_TYPES_H_
#define	_KERNEL_TYPES_H_


#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif

typedef void				VOID;
typedef unsigned long       DWORD;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef unsigned int        UINT;
typedef int                 BOOL;
typedef float               FLOAT;
typedef int                 INT;
typedef void*				HANDLE;
typedef long 				LONG;

typedef UINT				*PUINT;
typedef VOID				*PVOID;
typedef FLOAT               *PFLOAT;
typedef BOOL	            *PBOOL;
typedef BYTE				*PBYTE;
typedef INT					*PINT;
typedef WORD				*PWORD;
typedef DWORD				*PDWORD;
typedef HANDLE				*PHANDLE;

typedef	signed char			int8_t;
typedef	unsigned char		uint8_t;
typedef	short				int16_t;
typedef	unsigned short		uint16_t;
typedef	int			  		int32_t;
typedef	unsigned int		uint32_t;

typedef	unsigned char		u_char;
typedef	unsigned short		u_short;
typedef	unsigned int		u_int;
typedef	unsigned long		u_long;
typedef	unsigned short		ushort;		/* Sys V compatibility */
typedef	unsigned int		uint;		/* Sys V compatibility */

typedef unsigned short 		wchar_t;

#ifdef	_BSD_CLOCK_T_
typedef	_BSD_CLOCK_T_		clock_t;
#undef	_BSD_CLOCK_T_
#endif

#ifdef	_BSD_SIZE_T_
typedef	_BSD_SIZE_T_		size_t;
#undef	_BSD_SIZE_T_
#endif

#ifdef	_BSD_SSIZE_T_
typedef	_BSD_SSIZE_T_		ssize_t;
#undef	_BSD_SSIZE_T_
#endif

#ifdef	_BSD_TIME_T_
typedef	_BSD_TIME_T_		time_t;
#undef	_BSD_TIME_T_
#endif

#define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
#define LOWORD(l)           ((WORD)(l))
#define HIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#define LOBYTE(w)           ((BYTE)(w))
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))

#endif /* !_KERNEL_TYPES_H_ */
