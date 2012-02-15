
#ifndef _WCHAR_H
#define _WCHAR_H

#include <sys/cdefs.h>
#include <sys/types.h>

wchar_t *_wcsupr(wchar_t *x);
size_t wcsxfrm(wchar_t *dst,const wchar_t *src, size_t n);
int _wtoi( const wchar_t *str );
long _wtol( const wchar_t *str );
long wcstol(const wchar_t *cp,wchar_t **endp,int base);
unsigned long wcstoul(const wchar_t *cp,wchar_t **endp,int base);
double wcstod(const wchar_t *s, wchar_t **sret);
wchar_t *wcstok(wchar_t *s, const wchar_t *ct);
wchar_t * _wcsrev(wchar_t *s) ;
wchar_t* _wcsnset (wchar_t* wsToFill, wchar_t wcFill, size_t sizeMaxFill);
wchar_t* _wcsset (wchar_t* wsToFill, wchar_t wcFill);
size_t wcsspn(const wchar_t *str,const wchar_t *accept);
wchar_t *wcsstr(const wchar_t *s,const wchar_t *b);
wchar_t * _wcslwr(wchar_t *x);
wchar_t *wcsncat(wchar_t *dst, const wchar_t *src, size_t n);
int wcsncmp(const wchar_t * cs,const wchar_t * ct,size_t count);
wchar_t * wcsncpy(wchar_t * dest,const wchar_t *src,size_t count);
int _wcsnicmp (const wchar_t *cs, const wchar_t *ct, size_t count);
wchar_t * wcscat(wchar_t * dest,const wchar_t * src);
wchar_t *wcspbrk(const wchar_t *s1, const wchar_t *s2);
wchar_t* wcsrchr(const wchar_t* str, wchar_t ch);
int wcscoll(const wchar_t *a1,const wchar_t *a2);
int _wcsicoll(const wchar_t *a1,const wchar_t *a2);
wchar_t * wcscpy(wchar_t * str1,const wchar_t * str2);
size_t wcscspn(const wchar_t *str,const wchar_t *reject);
int _wcsicmp(const wchar_t* cs,const wchar_t * ct);
size_t wcslen(const wchar_t * s);
size_t wstrlen(const wchar_t *s);
size_t _wcsnlen(const wchar_t * s, size_t count);
wchar_t* wcschr(const wchar_t* str, wchar_t ch);
int wcscmp(const wchar_t* cs,const wchar_t * ct);

int iswalnum(int c);
int iswalpha(int c);
int iswblank(int c);
int iswcntrl(int c);
int iswdigit(int c);
int iswgraph(int c);
int iswlower(int c);
int iswprint(int c);
int iswpunct(int c);
int iswspace(int c);
int iswupper(int c);
int iswxdigit(int c);
int towlower(int c);
int towupper(int c);


#endif  /* _MAC */
