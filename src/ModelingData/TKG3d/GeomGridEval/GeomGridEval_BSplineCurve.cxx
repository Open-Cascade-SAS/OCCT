// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <GeomGridEval_BSplineCurve.hxx>

#include <BSplCLib.hxx>
#include <gp_Pnt.hxx>

//==================================================================================================

void GeomGridEval_BSplineCurve::SetParams(const TColStd_Array1OfReal& theParams)
{
  mySpanRanges = NCollection_Array1<SpanRange>(); // Clear cached span data
  myRawParams.Resize(theParams.Lower(), theParams.Upper(), false);
  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    myRawParams.SetValue(i, theParams.Value(i));
  }
}

//==================================================================================================

void GeomGridEval_BSplineCurve::prepare() const
{
  // Check if already prepared using span ranges emptiness
  if (!mySpanRanges.IsEmpty())
  {
    return;
  }

  if (myGeom.IsNull() || myRawParams.IsEmpty())
  {
    myParams = NCollection_Array1<ParamWithSpan>();
    return;
  }

  // Get flat knots directly from geometry
  const Handle(TColStd_HArray1OfReal)& aFlatKnotsHandle = myGeom->HArrayFlatKnots();
  if (aFlatKnotsHandle.IsNull())
  {
    myParams = NCollection_Array1<ParamWithSpan>();
    return;
  }
  const TColStd_Array1OfReal& aFlatKnots = aFlatKnotsHandle->Array1();

  const int aDegree   = myGeom->Degree();
  const int aNbParams = myRawParams.Size();
  myParams.Resize(0, aNbParams - 1, false);

  // Use hint-based span location for efficiency on sorted parameters
  int aPrevSpan = aFlatKnots.Lower() + aDegree;

  for (int i = myRawParams.Lower(); i <= myRawParams.Upper(); ++i)
  {
    const double aParam   = myRawParams.Value(i);
    const int    aSpanIdx = locateSpanWithHint(aParam, aPrevSpan, aFlatKnots);
    aPrevSpan             = aSpanIdx;

    // Pre-compute local parameter for this span
    const double aSpanStart  = aFlatKnots.Value(aSpanIdx);
    const double aSpanLength = aFlatKnots.Value(aSpanIdx + 1) - aSpanStart;
    const double aLocalParam = (aParam - aSpanStart) / aSpanLength;

    myParams.SetValue(i - myRawParams.Lower(), ParamWithSpan{aParam, aLocalParam, aSpanIdx});
  }

  // Compute span ranges for optimized iteration
  computeSpanRanges(myParams, aFlatKnots, mySpanRanges);
}

//==================================================================================================

int GeomGridEval_BSplineCurve::locateSpan(double                      theParam,
                                          const TColStd_Array1OfReal& theFlatKnots) const
{
  int    aSpanIndex = 0;
  double aNewParam  = theParam;
  BSplCLib::LocateParameter(myGeom->Degree(),
                            theFlatKnots,
                            BSplCLib::NoMults(),
                            theParam,
                            myGeom->IsPeriodic(),
                            aSpanIndex,
                            aNewParam);
  return aSpanIndex;
}

//==================================================================================================

int GeomGridEval_BSplineCurve::locateSpanWithHint(double                      theParam,
                                                  int                         theHint,
                                                  const TColStd_Array1OfReal& theFlatKnots) const
{
  const int aDegree = myGeom->Degree();
  const int aLower  = theFlatKnots.Lower() + aDegree;
  const int aUpper  = theFlatKnots.Upper() - aDegree - 1;

  // Quick check if hint is still valid
  if (theHint >= aLower && theHint <= aUpper)
  {
    const double aSpanStart = theFlatKnots.Value(theHint);
    const double aSpanEnd   = theFlatKnots.Value(theHint + 1);

    if (theParam >= aSpanStart && theParam < aSpanEnd)
    {
      return theHint;
    }

    // Check next span (common case for sorted parameters)
    if (theHint < aUpper && theParam >= aSpanEnd)
    {
      const double aNextEnd = theFlatKnots.Value(theHint + 2);
      if (theParam < aNextEnd)
      {
        return theHint + 1;
      }
    }
  }

  // Fall back to standard locate
  return locateSpan(theParam, theFlatKnots);
}

//==================================================================================================

void GeomGridEval_BSplineCurve::computeSpanRanges(
  const NCollection_Array1<ParamWithSpan>& theParams,
  const TColStd_Array1OfReal&              theFlatKnots,
  NCollection_Array1<SpanRange>&           theSpanRanges)
{
  if (theParams.IsEmpty())
  {
    theSpanRanges = NCollection_Array1<SpanRange>();
    return;
  }

  // Count distinct spans
  int aNbSpans     = 1;
  int aCurrentSpan = theParams.Value(0).SpanIndex;
  for (int i = 1; i < theParams.Size(); ++i)
  {
    if (theParams.Value(i).SpanIndex != aCurrentSpan)
    {
      ++aNbSpans;
      aCurrentSpan = theParams.Value(i).SpanIndex;
    }
  }

  theSpanRanges.Resize(0, aNbSpans - 1, false);

  // Fill span ranges
  aCurrentSpan    = theParams.Value(0).SpanIndex;
  int aRangeStart = 0;
  int aRangeIdx   = 0;

  for (int i = 1; i < theParams.Size(); ++i)
  {
    const int aSpan = theParams.Value(i).SpanIndex;
    if (aSpan != aCurrentSpan)
    {
      const double aSpanStart   = theFlatKnots.Value(aCurrentSpan);
      const double aSpanLength  = theFlatKnots.Value(aCurrentSpan + 1) - aSpanStart;
      const double aSpanMid     = aSpanStart + aSpanLength * 0.5;
      const double aSpanHalfLen = aSpanLength * 0.5;

      theSpanRanges.SetValue(aRangeIdx,
                             SpanRange{aCurrentSpan, aRangeStart, i, aSpanMid, aSpanHalfLen});
      ++aRangeIdx;
      aCurrentSpan = aSpan;
      aRangeStart  = i;
    }
  }

  // Add the last range
  const double aSpanStart   = theFlatKnots.Value(aCurrentSpan);
  const double aSpanLength  = theFlatKnots.Value(aCurrentSpan + 1) - aSpanStart;
  const double aSpanMid     = aSpanStart + aSpanLength * 0.5;
  const double aSpanHalfLen = aSpanLength * 0.5;

  theSpanRanges.SetValue(
    aRangeIdx,
    SpanRange{aCurrentSpan, aRangeStart, theParams.Size(), aSpanMid, aSpanHalfLen});
}

//==================================================================================================

NCollection_Array1<gp_Pnt> GeomGridEval_BSplineCurve::EvaluateGrid() const
{
  if (myGeom.IsNull() || myRawParams.IsEmpty())
  {
    return NCollection_Array1<gp_Pnt>();
  }

  // Prepare span data if needed
  prepare();

  if (myParams.IsEmpty() || mySpanRanges.IsEmpty())
  {
    return NCollection_Array1<gp_Pnt>();
  }

  const int                  aNbParams = myParams.Size();
  NCollection_Array1<gp_Pnt> aPoints(1, aNbParams);

  // Get flat knots directly from geometry
  const Handle(TColStd_HArray1OfReal)& aFlatKnotsHandle = myGeom->HArrayFlatKnots();
  if (aFlatKnotsHandle.IsNull())
  {
    return NCollection_Array1<gp_Pnt>();
  }
  const TColStd_Array1OfReal& aFlatKnots = aFlatKnotsHandle->Array1();

  // Get poles and weights handles directly from geometry
  const int                            aDegree    = myGeom->Degree();
  const Handle(TColgp_HArray1OfPnt)&   aPoles     = myGeom->HArrayPoles();
  const Handle(TColStd_HArray1OfReal)& aWeights   = myGeom->HArrayWeights();
  const bool                           isRational = myGeom->IsRational();

  // Create or update cache
  if (myCache.IsNull())
  {
    myCache = new BSplCLib_Cache(aDegree,
                                 myGeom->IsPeriodic(),
                                 aFlatKnots,
                                 aPoles->Array1(),
                                 isRational ? &aWeights->Array1() : nullptr);
  }

  // Iterate over pre-computed span ranges
  for (int iRange = 0; iRange < mySpanRanges.Size(); ++iRange)
  {
    const SpanRange& aRange = mySpanRanges.Value(iRange);

    // Rebuild cache once for this span block using first parameter in range
    myCache->BuildCache(myParams.Value(aRange.StartIdx).Param,
                        aFlatKnots,
                        aPoles->Array1(),
                        isRational ? &aWeights->Array1() : nullptr);

    // Evaluate all points in this span block using pre-computed local parameters
    for (int i = aRange.StartIdx; i < aRange.EndIdx; ++i)
    {
      myCache->D0Local(myParams.Value(i).LocalParam, aPoints.ChangeValue(i + 1));
    }
  }

  return aPoints;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD1> GeomGridEval_BSplineCurve::EvaluateGridD1() const
{
  if (myGeom.IsNull() || myRawParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD1>();
  }

  prepare();

  if (myParams.IsEmpty() || mySpanRanges.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD1>();
  }

  const int                                 aNbParams = myParams.Size();
  NCollection_Array1<GeomGridEval::CurveD1> aResults(1, aNbParams);

  const Handle(TColStd_HArray1OfReal)& aFlatKnotsHandle = myGeom->HArrayFlatKnots();
  if (aFlatKnotsHandle.IsNull())
  {
    return NCollection_Array1<GeomGridEval::CurveD1>();
  }
  const TColStd_Array1OfReal& aFlatKnots = aFlatKnotsHandle->Array1();

  // Get poles and weights handles directly from geometry
  const int                            aDegree    = myGeom->Degree();
  const Handle(TColgp_HArray1OfPnt)&   aPoles     = myGeom->HArrayPoles();
  const Handle(TColStd_HArray1OfReal)& aWeights   = myGeom->HArrayWeights();
  const bool                           isRational = myGeom->IsRational();

  if (myCache.IsNull())
  {
    myCache = new BSplCLib_Cache(aDegree,
                                 myGeom->IsPeriodic(),
                                 aFlatKnots,
                                 aPoles->Array1(),
                                 isRational ? &aWeights->Array1() : nullptr);
  }

  for (int iRange = 0; iRange < mySpanRanges.Size(); ++iRange)
  {
    const SpanRange& aRange = mySpanRanges.Value(iRange);

    myCache->BuildCache(myParams.Value(aRange.StartIdx).Param,
                        aFlatKnots,
                        aPoles->Array1(),
                        isRational ? &aWeights->Array1() : nullptr);

    for (int i = aRange.StartIdx; i < aRange.EndIdx; ++i)
    {
      gp_Pnt aPoint;
      gp_Vec aD1;
      myCache->D1Local(myParams.Value(i).LocalParam, aPoint, aD1);
      aResults.ChangeValue(i + 1) = {aPoint, aD1};
    }
  }

  return aResults;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD2> GeomGridEval_BSplineCurve::EvaluateGridD2() const
{
  if (myGeom.IsNull() || myRawParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD2>();
  }

  prepare();

  if (myParams.IsEmpty() || mySpanRanges.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD2>();
  }

  const int                                 aNbParams = myParams.Size();
  NCollection_Array1<GeomGridEval::CurveD2> aResults(1, aNbParams);

  const Handle(TColStd_HArray1OfReal)& aFlatKnotsHandle = myGeom->HArrayFlatKnots();
  if (aFlatKnotsHandle.IsNull())
  {
    return NCollection_Array1<GeomGridEval::CurveD2>();
  }
  const TColStd_Array1OfReal& aFlatKnots = aFlatKnotsHandle->Array1();

  // Get poles and weights handles directly from geometry
  const int                            aDegree    = myGeom->Degree();
  const Handle(TColgp_HArray1OfPnt)&   aPoles     = myGeom->HArrayPoles();
  const Handle(TColStd_HArray1OfReal)& aWeights   = myGeom->HArrayWeights();
  const bool                           isRational = myGeom->IsRational();

  if (myCache.IsNull())
  {
    myCache = new BSplCLib_Cache(aDegree,
                                 myGeom->IsPeriodic(),
                                 aFlatKnots,
                                 aPoles->Array1(),
                                 isRational ? &aWeights->Array1() : nullptr);
  }

  for (int iRange = 0; iRange < mySpanRanges.Size(); ++iRange)
  {
    const SpanRange& aRange = mySpanRanges.Value(iRange);

    myCache->BuildCache(myParams.Value(aRange.StartIdx).Param,
                        aFlatKnots,
                        aPoles->Array1(),
                        isRational ? &aWeights->Array1() : nullptr);

    for (int i = aRange.StartIdx; i < aRange.EndIdx; ++i)
    {
      gp_Pnt aPoint;
      gp_Vec aD1, aD2;
      myCache->D2Local(myParams.Value(i).LocalParam, aPoint, aD1, aD2);
      aResults.ChangeValue(i + 1) = {aPoint, aD1, aD2};
    }
  }

  return aResults;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD3> GeomGridEval_BSplineCurve::EvaluateGridD3() const
{
  if (myGeom.IsNull() || myRawParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD3>();
  }

  prepare();

  if (myParams.IsEmpty() || mySpanRanges.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD3>();
  }

  const int                                 aNbParams = myParams.Size();
  NCollection_Array1<GeomGridEval::CurveD3> aResults(1, aNbParams);

  const Handle(TColStd_HArray1OfReal)& aFlatKnotsHandle = myGeom->HArrayFlatKnots();
  if (aFlatKnotsHandle.IsNull())
  {
    return NCollection_Array1<GeomGridEval::CurveD3>();
  }
  const TColStd_Array1OfReal& aFlatKnots = aFlatKnotsHandle->Array1();

  // Get poles and weights handles directly from geometry
  const int                            aDegree    = myGeom->Degree();
  const Handle(TColgp_HArray1OfPnt)&   aPoles     = myGeom->HArrayPoles();
  const Handle(TColStd_HArray1OfReal)& aWeights   = myGeom->HArrayWeights();
  const bool                           isRational = myGeom->IsRational();

  if (myCache.IsNull())
  {
    myCache = new BSplCLib_Cache(aDegree,
                                 myGeom->IsPeriodic(),
                                 aFlatKnots,
                                 aPoles->Array1(),
                                 isRational ? &aWeights->Array1() : nullptr);
  }

  for (int iRange = 0; iRange < mySpanRanges.Size(); ++iRange)
  {
    const SpanRange& aRange = mySpanRanges.Value(iRange);

    myCache->BuildCache(myParams.Value(aRange.StartIdx).Param,
                        aFlatKnots,
                        aPoles->Array1(),
                        isRational ? &aWeights->Array1() : nullptr);

    for (int i = aRange.StartIdx; i < aRange.EndIdx; ++i)
    {
      gp_Pnt aPoint;
      gp_Vec aD1, aD2, aD3;
      myCache->D3Local(myParams.Value(i).LocalParam, aPoint, aD1, aD2, aD3);
      aResults.ChangeValue(i + 1) = {aPoint, aD1, aD2, aD3};
    }
  }

  return aResults;
}
