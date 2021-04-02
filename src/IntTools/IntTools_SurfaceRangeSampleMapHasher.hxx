// Created on: 2005-10-14
// Created by: Mikhail KLOKOV
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#ifndef _IntTools_SurfaceRangeSampleMapHasher_HeaderFile
#define _IntTools_SurfaceRangeSampleMapHasher_HeaderFile

#include <IntTools_SurfaceRangeSample.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Integer.hxx>

class IntTools_SurfaceRangeSampleMapHasher 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Computes a hash code for the given key, in the range [1, theUpperBound]
  //! @param theKey the key which hash code is to be computed
  //! @param theUpperBound the upper bound of the range a computing hash code must be within
  //! @return a computed hash code, in the range [1, theUpperBound]
  static Standard_Integer HashCode (const IntTools_SurfaceRangeSample& theKey, Standard_Integer theUpperBound)
  {
    return ::HashCode (theKey.GetIndexU() * theKey.GetIndexV(), theUpperBound);
  }

  //! Returns True  when the two  keys are the same. Two
  //! same  keys  must   have  the  same  hashcode,  the
  //! contrary is not necessary.
  static Standard_Boolean IsEqual (const IntTools_SurfaceRangeSample& S1, const IntTools_SurfaceRangeSample& S2)
  {
    return S1.IsEqual(S2);
  }

};

#endif // _IntTools_SurfaceRangeSampleMapHasher_HeaderFile
