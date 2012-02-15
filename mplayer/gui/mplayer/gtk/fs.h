#ifndef GUI_FS_H
#define GUI_FS_H

#include <gtk/gtk.h>

#define fsVideoSelector    0
#define fsSubtitleSelector 1
#define fsOtherSelector    2
#define fsAudioSelector    3
#define fsFontSelector     4

#include <errno.h>

extern GtkWidget   * fsFileSelect;

extern void HideFileSelect( void );
extern void ShowFileSelect( int type, int modal );

extern GtkWidget * create_FileSelect( void );

#endif
