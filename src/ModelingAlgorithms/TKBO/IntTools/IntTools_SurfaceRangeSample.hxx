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

#ifndef _IntTools_SurfaceRangeSample_HeaderFile
#define _IntTools_SurfaceRangeSample_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <IntTools_CurveRangeSample.hxx>
#include <Standard_Integer.hxx>
class IntTools_Range;

//! class for range index management of surface
class IntTools_SurfaceRangeSample
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT IntTools_SurfaceRangeSample();

  Standard_EXPORT IntTools_SurfaceRangeSample(const int theIndexU,
                                              const int theDepthU,
                                              const int theIndexV,
                                              const int theDepthV);

  Standard_EXPORT IntTools_SurfaceRangeSample(const IntTools_CurveRangeSample& theRangeU,
                                              const IntTools_CurveRangeSample& theRangeV);

  Standard_EXPORT IntTools_SurfaceRangeSample(const IntTools_SurfaceRangeSample& Other);

  Standard_EXPORT IntTools_SurfaceRangeSample& Assign(const IntTools_SurfaceRangeSample& Other);

  IntTools_SurfaceRangeSample& operator=(const IntTools_SurfaceRangeSample& Other)
  {
    return Assign(Other);
  }

  void SetRanges(const IntTools_CurveRangeSample& theRangeU,
                 const IntTools_CurveRangeSample& theRangeV);

  void GetRanges(IntTools_CurveRangeSample& theRangeU, IntTools_CurveRangeSample& theRangeV) const;

  void SetIndexes(const int theIndexU, const int theIndexV);

  void GetIndexes(int& theIndexU, int& theIndexV) const;

  void GetDepths(int& theDepthU, int& theDepthV) const;

  void SetSampleRangeU(const IntTools_CurveRangeSample& theRangeSampleU);

  const IntTools_CurveRangeSample& GetSampleRangeU() const;

  void SetSampleRangeV(const IntTools_CurveRangeSample& theRangeSampleV);

  const IntTools_CurveRangeSample& GetSampleRangeV() const;

  void SetIndexU(const int theIndexU);

  int GetIndexU() const;

  void SetIndexV(const int theIndexV);

  int GetIndexV() const;

  void SetDepthU(const int theDepthU);

  int GetDepthU() const;

  void SetDepthV(const int theDepthV);

  int GetDepthV() const;

  Standard_EXPORT IntTools_Range GetRangeU(const double theFirstU,
                                           const double theLastU,
                                           const int    theNbSampleU) const;

  Standard_EXPORT IntTools_Range GetRangeV(const double theFirstV,
                                           const double theLastV,
                                           const int    theNbSampleV) const;

  bool IsEqual(const IntTools_SurfaceRangeSample& Other) const;

  int GetRangeIndexUDeeper(const int theNbSampleU) const;

  int GetRangeIndexVDeeper(const int theNbSampleV) const;

  bool operator==(const IntTools_SurfaceRangeSample& theOther) const { return IsEqual(theOther); }

private:
  IntTools_CurveRangeSample myRangeU;
  IntTools_CurveRangeSample myRangeV;
};

#include <IntTools_SurfaceRangeSample.lxx>

#endif // _IntTools_SurfaceRangeSample_HeaderFile
