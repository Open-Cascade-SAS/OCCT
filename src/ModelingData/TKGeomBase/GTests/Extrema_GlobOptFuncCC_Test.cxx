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

#include <Extrema_GlobOptFuncCC.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom2d_Line.hxx>
#include <Precision.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <gtest/gtest.h>

TEST(Extrema_GlobOptFuncCCTest, SquaredDistanceDerivatives3d)
{
  const occ::handle<Geom_Line> aCurve1 = new Geom_Line(gp_Pnt(), gp_Dir(1.0, 0.0, 0.0));
  const occ::handle<Geom_Line> aCurve2 =
    new Geom_Line(gp_Pnt(0.0, 0.0, 3.0), gp_Dir(0.0, 1.0, 0.0));
  GeomAdaptor_Curve       anAdaptor1(aCurve1), anAdaptor2(aCurve2);
  Extrema_GlobOptFuncCCC1 aFirstOrder(anAdaptor1, anAdaptor2);
  Extrema_GlobOptFuncCCC2 aSecondOrder(anAdaptor1, anAdaptor2);
  math_Vector             aParameters(1, 2), aGradient(1, 2);
  aParameters(1)              = 1.0;
  aParameters(2)              = -2.0;
  const double aValueExpected = 14.0;
  double       aValue;
  ASSERT_TRUE(aFirstOrder.Values(aParameters, aValue, aGradient));
  EXPECT_NEAR(aValue, aValueExpected, Precision::Confusion());
  for (size_t anIndex = 0; anIndex < 2; ++anIndex)
    EXPECT_NEAR(aGradient.At(anIndex), 2.0 * aParameters.At(anIndex), Precision::Confusion());
  math_Matrix aHessian(1, 2, 1, 2);
  ASSERT_TRUE(aSecondOrder.Values(aParameters, aValue, aGradient, aHessian));
  for (size_t aRow = 0; aRow < 2; ++aRow)
  {
    EXPECT_NEAR(aGradient.At(aRow), 2.0 * aParameters.At(aRow), Precision::Confusion());
    for (size_t aColumn = 0; aColumn < 2; ++aColumn)
    {
      const double anExpected = (aRow == aColumn ? 2.0 : 0.0);
      EXPECT_NEAR(aHessian.At(aRow, aColumn), anExpected, Precision::Confusion());
    }
  }
}

TEST(Extrema_GlobOptFuncCCTest, SquaredDistanceDerivatives2d)
{
  const occ::handle<Geom2d_Line> aCurve1 = new Geom2d_Line(gp_Pnt2d(), gp_Dir2d(1.0, 0.0));
  const occ::handle<Geom2d_Line> aCurve2 = new Geom2d_Line(gp_Pnt2d(), gp_Dir2d(0.0, 1.0));
  Geom2dAdaptor_Curve            anAdaptor1(aCurve1), anAdaptor2(aCurve2);
  Extrema_GlobOptFuncCCC1        aFirstOrder(anAdaptor1, anAdaptor2);
  Extrema_GlobOptFuncCCC2        aSecondOrder(anAdaptor1, anAdaptor2);
  math_Vector                    aParameters(1, 2), aGradient(1, 2);
  aParameters(1)              = 1.0;
  aParameters(2)              = -2.0;
  const double aValueExpected = 5.0;
  double       aValue;
  ASSERT_TRUE(aFirstOrder.Values(aParameters, aValue, aGradient));
  EXPECT_NEAR(aValue, aValueExpected, Precision::Confusion());
  for (size_t anIndex = 0; anIndex < 2; ++anIndex)
    EXPECT_NEAR(aGradient.At(anIndex), 2.0 * aParameters.At(anIndex), Precision::Confusion());
  math_Matrix aHessian(1, 2, 1, 2);
  ASSERT_TRUE(aSecondOrder.Values(aParameters, aValue, aGradient, aHessian));
  for (size_t aRow = 0; aRow < 2; ++aRow)
  {
    EXPECT_NEAR(aGradient.At(aRow), 2.0 * aParameters.At(aRow), Precision::Confusion());
    for (size_t aColumn = 0; aColumn < 2; ++aColumn)
    {
      const double anExpected = (aRow == aColumn ? 2.0 : 0.0);
      EXPECT_NEAR(aHessian.At(aRow, aColumn), anExpected, Precision::Confusion());
    }
  }
  aParameters.Init(0.0);
  ASSERT_TRUE(aFirstOrder.Value(aParameters, aValue));
  EXPECT_EQ(aValue, 0.0);
  ASSERT_TRUE(aFirstOrder.Gradient(aParameters, aGradient));
  EXPECT_EQ(aGradient(1), 0.0);
  EXPECT_EQ(aGradient(2), 0.0);
  ASSERT_TRUE(aSecondOrder.Values(aParameters, aValue, aGradient, aHessian));
  EXPECT_EQ(aHessian(1, 1), 2.0);
  EXPECT_EQ(aHessian(2, 2), 2.0);
}
