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

#include <GeomGridEvaluator_BSplineSurface.hxx>

#include <BSplCLib.hxx>
#include <BSplSLib.hxx>
#include <gp_Pnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array2OfReal.hxx>

//==================================================================================================

void GeomGridEvaluator_BSplineSurface::SetUVParams(const TColStd_Array1OfReal& theUParams,
                                                   const TColStd_Array1OfReal& theVParams)
{
  const int aNbU = theUParams.Size();
  const int aNbV = theVParams.Size();

  myRawUParams.Resize(1, aNbU, false);
  for (int i = 1; i <= aNbU; ++i)
  {
    myRawUParams.SetValue(i, theUParams.Value(theUParams.Lower() + i - 1));
  }

  myRawVParams.Resize(1, aNbV, false);
  for (int j = 1; j <= aNbV; ++j)
  {
    myRawVParams.SetValue(j, theVParams.Value(theVParams.Lower() + j - 1));
  }

  // Clear cached span data
  myUSpanRanges = NCollection_Array1<SpanRange>();
}

//==================================================================================================

void GeomGridEvaluator_BSplineSurface::prepare() const
{
  // Check if already prepared using span ranges emptiness
  if (!myUSpanRanges.IsEmpty())
  {
    return;
  }

  if (myGeom.IsNull() || myRawUParams.IsEmpty() || myRawVParams.IsEmpty())
  {
    myUParams     = NCollection_Array1<ParamWithSpan>();
    myVParams     = NCollection_Array1<ParamWithSpan>();
    myVSpanRanges = NCollection_Array1<SpanRange>();
    return;
  }

  // Get flat knots directly from geometry
  const Handle(TColStd_HArray1OfReal)& aUFlatKnotsHandle = myGeom->HArrayUFlatKnots();
  const Handle(TColStd_HArray1OfReal)& aVFlatKnotsHandle = myGeom->HArrayVFlatKnots();
  if (aUFlatKnotsHandle.IsNull() || aVFlatKnotsHandle.IsNull())
  {
    myUParams     = NCollection_Array1<ParamWithSpan>();
    myVParams     = NCollection_Array1<ParamWithSpan>();
    myVSpanRanges = NCollection_Array1<SpanRange>();
    return;
  }
  const TColStd_Array1OfReal& aUFlatKnots = aUFlatKnotsHandle->Array1();
  const TColStd_Array1OfReal& aVFlatKnots = aVFlatKnotsHandle->Array1();

  const int aUDegree = myGeom->UDegree();
  const int aVDegree = myGeom->VDegree();

  const int aNbU = myRawUParams.Size();
  const int aNbV = myRawVParams.Size();

  // Process U parameters
  myUParams.Resize(0, aNbU - 1, false);

  int aPrevUSpan = aUFlatKnots.Lower() + aUDegree;
  for (int i = 1; i <= aNbU; ++i)
  {
    const double aParam   = myRawUParams.Value(i);
    const int    aSpanIdx = locateSpanWithHint(aParam, true, aPrevUSpan, aUFlatKnots);
    aPrevUSpan            = aSpanIdx;

    const double aSpanStart   = aUFlatKnots.Value(aSpanIdx);
    const double aSpanHalfLen = 0.5 * (aUFlatKnots.Value(aSpanIdx + 1) - aSpanStart);
    const double aSpanMid     = aSpanStart + aSpanHalfLen;
    const double aLocalParam  = (aParam - aSpanMid) / aSpanHalfLen;

    myUParams.SetValue(i - 1, ParamWithSpan{aParam, aLocalParam, aSpanIdx});
  }

  // Process V parameters
  myVParams.Resize(0, aNbV - 1, false);

  int aPrevVSpan = aVFlatKnots.Lower() + aVDegree;
  for (int j = 1; j <= aNbV; ++j)
  {
    const double aParam   = myRawVParams.Value(j);
    const int    aSpanIdx = locateSpanWithHint(aParam, false, aPrevVSpan, aVFlatKnots);
    aPrevVSpan            = aSpanIdx;

    const double aSpanStart   = aVFlatKnots.Value(aSpanIdx);
    const double aSpanHalfLen = 0.5 * (aVFlatKnots.Value(aSpanIdx + 1) - aSpanStart);
    const double aSpanMid     = aSpanStart + aSpanHalfLen;
    const double aLocalParam  = (aParam - aSpanMid) / aSpanHalfLen;

    myVParams.SetValue(j - 1, ParamWithSpan{aParam, aLocalParam, aSpanIdx});
  }

  // Compute span ranges
  computeSpanRanges(myUParams, aUFlatKnots, myUSpanRanges);
  computeSpanRanges(myVParams, aVFlatKnots, myVSpanRanges);
}

//==================================================================================================

int GeomGridEvaluator_BSplineSurface::locateSpan(double                      theParam,
                                                 bool                        theUDir,
                                                 const TColStd_Array1OfReal& theFlatKnots) const
{
  int    aSpanIndex = 0;
  double aNewParam  = theParam;

  const int  aDegree    = theUDir ? myGeom->UDegree() : myGeom->VDegree();
  const bool isPeriodic = theUDir ? myGeom->IsUPeriodic() : myGeom->IsVPeriodic();

  BSplCLib::LocateParameter(aDegree,
                            theFlatKnots,
                            BSplCLib::NoMults(),
                            theParam,
                            isPeriodic,
                            aSpanIndex,
                            aNewParam);
  return aSpanIndex;
}

//==================================================================================================

int GeomGridEvaluator_BSplineSurface::locateSpanWithHint(double                      theParam,
                                                         bool                        theUDir,
                                                         int                         theHint,
                                                         const TColStd_Array1OfReal& theFlatKnots) const
{
  const int aDegree = theUDir ? myGeom->UDegree() : myGeom->VDegree();
  const int aLower  = theFlatKnots.Lower() + aDegree;
  const int aUpper  = theFlatKnots.Upper() - aDegree - 1;

  if (theHint >= aLower && theHint <= aUpper)
  {
    const double aSpanStart = theFlatKnots.Value(theHint);
    const double aSpanEnd   = theFlatKnots.Value(theHint + 1);

    if (theParam >= aSpanStart && theParam < aSpanEnd)
    {
      return theHint;
    }

    if (theHint < aUpper && theParam >= aSpanEnd)
    {
      const double aNextEnd = theFlatKnots.Value(theHint + 2);
      if (theParam < aNextEnd)
      {
        return theHint + 1;
      }
    }
  }

  return locateSpan(theParam, theUDir, theFlatKnots);
}

//==================================================================================================

void GeomGridEvaluator_BSplineSurface::computeSpanRanges(
  const NCollection_Array1<ParamWithSpan>& theParams,
  const TColStd_Array1OfReal&              theFlatKnots,
  NCollection_Array1<SpanRange>&           theSpanRanges)
{
  const int aNbParams = theParams.Size();
  if (aNbParams == 0)
  {
    theSpanRanges = NCollection_Array1<SpanRange>();
    return;
  }

  // Count distinct spans
  int aNbRanges    = 1;
  int aCurrentSpan = theParams.Value(0).SpanIndex;
  for (int i = 1; i < aNbParams; ++i)
  {
    if (theParams.Value(i).SpanIndex != aCurrentSpan)
    {
      ++aNbRanges;
      aCurrentSpan = theParams.Value(i).SpanIndex;
    }
  }

  theSpanRanges.Resize(0, aNbRanges - 1, false);

  // Fill ranges
  aCurrentSpan    = theParams.Value(0).SpanIndex;
  int aRangeStart = 0;
  int aRangeIdx   = 0;

  for (int i = 1; i < aNbParams; ++i)
  {
    if (theParams.Value(i).SpanIndex != aCurrentSpan)
    {
      const double aSpanStart   = theFlatKnots.Value(aCurrentSpan);
      const double aSpanHalfLen = 0.5 * (theFlatKnots.Value(aCurrentSpan + 1) - aSpanStart);
      const double aSpanMid     = aSpanStart + aSpanHalfLen;

      theSpanRanges.SetValue(aRangeIdx++,
                             SpanRange{aCurrentSpan, aRangeStart, i, aSpanMid, aSpanHalfLen});
      aCurrentSpan = theParams.Value(i).SpanIndex;
      aRangeStart  = i;
    }
  }

  // Last range
  const double aSpanStart   = theFlatKnots.Value(aCurrentSpan);
  const double aSpanHalfLen = 0.5 * (theFlatKnots.Value(aCurrentSpan + 1) - aSpanStart);
  const double aSpanMid     = aSpanStart + aSpanHalfLen;

  theSpanRanges.SetValue(aRangeIdx,
                         SpanRange{aCurrentSpan, aRangeStart, aNbParams, aSpanMid, aSpanHalfLen});
}

//==================================================================================================

NCollection_Array2<gp_Pnt> GeomGridEvaluator_BSplineSurface::EvaluateGrid() const
{
  if (myGeom.IsNull() || myRawUParams.IsEmpty() || myRawVParams.IsEmpty())
  {
    return NCollection_Array2<gp_Pnt>();
  }

  // Prepare span data if needed
  prepare();

  const int aNbUParams = myUParams.Size();
  const int aNbVParams = myVParams.Size();

  if (aNbUParams == 0 || aNbVParams == 0 || myUSpanRanges.IsEmpty() || myVSpanRanges.IsEmpty())
  {
    return NCollection_Array2<gp_Pnt>();
  }

  NCollection_Array2<gp_Pnt> aPoints(1, aNbUParams, 1, aNbVParams);

  // Get flat knots directly from geometry
  const Handle(TColStd_HArray1OfReal)& aUFlatKnotsHandle = myGeom->HArrayUFlatKnots();
  const Handle(TColStd_HArray1OfReal)& aVFlatKnotsHandle = myGeom->HArrayVFlatKnots();
  if (aUFlatKnotsHandle.IsNull() || aVFlatKnotsHandle.IsNull())
  {
    return NCollection_Array2<gp_Pnt>();
  }
  const TColStd_Array1OfReal& aUFlatKnots = aUFlatKnotsHandle->Array1();
  const TColStd_Array1OfReal& aVFlatKnots = aVFlatKnotsHandle->Array1();

  const int aUDegree = myGeom->UDegree();
  const int aVDegree = myGeom->VDegree();

  // Get poles and weights from geometry
  TColgp_Array2OfPnt aPoles(1, myGeom->NbUPoles(), 1, myGeom->NbVPoles());
  myGeom->Poles(aPoles);

  TColStd_Array2OfReal aWeights;
  const bool           isRational = myGeom->IsURational() || myGeom->IsVRational();
  if (isRational)
  {
    aWeights.Resize(1, myGeom->NbUPoles(), 1, myGeom->NbVPoles(), false);
    myGeom->Weights(aWeights);
  }

  // Create or update cache
  if (myCache.IsNull())
  {
    myCache = new BSplSLib_Cache(aUDegree,
                                 myGeom->IsUPeriodic(),
                                 aUFlatKnots,
                                 aVDegree,
                                 myGeom->IsVPeriodic(),
                                 aVFlatKnots,
                                 isRational ? &aWeights : nullptr);
  }

  // Threshold for cache vs direct evaluation
  constexpr int THE_CACHE_THRESHOLD = 4;

  const int aNbUSpans = myUSpanRanges.Size();
  const int aNbVSpans = myVSpanRanges.Size();

  for (int iURange = 0; iURange < aNbUSpans; ++iURange)
  {
    const SpanRange& aURange    = myUSpanRanges.Value(iURange);
    const int        aNbUInSpan = aURange.EndIdx - aURange.StartIdx;

    for (int iVRange = 0; iVRange < aNbVSpans; ++iVRange)
    {
      const SpanRange& aVRange    = myVSpanRanges.Value(iVRange);
      const int        aNbVInSpan = aVRange.EndIdx - aVRange.StartIdx;
      const int        aNbPoints  = aNbUInSpan * aNbVInSpan;

      if (aNbPoints >= THE_CACHE_THRESHOLD)
      {
        // Large block: use cache
        myCache->BuildCache(myUParams.Value(aURange.StartIdx).Param,
                            myVParams.Value(aVRange.StartIdx).Param,
                            aUFlatKnots,
                            aVFlatKnots,
                            aPoles,
                            isRational ? &aWeights : nullptr);

        for (int iu = aURange.StartIdx; iu < aURange.EndIdx; ++iu)
        {
          const double aLocalU = myUParams.Value(iu).LocalParam;
          for (int iv = aVRange.StartIdx; iv < aVRange.EndIdx; ++iv)
          {
            myCache->D0Local(aLocalU,
                             myVParams.Value(iv).LocalParam,
                             aPoints.ChangeValue(iu + 1, iv + 1));
          }
        }
      }
      else
      {
        // Small block: direct evaluation
        for (int iu = aURange.StartIdx; iu < aURange.EndIdx; ++iu)
        {
          const double aUParam   = myUParams.Value(iu).Param;
          const int    aUSpanIdx = aURange.SpanIndex;

          for (int iv = aVRange.StartIdx; iv < aVRange.EndIdx; ++iv)
          {
            const double aVParam   = myVParams.Value(iv).Param;
            const int    aVSpanIdx = aVRange.SpanIndex;

            BSplSLib::D0(aUParam,
                         aVParam,
                         aUSpanIdx,
                         aVSpanIdx,
                         aPoles,
                         isRational ? &aWeights : nullptr,
                         aUFlatKnots,
                         aVFlatKnots,
                         nullptr,
                         nullptr,
                         aUDegree,
                         aVDegree,
                         myGeom->IsURational(),
                         myGeom->IsVRational(),
                         myGeom->IsUPeriodic(),
                         myGeom->IsVPeriodic(),
                         aPoints.ChangeValue(iu + 1, iv + 1));
          }
        }
      }
    }
  }

  return aPoints;
}
