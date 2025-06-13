/*
 * InBuffer.h
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

#ifndef __INBUFFER_H
#define __INBUFFER_H

#include "../IStream.h"
#include "../../Common/MyCom.h"

#ifndef _NO_EXCEPTIONS
class CInBufferException
{
public:
  HRESULT ErrorCode;
  CInBufferException(HRESULT errorCode): ErrorCode(errorCode) {}
};
#endif

class CInBuffer
{
  Byte *_buffer;
  Byte *_bufferLimit;
  Byte *_bufferBase;
  CMyComPtr<ISequentialInStream> _stream;
  UInt64 _processedSize;
  UInt32 _bufferSize;
  bool _wasFinished;

  bool ReadBlock();
  Byte ReadBlock2();

public:
  #ifdef _NO_EXCEPTIONS
  HRESULT ErrorCode;
  #endif

  CInBuffer();
  ~CInBuffer() { Free(); }

  bool Create(UInt32 bufferSize);
  void Free();
  
  void SetStream(ISequentialInStream *stream);
  void Init();
  void ReleaseStream() { _stream.Release(); }

  bool ReadByte(Byte &b)
  {
    if(_buffer >= _bufferLimit)
      if(!ReadBlock())
        return false;
    b = *_buffer++;
    return true;
  }
  Byte ReadByte()
  {
    if(_buffer >= _bufferLimit)
      return ReadBlock2();
    return *_buffer++;
  }
  void ReadBytes(void *data, UInt32 size, UInt32 &processedSize)
  {
    for(processedSize = 0; processedSize < size; processedSize++)
      if (!ReadByte(((Byte *)data)[processedSize]))
        return;
  }
  bool ReadBytes(void *data, UInt32 size)
  {
    UInt32 processedSize;
    ReadBytes(data, size, processedSize);
    return (processedSize == size);
  }
  UInt64 GetProcessedSize() const { return _processedSize + (_buffer - _bufferBase); }
  bool WasFinished() const { return _wasFinished; }
};

#endif
