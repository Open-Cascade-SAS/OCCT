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

#include <Geom2dAPI_PointsToBSpline.hxx>

#include <Geom2d_BSplineCurve.hxx>
#include <GeomAbs_Shape.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>
#include <gp_Pnt2d.hxx>

#include <gtest/gtest.h>

TEST(Geom2dAPI_PointsToBSpline_Test, DegenerateXRangeFallsBackToUniformParameters)
{
  NCollection_Array1<double> aYValues(1, 3);
  aYValues(1) = 0.0;
  aYValues(2) = 1.0;
  aYValues(3) = 0.0;

  Geom2dAPI_PointsToBSpline anApprox;
  EXPECT_NO_THROW(anApprox.Init(aYValues, -2.0, 1.0, 3, 5, GeomAbs_C2, Precision::Confusion()));
  ASSERT_TRUE(anApprox.IsDone());

  const occ::handle<Geom2d_BSplineCurve>& aCurve = anApprox.Curve();
  ASSERT_FALSE(aCurve.IsNull());
}

TEST(Geom2dAPI_PointsToBSpline_Test, DegenerateExplicitParametersResetDoneState)
{
  NCollection_Array1<gp_Pnt2d> aPoints(1, 3);
  aPoints(1) = gp_Pnt2d(0.0, 0.0);
  aPoints(2) = gp_Pnt2d(1.0, 1.0);
  aPoints(3) = gp_Pnt2d(2.0, 0.0);

  NCollection_Array1<double> aParameters(1, 3);
  aParameters(1) = 5.0;
  aParameters(2) = 5.0;
  aParameters(3) = 5.0;

  Geom2dAPI_PointsToBSpline anApprox(aPoints, 3, 5, GeomAbs_C2, Precision::Confusion());
  ASSERT_TRUE(anApprox.IsDone());

  EXPECT_NO_THROW(anApprox.Init(aPoints, aParameters, 3, 5, GeomAbs_C2, Precision::Confusion()));
  EXPECT_FALSE(anApprox.IsDone());
}
