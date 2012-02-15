
#define NULL ((void *)0)

unsigned int __errno_location = 0;

fputs (char *cptr)
{
  return strlen (cptr);
}


#define _U      01
#define _L      02
#define _N      04
#define _S      010
#define _P      020
#define _C      040
#define _X      0100
#define _B      0200

char _ctype_[1 + 256] = {
        0,
        _C,     _C,     _C,     _C,     _C,     _C,     _C,     _C,
        _C,     _C|_S,  _C|_S,  _C|_S,  _C|_S,  _C|_S,  _C,     _C,
        _C,     _C,     _C,     _C,     _C,     _C,     _C,     _C,
        _C,     _C,     _C,     _C,     _C,     _C,     _C,     _C,
        _S|_B,  _P,     _P,     _P,     _P,     _P,     _P,     _P,
        _P,     _P,     _P,     _P,     _P,     _P,     _P,     _P,
        _N,     _N,     _N,     _N,     _N,     _N,     _N,     _N,
        _N,     _N,     _P,     _P,     _P,     _P,     _P,     _P,
        _P,     _U|_X,  _U|_X,  _U|_X,  _U|_X,  _U|_X,  _U|_X,  _U,
        _U,     _U,     _U,     _U,     _U,     _U,     _U,     _U,
        _U,     _U,     _U,     _U,     _U,     _U,     _U,     _U,
        _U,     _U,     _U,     _P,     _P,     _P,     _P,     _P,
        _P,     _L|_X,  _L|_X,  _L|_X,  _L|_X,  _L|_X,  _L|_X,  _L,
        _L,     _L,     _L,     _L,     _L,     _L,     _L,     _L,
        _L,     _L,     _L,     _L,     _L,     _L,     _L,     _L,
        _L,     _L,     _L,     _P,     _P,     _P,     _P,     _C
};

int
isupper(int c)
{
        return((_ctype_ + 1)[c] & _U);
}

int
tolower(int c)
{
        return isupper(c) ? (c) - 'A' + 'a' : c;
}

int strncasecmp (char *s1, char *s2, int n)
{
  if (n == 0)
    return 0;

  while (n-- != 0 && tolower(*s1) == tolower(*s2))
    {
      if (n == 0 || *s1 == '\0' || *s2 == '\0')
        break;
      s1++;
      s2++;
    }

  return tolower(*(unsigned char *) s1) - tolower(*(unsigned char *) s2);
}

void *memchr(void *src_void, int c, int length)
{
  unsigned char *src = (unsigned char *) src_void;

  c &= 0xff;

  while (length--)
    {
      if (*src == c)
        return (char *) src;
      src++;
    }
  return NULL;
}

long labs(long x)
{
  if (x < 0)
    {
      x = -x;
    }
  return x;
}
 
