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
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

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
