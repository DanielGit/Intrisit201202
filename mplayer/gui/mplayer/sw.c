
// sub window

#include <inttypes.h>
#include <string.h>

#include "../config.h"
#include "../libvo/x11_common.h"

#include "gmplayer.h"
#include "app.h"
#include "interface.h"
#include "../help_mp.h"
#include "widgets.h"

int             mplSubRender = 0;
int             SubVisible = 0;

extern int             boxMoved;
extern int             sx,sy;
extern int             i,pot;

void mplSubDraw( void )
{
 if ( appMPlayer.subWindow.State == wsWindowClosed ) exit_player( MSGTR_Exit_quit );
 
 if ( appMPlayer.subWindow.State == wsWindowFocusIn ) SubVisible++;
 if ( appMPlayer.subWindow.State == wsWindowFocusOut && metacity_hack != 3 ) SubVisible--;

 if ( !appMPlayer.subWindow.Mapped ||
      appMPlayer.subWindow.Visible == wsWindowNotVisible ) return;

 if ( guiIntfStruct.Playing ) mplSubRender=0;

 if ( mplSubRender && appMPlayer.subWindow.State == wsWindowExpose )
  {
   if ( appMPlayer.sub.Bitmap.Image ) wsPutImage( &appMPlayer.subWindow );
  }
 appMPlayer.subWindow.State=0;
}

void mplSubMouseHandle( int Button,int X,int Y,int RX,int RY )
{
 static int mplSubMoved = 0;
 static int msButton = 0;

 mplPBShow( X,Y );

 switch( Button )
  {
   case wsRRMouseButton:
          gtkShow( evShowPopUpMenu,NULL );
          break;
   case wsPMMouseButton:
          gtkShow( evHidePopUpMenu,NULL );
          mplShowMenu( RX,RY );
          msButton=wsPMMouseButton;
          break;
   case wsRMMouseButton:
          mplHideMenu( RX,RY,1 );
          msButton=0;
          break;
// ---
   case wsPLMouseButton:
          gtkShow( evHidePopUpMenu,NULL );
          sx=X; sy=Y;
          msButton=wsPLMouseButton;
          mplSubMoved=0;
          break;
   case wsMoveMouse:
          switch ( msButton )
           {
            case wsPLMouseButton:
                   mplSubMoved=1;
                   if ( !appMPlayer.subWindow.isFullScreen ) wsMoveWindow( &appMPlayer.subWindow,False,RX - sx,RY - sy );
                   break;
            case wsPMMouseButton:
                   mplMenuMouseHandle( X,Y,RX,RY );
                   break;
	    default: mplPBShow( X,Y ); break;
           }
          break;
   case wsRLMouseButton:
          if ( ( !mplSubMoved )&&( appMPlayer.subWindow.isFullScreen ) )
           {
            if( SubVisible++%2 ) wsMoveTopWindow( wsDisplay,appMPlayer.mainWindow.WindowID );
             else wsMoveTopWindow( wsDisplay,appMPlayer.subWindow.WindowID );
	   }
          msButton=0;
          mplSubMoved=0;
          break;
  }
}
