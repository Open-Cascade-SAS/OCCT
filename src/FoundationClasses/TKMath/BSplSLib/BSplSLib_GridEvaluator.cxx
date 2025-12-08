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

#include <BSplSLib_GridEvaluator.hxx>

#include <BSplCLib.hxx>
#include <BSplSLib.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <NCollection_Vector.hxx>
#include <Precision.hxx>

//==================================================================================================

BSplSLib_GridEvaluator::BSplSLib_GridEvaluator()
    : myDegreeU(0),
      myDegreeV(0),
      myURational(false),
      myVRational(false),
      myUPeriodic(false),
      myVPeriodic(false),
      myIsInitialized(false),
      myCachedUSpanIndex(-1),
      myCachedVSpanIndex(-1)
{
}

//==================================================================================================

bool BSplSLib_GridEvaluator::Initialize(int                                  theDegreeU,
                                        int                                  theDegreeV,
                                        const Handle(TColgp_HArray2OfPnt)&   thePoles,
                                        const Handle(TColStd_HArray2OfReal)& theWeights,
                                        const Handle(TColStd_HArray1OfReal)& theUFlatKnots,
                                        const Handle(TColStd_HArray1OfReal)& theVFlatKnots,
                                        bool                                 theURational,
                                        bool                                 theVRational,
                                        bool                                 theUPeriodic,
                                        bool                                 theVPeriodic)
{
  myIsInitialized = false;

  // Validate degrees
  if (theDegreeU < 1 || theDegreeV < 1)
  {
    return false;
  }

  // Validate poles handle
  if (thePoles.IsNull())
  {
    return false;
  }

  // Validate flat knots handles
  if (theUFlatKnots.IsNull() || theVFlatKnots.IsNull())
  {
    return false;
  }

  // Validate rational surface has weights
  if ((theURational || theVRational) && theWeights.IsNull())
  {
    return false;
  }

  // Validate weights array dimensions match poles if provided
  if (!theWeights.IsNull())
  {
    if (theWeights->RowLength() != thePoles->RowLength()
        || theWeights->ColLength() != thePoles->ColLength())
    {
      return false;
    }
  }

  // Validate flat knots sizes
  const int aNbUPoles        = thePoles->ColLength();
  const int aNbVPoles        = thePoles->RowLength();
  const int anExpectedUKnots = aNbUPoles + theDegreeU + 1;
  const int anExpectedVKnots = aNbVPoles + theDegreeV + 1;

  if (!theUPeriodic && theUFlatKnots->Length() != anExpectedUKnots)
  {
    return false;
  }
  if (!theVPeriodic && theVFlatKnots->Length() != anExpectedVKnots)
  {
    return false;
  }

  myDegreeU       = theDegreeU;
  myDegreeV       = theDegreeV;
  myPoles         = thePoles;
  myWeights       = theWeights;
  myUFlatKnots    = theUFlatKnots;
  myVFlatKnots    = theVFlatKnots;
  myURational     = theURational;
  myVRational     = theVRational;
  myUPeriodic     = theUPeriodic;
  myVPeriodic     = theVPeriodic;
  myIsInitialized = true;

  // Reset cache
  myCache.Nullify();
  myCachedUSpanIndex = -1;
  myCachedVSpanIndex = -1;

  return true;
}

//==================================================================================================

bool BSplSLib_GridEvaluator::InitializeBezier(const Handle(TColgp_HArray2OfPnt)&   thePoles,
                                              const Handle(TColStd_HArray2OfReal)& theWeights)
{
  myIsInitialized = false;

  // Validate poles handle
  if (thePoles.IsNull())
  {
    return false;
  }

  const int aNbUPoles = thePoles->ColLength();
  const int aNbVPoles = thePoles->RowLength();

  if (aNbUPoles < 2 || aNbVPoles < 2)
  {
    return false;
  }

  // Validate weights array dimensions match poles if provided
  if (!theWeights.IsNull())
  {
    if (theWeights->RowLength() != aNbVPoles || theWeights->ColLength() != aNbUPoles)
    {
      return false;
    }
  }

  const int aDegreeU = aNbUPoles - 1;
  const int aDegreeV = aNbVPoles - 1;

  // Generate Bezier flat knots for U: [0,0,...,0,1,1,...,1]
  const int                     aNbUKnots     = 2 * (aDegreeU + 1);
  Handle(TColStd_HArray1OfReal) aBezierUKnots = new TColStd_HArray1OfReal(1, aNbUKnots);
  for (int i = 1; i <= aDegreeU + 1; ++i)
  {
    aBezierUKnots->SetValue(i, 0.0);
  }
  for (int i = aDegreeU + 2; i <= aNbUKnots; ++i)
  {
    aBezierUKnots->SetValue(i, 1.0);
  }

  // Generate Bezier flat knots for V: [0,0,...,0,1,1,...,1]
  const int                     aNbVKnots     = 2 * (aDegreeV + 1);
  Handle(TColStd_HArray1OfReal) aBezierVKnots = new TColStd_HArray1OfReal(1, aNbVKnots);
  for (int i = 1; i <= aDegreeV + 1; ++i)
  {
    aBezierVKnots->SetValue(i, 0.0);
  }
  for (int i = aDegreeV + 2; i <= aNbVKnots; ++i)
  {
    aBezierVKnots->SetValue(i, 1.0);
  }

  myDegreeU       = aDegreeU;
  myDegreeV       = aDegreeV;
  myPoles         = thePoles;
  myWeights       = theWeights;
  myUFlatKnots    = aBezierUKnots;
  myVFlatKnots    = aBezierVKnots;
  myURational     = !theWeights.IsNull();
  myVRational     = !theWeights.IsNull();
  myUPeriodic     = false;
  myVPeriodic     = false;
  myIsInitialized = true;

  // Reset cache
  myCache.Nullify();
  myCachedUSpanIndex = -1;
  myCachedVSpanIndex = -1;

  return true;
}

//==================================================================================================

void BSplSLib_GridEvaluator::PrepareUParamsFromKnots(double theUMin,
                                                     double theUMax,
                                                     int    theMinSamples,
                                                     bool   theIncludeEnds)
{
  if (!myIsInitialized || myUFlatKnots.IsNull())
  {
    return;
  }
  computeKnotAlignedParams(myUFlatKnots,
                           myDegreeU,
                           myUPeriodic,
                           theUMin,
                           theUMax,
                           theMinSamples,
                           theIncludeEnds,
                           myUParams);
}

//==================================================================================================

void BSplSLib_GridEvaluator::PrepareVParamsFromKnots(double theVMin,
                                                     double theVMax,
                                                     int    theMinSamples,
                                                     bool   theIncludeEnds)
{
  if (!myIsInitialized || myVFlatKnots.IsNull())
  {
    return;
  }
  computeKnotAlignedParams(myVFlatKnots,
                           myDegreeV,
                           myVPeriodic,
                           theVMin,
                           theVMax,
                           theMinSamples,
                           theIncludeEnds,
                           myVParams);
}

//==================================================================================================

void BSplSLib_GridEvaluator::PrepareUParams(double theUMin,
                                            double theUMax,
                                            int    theNbU,
                                            bool   theIncludeEnds)
{
  if (!myIsInitialized || myUFlatKnots.IsNull())
  {
    return;
  }
  computeUniformParams(myUFlatKnots,
                       myDegreeU,
                       myUPeriodic,
                       theUMin,
                       theUMax,
                       theNbU,
                       theIncludeEnds,
                       myUParams);
}

//==================================================================================================

void BSplSLib_GridEvaluator::PrepareVParams(double theVMin,
                                            double theVMax,
                                            int    theNbV,
                                            bool   theIncludeEnds)
{
  if (!myIsInitialized || myVFlatKnots.IsNull())
  {
    return;
  }
  computeUniformParams(myVFlatKnots,
                       myDegreeV,
                       myVPeriodic,
                       theVMin,
                       theVMax,
                       theNbV,
                       theIncludeEnds,
                       myVParams);
}

//==================================================================================================

void BSplSLib_GridEvaluator::SetUParams(const Handle(TColStd_HArray1OfReal)& theUParams)
{
  if (!myIsInitialized || myUFlatKnots.IsNull() || theUParams.IsNull())
  {
    return;
  }

  const int aNbParams = theUParams->Length();
  if (aNbParams < 2)
  {
    return;
  }

  myUParams.Resize(1, aNbParams, false);

  // Use hint-based span location for efficiency on sorted parameters
  int aPrevSpan = myUFlatKnots->Lower() + myDegreeU;

  for (int i = 1; i <= aNbParams; ++i)
  {
    const double aParam = theUParams->Value(i);
    const int    aSpanIdx =
      locateSpanWithHint(myUFlatKnots, myDegreeU, myUPeriodic, aParam, aPrevSpan);
    aPrevSpan = aSpanIdx;
    myUParams.SetValue(i, {aParam, aSpanIdx});
  }
}

//==================================================================================================

void BSplSLib_GridEvaluator::SetVParams(const Handle(TColStd_HArray1OfReal)& theVParams)
{
  if (!myIsInitialized || myVFlatKnots.IsNull() || theVParams.IsNull())
  {
    return;
  }

  const int aNbParams = theVParams->Length();
  if (aNbParams < 2)
  {
    return;
  }

  myVParams.Resize(1, aNbParams, false);

  // Use hint-based span location for efficiency on sorted parameters
  int aPrevSpan = myVFlatKnots->Lower() + myDegreeV;

  for (int i = 1; i <= aNbParams; ++i)
  {
    const double aParam = theVParams->Value(i);
    const int    aSpanIdx =
      locateSpanWithHint(myVFlatKnots, myDegreeV, myVPeriodic, aParam, aPrevSpan);
    aPrevSpan = aSpanIdx;
    myVParams.SetValue(i, {aParam, aSpanIdx});
  }
}

//==================================================================================================

void BSplSLib_GridEvaluator::computeKnotAlignedParams(
  const Handle(TColStd_HArray1OfReal)& theFlatKnots,
  int                                  theDegree,
  bool                                 thePeriodic,
  double                               theParamMin,
  double                               theParamMax,
  int                                  theMinSamples,
  bool                                 theIncludeEnds,
  NCollection_Array1<ParamWithSpan>&   theParams) const
{
  // Use NCollection_Vector for single-pass algorithm (dynamic growth)
  NCollection_Vector<ParamWithSpan> aParamsVec;

  const TColStd_Array1OfReal& aKnots = theFlatKnots->Array1();

  // First valid span index in flat knots
  const int aFirstSpan = aKnots.Lower() + theDegree;
  // Last valid span index
  const int aLastSpan = aKnots.Upper() - theDegree - 1;

  double aPrevParam = theParamMin - 1.0; // Ensure first point is added

  // Add starting boundary if requested
  if (theIncludeEnds)
  {
    const int aStartSpan = locateSpan(theFlatKnots, theDegree, thePeriodic, theParamMin);
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

    const int    aNbSteps = std::max(theDegree, 2);
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
    const int anEndSpan = locateSpan(theFlatKnots, theDegree, thePeriodic, theParamMax);
    aParamsVec.Append({theParamMax, anEndSpan});
  }

  // If we don't have enough samples, fall back to uniform distribution
  if (aParamsVec.Length() < theMinSamples)
  {
    computeUniformParams(theFlatKnots,
                         theDegree,
                         thePeriodic,
                         theParamMin,
                         theParamMax,
                         theMinSamples,
                         theIncludeEnds,
                         theParams);
    return;
  }

  // Copy to fixed-size array
  theParams.Resize(1, aParamsVec.Length(), false);
  for (int i = 0; i < aParamsVec.Length(); ++i)
  {
    theParams.SetValue(i + 1, aParamsVec.Value(i));
  }
}

//==================================================================================================

void BSplSLib_GridEvaluator::computeUniformParams(
  const Handle(TColStd_HArray1OfReal)& theFlatKnots,
  int                                  theDegree,
  bool                                 thePeriodic,
  double                               theParamMin,
  double                               theParamMax,
  int                                  theNbSamples,
  bool                                 theIncludeEnds,
  NCollection_Array1<ParamWithSpan>&   theParams) const
{
  if (theNbSamples < 2)
  {
    theNbSamples = 2;
  }

  theParams.Resize(1, theNbSamples, false);

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

  const TColStd_Array1OfReal& aKnots = theFlatKnots->Array1();

  // Use hint-based span location for efficiency on sorted parameters
  int aPrevSpan = aKnots.Lower() + theDegree;

  for (int i = 1; i <= theNbSamples; ++i)
  {
    double aParam = aStart + (i - 1) * aStep;

    // Ensure last point is exactly at boundary when including ends
    if (theIncludeEnds && i == theNbSamples)
    {
      aParam = theParamMax;
    }

    // Find span index - use previous span as hint for efficiency
    const int aSpanIdx =
      locateSpanWithHint(theFlatKnots, theDegree, thePeriodic, aParam, aPrevSpan);
    aPrevSpan = aSpanIdx;

    theParams.SetValue(i, {aParam, aSpanIdx});
  }
}

//==================================================================================================

int BSplSLib_GridEvaluator::locateSpan(const Handle(TColStd_HArray1OfReal)& theFlatKnots,
                                       int                                  theDegree,
                                       bool                                 thePeriodic,
                                       double                               theParam) const
{
  int    aSpanIndex = 0;
  double aNewParam  = theParam;
  BSplCLib::LocateParameter(theDegree,
                            theFlatKnots->Array1(),
                            BSplCLib::NoMults(),
                            theParam,
                            thePeriodic,
                            aSpanIndex,
                            aNewParam);
  return aSpanIndex;
}

//==================================================================================================

int BSplSLib_GridEvaluator::locateSpanWithHint(const Handle(TColStd_HArray1OfReal)& theFlatKnots,
                                               int                                  theDegree,
                                               bool                                 thePeriodic,
                                               double                               theParam,
                                               int                                  theHint) const
{
  const TColStd_Array1OfReal& aKnots = theFlatKnots->Array1();
  const int                   aLower = aKnots.Lower() + theDegree;
  const int                   aUpper = aKnots.Upper() - theDegree - 1;

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
  return locateSpan(theFlatKnots, theDegree, thePeriodic, theParam);
}

//==================================================================================================

void BSplSLib_GridEvaluator::ensureCacheValid(int    theUSpanIndex,
                                              int    theVSpanIndex,
                                              double theUParam,
                                              double theVParam) const
{
  if (myCachedUSpanIndex == theUSpanIndex && myCachedVSpanIndex == theVSpanIndex
      && !myCache.IsNull())
  {
    return;
  }

  // Create cache if needed
  if (myCache.IsNull())
  {
    myCache = new BSplSLib_Cache(myDegreeU,
                                 myUPeriodic,
                                 myUFlatKnots->Array1(),
                                 myDegreeV,
                                 myVPeriodic,
                                 myVFlatKnots->Array1(),
                                 myWeights.IsNull() ? nullptr : &myWeights->Array2());
  }

  // Build cache for the parameters (constructor doesn't build cache data)
  myCache->BuildCache(theUParam,
                      theVParam,
                      myUFlatKnots->Array1(),
                      myVFlatKnots->Array1(),
                      myPoles->Array2(),
                      myWeights.IsNull() ? nullptr : &myWeights->Array2());
  myCachedUSpanIndex = theUSpanIndex;
  myCachedVSpanIndex = theVSpanIndex;
}

//==================================================================================================

std::optional<gp_Pnt> BSplSLib_GridEvaluator::Value(int theIU, int theIV) const
{
  gp_Pnt aResult;
  if (!D0(theIU, theIV, aResult))
  {
    return std::nullopt;
  }
  return aResult;
}

//==================================================================================================

bool BSplSLib_GridEvaluator::D0(int theIU, int theIV, gp_Pnt& theP) const
{
  if (!areValidIndices(theIU, theIV))
  {
    return false;
  }

  const ParamWithSpan& aUParam = myUParams.Value(theIU);
  const ParamWithSpan& aVParam = myVParams.Value(theIV);

  ensureCacheValid(aUParam.SpanIndex, aVParam.SpanIndex, aUParam.Param, aVParam.Param);
  myCache->D0(aUParam.Param, aVParam.Param, theP);
  return true;
}

//==================================================================================================

bool BSplSLib_GridEvaluator::D1(int     theIU,
                                int     theIV,
                                gp_Pnt& theP,
                                gp_Vec& theDU,
                                gp_Vec& theDV) const
{
  if (!areValidIndices(theIU, theIV))
  {
    return false;
  }

  const ParamWithSpan& aUParam = myUParams.Value(theIU);
  const ParamWithSpan& aVParam = myVParams.Value(theIV);

  ensureCacheValid(aUParam.SpanIndex, aVParam.SpanIndex, aUParam.Param, aVParam.Param);
  myCache->D1(aUParam.Param, aVParam.Param, theP, theDU, theDV);
  return true;
}

//==================================================================================================

bool BSplSLib_GridEvaluator::D2(int     theIU,
                                int     theIV,
                                gp_Pnt& theP,
                                gp_Vec& theDU,
                                gp_Vec& theDV,
                                gp_Vec& theDUU,
                                gp_Vec& theDVV,
                                gp_Vec& theDUV) const
{
  if (!areValidIndices(theIU, theIV))
  {
    return false;
  }

  const ParamWithSpan& aUParam = myUParams.Value(theIU);
  const ParamWithSpan& aVParam = myVParams.Value(theIV);

  ensureCacheValid(aUParam.SpanIndex, aVParam.SpanIndex, aUParam.Param, aVParam.Param);
  myCache->D2(aUParam.Param, aVParam.Param, theP, theDU, theDV, theDUU, theDVV, theDUV);
  return true;
}

//==================================================================================================

NCollection_Array2<gp_Pnt> BSplSLib_GridEvaluator::EvaluateGrid() const
{
  if (!myIsInitialized || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<gp_Pnt>();
  }

  const int                  aNbU = myUParams.Length();
  const int                  aNbV = myVParams.Length();
  NCollection_Array2<gp_Pnt> aPoints(1, aNbU, 1, aNbV);

  for (int iu = 1; iu <= aNbU; ++iu)
  {
    const ParamWithSpan& aUParam = myUParams.Value(iu);
    for (int iv = 1; iv <= aNbV; ++iv)
    {
      const ParamWithSpan& aVParam = myVParams.Value(iv);
      ensureCacheValid(aUParam.SpanIndex, aVParam.SpanIndex, aUParam.Param, aVParam.Param);
      myCache->D0(aUParam.Param, aVParam.Param, aPoints.ChangeValue(iu, iv));
    }
  }

  return aPoints;
}
