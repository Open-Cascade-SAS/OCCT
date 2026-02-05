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
#include <ElSLib.hxx>
#include <ElCLib.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomGridEval_Surface.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Cone.hxx>
#include <Precision.hxx>
#include <NCollection_Array2.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <math_PSO.hxx>
#include <math_Powell.hxx>
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

static void ComputePolesIndexes(const NCollection_Array1<double>& theKnots,
                                const NCollection_Array1<int>&    theMults,
                                const int                         theDegree,
                                const double                      theMin,
                                const double                      theMax,
                                const int                         theMaxPoleIdx,
                                const bool                        theIsPeriodic,
                                int&                              theOutMinIdx,
                                int&                              theOutMaxIdx);

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

// Compute start and finish indexes used in convex hull.
// theMinIdx - minimum poles index, that can be used.
// theMaxIdx - maximum poles index, that can be used.
// theShiftCoeff - shift between flatknots array and poles array.
// This value should be equal to 1 in case of non periodic BSpline,
// and (degree + 1) - mults(the lowest index).

void ComputePolesIndexes(const NCollection_Array1<double>& theKnots,
                         const NCollection_Array1<int>&    theMults,
                         const int                         theDegree,
                         const double                      theMin,
                         const double                      theMax,
                         const int                         theMaxPoleIdx,
                         const bool                        theIsPeriodic,
                         int&                              theOutMinIdx,
                         int&                              theOutMaxIdx)
{
  BSplCLib::Hunt(theKnots, theMin, theOutMinIdx);
  theOutMinIdx = std::clamp(theOutMinIdx, theKnots.Lower(), theKnots.Upper());

  BSplCLib::Hunt(theKnots, theMax, theOutMaxIdx);
  theOutMaxIdx++;
  theOutMaxIdx          = std::clamp(theOutMaxIdx, theKnots.Lower(), theKnots.Upper());
  const int aMultiplier = theMults(theOutMaxIdx);

  theOutMinIdx = BSplCLib::PoleIndex(theDegree, theOutMinIdx, theIsPeriodic, theMults) + 1;
  theOutMinIdx = std::max(theOutMinIdx, 1);
  theOutMaxIdx = BSplCLib::PoleIndex(theDegree, theOutMaxIdx, theIsPeriodic, theMults) + 1;
  theOutMaxIdx += theDegree - aMultiplier;
  if (!theIsPeriodic)
    theOutMaxIdx = std::min(theOutMaxIdx, theMaxPoleIdx);
}

//  Modified by skv - Fri Aug 27 12:29:04 2004 OCC6503 End
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
    case GeomAbs_BezierSurface:
    case GeomAbs_BSplineSurface: {
      bool   isUseConvexHullAlgorithm = true;
      double PTol                     = Precision::Parametric(Precision::Confusion());
      // Borders of underlying geometry.
      double anUMinParam = UMin, anUMaxParam = UMax, // BSpline case.
        aVMinParam = VMin, aVMaxParam = VMax;
      occ::handle<Geom_BSplineSurface> aBS;
      if (Type == GeomAbs_BezierSurface)
      {
        // Bezier surface:
        // All of poles used for any parameter,
        // that's why in case of trimmed parameters handled by grid algorithm.

        if (std::abs(UMin - S.FirstUParameter()) > PTol
            || std::abs(VMin - S.FirstVParameter()) > PTol
            || std::abs(UMax - S.LastUParameter()) > PTol
            || std::abs(VMax - S.LastVParameter()) > PTol)
        {
          // Borders not equal to topology borders.
          isUseConvexHullAlgorithm = false;
        }
      }
      else
      {
        // BSpline:
        // If Umin, Vmin, Umax, Vmax lies inside geometry bounds then:
        // use convex hull algorithm,
        // if Umin, VMin, Umax, Vmax lies outside then:
        // use grid algorithm on analytic continuation (default case).
        aBS = S.BSpline();
        aBS->Bounds(anUMinParam, anUMaxParam, aVMinParam, aVMaxParam);
        if ((UMin - anUMinParam) < -PTol || (VMin - aVMinParam) < -PTol
            || (UMax - anUMaxParam) > PTol || (VMax - aVMaxParam) > PTol)
        {
          // Out of geometry borders.
          isUseConvexHullAlgorithm = false;
        }
      }

      if (isUseConvexHullAlgorithm)
      {
        int                        aNbUPoles = S.NbUPoles(), aNbVPoles = S.NbVPoles();
        NCollection_Array2<gp_Pnt> Tp(1, aNbUPoles, 1, aNbVPoles);
        int                        UMinIdx = 0, UMaxIdx = 0;
        int                        VMinIdx = 0, VMaxIdx = 0;
        bool                       isUPeriodic = S.IsUPeriodic(), isVPeriodic = S.IsVPeriodic();
        if (Type == GeomAbs_BezierSurface)
        {
          S.Bezier()->Poles(Tp);
          UMinIdx = 1;
          UMaxIdx = aNbUPoles;
          VMinIdx = 1;
          VMaxIdx = aNbVPoles;
        }
        else
        {
          aBS->Poles(Tp);

          UMinIdx = 1;
          UMaxIdx = aNbUPoles;
          VMinIdx = 1;
          VMaxIdx = aNbVPoles;

          if (UMin > anUMinParam || UMax < anUMaxParam)
          {
            NCollection_Array1<int>    aMults(1, aBS->NbUKnots());
            NCollection_Array1<double> aKnots(1, aBS->NbUKnots());
            aBS->UKnots(aKnots);
            aBS->UMultiplicities(aMults);

            ComputePolesIndexes(aKnots,
                                aMults,
                                aBS->UDegree(),
                                UMin,
                                UMax,
                                aNbUPoles,
                                isUPeriodic,
                                UMinIdx,
                                UMaxIdx); // the Output indexes
          }

          if (VMin > aVMinParam || VMax < aVMaxParam)
          {
            NCollection_Array1<int>    aMults(1, aBS->NbVKnots());
            NCollection_Array1<double> aKnots(1, aBS->NbVKnots());
            aBS->VKnots(aKnots);
            aBS->VMultiplicities(aMults);

            ComputePolesIndexes(aKnots,
                                aMults,
                                aBS->VDegree(),
                                VMin,
                                VMax,
                                aNbVPoles,
                                isVPeriodic,
                                VMinIdx,
                                VMaxIdx); // the Output indexes
          }
        }

        // Use poles to build convex hull.
        int ip, jp;
        for (int i = UMinIdx; i <= UMaxIdx; i++)
        {
          ip = i;
          if (isUPeriodic && ip > aNbUPoles)
          {
            ip = ip - aNbUPoles;
          }
          for (int j = VMinIdx; j <= VMaxIdx; j++)
          {
            jp = j;
            if (isVPeriodic && jp > aNbVPoles)
            {
              jp = jp - aNbVPoles;
            }
            B.Add(Tp(ip, jp));
          }
        }

        B.Enlarge(Tol);
        break;
      }
    }
      [[fallthrough]];
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
class SurfMaxMinCoord : public math_MultipleVarFunction
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
        mySign(Sign),
        myPenalty(0.)
  {
    math_Vector X(1, 2);
    X(1) = UMin;
    X(2) = (VMin + VMax) / 2.;
    double F1, F2;
    Value(X, F1);
    X(1) = UMax;
    Value(X, F2);
    double DU = std::abs((F2 - F1) / (UMax - UMin));
    X(1)      = (UMin + UMax) / 2.;
    X(2)      = VMin;
    Value(X, F1);
    X(2) = VMax;
    Value(X, F2);
    double DV = std::abs((F2 - F1) / (VMax - VMin));
    myPenalty = 10. * std::max(DU, DV);
    myPenalty = std::max(myPenalty, 1.);
  }

  bool Value(const math_Vector& X, double& F) override
  {
    if (CheckInputData(X))
    {
      gp_Pnt aP = mySurf.Value(X(1), X(2));
      F         = mySign * aP.Coord(myCoordIndx);
    }
    else
    {
      double UPen = 0., VPen = 0., u0, v0;
      if (X(1) < myUMin)
      {
        UPen = myPenalty * (myUMin - X(1));
        u0   = myUMin;
      }
      else if (X(1) > myUMax)
      {
        UPen = myPenalty * (X(1) - myUMax);
        u0   = myUMax;
      }
      else
      {
        u0 = X(1);
      }
      //
      if (X(2) < myVMin)
      {
        VPen = myPenalty * (myVMin - X(2));
        v0   = myVMin;
      }
      else if (X(2) > myVMax)
      {
        VPen = myPenalty * (X(2) - myVMax);
        v0   = myVMax;
      }
      else
      {
        v0 = X(2);
      }
      //
      gp_Pnt aP = mySurf.Value(u0, v0);
      F         = mySign * aP.Coord(myCoordIndx) + UPen + VPen;
    }

    return true;
  }

  int NbVariables() const override { return 2; }

private:
  SurfMaxMinCoord& operator=(const SurfMaxMinCoord&) = delete;

  bool CheckInputData(const math_Vector& theParams)
  {
    return theParams(1) >= myUMin && theParams(1) <= myUMax && theParams(2) >= myVMin
           && theParams(2) <= myVMax;
  }

  const Adaptor3d_Surface& mySurf;
  double                   myUMin;
  double                   myUMax;
  double                   myVMin;
  double                   myVMax;
  int                      myCoordIndx;
  double                   mySign;
  double                   myPenalty;
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
  double aSign  = IsMin ? 1. : -1.;
  double extr   = aSign * Extr0;
  double relTol = 2. * Tol;
  if (std::abs(extr) > Tol)
  {
    relTol /= std::abs(extr);
  }
  double Du = (S.LastUParameter() - S.FirstUParameter());
  double Dv = (S.LastVParameter() - S.FirstVParameter());
  //
  math_Vector aT(1, 2);
  math_Vector aLowBorder(1, 2);
  math_Vector aUppBorder(1, 2);
  math_Vector aSteps(1, 2);
  aLowBorder(1) = UMin;
  aUppBorder(1) = UMax;
  aLowBorder(2) = VMin;
  aUppBorder(2) = VMax;

  int    aNbU         = std::max(8, RealToInt(32 * (UMax - UMin) / Du));
  int    aNbV         = std::max(8, RealToInt(32 * (VMax - VMin) / Dv));
  int    aNbParticles = aNbU * aNbV;
  double aMaxUStep    = (UMax - UMin) / (aNbU + 1);
  aSteps(1)           = std::min(0.1 * Du, aMaxUStep);
  double aMaxVStep    = (VMax - VMin) / (aNbV + 1);
  aSteps(2)           = std::min(0.1 * Dv, aMaxVStep);

  SurfMaxMinCoord aFunc(S, UMin, UMax, VMin, VMax, CoordIndx, aSign);
  math_PSO        aFinder(&aFunc, aLowBorder, aUppBorder, aSteps, aNbParticles);
  aFinder.Perform(aSteps, extr, aT);

  // Refinement of extremal value
  math_Matrix aDir(1, 2, 1, 2, 0.0);
  aDir(1, 1) = 1.;
  aDir(2, 1) = 0.;
  aDir(1, 2) = 0.;
  aDir(2, 2) = 1.;

  int         aNbIter = 200;
  math_Powell powell(aFunc, relTol, aNbIter, Tol);
  powell.Perform(aFunc, aT, aDir);

  if (powell.IsDone())
  {
    powell.Location(aT);
    extr = powell.Minimum();
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
