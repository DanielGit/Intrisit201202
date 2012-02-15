/*
 * Modified for use with MPlayer, detailed changelog at
 * http://svn.mplayerhq.hu/mplayer/trunk/
 * $Id: driver.h,v 1.1.1.1 2007-12-14 02:15:34 zpxu Exp $
 */

#ifndef LOADER_DRIVER_H
#define LOADER_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "wine/windef.h"
#include "wine/driver.h"

void SetCodecPath(const char* path);
void CodecAlloc(void);
void CodecRelease(void);

HDRVR DrvOpen(LPARAM lParam2);
void DrvClose(HDRVR hdrvr);

#ifdef __cplusplus
}
#endif

#endif /* LOADER_DRIVER_H */
