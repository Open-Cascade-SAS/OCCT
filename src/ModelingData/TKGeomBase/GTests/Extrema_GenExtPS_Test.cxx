// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <gtest/gtest.h>

#include <Extrema_GenExtPS.hxx>
#include <Extrema_POnSurf.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <Precision.hxx>
#include <gp_Pnt.hxx>

#include <algorithm>
#include <cmath>

namespace
{
//! Returns a bicubic B-spline surface on [0, 1] x [0, 1] with theNbPoles x theNbPoles poles
//! forming waves of the given amplitude.
occ::handle<Geom_BSplineSurface> makeWavySurface(const int theNbPoles, const double theAmplitude)
{
  const int                  aDegree  = 3;
  const int                  aNbKnots = theNbPoles - aDegree + 1;
  NCollection_Array2<gp_Pnt> aPoles(1, theNbPoles, 1, theNbPoles);
  for (int i = 1; i <= theNbPoles; ++i)
  {
    for (int j = 1; j <= theNbPoles; ++j)
    {
      const double aX = (i - 1.0) / (theNbPoles - 1.0);
      const double aY = (j - 1.0) / (theNbPoles - 1.0);
      aPoles(i, j) =
        gp_Pnt(aX, aY, theAmplitude * std::sin(7.0 * aX) * std::cos(5.0 * aY + 0.3 * i));
    }
  }
  NCollection_Array1<double> aKnots(1, aNbKnots);
  NCollection_Array1<int>    aMults(1, aNbKnots);
  for (int i = 1; i <= aNbKnots; ++i)
  {
    aKnots(i) = (i - 1.0) / (aNbKnots - 1.0);
    aMults(i) = (i == 1 || i == aNbKnots) ? aDegree + 1 : 1;
  }
  return new Geom_BSplineSurface(aPoles, aKnots, aKnots, aMults, aMults, aDegree, aDegree);
}

//! Returns the points to project: above, below and around the unit square
NCollection_Array1<gp_Pnt> pointsToProject()
{
  NCollection_Array1<gp_Pnt> aPoints(1, 64);
  for (int i = 1; i <= aPoints.Length(); ++i)
  {
    const double aT = 0.37 * i;
    aPoints(i) =
      gp_Pnt(0.5 + 0.7 * std::cos(aT), 0.5 + 0.7 * std::sin(1.3 * aT), 0.4 * std::sin(2.1 * aT));
  }
  return aPoints;
}

//! Checks that both algorithms found the same extrema
void expectSameExtrema(const Extrema_GenExtPS& theExtrema, const Extrema_GenExtPS& theReference)
{
  ASSERT_EQ(theExtrema.IsDone(), theReference.IsDone());
  if (!theReference.IsDone())
  {
    return;
  }
  ASSERT_EQ(theExtrema.NbExt(), theReference.NbExt());
  for (int k = 1; k <= theReference.NbExt(); ++k)
  {
    EXPECT_EQ(theExtrema.SquareDistance(k), theReference.SquareDistance(k));
    double aU1, aV1, aU2, aV2;
    theExtrema.Point(k).Parameter(aU1, aV1);
    theReference.Point(k).Parameter(aU2, aV2);
    EXPECT_EQ(aU1, aU2);
    EXPECT_EQ(aV1, aV2);
  }
}
} // namespace

// A projector performed for many points, then initialized again for surfaces with grids of
// other sizes, gives the same results as new projectors.
TEST(Extrema_GenExtPSTest, ReusedProjectorGivesSameResultsAsNewOne)
{
  const occ::handle<Geom_BSplineSurface> aSurfaces[] = {makeWavySurface(12, 0.15),
                                                        makeWavySurface(30, 0.1),
                                                        makeWavySurface(6, 0.2)};
  const NCollection_Array1<gp_Pnt>       aPoints     = pointsToProject();
  const double                           aTol        = Precision::PConfusion();
  for (const Extrema_ExtFlag aFlag : {Extrema_ExtFlag_MIN, Extrema_ExtFlag_MINMAX})
  {
    Extrema_GenExtPS aReused;
    aReused.SetFlag(aFlag);
    for (const occ::handle<Geom_BSplineSurface>& aSurface : aSurfaces)
    {
      const GeomAdaptor_Surface anAdaptor(aSurface);
      aReused.Initialize(anAdaptor, 10, 10, aTol, aTol);
      for (const gp_Pnt& aPoint : aPoints)
      {
        SCOPED_TRACE(::testing::Message() << aFlag << ' ' << aSurface->NbUPoles() << ' '
                                          << aPoint.X() << ' ' << aPoint.Y() << ' ' << aPoint.Z());
        aReused.Perform(aPoint);
        const Extrema_GenExtPS aNew(aPoint, anAdaptor, 10, 10, aTol, aTol, aFlag);
        expectSameExtrema(aReused, aNew);
      }
    }
  }
}

// For points close above the middle of the surface, so that the closest point is not on its
// boundary, the minimal distance found is not larger than the distance to a dense sampling
TEST(Extrema_GenExtPSTest, MinimumIsNotAboveDenseSampling)
{
  const occ::handle<Geom_BSplineSurface> aSurface = makeWavySurface(30, 0.1);
  const GeomAdaptor_Surface              anAdaptor(aSurface);
  const double                           aTol = Precision::PConfusion();
  Extrema_GenExtPS                       anExtrema;
  anExtrema.SetFlag(Extrema_ExtFlag_MIN);
  anExtrema.Initialize(anAdaptor, 10, 10, aTol, aTol);
  for (int i = 1; i <= 64; ++i)
  {
    const double aT = 0.37 * i;
    const gp_Pnt aPoint(0.5 + 0.3 * std::cos(aT),
                        0.5 + 0.3 * std::sin(1.3 * aT),
                        0.12 * std::sin(2.1 * aT));
    SCOPED_TRACE(::testing::Message() << aPoint.X() << ' ' << aPoint.Y() << ' ' << aPoint.Z());
    anExtrema.Perform(aPoint);
    ASSERT_TRUE(anExtrema.IsDone());
    ASSERT_GT(anExtrema.NbExt(), 0);
    double aMinFound = RealLast();
    for (int k = 1; k <= anExtrema.NbExt(); ++k)
    {
      aMinFound = std::min(aMinFound, anExtrema.SquareDistance(k));
    }
    double    aMinSampled = RealLast();
    const int aNbSamples  = 200;
    for (int i = 0; i <= aNbSamples; ++i)
    {
      for (int j = 0; j <= aNbSamples; ++j)
      {
        const gp_Pnt aP = aSurface->EvalD0(double(i) / aNbSamples, double(j) / aNbSamples);
        aMinSampled     = std::min(aMinSampled, aPoint.SquareDistance(aP));
      }
    }
    EXPECT_LE(std::sqrt(aMinFound), std::sqrt(aMinSampled) + 1.e-9);
  }
}
