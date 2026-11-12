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

#include <GCPnts_TangentialDeflection.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>

#include <gtest/gtest.h>

#include <algorithm>
#include <limits>

//==================================================================================================

TEST(GCPnts_TangentialDeflectionTest, ShallowPolylineRespectsLinearDeflection)
{
  const double                     aKnotValues[] = {0.0, 0.5, 1.0};
  const int                        aMultValues[] = {2, 1, 2};
  const NCollection_Array1<double> aKnots(aKnotValues[0], 1, 3);
  const NCollection_Array1<int>    aMults(aMultValues[0], 1, 3);
  constexpr double                 aDeflection = 0.01;
  for (const double aHeight : {0.0, 0.005, 0.01, 0.02})
  {
    SCOPED_TRACE(aHeight);
    for (const bool is2d : {false, true})
    {
      SCOPED_TRACE(is2d);
      GCPnts_TangentialDeflection aSamples;
      if (is2d)
      {
        NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
        aPoles.ChangeAt(0) = gp_Pnt2d(0.0, 0.0);
        aPoles.ChangeAt(1) = gp_Pnt2d(500.0, aHeight);
        aPoles.ChangeAt(2) = gp_Pnt2d(1000.0, 0.0);
        const Geom2dAdaptor_Curve aCurve(new Geom2d_BSplineCurve(aPoles, aKnots, aMults, 1));
        aSamples.Initialize(aCurve, 0.1, aDeflection);
      }
      else
      {
        NCollection_Array1<gp_Pnt> aPoles(1, 3);
        aPoles.ChangeAt(0) = gp_Pnt(0.0, 0.0, 0.0);
        aPoles.ChangeAt(1) = gp_Pnt(500.0, aHeight, 0.0);
        aPoles.ChangeAt(2) = gp_Pnt(1000.0, 0.0, 0.0);
        const GeomAdaptor_Curve aCurve(new Geom_BSplineCurve(aPoles, aKnots, aMults, 1));
        aSamples.Initialize(aCurve, 0.1, aDeflection);
      }
      ASSERT_GE(aSamples.NbPoints(), 2);
      EXPECT_LE(aSamples.Value(1).Distance(gp_Pnt(0.0, 0.0, 0.0)), Precision::Confusion());
      EXPECT_LE(aSamples.Value(aSamples.NbPoints()).Distance(gp_Pnt(1000.0, 0.0, 0.0)),
                Precision::Confusion());

      const gp_Pnt aPeak(500.0, aHeight, 0.0);
      double       aDistance = std::numeric_limits<double>::max();
      for (int anIndex = 2; anIndex <= aSamples.NbPoints(); ++anIndex)
      {
        const gp_Pnt& aStart = aSamples.Value(anIndex - 1);
        const gp_Vec  aSegment(aStart, aSamples.Value(anIndex));
        const double  aSquareLength = aSegment.SquareMagnitude();
        const double  aParameter =
          aSquareLength == 0.0
             ? 0.0
             : std::clamp(gp_Vec(aStart, aPeak).Dot(aSegment) / aSquareLength, 0.0, 1.0);
        aDistance = std::min(aDistance, aPeak.Distance(aStart.Translated(aSegment * aParameter)));
      }
      EXPECT_LE(aDistance, aDeflection + Precision::Confusion());
    }
  }
}
