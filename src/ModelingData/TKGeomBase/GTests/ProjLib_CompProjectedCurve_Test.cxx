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
#include <GeomAbs_Shape.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Line.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <Precision.hxx>
#include <ProjLib_CompProjectedCurve.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

TEST(ProjLib_CompProjectedCurveTest, SurfaceCrossingsFollowCurveParameters)
{
  const double                     aUKnotValues[] = {0.0, 1.0};
  const double                     aVKnotValues[] = {0.0, 0.25, 0.5, 1.0};
  const int                        aUMultValues[] = {2, 2};
  const int                        aVMultValues[] = {2, 1, 1, 2};
  const double                     aYValues[]     = {0.0, 0.7, 0.9, 1.0};
  const NCollection_Array1<double> aUKnots(aUKnotValues[0], 1, 2);
  const NCollection_Array1<double> aVKnots(aVKnotValues[0], 1, 4);
  const NCollection_Array1<int>    aUMults(aUMultValues[0], 1, 2);
  const NCollection_Array1<int>    aVMults(aVMultValues[0], 1, 4);
  for (const bool isU : {false, true})
  {
    SCOPED_TRACE(isU);
    NCollection_Array2<gp_Pnt> aPoles(1, isU ? 4 : 2, 1, isU ? 2 : 4);
    for (size_t anIndex = 0; anIndex < 4; ++anIndex)
    {
      for (size_t aLinearIndex = 0; aLinearIndex < 2; ++aLinearIndex)
      {
        aPoles.ChangeAt(isU ? anIndex : aLinearIndex, isU ? aLinearIndex : anIndex) =
          gp_Pnt(isU ? aYValues[anIndex] : double(aLinearIndex),
                 isU ? double(aLinearIndex) : aYValues[anIndex],
                 0.0);
      }
    }
    const occ::handle<GeomAdaptor_Surface> aSurface =
      new GeomAdaptor_Surface(new Geom_BSplineSurface(aPoles,
                                                      isU ? aVKnots : aUKnots,
                                                      isU ? aUKnots : aVKnots,
                                                      isU ? aVMults : aUMults,
                                                      isU ? aUMults : aVMults,
                                                      1,
                                                      1));
    for (const bool isReversed : {false, true})
    {
      for (const bool isTrimmed : {false, true})
      {
        SCOPED_TRACE(isReversed);
        SCOPED_TRACE(isTrimmed);
        const double                         aStart     = isReversed ? 1.0 : 0.0;
        const double                         aDirection = isReversed ? -1.0 : 1.0;
        const double                         aFirst     = isTrimmed ? 0.05 : 0.0;
        const double                         aLast      = isTrimmed ? 0.95 : 1.0;
        const occ::handle<GeomAdaptor_Curve> aCurve     = new GeomAdaptor_Curve(
          new Geom_Line(gp_Pnt(isU ? aStart : 0.5, isU ? 0.5 : aStart, 0.0),
                        gp_Dir(isU ? aDirection : 0.0, isU ? 0.0 : aDirection, 0.0)),
          aFirst,
          aLast);
        ProjLib_CompProjectedCurve aProjection(Precision::Confusion(), aSurface, aCurve);
        ASSERT_EQ(aProjection.NbCurves(), 1);
        for (const GeomAbs_Shape aContinuity : {GeomAbs_C1, GeomAbs_C2})
        {
          SCOPED_TRACE(aContinuity);
          const int aNbIntervals = aProjection.NbIntervals(aContinuity);
          ASSERT_EQ(aNbIntervals, 3);
          NCollection_Array1<double> anIntervals(1, aNbIntervals + 1);
          aProjection.Intervals(anIntervals, aContinuity);
          EXPECT_DOUBLE_EQ(anIntervals.At(0), aFirst);
          EXPECT_NEAR(anIntervals.At(1), isReversed ? 0.1 : 0.7, Precision::PConfusion());
          EXPECT_NEAR(anIntervals.At(2), isReversed ? 0.3 : 0.9, Precision::PConfusion());
          EXPECT_DOUBLE_EQ(anIntervals.At(3), aLast);
        }
      }
    }
  }
}
