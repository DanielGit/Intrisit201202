/*
 * Modified for use with MPlayer, detailed changelog at
 * http://svn.mplayerhq.hu/mplayer/trunk/
 * $Id: win32.h,v 1.1.1.1 2007-12-14 02:15:34 zpxu Exp $
 */

#ifndef LOADER_WIN32_H
#define LOADER_WIN32_H

#include <time.h>

#include "wine/windef.h"
#include "wine/winbase.h"
#include "com.h"

#ifdef AVIFILE
#ifdef __GNUC__
#include "avm_output.h"
#ifndef __cplusplus
#define printf(a, ...)  avm_printf("Win32 plugin", a, ## __VA_ARGS__)
#endif
#endif
#endif

extern void my_garbagecollection(void);

typedef struct {
    UINT             uDriverSignature;
    HINSTANCE        hDriverModule;
    DRIVERPROC       DriverProc;
    DWORD            dwDriverID;
} DRVR;

typedef DRVR  *PDRVR;
typedef DRVR  *NPDRVR;
typedef DRVR  *LPDRVR;

typedef struct tls_s tls_t;


extern void* LookupExternal(const char* library, int ordinal);
extern void* LookupExternalByName(const char* library, const char* name);

#endif /* LOADER_WIN32_H */
