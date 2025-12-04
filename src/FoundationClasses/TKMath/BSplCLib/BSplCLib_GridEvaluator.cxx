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

#include <BSplCLib_GridEvaluator.hxx>

#include <BSplCLib.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>

//==================================================================================================

BSplCLib_GridEvaluator::BSplCLib_GridEvaluator()
    : myDegree(0),
      myPoles(nullptr),
      myWeights(nullptr),
      myFlatKnots(nullptr),
      myRational(false),
      myPeriodic(false),
      myIsInitialized(false)
{
}

//==================================================================================================

void BSplCLib_GridEvaluator::Initialize(int                         theDegree,
                                        const TColgp_Array1OfPnt&   thePoles,
                                        const TColStd_Array1OfReal* theWeights,
                                        const TColStd_Array1OfReal& theFlatKnots,
                                        bool                        theRational,
                                        bool                        thePeriodic)
{
  myDegree      = theDegree;
  myPoles       = &thePoles;
  myWeights     = theWeights;
  myFlatKnots   = &theFlatKnots;
  myRational    = theRational;
  myPeriodic    = thePeriodic;
  myIsInitialized = true;
  // Parameter array starts empty by default, no resize needed
}

//==================================================================================================

void BSplCLib_GridEvaluator::PrepareParamsFromKnots(double theParamMin, double theParamMax, int theMinSamples)
{
  if (!myIsInitialized || myFlatKnots == nullptr)
  {
    return;
  }
  computeKnotAlignedParams(*myFlatKnots, myDegree, myPeriodic, theParamMin, theParamMax, theMinSamples, myParams);
}

//==================================================================================================

void BSplCLib_GridEvaluator::PrepareParams(double theParamMin, double theParamMax, int theNbSamples)
{
  if (!myIsInitialized || myFlatKnots == nullptr)
  {
    return;
  }
  computeUniformParams(*myFlatKnots, myDegree, myPeriodic, theParamMin, theParamMax, theNbSamples, myParams);
}

//==================================================================================================

void BSplCLib_GridEvaluator::computeKnotAlignedParams(const TColStd_Array1OfReal&        theFlatKnots,
                                                      int                                theDegree,
                                                      bool                               thePeriodic,
                                                      double                             theParamMin,
                                                      double                             theParamMax,
                                                      int                                theMinSamples,
                                                      NCollection_Array1<ParamWithSpan>& theParams) const
{
  // First valid span index in flat knots
  const int aFirstSpan = theFlatKnots.Lower() + theDegree;
  // Last valid span index
  const int aLastSpan = theFlatKnots.Upper() - theDegree - 1;

  // First pass: count the number of parameters to avoid dynamic allocations
  int    aNbParams  = 1; // Start with theParamMin
  double aPrevParam = theParamMin;

  for (int aSpanIdx = aFirstSpan; aSpanIdx <= aLastSpan; ++aSpanIdx)
  {
    const double aSpanStart = theFlatKnots.Value(aSpanIdx);
    const double aSpanEnd   = theFlatKnots.Value(aSpanIdx + 1);

    if (aSpanEnd < theParamMin + Precision::PConfusion())
    {
      continue;
    }
    if (aSpanStart > theParamMax - Precision::PConfusion())
    {
      break;
    }

    const double aSpanLength = aSpanEnd - aSpanStart;
    const int    aNbSteps    = std::max(theDegree, 2);
    const double aStep       = aSpanLength / aNbSteps;

    for (int k = 1; k <= aNbSteps; ++k)
    {
      double aParam = aSpanStart + k * aStep;
      if (aParam > theParamMax - Precision::PConfusion())
      {
        break;
      }
      if (aParam < theParamMin + Precision::PConfusion())
      {
        continue;
      }
      if (aParam > aPrevParam + Precision::PConfusion())
      {
        ++aNbParams;
        aPrevParam = aParam;
      }
    }
  }

  // Add one for theParamMax if needed
  if (theParamMax > aPrevParam + Precision::PConfusion())
  {
    ++aNbParams;
  }

  // If we don't have enough samples, fall back to uniform distribution
  if (aNbParams < theMinSamples)
  {
    computeUniformParams(theFlatKnots, theDegree, thePeriodic, theParamMin, theParamMax, theMinSamples, theParams);
    return;
  }

  // Second pass: resize once and fill directly
  theParams.Resize(1, aNbParams, false);

  int aParamIdx = 1;
  aPrevParam    = theParamMin;
  theParams.SetValue(aParamIdx++, {aPrevParam, locateSpan(theFlatKnots, theDegree, thePeriodic, aPrevParam)});

  for (int aSpanIdx = aFirstSpan; aSpanIdx <= aLastSpan; ++aSpanIdx)
  {
    const double aSpanStart = theFlatKnots.Value(aSpanIdx);
    const double aSpanEnd   = theFlatKnots.Value(aSpanIdx + 1);

    if (aSpanEnd < theParamMin + Precision::PConfusion())
    {
      continue;
    }
    if (aSpanStart > theParamMax - Precision::PConfusion())
    {
      break;
    }

    const double aSpanLength = aSpanEnd - aSpanStart;
    const int    aNbSteps    = std::max(theDegree, 2);
    const double aStep       = aSpanLength / aNbSteps;

    for (int k = 1; k <= aNbSteps; ++k)
    {
      double aParam = aSpanStart + k * aStep;
      if (aParam > theParamMax - Precision::PConfusion())
      {
        break;
      }
      if (aParam < theParamMin + Precision::PConfusion())
      {
        continue;
      }
      if (aParam > aPrevParam + Precision::PConfusion())
      {
        int aEffectiveSpan = aSpanIdx;
        if (k == aNbSteps && aSpanIdx < aLastSpan)
        {
          aEffectiveSpan = aSpanIdx + 1;
        }
        theParams.SetValue(aParamIdx++, {aParam, aEffectiveSpan});
        aPrevParam = aParam;
      }
    }
  }

  // Add the last parameter if not already added
  if (theParamMax > aPrevParam + Precision::PConfusion())
  {
    theParams.SetValue(aParamIdx, {theParamMax, locateSpan(theFlatKnots, theDegree, thePeriodic, theParamMax)});
  }
}

//==================================================================================================

void BSplCLib_GridEvaluator::computeUniformParams(const TColStd_Array1OfReal&        theFlatKnots,
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
  // Small offset from boundaries
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

int BSplCLib_GridEvaluator::locateSpan(const TColStd_Array1OfReal& theFlatKnots,
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

int BSplCLib_GridEvaluator::locateSpanWithHint(const TColStd_Array1OfReal& theFlatKnots,
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

gp_Pnt BSplCLib_GridEvaluator::Value(int theIndex) const
{
  gp_Pnt aResult;
  D0(theIndex, aResult);
  return aResult;
}

//==================================================================================================

void BSplCLib_GridEvaluator::D0(int theIndex, gp_Pnt& theP) const
{
  const ParamWithSpan& aParam = myParams.Value(theIndex);

  BSplCLib::D0(aParam.Param,
               aParam.SpanIndex,
               myDegree,
               myPeriodic,
               *myPoles,
               myWeights,
               *myFlatKnots,
               nullptr, // NoMults for flat knots
               theP);
}

//==================================================================================================

void BSplCLib_GridEvaluator::D1(int theIndex, gp_Pnt& theP, gp_Vec& theD1) const
{
  const ParamWithSpan& aParam = myParams.Value(theIndex);

  BSplCLib::D1(aParam.Param,
               aParam.SpanIndex,
               myDegree,
               myPeriodic,
               *myPoles,
               myWeights,
               *myFlatKnots,
               nullptr, // NoMults for flat knots
               theP,
               theD1);
}

//==================================================================================================

void BSplCLib_GridEvaluator::D2(int theIndex, gp_Pnt& theP, gp_Vec& theD1, gp_Vec& theD2) const
{
  const ParamWithSpan& aParam = myParams.Value(theIndex);

  BSplCLib::D2(aParam.Param,
               aParam.SpanIndex,
               myDegree,
               myPeriodic,
               *myPoles,
               myWeights,
               *myFlatKnots,
               nullptr, // NoMults for flat knots
               theP,
               theD1,
               theD2);
}
