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

Bnd_Box GeomBndLib_BezierSurface::Box(double theTol) const
{
  Bnd_Box aBox;
  // For full Bezier surface, use convex hull of all poles.
  const NCollection_Array2<gp_Pnt>& aPoles = myGeom->Poles();
  for (int anI = aPoles.LowerRow(); anI <= aPoles.UpperRow(); anI++)
  {
    for (int aJ = aPoles.LowerCol(); aJ <= aPoles.UpperCol(); aJ++)
    {
      aBox.Add(aPoles.Value(anI, aJ));
    }
  }
  aBox.Enlarge(theTol);
  return aBox;
}

//=================================================================================================

Bnd_Box GeomBndLib_BezierSurface::Box(double theUMin,
                                      double theUMax,
                                      double theVMin,
                                      double theVMax,
                                      double theTol) const
{
  Bnd_Box      aBox;
  const double PTol = Precision::Parametric(Precision::Confusion());

  // Check if the parameters match the full surface bounds.
  double aU1, aU2, aV1, aV2;
  myGeom->Bounds(aU1, aU2, aV1, aV2);
  if (std::abs(theUMin - aU1) <= PTol && std::abs(theVMin - aV1) <= PTol
      && std::abs(theUMax - aU2) <= PTol && std::abs(theVMax - aV2) <= PTol)
  {
    // Full surface: use convex hull of all poles.
    return Box(theTol);
  }

  // Trimmed Bezier: fall back to grid sampling.
  GeomAdaptor_Surface aGASurf(myGeom);
  const int aNbUSamples = GeomBndLib_SamplingHelpers::ComputeNbUSamples(aGASurf);
  const int aNbVSamples = GeomBndLib_SamplingHelpers::ComputeNbVSamples(aGASurf);

  NCollection_Array1<double> aUParams(1, aNbUSamples);
  NCollection_Array1<double> aVParams(1, aNbVSamples);

  for (int anI = 1; anI <= aNbUSamples; anI++)
  {
    aUParams.SetValue(anI, theUMin + ((theUMax - theUMin) * (anI - 1) / (aNbUSamples - 1)));
  }
  for (int aJ = 1; aJ <= aNbVSamples; aJ++)
  {
    aVParams.SetValue(aJ, theVMin + ((theVMax - theVMin) * (aJ - 1) / (aNbVSamples - 1)));
  }

  const GeomGridEval_Surface       anEvaluator(aGASurf);
  const NCollection_Array2<gp_Pnt> aGrid = anEvaluator.EvaluateGrid(aUParams, aVParams);
  for (int anI = aGrid.LowerRow(); anI <= aGrid.UpperRow(); anI++)
  {
    for (int aJ = aGrid.LowerCol(); aJ <= aGrid.UpperCol(); aJ++)
    {
      aBox.Add(aGrid.Value(anI, aJ));
    }
  }
  aBox.Enlarge(theTol);
  return aBox;
}

//=================================================================================================

Bnd_Box GeomBndLib_BezierSurface::BoxOptimal(double theTol) const
{
  double aU1, aU2, aV1, aV2;
  myGeom->Bounds(aU1, aU2, aV1, aV2);
  return BoxOptimal(aU1, aU2, aV1, aV2, theTol);
}

//=================================================================================================

Bnd_Box GeomBndLib_BezierSurface::BoxOptimal(double theUMin,
                                             double theUMax,
                                             double theVMin,
                                             double theVMax,
                                             double theTol) const
{
  Bnd_Box             aBox;
  GeomAdaptor_Surface aGASurf(myGeom);
  const int aNbUSamples = GeomBndLib_SamplingHelpers::ComputeNbUSamples(aGASurf, theUMin, theUMax);
  const int aNbVSamples = GeomBndLib_SamplingHelpers::ComputeNbVSamples(aGASurf, theVMin, theVMax);

  double aCoordMin[3] = {RealLast(), RealLast(), RealLast()};
  double aCoordMax[3] = {-RealLast(), -RealLast(), -RealLast()};
  double aDeflMax[3]  = {-RealLast(), -RealLast(), -RealLast()};

  const double aDU  = (theUMax - theUMin) / (aNbUSamples - 1);
  const double aDU2 = aDU / 2.;
  const double aDV  = (theVMax - theVMin) / (aNbVSamples - 1);
  const double aDV2 = aDV / 2.;

  // Use batch grid evaluation with finer grid to include midpoints.
  const int aNbUSamplesFine = 2 * aNbUSamples - 1;
  const int aNbVSamplesFine = 2 * aNbVSamples - 1;

  NCollection_Array1<double> aUParams(1, aNbUSamplesFine);
  NCollection_Array1<double> aVParams(1, aNbVSamplesFine);

  for (int anI = 1; anI <= aNbUSamplesFine; anI++)
  {
    aUParams.SetValue(anI, theUMin + (anI - 1) * aDU2);
  }
  for (int aJ = 1; aJ <= aNbVSamplesFine; aJ++)
  {
    aVParams.SetValue(aJ, theVMin + (aJ - 1) * aDV2);
  }

  const GeomGridEval_Surface       anEvaluator(aGASurf);
  const NCollection_Array2<gp_Pnt> aFineGrid = anEvaluator.EvaluateGrid(aUParams, aVParams);

  NCollection_Array2<gp_XYZ> aPnts(1, aNbUSamples, 1, aNbVSamples);

  for (int anI = 1; anI <= aNbUSamples; anI++)
  {
    const int anIFine = 2 * anI - 1;
    for (int aJ = 1; aJ <= aNbVSamples; aJ++)
    {
      const int     aJFine = 2 * aJ - 1;
      const gp_Pnt& aP     = aFineGrid.Value(anIFine, aJFine);
      aPnts(anI, aJ)       = aP.XYZ();

      for (int aK = 0; aK < 3; ++aK)
      {
        if (aCoordMin[aK] > aP.Coord(aK + 1))
          aCoordMin[aK] = aP.Coord(aK + 1);
        if (aCoordMax[aK] < aP.Coord(aK + 1))
          aCoordMax[aK] = aP.Coord(aK + 1);
      }

      // U-midpoint deflection.
      if (anI > 1)
      {
        const gp_XYZ  aPm = 0.5 * (aPnts(anI - 1, aJ) + aPnts(anI, aJ));
        const gp_Pnt& aPM = aFineGrid.Value(anIFine - 1, aJFine);
        const gp_XYZ  aD  = (aPM.XYZ() - aPm);
        for (int aK = 0; aK < 3; ++aK)
        {
          if (aCoordMin[aK] > aPM.Coord(aK + 1))
            aCoordMin[aK] = aPM.Coord(aK + 1);
          if (aCoordMax[aK] < aPM.Coord(aK + 1))
            aCoordMax[aK] = aPM.Coord(aK + 1);
          const double aDiff = std::abs(aD.Coord(aK + 1));
          if (aDeflMax[aK] < aDiff)
            aDeflMax[aK] = aDiff;
        }
      }
      // V-midpoint deflection.
      if (aJ > 1)
      {
        const gp_XYZ  aPm = 0.5 * (aPnts(anI, aJ - 1) + aPnts(anI, aJ));
        const gp_Pnt& aPM = aFineGrid.Value(anIFine, aJFine - 1);
        const gp_XYZ  aD  = (aPM.XYZ() - aPm);
        for (int aK = 0; aK < 3; ++aK)
        {
          if (aCoordMin[aK] > aPM.Coord(aK + 1))
            aCoordMin[aK] = aPM.Coord(aK + 1);
          if (aCoordMax[aK] < aPM.Coord(aK + 1))
            aCoordMax[aK] = aPM.Coord(aK + 1);
          const double aDiff = std::abs(aD.Coord(aK + 1));
          if (aDeflMax[aK] < aDiff)
            aDeflMax[aK] = aDiff;
        }
      }
    }
  }

  // Adjust minmax using PSO + Powell optimization.
  const double anEps = std::max(theTol, Precision::Confusion());
  for (int aK = 0; aK < 3; ++aK)
  {
    const double aDiff = aDeflMax[aK];
    if (aDiff <= anEps)
      continue;

    double aCMin = aCoordMin[aK];
    double aCMax = aCoordMax[aK];
    for (int anI = 1; anI <= aNbUSamples; ++anI)
    {
      for (int aJ = 1; aJ <= aNbVSamples; ++aJ)
      {
        if (aPnts(anI, aJ).Coord(aK + 1) - aCMin < aDiff)
        {
          const double anUMin  = theUMin + std::max(0, anI - 2) * aDU;
          const double anUMax  = theUMin + std::min(aNbUSamples - 1, anI) * aDU;
          const double aVMin   = theVMin + std::max(0, aJ - 2) * aDV;
          const double aVMax   = theVMin + std::min(aNbVSamples - 1, aJ) * aDV;
          const double aLocMin = GeomBndLib_OptimizationHelpers::AdjustExtrSurf(aGASurf,
                                                                                anUMin,
                                                                                anUMax,
                                                                                aVMin,
                                                                                aVMax,
                                                                                aCMin,
                                                                                aK + 1,
                                                                                anEps,
                                                                                true);
          if (aLocMin < aCMin)
            aCMin = aLocMin;
        }
        else if (aCMax - aPnts(anI, aJ).Coord(aK + 1) < aDiff)
        {
          const double anUMin  = theUMin + std::max(0, anI - 2) * aDU;
          const double anUMax  = theUMin + std::min(aNbUSamples - 1, anI) * aDU;
          const double aVMin   = theVMin + std::max(0, aJ - 2) * aDV;
          const double aVMax   = theVMin + std::min(aNbVSamples - 1, aJ) * aDV;
          const double aLocMax = GeomBndLib_OptimizationHelpers::AdjustExtrSurf(aGASurf,
                                                                                anUMin,
                                                                                anUMax,
                                                                                aVMin,
                                                                                aVMax,
                                                                                aCMax,
                                                                                aK + 1,
                                                                                anEps,
                                                                                false);
          if (aLocMax > aCMax)
            aCMax = aLocMax;
        }
      }
    }
    aCoordMin[aK] = aCMin;
    aCoordMax[aK] = aCMax;
  }

  aBox.Add(gp_Pnt(aCoordMin[0], aCoordMin[1], aCoordMin[2]));
  aBox.Add(gp_Pnt(aCoordMax[0], aCoordMax[1], aCoordMax[2]));
  aBox.Enlarge(anEps);
  return aBox;
}
