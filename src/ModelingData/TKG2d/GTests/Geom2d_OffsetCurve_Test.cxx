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

#include <NCollection_Array1.hxx>
#include <Adaptor2d_OffsetCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_Ellipse.hxx>

#include <algorithm>
#include <cmath>

#include <gtest/gtest.h>

#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>

class Geom2d_OffsetCurve_Test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Create a circle as basis curve
    gp_Circ2d                  aCircle(gp_Ax2d(gp_Pnt2d(0, 0), gp_Dir2d(gp_Dir2d::D::X)), 5.0);
    occ::handle<Geom2d_Circle> aBasisCurve = new Geom2d_Circle(aCircle);

    // Create offset curve
    double anOffsetValue = 2.0;

    myOriginalCurve = new Geom2d_OffsetCurve(aBasisCurve, anOffsetValue);
  }

  occ::handle<Geom2d_OffsetCurve> myOriginalCurve;
};

TEST_F(Geom2d_OffsetCurve_Test, CopyConstructorBasicProperties)
{
  // Test copy constructor
  occ::handle<Geom2d_OffsetCurve> aCopiedCurve = new Geom2d_OffsetCurve(*myOriginalCurve);

  // Verify basic properties are identical
  EXPECT_DOUBLE_EQ(myOriginalCurve->Offset(), aCopiedCurve->Offset());
  EXPECT_EQ(myOriginalCurve->IsPeriodic(), aCopiedCurve->IsPeriodic());
  EXPECT_EQ(myOriginalCurve->IsClosed(), aCopiedCurve->IsClosed());
}

TEST_F(Geom2d_OffsetCurve_Test, CopyConstructorBasisCurve)
{
  occ::handle<Geom2d_OffsetCurve> aCopiedCurve = new Geom2d_OffsetCurve(*myOriginalCurve);

  // Verify basis curves are equivalent but independent
  occ::handle<Geom2d_Curve> anOrigBasis = myOriginalCurve->BasisCurve();
  occ::handle<Geom2d_Curve> aCopyBasis  = aCopiedCurve->BasisCurve();

  // They should be different objects
  EXPECT_NE(anOrigBasis.get(), aCopyBasis.get());

  // But functionally equivalent
  EXPECT_DOUBLE_EQ(anOrigBasis->FirstParameter(), aCopyBasis->FirstParameter());
  EXPECT_DOUBLE_EQ(anOrigBasis->LastParameter(), aCopyBasis->LastParameter());
}

TEST_F(Geom2d_OffsetCurve_Test, CopyMethodUsesOptimizedConstructor)
{
  // Test that Copy() method uses the optimized copy constructor
  occ::handle<Geom2d_Geometry>    aCopiedGeom  = myOriginalCurve->Copy();
  occ::handle<Geom2d_OffsetCurve> aCopiedCurve = occ::down_cast<Geom2d_OffsetCurve>(aCopiedGeom);

  EXPECT_FALSE(aCopiedCurve.IsNull());

  // Verify the copy is functionally identical
  EXPECT_DOUBLE_EQ(myOriginalCurve->Offset(), aCopiedCurve->Offset());

  // Test evaluation at several points
  double anUFirst = myOriginalCurve->FirstParameter();
  double anULast  = myOriginalCurve->LastParameter();
  double aStep    = (anULast - anUFirst) / 4.0;

  for (double u = anUFirst; u <= anULast; u += aStep)
  {
    gp_Pnt2d anOrigPnt = myOriginalCurve->Value(u);
    gp_Pnt2d aCopyPnt  = aCopiedCurve->Value(u);
    EXPECT_TRUE(anOrigPnt.IsEqual(aCopyPnt, 1e-10));
  }
}

TEST_F(Geom2d_OffsetCurve_Test, CopyIndependence)
{
  occ::handle<Geom2d_OffsetCurve> aCopiedCurve = new Geom2d_OffsetCurve(*myOriginalCurve);

  // Store original offset value
  double anOrigOffset = aCopiedCurve->Offset();

  // Modify the original curve
  myOriginalCurve->SetOffsetValue(10.0);

  // Verify the copied curve is not affected
  EXPECT_DOUBLE_EQ(aCopiedCurve->Offset(), anOrigOffset);
  EXPECT_NE(aCopiedCurve->Offset(), myOriginalCurve->Offset());
}

//==================================================================================================

TEST(Geom2dOffsetCurveTest, EvalD3_RegularAndSmallEllipse_AgreesWithLowerOrders)
{
  for (const double aScale : {1.0, 1.e-45})
  {
    const occ::handle<Geom2d_Curve> anEllipse = new Geom2d_Ellipse(gp_Ax2d(), 2.0 * aScale, aScale);
    const Geom2d_OffsetCurve        anOffset(anEllipse, 0.25 * aScale);
    const occ::handle<Geom2dAdaptor_Curve> aBasis = new Geom2dAdaptor_Curve(anEllipse);
    const Adaptor2d_OffsetCurve            anAdaptor(aBasis, 0.25 * aScale);
    for (const double aParameter : {0.0, 0.37, 1.2})
    {
      const Geom2d_Curve::ResD2 aD2 = anOffset.EvalD2(aParameter);
      const Geom2d_Curve::ResD3 aD3 = anOffset.EvalD3(aParameter);
      const auto [anAdaptorPoint, anAdaptorD1, anAdaptorD2, anAdaptorD3] =
        anAdaptor.EvalD3(aParameter);
      EXPECT_NEAR(aD3.D2.X() / aScale, aD2.D2.X() / aScale, 1.e-12);
      EXPECT_NEAR(aD3.D2.Y() / aScale, aD2.D2.Y() / aScale, 1.e-12);
      EXPECT_NEAR(anAdaptorD2.X() / aScale, aD2.D2.X() / aScale, 1.e-12);
      EXPECT_NEAR(anAdaptorD2.Y() / aScale, aD2.D2.Y() / aScale, 1.e-12);
      EXPECT_NEAR(anAdaptorD3.X() / aScale, aD3.D3.X() / aScale, 1.e-12);
      EXPECT_NEAR(anAdaptorD3.Y() / aScale, aD3.D3.Y() / aScale, 1.e-12);
    }
    EXPECT_NEAR(anOffset.EvalD3(0.0).D2.X() / aScale, -3.0, 1.e-12);
    EXPECT_NEAR(anOffset.EvalD3(0.0).D3.Y() / aScale, -6.0, 1.e-12);
  }
}

//==================================================================================================

TEST(Geom2dOffsetCurveTest, ScaledParabolaHasFiniteThirdNormalDerivative)
{
  for (const double aScale : {1.0, 1.e-45, 1.e-53, 1.e53})
  {
    SCOPED_TRACE(aScale);
    NCollection_Array1<gp_Pnt2d> aPoles(size_t{3});
    aPoles.ChangeAt(0)                           = gp_Pnt2d(0, 0);
    aPoles.ChangeAt(1)                           = gp_Pnt2d(.5 * aScale, 0);
    aPoles.ChangeAt(2)                           = gp_Pnt2d(aScale, aScale);
    const occ::handle<Geom2d_BezierCurve> aBasis = new Geom2d_BezierCurve(aPoles);
    const Geom2d_OffsetCurve              anOffset(aBasis, .25);
    Geom2d_Curve::ResD3                   aAtZero;
    ASSERT_NO_THROW(aAtZero = anOffset.EvalD3(0));
    EXPECT_NEAR(aAtZero.D3.X(), -6.0, 1.e-12);
    EXPECT_NEAR(aAtZero.D3.Y(), 0.0, 1.e-12);
    const Geom2d_Curve::ResD3 aAtOne       = anOffset.EvalD3(1);
    const double              aDenominator = 25 * std::sqrt(5.0);
    EXPECT_NEAR(aAtOne.D3.X(), 18 / aDenominator, 1.e-12);
    EXPECT_NEAR(aAtOne.D3.Y(), 12 / aDenominator, 1.e-12);
    const Geom2d_Curve::ResD2 aD2 = anOffset.EvalD2(0);
    EXPECT_NEAR(aAtZero.D2.Y(), aD2.D2.Y(), 1.e-12 * std::max(1.0, aScale));
  }
}
