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

#include <BSplCLib_GridEvaluator.hxx>

#include <BSplCLib.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <NCollection_Vector.hxx>
#include <Precision.hxx>

//==================================================================================================

BSplCLib_GridEvaluator::BSplCLib_GridEvaluator()
    : myDegree(0),
      myRational(false),
      myPeriodic(false),
      myIsInitialized(false),
      myCachedSpanIndex(-1)
{
}

//==================================================================================================

bool BSplCLib_GridEvaluator::Initialize(int                                  theDegree,
                                        const Handle(TColgp_HArray1OfPnt)&   thePoles,
                                        const Handle(TColStd_HArray1OfReal)& theWeights,
                                        const Handle(TColStd_HArray1OfReal)& theFlatKnots,
                                        bool                                 theRational,
                                        bool                                 thePeriodic)
{
  myIsInitialized = false;

  // Validate degree
  if (theDegree < 1)
  {
    return false;
  }

  // Validate poles handle
  if (thePoles.IsNull() || thePoles->Length() < 2)
  {
    return false;
  }

  // Validate flat knots handle
  if (theFlatKnots.IsNull())
  {
    return false;
  }

  // Validate rational curve has weights
  if (theRational && theWeights.IsNull())
  {
    return false;
  }

  // Validate weights array size matches poles if provided
  if (!theWeights.IsNull() && theWeights->Length() != thePoles->Length())
  {
    return false;
  }

  // Validate flat knots size: should be NbPoles + Degree + 1
  const int anExpectedKnots = thePoles->Length() + theDegree + 1;
  if (theFlatKnots->Length() != anExpectedKnots)
  {
    // For periodic curves, the knot vector size can differ
    if (!thePeriodic)
    {
      return false;
    }
  }

  myDegree        = theDegree;
  myPoles         = thePoles;
  myWeights       = theWeights;
  myFlatKnots     = theFlatKnots;
  myRational      = theRational;
  myPeriodic      = thePeriodic;
  myIsInitialized = true;

  // Reset cache
  myCache.Nullify();
  myCachedSpanIndex = -1;

  return true;
}

//==================================================================================================

bool BSplCLib_GridEvaluator::InitializeBezier(const Handle(TColgp_HArray1OfPnt)&   thePoles,
                                              const Handle(TColStd_HArray1OfReal)& theWeights)
{
  myIsInitialized = false;

  // Validate poles handle
  if (thePoles.IsNull())
  {
    return false;
  }

  const int aNbPoles = thePoles->Length();
  if (aNbPoles < 2)
  {
    return false;
  }

  // Validate weights array size matches poles if provided
  if (!theWeights.IsNull() && theWeights->Length() != aNbPoles)
  {
    return false;
  }

  const int aDegree = aNbPoles - 1;

  // Generate Bezier flat knots: [0,0,...,0,1,1,...,1] with (degree+1) zeros and ones
  const int                     aNbKnots     = 2 * (aDegree + 1);
  Handle(TColStd_HArray1OfReal) aBezierKnots = new TColStd_HArray1OfReal(1, aNbKnots);

  for (int i = 1; i <= aDegree + 1; ++i)
  {
    aBezierKnots->SetValue(i, 0.0);
  }
  for (int i = aDegree + 2; i <= aNbKnots; ++i)
  {
    aBezierKnots->SetValue(i, 1.0);
  }

  myDegree        = aDegree;
  myPoles         = thePoles;
  myWeights       = theWeights;
  myFlatKnots     = aBezierKnots;
  myRational      = !theWeights.IsNull();
  myPeriodic      = false;
  myIsInitialized = true;

  // Reset cache
  myCache.Nullify();
  myCachedSpanIndex = -1;

  return true;
}

//==================================================================================================

void BSplCLib_GridEvaluator::PrepareParamsFromKnots(double theParamMin,
                                                    double theParamMax,
                                                    int    theMinSamples,
                                                    bool   theIncludeEnds)
{
  if (!myIsInitialized || myFlatKnots.IsNull())
  {
    return;
  }
  computeKnotAlignedParams(theParamMin, theParamMax, theMinSamples, theIncludeEnds);
}

//==================================================================================================

void BSplCLib_GridEvaluator::PrepareParams(double theParamMin,
                                           double theParamMax,
                                           int    theNbSamples,
                                           bool   theIncludeEnds)
{
  if (!myIsInitialized || myFlatKnots.IsNull())
  {
    return;
  }
  computeUniformParams(theParamMin, theParamMax, theNbSamples, theIncludeEnds);
}

//==================================================================================================

void BSplCLib_GridEvaluator::SetParams(const Handle(TColStd_HArray1OfReal)& theParams)
{
  if (!myIsInitialized || myFlatKnots.IsNull() || theParams.IsNull())
  {
    return;
  }

  const int aNbParams = theParams->Length();
  if (aNbParams < 2)
  {
    return;
  }

  myParams.Resize(1, aNbParams, false);

  // Use hint-based span location for efficiency on sorted parameters
  int aPrevSpan = myFlatKnots->Lower() + myDegree;

  for (int i = 1; i <= aNbParams; ++i)
  {
    const double aParam   = theParams->Value(i);
    const int    aSpanIdx = locateSpanWithHint(aParam, aPrevSpan);
    aPrevSpan             = aSpanIdx;
    myParams.SetValue(i, {aParam, aSpanIdx});
  }
}

//==================================================================================================

void BSplCLib_GridEvaluator::computeKnotAlignedParams(double theParamMin,
                                                      double theParamMax,
                                                      int    theMinSamples,
                                                      bool   theIncludeEnds)
{
  // Use NCollection_Vector for single-pass algorithm (dynamic growth)
  NCollection_Vector<ParamWithSpan> aParamsVec;

  const TColStd_Array1OfReal& aKnots = myFlatKnots->Array1();

  // First valid span index in flat knots
  const int aFirstSpan = aKnots.Lower() + myDegree;
  // Last valid span index
  const int aLastSpan = aKnots.Upper() - myDegree - 1;

  double aPrevParam = theParamMin - 1.0; // Ensure first point is added

  // Add starting boundary if requested
  if (theIncludeEnds)
  {
    const int aStartSpan = locateSpan(theParamMin);
    aParamsVec.Append({theParamMin, aStartSpan});
    aPrevParam = theParamMin;
  }

  // Iterate through spans and add sample points
  for (int aSpanIdx = aFirstSpan; aSpanIdx <= aLastSpan; ++aSpanIdx)
  {
    const double aSpanStart = aKnots.Value(aSpanIdx);
    const double aSpanEnd   = aKnots.Value(aSpanIdx + 1);

    // Skip spans outside the parameter range
    if (aSpanEnd < theParamMin + Precision::PConfusion())
    {
      continue;
    }
    if (aSpanStart > theParamMax - Precision::PConfusion())
    {
      break;
    }

    // Skip degenerate spans (zero length)
    const double aSpanLength = aSpanEnd - aSpanStart;
    if (aSpanLength < Precision::PConfusion())
    {
      continue;
    }

    const int    aNbSteps = std::max(myDegree, 2);
    const double aStep    = aSpanLength / aNbSteps;

    for (int k = 1; k <= aNbSteps; ++k)
    {
      double aParam = aSpanStart + k * aStep;

      // Clamp to parameter range
      if (aParam > theParamMax - Precision::PConfusion())
      {
        break;
      }
      if (aParam < theParamMin + Precision::PConfusion())
      {
        continue;
      }

      // Avoid duplicate points
      if (aParam > aPrevParam + Precision::PConfusion())
      {
        // For points at span boundaries, use the next span
        int anEffectiveSpan = aSpanIdx;
        if (k == aNbSteps && aSpanIdx < aLastSpan)
        {
          anEffectiveSpan = aSpanIdx + 1;
        }
        aParamsVec.Append({aParam, anEffectiveSpan});
        aPrevParam = aParam;
      }
    }
  }

  // Add ending boundary if requested and not already added
  if (theIncludeEnds && theParamMax > aPrevParam + Precision::PConfusion())
  {
    const int anEndSpan = locateSpan(theParamMax);
    aParamsVec.Append({theParamMax, anEndSpan});
  }

  // If we don't have enough samples, fall back to uniform distribution
  if (aParamsVec.Length() < theMinSamples)
  {
    computeUniformParams(theParamMin, theParamMax, theMinSamples, theIncludeEnds);
    return;
  }

  // Copy to fixed-size array
  myParams.Resize(1, aParamsVec.Length(), false);
  for (int i = 0; i < aParamsVec.Length(); ++i)
  {
    myParams.SetValue(i + 1, aParamsVec.Value(i));
  }
}

//==================================================================================================

void BSplCLib_GridEvaluator::computeUniformParams(double theParamMin,
                                                  double theParamMax,
                                                  int    theNbSamples,
                                                  bool   theIncludeEnds)
{
  if (theNbSamples < 2)
  {
    theNbSamples = 2;
  }

  myParams.Resize(1, theNbSamples, false);

  const double aRange = theParamMax - theParamMin;

  double aStart, aStep;
  if (theIncludeEnds)
  {
    // Include exact boundary values
    aStart = theParamMin;
    aStep  = aRange / (theNbSamples - 1);
  }
  else
  {
    // Small offset from boundaries (useful for avoiding singularities)
    const double aOffset = aRange / theNbSamples / 100.0;
    aStart               = theParamMin + aOffset / 2.0;
    aStep                = (aRange - aOffset) / (theNbSamples - 1);
  }

  const TColStd_Array1OfReal& aKnots = myFlatKnots->Array1();

  // Use hint-based span location for efficiency on sorted parameters
  int aPrevSpan = aKnots.Lower() + myDegree;

  for (int i = 1; i <= theNbSamples; ++i)
  {
    double aParam = aStart + (i - 1) * aStep;

    // Ensure last point is exactly at boundary when including ends
    if (theIncludeEnds && i == theNbSamples)
    {
      aParam = theParamMax;
    }

    // Find span index - use previous span as hint for efficiency
    const int aSpanIdx = locateSpanWithHint(aParam, aPrevSpan);
    aPrevSpan          = aSpanIdx;

    myParams.SetValue(i, {aParam, aSpanIdx});
  }
}

//==================================================================================================

int BSplCLib_GridEvaluator::locateSpan(double theParam) const
{
  int    aSpanIndex = 0;
  double aNewParam  = theParam;
  BSplCLib::LocateParameter(myDegree,
                            myFlatKnots->Array1(),
                            BSplCLib::NoMults(),
                            theParam,
                            myPeriodic,
                            aSpanIndex,
                            aNewParam);
  return aSpanIndex;
}

//==================================================================================================

int BSplCLib_GridEvaluator::locateSpanWithHint(double theParam, int theHint) const
{
  const TColStd_Array1OfReal& aKnots = myFlatKnots->Array1();
  const int                   aLower = aKnots.Lower() + myDegree;
  const int                   aUpper = aKnots.Upper() - myDegree - 1;

  // Quick check if hint is still valid
  if (theHint >= aLower && theHint <= aUpper)
  {
    const double aSpanStart = aKnots.Value(theHint);
    const double aSpanEnd   = aKnots.Value(theHint + 1);

    if (theParam >= aSpanStart && theParam < aSpanEnd)
    {
      return theHint;
    }

    // Check next span (common case for sorted parameters)
    if (theHint < aUpper && theParam >= aSpanEnd)
    {
      const double aNextEnd = aKnots.Value(theHint + 2);
      if (theParam < aNextEnd)
      {
        return theHint + 1;
      }
    }
  }

  // Fall back to standard locate
  return locateSpan(theParam);
}

//==================================================================================================

void BSplCLib_GridEvaluator::ensureCacheValid(int theSpanIndex, double theParam) const
{
  if (myCachedSpanIndex == theSpanIndex && !myCache.IsNull())
  {
    return;
  }

  // Create cache if needed
  if (myCache.IsNull())
  {
    myCache = new BSplCLib_Cache(myDegree,
                                 myPeriodic,
                                 myFlatKnots->Array1(),
                                 myPoles->Array1(),
                                 myWeights.IsNull() ? nullptr : &myWeights->Array1());
  }

  // Build cache for the parameter (constructor doesn't build cache data)
  myCache->BuildCache(theParam,
                      myFlatKnots->Array1(),
                      myPoles->Array1(),
                      myWeights.IsNull() ? nullptr : &myWeights->Array1());
  myCachedSpanIndex = theSpanIndex;
}

//==================================================================================================

std::optional<gp_Pnt> BSplCLib_GridEvaluator::Value(int theIndex) const
{
  gp_Pnt aResult;
  if (!D0(theIndex, aResult))
  {
    return std::nullopt;
  }
  return aResult;
}

//==================================================================================================

bool BSplCLib_GridEvaluator::D0(int theIndex, gp_Pnt& theP) const
{
  if (!isValidIndex(theIndex))
  {
    return false;
  }

  const ParamWithSpan& aParam = myParams.Value(theIndex);

  ensureCacheValid(aParam.SpanIndex, aParam.Param);
  myCache->D0(aParam.Param, theP);
  return true;
}

//==================================================================================================

bool BSplCLib_GridEvaluator::D1(int theIndex, gp_Pnt& theP, gp_Vec& theD1) const
{
  if (!isValidIndex(theIndex))
  {
    return false;
  }

  const ParamWithSpan& aParam = myParams.Value(theIndex);

  ensureCacheValid(aParam.SpanIndex, aParam.Param);
  myCache->D1(aParam.Param, theP, theD1);
  return true;
}

//==================================================================================================

bool BSplCLib_GridEvaluator::D2(int theIndex, gp_Pnt& theP, gp_Vec& theD1, gp_Vec& theD2) const
{
  if (!isValidIndex(theIndex))
  {
    return false;
  }

  const ParamWithSpan& aParam = myParams.Value(theIndex);

  ensureCacheValid(aParam.SpanIndex, aParam.Param);
  myCache->D2(aParam.Param, theP, theD1, theD2);
  return true;
}

//==================================================================================================

NCollection_Array1<gp_Pnt> BSplCLib_GridEvaluator::EvaluateGrid() const
{
  if (!myIsInitialized || myParams.IsEmpty())
  {
    return NCollection_Array1<gp_Pnt>();
  }

  const int                  aNbParams = myParams.Length();
  NCollection_Array1<gp_Pnt> aPoints(1, aNbParams);

  for (int i = 1; i <= aNbParams; ++i)
  {
    const ParamWithSpan& aParam = myParams.Value(i);
    ensureCacheValid(aParam.SpanIndex, aParam.Param);
    myCache->D0(aParam.Param, aPoints.ChangeValue(i));
  }

  return aPoints;
}
