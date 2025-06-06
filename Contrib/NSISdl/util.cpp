/*
** JNetLib
** Copyright (C) 2000-2001 Nullsoft, Inc.
** Author: Justin Frankel
** File: util.cpp - JNL implementation of basic network utilities
** License: see License.txt
**
** Unicode support by Jim Park -- 08/24/2007
**   Keep everything here strictly ANSI.  No TCHAR style stuff.
*/

#include <windows.h>
#include "util.h"

int my_atoi(char *s)
{
  int sign=0;
  int v=0;
  if (*s == '-') { s++; sign++; }
  for (;;)
  {
    int c=*s++ - '0';
    if (c < 0 || c > 9) break;
    v*=10;
    v+=c;
  }
  if (sign) return -(int) v;
  return (int)v;
}

__int64 myatoi64(const char *s)
{
  __int64 v=0;
  int sign=0;

  if (*s == '-')
    sign++;
  else
    s--;

  for (;;)
  {
    int c=*(++s) - '0';
    if (c < 0 || c > 9) break;
    v*=10;
    v+=c;
  }

  if (sign)
    v = -v;

  return v;
}

void myitoa64(__int64 i, char *buffer)
{
    char buf[128], *b = buf;

    if (i < 0)
    {
        *(buffer++) = '-';
        i = -i;
    }
    if (i == 0) *(buffer++) = '0';
    else 
    {
        while (i > 0) 
        {
            *(b++) = '0' + ((char) (i%10));
            i /= 10;
        }
        while (b > buf) *(buffer++) = *(--b);
    }
    *buffer = 0;
}

// Visual Studio 2015 (CLv19 x86) and some older versions of CLv14 x64 will optimize
// our loop into a direct call to _memset and this fails to link because we don't use the CRT
#if defined(_MSC_VER) && _MSC_VER+0 >= 1400
#  if defined(_MSC_FULL_VER) && _MSC_FULL_VER+0 >= 140050727
#    include <intrin.h>
#  else
     EXTERN_C void __stosb(BYTE*,BYTE,size_t);
#  endif //~ _MSC_FULL_VER >= 140050727
#  if defined(_M_IX86) || defined(_M_AMD64) || defined(_M_X64) // __stosb not available under _M_ARM nor _M_ARM64
#    pragma intrinsic(__stosb)
#    define CRTINTRINSIC_memset(p,c,s) __stosb((BYTE*)(p),(BYTE)(c),(s))
#  elif defined(_M_ARM) || defined(_M_ARM64) // For _MSC_VER=1914 (VS 15.7.27703.2026/CL 19.14.26430)
     EXTERN_C void* __cdecl memset(void *d, int v, size_t l) { char *p=(char*)d; while (l-- > 0) *p++=v; return d; }
#    pragma function(memset)
#  endif
#endif //~ _MSC_VER

void mini_memset(void *o,char i,int l)
{
#ifdef CRTINTRINSIC_memset
  CRTINTRINSIC_memset(o, i, l);
#else
  char *oo=(char*)o;
  while (l-- > 0) *oo++=i;
#endif
}

void mini_memcpy(void *o,void*i,int l)
{
  char *oo=(char*)o;
  char *ii=(char*)i;
  while (l-- > 0) *oo++=*ii++;
}

#include <stddef.h>

#ifdef __clang__
#pragma function(memset)
#pragma function(memcpy)
#endif

extern "C"
void * __cdecl memset(void *dst, int val, size_t len)     /* ① 回傳 void* ② 第 3 參數 size_t */
{
#if defined(_M_X64) || defined(_M_IX86)
    #pragma intrinsic(__stosb)
    __stosb(static_cast<unsigned char*>(dst),
            static_cast<unsigned char>(val), len);
#else
    unsigned char *p = static_cast<unsigned char*>(dst);
    while (len--) *p++ = static_cast<unsigned char>(val);
#endif
    return dst;
}

extern "C"
void * __cdecl memcpy(void *dst, const void *src, size_t len)   /* 同樣保持正式原型 */
{
#if defined(_M_X64) || defined(_M_IX86)
    #pragma intrinsic(__movsb)
    __movsb(static_cast<unsigned char*>(dst),
            static_cast<const unsigned char*>(src), len);
#else
    unsigned char *d = static_cast<unsigned char*>(dst);
    const unsigned char *s = static_cast<const unsigned char*>(src);
    while (len--) *d++ = *s++;
#endif
    return dst;
}