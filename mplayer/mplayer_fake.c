#include    <stdio.h>
#include    <stdarg.h>
#include    <smcvals.h>

extern void noah_kprintf(const char *format, ... );

// ================

void Buffer_Init ()
{
	return;
}

void pcm_write ()
{
	return;
}

void pcm_fill_buff ()
{
	return;
}

void pcm_set_write_pagesize ()
{
	return;
}

void snprintf (char *buf, int n, const char *format, ...)
{
	char *tmp;
	va_list ap;
  
	if(!n)
		return;
	if(n < 64)
		n = 64;
	tmp = (char*)malloc(n*4);
	if(tmp)
	{
 		va_start (ap, format);
		sprintf(tmp, format, ap);
		strncpy(buf, tmp, n);
		free(tmp);
	}
	else
		strcpy(buf, "");
}
void vsnprintf (char *buf, int n, const char *format, ...)
{
	char *tmp;
	va_list ap;
  
	if(!n)
		return;
	if(n < 64)
		n = 64;
	tmp = (char*)malloc(n*4);
	if(tmp)
	{
 		va_start (ap, format);
		sprintf(tmp, format, ap);
		strncpy(buf, tmp, n);
		free(tmp);
	}
	else
		strcpy(buf, "");
}


void stat(const char *path, struct stat *buf)
{
  kprintf ("+++++++++ calling stat +++++++++\n");
  kpanic("stat()\n");
}


void __assert (const char *file, int line, const char *msg)
{
  kprintf ("ASSERT: %s(line: %d), %s\n", file, line, msg);
  kpanic("__assert()\n");
}

void __assert_fail ()
{
  kprintf ("+++++++++ calling ___assert_fail +++++++++\n");
  kpanic("__assert_fail()\n");
  return;
}

