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

#include <GeomBndLib_BezierSurface.hxx>

#include <GeomBndLib_OptimizationHelpers.pxx>
#include <GeomBndLib_SamplingHelpers.pxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomGridEval_Surface.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array2.hxx>
#include <Precision.hxx>

//=================================================================================================

void GeomBndLib_BezierSurface::Add(double theTol, Bnd_Box& theBox) const
{
  // For full Bezier surface, use convex hull of all poles.
  const NCollection_Array2<gp_Pnt>& aPoles = myGeom->Poles();
  for (int i = aPoles.LowerRow(); i <= aPoles.UpperRow(); i++)
  {
    for (int j = aPoles.LowerCol(); j <= aPoles.UpperCol(); j++)
    {
      theBox.Add(aPoles.Value(i, j));
    }
  }
  theBox.Enlarge(theTol);
}

//=================================================================================================

void GeomBndLib_BezierSurface::Add(double   theUMin,
                               double   theUMax,
                               double   theVMin,
                               double   theVMax,
                               double   theTol,
                               Bnd_Box& theBox) const
{
  const double PTol = Precision::Parametric(Precision::Confusion());

  // Check if the parameters match the full surface bounds.
  double aU1, aU2, aV1, aV2;
  myGeom->Bounds(aU1, aU2, aV1, aV2);
  if (std::abs(theUMin - aU1) <= PTol
      && std::abs(theVMin - aV1) <= PTol
      && std::abs(theUMax - aU2) <= PTol
      && std::abs(theVMax - aV2) <= PTol)
  {
    // Full surface: use convex hull of all poles.
    Add(theTol, theBox);
    return;
  }

  // Trimmed Bezier: fall back to grid sampling.
  GeomAdaptor_Surface aGASurf(myGeom);
  const int           Nu = GeomBndLib_SamplingHelpers::ComputeNbUSamples(aGASurf, theUMin, theUMax);
  const int           Nv = GeomBndLib_SamplingHelpers::ComputeNbVSamples(aGASurf, theVMin, theVMax);

  NCollection_Array1<double> aUParams(1, Nu);
  NCollection_Array1<double> aVParams(1, Nv);

  for (int i = 1; i <= Nu; i++)
  {
    aUParams.SetValue(i, theUMin + ((theUMax - theUMin) * (i - 1) / (Nu - 1)));
  }
  for (int j = 1; j <= Nv; j++)
  {
    aVParams.SetValue(j, theVMin + ((theVMax - theVMin) * (j - 1) / (Nv - 1)));
  }

  GeomGridEval_Surface                anEvaluator(aGASurf);
  const NCollection_Array2<gp_Pnt> aGrid = anEvaluator.EvaluateGrid(aUParams, aVParams);
  for (int i = aGrid.LowerRow(); i <= aGrid.UpperRow(); i++)
  {
    for (int j = aGrid.LowerCol(); j <= aGrid.UpperCol(); j++)
    {
      theBox.Add(aGrid.Value(i, j));
    }
  }
  theBox.Enlarge(theTol);
}

//=================================================================================================

void GeomBndLib_BezierSurface::AddOptimal(double theTol, Bnd_Box& theBox) const
{
  double aU1, aU2, aV1, aV2;
  myGeom->Bounds(aU1, aU2, aV1, aV2);
  AddOptimal(aU1, aU2, aV1, aV2, theTol, theBox);
}

//=================================================================================================

void GeomBndLib_BezierSurface::AddOptimal(double   theUMin,
                                      double   theUMax,
                                      double   theVMin,
                                      double   theVMax,
                                      double   theTol,
                                      Bnd_Box& theBox) const
{
  GeomAdaptor_Surface aGASurf(myGeom);
  const int           Nu = GeomBndLib_SamplingHelpers::ComputeNbUSamples(aGASurf, theUMin, theUMax);
  const int           Nv = GeomBndLib_SamplingHelpers::ComputeNbVSamples(aGASurf, theVMin, theVMax);

  double CoordMin[3] = {RealLast(), RealLast(), RealLast()};
  double CoordMax[3] = {-RealLast(), -RealLast(), -RealLast()};
  double DeflMax[3]  = {-RealLast(), -RealLast(), -RealLast()};

  const double du  = (theUMax - theUMin) / (Nu - 1);
  const double du2 = du / 2.;
  const double dv  = (theVMax - theVMin) / (Nv - 1);
  const double dv2 = dv / 2.;

  // Use batch grid evaluation with finer grid to include midpoints.
  const int NuFine = 2 * Nu - 1;
  const int NvFine = 2 * Nv - 1;

  NCollection_Array1<double> aUParams(1, NuFine);
  NCollection_Array1<double> aVParams(1, NvFine);

  for (int i = 1; i <= NuFine; i++)
  {
    aUParams.SetValue(i, theUMin + (i - 1) * du2);
  }
  for (int j = 1; j <= NvFine; j++)
  {
    aVParams.SetValue(j, theVMin + (j - 1) * dv2);
  }

  GeomGridEval_Surface                anEvaluator(aGASurf);
  const NCollection_Array2<gp_Pnt> aFineGrid = anEvaluator.EvaluateGrid(aUParams, aVParams);

  NCollection_Array2<gp_XYZ> aPnts(1, Nu, 1, Nv);

  for (int i = 1; i <= Nu; i++)
  {
    const int iFine = 2 * i - 1;
    for (int j = 1; j <= Nv; j++)
    {
      const int     jFine = 2 * j - 1;
      const gp_Pnt& P     = aFineGrid.Value(iFine, jFine);
      aPnts(i, j)         = P.XYZ();

      for (int k = 0; k < 3; ++k)
      {
        if (CoordMin[k] > P.Coord(k + 1))
          CoordMin[k] = P.Coord(k + 1);
        if (CoordMax[k] < P.Coord(k + 1))
          CoordMax[k] = P.Coord(k + 1);
      }

      // U-midpoint deflection.
      if (i > 1)
      {
        const gp_XYZ  aPm = 0.5 * (aPnts(i - 1, j) + aPnts(i, j));
        const gp_Pnt& PM  = aFineGrid.Value(iFine - 1, jFine);
        const gp_XYZ  aD  = (PM.XYZ() - aPm);
        for (int k = 0; k < 3; ++k)
        {
          if (CoordMin[k] > PM.Coord(k + 1))
            CoordMin[k] = PM.Coord(k + 1);
          if (CoordMax[k] < PM.Coord(k + 1))
            CoordMax[k] = PM.Coord(k + 1);
          const double d = std::abs(aD.Coord(k + 1));
          if (DeflMax[k] < d)
            DeflMax[k] = d;
        }
      }
      // V-midpoint deflection.
      if (j > 1)
      {
        const gp_XYZ  aPm = 0.5 * (aPnts(i, j - 1) + aPnts(i, j));
        const gp_Pnt& PM  = aFineGrid.Value(iFine, jFine - 1);
        const gp_XYZ  aD  = (PM.XYZ() - aPm);
        for (int k = 0; k < 3; ++k)
        {
          if (CoordMin[k] > PM.Coord(k + 1))
            CoordMin[k] = PM.Coord(k + 1);
          if (CoordMax[k] < PM.Coord(k + 1))
            CoordMax[k] = PM.Coord(k + 1);
          const double d = std::abs(aD.Coord(k + 1));
          if (DeflMax[k] < d)
            DeflMax[k] = d;
        }
      }
    }
  }

  // Adjust minmax using PSO + Powell optimization.
  double eps = std::max(theTol, Precision::Confusion());
  for (int k = 0; k < 3; ++k)
  {
    double d = DeflMax[k];
    if (d <= eps)
      continue;

    double CMin = CoordMin[k];
    double CMax = CoordMax[k];
    for (int i = 1; i <= Nu; ++i)
    {
      for (int j = 1; j <= Nv; ++j)
      {
        if (aPnts(i, j).Coord(k + 1) - CMin < d)
        {
          double umin = theUMin + std::max(0, i - 2) * du;
          double umax = theUMin + std::min(Nu - 1, i) * du;
          double vmin = theVMin + std::max(0, j - 2) * dv;
          double vmax = theVMin + std::min(Nv - 1, j) * dv;
          double cmin =
            GeomBndLib_OptimizationHelpers::AdjustExtrSurf(aGASurf, umin, umax, vmin, vmax,
                                                       CMin, k + 1, eps, true);
          if (cmin < CMin)
            CMin = cmin;
        }
        else if (CMax - aPnts(i, j).Coord(k + 1) < d)
        {
          double umin = theUMin + std::max(0, i - 2) * du;
          double umax = theUMin + std::min(Nu - 1, i) * du;
          double vmin = theVMin + std::max(0, j - 2) * dv;
          double vmax = theVMin + std::min(Nv - 1, j) * dv;
          double cmax =
            GeomBndLib_OptimizationHelpers::AdjustExtrSurf(aGASurf, umin, umax, vmin, vmax,
                                                       CMax, k + 1, eps, false);
          if (cmax > CMax)
            CMax = cmax;
        }
      }
    }
    CoordMin[k] = CMin;
    CoordMax[k] = CMax;
  }

  theBox.Add(gp_Pnt(CoordMin[0], CoordMin[1], CoordMin[2]));
  theBox.Add(gp_Pnt(CoordMax[0], CoordMax[1], CoordMax[2]));
  theBox.Enlarge(eps);
}
