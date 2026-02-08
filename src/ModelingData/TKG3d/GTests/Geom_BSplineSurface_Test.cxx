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

#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Trsf.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>

class Geom_BSplineSurface_Test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Create a simple BSpline surface for testing
    NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
    for (int i = 1; i <= 3; ++i)
    {
      for (int j = 1; j <= 3; ++j)
      {
        aPoles(i, j) = gp_Pnt(i, j, (i + j) * 0.1);
      }
    }

    NCollection_Array1<double> anUKnots(1, 2);
    anUKnots(1) = 0.0;
    anUKnots(2) = 1.0;

    NCollection_Array1<double> aVKnots(1, 2);
    aVKnots(1) = 0.0;
    aVKnots(2) = 1.0;

    NCollection_Array1<int> anUMults(1, 2);
    anUMults(1) = 3;
    anUMults(2) = 3;

    NCollection_Array1<int> aVMults(1, 2);
    aVMults(1) = 3;
    aVMults(2) = 3;

    myOriginalSurface = new Geom_BSplineSurface(aPoles, anUKnots, aVKnots, anUMults, aVMults, 2, 2);
  }

  occ::handle<Geom_BSplineSurface> myOriginalSurface;
};

TEST_F(Geom_BSplineSurface_Test, CopyConstructorBasicProperties)
{
  // Test copy constructor
  occ::handle<Geom_BSplineSurface> aCopiedSurface = new Geom_BSplineSurface(*myOriginalSurface);

  // Verify basic properties are identical
  EXPECT_EQ(myOriginalSurface->UDegree(), aCopiedSurface->UDegree());
  EXPECT_EQ(myOriginalSurface->VDegree(), aCopiedSurface->VDegree());
  EXPECT_EQ(myOriginalSurface->NbUPoles(), aCopiedSurface->NbUPoles());
  EXPECT_EQ(myOriginalSurface->NbVPoles(), aCopiedSurface->NbVPoles());
  EXPECT_EQ(myOriginalSurface->NbUKnots(), aCopiedSurface->NbUKnots());
  EXPECT_EQ(myOriginalSurface->NbVKnots(), aCopiedSurface->NbVKnots());
  EXPECT_EQ(myOriginalSurface->IsUPeriodic(), aCopiedSurface->IsUPeriodic());
  EXPECT_EQ(myOriginalSurface->IsVPeriodic(), aCopiedSurface->IsVPeriodic());
}

TEST_F(Geom_BSplineSurface_Test, CopyConstructorPoles)
{
  occ::handle<Geom_BSplineSurface> aCopiedSurface = new Geom_BSplineSurface(*myOriginalSurface);

  // Verify all poles are identical
  for (int i = 1; i <= myOriginalSurface->NbUPoles(); ++i)
  {
    for (int j = 1; j <= myOriginalSurface->NbVPoles(); ++j)
    {
      gp_Pnt anOrigPole = myOriginalSurface->Pole(i, j);
      gp_Pnt aCopyPole  = aCopiedSurface->Pole(i, j);
      EXPECT_TRUE(anOrigPole.IsEqual(aCopyPole, 1e-10));
    }
  }
}

TEST_F(Geom_BSplineSurface_Test, CopyConstructorKnots)
{
  occ::handle<Geom_BSplineSurface> aCopiedSurface = new Geom_BSplineSurface(*myOriginalSurface);

  // Verify U knots are identical
  for (int i = 1; i <= myOriginalSurface->NbUKnots(); ++i)
  {
    EXPECT_DOUBLE_EQ(myOriginalSurface->UKnot(i), aCopiedSurface->UKnot(i));
    EXPECT_EQ(myOriginalSurface->UMultiplicity(i), aCopiedSurface->UMultiplicity(i));
  }

  // Verify V knots are identical
  for (int i = 1; i <= myOriginalSurface->NbVKnots(); ++i)
  {
    EXPECT_DOUBLE_EQ(myOriginalSurface->VKnot(i), aCopiedSurface->VKnot(i));
    EXPECT_EQ(myOriginalSurface->VMultiplicity(i), aCopiedSurface->VMultiplicity(i));
  }
}

TEST_F(Geom_BSplineSurface_Test, CopyMethodUsesOptimizedConstructor)
{
  // Test that Copy() method uses the optimized copy constructor
  occ::handle<Geom_Geometry>       aCopiedGeom = myOriginalSurface->Copy();
  occ::handle<Geom_BSplineSurface> aCopiedSurface =
    occ::down_cast<Geom_BSplineSurface>(aCopiedGeom);

  EXPECT_FALSE(aCopiedSurface.IsNull());

  // Verify the copy is functionally identical
  EXPECT_EQ(myOriginalSurface->UDegree(), aCopiedSurface->UDegree());
  EXPECT_EQ(myOriginalSurface->VDegree(), aCopiedSurface->VDegree());

  // Test evaluation at several points
  for (double u = 0.0; u <= 1.0; u += 0.5)
  {
    for (double v = 0.0; v <= 1.0; v += 0.5)
    {
      gp_Pnt anOrigPnt = myOriginalSurface->Value(u, v);
      gp_Pnt aCopyPnt  = aCopiedSurface->Value(u, v);
      EXPECT_TRUE(anOrigPnt.IsEqual(aCopyPnt, 1e-10));
    }
  }
}

TEST_F(Geom_BSplineSurface_Test, CopyIndependence)
{
  occ::handle<Geom_BSplineSurface> aCopiedSurface = new Geom_BSplineSurface(*myOriginalSurface);

  // Modify the original surface
  gp_Pnt aNewPole(10, 10, 10);
  myOriginalSurface->SetPole(2, 2, aNewPole);

  // Verify the copied surface is not affected
  gp_Pnt anOrigPole = aCopiedSurface->Pole(2, 2);
  EXPECT_FALSE(anOrigPole.IsEqual(aNewPole, 1e-10));
}

TEST_F(Geom_BSplineSurface_Test, Evaluation_D0)
{
  gp_Pnt aPnt;
  myOriginalSurface->D0(0.0, 0.0, aPnt);
  EXPECT_TRUE(aPnt.IsEqual(gp_Pnt(1, 1, 0.2), 1e-10));
  myOriginalSurface->D0(1.0, 1.0, aPnt);
  EXPECT_TRUE(aPnt.IsEqual(gp_Pnt(3, 3, 0.6), 1e-10));
}

TEST_F(Geom_BSplineSurface_Test, Evaluation_D1)
{
  gp_Pnt aPnt;
  gp_Vec aD1U, aD1V;
  myOriginalSurface->D1(0.5, 0.5, aPnt, aD1U, aD1V);
  EXPECT_GT(aD1U.Magnitude(), 0.0);
  EXPECT_GT(aD1V.Magnitude(), 0.0);
}

TEST_F(Geom_BSplineSurface_Test, Evaluation_D2)
{
  gp_Pnt aPnt;
  gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV;
  myOriginalSurface->D2(0.5, 0.5, aPnt, aD1U, aD1V, aD2U, aD2V, aD2UV);
  EXPECT_GT(aD1U.Magnitude(), 0.0);
}

TEST_F(Geom_BSplineSurface_Test, Evaluation_DN)
{
  gp_Vec aDN10 = myOriginalSurface->DN(0.5, 0.5, 1, 0);
  EXPECT_GT(aDN10.Magnitude(), 0.0);
  gp_Vec aDN01 = myOriginalSurface->DN(0.5, 0.5, 0, 1);
  EXPECT_GT(aDN01.Magnitude(), 0.0);
}

TEST_F(Geom_BSplineSurface_Test, Properties)
{
  EXPECT_EQ(myOriginalSurface->UDegree(), 2);
  EXPECT_EQ(myOriginalSurface->VDegree(), 2);
  EXPECT_FALSE(myOriginalSurface->IsUPeriodic());
  EXPECT_FALSE(myOriginalSurface->IsVPeriodic());
  EXPECT_FALSE(myOriginalSurface->IsURational());
  EXPECT_FALSE(myOriginalSurface->IsVRational());
  EXPECT_EQ(myOriginalSurface->NbUPoles(), 3);
  EXPECT_EQ(myOriginalSurface->NbVPoles(), 3);
  EXPECT_EQ(myOriginalSurface->NbUKnots(), 2);
  EXPECT_EQ(myOriginalSurface->NbVKnots(), 2);
}

TEST_F(Geom_BSplineSurface_Test, Bounds)
{
  double aU1, aU2, aV1, aV2;
  myOriginalSurface->Bounds(aU1, aU2, aV1, aV2);
  EXPECT_DOUBLE_EQ(aU1, 0.0);
  EXPECT_DOUBLE_EQ(aU2, 1.0);
  EXPECT_DOUBLE_EQ(aV1, 0.0);
  EXPECT_DOUBLE_EQ(aV2, 1.0);
}

TEST_F(Geom_BSplineSurface_Test, SetPole)
{
  gp_Pnt aNewPole(5, 5, 5);
  myOriginalSurface->SetPole(2, 2, aNewPole);
  EXPECT_TRUE(myOriginalSurface->Pole(2, 2).IsEqual(aNewPole, 1e-10));
}

TEST_F(Geom_BSplineSurface_Test, SetWeight)
{
  // Create a rational surface with non-planar poles and non-uniform weights
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  NCollection_Array2<double> aWeights(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      aPoles(i, j) = gp_Pnt(i, j, (i == 2 && j == 2) ? 1.0 : 0.0);
      aWeights(i, j) = 1.0;
    }
  }
  aWeights(2, 2) = 2.0;

  NCollection_Array1<double> anUKnots(1, 2), aVKnots(1, 2);
  anUKnots(1) = 0.0; anUKnots(2) = 1.0;
  aVKnots(1) = 0.0;  aVKnots(2) = 1.0;

  NCollection_Array1<int> anUMults(1, 2), aVMults(1, 2);
  anUMults(1) = 3; anUMults(2) = 3;
  aVMults(1) = 3;  aVMults(2) = 3;

  occ::handle<Geom_BSplineSurface> aSurf =
    new Geom_BSplineSurface(aPoles, aWeights, anUKnots, aVKnots, anUMults, aVMults, 2, 2);
  EXPECT_TRUE(aSurf->IsURational() || aSurf->IsVRational());

  gp_Pnt aValBefore = aSurf->Value(0.5, 0.5);
  aSurf->SetWeight(2, 2, 10.0);
  EXPECT_DOUBLE_EQ(aSurf->Weight(2, 2), 10.0);
  gp_Pnt aValAfter = aSurf->Value(0.5, 0.5);
  // Higher weight at the elevated center pole pulls surface up
  EXPECT_GT(aValAfter.Z(), aValBefore.Z());
}

TEST_F(Geom_BSplineSurface_Test, IncreaseDegree)
{
  gp_Pnt aValBefore = myOriginalSurface->Value(0.5, 0.5);
  myOriginalSurface->IncreaseDegree(4, 3);
  EXPECT_EQ(myOriginalSurface->UDegree(), 4);
  EXPECT_EQ(myOriginalSurface->VDegree(), 3);
  gp_Pnt aValAfter = myOriginalSurface->Value(0.5, 0.5);
  EXPECT_TRUE(aValBefore.IsEqual(aValAfter, 1e-10));
}

TEST_F(Geom_BSplineSurface_Test, InsertUKnot)
{
  gp_Pnt aValBefore = myOriginalSurface->Value(0.5, 0.5);
  myOriginalSurface->InsertUKnot(0.5, 1, 0.0);
  EXPECT_EQ(myOriginalSurface->NbUKnots(), 3);
  gp_Pnt aValAfter = myOriginalSurface->Value(0.5, 0.5);
  EXPECT_TRUE(aValBefore.IsEqual(aValAfter, 1e-10));
}

TEST_F(Geom_BSplineSurface_Test, InsertVKnot)
{
  gp_Pnt aValBefore = myOriginalSurface->Value(0.5, 0.5);
  myOriginalSurface->InsertVKnot(0.5, 1, 0.0);
  EXPECT_EQ(myOriginalSurface->NbVKnots(), 3);
  gp_Pnt aValAfter = myOriginalSurface->Value(0.5, 0.5);
  EXPECT_TRUE(aValBefore.IsEqual(aValAfter, 1e-10));
}

TEST_F(Geom_BSplineSurface_Test, RemoveUKnot)
{
  myOriginalSurface->InsertUKnot(0.5, 1, 0.0);
  EXPECT_EQ(myOriginalSurface->NbUKnots(), 3);

  gp_Pnt aValBefore = myOriginalSurface->Value(0.25, 0.5);
  bool isRemoved = myOriginalSurface->RemoveUKnot(2, 0, 1e-6);
  EXPECT_TRUE(isRemoved);
  EXPECT_EQ(myOriginalSurface->NbUKnots(), 2);
  gp_Pnt aValAfter = myOriginalSurface->Value(0.25, 0.5);
  EXPECT_TRUE(aValBefore.IsEqual(aValAfter, 1e-6));
}

TEST_F(Geom_BSplineSurface_Test, Segment)
{
  gp_Pnt aPntBefore = myOriginalSurface->Value(0.5, 0.5);
  myOriginalSurface->Segment(0.25, 0.75, 0.25, 0.75);

  double aU1, aU2, aV1, aV2;
  myOriginalSurface->Bounds(aU1, aU2, aV1, aV2);
  EXPECT_NEAR(aU1, 0.25, 1e-10);
  EXPECT_NEAR(aU2, 0.75, 1e-10);
  EXPECT_NEAR(aV1, 0.25, 1e-10);
  EXPECT_NEAR(aV2, 0.75, 1e-10);

  gp_Pnt aPntAfter = myOriginalSurface->Value(0.5, 0.5);
  EXPECT_TRUE(aPntBefore.IsEqual(aPntAfter, 1e-6));
}

TEST_F(Geom_BSplineSurface_Test, ExchangeUV)
{
  gp_Pnt aValBefore = myOriginalSurface->Value(0.3, 0.7);
  int aUDeg = myOriginalSurface->UDegree();
  int aVDeg = myOriginalSurface->VDegree();

  myOriginalSurface->ExchangeUV();
  EXPECT_EQ(myOriginalSurface->UDegree(), aVDeg);
  EXPECT_EQ(myOriginalSurface->VDegree(), aUDeg);

  gp_Pnt aValAfter = myOriginalSurface->Value(0.7, 0.3);
  EXPECT_TRUE(aValBefore.IsEqual(aValAfter, 1e-10));
}

TEST_F(Geom_BSplineSurface_Test, UReverse)
{
  gp_Pnt aValBefore = myOriginalSurface->Value(0.3, 0.5);
  myOriginalSurface->UReverse();
  gp_Pnt aValAfter = myOriginalSurface->Value(0.7, 0.5);
  EXPECT_TRUE(aValBefore.IsEqual(aValAfter, 1e-10));
}

TEST_F(Geom_BSplineSurface_Test, VReverse)
{
  gp_Pnt aValBefore = myOriginalSurface->Value(0.5, 0.3);
  myOriginalSurface->VReverse();
  gp_Pnt aValAfter = myOriginalSurface->Value(0.5, 0.7);
  EXPECT_TRUE(aValBefore.IsEqual(aValAfter, 1e-10));
}

TEST_F(Geom_BSplineSurface_Test, UIso)
{
  occ::handle<Geom_Curve> anIso = myOriginalSurface->UIso(0.5);
  EXPECT_FALSE(anIso.IsNull());

  // UIso at u=0.5 should match surface evaluation
  for (double v = 0.0; v <= 1.0; v += 0.25)
  {
    gp_Pnt aSurfPnt = myOriginalSurface->Value(0.5, v);
    gp_Pnt anIsoPnt = anIso->Value(v);
    EXPECT_TRUE(aSurfPnt.IsEqual(anIsoPnt, 1e-10));
  }
}

TEST_F(Geom_BSplineSurface_Test, VIso)
{
  occ::handle<Geom_Curve> anIso = myOriginalSurface->VIso(0.5);
  EXPECT_FALSE(anIso.IsNull());

  for (double u = 0.0; u <= 1.0; u += 0.25)
  {
    gp_Pnt aSurfPnt = myOriginalSurface->Value(u, 0.5);
    gp_Pnt anIsoPnt = anIso->Value(u);
    EXPECT_TRUE(aSurfPnt.IsEqual(anIsoPnt, 1e-10));
  }
}

TEST_F(Geom_BSplineSurface_Test, Resolution)
{
  double aUTol = 0.0, aVTol = 0.0;
  myOriginalSurface->Resolution(1.0, aUTol, aVTol);
  EXPECT_GT(aUTol, 0.0);
  EXPECT_GT(aVTol, 0.0);
}

TEST_F(Geom_BSplineSurface_Test, Transform)
{
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(10, 20, 30));
  gp_Pnt aPtBefore = myOriginalSurface->Value(0.5, 0.5);
  myOriginalSurface->Transform(aTrsf);
  gp_Pnt aPtAfter = myOriginalSurface->Value(0.5, 0.5);
  EXPECT_NEAR(aPtAfter.X(), aPtBefore.X() + 10.0, 1e-10);
  EXPECT_NEAR(aPtAfter.Y(), aPtBefore.Y() + 20.0, 1e-10);
  EXPECT_NEAR(aPtAfter.Z(), aPtBefore.Z() + 30.0, 1e-10);
}

TEST_F(Geom_BSplineSurface_Test, KnotsAccess)
{
  const NCollection_Array1<double>& aUKnots = myOriginalSurface->UKnots();
  EXPECT_EQ(aUKnots.Length(), 2);
  EXPECT_DOUBLE_EQ(aUKnots(1), 0.0);
  EXPECT_DOUBLE_EQ(aUKnots(2), 1.0);

  const NCollection_Array1<double>& aVKnots = myOriginalSurface->VKnots();
  EXPECT_EQ(aVKnots.Length(), 2);

  const NCollection_Array1<int>& aUMults = myOriginalSurface->UMultiplicities();
  EXPECT_EQ(aUMults.Length(), 2);
  EXPECT_EQ(aUMults(1), 3);

  const NCollection_Array1<int>& aVMults = myOriginalSurface->VMultiplicities();
  EXPECT_EQ(aVMults.Length(), 2);
}

TEST_F(Geom_BSplineSurface_Test, WeightsAccess_NonRational)
{
  const NCollection_Array2<double>* aWeights = myOriginalSurface->Weights();
  EXPECT_EQ(aWeights, nullptr);
}

TEST_F(Geom_BSplineSurface_Test, PeriodicSurface_SetUNotPeriodic)
{
  // U-periodic surface: degree 2, 4 U-knots of mult 1 → NbUPoles = 1+2 = 3
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
    for (int j = 1; j <= 3; ++j)
      aPoles(i, j) = gp_Pnt(cos(2.0 * M_PI * (i - 1) / 3.0),
                             sin(2.0 * M_PI * (i - 1) / 3.0),
                             j * 0.5);

  NCollection_Array1<double> anUKnots(1, 4);
  anUKnots(1) = 0.0;
  anUKnots(2) = 1.0;
  anUKnots(3) = 2.0;
  anUKnots(4) = 3.0;

  NCollection_Array1<double> aVKnots(1, 2);
  aVKnots(1) = 0.0;
  aVKnots(2) = 1.0;

  NCollection_Array1<int> anUMults(1, 4);
  anUMults.Init(1);

  NCollection_Array1<int> aVMults(1, 2);
  aVMults(1) = 3;
  aVMults(2) = 3;

  occ::handle<Geom_BSplineSurface> aSurf =
    new Geom_BSplineSurface(aPoles, anUKnots, aVKnots, anUMults, aVMults, 2, 2, true, false);
  EXPECT_TRUE(aSurf->IsUPeriodic());

  gp_Pnt aVal = aSurf->Value(0.5, 0.5);
  aSurf->SetUNotPeriodic();
  EXPECT_FALSE(aSurf->IsUPeriodic());
  gp_Pnt aVal2 = aSurf->Value(0.5, 0.5);
  EXPECT_TRUE(aVal.IsEqual(aVal2, 1e-10));
}

TEST_F(Geom_BSplineSurface_Test, PeriodicSurface_SetVNotPeriodic)
{
  // V-periodic surface: degree 2, 4 V-knots of mult 1 → NbVPoles = 1+2 = 3
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
    for (int j = 1; j <= 3; ++j)
      aPoles(i, j) = gp_Pnt(i * 0.5,
                             cos(2.0 * M_PI * (j - 1) / 3.0),
                             sin(2.0 * M_PI * (j - 1) / 3.0));

  NCollection_Array1<double> anUKnots(1, 2);
  anUKnots(1) = 0.0;
  anUKnots(2) = 1.0;

  NCollection_Array1<double> aVKnots(1, 4);
  aVKnots(1) = 0.0;
  aVKnots(2) = 1.0;
  aVKnots(3) = 2.0;
  aVKnots(4) = 3.0;

  NCollection_Array1<int> anUMults(1, 2);
  anUMults(1) = 3;
  anUMults(2) = 3;

  NCollection_Array1<int> aVMults(1, 4);
  aVMults.Init(1);

  occ::handle<Geom_BSplineSurface> aSurf =
    new Geom_BSplineSurface(aPoles, anUKnots, aVKnots, anUMults, aVMults, 2, 2, false, true);
  EXPECT_TRUE(aSurf->IsVPeriodic());

  gp_Pnt aVal = aSurf->Value(0.5, 0.5);
  aSurf->SetVNotPeriodic();
  EXPECT_FALSE(aSurf->IsVPeriodic());
  gp_Pnt aVal2 = aSurf->Value(0.5, 0.5);
  EXPECT_TRUE(aVal.IsEqual(aVal2, 1e-10));
}

TEST_F(Geom_BSplineSurface_Test, RationalSurface)
{
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  NCollection_Array2<double> aWeights(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      aPoles(i, j) = gp_Pnt(i, j, 0);
      aWeights(i, j) = 1.0;
    }
  }
  aWeights(2, 2) = 3.0;

  NCollection_Array1<double> anUKnots(1, 2), aVKnots(1, 2);
  anUKnots(1) = 0.0; anUKnots(2) = 1.0;
  aVKnots(1) = 0.0;  aVKnots(2) = 1.0;

  NCollection_Array1<int> anUMults(1, 2), aVMults(1, 2);
  anUMults(1) = 3; anUMults(2) = 3;
  aVMults(1) = 3;  aVMults(2) = 3;

  occ::handle<Geom_BSplineSurface> aSurf =
    new Geom_BSplineSurface(aPoles, aWeights, anUKnots, aVKnots, anUMults, aVMults, 2, 2);
  EXPECT_TRUE(aSurf->IsURational());
  EXPECT_TRUE(aSurf->IsVRational());

  const NCollection_Array2<double>* aW = aSurf->Weights();
  ASSERT_NE(aW, nullptr);
  EXPECT_DOUBLE_EQ((*aW)(2, 2), 3.0);

  // Evaluate
  gp_Pnt aMid = aSurf->Value(0.5, 0.5);
  // High weight at center should pull surface toward (2,2,0)
  EXPECT_NEAR(aMid.X(), 2.0, 0.1);
  EXPECT_NEAR(aMid.Y(), 2.0, 0.1);
}

TEST_F(Geom_BSplineSurface_Test, LocalEvaluation)
{
  myOriginalSurface->InsertUKnot(0.5, 1, 0.0);
  EXPECT_EQ(myOriginalSurface->NbUKnots(), 3);

  gp_Pnt aGlobal = myOriginalSurface->Value(0.25, 0.5);
  gp_Pnt aLocal;
  myOriginalSurface->LocalD0(0.25, 0.5, 1, 2, 1, 2, aLocal);
  EXPECT_TRUE(aGlobal.IsEqual(aLocal, 1e-10));
}

TEST_F(Geom_BSplineSurface_Test, SetPoleRow)
{
  NCollection_Array1<gp_Pnt> aNewRow(1, 3);
  aNewRow(1) = gp_Pnt(10, 1, 0);
  aNewRow(2) = gp_Pnt(10, 2, 0);
  aNewRow(3) = gp_Pnt(10, 3, 0);

  myOriginalSurface->SetPoleRow(2, aNewRow);
  for (int j = 1; j <= 3; ++j)
  {
    EXPECT_NEAR(myOriginalSurface->Pole(2, j).X(), 10.0, 1e-10);
  }
}

TEST_F(Geom_BSplineSurface_Test, SetPoleCol)
{
  NCollection_Array1<gp_Pnt> aNewCol(1, 3);
  aNewCol(1) = gp_Pnt(1, 10, 0);
  aNewCol(2) = gp_Pnt(2, 10, 0);
  aNewCol(3) = gp_Pnt(3, 10, 0);

  myOriginalSurface->SetPoleCol(2, aNewCol);
  for (int i = 1; i <= 3; ++i)
  {
    EXPECT_NEAR(myOriginalSurface->Pole(i, 2).Y(), 10.0, 1e-10);
  }
}

TEST_F(Geom_BSplineSurface_Test, SetWeightRow)
{
  // Create rational surface
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  NCollection_Array2<double> aWeights(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
    for (int j = 1; j <= 3; ++j)
    {
      aPoles(i, j) = gp_Pnt(i, j, (i == 2 && j == 2) ? 1.0 : 0.0);
      aWeights(i, j) = 1.0;
    }
  aWeights(2, 2) = 2.0;

  NCollection_Array1<double> anUKnots(1, 2), aVKnots(1, 2);
  anUKnots(1) = 0.0; anUKnots(2) = 1.0;
  aVKnots(1) = 0.0;  aVKnots(2) = 1.0;

  NCollection_Array1<int> anUMults(1, 2), aVMults(1, 2);
  anUMults(1) = 3; anUMults(2) = 3;
  aVMults(1) = 3;  aVMults(2) = 3;

  occ::handle<Geom_BSplineSurface> aSurf =
    new Geom_BSplineSurface(aPoles, aWeights, anUKnots, aVKnots, anUMults, aVMults, 2, 2);

  NCollection_Array1<double> aNewWeights(1, 3);
  aNewWeights(1) = 3.0;
  aNewWeights(2) = 4.0;
  aNewWeights(3) = 5.0;

  aSurf->SetWeightRow(2, aNewWeights);
  EXPECT_DOUBLE_EQ(aSurf->Weight(2, 1), 3.0);
  EXPECT_DOUBLE_EQ(aSurf->Weight(2, 2), 4.0);
  EXPECT_DOUBLE_EQ(aSurf->Weight(2, 3), 5.0);
}

TEST_F(Geom_BSplineSurface_Test, SetWeightCol)
{
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  NCollection_Array2<double> aWeights(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
    for (int j = 1; j <= 3; ++j)
    {
      aPoles(i, j) = gp_Pnt(i, j, (i == 2 && j == 2) ? 1.0 : 0.0);
      aWeights(i, j) = 1.0;
    }
  aWeights(2, 2) = 2.0;

  NCollection_Array1<double> anUKnots(1, 2), aVKnots(1, 2);
  anUKnots(1) = 0.0; anUKnots(2) = 1.0;
  aVKnots(1) = 0.0;  aVKnots(2) = 1.0;

  NCollection_Array1<int> anUMults(1, 2), aVMults(1, 2);
  anUMults(1) = 3; anUMults(2) = 3;
  aVMults(1) = 3;  aVMults(2) = 3;

  occ::handle<Geom_BSplineSurface> aSurf =
    new Geom_BSplineSurface(aPoles, aWeights, anUKnots, aVKnots, anUMults, aVMults, 2, 2);

  NCollection_Array1<double> aNewWeights(1, 3);
  aNewWeights(1) = 6.0;
  aNewWeights(2) = 7.0;
  aNewWeights(3) = 8.0;

  aSurf->SetWeightCol(2, aNewWeights);
  EXPECT_DOUBLE_EQ(aSurf->Weight(1, 2), 6.0);
  EXPECT_DOUBLE_EQ(aSurf->Weight(2, 2), 7.0);
  EXPECT_DOUBLE_EQ(aSurf->Weight(3, 2), 8.0);
}

TEST_F(Geom_BSplineSurface_Test, RemoveVKnot)
{
  myOriginalSurface->InsertVKnot(0.5, 1, 0.0);
  EXPECT_EQ(myOriginalSurface->NbVKnots(), 3);

  gp_Pnt aValBefore = myOriginalSurface->Value(0.5, 0.25);
  bool isRemoved = myOriginalSurface->RemoveVKnot(2, 0, 1e-6);
  EXPECT_TRUE(isRemoved);
  EXPECT_EQ(myOriginalSurface->NbVKnots(), 2);
  gp_Pnt aValAfter = myOriginalSurface->Value(0.5, 0.25);
  EXPECT_TRUE(aValBefore.IsEqual(aValAfter, 1e-6));
}

TEST_F(Geom_BSplineSurface_Test, MovePoint)
{
  gp_Pnt aTarget(2, 2, 1);
  int aUFirst = 0, aULast = 0, aVFirst = 0, aVLast = 0;
  myOriginalSurface->MovePoint(0.5, 0.5, aTarget,
                               1, myOriginalSurface->NbUPoles(),
                               1, myOriginalSurface->NbVPoles(),
                               aUFirst, aULast, aVFirst, aVLast);
  gp_Pnt aMoved = myOriginalSurface->Value(0.5, 0.5);
  EXPECT_TRUE(aMoved.IsEqual(aTarget, 1e-6));
}

TEST_F(Geom_BSplineSurface_Test, LocateU)
{
  myOriginalSurface->InsertUKnot(0.5, 1, 0.0);
  int anI1 = 0, anI2 = 0;
  myOriginalSurface->LocateU(0.25, 1e-10, anI1, anI2);
  EXPECT_GE(anI1, 1);
  EXPECT_LE(anI2, myOriginalSurface->NbUKnots());
}

TEST_F(Geom_BSplineSurface_Test, LocateV)
{
  myOriginalSurface->InsertVKnot(0.5, 1, 0.0);
  int anI1 = 0, anI2 = 0;
  myOriginalSurface->LocateV(0.25, 1e-10, anI1, anI2);
  EXPECT_GE(anI1, 1);
  EXPECT_LE(anI2, myOriginalSurface->NbVKnots());
}

TEST_F(Geom_BSplineSurface_Test, RationalSurface_SetUNotPeriodic)
{
  // Rational U-periodic surface: exercises the SetUNotPeriodic weight-handling code
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  NCollection_Array2<double> aWeights(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
    for (int j = 1; j <= 3; ++j)
    {
      aPoles(i, j) = gp_Pnt(cos(2.0 * M_PI * (i - 1) / 3.0),
                             sin(2.0 * M_PI * (i - 1) / 3.0),
                             j * 0.5);
      aWeights(i, j) = 1.0 + 0.5 * (i - 1);
    }

  NCollection_Array1<double> anUKnots(1, 4);
  anUKnots(1) = 0.0; anUKnots(2) = 1.0; anUKnots(3) = 2.0; anUKnots(4) = 3.0;

  NCollection_Array1<double> aVKnots(1, 2);
  aVKnots(1) = 0.0; aVKnots(2) = 1.0;

  NCollection_Array1<int> anUMults(1, 4);
  anUMults.Init(1);

  NCollection_Array1<int> aVMults(1, 2);
  aVMults(1) = 3; aVMults(2) = 3;

  occ::handle<Geom_BSplineSurface> aSurf =
    new Geom_BSplineSurface(aPoles, aWeights, anUKnots, aVKnots, anUMults, aVMults,
                            2, 2, true, false);
  EXPECT_TRUE(aSurf->IsUPeriodic());
  EXPECT_TRUE(aSurf->IsURational() || aSurf->IsVRational());

  gp_Pnt aVal = aSurf->Value(0.5, 0.5);
  aSurf->SetUNotPeriodic();
  EXPECT_FALSE(aSurf->IsUPeriodic());
  gp_Pnt aVal2 = aSurf->Value(0.5, 0.5);
  EXPECT_TRUE(aVal.IsEqual(aVal2, 1e-10));
}

TEST_F(Geom_BSplineSurface_Test, RationalSurface_SetVNotPeriodic)
{
  // Rational V-periodic surface
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  NCollection_Array2<double> aWeights(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
    for (int j = 1; j <= 3; ++j)
    {
      aPoles(i, j) = gp_Pnt(i * 0.5,
                             cos(2.0 * M_PI * (j - 1) / 3.0),
                             sin(2.0 * M_PI * (j - 1) / 3.0));
      aWeights(i, j) = 1.0 + 0.5 * (j - 1);
    }

  NCollection_Array1<double> anUKnots(1, 2);
  anUKnots(1) = 0.0; anUKnots(2) = 1.0;

  NCollection_Array1<double> aVKnots(1, 4);
  aVKnots(1) = 0.0; aVKnots(2) = 1.0; aVKnots(3) = 2.0; aVKnots(4) = 3.0;

  NCollection_Array1<int> anUMults(1, 2);
  anUMults(1) = 3; anUMults(2) = 3;

  NCollection_Array1<int> aVMults(1, 4);
  aVMults.Init(1);

  occ::handle<Geom_BSplineSurface> aSurf =
    new Geom_BSplineSurface(aPoles, aWeights, anUKnots, aVKnots, anUMults, aVMults,
                            2, 2, false, true);
  EXPECT_TRUE(aSurf->IsVPeriodic());
  EXPECT_TRUE(aSurf->IsURational() || aSurf->IsVRational());

  gp_Pnt aVal = aSurf->Value(0.5, 0.5);
  aSurf->SetVNotPeriodic();
  EXPECT_FALSE(aSurf->IsVPeriodic());
  gp_Pnt aVal2 = aSurf->Value(0.5, 0.5);
  EXPECT_TRUE(aVal.IsEqual(aVal2, 1e-10));
}

TEST_F(Geom_BSplineSurface_Test, Evaluation_D3)
{
  gp_Pnt aPnt;
  gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV;
  gp_Vec aD3U, aD3V, aD3UUV, aD3UVV;
  myOriginalSurface->D3(0.5, 0.5, aPnt, aD1U, aD1V, aD2U, aD2V, aD2UV,
                        aD3U, aD3V, aD3UUV, aD3UVV);
  // Degree 2 surface: D3 should be zero
  EXPECT_NEAR(aD3U.Magnitude(), 0.0, 1e-10);
  EXPECT_NEAR(aD3V.Magnitude(), 0.0, 1e-10);
}

TEST_F(Geom_BSplineSurface_Test, IncreaseUMultiplicity)
{
  myOriginalSurface->InsertUKnot(0.5, 1, 0.0);
  EXPECT_EQ(myOriginalSurface->UMultiplicity(2), 1);

  gp_Pnt aValBefore = myOriginalSurface->Value(0.5, 0.5);
  myOriginalSurface->IncreaseUMultiplicity(2, 2);
  EXPECT_EQ(myOriginalSurface->UMultiplicity(2), 2);
  gp_Pnt aValAfter = myOriginalSurface->Value(0.5, 0.5);
  EXPECT_TRUE(aValBefore.IsEqual(aValAfter, 1e-10));
}

TEST_F(Geom_BSplineSurface_Test, SetUKnot)
{
  myOriginalSurface->InsertUKnot(0.5, 1, 0.0);
  myOriginalSurface->SetUKnot(2, 0.6);
  EXPECT_DOUBLE_EQ(myOriginalSurface->UKnot(2), 0.6);
}

TEST_F(Geom_BSplineSurface_Test, SetVKnot)
{
  myOriginalSurface->InsertVKnot(0.5, 1, 0.0);
  myOriginalSurface->SetVKnot(2, 0.6);
  EXPECT_DOUBLE_EQ(myOriginalSurface->VKnot(2), 0.6);
}

TEST_F(Geom_BSplineSurface_Test, RationalSurface_UIso)
{
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  NCollection_Array2<double> aWeights(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
    for (int j = 1; j <= 3; ++j)
    {
      aPoles(i, j) = gp_Pnt(i, j, (i + j) * 0.1);
      aWeights(i, j) = 1.0 + 0.5 * ((i - 1) + (j - 1));
    }

  NCollection_Array1<double> anUKnots(1, 2), aVKnots(1, 2);
  anUKnots(1) = 0.0; anUKnots(2) = 1.0;
  aVKnots(1) = 0.0;  aVKnots(2) = 1.0;

  NCollection_Array1<int> anUMults(1, 2), aVMults(1, 2);
  anUMults(1) = 3; anUMults(2) = 3;
  aVMults(1) = 3;  aVMults(2) = 3;

  occ::handle<Geom_BSplineSurface> aSurf =
    new Geom_BSplineSurface(aPoles, aWeights, anUKnots, aVKnots, anUMults, aVMults, 2, 2);

  occ::handle<Geom_Curve> anIso = aSurf->UIso(0.5);
  EXPECT_FALSE(anIso.IsNull());

  for (double v = 0.0; v <= 1.0; v += 0.25)
  {
    gp_Pnt aSurfPnt = aSurf->Value(0.5, v);
    gp_Pnt anIsoPnt = anIso->Value(v);
    EXPECT_TRUE(aSurfPnt.IsEqual(anIsoPnt, 1e-10));
  }
}

TEST_F(Geom_BSplineSurface_Test, CopyIndependence_Knots)
{
  occ::handle<Geom_BSplineSurface> aCopy = new Geom_BSplineSurface(*myOriginalSurface);
  myOriginalSurface->InsertUKnot(0.5, 1, 0.0);
  // Original now has 3 U-knots, copy should still have 2
  EXPECT_EQ(aCopy->NbUKnots(), 2);
  EXPECT_EQ(myOriginalSurface->NbUKnots(), 3);
}