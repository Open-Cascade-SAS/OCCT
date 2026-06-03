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

bool samePoleTable(const occ::handle<Geom_BSplineSurface>& theSurface1,
                   const occ::handle<Geom_BSplineSurface>& theSurface2)
{
  return theSurface1->NbUPoles() == theSurface2->NbUPoles()
         && theSurface1->NbVPoles() == theSurface2->NbVPoles();
}

//! Moves the copied profile poles by the guide correction measured from the reference surface.
void applyGuideDeviation(const occ::handle<Geom_BSplineSurface>& theGuideSurface,
                         const occ::handle<Geom_BSplineSurface>& theReferenceSurface,
                         NCollection_Array2<gp_Pnt>&             thePoles)
{
  for (int aUIdx = thePoles.LowerRow(); aUIdx <= thePoles.UpperRow(); ++aUIdx)
  {
    for (int aVIdx = thePoles.LowerCol(); aVIdx <= thePoles.UpperCol(); ++aVIdx)
    {
      gp_Pnt aPole = thePoles.Value(aUIdx, aVIdx);
      aPole.Translate(
        gp_Vec(theReferenceSurface->Pole(aUIdx, aVIdx), theGuideSurface->Pole(aUIdx, aVIdx)));
      thePoles.SetValue(aUIdx, aVIdx, aPole);
    }
  }
}

//! Builds the final network surface by correcting the profile skin with guide deviations.
occ::handle<Geom_BSplineSurface> makeCorrectedProfileSkin(
  const occ::handle<Geom_BSplineSurface>& theProfileSurface,
  const occ::handle<Geom_BSplineSurface>& theGuideSurface,
  const occ::handle<Geom_BSplineSurface>& theReferenceSurface)
{
  if (!samePoleTable(theProfileSurface, theGuideSurface)
      || !samePoleTable(theProfileSurface, theReferenceSurface))
  {
    return nullptr;
  }

  NCollection_Array2<gp_Pnt> aResultPoles(theProfileSurface->Poles());
  applyGuideDeviation(theGuideSurface, theReferenceSurface, aResultPoles);

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

bool isReadyToBuild(const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theProfiles,
                    const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theGuides,
                    const NCollection_Array1<double>&                         theProfileParameters,
                    const NCollection_Array1<double>&                         theGuideParameters)
{
  return theProfiles.Length() >= 2 && theGuides.Length() >= 2
         && theProfiles.Length() == theProfileParameters.Length()
         && theGuides.Length() == theGuideParameters.Length()
         && checkParameters(theProfileParameters) && checkParameters(theGuideParameters)
         && sameCurveSpace(theProfiles) && sameCurveSpace(theGuides);
}

//! Temporary surfaces sharing one Gordon network before final correction.
struct NetworkSkins
{
  occ::handle<Geom_BSplineSurface> Profile;
  occ::handle<Geom_BSplineSurface> Guide;
  occ::handle<Geom_BSplineSurface> Reference;

  bool Init(const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theProfiles,
            const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theGuides,
            const NCollection_Array1<double>&                         theProfileParameters,
            const NCollection_Array1<double>&                         theGuideParameters,
            const SkinningBasis&                                      theUBasis,
            const SkinningBasis&                                      theVBasis)
  {
    Profile = makeProfileSkin(theProfiles,
                              theProfileParameters,
                              theVBasis.Degree,
                              theVBasis.Knots,
                              theVBasis.Mults,
                              theVBasis.FlatKnots);
    if (Profile.IsNull())
    {
      return false;
    }

    Guide = makeGuideSkin(theGuides,
                          theGuideParameters,
                          theUBasis.Degree,
                          theUBasis.Knots,
                          theUBasis.Mults,
                          theUBasis.FlatKnots);
    if (Guide.IsNull())
    {
      return false;
    }

    NCollection_Array2<gp_Pnt> aReferencePoles(1,
                                               theGuideParameters.Length(),
                                               1,
                                               theProfileParameters.Length());
    for (int aGuideIdx = 1; aGuideIdx <= theGuideParameters.Length(); ++aGuideIdx)
    {
      for (int aProfileIdx = 1; aProfileIdx <= theProfileParameters.Length(); ++aProfileIdx)
      {
        aReferencePoles(aGuideIdx, aProfileIdx) =
          theProfiles(aProfileIdx)->Value(theGuideParameters(aGuideIdx));
      }
    }

    int anInversionProblem = 0;
    BSplSLib::Interpolate(theUBasis.Degree,
                          theVBasis.Degree,
                          theUBasis.FlatKnots,
                          theVBasis.FlatKnots,
                          theGuideParameters,
                          theProfileParameters,
                          aReferencePoles,
                          anInversionProblem);
    if (anInversionProblem != 0)
    {
      return false;
    }

    Reference = new Geom_BSplineSurface(aReferencePoles,
                                        theUBasis.Knots,
                                        theVBasis.Knots,
                                        theUBasis.Mults,
                                        theVBasis.Mults,
                                        theUBasis.Degree,
                                        theVBasis.Degree);
    return true;
  }

  bool PrepareCommonBasis() { return alignSurfaces(Profile, Guide, Reference); }

  occ::handle<Geom_BSplineSurface> CorrectedProfile() const
  {
    return makeCorrectedProfileSkin(Profile, Guide, Reference);
  }
};

occ::handle<Geom_BSplineSurface> makeNetworkSurface(
  const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theProfiles,
  const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theGuides,
  const NCollection_Array1<double>&                         theProfileParameters,
  const NCollection_Array1<double>&                         theGuideParameters)
{
  SkinningBasis anUBasis;
  anUBasis.Init(theGuideParameters);

  SkinningBasis aVBasis;
  aVBasis.Init(theProfileParameters);

  NetworkSkins aSkins;
  if (!aSkins
         .Init(theProfiles, theGuides, theProfileParameters, theGuideParameters, anUBasis, aVBasis)
      || !aSkins.PrepareCommonBasis())
  {
    return nullptr;
  }

  return aSkins.CorrectedProfile();
}

bool applyPeriodicity(occ::handle<Geom_BSplineSurface>& theSurface,
                      const bool                        theIsUClosed,
                      const bool                        theIsVClosed,
                      const double                      theTolerance)
{
  const double aTolerance = std::max(theTolerance, Precision::Confusion());
  if (theIsUClosed)
  {
    if (!canSetUPeriodic(theSurface, aTolerance))
    {
      return false;
    }
    theSurface->SetUPeriodic();
  }

  if (theIsVClosed)
  {
    if (!canSetVPeriodic(theSurface, aTolerance))
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
  myStatus            = ResultStatus::NotStarted;
  mySurface.Nullify();
}

//=================================================================================================

void GeomFill_NetworkSurface::Perform()
{
  myStatus = ResultStatus::NotStarted;
  mySurface.Nullify();

  if (!isReadyToBuild(myProfiles, myGuides, myProfileParameters, myGuideParameters))
  {
    myStatus = ResultStatus::InvalidInput;
    return;
  }

  try
  {
    mySurface = makeNetworkSurface(myProfiles, myGuides, myProfileParameters, myGuideParameters);
    if (mySurface.IsNull())
    {
      myStatus = ResultStatus::ConstructionFailed;
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
