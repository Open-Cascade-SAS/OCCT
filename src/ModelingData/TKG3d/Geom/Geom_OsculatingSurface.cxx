// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include "Geom_OsculatingSurface.pxx"

#include <BSplSLib.hxx>
#include <Convert_GridPolynomialToPoles.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <NCollection_LocalArray.hxx>
#include <PLib.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_Array1.hxx>

#include <algorithm>
#include <cstddef>
#include <utility>

namespace
{
//! Clears osculating-surface flags unless initialization completes successfully.
class OsculatingSurfaceInitGuard
{
public:
  OsculatingSurfaceInitGuard(std::array<bool, 4>& theFlags, bool& theIsDone)
      : myFlags(theFlags),
        myIsDone(theIsDone)
  {
  }

  ~OsculatingSurfaceInitGuard()
  {
    if (!myIsDone)
    {
      myFlags.fill(false);
    }
  }

private:
  std::array<bool, 4>& myFlags;
  bool&                myIsDone;
};
} // namespace

//=================================================================================================

Geom_OsculatingSurface::Geom_OsculatingSurface()
    : myTol(0.0),
      myAlong{false, false, false, false}
{
}

//=================================================================================================

Geom_OsculatingSurface::Geom_OsculatingSurface(const occ::handle<Geom_Surface>& theBS,
                                               double                           theTol)
    : myAlong{false, false, false, false}
{
  Init(theBS, theTol);
}

//=================================================================================================

Geom_OsculatingSurface::Geom_OsculatingSurface(const Geom_OsculatingSurface& theOther)

  = default;

//=================================================================================================

Geom_OsculatingSurface::Geom_OsculatingSurface(Geom_OsculatingSurface&& theOther) noexcept
    : myBasisSurf(std::move(theOther.myBasisSurf)),
      myTol(theOther.myTol),
      myOsculSurf1(std::move(theOther.myOsculSurf1)),
      myOsculSurf2(std::move(theOther.myOsculSurf2)),
      myAlong(theOther.myAlong)
{
  theOther.myTol = 0.0;
  theOther.myAlong.fill(false);
}

//=================================================================================================

Geom_OsculatingSurface& Geom_OsculatingSurface::operator=(const Geom_OsculatingSurface& theOther)
{
  if (this != &theOther)
  {
    myBasisSurf  = theOther.myBasisSurf;
    myTol        = theOther.myTol;
    myOsculSurf1 = theOther.myOsculSurf1;
    myOsculSurf2 = theOther.myOsculSurf2;
    myAlong      = theOther.myAlong;
  }
  return *this;
}

//=================================================================================================

Geom_OsculatingSurface& Geom_OsculatingSurface::operator=(
  Geom_OsculatingSurface&& theOther) noexcept
{
  if (this != &theOther)
  {
    myBasisSurf  = std::move(theOther.myBasisSurf);
    myTol        = theOther.myTol;
    myOsculSurf1 = std::move(theOther.myOsculSurf1);
    myOsculSurf2 = std::move(theOther.myOsculSurf2);
    myAlong      = theOther.myAlong;

    theOther.myTol = 0.0;
    theOther.myAlong.fill(false);
  }
  return *this;
}

//=================================================================================================

void Geom_OsculatingSurface::Init(const occ::handle<Geom_Surface>& theBS, double theTol)
{
  myAlong.fill(false);
  bool                       aIsDone = false;
  OsculatingSurfaceInitGuard aInitGuard(myAlong, aIsDone);

  myTol = theTol;
  myBasisSurf.Nullify();
  myOsculSurf1.Clear();
  myOsculSurf2.Clear();

  if (theBS.IsNull())
  {
    return;
  }

  myBasisSurf = occ::down_cast<Geom_Surface>(theBS->Copy());

  const bool aIsBSpline = theBS->IsKind(STANDARD_TYPE(Geom_BSplineSurface));
  const bool aIsBezier  = theBS->IsKind(STANDARD_TYPE(Geom_BezierSurface));
  if (!aIsBSpline && !aIsBezier)
  {
    return;
  }

  // Consider all singularities below myTol, not just above 1.e-12.
  constexpr double THE_TOL_MIN = 0.0;

  double aU1 = 0.0;
  double aU2 = 0.0;
  double aV1 = 0.0;
  double aV2 = 0.0;
  theBS->Bounds(aU1, aU2, aV1, aV2);
  const GeomAdaptor_Surface anAdaptor(theBS, aU1, aU2, aV1, aV2);
  myAlong[0] = isQPunctual(anAdaptor, aV1, GeomAbs_IsoV, THE_TOL_MIN, theTol);
  myAlong[1] = isQPunctual(anAdaptor, aV2, GeomAbs_IsoV, THE_TOL_MIN, theTol);
  myAlong[2] = isQPunctual(anAdaptor, aU1, GeomAbs_IsoU, THE_TOL_MIN, theTol);
  myAlong[3] = isQPunctual(anAdaptor, aU2, GeomAbs_IsoU, THE_TOL_MIN, theTol);

  if (!HasOscSurf())
  {
    return;
  }

  // Promote Bezier to BSpline so the iso-scan can use the shared knot indexing.
  occ::handle<Geom_BSplineSurface> aInitSurf;
  if (aIsBezier)
  {
    const occ::handle<Geom_BezierSurface> aBzS = occ::down_cast<Geom_BezierSurface>(theBS);
    NCollection_LocalArray<double, 2>     aUKnotStorage(2);
    NCollection_LocalArray<double, 2>     aVKnotStorage(2);
    NCollection_LocalArray<int, 2>        aUMultStorage(2);
    NCollection_LocalArray<int, 2>        aVMultStorage(2);
    NCollection_Array1<double>            aUKnots(aUKnotStorage.begin()[0], 1, 2);
    NCollection_Array1<double>            aVKnots(aVKnotStorage.begin()[0], 1, 2);
    NCollection_Array1<int>               aUMults(aUMultStorage.begin()[0], 1, 2);
    NCollection_Array1<int>               aVMults(aVMultStorage.begin()[0], 1, 2);
    for (size_t i = 0; i < 2; ++i)
    {
      aUKnots.ChangeAt(i) = static_cast<double>(i);
      aVKnots.ChangeAt(i) = static_cast<double>(i);
      aUMults.ChangeAt(i) = aBzS->UDegree() + 1;
      aVMults.ChangeAt(i) = aBzS->VDegree() + 1;
    }
    aInitSurf = new Geom_BSplineSurface(aBzS->Poles(),
                                        aUKnots,
                                        aVKnots,
                                        aUMults,
                                        aVMults,
                                        aBzS->UDegree(),
                                        aBzS->VDegree(),
                                        aBzS->IsUPeriodic(),
                                        aBzS->IsVPeriodic());
  }
  else
  {
    aInitSurf = occ::down_cast<Geom_BSplineSurface>(myBasisSurf);
  }

  // Cannot handle simultaneous degeneracy in both directions.
  if (IsAlongU() && IsAlongV())
  {
    return;
  }

  // The reduction needs at least one direction to have degree > 1.
  if ((IsAlongU() && aInitSurf->VDegree() <= 1) || (IsAlongV() && aInitSurf->UDegree() <= 1))
  {
    return;
  }

  const size_t aNbSurfaces = IsAlongU() ? static_cast<size_t>(aInitSurf->NbUKnots() - 1)
                                        : static_cast<size_t>(aInitSurf->NbVKnots() - 1);
  myOsculSurf1.Reserve(aNbSurfaces);
  myOsculSurf2.Reserve(aNbSurfaces);

  if (myAlong[0] || myAlong[1])
  {
    // One reduction per U-span; both V-bound isos share the same span.
    for (int i = 1; i < aInitSurf->NbUKnots(); ++i)
    {
      occ::handle<Geom_BSplineSurface> aSurface;
      size_t                           aNbReductions = 0;
      if (myAlong[0])
      {
        if (!reduceAlongIso(aInitSurf, aV1, GeomAbs_IsoV, i, 1, aSurface, aNbReductions))
        {
          return;
        }
        myOsculSurf1.Append(aSurface);
      }
      if (myAlong[1])
      {
        if (!reduceAlongIso(aInitSurf,
                            aV2,
                            GeomAbs_IsoV,
                            i,
                            aInitSurf->NbVKnots() - 1,
                            aSurface,
                            aNbReductions))
        {
          return;
        }
        appendOsculatingSurface2(aSurface, aNbReductions);
      }
    }
  }
  if (myAlong[2] || myAlong[3])
  {
    // One reduction per V-span; both U-bound isos share the same span.
    for (int i = 1; i < aInitSurf->NbVKnots(); ++i)
    {
      occ::handle<Geom_BSplineSurface> aSurface;
      size_t                           aNbReductions = 0;
      if (myAlong[2])
      {
        if (!reduceAlongIso(aInitSurf, aU1, GeomAbs_IsoU, 1, i, aSurface, aNbReductions))
        {
          return;
        }
        myOsculSurf1.Append(aSurface);
      }
      if (myAlong[3])
      {
        if (!reduceAlongIso(aInitSurf,
                            aU2,
                            GeomAbs_IsoU,
                            aInitSurf->NbUKnots() - 1,
                            i,
                            aSurface,
                            aNbReductions))
        {
          return;
        }
        appendOsculatingSurface2(aSurface, aNbReductions);
      }
    }
  }

  aIsDone = true;
}

//=================================================================================================

bool Geom_OsculatingSurface::reduceAlongIso(const occ::handle<Geom_BSplineSurface>& theInitSurf,
                                            double                                  theParam,
                                            GeomAbs_IsoType                         theIso,
                                            int                                     theInitUKnot,
                                            int                                     theInitVKnot,
                                            occ::handle<Geom_BSplineSurface>&       theSurface,
                                            size_t& theNbReductions) const
{
  theSurface.Nullify();
  theNbReductions = 0;

  occ::handle<Geom_BSplineSurface> aSurface = theInitSurf;
  occ::handle<Geom_BSplineSurface> aReducedSurface;
  int                              aUKnot       = theInitUKnot;
  int                              aVKnot       = theInitVKnot;
  bool                             aIsQPunctual = true;
  GeomAdaptor_Surface              anAdaptor;
  while (aIsQPunctual)
  {
    if (!buildOsculatingSurface(theParam, aUKnot, aVKnot, aSurface, aReducedSurface))
    {
      return false;
    }

    ++theNbReductions;
    anAdaptor.Load(aReducedSurface);
    aIsQPunctual = isQPunctual(anAdaptor, theParam, theIso, 0.0, myTol);
    aUKnot       = 1;
    aVKnot       = 1;
    aSurface     = aReducedSurface;
  }

  theSurface = aSurface;
  return true;
}

//=================================================================================================

void Geom_OsculatingSurface::appendOsculatingSurface2(
  const occ::handle<Geom_BSplineSurface>& theSurface,
  size_t                                  theNbReductions)
{
  OsculatingSurfaceData anOsculatingSurface;
  anOsculatingSurface.Surface    = theSurface;
  anOsculatingSurface.IsOpposite = (theNbReductions % 2) != 0;
  myOsculSurf2.Append(anOsculatingSurface);
}

//=================================================================================================

bool Geom_OsculatingSurface::UOsculatingSurface(double                            theU,
                                                double                            theV,
                                                bool&                             theT,
                                                occ::handle<Geom_BSplineSurface>& theL) const
{
  if (!(myAlong[0] || myAlong[1]))
  {
    return false;
  }

  theT                 = false;
  int  aNU             = 1;
  int  aNV             = 1;
  int  aNbUK           = 2;
  int  aNbVK           = 2;
  bool aIsToSkipSecond = false;
  if (myBasisSurf->IsKind(STANDARD_TYPE(Geom_BSplineSurface)))
  {
    const occ::handle<Geom_BSplineSurface> aBSur = occ::down_cast<Geom_BSplineSurface>(myBasisSurf);
    aNbUK                                        = aBSur->NbUKnots();
    aNbVK                                        = aBSur->NbVKnots();
    const NCollection_Array1<double>& aUKnots    = aBSur->UKnots();
    const NCollection_Array1<double>& aVKnots    = aBSur->VKnots();
    BSplCLib::Hunt(aUKnots, theU, aNU);
    BSplCLib::Hunt(aVKnots, theV, aNV);
    aNU = std::clamp(aNU, 1, aNbUK - 1);
    if (aNbVK == 2 && aNV == 1)
    {
      // Pick the closest V end when the surface has a single span.
      if (aVKnots(aNbVK) - theV > theV - aVKnots(1))
      {
        aIsToSkipSecond = true;
      }
    }
  }

  bool aAlong = false;
  if (myAlong[0] && aNV == 1)
  {
    theL   = myOsculSurf1.Value(static_cast<size_t>(aNU - 1));
    aAlong = true;
  }
  if (myAlong[1] && aNV == aNbVK - 1 && !aIsToSkipSecond)
  {
    // The derivative is reversed when the number of reductions is odd.
    const OsculatingSurfaceData& anOsculatingSurface =
      myOsculSurf2.Value(static_cast<size_t>(aNU - 1));
    theT   = anOsculatingSurface.IsOpposite;
    theL   = anOsculatingSurface.Surface;
    aAlong = true;
  }
  return aAlong;
}

//=================================================================================================

bool Geom_OsculatingSurface::VOsculatingSurface(double                            theU,
                                                double                            theV,
                                                bool&                             theT,
                                                occ::handle<Geom_BSplineSurface>& theL) const
{
  if (!(myAlong[2] || myAlong[3]))
  {
    return false;
  }

  theT                 = false;
  int  aNU             = 1;
  int  aNV             = 1;
  int  aNbUK           = 2;
  int  aNbVK           = 2;
  bool aIsToSkipSecond = false;
  if (myBasisSurf->IsKind(STANDARD_TYPE(Geom_BSplineSurface)))
  {
    const occ::handle<Geom_BSplineSurface> aBSur = occ::down_cast<Geom_BSplineSurface>(myBasisSurf);
    aNbUK                                        = aBSur->NbUKnots();
    aNbVK                                        = aBSur->NbVKnots();
    const NCollection_Array1<double>& aUKnots    = aBSur->UKnots();
    const NCollection_Array1<double>& aVKnots    = aBSur->VKnots();
    BSplCLib::Hunt(aUKnots, theU, aNU);
    BSplCLib::Hunt(aVKnots, theV, aNV);
    aNV = std::clamp(aNV, 1, aNbVK - 1);
    if (aNbUK == 2 && aNU == 1)
    {
      // Pick the closest U end when the surface has a single span.
      if (aUKnots(aNbUK) - theU > theU - aUKnots(1))
      {
        aIsToSkipSecond = true;
      }
    }
  }

  bool aAlong = false;
  if (myAlong[2] && aNU == 1)
  {
    theL   = myOsculSurf1.Value(static_cast<size_t>(aNV - 1));
    aAlong = true;
  }
  if (myAlong[3] && aNU == aNbUK - 1 && !aIsToSkipSecond)
  {
    const OsculatingSurfaceData& anOsculatingSurface =
      myOsculSurf2.Value(static_cast<size_t>(aNV - 1));
    theT   = anOsculatingSurface.IsOpposite;
    theL   = anOsculatingSurface.Surface;
    aAlong = true;
  }
  return aAlong;
}

//=================================================================================================

bool Geom_OsculatingSurface::buildOsculatingSurface(double theParam,
                                                    int    theSUKnot,
                                                    int    theSVKnot,
                                                    const occ::handle<Geom_BSplineSurface>& theBS,
                                                    occ::handle<Geom_BSplineSurface>& theBSpl) const
{
  const int aUDeg = theBS->UDegree();
  const int aVDeg = theBS->VDegree();
  if ((IsAlongU() && aVDeg <= 1) || (IsAlongV() && aUDeg <= 1))
  {
    return false;
  }

  const int aMinDeg = std::min(aUDeg, aVDeg);
  const int aMaxDeg = std::max(aUDeg, aVDeg);

  NCollection_Array2<gp_Pnt> aCachePoles(1, aMaxDeg + 1, 1, aMinDeg + 1);

  NCollection_LocalArray<int, 2>    aNumCoeffStorage(2);
  NCollection_LocalArray<double, 2> aPolyUStorage(2);
  NCollection_LocalArray<double, 2> aPolyVStorage(2);
  NCollection_LocalArray<double, 2> aTrueUStorage(2);
  NCollection_LocalArray<double, 2> aTrueVStorage(2);
  NCollection_Array2<int>           aNumCoeffPerSurface(aNumCoeffStorage.begin()[0], 1, 1, 1, 2);
  NCollection_Array1<double>        aPolyUIntervals(aPolyUStorage.begin()[0], 1, 2);
  NCollection_Array1<double>        aPolyVIntervals(aPolyVStorage.begin()[0], 1, 2);
  NCollection_Array1<double>        aTrueUIntervals(aTrueUStorage.begin()[0], 1, 2);
  NCollection_Array1<double>        aTrueVIntervals(aTrueVStorage.begin()[0], 1, 2);

  for (size_t i = 0; i < 2; ++i)
  {
    aPolyUIntervals.ChangeAt(i) = static_cast<double>(i);
    aPolyVIntervals.ChangeAt(i) = static_cast<double>(i);
    aTrueUIntervals.ChangeAt(i) = theBS->UKnot(theSUKnot + static_cast<int>(i));
    aTrueVIntervals.ChangeAt(i) = theBS->VKnot(theSVKnot + static_cast<int>(i));
  }

  int aOscUNumCoeff = 0;
  int aOscVNumCoeff = 0;
  if (IsAlongU())
  {
    aOscUNumCoeff = aUDeg + 1;
    aOscVNumCoeff = aVDeg;
  }
  if (IsAlongV())
  {
    aOscUNumCoeff = aUDeg;
    aOscVNumCoeff = aVDeg + 1;
  }
  aNumCoeffPerSurface.ChangeAt(0, 0) = aOscUNumCoeff;
  aNumCoeffPerSurface.ChangeAt(0, 1) = aOscVNumCoeff;
  const int aNbCoef                  = aOscUNumCoeff * aOscVNumCoeff * 3;
  if (aNbCoef == 0)
  {
    return false;
  }

  NCollection_LocalArray<double> aCoefficientStorage(static_cast<size_t>(aNbCoef));
  NCollection_Array1<double>     aCoefficients(aCoefficientStorage.begin()[0], 1, aNbCoef);

  const NCollection_Array2<gp_Pnt>& aPoles     = theBS->Poles();
  const NCollection_Array1<double>& aUFlatKnts = theBS->UKnotSequence();
  const NCollection_Array1<double>& aVFlatKnts = theBS->VKnotSequence();

  double       aUCacheParam = theBS->UKnot(theSUKnot);
  double       aVCacheParam = theBS->VKnot(theSVKnot);
  const double aUSpanLen    = theBS->UKnot(theSUKnot + 1) - aUCacheParam;
  const double aVSpanLen    = theBS->VKnot(theSVKnot + 1) - aVCacheParam;

  // Always reduce to a parametrization such that locally it is the iso
  // u=0 or v=0 that is degenerate.
  const bool aIsVNeg = theParam > aVCacheParam + aVSpanLen / 2;
  const bool aIsUNeg = theParam > aUCacheParam + aUSpanLen / 2;
  if (IsAlongU() && aIsVNeg)
  {
    aVCacheParam += aVSpanLen;
  }
  if (IsAlongV() && aIsUNeg)
  {
    aUCacheParam += aUSpanLen;
  }

  BSplSLib::BuildCache(aUCacheParam,
                       aVCacheParam,
                       aUSpanLen,
                       aVSpanLen,
                       theBS->IsUPeriodic(),
                       theBS->IsVPeriodic(),
                       aUDeg,
                       aVDeg,
                       /*ULocalIndex*/ 0,
                       /*VLocalIndex*/ 0,
                       aUFlatKnts,
                       aVFlatKnts,
                       aPoles,
                       BSplSLib::NoWeights(),
                       aCachePoles,
                       BSplSLib::NoWeights());

  NCollection_Array2<gp_Pnt> aOscCoeff(1, aOscUNumCoeff, 1, aOscVNumCoeff);
  const size_t               aUNbPoles = static_cast<size_t>(aUDeg) + 1;
  const size_t               aVNbPoles = static_cast<size_t>(aVDeg) + 1;

  if (IsAlongU())
  {
    if (aUDeg > aVDeg)
    {
      for (size_t n = 0; n < aUNbPoles; ++n)
      {
        for (size_t m = 0; m < aVNbPoles - 1; ++m)
        {
          aOscCoeff.ChangeAt(n, m) = aCachePoles.At(n, m + 1);
        }
      }
    }
    else
    {
      for (size_t n = 0; n < aUNbPoles; ++n)
      {
        for (size_t m = 0; m < aVNbPoles - 1; ++m)
        {
          aOscCoeff.ChangeAt(n, m) = aCachePoles.At(m + 1, n);
        }
      }
    }
    if (aIsVNeg)
    {
      PLib::VTrimming(-1, 0, aOscCoeff, PLib::NoWeights2());
    }

    size_t aIdx = 0;
    for (size_t n = 0; n < aUNbPoles; ++n)
    {
      for (size_t m = 0; m < aVNbPoles - 1; ++m)
      {
        const gp_Pnt& aPole            = aOscCoeff.At(n, m);
        aCoefficients.ChangeAt(aIdx++) = aPole.X();
        aCoefficients.ChangeAt(aIdx++) = aPole.Y();
        aCoefficients.ChangeAt(aIdx++) = aPole.Z();
      }
    }
  }
  else
  {
    if (aUDeg > aVDeg)
    {
      for (size_t n = 0; n < aUNbPoles - 1; ++n)
      {
        for (size_t m = 0; m < aVNbPoles; ++m)
        {
          aOscCoeff.ChangeAt(n, m) = aCachePoles.At(n + 1, m);
        }
      }
    }
    else
    {
      for (size_t n = 0; n < aUNbPoles - 1; ++n)
      {
        for (size_t m = 0; m < aVNbPoles; ++m)
        {
          aOscCoeff.ChangeAt(n, m) = aCachePoles.At(m, n + 1);
        }
      }
    }
    if (aIsUNeg)
    {
      PLib::UTrimming(-1, 0, aOscCoeff, PLib::NoWeights2());
    }

    size_t aIdx = 0;
    for (size_t n = 0; n < aUNbPoles - 1; ++n)
    {
      for (size_t m = 0; m < aVNbPoles; ++m)
      {
        const gp_Pnt& aPole            = aOscCoeff.At(n, m);
        aCoefficients.ChangeAt(aIdx++) = aPole.X();
        aCoefficients.ChangeAt(aIdx++) = aPole.Y();
        aCoefficients.ChangeAt(aIdx++) = aPole.Z();
      }
    }
  }

  const int aMaxUDegOut = IsAlongV() ? aUDeg - 1 : aUDeg;
  const int aMaxVDegOut = IsAlongU() ? aVDeg - 1 : aVDeg;

  Convert_GridPolynomialToPoles aData(1,
                                      1,
                                      /*UContinuity*/ -1,
                                      /*VContinuity*/ -1,
                                      aMaxUDegOut,
                                      aMaxVDegOut,
                                      aNumCoeffPerSurface,
                                      aCoefficients,
                                      aPolyUIntervals,
                                      aPolyVIntervals,
                                      aTrueUIntervals,
                                      aTrueVIntervals);

  theBSpl = new Geom_BSplineSurface(aData.Poles(),
                                    aData.UKnots(),
                                    aData.VKnots(),
                                    aData.UMultiplicities(),
                                    aData.VMultiplicities(),
                                    aData.UDegree(),
                                    aData.VDegree(),
                                    false,
                                    false);
  return true;
}

//=================================================================================================

bool Geom_OsculatingSurface::isQPunctual(const GeomAdaptor_Surface& theAdaptor,
                                         double                     theParam,
                                         GeomAbs_IsoType            theIT,
                                         double                     theTolMin,
                                         double                     theTolMax) const
{
  const bool   anIsIsoV = theIT == GeomAbs_IsoV;
  const double aPmin    = anIsIsoV ? theAdaptor.FirstUParameter() : theAdaptor.FirstVParameter();
  const double aPmax    = anIsIsoV ? theAdaptor.LastUParameter() : theAdaptor.LastVParameter();
  const int    aNbIntervals =
    anIsIsoV ? theAdaptor.NbUIntervals(GeomAbs_CN) : theAdaptor.NbVIntervals(GeomAbs_CN);
  if (aNbIntervals < 1)
  {
    return false;
  }

  NCollection_LocalArray<double> aIntervalStorage(static_cast<size_t>(aNbIntervals + 1));
  NCollection_Array1<double>     aIntervals(aIntervalStorage.begin()[0], 1, aNbIntervals + 1);
  if (anIsIsoV)
  {
    theAdaptor.UIntervals(aIntervals, GeomAbs_CN);
  }
  else
  {
    theAdaptor.VIntervals(aIntervals, GeomAbs_CN);
  }
  aIntervals.ChangeAt(0)                                 = aPmin;
  aIntervals.ChangeAt(static_cast<size_t>(aNbIntervals)) = aPmax;

  const int    aDegree      = anIsIsoV ? theAdaptor.UDegree() : theAdaptor.VDegree();
  const size_t aNbSamples   = static_cast<size_t>(std::max(aDegree, 1));
  double       aD1NormMax   = 0.0;
  auto         updateD1Norm = [&](const double theParameter) {
    if (anIsIsoV)
    {
      const Geom_Surface::ResD1 aResult = theAdaptor.EvalD1(theParameter, theParam);
      aD1NormMax                        = std::max(aD1NormMax, aResult.D1U.Magnitude());
    }
    else
    {
      const Geom_Surface::ResD1 aResult = theAdaptor.EvalD1(theParam, theParameter);
      aD1NormMax                        = std::max(aD1NormMax, aResult.D1V.Magnitude());
    }
  };

  for (size_t anInterval = 0; anInterval < static_cast<size_t>(aNbIntervals); ++anInterval)
  {
    const double aFirst = aIntervals.At(anInterval);
    const double aLast  = aIntervals.At(anInterval + 1);
    const double aRange = aLast - aFirst;
    for (size_t anIndex = 0; anIndex <= aNbSamples; ++anIndex)
    {
      const double aParameter =
        anIndex == aNbSamples
          ? aLast
          : aFirst + aRange * static_cast<double>(anIndex) / static_cast<double>(aNbSamples);
      updateD1Norm(aParameter);
    }
  }

  return !(aD1NormMax > theTolMax || aD1NormMax < theTolMin);
}
