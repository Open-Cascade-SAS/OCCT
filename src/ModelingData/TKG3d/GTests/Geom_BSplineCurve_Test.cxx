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

#include <Geom_BSplineCurve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Trsf.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>

class Geom_BSplineCurve_Test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Create a simple BSpline curve for testing
    NCollection_Array1<gp_Pnt> aPoles(1, 4);
    aPoles(1) = gp_Pnt(0, 0, 0);
    aPoles(2) = gp_Pnt(1, 1, 0);
    aPoles(3) = gp_Pnt(2, 1, 0);
    aPoles(4) = gp_Pnt(3, 0, 0);

    NCollection_Array1<double> aKnots(1, 2);
    aKnots(1) = 0.0;
    aKnots(2) = 1.0;

    NCollection_Array1<int> aMults(1, 2);
    aMults(1) = 4;
    aMults(2) = 4;

    myOriginalCurve = new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);
  }

  occ::handle<Geom_BSplineCurve> myOriginalCurve;
};

TEST_F(Geom_BSplineCurve_Test, CopyConstructorBasicProperties)
{
  // Test copy constructor
  occ::handle<Geom_BSplineCurve> aCopiedCurve = new Geom_BSplineCurve(*myOriginalCurve);

  // Verify basic properties are identical
  EXPECT_EQ(myOriginalCurve->Degree(), aCopiedCurve->Degree());
  EXPECT_EQ(myOriginalCurve->NbPoles(), aCopiedCurve->NbPoles());
  EXPECT_EQ(myOriginalCurve->NbKnots(), aCopiedCurve->NbKnots());
  EXPECT_EQ(myOriginalCurve->IsPeriodic(), aCopiedCurve->IsPeriodic());
  EXPECT_EQ(myOriginalCurve->IsRational(), aCopiedCurve->IsRational());
}

TEST_F(Geom_BSplineCurve_Test, CopyConstructorPoles)
{
  occ::handle<Geom_BSplineCurve> aCopiedCurve = new Geom_BSplineCurve(*myOriginalCurve);

  // Verify all poles are identical
  for (int i = 1; i <= myOriginalCurve->NbPoles(); ++i)
  {
    gp_Pnt anOrigPole = myOriginalCurve->Pole(i);
    gp_Pnt aCopyPole  = aCopiedCurve->Pole(i);
    EXPECT_TRUE(anOrigPole.IsEqual(aCopyPole, 1e-10));
  }
}

TEST_F(Geom_BSplineCurve_Test, CopyConstructorKnots)
{
  occ::handle<Geom_BSplineCurve> aCopiedCurve = new Geom_BSplineCurve(*myOriginalCurve);

  // Verify all knots are identical
  for (int i = 1; i <= myOriginalCurve->NbKnots(); ++i)
  {
    EXPECT_DOUBLE_EQ(myOriginalCurve->Knot(i), aCopiedCurve->Knot(i));
    EXPECT_EQ(myOriginalCurve->Multiplicity(i), aCopiedCurve->Multiplicity(i));
  }
}

TEST_F(Geom_BSplineCurve_Test, CopyMethodUsesOptimizedConstructor)
{
  // Test that Copy() method uses the optimized copy constructor
  occ::handle<Geom_Geometry>     aCopiedGeom  = myOriginalCurve->Copy();
  occ::handle<Geom_BSplineCurve> aCopiedCurve = occ::down_cast<Geom_BSplineCurve>(aCopiedGeom);

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

TEST_F(Geom_BSplineCurve_Test, RationalCurveCopyConstructor)
{
  // Create a rational BSpline curve
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 1, 0);
  aPoles(3) = gp_Pnt(2, 0, 0);

  NCollection_Array1<double> aWeights(1, 3);
  aWeights(1) = 1.0;
  aWeights(2) = 2.0;
  aWeights(3) = 1.0;

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 3;
  aMults(2) = 3;

  occ::handle<Geom_BSplineCurve> aRationalCurve =
    new Geom_BSplineCurve(aPoles, aWeights, aKnots, aMults, 2);
  occ::handle<Geom_BSplineCurve> aCopiedRational = new Geom_BSplineCurve(*aRationalCurve);

  EXPECT_TRUE(aCopiedRational->IsRational());

  // Verify weights are copied correctly
  for (int i = 1; i <= aRationalCurve->NbPoles(); ++i)
  {
    EXPECT_DOUBLE_EQ(aRationalCurve->Weight(i), aCopiedRational->Weight(i));
  }
}

TEST_F(Geom_BSplineCurve_Test, Evaluation_D0)
{
  gp_Pnt aPnt;
  myOriginalCurve->D0(0.0, aPnt);
  EXPECT_TRUE(aPnt.IsEqual(gp_Pnt(0, 0, 0), 1e-10));
  myOriginalCurve->D0(1.0, aPnt);
  EXPECT_TRUE(aPnt.IsEqual(gp_Pnt(3, 0, 0), 1e-10));
}

TEST_F(Geom_BSplineCurve_Test, Evaluation_D1)
{
  gp_Pnt aPnt;
  gp_Vec aV1;
  myOriginalCurve->D1(0.5, aPnt, aV1);
  EXPECT_GT(aV1.Magnitude(), 0.0);
}

TEST_F(Geom_BSplineCurve_Test, Evaluation_D2)
{
  gp_Pnt aPnt;
  gp_Vec aV1, aV2;
  myOriginalCurve->D2(0.5, aPnt, aV1, aV2);
  EXPECT_GT(aV1.Magnitude(), 0.0);
}

TEST_F(Geom_BSplineCurve_Test, Evaluation_D3)
{
  gp_Pnt aPnt;
  gp_Vec aV1, aV2, aV3;
  myOriginalCurve->D3(0.5, aPnt, aV1, aV2, aV3);
  // D3 of a degree 3 curve is constant
  gp_Vec aV3b;
  gp_Pnt aPntB;
  gp_Vec aV1b, aV2b;
  myOriginalCurve->D3(0.25, aPntB, aV1b, aV2b, aV3b);
  EXPECT_NEAR(aV3.X(), aV3b.X(), 1e-8);
  EXPECT_NEAR(aV3.Y(), aV3b.Y(), 1e-8);
  EXPECT_NEAR(aV3.Z(), aV3b.Z(), 1e-8);
}

TEST_F(Geom_BSplineCurve_Test, Evaluation_DN)
{
  gp_Vec aDN = myOriginalCurve->DN(0.5, 1);
  EXPECT_GT(aDN.Magnitude(), 0.0);
  // DN(4) for a degree 3 curve should be zero
  gp_Vec aDN4 = myOriginalCurve->DN(0.5, 4);
  EXPECT_NEAR(aDN4.Magnitude(), 0.0, 1e-10);
}

TEST_F(Geom_BSplineCurve_Test, StartEndPoints)
{
  gp_Pnt aStart = myOriginalCurve->StartPoint();
  gp_Pnt anEnd  = myOriginalCurve->EndPoint();
  EXPECT_TRUE(aStart.IsEqual(gp_Pnt(0, 0, 0), 1e-10));
  EXPECT_TRUE(anEnd.IsEqual(gp_Pnt(3, 0, 0), 1e-10));
}

TEST_F(Geom_BSplineCurve_Test, SetPole)
{
  gp_Pnt aNewPole(1, 2, 3);
  myOriginalCurve->SetPole(2, aNewPole);
  EXPECT_TRUE(myOriginalCurve->Pole(2).IsEqual(aNewPole, 1e-10));
  // End points should not change
  EXPECT_TRUE(myOriginalCurve->StartPoint().IsEqual(gp_Pnt(0, 0, 0), 1e-10));
}

TEST_F(Geom_BSplineCurve_Test, SetWeight)
{
  // Create rational curve
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 1, 0);
  aPoles(3) = gp_Pnt(2, 0, 0);

  NCollection_Array1<double> aWeights(1, 3);
  aWeights(1) = 1.0;
  aWeights(2) = 1.0;
  aWeights(3) = 1.0;

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 3;
  aMults(2) = 3;

  occ::handle<Geom_BSplineCurve> aCurve =
    new Geom_BSplineCurve(aPoles, aWeights, aKnots, aMults, 2);
  gp_Pnt aMidBefore = aCurve->Value(0.5);

  aCurve->SetWeight(2, 5.0);
  EXPECT_DOUBLE_EQ(aCurve->Weight(2), 5.0);
  EXPECT_TRUE(aCurve->IsRational());

  gp_Pnt aMidAfter = aCurve->Value(0.5);
  EXPECT_FALSE(aMidBefore.IsEqual(aMidAfter, 1e-6));
}

TEST_F(Geom_BSplineCurve_Test, IncreaseDegree)
{
  gp_Pnt aValBefore = myOriginalCurve->Value(0.5);
  myOriginalCurve->IncreaseDegree(5);
  EXPECT_EQ(myOriginalCurve->Degree(), 5);
  gp_Pnt aValAfter = myOriginalCurve->Value(0.5);
  EXPECT_TRUE(aValBefore.IsEqual(aValAfter, 1e-10));
}

TEST_F(Geom_BSplineCurve_Test, InsertKnot)
{
  gp_Pnt aValBefore = myOriginalCurve->Value(0.5);
  myOriginalCurve->InsertKnot(0.5);
  EXPECT_EQ(myOriginalCurve->NbKnots(), 3);
  gp_Pnt aValAfter = myOriginalCurve->Value(0.5);
  EXPECT_TRUE(aValBefore.IsEqual(aValAfter, 1e-10));
}

TEST_F(Geom_BSplineCurve_Test, RemoveKnot)
{
  myOriginalCurve->InsertKnot(0.5);
  EXPECT_EQ(myOriginalCurve->NbKnots(), 3);
  gp_Pnt aValBefore = myOriginalCurve->Value(0.25);

  bool isRemoved = myOriginalCurve->RemoveKnot(2, 0, 1e-6);
  EXPECT_TRUE(isRemoved);
  EXPECT_EQ(myOriginalCurve->NbKnots(), 2);
  gp_Pnt aValAfter = myOriginalCurve->Value(0.25);
  EXPECT_TRUE(aValBefore.IsEqual(aValAfter, 1e-6));
}

TEST_F(Geom_BSplineCurve_Test, Segment)
{
  gp_Pnt aP25 = myOriginalCurve->Value(0.25);
  gp_Pnt aP75 = myOriginalCurve->Value(0.75);

  myOriginalCurve->Segment(0.25, 0.75);
  EXPECT_NEAR(myOriginalCurve->FirstParameter(), 0.25, 1e-10);
  EXPECT_NEAR(myOriginalCurve->LastParameter(), 0.75, 1e-10);
  EXPECT_TRUE(myOriginalCurve->StartPoint().IsEqual(aP25, 1e-6));
  EXPECT_TRUE(myOriginalCurve->EndPoint().IsEqual(aP75, 1e-6));
}

TEST_F(Geom_BSplineCurve_Test, Reverse)
{
  gp_Pnt aStart = myOriginalCurve->StartPoint();
  gp_Pnt anEnd  = myOriginalCurve->EndPoint();
  myOriginalCurve->Reverse();
  EXPECT_TRUE(myOriginalCurve->StartPoint().IsEqual(anEnd, 1e-10));
  EXPECT_TRUE(myOriginalCurve->EndPoint().IsEqual(aStart, 1e-10));
}

TEST_F(Geom_BSplineCurve_Test, Resolution)
{
  double aUTol = 0.0;
  myOriginalCurve->Resolution(1.0, aUTol);
  EXPECT_GT(aUTol, 0.0);
}

TEST_F(Geom_BSplineCurve_Test, Properties)
{
  EXPECT_EQ(myOriginalCurve->Degree(), 3);
  EXPECT_FALSE(myOriginalCurve->IsPeriodic());
  EXPECT_FALSE(myOriginalCurve->IsRational());
  EXPECT_FALSE(myOriginalCurve->IsClosed());
  EXPECT_TRUE(myOriginalCurve->IsCN(3));
  EXPECT_EQ(myOriginalCurve->Continuity(), GeomAbs_CN);
}

TEST_F(Geom_BSplineCurve_Test, Transform)
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

TEST_F(Geom_BSplineCurve_Test, PeriodicCurve)
{
  // Periodic BSpline: NbPoles = Sum(Mults) - Mults(last)
  // 5 poles, degree 3, 6 knots of mult 1 -> NbPoles = 1+4 = 5
  NCollection_Array1<gp_Pnt> aPoles(1, 5);
  aPoles(1) = gp_Pnt(1, 0, 0);
  aPoles(2) = gp_Pnt(0.309, 0.951, 0);
  aPoles(3) = gp_Pnt(-0.809, 0.588, 0);
  aPoles(4) = gp_Pnt(-0.809, -0.588, 0);
  aPoles(5) = gp_Pnt(0.309, -0.951, 0);

  NCollection_Array1<double> aKnots(1, 6);
  for (int i = 1; i <= 6; ++i)
    aKnots(i) = (i - 1) * 0.2;

  NCollection_Array1<int> aMults(1, 6);
  aMults.Init(1);

  occ::handle<Geom_BSplineCurve> aCurve = new Geom_BSplineCurve(aPoles, aKnots, aMults, 3, true);
  EXPECT_TRUE(aCurve->IsPeriodic());

  gp_Pnt aVal1 = aCurve->Value(0.5);
  aCurve->SetNotPeriodic();
  EXPECT_FALSE(aCurve->IsPeriodic());

  gp_Pnt aVal2 = aCurve->Value(0.5);
  EXPECT_TRUE(aVal1.IsEqual(aVal2, 1e-10));
}

TEST_F(Geom_BSplineCurve_Test, MultiKnotSpan)
{
  // BSpline with 3 knot spans, degree 2
  // NbPoles = Mf + Ml - (Deg+1) + Sum(interior) = 3+3-3+1+1 = 5
  NCollection_Array1<gp_Pnt> aPoles(1, 5);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 2, 0);
  aPoles(3) = gp_Pnt(2, 0, 0);
  aPoles(4) = gp_Pnt(3, 2, 0);
  aPoles(5) = gp_Pnt(4, 0, 0);

  NCollection_Array1<double> aKnots(1, 4);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;
  aKnots(3) = 2.0;
  aKnots(4) = 3.0;

  NCollection_Array1<int> aMults(1, 4);
  aMults(1) = 3;
  aMults(2) = 1;
  aMults(3) = 1;
  aMults(4) = 3;

  occ::handle<Geom_BSplineCurve> aCurve = new Geom_BSplineCurve(aPoles, aKnots, aMults, 2);
  EXPECT_EQ(aCurve->NbKnots(), 4);
  EXPECT_EQ(aCurve->NbPoles(), 5);

  // Evaluate at knot boundaries
  gp_Pnt aPnt;
  aCurve->D0(0.0, aPnt);
  EXPECT_TRUE(aPnt.IsEqual(gp_Pnt(0, 0, 0), 1e-10));
  aCurve->D0(3.0, aPnt);
  EXPECT_TRUE(aPnt.IsEqual(gp_Pnt(4, 0, 0), 1e-10));
}

TEST_F(Geom_BSplineCurve_Test, IncreaseMultiplicity)
{
  // Add a knot first to have an interior knot
  myOriginalCurve->InsertKnot(0.5, 1);
  EXPECT_EQ(myOriginalCurve->NbKnots(), 3);
  EXPECT_EQ(myOriginalCurve->Multiplicity(2), 1);

  gp_Pnt aValBefore = myOriginalCurve->Value(0.5);
  myOriginalCurve->IncreaseMultiplicity(2, 2);
  EXPECT_EQ(myOriginalCurve->Multiplicity(2), 2);
  gp_Pnt aValAfter = myOriginalCurve->Value(0.5);
  EXPECT_TRUE(aValBefore.IsEqual(aValAfter, 1e-10));
}

TEST_F(Geom_BSplineCurve_Test, KnotAccess)
{
  EXPECT_DOUBLE_EQ(myOriginalCurve->Knot(1), 0.0);
  EXPECT_DOUBLE_EQ(myOriginalCurve->Knot(2), 1.0);
  EXPECT_EQ(myOriginalCurve->FirstUKnotIndex(), 1);
  EXPECT_EQ(myOriginalCurve->LastUKnotIndex(), 2);

  const NCollection_Array1<double>& aKnots = myOriginalCurve->Knots();
  EXPECT_EQ(aKnots.Length(), 2);

  const NCollection_Array1<double>& aFlatKnots = myOriginalCurve->KnotSequence();
  EXPECT_EQ(aFlatKnots.Length(), 8);

  const NCollection_Array1<int>& aMults = myOriginalCurve->Multiplicities();
  EXPECT_EQ(aMults.Length(), 2);
  EXPECT_EQ(aMults(1), 4);
  EXPECT_EQ(aMults(2), 4);
}

TEST_F(Geom_BSplineCurve_Test, PolesAccess)
{
  const NCollection_Array1<gp_Pnt>& aPoles = myOriginalCurve->Poles();
  EXPECT_EQ(aPoles.Length(), 4);
  EXPECT_TRUE(aPoles(1).IsEqual(gp_Pnt(0, 0, 0), 1e-10));
}

TEST_F(Geom_BSplineCurve_Test, WeightsAccess_NonRational)
{
  const NCollection_Array1<double>* aWeights = myOriginalCurve->Weights();
  EXPECT_EQ(aWeights, nullptr);
}

TEST_F(Geom_BSplineCurve_Test, WeightsAccess_Rational)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 1, 0);
  aPoles(3) = gp_Pnt(2, 0, 0);

  NCollection_Array1<double> aWeights(1, 3);
  aWeights(1) = 1.0;
  aWeights(2) = 2.0;
  aWeights(3) = 1.0;

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 3;
  aMults(2) = 3;

  occ::handle<Geom_BSplineCurve> aCurve =
    new Geom_BSplineCurve(aPoles, aWeights, aKnots, aMults, 2);
  const NCollection_Array1<double>* aW = aCurve->Weights();
  ASSERT_NE(aW, nullptr);
  EXPECT_DOUBLE_EQ((*aW)(2), 2.0);
}

TEST_F(Geom_BSplineCurve_Test, LocalEvaluation)
{
  // Insert a knot to create 2 spans
  myOriginalCurve->InsertKnot(0.5);
  EXPECT_EQ(myOriginalCurve->NbKnots(), 3);

  gp_Pnt aGlobal = myOriginalCurve->Value(0.25);
  gp_Pnt aLocal  = myOriginalCurve->LocalValue(0.25, 1, 2);
  EXPECT_TRUE(aGlobal.IsEqual(aLocal, 1e-10));
}

TEST_F(Geom_BSplineCurve_Test, SetKnot)
{
  // Create a curve with 3 knots, degree 2
  // NbPoles = 3+3-3+1 = 4
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 1, 0);
  aPoles(3) = gp_Pnt(2, 0, 0);
  aPoles(4) = gp_Pnt(3, 1, 0);

  NCollection_Array1<double> aKnots(1, 3);
  aKnots(1) = 0.0;
  aKnots(2) = 0.5;
  aKnots(3) = 1.0;

  NCollection_Array1<int> aMults(1, 3);
  aMults(1) = 3;
  aMults(2) = 1;
  aMults(3) = 3;

  occ::handle<Geom_BSplineCurve> aCurve = new Geom_BSplineCurve(aPoles, aKnots, aMults, 2);

  aCurve->SetKnot(2, 0.6);
  EXPECT_DOUBLE_EQ(aCurve->Knot(2), 0.6);
}

TEST_F(Geom_BSplineCurve_Test, CopyIndependence)
{
  occ::handle<Geom_BSplineCurve> aCopy = new Geom_BSplineCurve(*myOriginalCurve);
  myOriginalCurve->SetPole(2, gp_Pnt(10, 10, 10));
  EXPECT_FALSE(aCopy->Pole(2).IsEqual(gp_Pnt(10, 10, 10), 1e-10));
}

TEST_F(Geom_BSplineCurve_Test, MovePoint)
{
  gp_Pnt aTarget(1.5, 0.8, 0);
  int    aFirst = 0, aLast = 0;
  myOriginalCurve->MovePoint(0.5, aTarget, 1, myOriginalCurve->NbPoles(), aFirst, aLast);
  EXPECT_GT(aFirst, 0);
  gp_Pnt aMoved = myOriginalCurve->Value(0.5);
  EXPECT_TRUE(aMoved.IsEqual(aTarget, 1e-6));
}

TEST_F(Geom_BSplineCurve_Test, LocateU)
{
  myOriginalCurve->InsertKnot(0.5);
  int anI1 = 0, anI2 = 0;
  myOriginalCurve->LocateU(0.25, 1e-10, anI1, anI2);
  EXPECT_GE(anI1, 1);
  EXPECT_LE(anI2, myOriginalCurve->NbKnots());
  EXPECT_LE(myOriginalCurve->Knot(anI1), 0.25 + 1e-10);
  EXPECT_GE(myOriginalCurve->Knot(anI2), 0.25 - 1e-10);
}

TEST_F(Geom_BSplineCurve_Test, IsEqual)
{
  occ::handle<Geom_BSplineCurve> aCopy = new Geom_BSplineCurve(*myOriginalCurve);
  EXPECT_TRUE(myOriginalCurve->IsEqual(aCopy, 1e-10));

  aCopy->SetPole(2, gp_Pnt(10, 10, 10));
  EXPECT_FALSE(myOriginalCurve->IsEqual(aCopy, 1e-10));
}

TEST_F(Geom_BSplineCurve_Test, KnotDistribution)
{
  GeomAbs_BSplKnotDistribution aDistr = myOriginalCurve->KnotDistribution();
  // Single span with end mults = degree+1 -> PiecewiseBezier
  EXPECT_EQ(aDistr, GeomAbs_PiecewiseBezier);
}

TEST_F(Geom_BSplineCurve_Test, RationalCurveSegment)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles(1) = gp_Pnt(1, 0, 0);
  aPoles(2) = gp_Pnt(1, 1, 0);
  aPoles(3) = gp_Pnt(0, 1, 0);

  NCollection_Array1<double> aWeights(1, 3);
  aWeights(1) = 1.0;
  aWeights(2) = 1.0 / std::sqrt(2.0);
  aWeights(3) = 1.0;

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 3;
  aMults(2) = 3;

  occ::handle<Geom_BSplineCurve> aCurve =
    new Geom_BSplineCurve(aPoles, aWeights, aKnots, aMults, 2);
  EXPECT_TRUE(aCurve->IsRational());

  gp_Pnt aMid = aCurve->Value(0.5);
  aCurve->Segment(0.25, 0.75);
  gp_Pnt aMidAfter = aCurve->Value(0.5);
  EXPECT_TRUE(aMid.IsEqual(aMidAfter, 1e-6));
  EXPECT_TRUE(aCurve->IsRational());
}

TEST_F(Geom_BSplineCurve_Test, RationalCurveIncreaseDegree)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 1, 0);
  aPoles(3) = gp_Pnt(2, 0, 0);

  NCollection_Array1<double> aWeights(1, 3);
  aWeights(1) = 1.0;
  aWeights(2) = 2.0;
  aWeights(3) = 1.0;

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 3;
  aMults(2) = 3;

  occ::handle<Geom_BSplineCurve> aCurve =
    new Geom_BSplineCurve(aPoles, aWeights, aKnots, aMults, 2);
  gp_Pnt aValBefore = aCurve->Value(0.5);

  aCurve->IncreaseDegree(4);
  EXPECT_EQ(aCurve->Degree(), 4);
  EXPECT_TRUE(aCurve->IsRational());
  gp_Pnt aValAfter = aCurve->Value(0.5);
  EXPECT_TRUE(aValBefore.IsEqual(aValAfter, 1e-10));
}

TEST_F(Geom_BSplineCurve_Test, PeriodicCurve_SetOrigin)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 5);
  aPoles(1) = gp_Pnt(1, 0, 0);
  aPoles(2) = gp_Pnt(0.309, 0.951, 0);
  aPoles(3) = gp_Pnt(-0.809, 0.588, 0);
  aPoles(4) = gp_Pnt(-0.809, -0.588, 0);
  aPoles(5) = gp_Pnt(0.309, -0.951, 0);

  NCollection_Array1<double> aKnots(1, 6);
  for (int i = 1; i <= 6; ++i)
    aKnots(i) = (i - 1) * 0.2;

  NCollection_Array1<int> aMults(1, 6);
  aMults.Init(1);

  occ::handle<Geom_BSplineCurve> aCurve = new Geom_BSplineCurve(aPoles, aKnots, aMults, 3, true);
  EXPECT_TRUE(aCurve->IsPeriodic());

  aCurve->SetOrigin(3);
  EXPECT_TRUE(aCurve->IsPeriodic());

  // Curve shape unchanged, just reparameterized -- can evaluate without crash
  gp_Pnt aVal2 = aCurve->Value(aCurve->FirstParameter());
  EXPECT_GT(aVal2.X() * aVal2.X() + aVal2.Y() * aVal2.Y(), 0.0);
}

TEST_F(Geom_BSplineCurve_Test, InsertKnots_Multiple)
{
  NCollection_Array1<double> aNewKnots(1, 2);
  aNewKnots(1) = 0.25;
  aNewKnots(2) = 0.75;

  NCollection_Array1<int> aNewMults(1, 2);
  aNewMults(1) = 1;
  aNewMults(2) = 1;

  gp_Pnt aValBefore = myOriginalCurve->Value(0.5);
  myOriginalCurve->InsertKnots(aNewKnots, aNewMults);
  EXPECT_EQ(myOriginalCurve->NbKnots(), 4);
  gp_Pnt aValAfter = myOriginalCurve->Value(0.5);
  EXPECT_TRUE(aValBefore.IsEqual(aValAfter, 1e-10));
}

TEST_F(Geom_BSplineCurve_Test, ClosedCurve)
{
  // Create a closed BSpline (first pole == last pole)
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 1, 0);
  aPoles(3) = gp_Pnt(2, 0, 0);
  aPoles(4) = gp_Pnt(0, 0, 0);

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 4;
  aMults(2) = 4;

  occ::handle<Geom_BSplineCurve> aCurve = new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);
  EXPECT_TRUE(aCurve->IsClosed());
}

TEST_F(Geom_BSplineCurve_Test, LocalD1)
{
  myOriginalCurve->InsertKnot(0.5);
  gp_Pnt aPnt, aPntL;
  gp_Vec aV1, aV1L;
  myOriginalCurve->D1(0.25, aPnt, aV1);
  myOriginalCurve->LocalD1(0.25, 1, 2, aPntL, aV1L);
  EXPECT_TRUE(aPnt.IsEqual(aPntL, 1e-10));
  EXPECT_NEAR(aV1.X(), aV1L.X(), 1e-10);
  EXPECT_NEAR(aV1.Y(), aV1L.Y(), 1e-10);
}

TEST_F(Geom_BSplineCurve_Test, WeightsArray_NonRational_ReturnsUnitWeights)
{
  ASSERT_FALSE(myOriginalCurve->IsRational());

  const NCollection_Array1<double> aWeights = myOriginalCurve->WeightsArray();
  EXPECT_EQ(aWeights.Length(), myOriginalCurve->NbPoles());
  EXPECT_FALSE(aWeights.IsDeletable());
  for (int i = 1; i <= aWeights.Length(); ++i)
  {
    EXPECT_DOUBLE_EQ(aWeights(i), 1.0);
  }
}

TEST_F(Geom_BSplineCurve_Test, WeightsArray_Rational_ReturnsCopy)
{
  // Create a rational BSpline
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 1, 0);
  aPoles(3) = gp_Pnt(2, 1, 0);
  aPoles(4) = gp_Pnt(3, 0, 0);

  NCollection_Array1<double> aWeightsIn(1, 4);
  aWeightsIn(1) = 1.0;
  aWeightsIn(2) = 2.0;
  aWeightsIn(3) = 3.0;
  aWeightsIn(4) = 1.0;

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 4;
  aMults(2) = 4;

  occ::handle<Geom_BSplineCurve> aRational =
    new Geom_BSplineCurve(aPoles, aWeightsIn, aKnots, aMults, 3);
  ASSERT_TRUE(aRational->IsRational());

  const NCollection_Array1<double> aWeights = aRational->WeightsArray();
  EXPECT_EQ(aWeights.Length(), 4);
  EXPECT_FALSE(aWeights.IsDeletable());
  EXPECT_DOUBLE_EQ(aWeights(1), 1.0);
  EXPECT_DOUBLE_EQ(aWeights(2), 2.0);
  EXPECT_DOUBLE_EQ(aWeights(3), 3.0);
  EXPECT_DOUBLE_EQ(aWeights(4), 1.0);
}

TEST_F(Geom_BSplineCurve_Test, CopyWeightsArray_NonRational_ReturnsOwning)
{
  ASSERT_FALSE(myOriginalCurve->IsRational());

  NCollection_Array1<double> aWeights = myOriginalCurve->CopyWeightsArray();
  EXPECT_EQ(aWeights.Length(), myOriginalCurve->NbPoles());
  EXPECT_TRUE(aWeights.IsDeletable());
  for (int i = 1; i <= aWeights.Length(); ++i)
  {
    EXPECT_DOUBLE_EQ(aWeights(i), 1.0);
  }
  // Verify it is safe to modify
  aWeights(1) = 2.0;
  EXPECT_DOUBLE_EQ(aWeights(1), 2.0);
}