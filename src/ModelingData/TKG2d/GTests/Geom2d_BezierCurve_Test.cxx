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

#include <gtest/gtest.h>

#include <cmath>

#include <Geom2d_BezierCurve.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_Trsf2d.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>

class Geom2d_BezierCurve_Test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Create a simple Bezier curve for testing
    NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
    aPoles(1) = gp_Pnt2d(0, 0);
    aPoles(2) = gp_Pnt2d(1, 1);
    aPoles(3) = gp_Pnt2d(2, 1);
    aPoles(4) = gp_Pnt2d(3, 0);

    myOriginalCurve = new Geom2d_BezierCurve(aPoles);
  }

  occ::handle<Geom2d_BezierCurve> myOriginalCurve;
};

TEST_F(Geom2d_BezierCurve_Test, CopyConstructorBasicProperties)
{
  // Test copy constructor
  occ::handle<Geom2d_BezierCurve> aCopiedCurve = new Geom2d_BezierCurve(*myOriginalCurve);

  // Verify basic properties are identical
  EXPECT_EQ(myOriginalCurve->Degree(), aCopiedCurve->Degree());
  EXPECT_EQ(myOriginalCurve->NbPoles(), aCopiedCurve->NbPoles());
  EXPECT_EQ(myOriginalCurve->IsRational(), aCopiedCurve->IsRational());
  EXPECT_EQ(myOriginalCurve->IsClosed(), aCopiedCurve->IsClosed());
}

TEST_F(Geom2d_BezierCurve_Test, CopyConstructorPoles)
{
  occ::handle<Geom2d_BezierCurve> aCopiedCurve = new Geom2d_BezierCurve(*myOriginalCurve);

  // Verify all poles are identical
  for (int i = 1; i <= myOriginalCurve->NbPoles(); ++i)
  {
    gp_Pnt2d anOrigPole = myOriginalCurve->Pole(i);
    gp_Pnt2d aCopyPole  = aCopiedCurve->Pole(i);
    EXPECT_TRUE(anOrigPole.IsEqual(aCopyPole, 1e-10));
  }
}

TEST_F(Geom2d_BezierCurve_Test, CopyMethodUsesOptimizedConstructor)
{
  // Test that Copy() method uses the optimized copy constructor
  occ::handle<Geom2d_Geometry>    aCopiedGeom  = myOriginalCurve->Copy();
  occ::handle<Geom2d_BezierCurve> aCopiedCurve = occ::down_cast<Geom2d_BezierCurve>(aCopiedGeom);

  EXPECT_FALSE(aCopiedCurve.IsNull());

  // Verify the copy is functionally identical
  EXPECT_EQ(myOriginalCurve->Degree(), aCopiedCurve->Degree());
  EXPECT_EQ(myOriginalCurve->NbPoles(), aCopiedCurve->NbPoles());

  // Test evaluation at several points
  for (double u = 0.0; u <= 1.0; u += 0.25)
  {
    gp_Pnt2d anOrigPnt = myOriginalCurve->Value(u);
    gp_Pnt2d aCopyPnt  = aCopiedCurve->Value(u);
    EXPECT_TRUE(anOrigPnt.IsEqual(aCopyPnt, 1e-10));
  }
}

TEST_F(Geom2d_BezierCurve_Test, RationalCurveCopyConstructor)
{
  // Create a rational Bezier curve
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles(1) = gp_Pnt2d(0, 0);
  aPoles(2) = gp_Pnt2d(1, 1);
  aPoles(3) = gp_Pnt2d(2, 0);

  NCollection_Array1<double> aWeights(1, 3);
  aWeights(1) = 1.0;
  aWeights(2) = 2.0;
  aWeights(3) = 1.0;

  occ::handle<Geom2d_BezierCurve> aRationalCurve  = new Geom2d_BezierCurve(aPoles, aWeights);
  occ::handle<Geom2d_BezierCurve> aCopiedRational = new Geom2d_BezierCurve(*aRationalCurve);

  EXPECT_TRUE(aCopiedRational->IsRational());

  // Verify weights are copied correctly
  for (int i = 1; i <= aRationalCurve->NbPoles(); ++i)
  {
    EXPECT_DOUBLE_EQ(aRationalCurve->Weight(i), aCopiedRational->Weight(i));
  }
}

TEST_F(Geom2d_BezierCurve_Test, CopyIndependence)
{
  occ::handle<Geom2d_BezierCurve> aCopiedCurve = new Geom2d_BezierCurve(*myOriginalCurve);

  // Modify the original curve
  gp_Pnt2d aNewPole(10, 10);
  myOriginalCurve->SetPole(2, aNewPole);

  // Verify the copied curve is not affected
  gp_Pnt2d anOrigPole = aCopiedCurve->Pole(2);
  EXPECT_FALSE(anOrigPole.IsEqual(aNewPole, 1e-10));
}

TEST_F(Geom2d_BezierCurve_Test, Evaluation_D0)
{
  gp_Pnt2d aPnt;
  myOriginalCurve->D0(0.0, aPnt);
  EXPECT_TRUE(aPnt.IsEqual(gp_Pnt2d(0, 0), 1e-10));
  myOriginalCurve->D0(1.0, aPnt);
  EXPECT_TRUE(aPnt.IsEqual(gp_Pnt2d(3, 0), 1e-10));
}

TEST_F(Geom2d_BezierCurve_Test, Evaluation_D1)
{
  gp_Pnt2d aPnt;
  gp_Vec2d aV1;
  myOriginalCurve->D1(0.5, aPnt, aV1);
  EXPECT_GT(aV1.Magnitude(), 0.0);
}

TEST_F(Geom2d_BezierCurve_Test, Evaluation_D2)
{
  gp_Pnt2d aPnt;
  gp_Vec2d aV1, aV2;
  myOriginalCurve->D2(0.5, aPnt, aV1, aV2);
  EXPECT_GT(aV1.Magnitude(), 0.0);
}

TEST_F(Geom2d_BezierCurve_Test, Evaluation_DN)
{
  gp_Vec2d aDN1 = myOriginalCurve->DN(0.5, 1);
  EXPECT_GT(aDN1.Magnitude(), 0.0);
  gp_Vec2d aDN4 = myOriginalCurve->DN(0.5, 4);
  EXPECT_NEAR(aDN4.Magnitude(), 0.0, 1e-10);
}

TEST_F(Geom2d_BezierCurve_Test, StartEndPoints)
{
  gp_Pnt2d aStart = myOriginalCurve->StartPoint();
  gp_Pnt2d anEnd  = myOriginalCurve->EndPoint();
  EXPECT_TRUE(aStart.IsEqual(gp_Pnt2d(0, 0), 1e-10));
  EXPECT_TRUE(anEnd.IsEqual(gp_Pnt2d(3, 0), 1e-10));
}

TEST_F(Geom2d_BezierCurve_Test, Properties)
{
  EXPECT_EQ(myOriginalCurve->Degree(), 3);
  EXPECT_FALSE(myOriginalCurve->IsPeriodic());
  EXPECT_FALSE(myOriginalCurve->IsRational());
  EXPECT_FALSE(myOriginalCurve->IsClosed());
  EXPECT_TRUE(myOriginalCurve->IsCN(100));
  EXPECT_DOUBLE_EQ(myOriginalCurve->FirstParameter(), 0.0);
  EXPECT_DOUBLE_EQ(myOriginalCurve->LastParameter(), 1.0);
}

TEST_F(Geom2d_BezierCurve_Test, SetPole)
{
  gp_Pnt2d aNewPole(1, 5);
  myOriginalCurve->SetPole(2, aNewPole);
  EXPECT_TRUE(myOriginalCurve->Pole(2).IsEqual(aNewPole, 1e-10));
  EXPECT_TRUE(myOriginalCurve->StartPoint().IsEqual(gp_Pnt2d(0, 0), 1e-10));
}

TEST_F(Geom2d_BezierCurve_Test, SetWeight)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles(1) = gp_Pnt2d(0, 0);
  aPoles(2) = gp_Pnt2d(1, 1);
  aPoles(3) = gp_Pnt2d(2, 0);

  NCollection_Array1<double> aWeights(1, 3);
  aWeights(1) = 1.0;
  aWeights(2) = 1.0;
  aWeights(3) = 1.0;

  occ::handle<Geom2d_BezierCurve> aCurve     = new Geom2d_BezierCurve(aPoles, aWeights);
  gp_Pnt2d                        aMidBefore = aCurve->Value(0.5);

  aCurve->SetWeight(2, 10.0);
  EXPECT_DOUBLE_EQ(aCurve->Weight(2), 10.0);
  EXPECT_TRUE(aCurve->IsRational());

  gp_Pnt2d aMidAfter = aCurve->Value(0.5);
  EXPECT_GT(aMidAfter.Y(), aMidBefore.Y());
}

TEST_F(Geom2d_BezierCurve_Test, InsertPoleAfter)
{
  int aNbBefore = myOriginalCurve->NbPoles();
  myOriginalCurve->InsertPoleAfter(2, gp_Pnt2d(1.5, 2));
  EXPECT_EQ(myOriginalCurve->NbPoles(), aNbBefore + 1);
  EXPECT_EQ(myOriginalCurve->Degree(), 4);
}

TEST_F(Geom2d_BezierCurve_Test, RemovePole)
{
  myOriginalCurve->InsertPoleAfter(2, gp_Pnt2d(1.5, 2));
  EXPECT_EQ(myOriginalCurve->NbPoles(), 5);

  myOriginalCurve->RemovePole(3);
  EXPECT_EQ(myOriginalCurve->NbPoles(), 4);
}

TEST_F(Geom2d_BezierCurve_Test, Increase)
{
  gp_Pnt2d aValBefore = myOriginalCurve->Value(0.5);
  myOriginalCurve->Increase(5);
  EXPECT_EQ(myOriginalCurve->Degree(), 5);
  gp_Pnt2d aValAfter = myOriginalCurve->Value(0.5);
  EXPECT_TRUE(aValBefore.IsEqual(aValAfter, 1e-10));
}

TEST_F(Geom2d_BezierCurve_Test, Segment)
{
  gp_Pnt2d aP25 = myOriginalCurve->Value(0.25);
  gp_Pnt2d aP75 = myOriginalCurve->Value(0.75);

  myOriginalCurve->Segment(0.25, 0.75);
  EXPECT_TRUE(myOriginalCurve->StartPoint().IsEqual(aP25, 1e-6));
  EXPECT_TRUE(myOriginalCurve->EndPoint().IsEqual(aP75, 1e-6));
}

TEST_F(Geom2d_BezierCurve_Test, Reverse)
{
  gp_Pnt2d aStart = myOriginalCurve->StartPoint();
  gp_Pnt2d anEnd  = myOriginalCurve->EndPoint();
  myOriginalCurve->Reverse();
  EXPECT_TRUE(myOriginalCurve->StartPoint().IsEqual(anEnd, 1e-10));
  EXPECT_TRUE(myOriginalCurve->EndPoint().IsEqual(aStart, 1e-10));
}

TEST_F(Geom2d_BezierCurve_Test, Resolution)
{
  double aUTol = 0.0;
  myOriginalCurve->Resolution(1.0, aUTol);
  EXPECT_GT(aUTol, 0.0);
}

TEST_F(Geom2d_BezierCurve_Test, Transform)
{
  gp_Trsf2d aTrsf;
  aTrsf.SetTranslation(gp_Vec2d(10, 20));
  gp_Pnt2d aPtBefore = myOriginalCurve->Value(0.5);
  myOriginalCurve->Transform(aTrsf);
  gp_Pnt2d aPtAfter = myOriginalCurve->Value(0.5);
  EXPECT_NEAR(aPtAfter.X(), aPtBefore.X() + 10.0, 1e-10);
  EXPECT_NEAR(aPtAfter.Y(), aPtBefore.Y() + 20.0, 1e-10);
}

TEST_F(Geom2d_BezierCurve_Test, PolesAccess)
{
  const NCollection_Array1<gp_Pnt2d>& aPoles = myOriginalCurve->Poles();
  EXPECT_EQ(aPoles.Length(), 4);
  EXPECT_TRUE(aPoles(1).IsEqual(gp_Pnt2d(0, 0), 1e-10));
}

TEST_F(Geom2d_BezierCurve_Test, WeightsAccess_NonRational)
{
  const NCollection_Array1<double>* aWeights = myOriginalCurve->Weights();
  EXPECT_EQ(aWeights, nullptr);
}

TEST_F(Geom2d_BezierCurve_Test, MaxDegree)
{
  EXPECT_GE(Geom2d_BezierCurve::MaxDegree(), 25);
}

TEST_F(Geom2d_BezierCurve_Test, RationalCurveEvaluation)
{
  // Rational quadratic Bezier approximating circular arc
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles(1) = gp_Pnt2d(1, 0);
  aPoles(2) = gp_Pnt2d(1, 1);
  aPoles(3) = gp_Pnt2d(0, 1);

  NCollection_Array1<double> aWeights(1, 3);
  aWeights(1) = 1.0;
  aWeights(2) = 1.0 / std::sqrt(2.0);
  aWeights(3) = 1.0;

  occ::handle<Geom2d_BezierCurve> aCurve = new Geom2d_BezierCurve(aPoles, aWeights);
  EXPECT_TRUE(aCurve->IsRational());

  gp_Pnt2d aMid    = aCurve->Value(0.5);
  double   aRadius = std::sqrt(aMid.X() * aMid.X() + aMid.Y() * aMid.Y());
  EXPECT_NEAR(aRadius, 1.0, 1e-6);
}

TEST_F(Geom2d_BezierCurve_Test, SetPoleWithWeight)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles(1) = gp_Pnt2d(0, 0);
  aPoles(2) = gp_Pnt2d(1, 1);
  aPoles(3) = gp_Pnt2d(2, 0);

  NCollection_Array1<double> aWeights(1, 3);
  aWeights(1) = 1.0;
  aWeights(2) = 1.0;
  aWeights(3) = 1.0;

  occ::handle<Geom2d_BezierCurve> aCurve = new Geom2d_BezierCurve(aPoles, aWeights);
  aCurve->SetPole(2, gp_Pnt2d(1, 2), 5.0);
  EXPECT_TRUE(aCurve->Pole(2).IsEqual(gp_Pnt2d(1, 2), 1e-10));
  EXPECT_DOUBLE_EQ(aCurve->Weight(2), 5.0);
  EXPECT_TRUE(aCurve->IsRational());
}

TEST_F(Geom2d_BezierCurve_Test, InsertPoleBeforeWithWeight)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles(1) = gp_Pnt2d(0, 0);
  aPoles(2) = gp_Pnt2d(1, 1);
  aPoles(3) = gp_Pnt2d(2, 0);

  NCollection_Array1<double> aWeights(1, 3);
  aWeights(1) = 1.0;
  aWeights(2) = 2.0;
  aWeights(3) = 1.0;

  occ::handle<Geom2d_BezierCurve> aCurve = new Geom2d_BezierCurve(aPoles, aWeights);
  aCurve->InsertPoleBefore(2, gp_Pnt2d(0.5, 0.5), 3.0);
  EXPECT_EQ(aCurve->NbPoles(), 4);
  EXPECT_DOUBLE_EQ(aCurve->Weight(2), 3.0);
}

TEST_F(Geom2d_BezierCurve_Test, ClosedCurve)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
  aPoles(1) = gp_Pnt2d(0, 0);
  aPoles(2) = gp_Pnt2d(1, 1);
  aPoles(3) = gp_Pnt2d(2, 0);
  aPoles(4) = gp_Pnt2d(0, 0);

  occ::handle<Geom2d_BezierCurve> aCurve = new Geom2d_BezierCurve(aPoles);
  EXPECT_TRUE(aCurve->IsClosed());
}

TEST_F(Geom2d_BezierCurve_Test, RationalSegment)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles(1) = gp_Pnt2d(1, 0);
  aPoles(2) = gp_Pnt2d(1, 1);
  aPoles(3) = gp_Pnt2d(0, 1);

  NCollection_Array1<double> aWeights(1, 3);
  aWeights(1) = 1.0;
  aWeights(2) = 1.0 / std::sqrt(2.0);
  aWeights(3) = 1.0;

  occ::handle<Geom2d_BezierCurve> aCurve = new Geom2d_BezierCurve(aPoles, aWeights);
  gp_Pnt2d                        aMid   = aCurve->Value(0.5);

  aCurve->Segment(0.25, 0.75);
  gp_Pnt2d aMidAfter = aCurve->Value(0.5);
  EXPECT_TRUE(aMid.IsEqual(aMidAfter, 1e-6));
  EXPECT_TRUE(aCurve->IsRational());
}

TEST_F(Geom2d_BezierCurve_Test, RationalReverse)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles(1) = gp_Pnt2d(0, 0);
  aPoles(2) = gp_Pnt2d(1, 1);
  aPoles(3) = gp_Pnt2d(2, 0);

  NCollection_Array1<double> aWeights(1, 3);
  aWeights(1) = 1.0;
  aWeights(2) = 3.0;
  aWeights(3) = 2.0;

  occ::handle<Geom2d_BezierCurve> aCurve = new Geom2d_BezierCurve(aPoles, aWeights);
  gp_Pnt2d                        aStart = aCurve->StartPoint();
  gp_Pnt2d                        anEnd  = aCurve->EndPoint();

  aCurve->Reverse();
  EXPECT_TRUE(aCurve->StartPoint().IsEqual(anEnd, 1e-10));
  EXPECT_TRUE(aCurve->EndPoint().IsEqual(aStart, 1e-10));
  EXPECT_DOUBLE_EQ(aCurve->Weight(1), 2.0);
  EXPECT_DOUBLE_EQ(aCurve->Weight(3), 1.0);
}

TEST_F(Geom2d_BezierCurve_Test, Evaluation_D3)
{
  gp_Pnt2d aPnt;
  gp_Vec2d aV1, aV2, aV3;
  myOriginalCurve->D3(0.5, aPnt, aV1, aV2, aV3);
  // D3 of degree 3 is constant
  gp_Pnt2d aPntB;
  gp_Vec2d aV1b, aV2b, aV3b;
  myOriginalCurve->D3(0.25, aPntB, aV1b, aV2b, aV3b);
  EXPECT_NEAR(aV3.X(), aV3b.X(), 1e-8);
  EXPECT_NEAR(aV3.Y(), aV3b.Y(), 1e-8);
}

TEST_F(Geom2d_BezierCurve_Test, ReversedParameter)
{
  EXPECT_DOUBLE_EQ(myOriginalCurve->ReversedParameter(0.3), 0.7);
  EXPECT_DOUBLE_EQ(myOriginalCurve->ReversedParameter(0.0), 1.0);
}

TEST_F(Geom2d_BezierCurve_Test, LinearCurve)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 2);
  aPoles(1) = gp_Pnt2d(0, 0);
  aPoles(2) = gp_Pnt2d(3, 4);

  occ::handle<Geom2d_BezierCurve> aCurve = new Geom2d_BezierCurve(aPoles);
  EXPECT_EQ(aCurve->Degree(), 1);

  gp_Pnt2d aMid = aCurve->Value(0.5);
  EXPECT_TRUE(aMid.IsEqual(gp_Pnt2d(1.5, 2.0), 1e-10));
}

TEST_F(Geom2d_BezierCurve_Test, RationalIncrease)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles(1) = gp_Pnt2d(0, 0);
  aPoles(2) = gp_Pnt2d(1, 1);
  aPoles(3) = gp_Pnt2d(2, 0);

  NCollection_Array1<double> aWeights(1, 3);
  aWeights(1) = 1.0;
  aWeights(2) = 2.0;
  aWeights(3) = 1.0;

  occ::handle<Geom2d_BezierCurve> aCurve     = new Geom2d_BezierCurve(aPoles, aWeights);
  gp_Pnt2d                        aValBefore = aCurve->Value(0.5);

  aCurve->Increase(5);
  EXPECT_EQ(aCurve->Degree(), 5);
  EXPECT_TRUE(aCurve->IsRational());
  gp_Pnt2d aValAfter = aCurve->Value(0.5);
  EXPECT_TRUE(aValBefore.IsEqual(aValAfter, 1e-10));
}

TEST_F(Geom2d_BezierCurve_Test, WeightsArray_NonRational_ReturnsUnitWeights)
{
  ASSERT_FALSE(myOriginalCurve->IsRational());

  const NCollection_Array1<double>& aWeights = myOriginalCurve->WeightsArray();
  EXPECT_EQ(aWeights.Length(), myOriginalCurve->NbPoles());
  EXPECT_FALSE(aWeights.IsDeletable());
  for (int i = 1; i <= aWeights.Length(); ++i)
  {
    EXPECT_DOUBLE_EQ(aWeights(i), 1.0);
  }
  EXPECT_EQ(&aWeights, &myOriginalCurve->WeightsArray());
}

TEST_F(Geom2d_BezierCurve_Test, WeightsArray_Rational_ReturnsOwning)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles(1) = gp_Pnt2d(0, 0);
  aPoles(2) = gp_Pnt2d(1, 1);
  aPoles(3) = gp_Pnt2d(2, 0);

  NCollection_Array1<double> aWeightsIn(1, 3);
  aWeightsIn(1) = 1.0;
  aWeightsIn(2) = 2.0;
  aWeightsIn(3) = 1.0;

  occ::handle<Geom2d_BezierCurve> aRational = new Geom2d_BezierCurve(aPoles, aWeightsIn);
  ASSERT_TRUE(aRational->IsRational());

  const NCollection_Array1<double>& aWeights = aRational->WeightsArray();
  EXPECT_EQ(aWeights.Length(), 3);
  EXPECT_TRUE(aWeights.IsDeletable());
  EXPECT_DOUBLE_EQ(aWeights(1), 1.0);
  EXPECT_DOUBLE_EQ(aWeights(2), 2.0);
  EXPECT_DOUBLE_EQ(aWeights(3), 1.0);
  EXPECT_EQ(&aWeights, &aRational->WeightsArray());
}
