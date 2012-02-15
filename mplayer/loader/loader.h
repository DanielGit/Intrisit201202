/********************************************************

	Win32 binary loader interface
	Copyright 2000 Eugene Kuznetsov (divx@euro.ru)
	Shamelessly stolen from Wine project

*********************************************************/

/*
 * Modified for use with MPlayer, detailed changelog at
 * http://svn.mplayerhq.hu/mplayer/trunk/
 * $Id: loader.h,v 1.1.1.1 2007-12-14 02:15:34 zpxu Exp $
 */

#ifndef LOADER_H
#define LOADER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "wine/windef.h"
#include "wine/driver.h"
#include "wine/mmreg.h"
#include "wine/vfw.h"
#include "wine/msacm.h"

unsigned int _GetPrivateProfileIntA(const char* appname, const char* keyname, int default_value, const char* filename);
int _GetPrivateProfileStringA(const char* appname, const char* keyname,
	const char* def_val, char* dest, unsigned int len, const char* filename);
int _WritePrivateProfileStringA(const char* appname, const char* keyname,
	const char* string, const char* filename);

INT WINAPI LoadStringA( HINSTANCE instance, UINT resource_id,
                            LPSTR buffer, INT buflen );

#ifdef __cplusplus
}
#endif

#endif /* LOADER_H */
