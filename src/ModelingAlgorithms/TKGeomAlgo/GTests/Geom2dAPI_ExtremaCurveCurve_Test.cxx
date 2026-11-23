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

#include <Geom2dAPI_ExtremaCurveCurve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>

// Migrated from tests/bugs/moddata_3/bug27467. Two bounded, intersecting lines
// must yield one zero-distance extrema solution.
TEST(Geom2dAPI_ExtremaCurveCurveTest, ModDataBug_27467_IntersectingTrimmedLines)
{
  const occ::handle<Geom2d_Line> aLine1 =
    new Geom2d_Line(gp_Lin2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(0.0, -1.0)));
  const occ::handle<Geom2d_Line> aLine2 =
    new Geom2d_Line(gp_Lin2d(gp_Pnt2d(1.0, -9.5), gp_Dir2d(-1.0, 0.0)));
  const occ::handle<Geom2d_TrimmedCurve> aTrimmedLine1 = new Geom2d_TrimmedCurve(aLine1, 0.0, 23.0);
  const occ::handle<Geom2d_TrimmedCurve> aTrimmedLine2 = new Geom2d_TrimmedCurve(aLine2, 0.0, 2.0);

  Geom2dAPI_ExtremaCurveCurve anExtrema(aTrimmedLine1,
                                        aTrimmedLine2,
                                        aTrimmedLine1->FirstParameter(),
                                        aTrimmedLine1->LastParameter(),
                                        aTrimmedLine2->FirstParameter(),
                                        aTrimmedLine2->LastParameter());
  ASSERT_EQ(anExtrema.NbExtrema(), 1);

  gp_Pnt2d aPoint1, aPoint2;
  anExtrema.Points(1, aPoint1, aPoint2);
  EXPECT_NEAR(anExtrema.Distance(1), 0.0, 1.0e-9);
  EXPECT_NEAR(aPoint1.X(), 0.0, 1.0e-9);
  EXPECT_NEAR(aPoint1.Y(), -9.5, 1.0e-9);
  EXPECT_NEAR(aPoint2.X(), 0.0, 1.0e-9);
  EXPECT_NEAR(aPoint2.Y(), -9.5, 1.0e-9);
}
