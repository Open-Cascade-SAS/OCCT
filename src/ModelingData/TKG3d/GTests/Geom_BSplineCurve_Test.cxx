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
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

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