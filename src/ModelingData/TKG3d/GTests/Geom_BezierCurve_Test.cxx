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

#include <Geom_BezierCurve.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <gp_Pnt.hxx>

class Geom_BezierCurve_Test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Create a simple Bezier curve for testing
    TColgp_Array1OfPnt aPoles(1, 4);
    aPoles(1) = gp_Pnt(0, 0, 0);
    aPoles(2) = gp_Pnt(1, 1, 0);
    aPoles(3) = gp_Pnt(2, 1, 0);
    aPoles(4) = gp_Pnt(3, 0, 0);

    myOriginalCurve = new Geom_BezierCurve(aPoles);
  }

  Handle(Geom_BezierCurve) myOriginalCurve;
};

TEST_F(Geom_BezierCurve_Test, CopyConstructorBasicProperties)
{
  // Test copy constructor
  Handle(Geom_BezierCurve) aCopiedCurve = new Geom_BezierCurve(*myOriginalCurve);

  // Verify basic properties are identical
  EXPECT_EQ(myOriginalCurve->Degree(), aCopiedCurve->Degree());
  EXPECT_EQ(myOriginalCurve->NbPoles(), aCopiedCurve->NbPoles());
  EXPECT_EQ(myOriginalCurve->IsRational(), aCopiedCurve->IsRational());
  EXPECT_EQ(myOriginalCurve->IsClosed(), aCopiedCurve->IsClosed());
}

TEST_F(Geom_BezierCurve_Test, CopyConstructorPoles)
{
  Handle(Geom_BezierCurve) aCopiedCurve = new Geom_BezierCurve(*myOriginalCurve);

  // Verify all poles are identical
  for (Standard_Integer i = 1; i <= myOriginalCurve->NbPoles(); ++i)
  {
    gp_Pnt anOrigPole = myOriginalCurve->Pole(i);
    gp_Pnt aCopyPole  = aCopiedCurve->Pole(i);
    EXPECT_TRUE(anOrigPole.IsEqual(aCopyPole, 1e-10));
  }
}

TEST_F(Geom_BezierCurve_Test, CopyMethodUsesOptimizedConstructor)
{
  // Test that Copy() method uses the optimized copy constructor
  Handle(Geom_Geometry)    aCopiedGeom  = myOriginalCurve->Copy();
  Handle(Geom_BezierCurve) aCopiedCurve = Handle(Geom_BezierCurve)::DownCast(aCopiedGeom);

  EXPECT_FALSE(aCopiedCurve.IsNull());

  // Verify the copy is functionally identical
  EXPECT_EQ(myOriginalCurve->Degree(), aCopiedCurve->Degree());
  EXPECT_EQ(myOriginalCurve->NbPoles(), aCopiedCurve->NbPoles());

  // Test evaluation at several points
  for (Standard_Real u = 0.0; u <= 1.0; u += 0.25)
  {
    gp_Pnt anOrigPnt = myOriginalCurve->Value(u);
    gp_Pnt aCopyPnt  = aCopiedCurve->Value(u);
    EXPECT_TRUE(anOrigPnt.IsEqual(aCopyPnt, 1e-10));
  }
}

TEST_F(Geom_BezierCurve_Test, RationalCurveCopyConstructor)
{
  // Create a rational Bezier curve
  TColgp_Array1OfPnt aPoles(1, 3);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 1, 0);
  aPoles(3) = gp_Pnt(2, 0, 0);

  TColStd_Array1OfReal aWeights(1, 3);
  aWeights(1) = 1.0;
  aWeights(2) = 2.0;
  aWeights(3) = 1.0;

  Handle(Geom_BezierCurve) aRationalCurve  = new Geom_BezierCurve(aPoles, aWeights);
  Handle(Geom_BezierCurve) aCopiedRational = new Geom_BezierCurve(*aRationalCurve);

  EXPECT_TRUE(aCopiedRational->IsRational());

  // Verify weights are copied correctly
  for (Standard_Integer i = 1; i <= aRationalCurve->NbPoles(); ++i)
  {
    EXPECT_DOUBLE_EQ(aRationalCurve->Weight(i), aCopiedRational->Weight(i));
  }
}

TEST_F(Geom_BezierCurve_Test, CopyIndependence)
{
  Handle(Geom_BezierCurve) aCopiedCurve = new Geom_BezierCurve(*myOriginalCurve);

  // Modify the original curve
  gp_Pnt aNewPole(10, 10, 10);
  myOriginalCurve->SetPole(2, aNewPole);

  // Verify the copied curve is not affected
  gp_Pnt anOrigPole = aCopiedCurve->Pole(2);
  EXPECT_FALSE(anOrigPole.IsEqual(aNewPole, 1e-10));
}