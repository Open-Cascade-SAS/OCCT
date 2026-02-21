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

#include <Geom_BezierCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Trsf.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <OSD_Parallel.hxx>
#include <Precision.hxx>

#include <atomic>

#include <gtest/gtest.h>

class Geom_BezierSurface_Test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Create a simple Bezier surface for testing
    NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
    for (int i = 1; i <= 3; ++i)
    {
      for (int j = 1; j <= 3; ++j)
      {
        aPoles(i, j) = gp_Pnt(i, j, (i + j) * 0.1);
      }
    }

    myOriginalSurface = new Geom_BezierSurface(aPoles);
  }

  occ::handle<Geom_BezierSurface> myOriginalSurface;
};

TEST_F(Geom_BezierSurface_Test, CopyConstructorBasicProperties)
{
  // Test copy constructor
  occ::handle<Geom_BezierSurface> aCopiedSurface = new Geom_BezierSurface(*myOriginalSurface);

  // Verify basic properties are identical
  EXPECT_EQ(myOriginalSurface->UDegree(), aCopiedSurface->UDegree());
  EXPECT_EQ(myOriginalSurface->VDegree(), aCopiedSurface->VDegree());
  EXPECT_EQ(myOriginalSurface->NbUPoles(), aCopiedSurface->NbUPoles());
  EXPECT_EQ(myOriginalSurface->NbVPoles(), aCopiedSurface->NbVPoles());
  EXPECT_EQ(myOriginalSurface->IsURational(), aCopiedSurface->IsURational());
  EXPECT_EQ(myOriginalSurface->IsVRational(), aCopiedSurface->IsVRational());
}

TEST_F(Geom_BezierSurface_Test, CopyConstructorPoles)
{
  occ::handle<Geom_BezierSurface> aCopiedSurface = new Geom_BezierSurface(*myOriginalSurface);

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

TEST_F(Geom_BezierSurface_Test, CopyMethodUsesOptimizedConstructor)
{
  // Test that Copy() method uses the optimized copy constructor
  occ::handle<Geom_Geometry>      aCopiedGeom    = myOriginalSurface->Copy();
  occ::handle<Geom_BezierSurface> aCopiedSurface = occ::down_cast<Geom_BezierSurface>(aCopiedGeom);

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

TEST_F(Geom_BezierSurface_Test, RationalSurfaceCopyConstructor)
{
  // Create a rational Bezier surface
  NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 2);
  aPoles(1, 1) = gp_Pnt(0, 0, 0);
  aPoles(1, 2) = gp_Pnt(1, 0, 0);
  aPoles(2, 1) = gp_Pnt(0, 1, 0);
  aPoles(2, 2) = gp_Pnt(1, 1, 0);

  NCollection_Array2<double> aWeights(1, 2, 1, 2);
  aWeights(1, 1) = 1.0;
  aWeights(1, 2) = 2.0;
  aWeights(2, 1) = 2.0;
  aWeights(2, 2) = 1.0;

  occ::handle<Geom_BezierSurface> aRationalSurface = new Geom_BezierSurface(aPoles, aWeights);
  occ::handle<Geom_BezierSurface> aCopiedRational  = new Geom_BezierSurface(*aRationalSurface);

  EXPECT_TRUE(aCopiedRational->IsURational() || aCopiedRational->IsVRational());

  // Verify weights are copied correctly
  for (int i = 1; i <= aRationalSurface->NbUPoles(); ++i)
  {
    for (int j = 1; j <= aRationalSurface->NbVPoles(); ++j)
    {
      EXPECT_DOUBLE_EQ(aRationalSurface->Weight(i, j), aCopiedRational->Weight(i, j));
    }
  }
}

TEST_F(Geom_BezierSurface_Test, CopyIndependence)
{
  occ::handle<Geom_BezierSurface> aCopiedSurface = new Geom_BezierSurface(*myOriginalSurface);

  // Modify the original surface
  gp_Pnt aNewPole(10, 10, 10);
  myOriginalSurface->SetPole(2, 2, aNewPole);

  // Verify the copied surface is not affected
  gp_Pnt anOrigPole = aCopiedSurface->Pole(2, 2);
  EXPECT_FALSE(anOrigPole.IsEqual(aNewPole, 1e-10));
}

TEST_F(Geom_BezierSurface_Test, Evaluation_D0)
{
  gp_Pnt aPnt;
  myOriginalSurface->D0(0.0, 0.0, aPnt);
  EXPECT_TRUE(aPnt.IsEqual(gp_Pnt(1, 1, 0.2), 1e-10));
  myOriginalSurface->D0(1.0, 1.0, aPnt);
  EXPECT_TRUE(aPnt.IsEqual(gp_Pnt(3, 3, 0.6), 1e-10));
}

TEST_F(Geom_BezierSurface_Test, Evaluation_D1)
{
  gp_Pnt aPnt;
  gp_Vec aD1U, aD1V;
  myOriginalSurface->D1(0.5, 0.5, aPnt, aD1U, aD1V);
  EXPECT_GT(aD1U.Magnitude(), 0.0);
  EXPECT_GT(aD1V.Magnitude(), 0.0);
}

TEST_F(Geom_BezierSurface_Test, Evaluation_D2)
{
  gp_Pnt aPnt;
  gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV;
  myOriginalSurface->D2(0.5, 0.5, aPnt, aD1U, aD1V, aD2U, aD2V, aD2UV);
  EXPECT_GT(aD1U.Magnitude(), 0.0);
}

TEST_F(Geom_BezierSurface_Test, Evaluation_DN)
{
  gp_Vec aDN10 = myOriginalSurface->DN(0.5, 0.5, 1, 0);
  EXPECT_GT(aDN10.Magnitude(), 0.0);
  gp_Vec aDN01 = myOriginalSurface->DN(0.5, 0.5, 0, 1);
  EXPECT_GT(aDN01.Magnitude(), 0.0);
}

TEST_F(Geom_BezierSurface_Test, Properties)
{
  EXPECT_EQ(myOriginalSurface->UDegree(), 2);
  EXPECT_EQ(myOriginalSurface->VDegree(), 2);
  EXPECT_FALSE(myOriginalSurface->IsUPeriodic());
  EXPECT_FALSE(myOriginalSurface->IsVPeriodic());
  EXPECT_FALSE(myOriginalSurface->IsURational());
  EXPECT_FALSE(myOriginalSurface->IsVRational());
  EXPECT_EQ(myOriginalSurface->NbUPoles(), 3);
  EXPECT_EQ(myOriginalSurface->NbVPoles(), 3);
  EXPECT_TRUE(myOriginalSurface->IsCNu(100));
  EXPECT_TRUE(myOriginalSurface->IsCNv(100));
}

TEST_F(Geom_BezierSurface_Test, Bounds)
{
  double aU1, aU2, aV1, aV2;
  myOriginalSurface->Bounds(aU1, aU2, aV1, aV2);
  EXPECT_DOUBLE_EQ(aU1, 0.0);
  EXPECT_DOUBLE_EQ(aU2, 1.0);
  EXPECT_DOUBLE_EQ(aV1, 0.0);
  EXPECT_DOUBLE_EQ(aV2, 1.0);
}

TEST_F(Geom_BezierSurface_Test, SetPole)
{
  gp_Pnt aNewPole(5, 5, 5);
  myOriginalSurface->SetPole(2, 2, aNewPole);
  EXPECT_TRUE(myOriginalSurface->Pole(2, 2).IsEqual(aNewPole, 1e-10));
}

TEST_F(Geom_BezierSurface_Test, SetWeight)
{
  NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 2);
  NCollection_Array2<double> aWeights(1, 2, 1, 2);
  aPoles(1, 1)   = gp_Pnt(0, 0, 0);
  aPoles(1, 2)   = gp_Pnt(1, 0, 0);
  aPoles(2, 1)   = gp_Pnt(0, 1, 0);
  aPoles(2, 2)   = gp_Pnt(1, 1, 0);
  aWeights(1, 1) = 1.0;
  aWeights(1, 2) = 1.0;
  aWeights(2, 1) = 1.0;
  aWeights(2, 2) = 1.0;

  occ::handle<Geom_BezierSurface> aSurf      = new Geom_BezierSurface(aPoles, aWeights);
  gp_Pnt                          aValBefore = aSurf->Value(0.5, 0.5);

  aSurf->SetWeight(2, 2, 10.0);
  EXPECT_DOUBLE_EQ(aSurf->Weight(2, 2), 10.0);
  EXPECT_TRUE(aSurf->IsURational() || aSurf->IsVRational());

  gp_Pnt aValAfter = aSurf->Value(0.5, 0.5);
  EXPECT_FALSE(aValBefore.IsEqual(aValAfter, 1e-6));
}

TEST_F(Geom_BezierSurface_Test, Increase)
{
  gp_Pnt aValBefore = myOriginalSurface->Value(0.5, 0.5);
  myOriginalSurface->Increase(4, 3);
  EXPECT_EQ(myOriginalSurface->UDegree(), 4);
  EXPECT_EQ(myOriginalSurface->VDegree(), 3);
  EXPECT_EQ(myOriginalSurface->NbUPoles(), 5);
  EXPECT_EQ(myOriginalSurface->NbVPoles(), 4);
  gp_Pnt aValAfter = myOriginalSurface->Value(0.5, 0.5);
  EXPECT_TRUE(aValBefore.IsEqual(aValAfter, 1e-10));
}

TEST_F(Geom_BezierSurface_Test, Segment)
{
  gp_Pnt aPntBefore = myOriginalSurface->Value(0.5, 0.5);
  myOriginalSurface->Segment(0.25, 0.75, 0.25, 0.75);

  EXPECT_TRUE(myOriginalSurface->Value(0.5, 0.5).IsEqual(aPntBefore, 1e-6));
}

TEST_F(Geom_BezierSurface_Test, ExchangeUV)
{
  gp_Pnt aValBefore = myOriginalSurface->Value(0.3, 0.7);
  int    aUDeg      = myOriginalSurface->UDegree();
  int    aVDeg      = myOriginalSurface->VDegree();

  myOriginalSurface->ExchangeUV();
  EXPECT_EQ(myOriginalSurface->UDegree(), aVDeg);
  EXPECT_EQ(myOriginalSurface->VDegree(), aUDeg);

  gp_Pnt aValAfter = myOriginalSurface->Value(0.7, 0.3);
  EXPECT_TRUE(aValBefore.IsEqual(aValAfter, 1e-10));
}

TEST_F(Geom_BezierSurface_Test, UReverse)
{
  gp_Pnt aValBefore = myOriginalSurface->Value(0.3, 0.5);
  myOriginalSurface->UReverse();
  gp_Pnt aValAfter = myOriginalSurface->Value(0.7, 0.5);
  EXPECT_TRUE(aValBefore.IsEqual(aValAfter, 1e-10));
}

TEST_F(Geom_BezierSurface_Test, VReverse)
{
  gp_Pnt aValBefore = myOriginalSurface->Value(0.5, 0.3);
  myOriginalSurface->VReverse();
  gp_Pnt aValAfter = myOriginalSurface->Value(0.5, 0.7);
  EXPECT_TRUE(aValBefore.IsEqual(aValAfter, 1e-10));
}

TEST_F(Geom_BezierSurface_Test, UIso)
{
  occ::handle<Geom_Curve> anIso = myOriginalSurface->UIso(0.5);
  EXPECT_FALSE(anIso.IsNull());

  for (double v = 0.0; v <= 1.0; v += 0.25)
  {
    gp_Pnt aSurfPnt = myOriginalSurface->Value(0.5, v);
    gp_Pnt anIsoPnt = anIso->Value(v);
    EXPECT_TRUE(aSurfPnt.IsEqual(anIsoPnt, 1e-10));
  }
}

TEST_F(Geom_BezierSurface_Test, VIso)
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

TEST_F(Geom_BezierSurface_Test, Resolution)
{
  double aUTol = 0.0, aVTol = 0.0;
  myOriginalSurface->Resolution(1.0, aUTol, aVTol);
  EXPECT_GT(aUTol, 0.0);
  EXPECT_GT(aVTol, 0.0);
}

TEST_F(Geom_BezierSurface_Test, Transform)
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

TEST_F(Geom_BezierSurface_Test, InsertPoleRowAfter)
{
  NCollection_Array1<gp_Pnt> aNewRow(1, 3);
  aNewRow(1) = gp_Pnt(1.5, 1, 0);
  aNewRow(2) = gp_Pnt(1.5, 2, 0);
  aNewRow(3) = gp_Pnt(1.5, 3, 0);

  int aNbUBefore = myOriginalSurface->NbUPoles();
  myOriginalSurface->InsertPoleRowAfter(1, aNewRow);
  EXPECT_EQ(myOriginalSurface->NbUPoles(), aNbUBefore + 1);
  EXPECT_EQ(myOriginalSurface->UDegree(), 3);
}

TEST_F(Geom_BezierSurface_Test, InsertPoleColAfter)
{
  NCollection_Array1<gp_Pnt> aNewCol(1, 3);
  aNewCol(1) = gp_Pnt(1, 1.5, 0);
  aNewCol(2) = gp_Pnt(2, 1.5, 0);
  aNewCol(3) = gp_Pnt(3, 1.5, 0);

  int aNbVBefore = myOriginalSurface->NbVPoles();
  myOriginalSurface->InsertPoleColAfter(1, aNewCol);
  EXPECT_EQ(myOriginalSurface->NbVPoles(), aNbVBefore + 1);
  EXPECT_EQ(myOriginalSurface->VDegree(), 3);
}

TEST_F(Geom_BezierSurface_Test, RemovePoleRow)
{
  // Insert a row first so we have 4 rows (need at least 2 to remove)
  NCollection_Array1<gp_Pnt> aNewRow(1, 3);
  aNewRow(1) = gp_Pnt(1.5, 1, 0);
  aNewRow(2) = gp_Pnt(1.5, 2, 0);
  aNewRow(3) = gp_Pnt(1.5, 3, 0);
  myOriginalSurface->InsertPoleRowAfter(1, aNewRow);
  EXPECT_EQ(myOriginalSurface->NbUPoles(), 4);

  myOriginalSurface->RemovePoleRow(2);
  EXPECT_EQ(myOriginalSurface->NbUPoles(), 3);
}

TEST_F(Geom_BezierSurface_Test, RemovePoleCol)
{
  NCollection_Array1<gp_Pnt> aNewCol(1, 3);
  aNewCol(1) = gp_Pnt(1, 1.5, 0);
  aNewCol(2) = gp_Pnt(2, 1.5, 0);
  aNewCol(3) = gp_Pnt(3, 1.5, 0);
  myOriginalSurface->InsertPoleColAfter(1, aNewCol);
  EXPECT_EQ(myOriginalSurface->NbVPoles(), 4);

  myOriginalSurface->RemovePoleCol(2);
  EXPECT_EQ(myOriginalSurface->NbVPoles(), 3);
}

TEST_F(Geom_BezierSurface_Test, SetPoleRow)
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

TEST_F(Geom_BezierSurface_Test, SetPoleCol)
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

TEST_F(Geom_BezierSurface_Test, WeightsAccess_NonRational)
{
  const NCollection_Array2<double>* aWeights = myOriginalSurface->Weights();
  EXPECT_EQ(aWeights, nullptr);
}

TEST_F(Geom_BezierSurface_Test, PolesAccess)
{
  const NCollection_Array2<gp_Pnt>& aPoles = myOriginalSurface->Poles();
  EXPECT_EQ(aPoles.ColLength(), 3);
  EXPECT_EQ(aPoles.RowLength(), 3);
  EXPECT_TRUE(aPoles(1, 1).IsEqual(gp_Pnt(1, 1, 0.2), 1e-10));
}

TEST_F(Geom_BezierSurface_Test, MaxDegree)
{
  EXPECT_GE(Geom_BezierSurface::MaxDegree(), 25);
}

TEST_F(Geom_BezierSurface_Test, RationalSurface_UIso)
{
  NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 2);
  NCollection_Array2<double> aWeights(1, 2, 1, 2);
  aPoles(1, 1)   = gp_Pnt(0, 0, 0);
  aPoles(1, 2)   = gp_Pnt(1, 0, 0);
  aPoles(2, 1)   = gp_Pnt(0, 1, 0);
  aPoles(2, 2)   = gp_Pnt(1, 1, 0);
  aWeights(1, 1) = 1.0;
  aWeights(1, 2) = 2.0;
  aWeights(2, 1) = 2.0;
  aWeights(2, 2) = 1.0;

  occ::handle<Geom_BezierSurface> aSurf = new Geom_BezierSurface(aPoles, aWeights);

  occ::handle<Geom_Curve> anIso = aSurf->UIso(0.5);
  EXPECT_FALSE(anIso.IsNull());

  for (double v = 0.0; v <= 1.0; v += 0.25)
  {
    gp_Pnt aSurfPnt = aSurf->Value(0.5, v);
    gp_Pnt anIsoPnt = anIso->Value(v);
    EXPECT_TRUE(aSurfPnt.IsEqual(anIsoPnt, 1e-10));
  }
}

TEST_F(Geom_BezierSurface_Test, Evaluation_D3)
{
  gp_Pnt aPnt;
  gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV;
  gp_Vec aD3U, aD3V, aD3UUV, aD3UVV;
  myOriginalSurface->D3(0.5, 0.5, aPnt, aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV);
  // Degree 2: D3 should be zero
  EXPECT_NEAR(aD3U.Magnitude(), 0.0, 1e-10);
  EXPECT_NEAR(aD3V.Magnitude(), 0.0, 1e-10);
}

TEST_F(Geom_BezierSurface_Test, InsertPoleRowBefore)
{
  NCollection_Array1<gp_Pnt> aNewRow(1, 3);
  aNewRow(1) = gp_Pnt(0.5, 1, 0);
  aNewRow(2) = gp_Pnt(0.5, 2, 0);
  aNewRow(3) = gp_Pnt(0.5, 3, 0);

  int aNbUBefore = myOriginalSurface->NbUPoles();
  myOriginalSurface->InsertPoleRowBefore(2, aNewRow);
  EXPECT_EQ(myOriginalSurface->NbUPoles(), aNbUBefore + 1);
  EXPECT_TRUE(myOriginalSurface->Pole(2, 1).IsEqual(gp_Pnt(0.5, 1, 0), 1e-10));
}

TEST_F(Geom_BezierSurface_Test, InsertPoleColBefore)
{
  NCollection_Array1<gp_Pnt> aNewCol(1, 3);
  aNewCol(1) = gp_Pnt(1, 0.5, 0);
  aNewCol(2) = gp_Pnt(2, 0.5, 0);
  aNewCol(3) = gp_Pnt(3, 0.5, 0);

  int aNbVBefore = myOriginalSurface->NbVPoles();
  myOriginalSurface->InsertPoleColBefore(2, aNewCol);
  EXPECT_EQ(myOriginalSurface->NbVPoles(), aNbVBefore + 1);
  EXPECT_TRUE(myOriginalSurface->Pole(1, 2).IsEqual(gp_Pnt(1, 0.5, 0), 1e-10));
}

TEST_F(Geom_BezierSurface_Test, RationalSegment)
{
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  NCollection_Array2<double> aWeights(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
    for (int j = 1; j <= 3; ++j)
    {
      aPoles(i, j)   = gp_Pnt(i, j, (i + j) * 0.1);
      aWeights(i, j) = 1.0 + 0.3 * ((i - 1) + (j - 1));
    }

  occ::handle<Geom_BezierSurface> aSurf = new Geom_BezierSurface(aPoles, aWeights);
  EXPECT_TRUE(aSurf->IsURational() || aSurf->IsVRational());

  gp_Pnt aMid = aSurf->Value(0.5, 0.5);
  aSurf->Segment(0.25, 0.75, 0.25, 0.75);
  gp_Pnt aMidAfter = aSurf->Value(0.5, 0.5);
  EXPECT_TRUE(aMid.IsEqual(aMidAfter, 1e-5));
}

TEST_F(Geom_BezierSurface_Test, RationalIncrease)
{
  NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 2);
  NCollection_Array2<double> aWeights(1, 2, 1, 2);
  aPoles(1, 1)   = gp_Pnt(0, 0, 0);
  aPoles(1, 2)   = gp_Pnt(1, 0, 0);
  aPoles(2, 1)   = gp_Pnt(0, 1, 0);
  aPoles(2, 2)   = gp_Pnt(1, 1, 1);
  aWeights(1, 1) = 1.0;
  aWeights(1, 2) = 2.0;
  aWeights(2, 1) = 2.0;
  aWeights(2, 2) = 1.0;

  occ::handle<Geom_BezierSurface> aSurf      = new Geom_BezierSurface(aPoles, aWeights);
  gp_Pnt                          aValBefore = aSurf->Value(0.5, 0.5);

  aSurf->Increase(3, 3);
  EXPECT_EQ(aSurf->UDegree(), 3);
  EXPECT_EQ(aSurf->VDegree(), 3);
  gp_Pnt aValAfter = aSurf->Value(0.5, 0.5);
  EXPECT_TRUE(aValBefore.IsEqual(aValAfter, 1e-10));
}

TEST_F(Geom_BezierSurface_Test, SetWeightRow)
{
  NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 2);
  NCollection_Array2<double> aWeights(1, 2, 1, 2);
  aPoles(1, 1)   = gp_Pnt(0, 0, 0);
  aPoles(1, 2)   = gp_Pnt(1, 0, 0);
  aPoles(2, 1)   = gp_Pnt(0, 1, 0);
  aPoles(2, 2)   = gp_Pnt(1, 1, 0);
  aWeights(1, 1) = 1.0;
  aWeights(1, 2) = 2.0;
  aWeights(2, 1) = 3.0;
  aWeights(2, 2) = 4.0;

  occ::handle<Geom_BezierSurface> aSurf = new Geom_BezierSurface(aPoles, aWeights);

  NCollection_Array1<double> aNewWeights(1, 2);
  aNewWeights(1) = 5.0;
  aNewWeights(2) = 6.0;

  aSurf->SetWeightRow(1, aNewWeights);
  EXPECT_DOUBLE_EQ(aSurf->Weight(1, 1), 5.0);
  EXPECT_DOUBLE_EQ(aSurf->Weight(1, 2), 6.0);
}

TEST_F(Geom_BezierSurface_Test, SetWeightCol)
{
  NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 2);
  NCollection_Array2<double> aWeights(1, 2, 1, 2);
  aPoles(1, 1)   = gp_Pnt(0, 0, 0);
  aPoles(1, 2)   = gp_Pnt(1, 0, 0);
  aPoles(2, 1)   = gp_Pnt(0, 1, 0);
  aPoles(2, 2)   = gp_Pnt(1, 1, 0);
  aWeights(1, 1) = 1.0;
  aWeights(1, 2) = 2.0;
  aWeights(2, 1) = 3.0;
  aWeights(2, 2) = 4.0;

  occ::handle<Geom_BezierSurface> aSurf = new Geom_BezierSurface(aPoles, aWeights);

  NCollection_Array1<double> aNewWeights(1, 2);
  aNewWeights(1) = 7.0;
  aNewWeights(2) = 8.0;

  aSurf->SetWeightCol(1, aNewWeights);
  EXPECT_DOUBLE_EQ(aSurf->Weight(1, 1), 7.0);
  EXPECT_DOUBLE_EQ(aSurf->Weight(2, 1), 8.0);
}

TEST_F(Geom_BezierSurface_Test, InsertPoleRowAfterWithWeights)
{
  NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 2);
  NCollection_Array2<double> aWeights(1, 2, 1, 2);
  aPoles(1, 1)   = gp_Pnt(0, 0, 0);
  aPoles(1, 2)   = gp_Pnt(1, 0, 0);
  aPoles(2, 1)   = gp_Pnt(0, 1, 0);
  aPoles(2, 2)   = gp_Pnt(1, 1, 0);
  aWeights(1, 1) = 1.0;
  aWeights(1, 2) = 2.0;
  aWeights(2, 1) = 2.0;
  aWeights(2, 2) = 1.0;

  occ::handle<Geom_BezierSurface> aSurf = new Geom_BezierSurface(aPoles, aWeights);

  NCollection_Array1<gp_Pnt> aNewRow(1, 2);
  aNewRow(1) = gp_Pnt(0, 0.5, 0);
  aNewRow(2) = gp_Pnt(1, 0.5, 0);

  NCollection_Array1<double> aNewWeights(1, 2);
  aNewWeights(1) = 3.0;
  aNewWeights(2) = 4.0;

  aSurf->InsertPoleRowAfter(1, aNewRow, aNewWeights);
  EXPECT_EQ(aSurf->NbUPoles(), 3);
  EXPECT_DOUBLE_EQ(aSurf->Weight(2, 1), 3.0);
  EXPECT_DOUBLE_EQ(aSurf->Weight(2, 2), 4.0);
}

TEST_F(Geom_BezierSurface_Test, VIso_Rational)
{
  NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 2);
  NCollection_Array2<double> aWeights(1, 2, 1, 2);
  aPoles(1, 1)   = gp_Pnt(0, 0, 0);
  aPoles(1, 2)   = gp_Pnt(1, 0, 0);
  aPoles(2, 1)   = gp_Pnt(0, 1, 0);
  aPoles(2, 2)   = gp_Pnt(1, 1, 0);
  aWeights(1, 1) = 1.0;
  aWeights(1, 2) = 2.0;
  aWeights(2, 1) = 2.0;
  aWeights(2, 2) = 1.0;

  occ::handle<Geom_BezierSurface> aSurf = new Geom_BezierSurface(aPoles, aWeights);

  occ::handle<Geom_Curve> anIso = aSurf->VIso(0.5);
  EXPECT_FALSE(anIso.IsNull());

  for (double u = 0.0; u <= 1.0; u += 0.25)
  {
    gp_Pnt aSurfPnt = aSurf->Value(u, 0.5);
    gp_Pnt anIsoPnt = anIso->Value(u);
    EXPECT_TRUE(aSurfPnt.IsEqual(anIsoPnt, 1e-10));
  }
}

TEST_F(Geom_BezierSurface_Test, SetPoleWithWeight)
{
  NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 2);
  NCollection_Array2<double> aWeights(1, 2, 1, 2);
  aPoles(1, 1)   = gp_Pnt(0, 0, 0);
  aPoles(1, 2)   = gp_Pnt(1, 0, 0);
  aPoles(2, 1)   = gp_Pnt(0, 1, 0);
  aPoles(2, 2)   = gp_Pnt(1, 1, 0);
  aWeights(1, 1) = 1.0;
  aWeights(1, 2) = 1.0;
  aWeights(2, 1) = 1.0;
  aWeights(2, 2) = 2.0;

  occ::handle<Geom_BezierSurface> aSurf = new Geom_BezierSurface(aPoles, aWeights);

  gp_Pnt aNewPole(0.5, 0.5, 1.0);
  aSurf->SetPole(1, 1, aNewPole, 5.0);
  EXPECT_TRUE(aSurf->Pole(1, 1).IsEqual(aNewPole, 1e-10));
  EXPECT_DOUBLE_EQ(aSurf->Weight(1, 1), 5.0);
}

TEST_F(Geom_BezierSurface_Test, SetPoleColWithWeights)
{
  NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 2);
  NCollection_Array2<double> aWeights(1, 2, 1, 2);
  aPoles(1, 1)   = gp_Pnt(0, 0, 0);
  aPoles(1, 2)   = gp_Pnt(1, 0, 0);
  aPoles(2, 1)   = gp_Pnt(0, 1, 0);
  aPoles(2, 2)   = gp_Pnt(1, 1, 0);
  aWeights(1, 1) = 1.0;
  aWeights(1, 2) = 2.0;
  aWeights(2, 1) = 2.0;
  aWeights(2, 2) = 1.0;

  occ::handle<Geom_BezierSurface> aSurf = new Geom_BezierSurface(aPoles, aWeights);

  NCollection_Array1<gp_Pnt> aNewCol(1, 2);
  aNewCol(1) = gp_Pnt(5, 0, 0);
  aNewCol(2) = gp_Pnt(5, 1, 0);

  NCollection_Array1<double> aNewWeights(1, 2);
  aNewWeights(1) = 3.0;
  aNewWeights(2) = 4.0;

  aSurf->SetPoleCol(2, aNewCol, aNewWeights);
  EXPECT_TRUE(aSurf->Pole(1, 2).IsEqual(gp_Pnt(5, 0, 0), 1e-10));
  EXPECT_DOUBLE_EQ(aSurf->Weight(1, 2), 3.0);
  EXPECT_DOUBLE_EQ(aSurf->Weight(2, 2), 4.0);
}

TEST_F(Geom_BezierSurface_Test, WeightsArray_NonRational_ReturnsUnitWeights)
{
  ASSERT_FALSE(myOriginalSurface->IsURational());
  ASSERT_FALSE(myOriginalSurface->IsVRational());

  const NCollection_Array2<double>& aWeights = myOriginalSurface->WeightsArray();
  EXPECT_EQ(aWeights.ColLength(), myOriginalSurface->NbUPoles());
  EXPECT_EQ(aWeights.RowLength(), myOriginalSurface->NbVPoles());
  EXPECT_FALSE(aWeights.IsDeletable());
  for (int i = aWeights.LowerRow(); i <= aWeights.UpperRow(); ++i)
  {
    for (int j = aWeights.LowerCol(); j <= aWeights.UpperCol(); ++j)
    {
      EXPECT_DOUBLE_EQ(aWeights(i, j), 1.0);
    }
  }
  EXPECT_EQ(&aWeights, &myOriginalSurface->WeightsArray());
}

TEST_F(Geom_BezierSurface_Test, WeightsArray_Rational_ReturnsOwning)
{
  NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 2);
  NCollection_Array2<double> aWeightsIn(1, 2, 1, 2);
  aPoles(1, 1)     = gp_Pnt(0, 0, 0);
  aPoles(1, 2)     = gp_Pnt(1, 0, 0);
  aPoles(2, 1)     = gp_Pnt(0, 1, 0);
  aPoles(2, 2)     = gp_Pnt(1, 1, 0);
  aWeightsIn(1, 1) = 1.0;
  aWeightsIn(1, 2) = 2.0;
  aWeightsIn(2, 1) = 1.0;
  aWeightsIn(2, 2) = 1.0;

  occ::handle<Geom_BezierSurface> aRational = new Geom_BezierSurface(aPoles, aWeightsIn);

  const NCollection_Array2<double>& aWeights = aRational->WeightsArray();
  EXPECT_EQ(aWeights.Size(), 4);
  EXPECT_TRUE(aWeights.IsDeletable());
  EXPECT_DOUBLE_EQ(aWeights(1, 2), 2.0);
  EXPECT_DOUBLE_EQ(aWeights(1, 1), 1.0);
  EXPECT_EQ(&aWeights, &aRational->WeightsArray());
}

TEST_F(Geom_BezierSurface_Test, ParallelEvalD0_SameObject)
{
  const int aNbSamples = 50;
  const int aNbTotal   = aNbSamples * aNbSamples;

  // Pre-compute reference values serially.
  NCollection_Array1<gp_Pnt> aRefPnts(0, aNbTotal - 1);
  for (int i = 0; i < aNbTotal; ++i)
  {
    const double aU = static_cast<double>(i / aNbSamples) / (aNbSamples - 1);
    const double aV = static_cast<double>(i % aNbSamples) / (aNbSamples - 1);
    aRefPnts(i)     = myOriginalSurface->Value(aU, aV);
  }

  // Invalidate cache to test concurrent cache build.
  occ::handle<Geom_BezierSurface> aCopy =
    Handle(Geom_BezierSurface)::DownCast(myOriginalSurface->Copy());

  std::atomic<int> aNbErrors(0);
  OSD_Parallel::For(0, aNbTotal, [&](int theIndex) {
    const double aU   = static_cast<double>(theIndex / aNbSamples) / (aNbSamples - 1);
    const double aV   = static_cast<double>(theIndex % aNbSamples) / (aNbSamples - 1);
    gp_Pnt       aPnt = aCopy->Value(aU, aV);
    if (!aPnt.IsEqual(aRefPnts(theIndex), Precision::Confusion()))
    {
      ++aNbErrors;
    }
  });
  EXPECT_EQ(aNbErrors.load(), 0);
}

TEST_F(Geom_BezierSurface_Test, ParallelEvalD1_SameObject)
{
  const int aNbSamples = 50;
  const int aNbTotal   = aNbSamples * aNbSamples;

  // Pre-compute reference values serially.
  NCollection_Array1<gp_Pnt> aRefPnts(0, aNbTotal - 1);
  for (int i = 0; i < aNbTotal; ++i)
  {
    const double aU = static_cast<double>(i / aNbSamples) / (aNbSamples - 1);
    const double aV = static_cast<double>(i % aNbSamples) / (aNbSamples - 1);
    gp_Pnt       aPnt;
    gp_Vec       aDU, aDV;
    myOriginalSurface->D1(aU, aV, aPnt, aDU, aDV);
    aRefPnts(i) = aPnt;
  }

  occ::handle<Geom_BezierSurface> aCopy =
    Handle(Geom_BezierSurface)::DownCast(myOriginalSurface->Copy());

  std::atomic<int> aNbErrors(0);
  OSD_Parallel::For(0, aNbTotal, [&](int theIndex) {
    const double aU = static_cast<double>(theIndex / aNbSamples) / (aNbSamples - 1);
    const double aV = static_cast<double>(theIndex % aNbSamples) / (aNbSamples - 1);
    gp_Pnt       aPnt;
    gp_Vec       aDU, aDV;
    aCopy->D1(aU, aV, aPnt, aDU, aDV);
    if (!aPnt.IsEqual(aRefPnts(theIndex), Precision::Confusion()))
    {
      ++aNbErrors;
    }
  });
  EXPECT_EQ(aNbErrors.load(), 0);
}

TEST(Geom_BezierSurface_ParallelTest, RationalSurface_ParallelEval)
{
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  NCollection_Array2<double> aWeights(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      aPoles(i, j)   = gp_Pnt((i - 1) * 5.0, (j - 1) * 5.0, (i == 2 && j == 2) ? 3.0 : 0.0);
      aWeights(i, j) = (i == 2 && j == 2) ? 3.0 : 1.0;
    }
  }

  occ::handle<Geom_BezierSurface> aSurface = new Geom_BezierSurface(aPoles, aWeights);

  // Pre-compute reference values serially.
  const int                  aNbSamples = 50;
  const int                  aNbTotal   = aNbSamples * aNbSamples;
  NCollection_Array1<gp_Pnt> aRefPnts(0, aNbTotal - 1);
  for (int i = 0; i < aNbTotal; ++i)
  {
    const double aU = static_cast<double>(i / aNbSamples) / (aNbSamples - 1);
    const double aV = static_cast<double>(i % aNbSamples) / (aNbSamples - 1);
    aRefPnts(i)     = aSurface->Value(aU, aV);
  }

  // Copy to get a fresh object with no cache.
  occ::handle<Geom_BezierSurface> aCopy = Handle(Geom_BezierSurface)::DownCast(aSurface->Copy());

  std::atomic<int> aNbErrors(0);
  OSD_Parallel::For(0, aNbTotal, [&](int theIndex) {
    const double aU   = static_cast<double>(theIndex / aNbSamples) / (aNbSamples - 1);
    const double aV   = static_cast<double>(theIndex % aNbSamples) / (aNbSamples - 1);
    gp_Pnt       aPnt = aCopy->Value(aU, aV);
    if (!aPnt.IsEqual(aRefPnts(theIndex), Precision::Confusion()))
    {
      ++aNbErrors;
    }
  });
  EXPECT_EQ(aNbErrors.load(), 0);
}
