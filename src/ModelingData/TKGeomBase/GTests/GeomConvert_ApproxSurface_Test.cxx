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

#include <GeomAbs_Shape.hxx>
#include <GeomConvert_ApproxSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

TEST(GeomConvert_ApproxSurfaceTest, SphereApproximationRespectsTolerance)
{
  constexpr double THE_TOLERANCE = 1.e-3;
  const occ::handle<Geom_SphericalSurface> aSphere =
    new Geom_SphericalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  const GeomConvert_ApproxSurface anApproximation(
    aSphere, THE_TOLERANCE, GeomAbs_C0, GeomAbs_C0, 8, 8, 100, 0);
  ASSERT_TRUE(anApproximation.IsDone());
  const occ::handle<Geom_BSplineSurface> aResult = anApproximation.Surface();
  ASSERT_FALSE(aResult.IsNull());
  EXPECT_LE(aResult->EvalD0(M_PI, 0.0).Distance(aSphere->EvalD0(M_PI, 0.0)), THE_TOLERANCE);
}
