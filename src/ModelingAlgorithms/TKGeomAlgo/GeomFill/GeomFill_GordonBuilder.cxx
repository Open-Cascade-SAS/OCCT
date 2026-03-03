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

#include <GeomFill_GordonBuilder.hxx>

#include <BSplCLib.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray1.hxx>
#include <OSD_Parallel.hxx>
#include <Precision.hxx>
#include <StdFail_NotDone.hxx>

#include <atomic>

//=================================================================================================

GeomFill_GordonBuilder::GeomFill_GordonBuilder() {}

//=================================================================================================

void GeomFill_GordonBuilder::Init(
  const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theProfiles,
  const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theGuides,
  const NCollection_Array1<double>&                         theProfileParams,
  const NCollection_Array1<double>&                         theGuideParams,
  double                                                    theTolerance,
  bool                                                      theIsUClosed,
  bool                                                      theIsVClosed)
{
  myProfiles      = NCollection_Array1<occ::handle<Geom_BSplineCurve>>(theProfiles);
  myGuides        = NCollection_Array1<occ::handle<Geom_BSplineCurve>>(theGuides);
  myProfileParams = NCollection_Array1<double>(theProfileParams);
  myGuideParams   = NCollection_Array1<double>(theGuideParams);
  myTolerance     = theTolerance;
  myIsUClosed     = theIsUClosed;
  myIsVClosed     = theIsVClosed;
  myIsDone        = false;
  mySurface.Nullify();
  myProfileSurface.Nullify();
  myGuideSurface.Nullify();
  myTensorSurface.Nullify();
}

//=================================================================================================

void GeomFill_GordonBuilder::Perform()
{
  myIsDone = false;

  if (myProfiles.Length() < 2 || myGuides.Length() < 2)
  {
    return;
  }

  if (myProfileParams.Length() != myProfiles.Length()
      || myGuideParams.Length() != myGuides.Length())
  {
    return;
  }

  // Reject rational curves (v1 limitation).
  for (int i = myProfiles.Lower(); i <= myProfiles.Upper(); ++i)
  {
    if (myProfiles(i).IsNull() || myProfiles(i)->IsRational())
    {
      return;
    }
  }
  for (int i = myGuides.Lower(); i <= myGuides.Upper(); ++i)
  {
    if (myGuides(i).IsNull() || myGuides(i)->IsRational())
    {
      return;
    }
  }

  // Extract intersection points by evaluating profiles at guide parameters.
  // intersection_pnts(aGuideIdx, aProfileIdx) =
  //   profile[aProfileIdx]->Value(guideParam[aGuideIdx])
  const int aNbGuides   = myGuides.Length();
  const int aNbProfiles = myProfiles.Length();

  NCollection_Array2<gp_Pnt> anIntersectionPnts(1, aNbGuides, 1, aNbProfiles);
  for (int aProfileIdx = 0; aProfileIdx < aNbProfiles; ++aProfileIdx)
  {
    const occ::handle<Geom_BSplineCurve>& aProfile = myProfiles(myProfiles.Lower() + aProfileIdx);
    for (int aGuideIdx = 0; aGuideIdx < aNbGuides; ++aGuideIdx)
    {
      double aParam = myGuideParams(myGuideParams.Lower() + aGuideIdx);
      anIntersectionPnts(aGuideIdx + 1, aProfileIdx + 1) = aProfile->Value(aParam);
    }
  }

  // Step 1: Skin profiles -> S_profiles (skinning in V-direction).
  // Profiles are skinned along the V parameter; closedness in V means periodic skinning.
  myProfileSurface = buildSkinSurface(myProfiles, myProfileParams, myIsVClosed);
  if (myProfileSurface.IsNull())
  {
    return;
  }

  // Step 2: Skin guides -> S_guides (skinning in U-direction), then flip U<->V.
  // Guides are skinned along the U parameter; closedness in U means periodic skinning.
  myGuideSurface = buildSkinSurface(myGuides, myGuideParams, myIsUClosed);
  if (myGuideSurface.IsNull())
  {
    return;
  }
  myGuideSurface->ExchangeUV();

  // Step 3: Build tensor product surface from intersection grid.
  myTensorSurface = buildTensorSurface(anIntersectionPnts,
                                       myGuideParams,
                                       myProfileParams,
                                       myIsUClosed,
                                       myIsVClosed);
  if (myTensorSurface.IsNull())
  {
    return;
  }

  // Step 4: Unify all three surfaces to common degrees and knots.
  unifySurfaces(myProfileSurface, myGuideSurface);
  unifySurfaces(myProfileSurface, myTensorSurface);
  unifySurfaces(myGuideSurface, myTensorSurface);

  // Step 5: Boolean sum.
  mySurface = computeBooleanSum(myProfileSurface, myGuideSurface, myTensorSurface);
  if (mySurface.IsNull())
  {
    return;
  }

  myIsDone = true;
}

//=================================================================================================

const occ::handle<Geom_BSplineSurface>& GeomFill_GordonBuilder::Surface() const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone("GeomFill_GordonBuilder::Surface");
  }
  return mySurface;
}

//=================================================================================================

const occ::handle<Geom_BSplineSurface>& GeomFill_GordonBuilder::ProfileSurface() const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone("GeomFill_GordonBuilder::ProfileSurface");
  }
  return myProfileSurface;
}

//=================================================================================================

const occ::handle<Geom_BSplineSurface>& GeomFill_GordonBuilder::GuideSurface() const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone("GeomFill_GordonBuilder::GuideSurface");
  }
  return myGuideSurface;
}

//=================================================================================================

const occ::handle<Geom_BSplineSurface>& GeomFill_GordonBuilder::TensorSurface() const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone("GeomFill_GordonBuilder::TensorSurface");
  }
  return myTensorSurface;
}

//=================================================================================================

occ::handle<Geom_BSplineSurface> GeomFill_GordonBuilder::buildSkinSurface(
  const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theSections,
  const NCollection_Array1<double>&                         theSectionParams,
  bool                                                      theIsClosed) const
{
  if (theSections.Length() < 2)
  {
    return nullptr;
  }

  // All sections must have the same number of poles (they are already compatible).
  const int aNbPolesU   = theSections(theSections.Lower())->NbPoles();
  const int aNbSections = theSections.Length();

  // Verify closedness: first and last sections must be geometrically identical.
  bool aMakeClosed = theIsClosed;
  if (aMakeClosed)
  {
    const occ::handle<Geom_BSplineCurve>& aFirst = theSections(theSections.Lower());
    const occ::handle<Geom_BSplineCurve>& aLast  = theSections(theSections.Upper());
    if (!aFirst->IsEqual(aLast, Precision::Confusion()))
    {
      aMakeClosed = false;
    }
  }

  // For periodic interpolation, GeomAPI_Interpolate requires one extra parameter
  // (the wrap-around parameter for the junction point).
  // Points: use only N-1 sections for periodic (last = first is implicit).
  const int aNbInterpPts = aMakeClosed ? aNbSections - 1 : aNbSections;
  const int aNbParamPts  = aMakeClosed ? aNbSections : aNbSections;

  occ::handle<NCollection_HArray1<double>> aSectionParamsH =
    new NCollection_HArray1<double>(1, aNbParamPts);
  for (int i = 0; i < aNbParamPts; ++i)
  {
    aSectionParamsH->SetValue(i + 1, theSectionParams(theSectionParams.Lower() + i));
  }

  // Interpolate the first pole column to determine V-direction knots/mults/degree.
  occ::handle<NCollection_HArray1<gp_Pnt>> aFirstPnts =
    new NCollection_HArray1<gp_Pnt>(1, aNbInterpPts);
  for (int aSectionIdx = 0; aSectionIdx < aNbInterpPts; ++aSectionIdx)
  {
    aFirstPnts->SetValue(aSectionIdx + 1, theSections(theSections.Lower() + aSectionIdx)->Pole(1));
  }

  GeomAPI_Interpolate aFirstInterp(aFirstPnts,
                                   aSectionParamsH,
                                   aMakeClosed,
                                   Precision::Confusion());
  aFirstInterp.Perform();
  if (!aFirstInterp.IsDone())
  {
    return nullptr;
  }

  occ::handle<Geom_BSplineCurve> aFirstInterpCurve = aFirstInterp.Curve();
  const int                      aNbPolesV         = aFirstInterpCurve->NbPoles();

  // Pre-allocate surface poles array and store first column.
  NCollection_Array2<gp_Pnt> aSurfPoles(1, aNbPolesU, 1, aNbPolesV);
  for (int j = 1; j <= aNbPolesV; ++j)
  {
    aSurfPoles.SetValue(1, j, aFirstInterpCurve->Pole(j));
  }

  // Interpolate remaining pole columns in parallel.
  std::atomic<bool> isOk{true};

  OSD_Parallel::For(
    2,
    aNbPolesU + 1,
    [&](int aUPoleIdx) {
      if (!isOk.load(std::memory_order_relaxed))
      {
        return;
      }

      occ::handle<NCollection_HArray1<gp_Pnt>> anInterpPnts =
        new NCollection_HArray1<gp_Pnt>(1, aNbInterpPts);
      for (int aSectionIdx = 0; aSectionIdx < aNbInterpPts; ++aSectionIdx)
      {
        anInterpPnts->SetValue(aSectionIdx + 1,
                               theSections(theSections.Lower() + aSectionIdx)->Pole(aUPoleIdx));
      }

      GeomAPI_Interpolate anInterp(anInterpPnts,
                                   aSectionParamsH,
                                   aMakeClosed,
                                   Precision::Confusion());
      anInterp.Perform();
      if (!anInterp.IsDone())
      {
        isOk.store(false, std::memory_order_relaxed);
        return;
      }

      occ::handle<Geom_BSplineCurve> anInterpCurve = anInterp.Curve();
      for (int j = 1; j <= anInterpCurve->NbPoles(); ++j)
      {
        aSurfPoles.SetValue(aUPoleIdx, j, anInterpCurve->Pole(j));
      }
    },
    !myToUseParallel);

  if (!isOk.load())
  {
    return nullptr;
  }

  // Construct the surface.
  // U-direction knots/mults come from the first section curve.
  // V-direction knots/mults come from the first interpolation curve.
  const occ::handle<Geom_BSplineCurve>& aFirstSection = theSections(theSections.Lower());

  return new Geom_BSplineSurface(aSurfPoles,
                                 aFirstSection->Knots(),
                                 aFirstInterpCurve->Knots(),
                                 aFirstSection->Multiplicities(),
                                 aFirstInterpCurve->Multiplicities(),
                                 aFirstSection->Degree(),
                                 aFirstInterpCurve->Degree());
}

//=================================================================================================

occ::handle<Geom_BSplineSurface> GeomFill_GordonBuilder::buildTensorSurface(
  const NCollection_Array2<gp_Pnt>& thePoints,
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams,
  bool                              theIsUClosed,
  bool                              theIsVClosed) const
{
  const int aNbU = thePoints.NbRows();
  const int aNbV = thePoints.NbColumns();

  if (aNbU < 2 || aNbV < 2)
  {
    return nullptr;
  }

  // For periodic U-interpolation, use N-1 points with N parameters.
  bool aMakeUClosed = theIsUClosed && aNbU >= 3;
  if (aMakeUClosed)
  {
    // Verify first and last rows are geometrically identical.
    for (int aVIdx = 1; aVIdx <= aNbV; ++aVIdx)
    {
      if (!thePoints(thePoints.LowerRow(), thePoints.LowerCol() + aVIdx - 1)
             .IsEqual(thePoints(thePoints.LowerRow() + aNbU - 1, thePoints.LowerCol() + aVIdx - 1),
                      Precision::Confusion()))
      {
        aMakeUClosed = false;
        break;
      }
    }
  }

  const int aNbInterpU = aMakeUClosed ? aNbU - 1 : aNbU;
  const int aNbParamU  = aMakeUClosed ? aNbU : aNbU;

  // Prepare parameter arrays for GeomAPI_Interpolate.
  occ::handle<NCollection_HArray1<double>> aUParamsH =
    new NCollection_HArray1<double>(1, aNbParamU);
  for (int i = 0; i < aNbParamU; ++i)
  {
    aUParamsH->SetValue(i + 1, theUParams(theUParams.Lower() + i));
  }

  // Interpolate each row (fixed V-index, varying U) to produce curves in U-direction.
  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aRowCurves(1, aNbV);
  for (int aVIdx = 1; aVIdx <= aNbV; ++aVIdx)
  {
    occ::handle<NCollection_HArray1<gp_Pnt>> aRowPnts =
      new NCollection_HArray1<gp_Pnt>(1, aNbInterpU);
    for (int aUIdx = 1; aUIdx <= aNbInterpU; ++aUIdx)
    {
      aRowPnts->SetValue(
        aUIdx,
        thePoints(thePoints.LowerRow() + aUIdx - 1, thePoints.LowerCol() + aVIdx - 1));
    }

    GeomAPI_Interpolate anInterp(aRowPnts, aUParamsH, aMakeUClosed, Precision::Confusion());
    anInterp.Perform();
    if (!anInterp.IsDone())
    {
      return nullptr;
    }

    aRowCurves(aVIdx) = anInterp.Curve();
  }

  // Make row curves compatible (same degree & knots) before skinning.
  int aMaxDegree = 0;
  for (int i = aRowCurves.Lower(); i <= aRowCurves.Upper(); ++i)
  {
    aMaxDegree = std::max(aMaxDegree, aRowCurves(i)->Degree());
  }
  for (int i = aRowCurves.Lower(); i <= aRowCurves.Upper(); ++i)
  {
    aRowCurves(i)->IncreaseDegree(aMaxDegree);
  }

  // Unify knot vectors across all row curves.
  for (int i = aRowCurves.Lower() + 1; i <= aRowCurves.Upper(); ++i)
  {
    NCollection_Array1<double> aKnots(aRowCurves(i)->Knots());
    NCollection_Array1<int>    aMults(aRowCurves(i)->Multiplicities());
    aRowCurves(aRowCurves.Lower())->InsertKnots(aKnots, aMults, Precision::PConfusion(), false);
  }
  const NCollection_Array1<double>& aUnifiedKnots = aRowCurves(aRowCurves.Lower())->Knots();
  const NCollection_Array1<int>& aUnifiedMults = aRowCurves(aRowCurves.Lower())->Multiplicities();
  for (int i = aRowCurves.Lower() + 1; i <= aRowCurves.Upper(); ++i)
  {
    aRowCurves(i)->InsertKnots(aUnifiedKnots, aUnifiedMults, Precision::PConfusion(), false);
  }

  // Skin row curves in V-direction, passing V-closedness.
  return buildSkinSurface(aRowCurves, theVParams, theIsVClosed);
}

//=================================================================================================

void GeomFill_GordonBuilder::unifySurfaces(occ::handle<Geom_BSplineSurface>& theSurf1,
                                           occ::handle<Geom_BSplineSurface>& theSurf2)
{
  // Raise degrees to maximum in both directions.
  const int aDegU = std::max(theSurf1->UDegree(), theSurf2->UDegree());
  const int aDegV = std::max(theSurf1->VDegree(), theSurf2->VDegree());
  theSurf1->IncreaseDegree(aDegU, aDegV);
  theSurf2->IncreaseDegree(aDegU, aDegV);

  // Unify U-knots: snapshot both knot vectors first, then cross-insert.
  // Copies are needed because InsertUKnots invalidates the surface's internal arrays.
  // Two passes suffice: after inserting surf2's knots into surf1, surf1 has the union.
  // Inserting surf1's original knots into surf2 ensures surf2 also has the union,
  // because InsertUKnots with Add=false uses max(existing, requested) multiplicities.
  {
    NCollection_Array1<double> aKnots1(theSurf1->UKnots());
    NCollection_Array1<int>    aMults1(theSurf1->UMultiplicities());
    NCollection_Array1<double> aKnots2(theSurf2->UKnots());
    NCollection_Array1<int>    aMults2(theSurf2->UMultiplicities());

    theSurf1->InsertUKnots(aKnots2, aMults2, Precision::PConfusion(), false);
    theSurf2->InsertUKnots(aKnots1, aMults1, Precision::PConfusion(), false);
  }

  // Unify V-knots: same two-pass approach.
  {
    NCollection_Array1<double> aKnots1(theSurf1->VKnots());
    NCollection_Array1<int>    aMults1(theSurf1->VMultiplicities());
    NCollection_Array1<double> aKnots2(theSurf2->VKnots());
    NCollection_Array1<int>    aMults2(theSurf2->VMultiplicities());

    theSurf1->InsertVKnots(aKnots2, aMults2, Precision::PConfusion(), false);
    theSurf2->InsertVKnots(aKnots1, aMults1, Precision::PConfusion(), false);
  }
}

//=================================================================================================

occ::handle<Geom_BSplineSurface> GeomFill_GordonBuilder::computeBooleanSum(
  const occ::handle<Geom_BSplineSurface>& theProfileSurf,
  const occ::handle<Geom_BSplineSurface>& theGuideSurf,
  const occ::handle<Geom_BSplineSurface>& theTensorSurf) const
{
  occ::handle<Geom_BSplineSurface> aResult =
    occ::handle<Geom_BSplineSurface>::DownCast(theProfileSurf->Copy());

  const int aNbUPoles = aResult->NbUPoles();
  const int aNbVPoles = aResult->NbVPoles();

  // Access weight arrays (for non-rational surfaces, these are all 1.0 via BSplCLib).
  const NCollection_Array2<double>& aProfWeights   = theProfileSurf->WeightsArray();
  const NCollection_Array2<double>& aGuideWeights  = theGuideSurf->WeightsArray();
  const NCollection_Array2<double>& aTensorWeights = theTensorSurf->WeightsArray();

  // Boolean sum: S_gordon = S_profiles + S_guides - S_tensor.
  // For rational surfaces, the formula applies in weighted (homogeneous) coordinates:
  //   w_gordon * P_gordon = w_prof * P_prof + w_guide * P_guide - w_tensor * P_tensor
  //   w_gordon = w_prof + w_guide - w_tensor
  // For non-rational surfaces (all weights = 1.0), this simplifies to P + G - T.
  for (int aUIdx = 1; aUIdx <= aNbUPoles; ++aUIdx)
  {
    for (int aVIdx = 1; aVIdx <= aNbVPoles; ++aVIdx)
    {
      const double aWProf   = aProfWeights(aUIdx, aVIdx);
      const double aWGuide  = aGuideWeights(aUIdx, aVIdx);
      const double aWTensor = aTensorWeights(aUIdx, aVIdx);
      const double aWGordon = aWProf + aWGuide - aWTensor;
      if (std::abs(aWGordon) < Precision::Confusion())
      {
        return nullptr;
      }

      gp_XYZ aProfPole   = theProfileSurf->Pole(aUIdx, aVIdx).XYZ() * aWProf;
      gp_XYZ aGuidePole  = theGuideSurf->Pole(aUIdx, aVIdx).XYZ() * aWGuide;
      gp_XYZ aTensorPole = theTensorSurf->Pole(aUIdx, aVIdx).XYZ() * aWTensor;

      gp_XYZ aGordonPole = (aProfPole + aGuidePole - aTensorPole) / aWGordon;
      aResult->SetPole(aUIdx, aVIdx, gp_Pnt(aGordonPole));
      aResult->SetWeight(aUIdx, aVIdx, aWGordon);
    }
  }

  return aResult;
}
