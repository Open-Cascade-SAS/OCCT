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

#include <Geom_OffsetCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_Ellipse.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomGridEval_Curve.hxx>
#include <NCollection_Array1.hxx>
#include <Geom_Circle.hxx>
#include <gp_Circ.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>

class Geom_OffsetCurve_Test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Create a circle as basis curve
    gp_Circ                  aCircle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(gp_Dir::D::Z)), 5.0);
    occ::handle<Geom_Circle> aBasisCurve = new Geom_Circle(aCircle);

    // Create offset curve
    gp_Dir anOffsetDir(gp_Dir::D::Z);
    double anOffsetValue = 2.0;

    myOriginalCurve = new Geom_OffsetCurve(aBasisCurve, anOffsetValue, anOffsetDir);
  }

  occ::handle<Geom_OffsetCurve> myOriginalCurve;
};

TEST_F(Geom_OffsetCurve_Test, CopyConstructorBasicProperties)
{
  // Test copy constructor
  occ::handle<Geom_OffsetCurve> aCopiedCurve = new Geom_OffsetCurve(*myOriginalCurve);

  // Verify basic properties are identical
  EXPECT_DOUBLE_EQ(myOriginalCurve->Offset(), aCopiedCurve->Offset());
  EXPECT_TRUE(myOriginalCurve->Direction().IsEqual(aCopiedCurve->Direction(), 1e-10));
  EXPECT_EQ(myOriginalCurve->IsPeriodic(), aCopiedCurve->IsPeriodic());
  EXPECT_EQ(myOriginalCurve->IsClosed(), aCopiedCurve->IsClosed());
}

TEST_F(Geom_OffsetCurve_Test, CopyConstructorBasisCurve)
{
  occ::handle<Geom_OffsetCurve> aCopiedCurve = new Geom_OffsetCurve(*myOriginalCurve);

  // Verify basis curves are equivalent but independent
  occ::handle<Geom_Curve> anOrigBasis = myOriginalCurve->BasisCurve();
  occ::handle<Geom_Curve> aCopyBasis  = aCopiedCurve->BasisCurve();

  // They should be different objects
  EXPECT_NE(anOrigBasis.get(), aCopyBasis.get());

  // But functionally equivalent
  EXPECT_DOUBLE_EQ(anOrigBasis->FirstParameter(), aCopyBasis->FirstParameter());
  EXPECT_DOUBLE_EQ(anOrigBasis->LastParameter(), aCopyBasis->LastParameter());
}

TEST_F(Geom_OffsetCurve_Test, CopyMethodUsesOptimizedConstructor)
{
  // Test that Copy() method uses the optimized copy constructor
  occ::handle<Geom_Geometry>    aCopiedGeom  = myOriginalCurve->Copy();
  occ::handle<Geom_OffsetCurve> aCopiedCurve = occ::down_cast<Geom_OffsetCurve>(aCopiedGeom);

  EXPECT_FALSE(aCopiedCurve.IsNull());

  // Verify the copy is functionally identical
  EXPECT_DOUBLE_EQ(myOriginalCurve->Offset(), aCopiedCurve->Offset());

  // Test evaluation at several points
  double anUFirst = myOriginalCurve->FirstParameter();
  double anULast  = myOriginalCurve->LastParameter();
  double aStep    = (anULast - anUFirst) / 4.0;

  for (double u = anUFirst; u <= anULast; u += aStep)
  {
    gp_Pnt anOrigPnt = myOriginalCurve->Value(u);
    gp_Pnt aCopyPnt  = aCopiedCurve->Value(u);
    EXPECT_TRUE(anOrigPnt.IsEqual(aCopyPnt, 1e-10));
  }
}

TEST_F(Geom_OffsetCurve_Test, CopyIndependence)
{
  occ::handle<Geom_OffsetCurve> aCopiedCurve = new Geom_OffsetCurve(*myOriginalCurve);

  // Store original offset value
  double anOrigOffset = aCopiedCurve->Offset();

  // Modify the original curve
  myOriginalCurve->SetOffsetValue(10.0);

  // Verify the copied curve is not affected
  EXPECT_DOUBLE_EQ(aCopiedCurve->Offset(), anOrigOffset);
  EXPECT_NE(aCopiedCurve->Offset(), myOriginalCurve->Offset());
}

//==================================================================================================

TEST_F(Geom_OffsetCurve_Test, PeriodicTrimPreservesNativeParameters)
{
  const occ::handle<Geom_Circle> aCircle = new Geom_Circle(gp_Ax2(), 5);
  const double                   aPeriod = aCircle->Period();
  for (const double aShift : {-2 * aPeriod, 0., aPeriod, 3 * aPeriod})
  {
    const double                  aFirst = aShift + .25, aLast = aShift + .75;
    const occ::handle<Geom_Curve> aTrim =
      new Geom_TrimmedCurve(aCircle, aFirst, aLast, true, false);
    const Geom_OffsetCurve anOffset(aTrim, 1, gp_Dir(0, 0, 1));
    EXPECT_EQ(anOffset.FirstParameter(), aFirst);
    EXPECT_EQ(anOffset.LastParameter(), aLast);
    EXPECT_EQ(anOffset.BasisCurve()->FirstParameter(), aFirst);
    EXPECT_EQ(anOffset.BasisCurve()->LastParameter(), aLast);
    for (const double aParameter : {aFirst, (aFirst + aLast) / 2, aLast})
    {
      const gp_Pnt aPoint = anOffset.EvalD0(aParameter);
      EXPECT_NEAR(aPoint.X(), 6 * std::cos(aParameter), 1.e-12);
      EXPECT_NEAR(aPoint.Y(), 6 * std::sin(aParameter), 1.e-12);
    }
    const Geom_TrimmedCurve aSecondTrim(new Geom_OffsetCurve(anOffset), aFirst, aLast, true, false);
    EXPECT_EQ(aSecondTrim.FirstParameter(), aFirst);
    EXPECT_EQ(aSecondTrim.LastParameter(), aLast);
  }
}

//==================================================================================================

TEST_F(Geom_OffsetCurve_Test, EllipseThirdOrderJetUsesNormalizedNormalDerivatives)
{
  const occ::handle<Geom_Curve>       aBasis = new Geom_Ellipse(gp_Ax2(), 2, 1);
  const occ::handle<Geom_OffsetCurve> aCurve = new Geom_OffsetCurve(aBasis, 0.25, gp_Dir(0, 0, 1));
  const GeomAdaptor_Curve             anAdaptor(aCurve);
  const GeomGridEval_Curve            anEvaluator(anAdaptor);
  NCollection_Array1<double>          aParameters(0, 0);
  aParameters.ChangeAt(0)                                 = 0;
  const NCollection_Array1<GeomGridEval::CurveD3> aGrid   = anEvaluator.EvaluateGridD3(aParameters);
  const Geom_Curve::ResD3                         aNative = aCurve->EvalD3(0);
  const Geom_Curve::ResD3                         anAdapted = anAdaptor.EvalD3(0);
  for (const auto& aResult : {aNative, anAdapted})
  {
    EXPECT_DOUBLE_EQ(aResult.Point.X(), 2.25);
    EXPECT_DOUBLE_EQ(aResult.D1.Y(), 1.5);
    EXPECT_DOUBLE_EQ(aResult.D2.X(), -3);
    EXPECT_DOUBLE_EQ(aResult.D3.Y(), -6);
  }
  ASSERT_EQ(aGrid.Size(), 1u);
  EXPECT_DOUBLE_EQ(aGrid.At(0).D2.X(), -3);
  EXPECT_DOUBLE_EQ(aGrid.At(0).D3.Y(), -6);
  EXPECT_DOUBLE_EQ(aCurve->EvalD2(0).D2.X(), -3);
}

//==================================================================================================

TEST_F(Geom_OffsetCurve_Test, ScaledParabolaHasFiniteThirdNormalDerivative)
{
  for (const double aScale : {1.0, 1.e-45, 1.e-53, 1.e53})
  {
    SCOPED_TRACE(aScale);
    NCollection_Array1<gp_Pnt> aPoles(size_t{3});
    aPoles.ChangeAt(0)                         = gp_Pnt(0, 0, 0);
    aPoles.ChangeAt(1)                         = gp_Pnt(.5 * aScale, 0, 0);
    aPoles.ChangeAt(2)                         = gp_Pnt(aScale, aScale, 0);
    const occ::handle<Geom_BezierCurve> aBasis = new Geom_BezierCurve(aPoles);
    const Geom_OffsetCurve              anOffset(aBasis, .25, gp_Dir(0, 0, 1));
    Geom_Curve::ResD3                   aAtZero;
    ASSERT_NO_THROW(aAtZero = anOffset.EvalD3(0));
    EXPECT_NEAR(aAtZero.D3.X(), -6.0, 1.e-12);
    EXPECT_NEAR(aAtZero.D3.Y(), 0.0, 1.e-12);
    const Geom_Curve::ResD3 aAtOne       = anOffset.EvalD3(1);
    const double            aDenominator = 25 * std::sqrt(5.0);
    EXPECT_NEAR(aAtOne.D3.X(), 18 / aDenominator, 1.e-12);
    EXPECT_NEAR(aAtOne.D3.Y(), 12 / aDenominator, 1.e-12);
    const Geom_Curve::ResD2 aD2 = anOffset.EvalD2(0);
    EXPECT_NEAR(aAtZero.D2.Y(), aD2.D2.Y(), 1.e-12 * std::max(1.0, aScale));
  }
}