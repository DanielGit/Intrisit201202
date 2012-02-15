
#ifndef GUI_WIDGETS_H
#define GUI_WIDGETS_H

#include <stdio.h>
#include <stdlib.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "../config.h"
#include "../osdep/shmem.h"
#include "play.h"
#include "mplayer.h"
#include "interface.h"
#include "wm/ws.h"

#define GTK_MB_SIMPLE 0
#define GTK_MB_MODAL 1
#define GTK_MB_FATAL 2
#define GTK_MB_ERROR 4
#define GTK_MB_WARNING 8

extern GtkWidget     * PlayList;
extern GtkWidget     * Options;
extern GtkWidget     * PopUpMenu;

extern GtkWidget     * WarningPixmap;
extern GtkWidget     * ErrorPixmap;

extern GtkWidget     * SkinList;
extern GtkWidget     * gtkMessageBoxText;

extern int             gtkPopupMenu;
extern int             gtkPopupMenuParam;

extern char          * sbMPlayerDirInHome;
extern char          * sbMPlayerPrefixDir;
extern char          * sbMPlayerDirInHome_obsolete;
extern char          * sbMPlayerPrefixDir_obsolete;

extern GdkPixmap * gtkIcon;
extern GdkBitmap * gtkIconMask;
extern Pixmap      guiIcon;
extern Pixmap	   guiIconMask;

extern void widgetsCreate( void );

extern void gtkInit( void );
extern void gtkAddIcon( GtkWidget * window );

extern int  gtkFillSkinList( gchar * dir );
extern void gtkClearList( GtkWidget * list );
extern void gtkSetDefaultToCList( GtkWidget * list,char * item );
extern int  gtkFindCList( GtkWidget * list,char * item );

extern void gtkEventHandling( void );

extern void gtkShow( int type,char * param );
extern void gtkMessageBox( int type,const gchar * str );
extern void gtkSetLayer( GtkWidget * wdg );
extern void gtkActive( GtkWidget * wdg );

#endif
