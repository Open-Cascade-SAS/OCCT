// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _math_SingleTab_HeaderFile
#define _math_SingleTab_HeaderFile

#include <Standard_OutOfRange.hxx>
#include <Standard_Failure.hxx>

#include <string.h>

static const Standard_Integer aLengthOfBuf = 512;

template<class T> class math_SingleTab
{
public:

  DEFINE_STANDARD_ALLOC

  math_SingleTab(const Standard_Integer LowerIndex, const Standard_Integer UpperIndex) :
      Addr(Buf),
      isAllocated(UpperIndex - LowerIndex + 1 > aLengthOfBuf),
      First(LowerIndex), Last(UpperIndex)
  {
    T* TheAddr = !isAllocated? Buf :
      (T*) Standard::Allocate((Last-First+1) * sizeof(T));
    Addr = (Standard_Address) (TheAddr - First);
  }

  math_SingleTab(const Standard_Address Tab, const Standard_Integer LowerIndex, const Standard_Integer UpperIndex) :
      Addr((void*)((const T*)Tab - LowerIndex)), 
      isAllocated(Standard_False),
      First(LowerIndex), Last(UpperIndex)
  {
  }

  void Init(const T InitValue)
  {
    for(Standard_Integer i = First; i<= Last; i++)
    {
      ((T*)Addr)[i] = InitValue;
    }
  }

  math_SingleTab(const math_SingleTab& Other) :
      isAllocated(Other.Last - Other.First + 1 > aLengthOfBuf),
      First(Other.First),
      Last(Other.Last)
  {
    T* TheAddr = !isAllocated? Buf : (T*) Standard::Allocate((Last-First+1) * sizeof(T));
    Addr = (Standard_Address) (TheAddr - First);
    T* TheOtherAddr = (T*) Other.Addr;
    memmove((void*) TheAddr, (const void*) (TheOtherAddr + First), (size_t)(Last - First + 1) * sizeof(T));
  }

  inline void Copy(math_SingleTab& Other) const
  {
    memmove((void*) (((T*)Other.Addr) + Other.First),
      (const void*) (((T*)Addr) + First),
      (size_t)(Last - First + 1) * sizeof(T));
  }

  void SetLower(const Standard_Integer LowerIndex)
  {
    T* TheAddr = (T*) Addr;
    Addr = (Standard_Address) (TheAddr + First - LowerIndex);
    Last = Last - First + LowerIndex;
    First = LowerIndex;
  }

  inline T& Value(const Standard_Integer Index) const
  {
    return ((T*)Addr)[Index];
  }

  T& operator()(const Standard_Integer Index) const
  {
    return Value(Index);
  }
  
  void Free()
  {
    if(isAllocated)
    {
      Standard_Address it = (Standard_Address)&((T*)Addr)[First];
      Standard::Free(it);
      Addr = 0;
    }
  }

  ~math_SingleTab()
  {
    Free();
  }

private:
  
  Standard_Address Addr;
  T Buf[aLengthOfBuf];
  Standard_Boolean isAllocated;
  Standard_Integer First;
  Standard_Integer Last;
};

#endif
