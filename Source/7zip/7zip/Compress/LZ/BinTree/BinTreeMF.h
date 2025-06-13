/*
 * BinTreeMF.h
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
 */

// #ifndef __BINTREEMF_H
// #define __BINTREEMF_H

#include "../../../ICoder.h"
#include "BinTree.h"

namespace BT_NAMESPACE {

#undef kIDByte
#undef kIDString

#ifdef HASH_ARRAY_2
  #ifdef HASH_ARRAY_3
    #ifdef HASH_BIG
      #define kIDByte 0x4
      #define kIDString TEXT("4b")
    #else
      #define kIDByte 0x3
      #define kIDString TEXT("4")
    #endif
  #else
    #define kIDByte 0x2
    #define kIDString TEXT("3")
  #endif
#else
  #ifdef HASH_ZIP
    #define kIDByte 0x0
    #define kIDString TEXT("3Z")
  #else
    #define kIDByte 0x1
    #define kIDString TEXT("2")
  #endif
#endif

#undef kIDUse3BytesByte
#undef kIDUse3BytesString

#define kIDUse3BytesByte 0x00
#define kIDUse3BytesString TEXT("")

// #undef kIDStringFull

// #define kIDStringFull TEXT("Compress.MatchFinderBT") kIDString kIDUse3BytesString

// {23170F69-40C1-278C-02XX-0000000000}
DEFINE_GUID(BT_CLSID, 
0x23170F69, 0x40C1, 0x278C, 0x02, kIDByte | kIDUse3BytesByte, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

class CInTree2: public CInTree
{
  CMyComPtr<IMatchFinderCallback> _callback;
  virtual void BeforeMoveBlock();
  virtual void AfterMoveBlock();
public:
  void SetCallback(IMatchFinderCallback *callback)
  {
    _callback = callback;
  }
};

class CMatchFinderBinTree: 
  public IMatchFinder,
  public IMatchFinderSetCallback,
  public CMyUnknownImp
{
  MY_UNKNOWN_IMP1(IMatchFinderSetCallback)

  STDMETHOD(Init)(ISequentialInStream *stream);
  STDMETHOD_(void, ReleaseStream)();
  STDMETHOD(MovePos)();
  STDMETHOD_(Byte, GetIndexByte)(Int32 index);
  STDMETHOD_(UInt32, GetMatchLen)(Int32 index, UInt32 back, UInt32 limit);
  STDMETHOD_(UInt32, GetNumAvailableBytes)();
  STDMETHOD_(const Byte *, GetPointerToCurrentPos)();
  STDMETHOD(Create)(UInt32 sizeHistory, 
      UInt32 keepAddBufferBefore, UInt32 matchMaxLen, 
      UInt32 keepAddBufferAfter);
  STDMETHOD_(UInt32, GetLongestMatch)(UInt32 *distances);
  STDMETHOD_(void, DummyLongestMatch)();

  // IMatchFinderSetCallback
  STDMETHOD(SetCallback)(IMatchFinderCallback *callback);

private:
  // UInt32 m_WindowReservSize;
  CInTree2 _matchFinder;
public:
  // CMatchFinderBinTree(): m_WindowReservSize((1 << 19) + 256) {};
  void SetCutValue(UInt32 cutValue) 
    { _matchFinder.SetCutValue(cutValue); }
  /*
  void SetWindowReservSize(UInt32 reservWindowSize)
    { m_WindowReservSize = reservWindowSize; }
  */
  virtual ~CMatchFinderBinTree() {}
};
 
}

// #endif

