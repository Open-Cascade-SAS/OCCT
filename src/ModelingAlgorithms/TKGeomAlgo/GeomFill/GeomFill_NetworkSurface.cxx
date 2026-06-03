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
#include <NCollection_Array2.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_LinearVector.hxx>
#include <Precision.hxx>
#include <Standard_Failure.hxx>
#include <StdFail_NotDone.hxx>
#include <gp_XYZ.hxx>

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

void reparametrizeSurface(occ::handle<Geom_BSplineSurface>& theSurface)
{
  NCollection_Array1<double> aUKnots(theSurface->UKnots());
  NCollection_Array1<double> aVKnots(theSurface->VKnots());
  BSplCLib::Reparametrize(0.0, 1.0, aUKnots);
  BSplCLib::Reparametrize(0.0, 1.0, aVKnots);
  theSurface->SetUKnots(aUKnots);
  theSurface->SetVKnots(aVKnots);
}

bool sameCurveSpace(const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theCurves)
{
  if (theCurves.Size() < 1 || theCurves.At(0).IsNull())
  {
    return false;
  }

  const occ::handle<Geom_BSplineCurve>& aFirst = theCurves.At(0);
  for (size_t aCurveIdx = 0; aCurveIdx < theCurves.Size(); ++aCurveIdx)
  {
    const occ::handle<Geom_BSplineCurve>& aCurve = theCurves.At(aCurveIdx);
    if (aCurve.IsNull() || aCurve->IsRational() || aCurve->Degree() != aFirst->Degree()
        || aCurve->NbPoles() != aFirst->NbPoles() || aCurve->NbKnots() != aFirst->NbKnots())
    {
      return false;
    }

    for (int aKnotIdx = 1; aKnotIdx <= aFirst->NbKnots(); ++aKnotIdx)
    {
      if (std::abs(aCurve->Knot(aKnotIdx) - aFirst->Knot(aKnotIdx)) > Precision::PConfusion()
          || aCurve->Multiplicity(aKnotIdx) != aFirst->Multiplicity(aKnotIdx))
      {
        return false;
      }
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
                      NCollection_Array1<gp_Pnt>&       thePoles)
{
  int anInversionProblem = 0;
  BSplCLib::Interpolate(theDegree,
                        theFlatKnots,
                        theParameters,
                        theContactOrders,
                        thePoles,
                        anInversionProblem);
  return anInversionProblem == 0;
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
  NCollection_Array1<gp_Pnt>            aColumn(1, theProfiles.Length());
  NCollection_Array1<int>               aContactOrders(theProfileParameters.Lower(),
                                                       theProfileParameters.Upper());
  aContactOrders.Init(0);

  for (int aUPoleIdx = 1; aUPoleIdx <= aBaseProfile->NbPoles(); ++aUPoleIdx)
  {
    for (int aProfileIdx = 1; aProfileIdx <= theProfiles.Length(); ++aProfileIdx)
    {
      aColumn(aProfileIdx) = theProfiles(aProfileIdx)->Pole(aUPoleIdx);
    }
    if (!interpolatePoles(theVDegree, theVFlatKnots, theProfileParameters, aContactOrders, aColumn))
    {
      return nullptr;
    }
    for (int aVIdx = 1; aVIdx <= theProfiles.Length(); ++aVIdx)
    {
      aPoles(aUPoleIdx, aVIdx) = aColumn(aVIdx);
    }
  }

  return new Geom_BSplineSurface(aPoles,
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
  NCollection_Array1<gp_Pnt>            aRow(1, theGuides.Length());
  NCollection_Array1<int> aContactOrders(theGuideParameters.Lower(), theGuideParameters.Upper());
  aContactOrders.Init(0);

  for (int aVPoleIdx = 1; aVPoleIdx <= aBaseGuide->NbPoles(); ++aVPoleIdx)
  {
    for (int aGuideIdx = 1; aGuideIdx <= theGuides.Length(); ++aGuideIdx)
    {
      aRow(aGuideIdx) = theGuides(aGuideIdx)->Pole(aVPoleIdx);
    }
    if (!interpolatePoles(theUDegree, theUFlatKnots, theGuideParameters, aContactOrders, aRow))
    {
      return nullptr;
    }
    for (int aUIdx = 1; aUIdx <= theGuides.Length(); ++aUIdx)
    {
      aPoles(aUIdx, aVPoleIdx) = aRow(aUIdx);
    }
  }

  return new Geom_BSplineSurface(aPoles,
                                 theUKnots,
                                 aBaseGuide->Knots(),
                                 theUMults,
                                 aBaseGuide->Multiplicities(),
                                 theUDegree,
                                 aBaseGuide->Degree());
}

occ::handle<Geom_BSplineSurface> makeTensorSurface(
  const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theProfiles,
  const NCollection_Array1<double>&                         theProfileParameters,
  const NCollection_Array1<double>&                         theGuideParameters,
  const int                                                 theUDegree,
  const int                                                 theVDegree,
  const NCollection_Array1<double>&                         theUKnots,
  const NCollection_Array1<int>&                            theUMults,
  const NCollection_Array1<double>&                         theUFlatKnots,
  const NCollection_Array1<double>&                         theVKnots,
  const NCollection_Array1<int>&                            theVMults,
  const NCollection_Array1<double>&                         theVFlatKnots)
{
  NCollection_Array2<gp_Pnt> aPoles(1,
                                    theGuideParameters.Length(),
                                    1,
                                    theProfileParameters.Length());
  for (int aGuideIdx = 1; aGuideIdx <= theGuideParameters.Length(); ++aGuideIdx)
  {
    for (int aProfileIdx = 1; aProfileIdx <= theProfileParameters.Length(); ++aProfileIdx)
    {
      aPoles(aGuideIdx, aProfileIdx) =
        theProfiles(aProfileIdx)->Value(theGuideParameters(aGuideIdx));
    }
  }

  int anInversionProblem = 0;
  BSplSLib::Interpolate(theUDegree,
                        theVDegree,
                        theUFlatKnots,
                        theVFlatKnots,
                        theGuideParameters,
                        theProfileParameters,
                        aPoles,
                        anInversionProblem);
  if (anInversionProblem != 0)
  {
    return nullptr;
  }

  return new Geom_BSplineSurface(aPoles,
                                 theUKnots,
                                 theVKnots,
                                 theUMults,
                                 theVMults,
                                 theUDegree,
                                 theVDegree);
}

void appendKnot(const int                         theDegree,
                const double                      theStart,
                const double                      theEnd,
                const double                      theKnot,
                const int                         theMult,
                NCollection_LinearVector<double>& theKnots,
                NCollection_LinearVector<int>&    theMults)
{
  const bool   isBoundary = std::abs(theKnot - theStart) <= Precision::PConfusion()
                            || std::abs(theKnot - theEnd) <= Precision::PConfusion();
  const int    aMaxMult   = isBoundary ? theDegree + 1 : theDegree;
  const int    aMult      = std::min(theMult, aMaxMult);
  const double aKnot      = std::abs(theKnot - theStart) <= Precision::PConfusion() ? theStart
                            : std::abs(theKnot - theEnd) <= Precision::PConfusion() ? theEnd
                                                                                    : theKnot;
  theKnots.Append(aKnot);
  theMults.Append(aMult);
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

  NCollection_LinearVector<double> aKnots;
  NCollection_LinearVector<int>    aMults;

  int aKnotIdx1 = theKnots1.Lower();
  int aKnotIdx2 = theKnots2.Lower();
  while (aKnotIdx1 <= theKnots1.Upper() || aKnotIdx2 <= theKnots2.Upper())
  {
    if (aKnotIdx2 > theKnots2.Upper()
        || (aKnotIdx1 <= theKnots1.Upper()
            && theKnots1(aKnotIdx1) < theKnots2(aKnotIdx2) - Precision::PConfusion()))
    {
      appendKnot(theDegree,
                 aStart,
                 anEnd,
                 theKnots1(aKnotIdx1),
                 theMults1(aKnotIdx1),
                 aKnots,
                 aMults);
      ++aKnotIdx1;
    }
    else if (aKnotIdx1 > theKnots1.Upper()
             || theKnots2(aKnotIdx2) < theKnots1(aKnotIdx1) - Precision::PConfusion())
    {
      appendKnot(theDegree,
                 aStart,
                 anEnd,
                 theKnots2(aKnotIdx2),
                 theMults2(aKnotIdx2),
                 aKnots,
                 aMults);
      ++aKnotIdx2;
    }
    else
    {
      appendKnot(theDegree,
                 aStart,
                 anEnd,
                 theKnots1(aKnotIdx1),
                 std::max(theMults1(aKnotIdx1), theMults2(aKnotIdx2)),
                 aKnots,
                 aMults);
      ++aKnotIdx1;
      ++aKnotIdx2;
    }
  }

  theMergedKnots = new NCollection_HArray1<double>(1, static_cast<int>(aKnots.Size()));
  theMergedMults = new NCollection_HArray1<int>(1, static_cast<int>(aMults.Size()));
  for (size_t aKnotIdx = 0; aKnotIdx < aKnots.Size(); ++aKnotIdx)
  {
    theMergedKnots->ChangeValue(static_cast<int>(aKnotIdx + 1)) = aKnots.Value(aKnotIdx);
    theMergedMults->ChangeValue(static_cast<int>(aKnotIdx + 1)) = aMults.Value(aKnotIdx);
  }
}

bool alignSurfaces(occ::handle<Geom_BSplineSurface>& theProfileSurface,
                   occ::handle<Geom_BSplineSurface>& theGuideSurface,
                   occ::handle<Geom_BSplineSurface>& theTensorSurface)
{
  const int aUDegree = std::max(
    {theProfileSurface->UDegree(), theGuideSurface->UDegree(), theTensorSurface->UDegree()});
  const int aVDegree = std::max(
    {theProfileSurface->VDegree(), theGuideSurface->VDegree(), theTensorSurface->VDegree()});

  theProfileSurface->IncreaseDegree(aUDegree, aVDegree);
  theGuideSurface->IncreaseDegree(aUDegree, aVDegree);
  theTensorSurface->IncreaseDegree(aUDegree, aVDegree);

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
             theTensorSurface->UKnots(),
             theTensorSurface->UMultiplicities(),
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
             theTensorSurface->VKnots(),
             theTensorSurface->VMultiplicities(),
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
  theTensorSurface->InsertUKnots(aUKnots->Array1(),
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
  theTensorSurface->InsertVKnots(aVKnots->Array1(),
                                 aVMults->Array1(),
                                 Precision::PConfusion(),
                                 false);
  return true;
}

occ::handle<Geom_BSplineSurface> makeBooleanSum(
  const occ::handle<Geom_BSplineSurface>& theProfileSurface,
  const occ::handle<Geom_BSplineSurface>& theGuideSurface,
  const occ::handle<Geom_BSplineSurface>& theTensorSurface)
{
  if (theProfileSurface->NbUPoles() != theGuideSurface->NbUPoles()
      || theProfileSurface->NbUPoles() != theTensorSurface->NbUPoles()
      || theProfileSurface->NbVPoles() != theGuideSurface->NbVPoles()
      || theProfileSurface->NbVPoles() != theTensorSurface->NbVPoles())
  {
    return nullptr;
  }

  NCollection_Array2<gp_Pnt> aResultPoles(1,
                                          theProfileSurface->NbUPoles(),
                                          1,
                                          theProfileSurface->NbVPoles());

  for (int aUIdx = 1; aUIdx <= theProfileSurface->NbUPoles(); ++aUIdx)
  {
    for (int aVIdx = 1; aVIdx <= theProfileSurface->NbVPoles(); ++aVIdx)
    {
      const gp_XYZ aPole         = theProfileSurface->Pole(aUIdx, aVIdx).XYZ()
                                   + theGuideSurface->Pole(aUIdx, aVIdx).XYZ()
                                   - theTensorSurface->Pole(aUIdx, aVIdx).XYZ();
      aResultPoles(aUIdx, aVIdx) = gp_Pnt(aPole);
    }
  }

  return new Geom_BSplineSurface(aResultPoles,
                                 theProfileSurface->UKnots(),
                                 theProfileSurface->VKnots(),
                                 theProfileSurface->UMultiplicities(),
                                 theProfileSurface->VMultiplicities(),
                                 theProfileSurface->UDegree(),
                                 theProfileSurface->VDegree());
}

bool canSetUPeriodic(const occ::handle<Geom_BSplineSurface>& theSurface, const double theTolerance)
{
  for (int aVIdx = 1; aVIdx <= theSurface->NbVPoles(); ++aVIdx)
  {
    if (theSurface->Pole(1, aVIdx).Distance(theSurface->Pole(theSurface->NbUPoles(), aVIdx))
        > theTolerance)
    {
      return false;
    }
  }
  return true;
}

bool canSetVPeriodic(const occ::handle<Geom_BSplineSurface>& theSurface, const double theTolerance)
{
  for (int aUIdx = 1; aUIdx <= theSurface->NbUPoles(); ++aUIdx)
  {
    if (theSurface->Pole(aUIdx, 1).Distance(theSurface->Pole(aUIdx, theSurface->NbVPoles()))
        > theTolerance)
    {
      return false;
    }
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
  double                                                    theTolerance,
  bool                                                      theIsUClosed,
  bool                                                      theIsVClosed)
{
  myProfiles          = NCollection_Array1<occ::handle<Geom_BSplineCurve>>(theProfiles);
  myGuides            = NCollection_Array1<occ::handle<Geom_BSplineCurve>>(theGuides);
  myProfileParameters = NCollection_Array1<double>(theProfileParameters);
  myGuideParameters   = NCollection_Array1<double>(theGuideParameters);
  myTolerance         = theTolerance;
  myIsUClosed         = theIsUClosed;
  myIsVClosed         = theIsVClosed;
  myIsDone            = false;
  mySurface.Nullify();
}

//=================================================================================================

void GeomFill_NetworkSurface::Perform()
{
  myIsDone = false;
  mySurface.Nullify();

  if (myProfiles.Length() < 2 || myGuides.Length() < 2)
  {
    return;
  }
  if (myProfiles.Length() != myProfileParameters.Length()
      || myGuides.Length() != myGuideParameters.Length())
  {
    return;
  }
  if (!checkParameters(myProfileParameters) || !checkParameters(myGuideParameters))
  {
    return;
  }
  if (!sameCurveSpace(myProfiles) || !sameCurveSpace(myGuides))
  {
    return;
  }

  try
  {
    int                        aUDegree = 0;
    NCollection_Array1<double> aUKnots;
    NCollection_Array1<int>    aUMults;
    NCollection_Array1<double> aUFlatKnots;
    makeInterpolationBasis(myGuideParameters, aUDegree, aUKnots, aUMults, aUFlatKnots);

    int                        aVDegree = 0;
    NCollection_Array1<double> aVKnots;
    NCollection_Array1<int>    aVMults;
    NCollection_Array1<double> aVFlatKnots;
    makeInterpolationBasis(myProfileParameters, aVDegree, aVKnots, aVMults, aVFlatKnots);

    occ::handle<Geom_BSplineSurface> aProfileSurface =
      makeProfileSkin(myProfiles, myProfileParameters, aVDegree, aVKnots, aVMults, aVFlatKnots);
    occ::handle<Geom_BSplineSurface> aGuideSurface =
      makeGuideSkin(myGuides, myGuideParameters, aUDegree, aUKnots, aUMults, aUFlatKnots);
    occ::handle<Geom_BSplineSurface> aTensorSurface = makeTensorSurface(myProfiles,
                                                                        myProfileParameters,
                                                                        myGuideParameters,
                                                                        aUDegree,
                                                                        aVDegree,
                                                                        aUKnots,
                                                                        aUMults,
                                                                        aUFlatKnots,
                                                                        aVKnots,
                                                                        aVMults,
                                                                        aVFlatKnots);
    if (aProfileSurface.IsNull() || aGuideSurface.IsNull() || aTensorSurface.IsNull())
    {
      return;
    }

    if (!alignSurfaces(aProfileSurface, aGuideSurface, aTensorSurface))
    {
      return;
    }

    mySurface = makeBooleanSum(aProfileSurface, aGuideSurface, aTensorSurface);
    if (mySurface.IsNull())
    {
      return;
    }

    if (myIsUClosed)
    {
      if (!canSetUPeriodic(mySurface, std::max(myTolerance, Precision::Confusion())))
      {
        mySurface.Nullify();
        return;
      }
      mySurface->SetUPeriodic();
    }
    if (myIsVClosed)
    {
      if (!canSetVPeriodic(mySurface, std::max(myTolerance, Precision::Confusion())))
      {
        mySurface.Nullify();
        return;
      }
      mySurface->SetVPeriodic();
    }
    reparametrizeSurface(mySurface);
  }
  catch (Standard_Failure const&)
  {
    mySurface.Nullify();
    return;
  }

  myIsDone = true;
}

//=================================================================================================

const occ::handle<Geom_BSplineSurface>& GeomFill_NetworkSurface::Surface() const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone("GeomFill_NetworkSurface::Surface");
  }
  return mySurface;
}
