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

#include <Geom2d_BSplineCurve.hxx>
#include <BSplCLib.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_Trsf2d.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

class Geom2d_BSplineCurve_Test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Create a simple BSpline curve for testing
    NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
    aPoles(1) = gp_Pnt2d(0, 0);
    aPoles(2) = gp_Pnt2d(1, 1);
    aPoles(3) = gp_Pnt2d(2, 1);
    aPoles(4) = gp_Pnt2d(3, 0);

    NCollection_Array1<double> aKnots(1, 2);
    aKnots(1) = 0.0;
    aKnots(2) = 1.0;

    NCollection_Array1<int> aMults(1, 2);
    aMults(1) = 4;
    aMults(2) = 4;

    myOriginalCurve = new Geom2d_BSplineCurve(aPoles, aKnots, aMults, 3);
  }

  occ::handle<Geom2d_BSplineCurve> myOriginalCurve;
};

namespace
{
void evaluateWithCompressedKnots(const Geom2d_BSplineCurve& theCurve,
                                 const double               theParameter,
                                 gp_Pnt2d&                  thePoint,
                                 gp_Vec2d&                  theD1,
                                 gp_Vec2d&                  theD2,
                                 gp_Vec2d&                  theD3)
{
  int    aSpanIndex = 0;
  double aParameter = theParameter;
  BSplCLib::LocateParameter(theCurve.Degree(),
                            theCurve.Knots(),
                            &theCurve.Multiplicities(),
                            theParameter,
                            theCurve.IsPeriodic(),
                            aSpanIndex,
                            aParameter);
  if (aParameter < theCurve.Knots().Value(aSpanIndex))
  {
    --aSpanIndex;
  }
  BSplCLib::D3(aParameter,
               aSpanIndex,
               theCurve.Degree(),
               theCurve.IsPeriodic(),
               theCurve.Poles(),
               theCurve.Weights(),
               theCurve.Knots(),
               &theCurve.Multiplicities(),
               thePoint,
               theD1,
               theD2,
               theD3);
}

void expectVectorNear(const gp_Vec2d& theActual, const gp_Vec2d& theExpected)
{
  EXPECT_NEAR(theActual.X(), theExpected.X(), 1.0e-12);
  EXPECT_NEAR(theActual.Y(), theExpected.Y(), 1.0e-12);
}
} // namespace

TEST_F(Geom2d_BSplineCurve_Test, CopyConstructorBasicProperties)
{
  // Test copy constructor
  occ::handle<Geom2d_BSplineCurve> aCopiedCurve = new Geom2d_BSplineCurve(*myOriginalCurve);

  // Verify basic properties are identical
  EXPECT_EQ(myOriginalCurve->Degree(), aCopiedCurve->Degree());
  EXPECT_EQ(myOriginalCurve->NbPoles(), aCopiedCurve->NbPoles());
  EXPECT_EQ(myOriginalCurve->NbKnots(), aCopiedCurve->NbKnots());
  EXPECT_EQ(myOriginalCurve->IsPeriodic(), aCopiedCurve->IsPeriodic());
  EXPECT_EQ(myOriginalCurve->IsRational(), aCopiedCurve->IsRational());
}

TEST_F(Geom2d_BSplineCurve_Test, CopyConstructorPoles)
{
  occ::handle<Geom2d_BSplineCurve> aCopiedCurve = new Geom2d_BSplineCurve(*myOriginalCurve);

  // Verify all poles are identical
  for (int i = 1; i <= myOriginalCurve->NbPoles(); ++i)
  {
    gp_Pnt2d anOrigPole = myOriginalCurve->Pole(i);
    gp_Pnt2d aCopyPole  = aCopiedCurve->Pole(i);
    EXPECT_TRUE(anOrigPole.IsEqual(aCopyPole, 1e-10));
  }
}

TEST_F(Geom2d_BSplineCurve_Test, CopyConstructorKnots)
{
  occ::handle<Geom2d_BSplineCurve> aCopiedCurve = new Geom2d_BSplineCurve(*myOriginalCurve);

  // Verify all knots are identical
  for (int i = 1; i <= myOriginalCurve->NbKnots(); ++i)
  {
    EXPECT_DOUBLE_EQ(myOriginalCurve->Knot(i), aCopiedCurve->Knot(i));
    EXPECT_EQ(myOriginalCurve->Multiplicity(i), aCopiedCurve->Multiplicity(i));
  }
}

TEST_F(Geom2d_BSplineCurve_Test, CopyMethodUsesOptimizedConstructor)
{
  // Test that Copy() method uses the optimized copy constructor
  occ::handle<Geom2d_Geometry>     aCopiedGeom  = myOriginalCurve->Copy();
  occ::handle<Geom2d_BSplineCurve> aCopiedCurve = occ::down_cast<Geom2d_BSplineCurve>(aCopiedGeom);

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

TEST_F(Geom2d_BSplineCurve_Test, RationalCurveCopyConstructor)
{
  // Create a rational BSpline curve
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles(1) = gp_Pnt2d(0, 0);
  aPoles(2) = gp_Pnt2d(1, 1);
  aPoles(3) = gp_Pnt2d(2, 0);

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

  occ::handle<Geom2d_BSplineCurve> aRationalCurve =
    new Geom2d_BSplineCurve(aPoles, aWeights, aKnots, aMults, 2);
  occ::handle<Geom2d_BSplineCurve> aCopiedRational = new Geom2d_BSplineCurve(*aRationalCurve);

  EXPECT_TRUE(aCopiedRational->IsRational());

  // Verify weights are copied correctly
  for (int i = 1; i <= aRationalCurve->NbPoles(); ++i)
  {
    EXPECT_DOUBLE_EQ(aRationalCurve->Weight(i), aCopiedRational->Weight(i));
  }
}

TEST_F(Geom2d_BSplineCurve_Test, Evaluation_D0)
{
  gp_Pnt2d aPnt;
  myOriginalCurve->D0(0.0, aPnt);
  EXPECT_TRUE(aPnt.IsEqual(gp_Pnt2d(0, 0), 1e-10));
  myOriginalCurve->D0(1.0, aPnt);
  EXPECT_TRUE(aPnt.IsEqual(gp_Pnt2d(3, 0), 1e-10));
}

TEST_F(Geom2d_BSplineCurve_Test, Evaluation_D1)
{
  gp_Pnt2d aPnt;
  gp_Vec2d aV1;
  myOriginalCurve->D1(0.5, aPnt, aV1);
  EXPECT_GT(aV1.Magnitude(), 0.0);
}

TEST_F(Geom2d_BSplineCurve_Test, Evaluation_D2)
{
  gp_Pnt2d aPnt;
  gp_Vec2d aV1, aV2;
  myOriginalCurve->D2(0.5, aPnt, aV1, aV2);
  EXPECT_GT(aV1.Magnitude(), 0.0);
}

TEST(Geom2d_BSplineCurve_EvaluationTest, FlatKnotsMatchCompressedKnots)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 10);
  for (int anIndex = 1; anIndex <= aPoles.Upper(); ++anIndex)
  {
    const double aX = static_cast<double>(anIndex - 1);
    aPoles(anIndex) = gp_Pnt2d(aX, std::sin(aX));
  }

  NCollection_Array1<double> aWeights(1, 10);
  for (int anIndex = 1; anIndex <= aWeights.Upper(); ++anIndex)
  {
    aWeights(anIndex) = 1.0 + 0.1 * static_cast<double>(anIndex % 3);
  }

  NCollection_Array1<double> aKnots(1, 5);
  aKnots(1) = 0.0;
  aKnots(2) = 0.2;
  aKnots(3) = 0.5;
  aKnots(4) = 0.75;
  aKnots(5) = 1.0;
  NCollection_Array1<int> aMults(1, 5);
  aMults(1) = 4;
  aMults(2) = 1;
  aMults(3) = 2;
  aMults(4) = 3;
  aMults(5) = 4;
  Geom2d_BSplineCurve aCurve(aPoles, aWeights, aKnots, aMults, 3);

  const double aParameters[] = {0.0,
                                0.1,
                                std::nextafter(0.2, 0.0),
                                0.2,
                                std::nextafter(0.2, 1.0),
                                std::nextafter(0.5, 0.0),
                                0.5,
                                std::nextafter(0.75, 0.0),
                                0.75,
                                0.9,
                                1.0};
  for (const double aParameter : aParameters)
  {
    gp_Pnt2d aRefPoint;
    gp_Vec2d aRefD1, aRefD2, aRefD3;
    evaluateWithCompressedKnots(aCurve, aParameter, aRefPoint, aRefD1, aRefD2, aRefD3);

    gp_Pnt2d aPoint;
    aCurve.D0(aParameter, aPoint);
    EXPECT_TRUE(aPoint.IsEqual(aRefPoint, 1.0e-12));

    gp_Vec2d aD1;
    aCurve.D1(aParameter, aPoint, aD1);
    EXPECT_TRUE(aPoint.IsEqual(aRefPoint, 1.0e-12));
    expectVectorNear(aD1, aRefD1);

    gp_Vec2d aD2;
    aCurve.D2(aParameter, aPoint, aD1, aD2);
    EXPECT_TRUE(aPoint.IsEqual(aRefPoint, 1.0e-12));
    expectVectorNear(aD1, aRefD1);
    expectVectorNear(aD2, aRefD2);

    gp_Vec2d aD3;
    aCurve.D3(aParameter, aPoint, aD1, aD2, aD3);
    EXPECT_TRUE(aPoint.IsEqual(aRefPoint, 1.0e-12));
    expectVectorNear(aD1, aRefD1);
    expectVectorNear(aD2, aRefD2);
    expectVectorNear(aD3, aRefD3);

    expectVectorNear(aCurve.DN(aParameter, 1), aRefD1);
    expectVectorNear(aCurve.DN(aParameter, 2), aRefD2);
    expectVectorNear(aCurve.DN(aParameter, 3), aRefD3);
  }
}

TEST_F(Geom2d_BSplineCurve_Test, Evaluation_DN)
{
  gp_Vec2d aDN1 = myOriginalCurve->DN(0.5, 1);
  EXPECT_GT(aDN1.Magnitude(), 0.0);
  // DN(4) for degree 3 should be zero
  gp_Vec2d aDN4 = myOriginalCurve->DN(0.5, 4);
  EXPECT_NEAR(aDN4.Magnitude(), 0.0, 1e-10);
}

TEST_F(Geom2d_BSplineCurve_Test, StartEndPoints)
{
  gp_Pnt2d aStart = myOriginalCurve->StartPoint();
  gp_Pnt2d anEnd  = myOriginalCurve->EndPoint();
  EXPECT_TRUE(aStart.IsEqual(gp_Pnt2d(0, 0), 1e-10));
  EXPECT_TRUE(anEnd.IsEqual(gp_Pnt2d(3, 0), 1e-10));
}

TEST_F(Geom2d_BSplineCurve_Test, Properties)
{
  EXPECT_EQ(myOriginalCurve->Degree(), 3);
  EXPECT_FALSE(myOriginalCurve->IsPeriodic());
  EXPECT_FALSE(myOriginalCurve->IsRational());
  EXPECT_FALSE(myOriginalCurve->IsClosed());
  EXPECT_TRUE(myOriginalCurve->IsCN(3));
}

TEST_F(Geom2d_BSplineCurve_Test, SetPole)
{
  gp_Pnt2d aNewPole(1, 5);
  myOriginalCurve->SetPole(2, aNewPole);
  EXPECT_TRUE(myOriginalCurve->Pole(2).IsEqual(aNewPole, 1e-10));
}

TEST_F(Geom2d_BSplineCurve_Test, IncreaseDegree)
{
  gp_Pnt2d aValBefore = myOriginalCurve->Value(0.5);
  myOriginalCurve->IncreaseDegree(5);
  EXPECT_EQ(myOriginalCurve->Degree(), 5);
  gp_Pnt2d aValAfter = myOriginalCurve->Value(0.5);
  EXPECT_TRUE(aValBefore.IsEqual(aValAfter, 1e-10));
}

TEST_F(Geom2d_BSplineCurve_Test, InsertKnot)
{
  gp_Pnt2d aValBefore = myOriginalCurve->Value(0.5);
  myOriginalCurve->InsertKnot(0.5);
  EXPECT_EQ(myOriginalCurve->NbKnots(), 3);
  gp_Pnt2d aValAfter = myOriginalCurve->Value(0.5);
  EXPECT_TRUE(aValBefore.IsEqual(aValAfter, 1e-10));
}

TEST_F(Geom2d_BSplineCurve_Test, RemoveKnot)
{
  myOriginalCurve->InsertKnot(0.5);
  EXPECT_EQ(myOriginalCurve->NbKnots(), 3);

  bool isRemoved = myOriginalCurve->RemoveKnot(2, 0, 1e-6);
  EXPECT_TRUE(isRemoved);
  EXPECT_EQ(myOriginalCurve->NbKnots(), 2);
}

TEST_F(Geom2d_BSplineCurve_Test, Segment)
{
  gp_Pnt2d aP25 = myOriginalCurve->Value(0.25);
  gp_Pnt2d aP75 = myOriginalCurve->Value(0.75);

  myOriginalCurve->Segment(0.25, 0.75);
  EXPECT_NEAR(myOriginalCurve->FirstParameter(), 0.25, 1e-10);
  EXPECT_NEAR(myOriginalCurve->LastParameter(), 0.75, 1e-10);
  EXPECT_TRUE(myOriginalCurve->StartPoint().IsEqual(aP25, 1e-6));
  EXPECT_TRUE(myOriginalCurve->EndPoint().IsEqual(aP75, 1e-6));
}

TEST_F(Geom2d_BSplineCurve_Test, Reverse)
{
  gp_Pnt2d aStart = myOriginalCurve->StartPoint();
  gp_Pnt2d anEnd  = myOriginalCurve->EndPoint();
  myOriginalCurve->Reverse();
  EXPECT_TRUE(myOriginalCurve->StartPoint().IsEqual(anEnd, 1e-10));
  EXPECT_TRUE(myOriginalCurve->EndPoint().IsEqual(aStart, 1e-10));
}

TEST_F(Geom2d_BSplineCurve_Test, Resolution)
{
  double aUTol = 0.0;
  myOriginalCurve->Resolution(1.0, aUTol);
  EXPECT_GT(aUTol, 0.0);
}

TEST_F(Geom2d_BSplineCurve_Test, Transform)
{
  gp_Trsf2d aTrsf;
  aTrsf.SetTranslation(gp_Vec2d(10, 20));
  gp_Pnt2d aPtBefore = myOriginalCurve->Value(0.5);
  myOriginalCurve->Transform(aTrsf);
  gp_Pnt2d aPtAfter = myOriginalCurve->Value(0.5);
  EXPECT_NEAR(aPtAfter.X(), aPtBefore.X() + 10.0, 1e-10);
  EXPECT_NEAR(aPtAfter.Y(), aPtBefore.Y() + 20.0, 1e-10);
}

TEST_F(Geom2d_BSplineCurve_Test, PeriodicCurve)
{
  // Periodic BSpline: degree 3, 6 knots of mult 1 -> NbPoles = 1+4 = 5
  NCollection_Array1<gp_Pnt2d> aPoles(1, 5);
  aPoles(1) = gp_Pnt2d(1, 0);
  aPoles(2) = gp_Pnt2d(0.309, 0.951);
  aPoles(3) = gp_Pnt2d(-0.809, 0.588);
  aPoles(4) = gp_Pnt2d(-0.809, -0.588);
  aPoles(5) = gp_Pnt2d(0.309, -0.951);

  NCollection_Array1<double> aKnots(1, 6);
  for (int i = 1; i <= 6; ++i)
  {
    aKnots(i) = (i - 1) * 0.2;
  }

  NCollection_Array1<int> aMults(1, 6);
  aMults.Init(1);

  occ::handle<Geom2d_BSplineCurve> aCurve =
    new Geom2d_BSplineCurve(aPoles, aKnots, aMults, 3, true);
  EXPECT_TRUE(aCurve->IsPeriodic());

  for (const double aParameter : {-0.1, 0.0, 0.2, 0.5, 1.0, 1.1})
  {
    gp_Pnt2d aRefPoint, aPoint;
    gp_Vec2d aRefD1, aRefD2, aRefD3, aD1, aD2, aD3;
    evaluateWithCompressedKnots(*aCurve, aParameter, aRefPoint, aRefD1, aRefD2, aRefD3);
    aCurve->D3(aParameter, aPoint, aD1, aD2, aD3);
    EXPECT_TRUE(aPoint.IsEqual(aRefPoint, 1.0e-12));
    expectVectorNear(aD1, aRefD1);
    expectVectorNear(aD2, aRefD2);
    expectVectorNear(aD3, aRefD3);
  }

  gp_Pnt2d aVal1 = aCurve->Value(0.5);
  aCurve->SetNotPeriodic();
  EXPECT_FALSE(aCurve->IsPeriodic());

  gp_Pnt2d aVal2 = aCurve->Value(0.5);
  EXPECT_TRUE(aVal1.IsEqual(aVal2, 1e-10));
}

TEST_F(Geom2d_BSplineCurve_Test, WeightsAccess_NonRational)
{
  const NCollection_Array1<double>* aWeights = myOriginalCurve->Weights();
  EXPECT_EQ(aWeights, nullptr);
}

TEST_F(Geom2d_BSplineCurve_Test, KnotAccess)
{
  EXPECT_DOUBLE_EQ(myOriginalCurve->Knot(1), 0.0);
  EXPECT_DOUBLE_EQ(myOriginalCurve->Knot(2), 1.0);

  const NCollection_Array1<double>& aKnots = myOriginalCurve->Knots();
  EXPECT_EQ(aKnots.Length(), 2);

  const NCollection_Array1<int>& aMults = myOriginalCurve->Multiplicities();
  EXPECT_EQ(aMults.Length(), 2);
  EXPECT_EQ(aMults(1), 4);
}

TEST_F(Geom2d_BSplineCurve_Test, CopyIndependence)
{
  occ::handle<Geom2d_BSplineCurve> aCopy = new Geom2d_BSplineCurve(*myOriginalCurve);
  myOriginalCurve->SetPole(2, gp_Pnt2d(10, 10));
  EXPECT_FALSE(aCopy->Pole(2).IsEqual(gp_Pnt2d(10, 10), 1e-10));
}

TEST_F(Geom2d_BSplineCurve_Test, SetWeight)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles(1) = gp_Pnt2d(0, 0);
  aPoles(2) = gp_Pnt2d(1, 1);
  aPoles(3) = gp_Pnt2d(2, 0);

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

  occ::handle<Geom2d_BSplineCurve> aCurve =
    new Geom2d_BSplineCurve(aPoles, aWeights, aKnots, aMults, 2);

  gp_Pnt2d aMidBefore = aCurve->Value(0.5);
  aCurve->SetWeight(2, 5.0);
  EXPECT_DOUBLE_EQ(aCurve->Weight(2), 5.0);
  EXPECT_TRUE(aCurve->IsRational());

  gp_Pnt2d aMidAfter = aCurve->Value(0.5);
  EXPECT_GT(aMidAfter.Y(), aMidBefore.Y());
}

TEST_F(Geom2d_BSplineCurve_Test, MovePoint)
{
  gp_Pnt2d aTarget(1.5, 0.8);
  int      aFirst = 0, aLast = 0;
  myOriginalCurve->MovePoint(0.5, aTarget, 1, myOriginalCurve->NbPoles(), aFirst, aLast);
  EXPECT_GT(aFirst, 0);
  gp_Pnt2d aMoved = myOriginalCurve->Value(0.5);
  EXPECT_TRUE(aMoved.IsEqual(aTarget, 1e-6));
}

TEST_F(Geom2d_BSplineCurve_Test, LocateU)
{
  myOriginalCurve->InsertKnot(0.5);
  int anI1 = 0, anI2 = 0;
  myOriginalCurve->LocateU(0.25, 1e-10, anI1, anI2);
  EXPECT_GE(anI1, 1);
  EXPECT_LE(anI2, myOriginalCurve->NbKnots());
}

TEST_F(Geom2d_BSplineCurve_Test, LocalD1)
{
  myOriginalCurve->InsertKnot(0.5);
  gp_Pnt2d aPnt, aPntL;
  gp_Vec2d aV1, aV1L;
  myOriginalCurve->D1(0.25, aPnt, aV1);
  myOriginalCurve->LocalD1(0.25, 1, 2, aPntL, aV1L);
  EXPECT_TRUE(aPnt.IsEqual(aPntL, 1e-10));
  EXPECT_NEAR(aV1.X(), aV1L.X(), 1e-10);
  EXPECT_NEAR(aV1.Y(), aV1L.Y(), 1e-10);
}

TEST_F(Geom2d_BSplineCurve_Test, RationalCurveSegment)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles(1) = gp_Pnt2d(1, 0);
  aPoles(2) = gp_Pnt2d(1, 1);
  aPoles(3) = gp_Pnt2d(0, 1);

  NCollection_Array1<double> aWeights(1, 3);
  aWeights(1) = 1.0;
  aWeights(2) = 1.0 / sqrt(2.0);
  aWeights(3) = 1.0;

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 3;
  aMults(2) = 3;

  occ::handle<Geom2d_BSplineCurve> aCurve =
    new Geom2d_BSplineCurve(aPoles, aWeights, aKnots, aMults, 2);

  gp_Pnt2d aMid = aCurve->Value(0.5);
  aCurve->Segment(0.25, 0.75);
  gp_Pnt2d aMidAfter = aCurve->Value(0.5);
  EXPECT_TRUE(aMid.IsEqual(aMidAfter, 1e-6));
  EXPECT_TRUE(aCurve->IsRational());
}

TEST_F(Geom2d_BSplineCurve_Test, ClosedCurve)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
  aPoles(1) = gp_Pnt2d(0, 0);
  aPoles(2) = gp_Pnt2d(1, 1);
  aPoles(3) = gp_Pnt2d(2, 0);
  aPoles(4) = gp_Pnt2d(0, 0);

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 4;
  aMults(2) = 4;

  occ::handle<Geom2d_BSplineCurve> aCurve = new Geom2d_BSplineCurve(aPoles, aKnots, aMults, 3);
  EXPECT_TRUE(aCurve->IsClosed());
}

TEST_F(Geom2d_BSplineCurve_Test, InsertKnots_Multiple)
{
  NCollection_Array1<double> aNewKnots(1, 2);
  aNewKnots(1) = 0.25;
  aNewKnots(2) = 0.75;

  NCollection_Array1<int> aNewMults(1, 2);
  aNewMults(1) = 1;
  aNewMults(2) = 1;

  gp_Pnt2d aValBefore = myOriginalCurve->Value(0.5);
  myOriginalCurve->InsertKnots(aNewKnots, aNewMults);
  EXPECT_EQ(myOriginalCurve->NbKnots(), 4);
  gp_Pnt2d aValAfter = myOriginalCurve->Value(0.5);
  EXPECT_TRUE(aValBefore.IsEqual(aValAfter, 1e-10));
}

TEST_F(Geom2d_BSplineCurve_Test, WeightsArray_NonRational_ReturnsUnitWeights)
{
  ASSERT_FALSE(myOriginalCurve->IsRational());

  const NCollection_Array1<double>& aWeights = myOriginalCurve->WeightsArray();
  EXPECT_EQ(aWeights.Length(), myOriginalCurve->NbPoles());
  EXPECT_FALSE(aWeights.IsDeletable());
  for (int i = 1; i <= aWeights.Length(); ++i)
  {
    EXPECT_DOUBLE_EQ(aWeights(i), 1.0);
  }
  // Verify that the reference is stable
  EXPECT_EQ(&aWeights, &myOriginalCurve->WeightsArray());
}

TEST_F(Geom2d_BSplineCurve_Test, WeightsArray_Rational_ReturnsOwning)
{
  // Create a rational BSpline
  NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
  aPoles(1) = gp_Pnt2d(0, 0);
  aPoles(2) = gp_Pnt2d(1, 1);
  aPoles(3) = gp_Pnt2d(2, 1);
  aPoles(4) = gp_Pnt2d(3, 0);

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

  occ::handle<Geom2d_BSplineCurve> aRational =
    new Geom2d_BSplineCurve(aPoles, aWeightsIn, aKnots, aMults, 3);
  ASSERT_TRUE(aRational->IsRational());

  const NCollection_Array1<double>& aWeights = aRational->WeightsArray();
  EXPECT_EQ(aWeights.Length(), 4);
  EXPECT_TRUE(aWeights.IsDeletable());
  EXPECT_DOUBLE_EQ(aWeights(1), 1.0);
  EXPECT_DOUBLE_EQ(aWeights(2), 2.0);
  EXPECT_DOUBLE_EQ(aWeights(3), 3.0);
  EXPECT_DOUBLE_EQ(aWeights(4), 1.0);
  // Verify that the reference is stable
  EXPECT_EQ(&aWeights, &aRational->WeightsArray());
}
