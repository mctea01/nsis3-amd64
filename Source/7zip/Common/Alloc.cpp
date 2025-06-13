/*
 * Alloc.cpp
 * 
 * This file is a part of LZMA compression module for NSIS.
 * 
 * Original LZMA SDK Copyright (C) 1999-2006 Igor Pavlov
 * Modifications Copyright (C) 2003-2025 Amir Szekely <kichik@netvision.net.il>
 * 
 * Licensed under the Common Public License version 1.0 (the "License");
 * you may not use this file except in compliance with the License.
 * 
 * Licence details can be found in the file COPYING.
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty.
 *
 * Unicode support by Jim Park -- 08/28/2007
 */

#include "StdAfx.h"

#ifdef _WIN32
#include "MyWindows.h"
#else
#include <stdlib.h>
#include "../../tchar.h"
#endif

#include "Alloc.h"

/* #define _SZ_ALLOC_DEBUG */
/* use _SZ_ALLOC_DEBUG to debug alloc/free operations */
#ifdef _SZ_ALLOC_DEBUG
#include <stdio.h>
int g_allocCount = 0;
int g_allocCountMid = 0;
int g_allocCountBig = 0;
#endif

void *MyAlloc(size_t size) throw()
{
  if (size == 0)
    return 0;
  #ifdef _SZ_ALLOC_DEBUG
  _ftprintf(stderr, _T("\nAlloc %10d bytes; count = %10d"), size, g_allocCount++);
  #endif
  return ::malloc(size);
}

void MyFree(void *address) throw()
{
  #ifdef _SZ_ALLOC_DEBUG
  if (address != 0)
    _ftprintf(stderr, _T("\nFree; count = %10d"), --g_allocCount);
  #endif
  
  ::free(address);
}

#ifdef _WIN32

void *MidAlloc(size_t size) throw()
{
  if (size == 0)
    return 0;
  #ifdef _SZ_ALLOC_DEBUG
  _ftprintf(stderr, _T("\nAlloc_Mid %10d bytes;  count = %10d"), size, g_allocCountMid++);
  #endif
  return ::VirtualAlloc(0, size, MEM_COMMIT, PAGE_READWRITE);
}

void MidFree(void *address) throw()
{
  #ifdef _SZ_ALLOC_DEBUG
  if (address != 0)
    _ftprintf(stderr, _T("\nFree_Mid; count = %10d"), --g_allocCountMid);
  #endif
  if (address == 0)
    return;
  ::VirtualFree(address, 0, MEM_RELEASE);
}

static SIZE_T g_LargePageSize = 
    #ifdef _WIN64
    (1 << 21);
    #else
    (1 << 22);
    #endif

typedef SIZE_T (WINAPI *GetLargePageMinimumP)();

bool SetLargePageSize()
{
  GetLargePageMinimumP largePageMinimum = (GetLargePageMinimumP)
        ::GetProcAddress(::GetModuleHandle(_T("kernel32.dll")), "GetLargePageMinimum");
  if (largePageMinimum == 0)
    return false;
  SIZE_T size = largePageMinimum();
  if (size == 0 || (size & (size - 1)) != 0)
    return false;
  g_LargePageSize = size;
  return true;
}


void *BigAlloc(size_t size) throw()
{
  if (size == 0)
    return 0;
  #ifdef _SZ_ALLOC_DEBUG
  _ftprintf(stderr, _T("\nAlloc_Big %10d bytes;  count = %10d"), size, g_allocCountBig++);
  #endif
  
  if (size >= (1 << 18))
  {
    void *res = ::VirtualAlloc(0, (size + g_LargePageSize - 1) & (~(g_LargePageSize - 1)), 
        MEM_COMMIT | MEM_LARGE_PAGES, PAGE_READWRITE);
    if (res != 0)
      return res;
  }
  return ::VirtualAlloc(0, size, MEM_COMMIT, PAGE_READWRITE);
}

void BigFree(void *address) throw()
{
  #ifdef _SZ_ALLOC_DEBUG
  if (address != 0)
    _ftprintf(stderr, _T("\nFree_Big; count = %10d"), --g_allocCountBig);
  #endif
  
  if (address == 0)
    return;
  ::VirtualFree(address, 0, MEM_RELEASE);
}

#endif
