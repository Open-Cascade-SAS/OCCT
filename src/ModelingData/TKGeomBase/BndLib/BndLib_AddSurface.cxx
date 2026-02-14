// Created on: 1995-07-24
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
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

//  Modified by skv - Fri Aug 27 12:29:04 2004 OCC6503

#include <Adaptor3d_Surface.hxx>
#include <Bnd_Box.hxx>
#include <BndLib.hxx>
#include <BndLib_AddSurface.hxx>
#include <BSplSLib_Cache.hxx>
#include <ElSLib.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomGridEval_Surface.hxx>
#include <gp_Cone.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <MathOpt_Powell.hxx>
#include <MathOpt_PSO.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <Precision.hxx>
#include <Standard_Integer.hxx>

/// Maximum bisection iterations for derivative root isolation.
/// 53 iterations guarantee full double precision (~2^-53 relative interval).
constexpr int THE_MAX_BISECTION_ITER = 53;
//
static int NbUSamples(const Adaptor3d_Surface& S, const double Umin, const double Umax);
//
static int NbVSamples(const Adaptor3d_Surface& S, const double Vmin, const double Vmax);
//
static double AdjustExtr(const Adaptor3d_Surface& S,
                         const double             UMin,
                         const double             UMax,
                         const double             VMin,
                         const double             VMax,
                         const double             Extr0,
                         const int                CoordIndx,
                         const double             Tol,
                         const bool               IsMin);

/// Finds coordinate extrema along one edge of a surface span using D1 + bisection.
/// @param theCache      surface cache built for the current span
/// @param theFixedParam value of the fixed parameter (u or v boundary)
/// @param theVaryMin    start of varying parameter range
/// @param theVaryMax    end of varying parameter range
/// @param theIsUFixed   true if u is fixed (edge parallel to v), false if v is fixed
/// @param theUMin       lower U bound of the current span
/// @param theUMax       upper U bound of the current span
/// @param theVMin       lower V bound of the current span
/// @param theVMax       upper V bound of the current span
/// @param theBox        bounding box to extend
static void addSurfEdgeExtrema(BSplSLib_Cache& theCache,
                               const double    theFixedParam,
                               const double    theVaryMin,
                               const double    theVaryMax,
                               const bool      theIsUFixed,
                               const double    theUMin,
                               const double    theUMax,
                               const double    theVMin,
                               const double    theVMax,
                               Bnd_Box&        theBox)
{
  const double aURange = theUMax - theUMin;
  const double aVRange = theVMax - theVMin;
  if (aURange < Precision::PConfusion() || aVRange < Precision::PConfusion())
  {
    return;
  }

  const double aUMid       = 0.5 * (theUMin + theUMax);
  const double aVMid       = 0.5 * (theVMin + theVMax);
  const double aUInvLength = 2.0 / aURange;
  const double aVInvLength = 2.0 / aVRange;

  const double aFixedLocal =
    theIsUFixed ? (theFixedParam - aUMid) * aUInvLength : (theFixedParam - aVMid) * aVInvLength;
  const double aVaryLocalMin =
    theIsUFixed ? (theVaryMin - aVMid) * aVInvLength : (theVaryMin - aUMid) * aUInvLength;
  const double aVaryLocalMax =
    theIsUFixed ? (theVaryMax - aVMid) * aVInvLength : (theVaryMax - aUMid) * aUInvLength;

  const double aRangeLocal = aVaryLocalMax - aVaryLocalMin;
  if (std::abs(aRangeLocal) < Precision::PConfusion())
  {
    return;
  }

  // Sample D1Local along the edge
  constexpr int aNbSamples = 16;
  const double  aDt        = aRangeLocal / aNbSamples;
  const double  aTolParam  = std::max(1.0e-15, 1.0e-12 * std::abs(aRangeLocal));

  gp_Pnt aPnt;
  gp_Vec aDU, aDV;

  auto evalDerivAt = [&](const double theLocalVar, double theDeriv[3]) {
    if (theIsUFixed)
    {
      theCache.D1Local(aFixedLocal, theLocalVar, aPnt, aDU, aDV);
      theDeriv[0] = aDV.X();
      theDeriv[1] = aDV.Y();
      theDeriv[2] = aDV.Z();
    }
    else
    {
      theCache.D1Local(theLocalVar, aFixedLocal, aPnt, aDU, aDV);
      theDeriv[0] = aDU.X();
      theDeriv[1] = aDU.Y();
      theDeriv[2] = aDU.Z();
    }
  };

  double aPrevDeriv[3];
  double aCurrDeriv[3];
  evalDerivAt(aVaryLocalMin, aPrevDeriv);

  for (int i = 1; i <= aNbSamples; ++i)
  {
    const double aLocalVar = aVaryLocalMin + i * aDt;
    evalDerivAt(aLocalVar, aCurrDeriv);

    bool hasNearZero = false;
    for (int aCoord = 0; aCoord < 3; ++aCoord)
    {
      if (aPrevDeriv[aCoord] * aCurrDeriv[aCoord] < 0.0)
      {
        // Sign change detected - bisect
        double aTLow  = aLocalVar - aDt;
        double aTHigh = aLocalVar;
        double aDLow  = aPrevDeriv[aCoord];

        for (int anIter = 0; anIter < THE_MAX_BISECTION_ITER; ++anIter)
        {
          const double aTMid = (aTLow + aTHigh) * 0.5;
          double       aDerivMid[3];
          evalDerivAt(aTMid, aDerivMid);
          const double aDMid = aDerivMid[aCoord];

          if (aDLow * aDMid <= 0.0)
          {
            aTHigh = aTMid;
          }
          else
          {
            aTLow = aTMid;
            aDLow = aDMid;
          }

          if (aTHigh - aTLow <= aTolParam)
          {
            break;
          }
        }

        // Evaluate D0 at root and add to box
        const double aTRoot = (aTLow + aTHigh) * 0.5;
        if (theIsUFixed)
        {
          theCache.D0Local(aFixedLocal, aTRoot, aPnt);
        }
        else
        {
          theCache.D0Local(aTRoot, aFixedLocal, aPnt);
        }
        theBox.Add(aPnt);
      }
      else if (std::abs(aCurrDeriv[aCoord]) < Precision::Confusion())
      {
        hasNearZero = true;
      }
    }

    if (hasNearZero)
    {
      // Derivative is near zero at this sample for at least one coordinate.
      if (theIsUFixed)
      {
        theCache.D0Local(aFixedLocal, aLocalVar, aPnt);
      }
      else
      {
        theCache.D0Local(aLocalVar, aFixedLocal, aPnt);
      }
      theBox.Add(aPnt);
    }

    aPrevDeriv[0] = aCurrDeriv[0];
    aPrevDeriv[1] = aCurrDeriv[1];
    aPrevDeriv[2] = aCurrDeriv[2];
  }
}

/// Finds interior coordinate extrema within a surface span.
/// Samples D1 on a coarse grid and looks for cells where both partial derivatives
/// change sign simultaneously for a given coordinate, then refines with alternating bisection.
/// @param theCache  surface cache built for the current span
/// @param theUMin   lower bound of U parameter range for the span
/// @param theUMax   upper bound of U parameter range for the span
/// @param theVMin   lower bound of V parameter range for the span
/// @param theVMax   upper bound of V parameter range for the span
/// @param theBox    bounding box to extend
static void addSurfInteriorExtrema(BSplSLib_Cache& theCache,
                                   const double    theUMin,
                                   const double    theUMax,
                                   const double    theVMin,
                                   const double    theVMax,
                                   Bnd_Box&        theBox)
{
  const double aURng = theUMax - theUMin;
  const double aVRng = theVMax - theVMin;
  if (aURng < Precision::PConfusion() || aVRng < Precision::PConfusion())
  {
    return;
  }

  // Sample D1 on a coarse grid
  constexpr int aNbU = 8;
  constexpr int aNbV = 8;
  const double  aDU  = 2.0 / aNbU;
  const double  aDV  = 2.0 / aNbV;
  const double  aTol = 1.0e-12;

  // Store derivative components at grid nodes
  // For each coordinate c (0=X,1=Y,2=Z): dS_c/du and dS_c/dv
  double aDUArr[3][(aNbU + 1) * (aNbV + 1)];
  double aDVArr[3][(aNbU + 1) * (aNbV + 1)];

  for (int i = 0; i <= aNbU; ++i)
  {
    const double aU = -1.0 + i * aDU;
    for (int j = 0; j <= aNbV; ++j)
    {
      const double aV = -1.0 + j * aDV;
      gp_Pnt       aPnt;
      gp_Vec       aTanU, aTanV;
      theCache.D1Local(aU, aV, aPnt, aTanU, aTanV);

      const int anIdx  = i * (aNbV + 1) + j;
      aDUArr[0][anIdx] = aTanU.X();
      aDUArr[1][anIdx] = aTanU.Y();
      aDUArr[2][anIdx] = aTanU.Z();
      aDVArr[0][anIdx] = aTanV.X();
      aDVArr[1][anIdx] = aTanV.Y();
      aDVArr[2][anIdx] = aTanV.Z();
    }
  }

  // For each coordinate, find cells where both dS_c/du and dS_c/dv change sign
  for (int c = 0; c < 3; ++c)
  {
    for (int i = 0; i < aNbU; ++i)
    {
      for (int j = 0; j < aNbV; ++j)
      {
        // Check corners of cell (i,j) to (i+1,j+1)
        const int anIdx00 = i * (aNbV + 1) + j;
        const int anIdx10 = (i + 1) * (aNbV + 1) + j;
        const int anIdx01 = i * (aNbV + 1) + (j + 1);
        const int anIdx11 = (i + 1) * (aNbV + 1) + (j + 1);

        // Check if dS_c/du changes sign across the cell in u-direction.
        // Use <= 0.0 to handle the case where a derivative is exactly zero
        // at a grid node (the root coincides with a sample point).
        const bool aUSignChange = (aDUArr[c][anIdx00] * aDUArr[c][anIdx10] <= 0.0
                                   && (aDUArr[c][anIdx00] != 0.0 || aDUArr[c][anIdx10] != 0.0))
                                  || (aDUArr[c][anIdx01] * aDUArr[c][anIdx11] <= 0.0
                                      && (aDUArr[c][anIdx01] != 0.0 || aDUArr[c][anIdx11] != 0.0));

        // Check if dS_c/dv changes sign across the cell in v-direction
        const bool aVSignChange = (aDVArr[c][anIdx00] * aDVArr[c][anIdx01] <= 0.0
                                   && (aDVArr[c][anIdx00] != 0.0 || aDVArr[c][anIdx01] != 0.0))
                                  || (aDVArr[c][anIdx10] * aDVArr[c][anIdx11] <= 0.0
                                      && (aDVArr[c][anIdx10] != 0.0 || aDVArr[c][anIdx11] != 0.0));

        if (!aUSignChange || !aVSignChange)
        {
          continue;
        }

        // Both partial derivatives change sign in this cell.
        // Refine with alternating 1D bisection in u and v.
        double aULow  = -1.0 + i * aDU;
        double aUHigh = -1.0 + (i + 1) * aDU;
        double aVLow  = -1.0 + j * aDV;
        double aVHigh = -1.0 + (j + 1) * aDV;

        double aUMid = (aULow + aUHigh) * 0.5;
        double aVMid = (aVLow + aVHigh) * 0.5;

        for (int anIter = 0; anIter < THE_MAX_BISECTION_ITER; ++anIter)
        {
          gp_Pnt aPnt;
          gp_Vec aTanU, aTanV;
          theCache.D1Local(aUMid, aVMid, aPnt, aTanU, aTanV);
          const double aDMidU = (c == 0 ? aTanU.X() : (c == 1 ? aTanU.Y() : aTanU.Z()));
          const double aDMidV = (c == 0 ? aTanV.X() : (c == 1 ? aTanV.Y() : aTanV.Z()));

          if (anIter % 2 == 0)
          {
            // Bisect in u: narrow the u interval based on dS_c/du sign
            gp_Pnt aPntLow;
            gp_Vec aTanULow, aTanVLow;
            theCache.D1Local(aULow, aVMid, aPntLow, aTanULow, aTanVLow);
            const double aDLowU = (c == 0 ? aTanULow.X() : (c == 1 ? aTanULow.Y() : aTanULow.Z()));

            if (aDLowU * aDMidU <= 0.0)
            {
              aUHigh = aUMid;
            }
            else
            {
              aULow = aUMid;
            }
            aUMid = (aULow + aUHigh) * 0.5;
          }
          else
          {
            // Bisect in v: narrow the v interval based on dS_c/dv sign
            gp_Pnt aPntLow;
            gp_Vec aTanULow, aTanVLow;
            theCache.D1Local(aUMid, aVLow, aPntLow, aTanULow, aTanVLow);
            const double aDLowV = (c == 0 ? aTanVLow.X() : (c == 1 ? aTanVLow.Y() : aTanVLow.Z()));

            if (aDLowV * aDMidV <= 0.0)
            {
              aVHigh = aVMid;
            }
            else
            {
              aVLow = aVMid;
            }
            aVMid = (aVLow + aVHigh) * 0.5;
          }

          if (aUHigh - aULow <= aTol && aVHigh - aVLow <= aTol)
          {
            break;
          }
        }

        // Evaluate D0 at the converged point and add to box
        gp_Pnt aResult;
        theCache.D0Local(aUMid, aVMid, aResult);
        theBox.Add(aResult);
      }
    }
  }
}

/// Computes exact bounding box for a 3D BSpline surface.
/// Uses per-span edge derivative root-finding and interior extrema detection.
/// @param theSurf  BSpline surface
/// @param theTol   tolerance to enlarge the resulting box
/// @param theBox   bounding box to extend
static void addBSplineSurfBBox3d(const occ::handle<Geom_BSplineSurface>& theSurf,
                                 const double                            theTol,
                                 Bnd_Box&                                theBox)
{
  const int  aUDegree    = theSurf->UDegree();
  const int  aVDegree    = theSurf->VDegree();
  const bool anIsUPeriod = theSurf->IsUPeriodic();
  const bool anIsVPeriod = theSurf->IsVPeriodic();

  const NCollection_Array1<double>& aUKnots = theSurf->UKnots();
  const NCollection_Array1<double>& aVKnots = theSurf->VKnots();
  const NCollection_Array2<gp_Pnt>& aPoles  = theSurf->Poles();

  const NCollection_Array2<double>* aWeights = theSurf->Weights();

  const NCollection_Array1<double>& aUFlatKnots = theSurf->UKnotSequence();
  const NCollection_Array1<double>& aVFlatKnots = theSurf->VKnotSequence();

  // Add all knot grid intersection points
  for (int i = aUKnots.Lower(); i <= aUKnots.Upper(); ++i)
  {
    for (int j = aVKnots.Lower(); j <= aVKnots.Upper(); ++j)
    {
      theBox.Add(theSurf->Value(aUKnots(i), aVKnots(j)));
    }
  }

  // Create cache for surface span evaluation
  occ::handle<BSplSLib_Cache> aCache = new BSplSLib_Cache(aUDegree,
                                                          anIsUPeriod,
                                                          aUFlatKnots,
                                                          aVDegree,
                                                          anIsVPeriod,
                                                          aVFlatKnots,
                                                          aWeights);

  // Process each U-span x V-span
  for (int iU = aUKnots.Lower(); iU < aUKnots.Upper(); ++iU)
  {
    const double aUMin = aUKnots(iU);
    const double aUMax = aUKnots(iU + 1);
    if (aUMax - aUMin < Precision::PConfusion())
    {
      continue;
    }

    for (int iV = aVKnots.Lower(); iV < aVKnots.Upper(); ++iV)
    {
      const double aVMin = aVKnots(iV);
      const double aVMax = aVKnots(iV + 1);
      if (aVMax - aVMin < Precision::PConfusion())
      {
        continue;
      }

      // Build cache for this span
      aCache->BuildCache(aUMin, aVMin, aUFlatKnots, aVFlatKnots, aPoles, aWeights);

      // Edge extrema: 4 edges of the span
      // Bottom edge: v = aVMin, u varies
      addSurfEdgeExtrema(*aCache, aVMin, aUMin, aUMax, false, aUMin, aUMax, aVMin, aVMax, theBox);
      // Top edge: v = aVMax, u varies
      addSurfEdgeExtrema(*aCache, aVMax, aUMin, aUMax, false, aUMin, aUMax, aVMin, aVMax, theBox);
      // Left edge: u = aUMin, v varies
      addSurfEdgeExtrema(*aCache, aUMin, aVMin, aVMax, true, aUMin, aUMax, aVMin, aVMax, theBox);
      // Right edge: u = aUMax, v varies
      addSurfEdgeExtrema(*aCache, aUMax, aVMin, aVMax, true, aUMin, aUMax, aVMin, aVMax, theBox);

      // Interior extrema
      addSurfInteriorExtrema(*aCache, aUMin, aUMax, aVMin, aVMax, theBox);
    }
  }

  theBox.Enlarge(theTol);
}

/// Computes exact bounding box for a 3D Bezier surface.
/// @param theSurf  Bezier surface
/// @param theTol   tolerance to enlarge the resulting box
/// @param theBox   bounding box to extend
static void addBezierSurfBBox3d(const occ::handle<Geom_BezierSurface>& theSurf,
                                const double                           theTol,
                                Bnd_Box&                               theBox)
{
  const int  aUDegree = theSurf->UDegree();
  const int  aVDegree = theSurf->VDegree();
  const bool anIsRat  = theSurf->IsURational() || theSurf->IsVRational();

  // Add corner points
  double aUMin, aUMax, aVMin, aVMax;
  theSurf->Bounds(aUMin, aUMax, aVMin, aVMax);

  theBox.Add(theSurf->Value(aUMin, aVMin));
  theBox.Add(theSurf->Value(aUMin, aVMax));
  theBox.Add(theSurf->Value(aUMax, aVMin));
  theBox.Add(theSurf->Value(aUMax, aVMax));

  if (aUDegree < 2 && aVDegree < 2)
  {
    theBox.Enlarge(theTol);
    return;
  }

  // Build flat knots for Bezier (single span)
  NCollection_Array1<double> aUFlatKnots(1, 2 * (aUDegree + 1));
  NCollection_Array1<double> aVFlatKnots(1, 2 * (aVDegree + 1));
  for (int i = 1; i <= aUDegree + 1; ++i)
  {
    aUFlatKnots(i)                = aUMin;
    aUFlatKnots(i + aUDegree + 1) = aUMax;
  }
  for (int i = 1; i <= aVDegree + 1; ++i)
  {
    aVFlatKnots(i)                = aVMin;
    aVFlatKnots(i + aVDegree + 1) = aVMax;
  }

  const NCollection_Array2<gp_Pnt>& aPoles   = theSurf->Poles();
  const NCollection_Array2<double>* aWeights = anIsRat ? theSurf->Weights() : nullptr;

  // Create cache and build for the single span
  occ::handle<BSplSLib_Cache> aCache =
    new BSplSLib_Cache(aUDegree, false, aUFlatKnots, aVDegree, false, aVFlatKnots, aWeights);
  aCache->BuildCache(aUMin, aVMin, aUFlatKnots, aVFlatKnots, aPoles, aWeights);

  // Edge extrema: 4 edges
  addSurfEdgeExtrema(*aCache, aVMin, aUMin, aUMax, false, aUMin, aUMax, aVMin, aVMax, theBox);
  addSurfEdgeExtrema(*aCache, aVMax, aUMin, aUMax, false, aUMin, aUMax, aVMin, aVMax, theBox);
  addSurfEdgeExtrema(*aCache, aUMin, aVMin, aVMax, true, aUMin, aUMax, aVMin, aVMax, theBox);
  addSurfEdgeExtrema(*aCache, aUMax, aVMin, aVMax, true, aUMin, aUMax, aVMin, aVMax, theBox);

  // Interior extrema
  addSurfInteriorExtrema(*aCache, aUMin, aUMax, aVMin, aVMax, theBox);

  theBox.Enlarge(theTol);
}

//=================================================================================================

void BndLib_AddSurface::Add(const Adaptor3d_Surface& S, const double Tol, Bnd_Box& B)
{

  BndLib_AddSurface::Add(S,
                         S.FirstUParameter(),
                         S.LastUParameter(),
                         S.FirstVParameter(),
                         S.LastVParameter(),
                         Tol,
                         B);
}

//=================================================================================================

static int NbUSamples(const Adaptor3d_Surface& S)
{
  int                 N;
  GeomAbs_SurfaceType Type = S.GetType();
  switch (Type)
  {
    case GeomAbs_BezierSurface: {
      N = 2 * S.NbUPoles();
      break;
    }
    case GeomAbs_BSplineSurface: {
      const occ::handle<Geom_BSplineSurface>& BS = S.BSpline();
      N                                          = 2 * (BS->UDegree() + 1) * (BS->NbUKnots() - 1);
      break;
    }
    default:
      N = 33;
  }
  return std::min(50, N);
}

//=================================================================================================

static int NbVSamples(const Adaptor3d_Surface& S)
{
  int                 N;
  GeomAbs_SurfaceType Type = S.GetType();
  switch (Type)
  {
    case GeomAbs_BezierSurface: {
      N = 2 * S.NbVPoles();
      break;
    }
    case GeomAbs_BSplineSurface: {
      const occ::handle<Geom_BSplineSurface>& BS = S.BSpline();
      N                                          = 2 * (BS->VDegree() + 1) * (BS->NbVKnots() - 1);
      break;
    }
    default:
      N = 33;
  }
  return std::min(50, N);
}

//  Modified by skv - Fri Aug 27 12:29:04 2004 OCC6503 Begin
static gp_Pnt BaryCenter(const gp_Pln& aPlane,
                         const double  aUMin,
                         const double  aUMax,
                         const double  aVMin,
                         const double  aVMax)
{
  double aU, aV;
  bool   isU1Inf = Precision::IsInfinite(aUMin);
  bool   isU2Inf = Precision::IsInfinite(aUMax);
  bool   isV1Inf = Precision::IsInfinite(aVMin);
  bool   isV2Inf = Precision::IsInfinite(aVMax);

  if (isU1Inf && isU2Inf)
    aU = 0;
  else if (isU1Inf)
    aU = aUMax - 10.;
  else if (isU2Inf)
    aU = aUMin + 10.;
  else
    aU = (aUMin + aUMax) / 2.;

  if (isV1Inf && isV2Inf)
    aV = 0;
  else if (isV1Inf)
    aV = aVMax - 10.;
  else if (isV2Inf)
    aV = aVMin + 10.;
  else
    aV = (aVMin + aVMax) / 2.;

  gp_Pnt aCenter = ElSLib::Value(aU, aV, aPlane);

  return aCenter;
}

static void TreatInfinitePlane(const gp_Pln& aPlane,
                               const double  aUMin,
                               const double  aUMax,
                               const double  aVMin,
                               const double  aVMax,
                               const double  aTol,
                               Bnd_Box&      aB)
{
  // Get 3 coordinate axes of the plane.
  const gp_Dir&    aNorm        = aPlane.Axis().Direction();
  constexpr double anAngularTol = RealEpsilon();

  // Get location of the plane as its barycenter
  gp_Pnt aLocation = BaryCenter(aPlane, aUMin, aUMax, aVMin, aVMax);

  if (aNorm.IsParallel(gp::DX(), anAngularTol))
  {
    aB.Add(aLocation);
    aB.OpenYmin();
    aB.OpenYmax();
    aB.OpenZmin();
    aB.OpenZmax();
  }
  else if (aNorm.IsParallel(gp::DY(), anAngularTol))
  {
    aB.Add(aLocation);
    aB.OpenXmin();
    aB.OpenXmax();
    aB.OpenZmin();
    aB.OpenZmax();
  }
  else if (aNorm.IsParallel(gp::DZ(), anAngularTol))
  {
    aB.Add(aLocation);
    aB.OpenXmin();
    aB.OpenXmax();
    aB.OpenYmin();
    aB.OpenYmax();
  }
  else
  {
    aB.SetWhole();
    return;
  }

  aB.Enlarge(aTol);
}

//=================================================================================================

void BndLib_AddSurface::Add(const Adaptor3d_Surface& S,
                            const double             UMin,
                            const double             UMax,
                            const double             VMin,
                            const double             VMax,
                            const double             Tol,
                            Bnd_Box&                 B)
{
  GeomAbs_SurfaceType Type = S.GetType(); // skv OCC6503

  if (Precision::IsInfinite(VMin) || Precision::IsInfinite(VMax) || Precision::IsInfinite(UMin)
      || Precision::IsInfinite(UMax))
  {
    //  Modified by skv - Fri Aug 27 12:29:04 2004 OCC6503 Begin
    //     B.SetWhole();
    //     return;
    switch (Type)
    {
      case GeomAbs_Plane: {
        TreatInfinitePlane(S.Plane(), UMin, UMax, VMin, VMax, Tol, B);
        return;
      }
      default: {
        B.SetWhole();
        return;
      }
    }
    //  Modified by skv - Fri Aug 27 12:29:04 2004 OCC6503 End
  }

  //   GeomAbs_SurfaceType Type = S.GetType(); // skv OCC6503

  switch (Type)
  {

    case GeomAbs_Plane: {
      gp_Pln Plan = S.Plane();
      B.Add(ElSLib::Value(UMin, VMin, Plan));
      B.Add(ElSLib::Value(UMin, VMax, Plan));
      B.Add(ElSLib::Value(UMax, VMin, Plan));
      B.Add(ElSLib::Value(UMax, VMax, Plan));
      B.Enlarge(Tol);
      break;
    }
    case GeomAbs_Cylinder: {
      BndLib::Add(S.Cylinder(), UMin, UMax, VMin, VMax, Tol, B);
      break;
    }
    case GeomAbs_Cone: {
      BndLib::Add(S.Cone(), UMin, UMax, VMin, VMax, Tol, B);
      break;
    }
    case GeomAbs_Torus: {
      BndLib::Add(S.Torus(), UMin, UMax, VMin, VMax, Tol, B);
      break;
    }
    case GeomAbs_Sphere: {
      if (std::abs(UMin) < Precision::Angular() && std::abs(UMax - 2. * M_PI) < Precision::Angular()
          && std::abs(VMin + M_PI / 2.) < Precision::Angular()
          && std::abs(VMax - M_PI / 2.) < Precision::Angular()) // a whole sphere
        BndLib::Add(S.Sphere(), Tol, B);
      else
        BndLib::Add(S.Sphere(), UMin, UMax, VMin, VMax, Tol, B);
      break;
    }
    case GeomAbs_OffsetSurface: {
      occ::handle<Adaptor3d_Surface> HS = S.BasisSurface();
      Add(*HS, UMin, UMax, VMin, VMax, Tol, B);
      B.Enlarge(S.OffsetValue());
      B.Enlarge(Tol);
      break;
    }
    case GeomAbs_BezierSurface: {
      addBezierSurfBBox3d(S.Bezier(), Tol, B);
      break;
    }
    case GeomAbs_BSplineSurface: {
      occ::handle<Geom_BSplineSurface> aBS = S.BSpline();
      double                           anUMinParam, anUMaxParam, aVMinParam, aVMaxParam;
      aBS->Bounds(anUMinParam, anUMaxParam, aVMinParam, aVMaxParam);

      const double PTol = Precision::Parametric(Precision::Confusion());
      if (std::abs(UMin - anUMinParam) > PTol || std::abs(UMax - anUMaxParam) > PTol
          || std::abs(VMin - aVMinParam) > PTol || std::abs(VMax - aVMaxParam) > PTol)
      {
        // Trim to the requested parameter range using Segment
        occ::handle<Geom_Geometry>       aG = aBS->Copy();
        occ::handle<Geom_BSplineSurface> aBSaux(occ::down_cast<Geom_BSplineSurface>(aG));
        double                           u1 = UMin, u2 = UMax, v1 = VMin, v2 = VMax;
        if (!aBSaux->IsUPeriodic())
        {
          u1 = std::max(u1, anUMinParam);
          u2 = std::min(u2, anUMaxParam);
        }
        if (!aBSaux->IsVPeriodic())
        {
          v1 = std::max(v1, aVMinParam);
          v2 = std::min(v2, aVMaxParam);
        }
        double aUSegTol = 2. * Precision::PConfusion();
        double aVSegTol = 2. * Precision::PConfusion();
        if (std::abs(u2 - u1) < aUSegTol)
          aUSegTol = std::abs(u2 - u1) * 0.01;
        if (std::abs(v2 - v1) < aVSegTol)
          aVSegTol = std::abs(v2 - v1) * 0.01;
        aBSaux->Segment(u1, u2, v1, v2, aUSegTol, aVSegTol);
        aBS = aBSaux;
      }
      addBSplineSurfBBox3d(aBS, Tol, B);
      break;
    }
    default: {
      // Use batch grid evaluation for optimized surface point computation
      const int Nu = NbUSamples(S);
      const int Nv = NbVSamples(S);

      NCollection_Array1<double> aUParams(1, Nu);
      NCollection_Array1<double> aVParams(1, Nv);

      for (int i = 1; i <= Nu; i++)
      {
        aUParams.SetValue(i, UMin + ((UMax - UMin) * (i - 1) / (Nu - 1)));
      }
      for (int j = 1; j <= Nv; j++)
      {
        aVParams.SetValue(j, VMin + ((VMax - VMin) * (j - 1) / (Nv - 1)));
      }

      GeomGridEval_Surface anEvaluator;
      anEvaluator.Initialize(S);

      const NCollection_Array2<gp_Pnt> aGrid = anEvaluator.EvaluateGrid(aUParams, aVParams);
      for (int i = aGrid.LowerRow(); i <= aGrid.UpperRow(); i++)
      {
        for (int j = aGrid.LowerCol(); j <= aGrid.UpperCol(); j++)
        {
          B.Add(aGrid.Value(i, j));
        }
      }
      B.Enlarge(Tol);
    }
  }
}

//----- Methods for AddOptimal ---------------------------------------

//=================================================================================================

void BndLib_AddSurface::AddOptimal(const Adaptor3d_Surface& S, const double Tol, Bnd_Box& B)
{

  BndLib_AddSurface::AddOptimal(S,
                                S.FirstUParameter(),
                                S.LastUParameter(),
                                S.FirstVParameter(),
                                S.LastVParameter(),
                                Tol,
                                B);
}

//=================================================================================================

void BndLib_AddSurface::AddOptimal(const Adaptor3d_Surface& S,
                                   const double             UMin,
                                   const double             UMax,
                                   const double             VMin,
                                   const double             VMax,
                                   const double             Tol,
                                   Bnd_Box&                 B)
{
  GeomAbs_SurfaceType Type = S.GetType();

  if (Precision::IsInfinite(VMin) || Precision::IsInfinite(VMax) || Precision::IsInfinite(UMin)
      || Precision::IsInfinite(UMax))
  {
    switch (Type)
    {
      case GeomAbs_Plane: {
        TreatInfinitePlane(S.Plane(), UMin, UMax, VMin, VMax, Tol, B);
        return;
      }
      default: {
        B.SetWhole();
        return;
      }
    }
  }

  switch (Type)
  {

    case GeomAbs_Plane: {
      gp_Pln Plan = S.Plane();
      B.Add(ElSLib::Value(UMin, VMin, Plan));
      B.Add(ElSLib::Value(UMin, VMax, Plan));
      B.Add(ElSLib::Value(UMax, VMin, Plan));
      B.Add(ElSLib::Value(UMax, VMax, Plan));
      B.Enlarge(Tol);
      break;
    }
    case GeomAbs_Cylinder: {
      BndLib::Add(S.Cylinder(), UMin, UMax, VMin, VMax, Tol, B);
      break;
    }
    case GeomAbs_Cone: {
      BndLib::Add(S.Cone(), UMin, UMax, VMin, VMax, Tol, B);
      break;
    }
    case GeomAbs_Sphere: {
      BndLib::Add(S.Sphere(), UMin, UMax, VMin, VMax, Tol, B);
      break;
    }
    case GeomAbs_BezierSurface: {
      addBezierSurfBBox3d(S.Bezier(), Tol, B);
      break;
    }
    case GeomAbs_BSplineSurface: {
      occ::handle<Geom_BSplineSurface> aBS = S.BSpline();
      double                           anUMinParam, anUMaxParam, aVMinParam, aVMaxParam;
      aBS->Bounds(anUMinParam, anUMaxParam, aVMinParam, aVMaxParam);

      const double PTol = Precision::Parametric(Precision::Confusion());
      if (std::abs(UMin - anUMinParam) > PTol || std::abs(UMax - anUMaxParam) > PTol
          || std::abs(VMin - aVMinParam) > PTol || std::abs(VMax - aVMaxParam) > PTol)
      {
        occ::handle<Geom_Geometry>       aG = aBS->Copy();
        occ::handle<Geom_BSplineSurface> aBSaux(occ::down_cast<Geom_BSplineSurface>(aG));
        double                           u1 = UMin, u2 = UMax, v1 = VMin, v2 = VMax;
        if (!aBSaux->IsUPeriodic())
        {
          u1 = std::max(u1, anUMinParam);
          u2 = std::min(u2, anUMaxParam);
        }
        if (!aBSaux->IsVPeriodic())
        {
          v1 = std::max(v1, aVMinParam);
          v2 = std::min(v2, aVMaxParam);
        }
        double aUSegTol = 2. * Precision::PConfusion();
        double aVSegTol = 2. * Precision::PConfusion();
        if (std::abs(u2 - u1) < aUSegTol)
          aUSegTol = std::abs(u2 - u1) * 0.01;
        if (std::abs(v2 - v1) < aVSegTol)
          aVSegTol = std::abs(v2 - v1) * 0.01;
        aBSaux->Segment(u1, u2, v1, v2, aUSegTol, aVSegTol);
        aBS = aBSaux;
      }
      addBSplineSurfBBox3d(aBS, Tol, B);
      break;
    }
    default: {
      AddGenSurf(S, UMin, UMax, VMin, VMax, Tol, B);
    }
  }
}

//=================================================================================================

void BndLib_AddSurface::AddGenSurf(const Adaptor3d_Surface& S,
                                   const double             UMin,
                                   const double             UMax,
                                   const double             VMin,
                                   const double             VMax,
                                   const double             Tol,
                                   Bnd_Box&                 B)
{
  const int Nu = NbUSamples(S, UMin, UMax);
  const int Nv = NbVSamples(S, VMin, VMax);
  //
  double CoordMin[3] = {RealLast(), RealLast(), RealLast()};
  double CoordMax[3] = {-RealLast(), -RealLast(), -RealLast()};
  double DeflMax[3]  = {-RealLast(), -RealLast(), -RealLast()};
  //
  const double du = (UMax - UMin) / (Nu - 1), du2 = du / 2.;
  const double dv = (VMax - VMin) / (Nv - 1), dv2 = dv / 2.;

  // Use batch grid evaluation with finer grid (2*Nu-1) x (2*Nv-1) to include midpoints
  const int NuFine = 2 * Nu - 1;
  const int NvFine = 2 * Nv - 1;

  NCollection_Array1<double> aUParams(1, NuFine);
  NCollection_Array1<double> aVParams(1, NvFine);

  for (int i = 1; i <= NuFine; i++)
  {
    aUParams.SetValue(i, UMin + (i - 1) * du2);
  }
  for (int j = 1; j <= NvFine; j++)
  {
    aVParams.SetValue(j, VMin + (j - 1) * dv2);
  }

  GeomGridEval_Surface anEvaluator;
  anEvaluator.Initialize(S);

  const NCollection_Array2<gp_Pnt> aFineGrid = anEvaluator.EvaluateGrid(aUParams, aVParams);

  // Extract main grid points (at even indices in fine grid: 1, 3, 5, ...)
  // Main grid indices in fine grid: iFine = 2*i - 1, jFine = 2*j - 1
  NCollection_Array2<gp_XYZ> aPnts(1, Nu, 1, Nv);

  int i, j, k;
  for (i = 1; i <= Nu; i++)
  {
    const int iFine = 2 * i - 1;
    for (j = 1; j <= Nv; j++)
    {
      const int     jFine = 2 * j - 1;
      const gp_Pnt& P     = aFineGrid.Value(iFine, jFine);
      aPnts(i, j)         = P.XYZ();
      //
      for (k = 0; k < 3; ++k)
      {
        if (CoordMin[k] > P.Coord(k + 1))
        {
          CoordMin[k] = P.Coord(k + 1);
        }
        if (CoordMax[k] < P.Coord(k + 1))
        {
          CoordMax[k] = P.Coord(k + 1);
        }
      }
      //
      // U-midpoint: between (i-1, j) and (i, j) in main grid
      // Fine grid index: (2*i - 2, 2*j - 1) = (iFine - 1, jFine)
      if (i > 1)
      {
        const gp_XYZ  aPm = 0.5 * (aPnts(i - 1, j) + aPnts(i, j));
        const gp_Pnt& PM  = aFineGrid.Value(iFine - 1, jFine);
        const gp_XYZ  aD  = (PM.XYZ() - aPm);
        for (k = 0; k < 3; ++k)
        {
          if (CoordMin[k] > PM.Coord(k + 1))
          {
            CoordMin[k] = PM.Coord(k + 1);
          }
          if (CoordMax[k] < PM.Coord(k + 1))
          {
            CoordMax[k] = PM.Coord(k + 1);
          }
          const double d = std::abs(aD.Coord(k + 1));
          if (DeflMax[k] < d)
          {
            DeflMax[k] = d;
          }
        }
      }
      // V-midpoint: between (i, j-1) and (i, j) in main grid
      // Fine grid index: (2*i - 1, 2*j - 2) = (iFine, jFine - 1)
      if (j > 1)
      {
        const gp_XYZ  aPm = 0.5 * (aPnts(i, j - 1) + aPnts(i, j));
        const gp_Pnt& PM  = aFineGrid.Value(iFine, jFine - 1);
        const gp_XYZ  aD  = (PM.XYZ() - aPm);
        for (k = 0; k < 3; ++k)
        {
          if (CoordMin[k] > PM.Coord(k + 1))
          {
            CoordMin[k] = PM.Coord(k + 1);
          }
          if (CoordMax[k] < PM.Coord(k + 1))
          {
            CoordMax[k] = PM.Coord(k + 1);
          }
          const double d = std::abs(aD.Coord(k + 1));
          if (DeflMax[k] < d)
          {
            DeflMax[k] = d;
          }
        }
      }
    }
  }
  //
  // Adjusting minmax
  double eps = std::max(Tol, Precision::Confusion());
  for (k = 0; k < 3; ++k)
  {
    double d = DeflMax[k];
    if (d <= eps)
    {
      continue;
    }

    double CMin = CoordMin[k];
    double CMax = CoordMax[k];
    for (i = 1; i <= Nu; ++i)
    {
      for (j = 1; j <= Nv; ++j)
      {
        if (aPnts(i, j).Coord(k + 1) - CMin < d)
        {
          double umin, umax, vmin, vmax;
          umin        = UMin + std::max(0, i - 2) * du;
          umax        = UMin + std::min(Nu - 1, i) * du;
          vmin        = VMin + std::max(0, j - 2) * dv;
          vmax        = VMin + std::min(Nv - 1, j) * dv;
          double cmin = AdjustExtr(S, umin, umax, vmin, vmax, CMin, k + 1, eps, true);
          if (cmin < CMin)
          {
            CMin = cmin;
          }
        }
        else if (CMax - aPnts(i, j).Coord(k + 1) < d)
        {
          double umin, umax, vmin, vmax;
          umin        = UMin + std::max(0, i - 2) * du;
          umax        = UMin + std::min(Nu - 1, i) * du;
          vmin        = VMin + std::max(0, j - 2) * dv;
          vmax        = VMin + std::min(Nv - 1, j) * dv;
          double cmax = AdjustExtr(S, umin, umax, vmin, vmax, CMax, k + 1, eps, false);
          if (cmax > CMax)
          {
            CMax = cmax;
          }
        }
      }
    }
    CoordMin[k] = CMin;
    CoordMax[k] = CMax;
  }

  B.Add(gp_Pnt(CoordMin[0], CoordMin[1], CoordMin[2]));
  B.Add(gp_Pnt(CoordMax[0], CoordMax[1], CoordMax[2]));
  B.Enlarge(eps);
}

//

//
class SurfMaxMinCoord
{
public:
  SurfMaxMinCoord(const Adaptor3d_Surface& theSurf,
                  const double             UMin,
                  const double             UMax,
                  const double             VMin,
                  const double             VMax,
                  const int                CoordIndx,
                  const double             Sign)
      : mySurf(theSurf),
        myUMin(UMin),
        myUMax(UMax),
        myVMin(VMin),
        myVMax(VMax),
        myCoordIndx(CoordIndx),
        mySign(Sign)
  {
  }

  bool Value(const math_Vector& X, double& F)
  {
    if (X(1) < myUMin || X(1) > myUMax || X(2) < myVMin || X(2) > myVMax)
    {
      return false;
    }
    gp_Pnt aP = mySurf.Value(X(1), X(2));
    F         = mySign * aP.Coord(myCoordIndx);
    return true;
  }

private:
  SurfMaxMinCoord& operator=(const SurfMaxMinCoord&) = delete;

  const Adaptor3d_Surface& mySurf;
  double                   myUMin;
  double                   myUMax;
  double                   myVMin;
  double                   myVMax;
  int                      myCoordIndx;
  double                   mySign;
};

//=================================================================================================

double AdjustExtr(const Adaptor3d_Surface& S,
                  const double             UMin,
                  const double             UMax,
                  const double             VMin,
                  const double             VMax,
                  const double             Extr0,
                  const int                CoordIndx,
                  const double             Tol,
                  const bool               IsMin)
{
  double aSign = IsMin ? 1. : -1.;
  double extr  = aSign * Extr0;
  double Du    = (S.LastUParameter() - S.FirstUParameter());
  double Dv    = (S.LastVParameter() - S.FirstVParameter());

  math_Vector aLowBorder(1, 2);
  math_Vector aUppBorder(1, 2);
  aLowBorder(1) = UMin;
  aUppBorder(1) = UMax;
  aLowBorder(2) = VMin;
  aUppBorder(2) = VMax;

  int aNbU         = std::max(8, RealToInt(32 * (UMax - UMin) / Du));
  int aNbV         = std::max(8, RealToInt(32 * (VMax - VMin) / Dv));
  int aNbParticles = aNbU * aNbV;

  SurfMaxMinCoord         aFunc(S, UMin, UMax, VMin, VMax, CoordIndx, aSign);
  MathOpt::PSOConfig      aPSOConfig(aNbParticles, 100);
  MathUtils::VectorResult aPSOResult = MathOpt::PSO(aFunc, aLowBorder, aUppBorder, aPSOConfig);

  if (aPSOResult.IsDone())
  {
    // Refine with Powell
    MathUtils::Config       aPowellConfig(Tol, 200);
    MathUtils::VectorResult aPowellResult =
      MathOpt::Powell(aFunc, *aPSOResult.Solution, aPowellConfig);

    if (aPowellResult.IsDone())
    {
      return aSign * (*aPowellResult.Value);
    }
    return aSign * (*aPSOResult.Value);
  }

  return aSign * extr;
}

//=================================================================================================

int NbUSamples(const Adaptor3d_Surface& S, const double Umin, const double Umax)
{
  int                 N;
  GeomAbs_SurfaceType Type = S.GetType();
  switch (Type)
  {
    case GeomAbs_BezierSurface: {
      N = 2 * S.NbUPoles();
      // By default parametric range of Bezier surf is [0, 1] [0, 1]
      double du = Umax - Umin;
      if (du < .9)
      {
        N = RealToInt(du * N) + 1;
        N = std::max(N, 5);
      }
      break;
    }
    case GeomAbs_BSplineSurface: {
      const occ::handle<Geom_BSplineSurface>& BS = S.BSpline();
      N                                          = 2 * (BS->UDegree() + 1) * (BS->NbUKnots() - 1);
      double umin, umax, vmin, vmax;
      BS->Bounds(umin, umax, vmin, vmax);
      double du = (Umax - Umin) / (umax - umin);
      if (du < .9)
      {
        N = RealToInt(du * N) + 1;
        N = std::max(N, 5);
      }
      break;
    }
    default:
      N = 33;
  }
  return std::min(50, N);
}

//=================================================================================================

int NbVSamples(const Adaptor3d_Surface& S, const double Vmin, const double Vmax)
{
  int                 N;
  GeomAbs_SurfaceType Type = S.GetType();
  switch (Type)
  {
    case GeomAbs_BezierSurface: {
      N = 2 * S.NbVPoles();
      // By default parametric range of Bezier surf is [0, 1] [0, 1]
      double dv = Vmax - Vmin;
      if (dv < .9)
      {
        N = RealToInt(dv * N) + 1;
        N = std::max(N, 5);
      }
      break;
    }
    case GeomAbs_BSplineSurface: {
      const occ::handle<Geom_BSplineSurface>& BS = S.BSpline();
      N                                          = 2 * (BS->VDegree() + 1) * (BS->NbVKnots() - 1);
      double umin, umax, vmin, vmax;
      BS->Bounds(umin, umax, vmin, vmax);
      double dv = (Vmax - Vmin) / (vmax - vmin);
      if (dv < .9)
      {
        N = RealToInt(dv * N) + 1;
        N = std::max(N, 5);
      }
      break;
    }
    default:
      N = 33;
  }
  return std::min(50, N);
}
