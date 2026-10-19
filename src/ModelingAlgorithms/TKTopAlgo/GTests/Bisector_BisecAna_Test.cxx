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

#include <Bisector_BisecAna.hxx>
#include <Geom2d_CartesianPoint.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Parabola.hxx>
#include <Precision.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>

#include <gtest/gtest.h>

#include <cmath>

//==================================================================================================

TEST(Bisector_BisecAnaTest, PeriodicBisectorUsesPrincipalParameters)
{
  const occ::handle<Geom2d_Circle>         aCircle = new Geom2d_Circle(gp_Ax2d(), 5.0);
  const occ::handle<Geom2d_CartesianPoint> aFocus  = new Geom2d_CartesianPoint(1.0, 0.0);
  const gp_Pnt2d aPoint(0.5 + 2.5 * std::cos(0.7), std::sqrt(6.0) * std::sin(0.7));
  for (const double aSide : {-1.0, 1.0})
  {
    SCOPED_TRACE(aSide);
    Bisector_BisecAna aBisector;
    aBisector.Perform(aCircle,
                      aFocus,
                      aPoint,
                      gp_Vec2d(0.0, 1.0),
                      gp_Vec2d(1.0, 0.0),
                      aSide,
                      Precision::Confusion(),
                      false);
    ASSERT_FALSE(aBisector.Geom2dCurve().IsNull());
    ASSERT_TRUE(aBisector.Geom2dCurve()->IsKind(STANDARD_TYPE(Geom2d_Ellipse)));
    EXPECT_GE(aBisector.FirstParameter(), 0.0);
    EXPECT_LT(aBisector.FirstParameter(), 2.0 * M_PI);
    EXPECT_NEAR(aBisector.LastParameter() - aBisector.FirstParameter(),
                2.0 * M_PI,
                Precision::PConfusion());
    const occ::handle<Bisector_BisecAna> aCopy =
      occ::down_cast<Bisector_BisecAna>(aBisector.Copy());
    ASSERT_FALSE(aCopy.IsNull());
    EXPECT_DOUBLE_EQ(aCopy->FirstParameter(), aBisector.FirstParameter());
    EXPECT_DOUBLE_EQ(aCopy->LastParameter(), aBisector.LastParameter());
    const gp_Pnt2d aSample = aBisector.EvalD0(aBisector.FirstParameter() + 1.0);
    EXPECT_NEAR(std::abs(aSample.Distance(gp_Pnt2d(0.0, 0.0)) - aCircle->Radius()),
                aSample.Distance(aFocus->Pnt2d()),
                Precision::Confusion());
  }
}

TEST(Bisector_BisecAnaTest, NonzeroFocalLengthRemainsParabolic)
{
  for (const double anOrigin : {0.0, 100.0})
  {
    const occ::handle<Geom2d_Line> aLine =
      new Geom2d_Line(gp_Pnt2d(anOrigin, -anOrigin), gp_Dir2d(1.0, 0.0));
    for (const double aDistance :
         {0.0, Precision::PConfusion(), Precision::Confusion(), 2.0 * Precision::Confusion()})
    {
      for (const double aSign : {-1.0, 1.0})
      {
        SCOPED_TRACE(::testing::Message() << anOrigin << "," << aDistance << "," << aSign);
        const occ::handle<Geom2d_CartesianPoint> aPoint =
          new Geom2d_CartesianPoint(anOrigin, -anOrigin + aSign * aDistance);
        const double      aFocal = 0.5 * aLine->Lin2d().Distance(aPoint->Pnt2d());
        Bisector_BisecAna aBisector;
        aBisector.Perform(aLine,
                          aPoint,
                          gp_Pnt2d(anOrigin, -anOrigin + aSign * aFocal),
                          gp_Vec2d(1.0, 0.0),
                          gp_Vec2d(0.0, 1.0),
                          1.0,
                          Precision::Confusion(),
                          false);
        const occ::handle<Geom2d_Curve> aCurve = aBisector.Geom2dCurve();
        ASSERT_FALSE(aCurve.IsNull());
        if (aDistance == 0.0)
        {
          EXPECT_TRUE(aCurve->IsKind(STANDARD_TYPE(Geom2d_Line)));
          continue;
        }
        const occ::handle<Geom2d_Parabola> aParabola = occ::down_cast<Geom2d_Parabola>(aCurve);
        ASSERT_FALSE(aParabola.IsNull());
        EXPECT_DOUBLE_EQ(aParabola->Focal(), aFocal);
        const double   aParameter = -std::sqrt(4.0 * aFocal);
        const gp_Pnt2d aSample    = aParabola->EvalD0(aParameter);
        EXPECT_NEAR(aLine->Lin2d().Distance(aSample),
                    aPoint->Pnt2d().Distance(aSample),
                    Precision::Confusion());
        EXPECT_NEAR(std::abs(aSample.X() - anOrigin), -aParameter, Precision::PConfusion());
      }
    }
  }
}
