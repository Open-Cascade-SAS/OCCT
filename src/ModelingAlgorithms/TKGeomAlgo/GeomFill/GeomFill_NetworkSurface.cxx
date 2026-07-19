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

#include <GeomFill_NetworkSurface.hxx>

#include <BSplCLib.hxx>
#include <BSplSLib.hxx>
#include <BSplSLib_EvaluatorFunction.hxx>
#include <GeomFill_Profiler.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_LinearVector.hxx>
#include <Precision.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <StdFail_NotDone.hxx>
#include <gp_Vec.hxx>

#include <algorithm>

namespace
{
bool checkParameters(const NCollection_Array1<double>& theParameters)
{
  if (theParameters.Size() < 2)
  {
    return false;
  }
  for (size_t aParamIdx = 1; aParamIdx < theParameters.Size(); ++aParamIdx)
  {
    if (theParameters.At(aParamIdx) <= theParameters.At(aParamIdx - 1) + Precision::PConfusion())
    {
      return false;
    }
  }
  return true;
}

bool checkWeights(const NCollection_Array2<double>& theWeights)
{
  if (theWeights.IsEmpty())
  {
    return false;
  }
  for (size_t aWeightIdx = 0; aWeightIdx < theWeights.Size(); ++aWeightIdx)
  {
    if (theWeights.NCollection_Array1<double>::At(aWeightIdx) <= Precision::Confusion())
    {
      return false;
    }
  }
  return true;
}

//! Wraps linear vector storage into a 1-based Array1 without copying.
template <typename T>
NCollection_Array1<T> toOneBasedArrayView(NCollection_LinearVector<T>& theValues)
{
  return NCollection_Array1<T>(theValues.ChangeValue(0), 1, static_cast<int>(theValues.Size()));
}

void reparametrizeSurface(occ::handle<Geom_BSplineSurface>& theSurface)
{
  NCollection_Array1<double> aUKnots(theSurface->UKnots());
  NCollection_Array1<double> aVKnots(theSurface->VKnots());
  BSplCLib::Reparametrize(0.0, 1.0, aUKnots);
  BSplCLib::Reparametrize(0.0, 1.0, aVKnots);
  theSurface->SetUKnots(aUKnots);
  theSurface->SetVKnots(aVKnots);
}

bool hasRationalCurve(const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theCurves)
{
  for (size_t aCurveIdx = 0; aCurveIdx < theCurves.Size(); ++aCurveIdx)
  {
    if (!theCurves.At(aCurveIdx).IsNull() && theCurves.At(aCurveIdx)->IsRational())
    {
      return true;
    }
  }
  return false;
}

bool hasPeriodicCurve(const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theCurves)
{
  for (size_t aCurveIdx = 0; aCurveIdx < theCurves.Size(); ++aCurveIdx)
  {
    if (theCurves.At(aCurveIdx).IsNull() || theCurves.At(aCurveIdx)->IsPeriodic())
    {
      return true;
    }
  }
  return false;
}

bool isCompatibleCurveFamily(const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theCurves)
{
  if (theCurves.IsEmpty() || theCurves.First().IsNull())
  {
    return false;
  }

  const occ::handle<Geom_BSplineCurve>& aReference = theCurves.First();
  const bool                            isRational = aReference->IsRational();
  for (size_t aCurveIdx = 1; aCurveIdx < theCurves.Size(); ++aCurveIdx)
  {
    const occ::handle<Geom_BSplineCurve>& aCurve = theCurves.At(aCurveIdx);
    if (aCurve.IsNull() || aCurve->IsRational() != isRational
        || aCurve->Degree() != aReference->Degree() || aCurve->NbPoles() != aReference->NbPoles()
        || aCurve->NbKnots() != aReference->NbKnots())
    {
      return false;
    }
    for (size_t aKnotIdx = 0; aKnotIdx < static_cast<size_t>(aCurve->NbKnots()); ++aKnotIdx)
    {
      const int aKnot = static_cast<int>(aKnotIdx) + 1;
      if (aCurve->Multiplicity(aKnot) != aReference->Multiplicity(aKnot)
          || std::abs(aCurve->Knot(aKnot) - aReference->Knot(aKnot)) > Precision::PConfusion())
      {
        return false;
      }
    }
  }
  return true;
}

bool prepareCurveFamily(NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theCurves)
{
  if (hasRationalCurve(theCurves))
  {
    return isCompatibleCurveFamily(theCurves);
  }

  GeomFill_Profiler aProfiler;
  for (size_t aCurveIdx = 0; aCurveIdx < theCurves.Size(); ++aCurveIdx)
  {
    if (theCurves.At(aCurveIdx).IsNull())
    {
      return false;
    }
    aProfiler.AddCurve(theCurves.At(aCurveIdx));
  }

  aProfiler.Perform(Precision::PConfusion());
  for (size_t aCurveIdx = 0; aCurveIdx < theCurves.Size(); ++aCurveIdx)
  {
    theCurves.ChangeAt(aCurveIdx) =
      occ::down_cast<Geom_BSplineCurve>(aProfiler.Curve(static_cast<int>(aCurveIdx) + 1));
    if (theCurves.At(aCurveIdx).IsNull())
    {
      return false;
    }
  }

  return true;
}

void makeInterpolationBasis(const NCollection_Array1<double>& theParameters,
                            int&                              theDegree,
                            NCollection_Array1<double>&       theKnots,
                            NCollection_Array1<int>&          theMults,
                            NCollection_Array1<double>&       theFlatKnots)
{
  const int aNbParams = static_cast<int>(theParameters.Size());
  theDegree           = std::min(3, aNbParams - 1);

  const int aNbInternalKnots = aNbParams - theDegree - 1;
  theKnots.Resize(1, aNbInternalKnots + 2, false);
  theMults.Resize(1, aNbInternalKnots + 2, false);

  theKnots(1) = theParameters.At(0);
  theMults(1) = theDegree + 1;
  for (int aKnotIdx = 1; aKnotIdx <= aNbInternalKnots; ++aKnotIdx)
  {
    double aSum = 0.0;
    for (int aParamOffset = 1; aParamOffset <= theDegree; ++aParamOffset)
    {
      aSum += theParameters.At(static_cast<size_t>(aKnotIdx + aParamOffset - 1));
    }
    theKnots(aKnotIdx + 1) = aSum / static_cast<double>(theDegree);
    theMults(aKnotIdx + 1) = 1;
  }
  theKnots(theKnots.Upper()) = theParameters.At(theParameters.Size() - 1);
  theMults(theMults.Upper()) = theDegree + 1;

  theFlatKnots.Resize(1, theDegree + aNbParams + 1, false);
  int aFlatIdx = 1;
  for (int aKnotIdx = theKnots.Lower(); aKnotIdx <= theKnots.Upper(); ++aKnotIdx)
  {
    for (int aMultIdx = 1; aMultIdx <= theMults(aKnotIdx); ++aMultIdx)
    {
      theFlatKnots(aFlatIdx++) = theKnots(aKnotIdx);
    }
  }
}

bool interpolatePoles(const int                         theDegree,
                      const NCollection_Array1<double>& theFlatKnots,
                      const NCollection_Array1<double>& theParameters,
                      const NCollection_Array1<int>&    theContactOrders,
                      NCollection_Array1<gp_Pnt>&       thePoles,
                      NCollection_Array1<double>&       theWeights)
{
  int anInversionProblem = 0;
  BSplCLib::Interpolate(theDegree,
                        theFlatKnots,
                        theParameters,
                        theContactOrders,
                        thePoles,
                        theWeights,
                        anInversionProblem);
  return anInversionProblem == 0;
}

double poleWeight(const occ::handle<Geom_BSplineCurve>& theCurve, const int thePoleIdx)
{
  return theCurve->IsRational() ? theCurve->Weight(thePoleIdx) : 1.0;
}

bool sameKnotRange(const NCollection_Array1<double>& theKnots1,
                   const NCollection_Array1<double>& theKnots2,
                   double&                           theStart,
                   double&                           theEnd)
{
  theStart = theKnots1.First();
  theEnd   = theKnots1.Last();
  return std::abs(theStart - theKnots2.First()) <= Precision::PConfusion()
         && std::abs(theEnd - theKnots2.Last()) <= Precision::PConfusion()
         && theStart < theEnd - Precision::PConfusion();
}

//! Candidate knot with multiplicity collected before tolerance compaction.
struct KnotEntry
{
  double Parameter    = 0.0;
  int    Multiplicity = 0;
};

//! Interpolation basis used to skin one curve family.
struct SkinningBasis
{
  int                        Degree = 0;
  NCollection_Array1<double> Knots;
  NCollection_Array1<int>    Mults;
  NCollection_Array1<double> FlatKnots;

  void Init(const NCollection_Array1<double>& theParameters)
  {
    makeInterpolationBasis(theParameters, Degree, Knots, Mults, FlatKnots);
  }
};

//! Evaluates the homogeneous denominator of a B-spline surface.
double denominatorValue(const occ::handle<Geom_BSplineSurface>& theSurface,
                        const double                            theU,
                        const double                            theV)
{
  double aWeight = 1.0;
  gp_Pnt aNumerator;
  BSplSLib::HomogeneousD0(theU,
                          theV,
                          0,
                          0,
                          theSurface->Poles(),
                          &theSurface->WeightsArray(),
                          theSurface->UKnots(),
                          theSurface->VKnots(),
                          &theSurface->UMultiplicities(),
                          &theSurface->VMultiplicities(),
                          theSurface->UDegree(),
                          theSurface->VDegree(),
                          true,
                          true,
                          false,
                          false,
                          aWeight,
                          aNumerator);
  return aWeight;
}

//! Multiplier for the product of two surface denominators.
class DenominatorProductEvaluator : public BSplSLib_EvaluatorFunction
{
public:
  DenominatorProductEvaluator(const occ::handle<Geom_BSplineSurface>& theFirst,
                              const occ::handle<Geom_BSplineSurface>& theSecond)
      : myFirst(theFirst),
        mySecond(theSecond)
  {
  }

  void Evaluate(const int    theDerivativeRequest,
                const double theU,
                const double theV,
                double&      theResult,
                int&         theErrorCode) const override
  {
    if (theDerivativeRequest != 0 || myFirst.IsNull() || mySecond.IsNull())
    {
      theErrorCode = 1;
      return;
    }

    theResult    = denominatorValue(myFirst, theU, theV) * denominatorValue(mySecond, theU, theV);
    theErrorCode = 0;
  }

private:
  occ::handle<Geom_BSplineSurface> myFirst;
  occ::handle<Geom_BSplineSurface> mySecond;
};

bool makeTripleProductBasis(const NCollection_Array1<double>& theKnots,
                            const NCollection_Array1<int>&    theMults,
                            const int                         theDegree,
                            int&                              theProductDegree,
                            NCollection_Array1<double>&       theProductKnots,
                            NCollection_Array1<int>&          theProductMults,
                            NCollection_Array1<double>&       theProductFlatKnots)
{
  theProductDegree = 3 * theDegree;
  if (theProductDegree > Geom_BSplineSurface::MaxDegree())
  {
    return false;
  }

  theProductKnots.Resize(theKnots.Lower(), theKnots.Upper(), false);
  theProductMults.Resize(theMults.Lower(), theMults.Upper(), false);
  int aFlatLength = 0;
  for (int aKnotIdx = theKnots.Lower(); aKnotIdx <= theKnots.Upper(); ++aKnotIdx)
  {
    theProductKnots(aKnotIdx) = theKnots(aKnotIdx);
    if (aKnotIdx == theKnots.Lower() || aKnotIdx == theKnots.Upper())
    {
      theProductMults(aKnotIdx) = theProductDegree + 1;
    }
    else
    {
      theProductMults(aKnotIdx) = std::min(theProductDegree, 2 * theDegree + theMults(aKnotIdx));
    }
    aFlatLength += theProductMults(aKnotIdx);
  }

  theProductFlatKnots.Resize(1, aFlatLength, false);
  BSplCLib::KnotSequence(theProductKnots, theProductMults, theProductFlatKnots);
  return true;
}

bool multiplyByDenominators(const occ::handle<Geom_BSplineSurface>& theSurface,
                            const occ::handle<Geom_BSplineSurface>& theFirstDenominator,
                            const occ::handle<Geom_BSplineSurface>& theSecondDenominator,
                            const int                               theUDegree,
                            const int                               theVDegree,
                            const NCollection_Array1<double>&       theUFlatKnots,
                            const NCollection_Array1<double>&       theVFlatKnots,
                            NCollection_Array2<gp_Pnt>&             theNumerator,
                            NCollection_Array2<double>&             theDenominator)
{
  DenominatorProductEvaluator aMultiplier(theFirstDenominator, theSecondDenominator);

  int aStatus = 0;
  BSplSLib::FunctionMultiply(aMultiplier,
                             theSurface->UDegree(),
                             theSurface->VDegree(),
                             theSurface->UKnots(),
                             theSurface->VKnots(),
                             &theSurface->UMultiplicities(),
                             &theSurface->VMultiplicities(),
                             theSurface->Poles(),
                             &theSurface->WeightsArray(),
                             theUFlatKnots,
                             theVFlatKnots,
                             theUDegree,
                             theVDegree,
                             theNumerator,
                             theDenominator,
                             aStatus);
  return aStatus == 0;
}

occ::handle<Geom_BSplineSurface> makeProfileSkin(
  const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theProfiles,
  const NCollection_Array1<double>&                         theProfileParameters,
  const int                                                 theVDegree,
  const NCollection_Array1<double>&                         theVKnots,
  const NCollection_Array1<int>&                            theVMults,
  const NCollection_Array1<double>&                         theVFlatKnots)
{
  const occ::handle<Geom_BSplineCurve>& aBaseProfile = theProfiles(theProfiles.Lower());
  NCollection_Array2<gp_Pnt>            aPoles(1, aBaseProfile->NbPoles(), 1, theProfiles.Length());
  NCollection_Array2<double> aWeights(1, aBaseProfile->NbPoles(), 1, theProfiles.Length());
  NCollection_Array1<gp_Pnt> aColumn(1, theProfiles.Length());
  NCollection_Array1<double> aColumnWeights(1, theProfiles.Length());
  NCollection_Array1<int>    aContactOrders(theProfileParameters.Lower(),
                                         theProfileParameters.Upper());
  aContactOrders.Init(0);

  for (int aUPoleIdx = 1; aUPoleIdx <= aBaseProfile->NbPoles(); ++aUPoleIdx)
  {
    for (int aProfileIdx = 1; aProfileIdx <= theProfiles.Length(); ++aProfileIdx)
    {
      aColumn(aProfileIdx)        = theProfiles(aProfileIdx)->Pole(aUPoleIdx);
      aColumnWeights(aProfileIdx) = poleWeight(theProfiles(aProfileIdx), aUPoleIdx);
    }
    if (!interpolatePoles(theVDegree,
                          theVFlatKnots,
                          theProfileParameters,
                          aContactOrders,
                          aColumn,
                          aColumnWeights))
    {
      return nullptr;
    }
    for (int aVIdx = 1; aVIdx <= theProfiles.Length(); ++aVIdx)
    {
      aPoles(aUPoleIdx, aVIdx)   = aColumn(aVIdx);
      aWeights(aUPoleIdx, aVIdx) = aColumnWeights(aVIdx);
    }
  }

  return new Geom_BSplineSurface(aPoles,
                                 aWeights,
                                 aBaseProfile->Knots(),
                                 theVKnots,
                                 aBaseProfile->Multiplicities(),
                                 theVMults,
                                 aBaseProfile->Degree(),
                                 theVDegree);
}

occ::handle<Geom_BSplineSurface> makeGuideSkin(
  const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theGuides,
  const NCollection_Array1<double>&                         theGuideParameters,
  const int                                                 theUDegree,
  const NCollection_Array1<double>&                         theUKnots,
  const NCollection_Array1<int>&                            theUMults,
  const NCollection_Array1<double>&                         theUFlatKnots)
{
  const occ::handle<Geom_BSplineCurve>& aBaseGuide = theGuides(theGuides.Lower());
  NCollection_Array2<gp_Pnt>            aPoles(1, theGuides.Length(), 1, aBaseGuide->NbPoles());
  NCollection_Array2<double>            aWeights(1, theGuides.Length(), 1, aBaseGuide->NbPoles());
  NCollection_Array1<gp_Pnt>            aRow(1, theGuides.Length());
  NCollection_Array1<double>            aRowWeights(1, theGuides.Length());
  NCollection_Array1<int> aContactOrders(theGuideParameters.Lower(), theGuideParameters.Upper());
  aContactOrders.Init(0);

  for (int aVPoleIdx = 1; aVPoleIdx <= aBaseGuide->NbPoles(); ++aVPoleIdx)
  {
    for (int aGuideIdx = 1; aGuideIdx <= theGuides.Length(); ++aGuideIdx)
    {
      aRow(aGuideIdx)        = theGuides(aGuideIdx)->Pole(aVPoleIdx);
      aRowWeights(aGuideIdx) = poleWeight(theGuides(aGuideIdx), aVPoleIdx);
    }
    if (!interpolatePoles(theUDegree,
                          theUFlatKnots,
                          theGuideParameters,
                          aContactOrders,
                          aRow,
                          aRowWeights))
    {
      return nullptr;
    }
    for (int aUIdx = 1; aUIdx <= theGuides.Length(); ++aUIdx)
    {
      aPoles(aUIdx, aVPoleIdx)   = aRow(aUIdx);
      aWeights(aUIdx, aVPoleIdx) = aRowWeights(aUIdx);
    }
  }

  return new Geom_BSplineSurface(aPoles,
                                 aWeights,
                                 theUKnots,
                                 aBaseGuide->Knots(),
                                 theUMults,
                                 aBaseGuide->Multiplicities(),
                                 theUDegree,
                                 aBaseGuide->Degree());
}

double normalizedKnot(const double theStart, const double theEnd, const double theKnot)
{
  if (std::abs(theKnot - theStart) <= Precision::PConfusion())
  {
    return theStart;
  }
  if (std::abs(theKnot - theEnd) <= Precision::PConfusion())
  {
    return theEnd;
  }
  return theKnot;
}

//! Adds knots from one basis to a flat collection, clamping boundary values.
void collectKnots(const int                            theDegree,
                  const double                         theStart,
                  const double                         theEnd,
                  const NCollection_Array1<double>&    theKnots,
                  const NCollection_Array1<int>&       theMults,
                  NCollection_LinearVector<KnotEntry>& theEntries)
{
  for (int aKnotIdx = theKnots.Lower(); aKnotIdx <= theKnots.Upper(); ++aKnotIdx)
  {
    const double aKnot      = normalizedKnot(theStart, theEnd, theKnots(aKnotIdx));
    const bool   isBoundary = aKnot == theStart || aKnot == theEnd;
    const int    aMaxMult   = isBoundary ? theDegree + 1 : theDegree;
    KnotEntry    anEntry;
    anEntry.Parameter    = aKnot;
    anEntry.Multiplicity = std::min(theMults(aKnotIdx), aMaxMult);
    theEntries.Append(anEntry);
  }
}

//! Sorts candidate knots and keeps the largest multiplicity in each tolerance group.
void compactKnots(NCollection_LinearVector<KnotEntry>& theEntries,
                  NCollection_LinearVector<double>&    theKnots,
                  NCollection_LinearVector<int>&       theMults)
{
  if (theEntries.IsEmpty())
  {
    return;
  }

  std::sort(theEntries.begin(),
            theEntries.end(),
            [](const KnotEntry& theLeft, const KnotEntry& theRight) {
              return theLeft.Parameter < theRight.Parameter;
            });

  KnotEntry aCurrent = theEntries.Value(0);
  for (size_t anEntryIdx = 1; anEntryIdx < theEntries.Size(); ++anEntryIdx)
  {
    const KnotEntry& anEntry = theEntries.Value(anEntryIdx);
    if (std::abs(anEntry.Parameter - aCurrent.Parameter) <= Precision::PConfusion())
    {
      aCurrent.Multiplicity = std::max(aCurrent.Multiplicity, anEntry.Multiplicity);
      continue;
    }

    theKnots.Append(aCurrent.Parameter);
    theMults.Append(aCurrent.Multiplicity);
    aCurrent = anEntry;
  }

  theKnots.Append(aCurrent.Parameter);
  theMults.Append(aCurrent.Multiplicity);
}

void uniteKnots(const int                                 theDegree,
                const NCollection_Array1<double>&         theKnots1,
                const NCollection_Array1<int>&            theMults1,
                const NCollection_Array1<double>&         theKnots2,
                const NCollection_Array1<int>&            theMults2,
                occ::handle<NCollection_HArray1<double>>& theMergedKnots,
                occ::handle<NCollection_HArray1<int>>&    theMergedMults)
{
  double aStart = 0.0;
  double anEnd  = 0.0;
  if (!sameKnotRange(theKnots1, theKnots2, aStart, anEnd))
  {
    return;
  }

  NCollection_LinearVector<KnotEntry> aEntries;
  collectKnots(theDegree, aStart, anEnd, theKnots1, theMults1, aEntries);
  collectKnots(theDegree, aStart, anEnd, theKnots2, theMults2, aEntries);

  NCollection_LinearVector<double> aKnots;
  NCollection_LinearVector<int>    aMults;
  compactKnots(aEntries, aKnots, aMults);

  if (aKnots.IsEmpty() || aMults.IsEmpty())
  {
    return;
  }

  NCollection_Array1<double> aKnotArray = toOneBasedArrayView(aKnots);
  NCollection_Array1<int>    aMultArray = toOneBasedArrayView(aMults);
  theMergedKnots                        = new NCollection_HArray1<double>(aKnotArray);
  theMergedMults                        = new NCollection_HArray1<int>(aMultArray);
}

bool alignSurfaces(occ::handle<Geom_BSplineSurface>& theProfileSurface,
                   occ::handle<Geom_BSplineSurface>& theGuideSurface,
                   occ::handle<Geom_BSplineSurface>& theReferenceSurface)
{
  const int aUDegree = std::max(
    {theProfileSurface->UDegree(), theGuideSurface->UDegree(), theReferenceSurface->UDegree()});
  const int aVDegree = std::max(
    {theProfileSurface->VDegree(), theGuideSurface->VDegree(), theReferenceSurface->VDegree()});

  theProfileSurface->IncreaseDegree(aUDegree, aVDegree);
  theGuideSurface->IncreaseDegree(aUDegree, aVDegree);
  theReferenceSurface->IncreaseDegree(aUDegree, aVDegree);

  occ::handle<NCollection_HArray1<double>> aUKnots12;
  occ::handle<NCollection_HArray1<int>>    aUMults12;
  uniteKnots(aUDegree,
             theProfileSurface->UKnots(),
             theProfileSurface->UMultiplicities(),
             theGuideSurface->UKnots(),
             theGuideSurface->UMultiplicities(),
             aUKnots12,
             aUMults12);
  if (aUKnots12.IsNull() || aUMults12.IsNull())
  {
    return false;
  }

  occ::handle<NCollection_HArray1<double>> aUKnots;
  occ::handle<NCollection_HArray1<int>>    aUMults;
  uniteKnots(aUDegree,
             aUKnots12->Array1(),
             aUMults12->Array1(),
             theReferenceSurface->UKnots(),
             theReferenceSurface->UMultiplicities(),
             aUKnots,
             aUMults);
  if (aUKnots.IsNull() || aUMults.IsNull())
  {
    return false;
  }

  occ::handle<NCollection_HArray1<double>> aVKnots12;
  occ::handle<NCollection_HArray1<int>>    aVMults12;
  uniteKnots(aVDegree,
             theProfileSurface->VKnots(),
             theProfileSurface->VMultiplicities(),
             theGuideSurface->VKnots(),
             theGuideSurface->VMultiplicities(),
             aVKnots12,
             aVMults12);
  if (aVKnots12.IsNull() || aVMults12.IsNull())
  {
    return false;
  }

  occ::handle<NCollection_HArray1<double>> aVKnots;
  occ::handle<NCollection_HArray1<int>>    aVMults;
  uniteKnots(aVDegree,
             aVKnots12->Array1(),
             aVMults12->Array1(),
             theReferenceSurface->VKnots(),
             theReferenceSurface->VMultiplicities(),
             aVKnots,
             aVMults);
  if (aVKnots.IsNull() || aVMults.IsNull())
  {
    return false;
  }

  theProfileSurface->InsertUKnots(aUKnots->Array1(),
                                  aUMults->Array1(),
                                  Precision::PConfusion(),
                                  false);
  theGuideSurface->InsertUKnots(aUKnots->Array1(),
                                aUMults->Array1(),
                                Precision::PConfusion(),
                                false);
  theReferenceSurface->InsertUKnots(aUKnots->Array1(),
                                    aUMults->Array1(),
                                    Precision::PConfusion(),
                                    false);

  theProfileSurface->InsertVKnots(aVKnots->Array1(),
                                  aVMults->Array1(),
                                  Precision::PConfusion(),
                                  false);
  theGuideSurface->InsertVKnots(aVKnots->Array1(),
                                aVMults->Array1(),
                                Precision::PConfusion(),
                                false);
  theReferenceSurface->InsertVKnots(aVKnots->Array1(),
                                    aVMults->Array1(),
                                    Precision::PConfusion(),
                                    false);
  return true;
}

//! Builds the final network surface by correcting the profile skin with guide deviations.
occ::handle<Geom_BSplineSurface> makeCorrectedProfileSkin(
  const occ::handle<Geom_BSplineSurface>& theProfileSurface,
  const occ::handle<Geom_BSplineSurface>& theGuideSurface,
  const occ::handle<Geom_BSplineSurface>& theReferenceSurface,
  GeomFill_NetworkSurface::ResultStatus&  theStatus)
{
  if (theProfileSurface->NbUPoles() != theGuideSurface->NbUPoles()
      || theProfileSurface->NbUPoles() != theReferenceSurface->NbUPoles()
      || theProfileSurface->NbVPoles() != theGuideSurface->NbVPoles()
      || theProfileSurface->NbVPoles() != theReferenceSurface->NbVPoles())
  {
    theStatus = GeomFill_NetworkSurface::ResultStatus::RationalConstructionFailed;
    return nullptr;
  }

  if (!theProfileSurface->IsURational() && !theProfileSurface->IsVRational()
      && !theGuideSurface->IsURational() && !theGuideSurface->IsVRational()
      && !theReferenceSurface->IsURational() && !theReferenceSurface->IsVRational())
  {
    const NCollection_Array2<gp_Pnt>& aProfilePoles   = theProfileSurface->Poles();
    const NCollection_Array2<gp_Pnt>& aGuidePoles     = theGuideSurface->Poles();
    const NCollection_Array2<gp_Pnt>& aReferencePoles = theReferenceSurface->Poles();
    NCollection_Array2<gp_Pnt>        aResultPoles(aProfilePoles);
    for (size_t aPoleIdx = 0; aPoleIdx < aResultPoles.Size(); ++aPoleIdx)
    {
      const gp_XYZ aPoint = aProfilePoles.NCollection_Array1<gp_Pnt>::At(aPoleIdx).XYZ()
                            + aGuidePoles.NCollection_Array1<gp_Pnt>::At(aPoleIdx).XYZ()
                            - aReferencePoles.NCollection_Array1<gp_Pnt>::At(aPoleIdx).XYZ();
      aResultPoles.NCollection_Array1<gp_Pnt>::ChangeAt(aPoleIdx) = gp_Pnt(aPoint);
    }

    return new Geom_BSplineSurface(aResultPoles,
                                   theProfileSurface->UKnots(),
                                   theProfileSurface->VKnots(),
                                   theProfileSurface->UMultiplicities(),
                                   theProfileSurface->VMultiplicities(),
                                   theProfileSurface->UDegree(),
                                   theProfileSurface->VDegree());
  }

  int                        aUDegree = 0;
  int                        aVDegree = 0;
  NCollection_Array1<double> aUKnots;
  NCollection_Array1<int>    aUMults;
  NCollection_Array1<double> aUFlatKnots;
  NCollection_Array1<double> aVKnots;
  NCollection_Array1<int>    aVMults;
  NCollection_Array1<double> aVFlatKnots;
  if (!makeTripleProductBasis(theProfileSurface->UKnots(),
                              theProfileSurface->UMultiplicities(),
                              theProfileSurface->UDegree(),
                              aUDegree,
                              aUKnots,
                              aUMults,
                              aUFlatKnots)
      || !makeTripleProductBasis(theProfileSurface->VKnots(),
                                 theProfileSurface->VMultiplicities(),
                                 theProfileSurface->VDegree(),
                                 aVDegree,
                                 aVKnots,
                                 aVMults,
                                 aVFlatKnots))
  {
    theStatus = GeomFill_NetworkSurface::ResultStatus::RationalDegreeOverflow;
    return nullptr;
  }

  const int aNbUPoles = aUFlatKnots.Length() - aUDegree - 1;
  const int aNbVPoles = aVFlatKnots.Length() - aVDegree - 1;
  if (aNbUPoles < 2 || aNbVPoles < 2)
  {
    theStatus = GeomFill_NetworkSurface::ResultStatus::RationalConstructionFailed;
    return nullptr;
  }

  NCollection_Array2<gp_Pnt> aProfileNumerator(1, aNbUPoles, 1, aNbVPoles);
  NCollection_Array2<double> aProfileDenominator(1, aNbUPoles, 1, aNbVPoles);
  NCollection_Array2<gp_Pnt> aGuideNumerator(1, aNbUPoles, 1, aNbVPoles);
  NCollection_Array2<double> aGuideDenominator(1, aNbUPoles, 1, aNbVPoles);
  NCollection_Array2<gp_Pnt> aReferenceNumerator(1, aNbUPoles, 1, aNbVPoles);
  NCollection_Array2<double> aReferenceDenominator(1, aNbUPoles, 1, aNbVPoles);

  if (!multiplyByDenominators(theProfileSurface,
                              theGuideSurface,
                              theReferenceSurface,
                              aUDegree,
                              aVDegree,
                              aUFlatKnots,
                              aVFlatKnots,
                              aProfileNumerator,
                              aProfileDenominator)
      || !multiplyByDenominators(theGuideSurface,
                                 theProfileSurface,
                                 theReferenceSurface,
                                 aUDegree,
                                 aVDegree,
                                 aUFlatKnots,
                                 aVFlatKnots,
                                 aGuideNumerator,
                                 aGuideDenominator)
      || !multiplyByDenominators(theReferenceSurface,
                                 theProfileSurface,
                                 theGuideSurface,
                                 aUDegree,
                                 aVDegree,
                                 aUFlatKnots,
                                 aVFlatKnots,
                                 aReferenceNumerator,
                                 aReferenceDenominator))
  {
    theStatus = GeomFill_NetworkSurface::ResultStatus::RationalConstructionFailed;
    return nullptr;
  }

  NCollection_Array2<gp_Pnt> aResultPoles(1, aNbUPoles, 1, aNbVPoles);
  NCollection_Array2<double> aResultWeights(1, aNbUPoles, 1, aNbVPoles);
  for (size_t aPoleIdx = 0; aPoleIdx < aResultPoles.Size(); ++aPoleIdx)
  {
    const double aWeight = aProfileDenominator.NCollection_Array1<double>::At(aPoleIdx);
    if (aWeight <= Precision::Confusion()
        || std::abs(aGuideDenominator.NCollection_Array1<double>::At(aPoleIdx) - aWeight)
             > Precision::Confusion()
        || std::abs(aReferenceDenominator.NCollection_Array1<double>::At(aPoleIdx) - aWeight)
             > Precision::Confusion())
    {
      theStatus = GeomFill_NetworkSurface::ResultStatus::RationalConstructionFailed;
      return nullptr;
    }

    const gp_XYZ aNumerator = aProfileNumerator.NCollection_Array1<gp_Pnt>::At(aPoleIdx).XYZ()
                              + aGuideNumerator.NCollection_Array1<gp_Pnt>::At(aPoleIdx).XYZ()
                              - aReferenceNumerator.NCollection_Array1<gp_Pnt>::At(aPoleIdx).XYZ();
    aResultWeights.NCollection_Array1<double>::ChangeAt(aPoleIdx) = aWeight;
    aResultPoles.NCollection_Array1<gp_Pnt>::ChangeAt(aPoleIdx)   = gp_Pnt(aNumerator / aWeight);
  }

  return new Geom_BSplineSurface(aResultPoles,
                                 aResultWeights,
                                 aUKnots,
                                 aVKnots,
                                 aUMults,
                                 aVMults,
                                 aUDegree,
                                 aVDegree);
}

bool canSetPeriodic(const occ::handle<Geom_BSplineSurface>& theSurface,
                    const bool                              theAlongU,
                    const double                            theTolerance)
{
  const int aNbChecks = theAlongU ? theSurface->NbVPoles() : theSurface->NbUPoles();
  for (int aCheckIdx = 1; aCheckIdx <= aNbChecks; ++aCheckIdx)
  {
    const gp_Pnt aFirstPole =
      theAlongU ? theSurface->Pole(1, aCheckIdx) : theSurface->Pole(aCheckIdx, 1);
    const gp_Pnt aLastPole = theAlongU ? theSurface->Pole(theSurface->NbUPoles(), aCheckIdx)
                                       : theSurface->Pole(aCheckIdx, theSurface->NbVPoles());
    if (aFirstPole.Distance(aLastPole) > theTolerance)
    {
      return false;
    }
  }
  return true;
}

bool isReadyToBuild(const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theProfiles,
                    const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theGuides,
                    const NCollection_Array1<double>&                         theProfileParameters,
                    const NCollection_Array1<double>&                         theGuideParameters,
                    const NCollection_Array2<gp_Pnt>&                         theIntersectionPoints,
                    const NCollection_Array2<double>& theIntersectionWeights)
{
  return theProfiles.Length() >= 2 && theGuides.Length() >= 2 && !hasPeriodicCurve(theProfiles)
         && !hasPeriodicCurve(theGuides) && theProfiles.Length() == theProfileParameters.Length()
         && theGuides.Length() == theGuideParameters.Length()
         && theIntersectionPoints.ColLength() == theGuideParameters.Length()
         && theIntersectionPoints.RowLength() == theProfileParameters.Length()
         && theIntersectionWeights.ColLength() == theGuideParameters.Length()
         && theIntersectionWeights.RowLength() == theProfileParameters.Length()
         && checkParameters(theProfileParameters) && checkParameters(theGuideParameters)
         && checkWeights(theIntersectionWeights);
}

occ::handle<Geom_BSplineSurface> makeNetworkSurface(
  const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theProfiles,
  const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theGuides,
  const NCollection_Array1<double>&                         theProfileParameters,
  const NCollection_Array1<double>&                         theGuideParameters,
  const NCollection_Array2<gp_Pnt>&                         theIntersectionPoints,
  const NCollection_Array2<double>&                         theIntersectionWeights,
  GeomFill_NetworkSurface::ResultStatus&                    theStatus)
{
  theStatus = GeomFill_NetworkSurface::ResultStatus::ConstructionFailed;

  SkinningBasis anUBasis;
  anUBasis.Init(theGuideParameters);

  SkinningBasis aVBasis;
  aVBasis.Init(theProfileParameters);

  occ::handle<Geom_BSplineSurface> aProfileSurface = makeProfileSkin(theProfiles,
                                                                     theProfileParameters,
                                                                     aVBasis.Degree,
                                                                     aVBasis.Knots,
                                                                     aVBasis.Mults,
                                                                     aVBasis.FlatKnots);
  if (aProfileSurface.IsNull())
  {
    theStatus = GeomFill_NetworkSurface::ResultStatus::SkinningFailed;
    return nullptr;
  }

  occ::handle<Geom_BSplineSurface> aGuideSurface = makeGuideSkin(theGuides,
                                                                 theGuideParameters,
                                                                 anUBasis.Degree,
                                                                 anUBasis.Knots,
                                                                 anUBasis.Mults,
                                                                 anUBasis.FlatKnots);
  if (aGuideSurface.IsNull())
  {
    theStatus = GeomFill_NetworkSurface::ResultStatus::SkinningFailed;
    return nullptr;
  }

  // The reference surface starts from the same contact grid validated during preparation.
  NCollection_Array2<gp_Pnt> aReferencePoles(theIntersectionPoints);
  NCollection_Array2<double> aReferenceWeights(theIntersectionWeights);

  int anInversionProblem = 0;
  BSplSLib::Interpolate(anUBasis.Degree,
                        aVBasis.Degree,
                        anUBasis.FlatKnots,
                        aVBasis.FlatKnots,
                        theGuideParameters,
                        theProfileParameters,
                        aReferencePoles,
                        aReferenceWeights,
                        anInversionProblem);
  if (anInversionProblem != 0)
  {
    theStatus = GeomFill_NetworkSurface::ResultStatus::ReferenceSurfaceFailed;
    return nullptr;
  }

  occ::handle<Geom_BSplineSurface> aReferenceSurface = new Geom_BSplineSurface(aReferencePoles,
                                                                               aReferenceWeights,
                                                                               anUBasis.Knots,
                                                                               aVBasis.Knots,
                                                                               anUBasis.Mults,
                                                                               aVBasis.Mults,
                                                                               anUBasis.Degree,
                                                                               aVBasis.Degree);
  if (!alignSurfaces(aProfileSurface, aGuideSurface, aReferenceSurface))
  {
    theStatus = GeomFill_NetworkSurface::ResultStatus::KnotAlignmentFailed;
    return nullptr;
  }
  return makeCorrectedProfileSkin(aProfileSurface, aGuideSurface, aReferenceSurface, theStatus);
}

bool applyPeriodicity(occ::handle<Geom_BSplineSurface>& theSurface,
                      const bool                        theIsUClosed,
                      const bool                        theIsVClosed,
                      const double                      theTolerance)
{
  const double aTolerance = std::max(theTolerance, Precision::Confusion());
  if (theIsUClosed)
  {
    if (!canSetPeriodic(theSurface, true, aTolerance))
    {
      return false;
    }
    theSurface->SetUPeriodic();
  }

  if (theIsVClosed)
  {
    if (!canSetPeriodic(theSurface, false, aTolerance))
    {
      return false;
    }
    theSurface->SetVPeriodic();
  }
  return true;
}

} // namespace

//=================================================================================================

GeomFill_NetworkSurface::GeomFill_NetworkSurface() = default;

//=================================================================================================

void GeomFill_NetworkSurface::Init(
  const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theProfiles,
  const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theGuides,
  const NCollection_Array1<double>&                         theProfileParameters,
  const NCollection_Array1<double>&                         theGuideParameters,
  const NCollection_Array2<gp_Pnt>&                         theIntersectionPoints,
  const NCollection_Array2<double>&                         theIntersectionWeights,
  double                                                    theTolerance,
  bool                                                      theIsUClosed,
  bool                                                      theIsVClosed)
{
  myProfiles            = NCollection_Array1<occ::handle<Geom_BSplineCurve>>(theProfiles);
  myGuides              = NCollection_Array1<occ::handle<Geom_BSplineCurve>>(theGuides);
  myProfileParameters   = NCollection_Array1<double>(theProfileParameters);
  myGuideParameters     = NCollection_Array1<double>(theGuideParameters);
  myIntersectionPoints  = NCollection_Array2<gp_Pnt>(theIntersectionPoints);
  myIntersectionWeights = NCollection_Array2<double>(theIntersectionWeights);
  myTolerance           = theTolerance;
  myIsUClosed           = theIsUClosed;
  myIsVClosed           = theIsVClosed;
  myStatus              = ResultStatus::NotStarted;
  mySurface.Nullify();
}

//=================================================================================================

void GeomFill_NetworkSurface::Perform()
{
  myStatus = ResultStatus::NotStarted;
  mySurface.Nullify();

  if (!isReadyToBuild(myProfiles,
                      myGuides,
                      myProfileParameters,
                      myGuideParameters,
                      myIntersectionPoints,
                      myIntersectionWeights))
  {
    myStatus = ResultStatus::InvalidInput;
    return;
  }

  try
  {
    OCC_CATCH_SIGNALS

    if (!prepareCurveFamily(myProfiles) || !prepareCurveFamily(myGuides))
    {
      myStatus = ResultStatus::CurveCompatibilityFailed;
      return;
    }

    ResultStatus aSurfaceStatus = ResultStatus::ConstructionFailed;
    mySurface                   = makeNetworkSurface(myProfiles,
                                   myGuides,
                                   myProfileParameters,
                                   myGuideParameters,
                                   myIntersectionPoints,
                                   myIntersectionWeights,
                                   aSurfaceStatus);
    if (mySurface.IsNull())
    {
      myStatus = aSurfaceStatus;
      return;
    }

    if (!applyPeriodicity(mySurface, myIsUClosed, myIsVClosed, myTolerance))
    {
      mySurface.Nullify();
      myStatus = ResultStatus::PeriodicityFailed;
      return;
    }
    reparametrizeSurface(mySurface);
  }
  catch (Standard_Failure const&)
  {
    mySurface.Nullify();
    myStatus = ResultStatus::ConstructionFailed;
    return;
  }

  myStatus = ResultStatus::Done;
}

//=================================================================================================

const occ::handle<Geom_BSplineSurface>& GeomFill_NetworkSurface::Surface() const
{
  if (!IsDone())
  {
    throw StdFail_NotDone("GeomFill_NetworkSurface::Surface");
  }
  return mySurface;
}
