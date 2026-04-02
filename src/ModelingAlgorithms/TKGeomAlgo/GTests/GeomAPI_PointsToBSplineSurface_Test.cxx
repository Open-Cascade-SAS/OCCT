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

#include <GeomAPI_PointsToBSplineSurface.hxx>

#include <Geom_BSplineSurface.hxx>
#include <GeomAbs_Shape.hxx>
#include <NCollection_Array2.hxx>
#include <Precision.hxx>
#include <StdFail_NotDone.hxx>

#include <gtest/gtest.h>

TEST(GeomAPI_PointsToBSplineSurface_Test, FailedDegenerateRebuildResetsDoneState)
{
  NCollection_Array2<double> aValidZPoints(1, 2, 1, 2);
  aValidZPoints(1, 1) = 0.0;
  aValidZPoints(1, 2) = 1.0;
  aValidZPoints(2, 1) = 1.0;
  aValidZPoints(2, 2) = 0.0;

  GeomAPI_PointsToBSplineSurface anApprox;
  EXPECT_NO_THROW(
    anApprox.Init(aValidZPoints, 0.0, 1.0, 0.0, 1.0, 3, 5, GeomAbs_C2, Precision::Confusion()));
  ASSERT_TRUE(anApprox.IsDone());

  NCollection_Array2<double> aZPoints(1, 3, 1, 1);
  aZPoints(1, 1) = 0.0;
  aZPoints(2, 1) = 1.0;
  aZPoints(3, 1) = 0.0;

  EXPECT_THROW(
    anApprox.Init(aZPoints, 0.0, 1.0, 0.0, 0.0, 3, 5, GeomAbs_C2, Precision::Confusion()),
    StdFail_NotDone);
  EXPECT_FALSE(anApprox.IsDone());
}
