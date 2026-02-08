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

#include <Geom_BezierCurve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Trsf.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>

class Geom_BezierCurve_Test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Create a simple Bezier curve for testing
    NCollection_Array1<gp_Pnt> aPoles(1, 4);
    aPoles(1) = gp_Pnt(0, 0, 0);
    aPoles(2) = gp_Pnt(1, 1, 0);
    aPoles(3) = gp_Pnt(2, 1, 0);
    aPoles(4) = gp_Pnt(3, 0, 0);

    myOriginalCurve = new Geom_BezierCurve(aPoles);
  }

  occ::handle<Geom_BezierCurve> myOriginalCurve;
};

TEST_F(Geom_BezierCurve_Test, CopyConstructorBasicProperties)
{
  // Test copy constructor
  occ::handle<Geom_BezierCurve> aCopiedCurve = new Geom_BezierCurve(*myOriginalCurve);

  // Verify basic properties are identical
  EXPECT_EQ(myOriginalCurve->Degree(), aCopiedCurve->Degree());
  EXPECT_EQ(myOriginalCurve->NbPoles(), aCopiedCurve->NbPoles());
  EXPECT_EQ(myOriginalCurve->IsRational(), aCopiedCurve->IsRational());
  EXPECT_EQ(myOriginalCurve->IsClosed(), aCopiedCurve->IsClosed());
}

TEST_F(Geom_BezierCurve_Test, CopyConstructorPoles)
{
  occ::handle<Geom_BezierCurve> aCopiedCurve = new Geom_BezierCurve(*myOriginalCurve);

  // Verify all poles are identical
  for (int i = 1; i <= myOriginalCurve->NbPoles(); ++i)
  {
    gp_Pnt anOrigPole = myOriginalCurve->Pole(i);
    gp_Pnt aCopyPole  = aCopiedCurve->Pole(i);
    EXPECT_TRUE(anOrigPole.IsEqual(aCopyPole, 1e-10));
  }
}

TEST_F(Geom_BezierCurve_Test, CopyMethodUsesOptimizedConstructor)
{
  // Test that Copy() method uses the optimized copy constructor
  occ::handle<Geom_Geometry>    aCopiedGeom  = myOriginalCurve->Copy();
  occ::handle<Geom_BezierCurve> aCopiedCurve = occ::down_cast<Geom_BezierCurve>(aCopiedGeom);

  EXPECT_FALSE(aCopiedCurve.IsNull());

  // Verify the copy is functionally identical
  EXPECT_EQ(myOriginalCurve->Degree(), aCopiedCurve->Degree());
  EXPECT_EQ(myOriginalCurve->NbPoles(), aCopiedCurve->NbPoles());

  // Test evaluation at several points
  for (double u = 0.0; u <= 1.0; u += 0.25)
  {
    gp_Pnt anOrigPnt = myOriginalCurve->Value(u);
    gp_Pnt aCopyPnt  = aCopiedCurve->Value(u);
    EXPECT_TRUE(anOrigPnt.IsEqual(aCopyPnt, 1e-10));
  }
}

TEST_F(Geom_BezierCurve_Test, RationalCurveCopyConstructor)
{
  // Create a rational Bezier curve
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 1, 0);
  aPoles(3) = gp_Pnt(2, 0, 0);

  NCollection_Array1<double> aWeights(1, 3);
  aWeights(1) = 1.0;
  aWeights(2) = 2.0;
  aWeights(3) = 1.0;

  occ::handle<Geom_BezierCurve> aRationalCurve  = new Geom_BezierCurve(aPoles, aWeights);
  occ::handle<Geom_BezierCurve> aCopiedRational = new Geom_BezierCurve(*aRationalCurve);

  EXPECT_TRUE(aCopiedRational->IsRational());

  // Verify weights are copied correctly
  for (int i = 1; i <= aRationalCurve->NbPoles(); ++i)
  {
    EXPECT_DOUBLE_EQ(aRationalCurve->Weight(i), aCopiedRational->Weight(i));
  }
}

TEST_F(Geom_BezierCurve_Test, CopyIndependence)
{
  occ::handle<Geom_BezierCurve> aCopiedCurve = new Geom_BezierCurve(*myOriginalCurve);

  // Modify the original curve
  gp_Pnt aNewPole(10, 10, 10);
  myOriginalCurve->SetPole(2, aNewPole);

  // Verify the copied curve is not affected
  gp_Pnt anOrigPole = aCopiedCurve->Pole(2);
  EXPECT_FALSE(anOrigPole.IsEqual(aNewPole, 1e-10));
}

TEST_F(Geom_BezierCurve_Test, Evaluation_D0)
{
  gp_Pnt aPnt;
  myOriginalCurve->D0(0.0, aPnt);
  EXPECT_TRUE(aPnt.IsEqual(gp_Pnt(0, 0, 0), 1e-10));
  myOriginalCurve->D0(1.0, aPnt);
  EXPECT_TRUE(aPnt.IsEqual(gp_Pnt(3, 0, 0), 1e-10));
}

TEST_F(Geom_BezierCurve_Test, Evaluation_D1)
{
  gp_Pnt aPnt;
  gp_Vec aV1;
  myOriginalCurve->D1(0.5, aPnt, aV1);
  EXPECT_GT(aV1.Magnitude(), 0.0);
}

TEST_F(Geom_BezierCurve_Test, Evaluation_D2)
{
  gp_Pnt aPnt;
  gp_Vec aV1, aV2;
  myOriginalCurve->D2(0.5, aPnt, aV1, aV2);
  EXPECT_GT(aV1.Magnitude(), 0.0);
}

TEST_F(Geom_BezierCurve_Test, Evaluation_D3)
{
  gp_Pnt aPnt;
  gp_Vec aV1, aV2, aV3;
  myOriginalCurve->D3(0.5, aPnt, aV1, aV2, aV3);
  // D3 of a degree 3 curve is constant
  gp_Pnt aPntB;
  gp_Vec aV1b, aV2b, aV3b;
  myOriginalCurve->D3(0.25, aPntB, aV1b, aV2b, aV3b);
  EXPECT_NEAR(aV3.X(), aV3b.X(), 1e-8);
  EXPECT_NEAR(aV3.Y(), aV3b.Y(), 1e-8);
}

TEST_F(Geom_BezierCurve_Test, Evaluation_DN)
{
  gp_Vec aDN1 = myOriginalCurve->DN(0.5, 1);
  EXPECT_GT(aDN1.Magnitude(), 0.0);
  // DN(4) for a degree 3 curve should be zero
  gp_Vec aDN4 = myOriginalCurve->DN(0.5, 4);
  EXPECT_NEAR(aDN4.Magnitude(), 0.0, 1e-10);
}

TEST_F(Geom_BezierCurve_Test, StartEndPoints)
{
  gp_Pnt aStart = myOriginalCurve->StartPoint();
  gp_Pnt anEnd  = myOriginalCurve->EndPoint();
  EXPECT_TRUE(aStart.IsEqual(gp_Pnt(0, 0, 0), 1e-10));
  EXPECT_TRUE(anEnd.IsEqual(gp_Pnt(3, 0, 0), 1e-10));
}

TEST_F(Geom_BezierCurve_Test, Properties)
{
  EXPECT_EQ(myOriginalCurve->Degree(), 3);
  EXPECT_FALSE(myOriginalCurve->IsPeriodic());
  EXPECT_FALSE(myOriginalCurve->IsRational());
  EXPECT_FALSE(myOriginalCurve->IsClosed());
  EXPECT_TRUE(myOriginalCurve->IsCN(100));
  EXPECT_EQ(myOriginalCurve->Continuity(), GeomAbs_CN);
  EXPECT_DOUBLE_EQ(myOriginalCurve->FirstParameter(), 0.0);
  EXPECT_DOUBLE_EQ(myOriginalCurve->LastParameter(), 1.0);
}

TEST_F(Geom_BezierCurve_Test, SetPole)
{
  gp_Pnt aNewPole(1, 5, 0);
  myOriginalCurve->SetPole(2, aNewPole);
  EXPECT_TRUE(myOriginalCurve->Pole(2).IsEqual(aNewPole, 1e-10));
  // Endpoints unchanged
  EXPECT_TRUE(myOriginalCurve->StartPoint().IsEqual(gp_Pnt(0, 0, 0), 1e-10));
  EXPECT_TRUE(myOriginalCurve->EndPoint().IsEqual(gp_Pnt(3, 0, 0), 1e-10));
}

TEST_F(Geom_BezierCurve_Test, SetWeight)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 1, 0);
  aPoles(3) = gp_Pnt(2, 0, 0);

  NCollection_Array1<double> aWeights(1, 3);
  aWeights(1) = 1.0;
  aWeights(2) = 1.0;
  aWeights(3) = 1.0;

  occ::handle<Geom_BezierCurve> aCurve     = new Geom_BezierCurve(aPoles, aWeights);
  gp_Pnt                        aMidBefore = aCurve->Value(0.5);

  aCurve->SetWeight(2, 10.0);
  EXPECT_DOUBLE_EQ(aCurve->Weight(2), 10.0);
  EXPECT_TRUE(aCurve->IsRational());

  gp_Pnt aMidAfter = aCurve->Value(0.5);
  // High weight pulls curve toward the control point
  EXPECT_GT(aMidAfter.Y(), aMidBefore.Y());
}

TEST_F(Geom_BezierCurve_Test, InsertPoleAfter)
{
  int    aNbBefore = myOriginalCurve->NbPoles();
  gp_Pnt aNewPole(1.5, 2.0, 0);
  myOriginalCurve->InsertPoleAfter(2, aNewPole);
  EXPECT_EQ(myOriginalCurve->NbPoles(), aNbBefore + 1);
  EXPECT_EQ(myOriginalCurve->Degree(), 4);
  EXPECT_TRUE(myOriginalCurve->Pole(3).IsEqual(aNewPole, 1e-10));
}

TEST_F(Geom_BezierCurve_Test, InsertPoleBefore)
{
  int    aNbBefore = myOriginalCurve->NbPoles();
  gp_Pnt aNewPole(0.5, 2.0, 0);
  myOriginalCurve->InsertPoleBefore(2, aNewPole);
  EXPECT_EQ(myOriginalCurve->NbPoles(), aNbBefore + 1);
  EXPECT_TRUE(myOriginalCurve->Pole(2).IsEqual(aNewPole, 1e-10));
}

TEST_F(Geom_BezierCurve_Test, RemovePole)
{
  // Add a pole first so we have 5, then remove one
  myOriginalCurve->InsertPoleAfter(2, gp_Pnt(1.5, 2, 0));
  EXPECT_EQ(myOriginalCurve->NbPoles(), 5);

  myOriginalCurve->RemovePole(3);
  EXPECT_EQ(myOriginalCurve->NbPoles(), 4);
  EXPECT_EQ(myOriginalCurve->Degree(), 3);
}

TEST_F(Geom_BezierCurve_Test, Increase)
{
  gp_Pnt aValBefore = myOriginalCurve->Value(0.5);
  myOriginalCurve->Increase(5);
  EXPECT_EQ(myOriginalCurve->Degree(), 5);
  EXPECT_EQ(myOriginalCurve->NbPoles(), 6);
  gp_Pnt aValAfter = myOriginalCurve->Value(0.5);
  EXPECT_TRUE(aValBefore.IsEqual(aValAfter, 1e-10));
}

TEST_F(Geom_BezierCurve_Test, Segment)
{
  gp_Pnt aP25 = myOriginalCurve->Value(0.25);
  gp_Pnt aP75 = myOriginalCurve->Value(0.75);

  myOriginalCurve->Segment(0.25, 0.75);
  EXPECT_TRUE(myOriginalCurve->StartPoint().IsEqual(aP25, 1e-6));
  EXPECT_TRUE(myOriginalCurve->EndPoint().IsEqual(aP75, 1e-6));
}

TEST_F(Geom_BezierCurve_Test, Reverse)
{
  gp_Pnt aStart = myOriginalCurve->StartPoint();
  gp_Pnt anEnd  = myOriginalCurve->EndPoint();
  myOriginalCurve->Reverse();
  EXPECT_TRUE(myOriginalCurve->StartPoint().IsEqual(anEnd, 1e-10));
  EXPECT_TRUE(myOriginalCurve->EndPoint().IsEqual(aStart, 1e-10));
}

TEST_F(Geom_BezierCurve_Test, Resolution)
{
  double aUTol = 0.0;
  myOriginalCurve->Resolution(1.0, aUTol);
  EXPECT_GT(aUTol, 0.0);
}

TEST_F(Geom_BezierCurve_Test, Transform)
{
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(10, 20, 30));
  gp_Pnt aPtBefore = myOriginalCurve->Value(0.5);
  myOriginalCurve->Transform(aTrsf);
  gp_Pnt aPtAfter = myOriginalCurve->Value(0.5);
  EXPECT_NEAR(aPtAfter.X(), aPtBefore.X() + 10.0, 1e-10);
  EXPECT_NEAR(aPtAfter.Y(), aPtBefore.Y() + 20.0, 1e-10);
  EXPECT_NEAR(aPtAfter.Z(), aPtBefore.Z() + 30.0, 1e-10);
}

TEST_F(Geom_BezierCurve_Test, PolesAccess)
{
  const NCollection_Array1<gp_Pnt>& aPoles = myOriginalCurve->Poles();
  EXPECT_EQ(aPoles.Length(), 4);
  EXPECT_TRUE(aPoles(1).IsEqual(gp_Pnt(0, 0, 0), 1e-10));
}

TEST_F(Geom_BezierCurve_Test, WeightsAccess_NonRational)
{
  const NCollection_Array1<double>* aWeights = myOriginalCurve->Weights();
  EXPECT_EQ(aWeights, nullptr);
}

TEST_F(Geom_BezierCurve_Test, RationalCurveEvaluation)
{
  // Create a rational quadratic Bezier (approximation of circular arc)
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles(1) = gp_Pnt(1, 0, 0);
  aPoles(2) = gp_Pnt(1, 1, 0);
  aPoles(3) = gp_Pnt(0, 1, 0);

  NCollection_Array1<double> aWeights(1, 3);
  aWeights(1) = 1.0;
  aWeights(2) = 1.0 / std::sqrt(2.0);
  aWeights(3) = 1.0;

  occ::handle<Geom_BezierCurve> aCurve = new Geom_BezierCurve(aPoles, aWeights);
  EXPECT_TRUE(aCurve->IsRational());

  // At u=0.5, the rational curve should be close to the unit circle
  gp_Pnt aMid    = aCurve->Value(0.5);
  double aRadius = std::sqrt(aMid.X() * aMid.X() + aMid.Y() * aMid.Y());
  EXPECT_NEAR(aRadius, 1.0, 1e-6);
}

TEST_F(Geom_BezierCurve_Test, MaxDegree)
{
  EXPECT_GE(Geom_BezierCurve::MaxDegree(), 25);
}

TEST_F(Geom_BezierCurve_Test, SetPoleWithWeight)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 1, 0);
  aPoles(3) = gp_Pnt(2, 0, 0);

  NCollection_Array1<double> aWeights(1, 3);
  aWeights(1) = 1.0;
  aWeights(2) = 1.0;
  aWeights(3) = 1.0;

  occ::handle<Geom_BezierCurve> aCurve = new Geom_BezierCurve(aPoles, aWeights);
  gp_Pnt                        aNewPole(1, 2, 0);
  aCurve->SetPole(2, aNewPole, 5.0);
  EXPECT_TRUE(aCurve->Pole(2).IsEqual(aNewPole, 1e-10));
  EXPECT_DOUBLE_EQ(aCurve->Weight(2), 5.0);
  EXPECT_TRUE(aCurve->IsRational());
}

TEST_F(Geom_BezierCurve_Test, InsertPoleAfterWithWeight)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 1, 0);
  aPoles(3) = gp_Pnt(2, 0, 0);

  NCollection_Array1<double> aWeights(1, 3);
  aWeights(1) = 1.0;
  aWeights(2) = 2.0;
  aWeights(3) = 1.0;

  occ::handle<Geom_BezierCurve> aCurve = new Geom_BezierCurve(aPoles, aWeights);
  aCurve->InsertPoleAfter(2, gp_Pnt(1.5, 0.5, 0), 3.0);
  EXPECT_EQ(aCurve->NbPoles(), 4);
  EXPECT_DOUBLE_EQ(aCurve->Weight(3), 3.0);
  EXPECT_TRUE(aCurve->IsRational());
}

TEST_F(Geom_BezierCurve_Test, ClosedCurve)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 1, 0);
  aPoles(3) = gp_Pnt(2, 0, 0);
  aPoles(4) = gp_Pnt(0, 0, 0);

  occ::handle<Geom_BezierCurve> aCurve = new Geom_BezierCurve(aPoles);
  EXPECT_TRUE(aCurve->IsClosed());
}

TEST_F(Geom_BezierCurve_Test, RationalSegment)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles(1) = gp_Pnt(1, 0, 0);
  aPoles(2) = gp_Pnt(1, 1, 0);
  aPoles(3) = gp_Pnt(0, 1, 0);

  NCollection_Array1<double> aWeights(1, 3);
  aWeights(1) = 1.0;
  aWeights(2) = 1.0 / std::sqrt(2.0);
  aWeights(3) = 1.0;

  occ::handle<Geom_BezierCurve> aCurve = new Geom_BezierCurve(aPoles, aWeights);
  gp_Pnt                        aMid   = aCurve->Value(0.5);

  aCurve->Segment(0.25, 0.75);
  gp_Pnt aMidAfter = aCurve->Value(0.5);
  EXPECT_TRUE(aMid.IsEqual(aMidAfter, 1e-6));
  EXPECT_TRUE(aCurve->IsRational());
}

TEST_F(Geom_BezierCurve_Test, RationalIncrease)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 1, 0);
  aPoles(3) = gp_Pnt(2, 0, 0);

  NCollection_Array1<double> aWeights(1, 3);
  aWeights(1) = 1.0;
  aWeights(2) = 2.0;
  aWeights(3) = 1.0;

  occ::handle<Geom_BezierCurve> aCurve     = new Geom_BezierCurve(aPoles, aWeights);
  gp_Pnt                        aValBefore = aCurve->Value(0.5);

  aCurve->Increase(5);
  EXPECT_EQ(aCurve->Degree(), 5);
  EXPECT_TRUE(aCurve->IsRational());
  gp_Pnt aValAfter = aCurve->Value(0.5);
  EXPECT_TRUE(aValBefore.IsEqual(aValAfter, 1e-10));
}

TEST_F(Geom_BezierCurve_Test, RationalReverse)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 1, 0);
  aPoles(3) = gp_Pnt(2, 0, 0);

  NCollection_Array1<double> aWeights(1, 3);
  aWeights(1) = 1.0;
  aWeights(2) = 3.0;
  aWeights(3) = 2.0;

  occ::handle<Geom_BezierCurve> aCurve = new Geom_BezierCurve(aPoles, aWeights);
  gp_Pnt                        aStart = aCurve->StartPoint();
  gp_Pnt                        anEnd  = aCurve->EndPoint();

  aCurve->Reverse();
  EXPECT_TRUE(aCurve->StartPoint().IsEqual(anEnd, 1e-10));
  EXPECT_TRUE(aCurve->EndPoint().IsEqual(aStart, 1e-10));
  // Weights should be reversed
  EXPECT_DOUBLE_EQ(aCurve->Weight(1), 2.0);
  EXPECT_DOUBLE_EQ(aCurve->Weight(3), 1.0);
}

TEST_F(Geom_BezierCurve_Test, ReversedParameter)
{
  EXPECT_DOUBLE_EQ(myOriginalCurve->ReversedParameter(0.3), 0.7);
  EXPECT_DOUBLE_EQ(myOriginalCurve->ReversedParameter(0.0), 1.0);
}

TEST_F(Geom_BezierCurve_Test, LinearCurve)
{
  // Degree 1 Bezier = line segment
  NCollection_Array1<gp_Pnt> aPoles(1, 2);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(3, 4, 0);

  occ::handle<Geom_BezierCurve> aCurve = new Geom_BezierCurve(aPoles);
  EXPECT_EQ(aCurve->Degree(), 1);

  gp_Pnt aMid = aCurve->Value(0.5);
  EXPECT_TRUE(aMid.IsEqual(gp_Pnt(1.5, 2.0, 0), 1e-10));

  // D2 should be zero for a line
  gp_Pnt aPnt;
  gp_Vec aV1, aV2;
  aCurve->D2(0.5, aPnt, aV1, aV2);
  EXPECT_NEAR(aV2.Magnitude(), 0.0, 1e-10);
}