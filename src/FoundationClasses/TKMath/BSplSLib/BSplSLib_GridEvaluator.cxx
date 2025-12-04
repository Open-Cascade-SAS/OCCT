// Copyright (c) 2024 OPEN CASCADE SAS
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
      myPoles(nullptr),
      myWeights(nullptr),
      myUFlatKnots(nullptr),
      myVFlatKnots(nullptr),
      myURational(false),
      myVRational(false),
      myUPeriodic(false),
      myVPeriodic(false),
      myIsInitialized(false)
{
}

//==================================================================================================

void BSplSLib_GridEvaluator::Initialize(int                         theDegreeU,
                                        int                         theDegreeV,
                                        const TColgp_Array2OfPnt&   thePoles,
                                        const TColStd_Array2OfReal* theWeights,
                                        const TColStd_Array1OfReal& theUFlatKnots,
                                        const TColStd_Array1OfReal& theVFlatKnots,
                                        bool                        theURational,
                                        bool                        theVRational,
                                        bool                        theUPeriodic,
                                        bool                        theVPeriodic)
{
  myDegreeU     = theDegreeU;
  myDegreeV     = theDegreeV;
  myPoles       = &thePoles;
  myWeights     = theWeights;
  myUFlatKnots  = &theUFlatKnots;
  myVFlatKnots  = &theVFlatKnots;
  myURational   = theURational;
  myVRational   = theVRational;
  myUPeriodic   = theUPeriodic;
  myVPeriodic   = theVPeriodic;
  myIsInitialized = true;
  // Parameter arrays start empty by default, no resize needed
}

//==================================================================================================

void BSplSLib_GridEvaluator::PrepareUParamsFromKnots(double theUMin, double theUMax, int theMinSamples)
{
  if (!myIsInitialized || myUFlatKnots == nullptr)
  {
    return;
  }
  computeKnotAlignedParams(*myUFlatKnots, myDegreeU, myUPeriodic, theUMin, theUMax, theMinSamples, myUParams);
}

//==================================================================================================

void BSplSLib_GridEvaluator::PrepareVParamsFromKnots(double theVMin, double theVMax, int theMinSamples)
{
  if (!myIsInitialized || myVFlatKnots == nullptr)
  {
    return;
  }
  computeKnotAlignedParams(*myVFlatKnots, myDegreeV, myVPeriodic, theVMin, theVMax, theMinSamples, myVParams);
}

//==================================================================================================

void BSplSLib_GridEvaluator::PrepareUParams(double theUMin, double theUMax, int theNbU)
{
  if (!myIsInitialized || myUFlatKnots == nullptr)
  {
    return;
  }
  computeUniformParams(*myUFlatKnots, myDegreeU, myUPeriodic, theUMin, theUMax, theNbU, myUParams);
}

//==================================================================================================

void BSplSLib_GridEvaluator::PrepareVParams(double theVMin, double theVMax, int theNbV)
{
  if (!myIsInitialized || myVFlatKnots == nullptr)
  {
    return;
  }
  computeUniformParams(*myVFlatKnots, myDegreeV, myVPeriodic, theVMin, theVMax, theNbV, myVParams);
}

//==================================================================================================

void BSplSLib_GridEvaluator::computeKnotAlignedParams(const TColStd_Array1OfReal&        theFlatKnots,
                                                      int                                theDegree,
                                                      bool                               thePeriodic,
                                                      double                             theParamMin,
                                                      double                             theParamMax,
                                                      int                                theMinSamples,
                                                      NCollection_Array1<ParamWithSpan>& theParams) const
{
  NCollection_Vector<ParamWithSpan> aTempParams;

  // First valid span index in flat knots
  const int aFirstSpan = theFlatKnots.Lower() + theDegree;
  // Last valid span index
  const int aLastSpan = theFlatKnots.Upper() - theDegree - 1;

  double aPrevParam = theParamMin;
  aTempParams.Append({aPrevParam, locateSpan(theFlatKnots, theDegree, thePeriodic, aPrevParam)});

  // Iterate through spans and generate samples within each span
  for (int aSpanIdx = aFirstSpan; aSpanIdx <= aLastSpan; ++aSpanIdx)
  {
    const double aSpanStart = theFlatKnots.Value(aSpanIdx);
    const double aSpanEnd   = theFlatKnots.Value(aSpanIdx + 1);

    // Skip spans outside the parameter range
    if (aSpanEnd < theParamMin + Precision::PConfusion())
    {
      continue;
    }
    if (aSpanStart > theParamMax - Precision::PConfusion())
    {
      break;
    }

    // Compute step within the span based on degree
    const double aSpanLength = aSpanEnd - aSpanStart;
    const int    aNbSteps    = std::max(theDegree, 2);
    const double aStep       = aSpanLength / aNbSteps;

    // Generate samples within this span
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

      // Avoid duplicate parameters
      if (aParam > aPrevParam + Precision::PConfusion())
      {
        // For the last point in span, the span index is still aSpanIdx
        // unless we're exactly at the boundary (handled by next span)
        int aEffectiveSpan = aSpanIdx;
        if (k == aNbSteps && aSpanIdx < aLastSpan)
        {
          // At span boundary, use the next span
          aEffectiveSpan = aSpanIdx + 1;
        }
        aTempParams.Append({aParam, aEffectiveSpan});
        aPrevParam = aParam;
      }
    }
  }

  // Add the last parameter if not already added
  if (theParamMax > aPrevParam + Precision::PConfusion())
  {
    aTempParams.Append({theParamMax, locateSpan(theFlatKnots, theDegree, thePeriodic, theParamMax)});
  }

  // If we don't have enough samples, fall back to uniform distribution
  if (aTempParams.Length() < theMinSamples)
  {
    computeUniformParams(theFlatKnots, theDegree, thePeriodic, theParamMin, theParamMax, theMinSamples, theParams);
    return;
  }

  // Copy to output array
  theParams.Resize(1, aTempParams.Length(), false);
  for (int i = 0; i < aTempParams.Length(); ++i)
  {
    theParams.SetValue(i + 1, aTempParams.Value(i));
  }
}

//==================================================================================================

void BSplSLib_GridEvaluator::computeUniformParams(const TColStd_Array1OfReal&        theFlatKnots,
                                                  int                                theDegree,
                                                  bool                               thePeriodic,
                                                  double                             theParamMin,
                                                  double                             theParamMax,
                                                  int                                theNbSamples,
                                                  NCollection_Array1<ParamWithSpan>& theParams) const
{
  if (theNbSamples < 2)
  {
    theNbSamples = 2;
  }

  theParams.Resize(1, theNbSamples, false);

  const double aRange = theParamMax - theParamMin;
  // Small offset from boundaries (similar to original Extrema code)
  const double aOffset = aRange / theNbSamples / 100.0;
  const double aStep   = (aRange - aOffset) / (theNbSamples - 1);
  const double aStart  = theParamMin + aOffset / 2.0;

  // Use Hunt algorithm for efficient span location when parameters are sorted
  int aPrevSpan = theFlatKnots.Lower() + theDegree;

  for (int i = 1; i <= theNbSamples; ++i)
  {
    const double aParam = aStart + (i - 1) * aStep;

    // Find span index - use previous span as hint for efficiency
    int aSpanIdx = locateSpanWithHint(theFlatKnots, theDegree, thePeriodic, aParam, aPrevSpan);
    aPrevSpan = aSpanIdx;

    theParams.SetValue(i, {aParam, aSpanIdx});
  }
}

//==================================================================================================

int BSplSLib_GridEvaluator::locateSpan(const TColStd_Array1OfReal& theFlatKnots,
                                       int                         theDegree,
                                       bool                        thePeriodic,
                                       double                      theParam) const
{
  int    aSpanIndex = 0;
  double aNewParam  = theParam;
  BSplCLib::LocateParameter(theDegree,
                            theFlatKnots,
                            BSplCLib::NoMults(),
                            theParam,
                            thePeriodic,
                            aSpanIndex,
                            aNewParam);
  return aSpanIndex;
}

//==================================================================================================

int BSplSLib_GridEvaluator::locateSpanWithHint(const TColStd_Array1OfReal& theFlatKnots,
                                               int                         theDegree,
                                               bool                        thePeriodic,
                                               double                      theParam,
                                               int                         theHint) const
{
  // Use Hunt algorithm starting from hint
  const int aLower = theFlatKnots.Lower() + theDegree;
  const int aUpper = theFlatKnots.Upper() - theDegree - 1;

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
  return locateSpan(theFlatKnots, theDegree, thePeriodic, theParam);
}

//==================================================================================================

gp_Pnt BSplSLib_GridEvaluator::Value(int theIU, int theIV) const
{
  gp_Pnt aResult;
  D0(theIU, theIV, aResult);
  return aResult;
}

//==================================================================================================

void BSplSLib_GridEvaluator::D0(int theIU, int theIV, gp_Pnt& theP) const
{
  const ParamWithSpan& aUParam = myUParams.Value(theIU);
  const ParamWithSpan& aVParam = myVParams.Value(theIV);

  BSplSLib::D0(aUParam.Param,
               aVParam.Param,
               aUParam.SpanIndex,
               aVParam.SpanIndex,
               *myPoles,
               myWeights,
               *myUFlatKnots,
               *myVFlatKnots,
               nullptr, // NoMults for flat knots
               nullptr, // NoMults for flat knots
               myDegreeU,
               myDegreeV,
               myURational,
               myVRational,
               myUPeriodic,
               myVPeriodic,
               theP);
}

//==================================================================================================

void BSplSLib_GridEvaluator::D1(int theIU, int theIV, gp_Pnt& theP, gp_Vec& theDU, gp_Vec& theDV) const
{
  const ParamWithSpan& aUParam = myUParams.Value(theIU);
  const ParamWithSpan& aVParam = myVParams.Value(theIV);

  BSplSLib::D1(aUParam.Param,
               aVParam.Param,
               aUParam.SpanIndex,
               aVParam.SpanIndex,
               *myPoles,
               myWeights,
               *myUFlatKnots,
               *myVFlatKnots,
               nullptr,
               nullptr,
               myDegreeU,
               myDegreeV,
               myURational,
               myVRational,
               myUPeriodic,
               myVPeriodic,
               theP,
               theDU,
               theDV);
}

//==================================================================================================

void BSplSLib_GridEvaluator::D2(int     theIU,
                                int     theIV,
                                gp_Pnt& theP,
                                gp_Vec& theDU,
                                gp_Vec& theDV,
                                gp_Vec& theDUU,
                                gp_Vec& theDVV,
                                gp_Vec& theDUV) const
{
  const ParamWithSpan& aUParam = myUParams.Value(theIU);
  const ParamWithSpan& aVParam = myVParams.Value(theIV);

  BSplSLib::D2(aUParam.Param,
               aVParam.Param,
               aUParam.SpanIndex,
               aVParam.SpanIndex,
               *myPoles,
               myWeights,
               *myUFlatKnots,
               *myVFlatKnots,
               nullptr,
               nullptr,
               myDegreeU,
               myDegreeV,
               myURational,
               myVRational,
               myUPeriodic,
               myVPeriodic,
               theP,
               theDU,
               theDV,
               theDUU,
               theDVV,
               theDUV);
}
