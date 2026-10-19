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

#include <GCPnts_AbscissaPoint.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Line.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_TransformedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <GCPnts_UniformAbscissa.hxx>
#include <gp_Trsf.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

#include <array>
#include <cmath>

TEST(GCPnts_AbscissaPointTest, LineLength)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  GeomAdaptor_Curve      anAdaptor(aLine, 0.0, 10.0);

  const double aLength = GCPnts_AbscissaPoint::Length(anAdaptor);

  EXPECT_NEAR(aLength, 10.0, Precision::Confusion());
}

TEST(GCPnts_AbscissaPointTest, CircleArcLength)
{
  occ::handle<Geom_Circle> aCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 1.0);
  GeomAdaptor_Curve anAdaptor(aCircle, 0.0, M_PI);

  const double aLength = GCPnts_AbscissaPoint::Length(anAdaptor);

  EXPECT_NEAR(aLength, M_PI, Precision::Confusion());
}

TEST(GCPnts_AbscissaPointTest, FullCircleLength)
{
  occ::handle<Geom_Circle> aCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 5.0);
  GeomAdaptor_Curve anAdaptor(aCircle, 0.0, 2.0 * M_PI);

  const double aLength = GCPnts_AbscissaPoint::Length(anAdaptor);

  EXPECT_NEAR(aLength, 2.0 * M_PI * 5.0, Precision::Confusion());
}

TEST(GCPnts_AbscissaPointTest, ParameterAtAbscissa_Line)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  GeomAdaptor_Curve      anAdaptor(aLine, 0.0, 10.0);

  GCPnts_AbscissaPoint anAbscissa(anAdaptor, 5.0, 0.0);

  ASSERT_TRUE(anAbscissa.IsDone());
  const double aParam = anAbscissa.Parameter();
  EXPECT_NEAR(aParam, 5.0, Precision::Confusion());
}

TEST(GCPnts_AbscissaPointTest, ParameterAtAbscissa_Circle)
{
  occ::handle<Geom_Circle> aCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 1.0);
  GeomAdaptor_Curve anAdaptor(aCircle, 0.0, 2.0 * M_PI);

  GCPnts_AbscissaPoint anAbscissa(anAdaptor, M_PI / 2.0, 0.0);

  ASSERT_TRUE(anAbscissa.IsDone());
  const double aParam = anAbscissa.Parameter();
  EXPECT_NEAR(aParam, M_PI / 2.0, Precision::Confusion());
}

//==================================================================================================

TEST(GCPnts_AbscissaPointTest, TransformedLine_UsesNativeParameterSpeed)
{
  for (const double aScale : std::array<double, 3>{{0.5, 2, -2}})
  {
    gp_Trsf aTrsf;
    aTrsf.SetScale(gp_Pnt(0, 0, 0), aScale);
    GeomAdaptor_TransformedCurve aCurve(new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)),
                                        3,
                                        7,
                                        aTrsf);
    const double                 aSpeed = std::abs(aScale);
    EXPECT_NEAR(GCPnts_AbscissaPoint::Length(aCurve, 3, 7), 4 * aSpeed, 1.0e-10);
    EXPECT_NEAR(GCPnts_AbscissaPoint::Length(aCurve, 7, 3, 1.0e-10), 4 * aSpeed, 1.0e-10);
    GCPnts_AbscissaPoint aPoint(aCurve, aSpeed, 3);
    ASSERT_TRUE(aPoint.IsDone());
    EXPECT_NEAR(aPoint.Parameter(), 4, 1.0e-10);
    GCPnts_AbscissaPoint aPrecisePoint(1.0e-10, aCurve, -aSpeed, 7);
    ASSERT_TRUE(aPrecisePoint.IsDone());
    EXPECT_NEAR(aPrecisePoint.Parameter(), 6, 1.0e-10);
    GCPnts_UniformAbscissa aUniform(aCurve, aSpeed, 3, 7, 1.0e-10);
    ASSERT_TRUE(aUniform.IsDone());
    ASSERT_EQ(aUniform.NbPoints(), 5);
    for (int i = 1; i <= aUniform.NbPoints(); ++i)
    {
      EXPECT_NEAR(aUniform.Parameter(i), 2 + i, 1.0e-10);
    }
  }
}

//==================================================================================================

TEST(GCPnts_AbscissaPointTest, TransformedLine_LargeNativeSpeedHasFiniteLength)
{
  gp_Trsf aTrsf;
  aTrsf.SetScale(gp_Pnt(0, 0, 0), 1.0e200);
  GeomAdaptor_TransformedCurve aCurve(new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)), 3, 7, aTrsf);
  const double                 aLength = GCPnts_AbscissaPoint::Length(aCurve, 3, 7);
  EXPECT_TRUE(std::isfinite(aLength));
  EXPECT_NEAR(aLength / 1.0e200, 4, 1.0e-12);
}

//==================================================================================================

TEST(GCPnts_AbscissaPointTest, Line2d_NativeUnitSpeedIsPreserved)
{
  Geom2dAdaptor_Curve aCurve(new Geom2d_Line(gp_Pnt2d(2, 3), gp_Dir2d(1, 2)), 3, 7);
  EXPECT_NEAR(GCPnts_AbscissaPoint::Length(aCurve, 3, 7), 4, 1.0e-10);
  GCPnts_AbscissaPoint aPoint(aCurve, 1, 3);
  ASSERT_TRUE(aPoint.IsDone());
  EXPECT_NEAR(aPoint.Parameter(), 4, 1.0e-10);
  GCPnts_UniformAbscissa aUniform(aCurve, 1.0, 3, 7, 1.0e-10);
  ASSERT_TRUE(aUniform.IsDone());
  ASSERT_EQ(aUniform.NbPoints(), 5);
  for (int i = 1; i <= aUniform.NbPoints(); ++i)
  {
    EXPECT_NEAR(aUniform.Parameter(i), 2 + i, 1.0e-10);
  }
}
