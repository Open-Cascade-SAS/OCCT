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

#include <IntPatch_TheSurfFunction.hxx>
#include <IntSurf_Quadric.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Adaptor3d_Surface.hxx>
#include <gp_Ax3.hxx>
#include <gp_Vec.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pln.hxx>
#include <gp_Sphere.hxx>
#include <gp.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <IntWalk_TheFunctionOfTheInt2S.hxx>
#include <Geom_SphericalSurface.hxx>
#include <gtest/gtest.h>

TEST(IntPatch_SurfFunctionTest, TangentAfterValueUsesCurrentImplicitGradient)
{
  const occ::handle<Adaptor3d_Surface> aSurface =
    new GeomAdaptor_Surface(new Geom_Plane(gp_Pln(gp::Origin(), gp::DZ())));
  const IntSurf_Quadric    aSphere(gp_Sphere(gp_Ax3(), 1.0));
  IntPatch_TheSurfFunction aFunction(aSurface, aSphere);
  math_Vector              aParameters(1, 2), aValue(1, 1);
  aParameters(1) = 1.0;
  aParameters(2) = 0.0;
  ASSERT_TRUE(aFunction.Value(aParameters, aValue));
  EXPECT_FALSE(aFunction.IsTangent());
}

TEST(IntPatch_SurfFunctionTest, DirectionAfterValueDoesNotUsePreviousGradient)
{
  const occ::handle<Adaptor3d_Surface> aSurface =
    new GeomAdaptor_Surface(new Geom_Plane(gp_Pln(gp::Origin(), gp::DZ())));
  const IntSurf_Quadric    aSphere(gp_Sphere(gp_Ax3(), 1.0));
  IntPatch_TheSurfFunction aFunction(aSurface, aSphere);
  math_Vector              aParameters(1, 2), aValue(1, 1);
  math_Matrix              aDerivatives(1, 1, 1, 2);
  aParameters(1) = 1.0;
  aParameters(2) = 0.0;
  ASSERT_TRUE(aFunction.Values(aParameters, aValue, aDerivatives));
  ASSERT_FALSE(aFunction.IsTangent());
  aParameters(1) = 0.0;
  aParameters(2) = 1.0;
  ASSERT_TRUE(aFunction.Value(aParameters, aValue));
  ASSERT_FALSE(aFunction.IsTangent());
  const gp_Vec aDirection = aFunction.Direction3d();
  EXPECT_NEAR(aDirection.Dot(gp_Vec(gp::Origin(), aFunction.Point())), 0.0, 1.e-14);
  EXPECT_NEAR(aDirection.Z(), 0.0, 1.e-14);
  EXPECT_GT(aDirection.SquareMagnitude(), 0.0);
}

TEST(IntWalk_SurfaceFunctionTest, TangencyAfterValueUsesCurrentDerivatives)
{
  const occ::handle<Adaptor3d_Surface> aPlane =
    new GeomAdaptor_Surface(new Geom_Plane(gp_Pln(gp::Origin(), gp::DZ())), -2.0, 2.0, -2.0, 2.0);
  const occ::handle<Adaptor3d_Surface> aSphere =
    new GeomAdaptor_Surface(new Geom_SphericalSurface(gp_Ax3(), 1.0));
  IntWalk_TheFunctionOfTheInt2S aFunction(aPlane, aSphere);
  NCollection_Array1<double>    aParameters(1, 4);
  aParameters.ChangeAt(0) = 1.0;
  aParameters.ChangeAt(1) = 0.0;
  aParameters.ChangeAt(2) = 0.0;
  aParameters.ChangeAt(3) = 0.0;
  math_Vector aUV(1, 3), aLower(1, 3), anUpper(1, 3), aTolerance(1, 3), aValue(1, 3);
  math_Matrix aDerivatives(1, 3, 1, 3);
  aFunction
    .ComputeParameters(IntImp_VIsoparametricOnCaro1, aParameters, aUV, aLower, anUpper, aTolerance);
  aUV.ChangeAt(0) = 0.0;
  aUV.ChangeAt(2) = M_PI / 2.0;
  ASSERT_TRUE(aFunction.Values(aUV, aValue, aDerivatives));
  aUV.ChangeAt(0) = 1.0;
  aUV.ChangeAt(2) = 0.0;
  ASSERT_TRUE(aFunction.Value(aUV, aValue));
  IntImp_ConstIsoparametric aChoice;
  ASSERT_FALSE(aFunction.IsTangent(aUV, aParameters, aChoice));
  EXPECT_NEAR(aFunction.Direction().Z(), 0.0, 1.e-14);
  EXPECT_NEAR(aFunction.Direction().X(), 0.0, 1.e-14);
  EXPECT_NEAR(std::abs(aFunction.Direction().Y()), 1.0, 1.e-14);
}
