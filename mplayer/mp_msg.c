
//#define MSG_USE_COLORS

#ifdef __LINUX__
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#else
#include <mplaylib.h>
#include <mplaylib.h>
#include <stdarg.h>
#include <mplaylib.h>
#endif

#include "config.h"

#ifdef USE_ICONV
#include <iconv.h>
#include <errno.h>
extern char* get_term_charset(void);
#endif

#if defined(FOR_MENCODER)
#undef HAVE_NEW_GUI
int use_gui;
#endif

#ifdef HAVE_NEW_GUI
#include "gui/interface.h"
#endif
#include "mp_msg.h"

/* maximum message length of mp_msg */
#define MSGSIZE_MAX 3072

int mp_msg_levels[MSGT_MAX]; // verbose level of this module. inited to -2
int mp_msg_level_all = MSGL_STATUS;
int verbose = 0;
#ifdef USE_ICONV
char *mp_msg_charset = NULL;
static char *old_charset = NULL;
static iconv_t msgiconv;
#endif

const char* filename_recode(const char* filename)
{
#ifndef __LINUX__
	return filename;
#else
#if !defined(USE_ICONV) || !defined(MSG_CHARSET)
    return filename;
#else
    static iconv_t inv_msgiconv = (iconv_t)(-1);
    static char recoded_filename[MSGSIZE_MAX];
    size_t filename_len, max_path;
    char* precoded;
    if (!mp_msg_charset ||
        !strcasecmp(mp_msg_charset, MSG_CHARSET) ||
        !strcasecmp(mp_msg_charset, "noconv"))
        return filename;
    if (inv_msgiconv == (iconv_t)(-1)) {
        inv_msgiconv = iconv_open(MSG_CHARSET, mp_msg_charset);
        if (inv_msgiconv == (iconv_t)(-1))
            return filename;
    }
    filename_len = strlen(filename);
    max_path = MSGSIZE_MAX - 4;
    precoded = recoded_filename;
    if (iconv(inv_msgiconv, &filename, &filename_len,
              &precoded, &max_path) == (size_t)(-1) && errno == E2BIG) {
        precoded[0] = precoded[1] = precoded[2] = '.';
        precoded += 3;
    }
    *precoded = '\0';
    return recoded_filename;
#endif
#endif
}

void mp_msg_init(void){
    
    int i;
#ifdef __LINUX__
    char *env = getenv("MPLAYER_VERBOSE");
    if (env)
        verbose = atoi(env);
#endif    
    for(i=0;i<MSGT_MAX;i++) mp_msg_levels[i] = -2;
    mp_msg_levels[MSGT_IDENTIFY] = -1; // no -identify output by default
#ifdef USE_ICONV
    mp_msg_charset = getenv("MPLAYER_CHARSET");
    if (!mp_msg_charset)
      mp_msg_charset = get_term_charset();
#endif
}

int mp_msg_test(int mod, int lev)
{
    return lev <= (mp_msg_levels[mod] == -2 ? mp_msg_level_all + verbose : mp_msg_levels[mod]);
}

extern void noah_kprintf(const char *format, ... );
void mp_msg(int mod, int lev, const char *format, ... ){
	va_list va;
	char tmp[MSGSIZE_MAX];
	
	if (!mp_msg_test(mod, lev)) 
		return; // do not display
	va_start(va, format);
	vsprintf(tmp, format, va);
	va_end(va);
	tmp[MSGSIZE_MAX-2] = '\n';
	tmp[MSGSIZE_MAX-1] = 0;
	
	noah_kprintf("%s", tmp);fflush(stdout);
}

