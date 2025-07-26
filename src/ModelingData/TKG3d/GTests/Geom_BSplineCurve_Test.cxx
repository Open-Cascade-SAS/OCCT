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

#include <Geom_BSplineCurve.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

class Geom_BSplineCurve_Test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Create a simple BSpline curve for testing
    TColgp_Array1OfPnt aPoles(1, 4);
    aPoles(1) = gp_Pnt(0, 0, 0);
    aPoles(2) = gp_Pnt(1, 1, 0);
    aPoles(3) = gp_Pnt(2, 1, 0);
    aPoles(4) = gp_Pnt(3, 0, 0);

    TColStd_Array1OfReal aKnots(1, 2);
    aKnots(1) = 0.0;
    aKnots(2) = 1.0;

    TColStd_Array1OfInteger aMults(1, 2);
    aMults(1) = 4;
    aMults(2) = 4;

    myOriginalCurve = new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);
  }

  Handle(Geom_BSplineCurve) myOriginalCurve;
};

TEST_F(Geom_BSplineCurve_Test, CopyConstructorBasicProperties)
{
  // Test copy constructor
  Handle(Geom_BSplineCurve) aCopiedCurve = new Geom_BSplineCurve(*myOriginalCurve);

  // Verify basic properties are identical
  EXPECT_EQ(myOriginalCurve->Degree(), aCopiedCurve->Degree());
  EXPECT_EQ(myOriginalCurve->NbPoles(), aCopiedCurve->NbPoles());
  EXPECT_EQ(myOriginalCurve->NbKnots(), aCopiedCurve->NbKnots());
  EXPECT_EQ(myOriginalCurve->IsPeriodic(), aCopiedCurve->IsPeriodic());
  EXPECT_EQ(myOriginalCurve->IsRational(), aCopiedCurve->IsRational());
}

TEST_F(Geom_BSplineCurve_Test, CopyConstructorPoles)
{
  Handle(Geom_BSplineCurve) aCopiedCurve = new Geom_BSplineCurve(*myOriginalCurve);

  // Verify all poles are identical
  for (Standard_Integer i = 1; i <= myOriginalCurve->NbPoles(); ++i)
  {
    gp_Pnt anOrigPole = myOriginalCurve->Pole(i);
    gp_Pnt aCopyPole  = aCopiedCurve->Pole(i);
    EXPECT_TRUE(anOrigPole.IsEqual(aCopyPole, 1e-10));
  }
}

TEST_F(Geom_BSplineCurve_Test, CopyConstructorKnots)
{
  Handle(Geom_BSplineCurve) aCopiedCurve = new Geom_BSplineCurve(*myOriginalCurve);

  // Verify all knots are identical
  for (Standard_Integer i = 1; i <= myOriginalCurve->NbKnots(); ++i)
  {
    EXPECT_DOUBLE_EQ(myOriginalCurve->Knot(i), aCopiedCurve->Knot(i));
    EXPECT_EQ(myOriginalCurve->Multiplicity(i), aCopiedCurve->Multiplicity(i));
  }
}

TEST_F(Geom_BSplineCurve_Test, CopyMethodUsesOptimizedConstructor)
{
  // Test that Copy() method uses the optimized copy constructor
  Handle(Geom_Geometry)     aCopiedGeom  = myOriginalCurve->Copy();
  Handle(Geom_BSplineCurve) aCopiedCurve = Handle(Geom_BSplineCurve)::DownCast(aCopiedGeom);

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

TEST_F(Geom_BSplineCurve_Test, RationalCurveCopyConstructor)
{
  // Create a rational BSpline curve
  TColgp_Array1OfPnt aPoles(1, 3);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 1, 0);
  aPoles(3) = gp_Pnt(2, 0, 0);

  TColStd_Array1OfReal aWeights(1, 3);
  aWeights(1) = 1.0;
  aWeights(2) = 2.0;
  aWeights(3) = 1.0;

  TColStd_Array1OfReal aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  TColStd_Array1OfInteger aMults(1, 2);
  aMults(1) = 3;
  aMults(2) = 3;

  Handle(Geom_BSplineCurve) aRationalCurve =
    new Geom_BSplineCurve(aPoles, aWeights, aKnots, aMults, 2);
  Handle(Geom_BSplineCurve) aCopiedRational = new Geom_BSplineCurve(*aRationalCurve);

  EXPECT_TRUE(aCopiedRational->IsRational());

  // Verify weights are copied correctly
  for (Standard_Integer i = 1; i <= aRationalCurve->NbPoles(); ++i)
  {
    EXPECT_DOUBLE_EQ(aRationalCurve->Weight(i), aCopiedRational->Weight(i));
  }
}