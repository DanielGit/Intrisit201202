
#ifndef GUI_PLAY_H
#define GUI_PLAY_H

#include "../config.h"

extern int mplGotoTheNext;

extern void mplEnd( void );
extern void mplFullScreen( void );
extern void mplPlay( void );
extern void mplPause( void );
extern void mplState( void );
extern void mplPrev( void );
extern void mplNext( void );
extern void mplCurr( void );

extern void mplIncAudioBufDelay( void );
extern void mplDecAudioBufDelay( void );

extern void  mplRelSeek( float s );
extern void  mplAbsSeek( float s );
extern float mplGetPosition( void );

extern void mplPlayFork( void );
extern void mplSigHandler( int s );
extern void mplPlayerThread( void );

extern void ChangeSkin( char * name );
extern void EventHandling( void );

extern void mplSetFileName( char * dir,char * name,int type );

#endif
