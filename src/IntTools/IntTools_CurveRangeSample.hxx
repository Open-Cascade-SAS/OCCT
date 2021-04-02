// Created on: 2005-10-05
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

#ifndef _IntTools_CurveRangeSample_HeaderFile
#define _IntTools_CurveRangeSample_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <IntTools_BaseRangeSample.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>

class IntTools_Range;

//! class for range index management of curve
class IntTools_CurveRangeSample  : public IntTools_BaseRangeSample
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT IntTools_CurveRangeSample();
  
  Standard_EXPORT IntTools_CurveRangeSample(const Standard_Integer theIndex);
  
  void SetRangeIndex (const Standard_Integer theIndex) { myIndex = theIndex; }

  Standard_Integer GetRangeIndex() const { return myIndex; }

  Standard_Boolean IsEqual (const IntTools_CurveRangeSample& Other) const
  {
    return ((myIndex == Other.myIndex) && (GetDepth() == Other.GetDepth()));
  }

  Standard_EXPORT IntTools_Range GetRange (const Standard_Real theFirst, const Standard_Real theLast, const Standard_Integer theNbSample) const;
  
  Standard_Integer GetRangeIndexDeeper (const Standard_Integer theNbSample) const
  {
    return myIndex * theNbSample;
  }

private:

  Standard_Integer myIndex;

};

#endif // _IntTools_CurveRangeSample_HeaderFile
