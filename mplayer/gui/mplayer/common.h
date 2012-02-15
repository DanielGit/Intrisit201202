
#ifndef GUI_COMMON_H
#define GUI_COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <unistd.h>

#include "app.h"
#include "bitmap.h"
#include "wm/ws.h"

extern inline void TranslateFilename( int c,char * tmp );
extern char * Translate( char * str );
extern void PutImage( txSample * bf,int x,int y,int max,int ofs );
extern void SimplePotmeterPutImage( txSample * bf,int x,int y,float frac );
extern void Render( wsTWindow * window,wItem * Items,int nrItems,char * db,int size );

#endif
