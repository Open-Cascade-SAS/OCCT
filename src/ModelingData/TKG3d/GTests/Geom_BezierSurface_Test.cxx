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

#include <Geom_BezierSurface.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

class Geom_BezierSurface_Test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Create a simple Bezier surface for testing
    TColgp_Array2OfPnt aPoles(1, 3, 1, 3);
    for (Standard_Integer i = 1; i <= 3; ++i)
    {
      for (Standard_Integer j = 1; j <= 3; ++j)
      {
        aPoles(i, j) = gp_Pnt(i, j, (i + j) * 0.1);
      }
    }

    myOriginalSurface = new Geom_BezierSurface(aPoles);
  }

  Handle(Geom_BezierSurface) myOriginalSurface;
};

TEST_F(Geom_BezierSurface_Test, CopyConstructorBasicProperties)
{
  // Test copy constructor
  Handle(Geom_BezierSurface) aCopiedSurface = new Geom_BezierSurface(*myOriginalSurface);

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
  Handle(Geom_BezierSurface) aCopiedSurface = new Geom_BezierSurface(*myOriginalSurface);

  // Verify all poles are identical
  for (Standard_Integer i = 1; i <= myOriginalSurface->NbUPoles(); ++i)
  {
    for (Standard_Integer j = 1; j <= myOriginalSurface->NbVPoles(); ++j)
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
  Handle(Geom_Geometry)      aCopiedGeom    = myOriginalSurface->Copy();
  Handle(Geom_BezierSurface) aCopiedSurface = Handle(Geom_BezierSurface)::DownCast(aCopiedGeom);

  EXPECT_FALSE(aCopiedSurface.IsNull());

  // Verify the copy is functionally identical
  EXPECT_EQ(myOriginalSurface->UDegree(), aCopiedSurface->UDegree());
  EXPECT_EQ(myOriginalSurface->VDegree(), aCopiedSurface->VDegree());

  // Test evaluation at several points
  for (Standard_Real u = 0.0; u <= 1.0; u += 0.5)
  {
    for (Standard_Real v = 0.0; v <= 1.0; v += 0.5)
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
  TColgp_Array2OfPnt aPoles(1, 2, 1, 2);
  aPoles(1, 1) = gp_Pnt(0, 0, 0);
  aPoles(1, 2) = gp_Pnt(1, 0, 0);
  aPoles(2, 1) = gp_Pnt(0, 1, 0);
  aPoles(2, 2) = gp_Pnt(1, 1, 0);

  TColStd_Array2OfReal aWeights(1, 2, 1, 2);
  aWeights(1, 1) = 1.0;
  aWeights(1, 2) = 2.0;
  aWeights(2, 1) = 2.0;
  aWeights(2, 2) = 1.0;

  Handle(Geom_BezierSurface) aRationalSurface = new Geom_BezierSurface(aPoles, aWeights);
  Handle(Geom_BezierSurface) aCopiedRational  = new Geom_BezierSurface(*aRationalSurface);

  EXPECT_TRUE(aCopiedRational->IsURational() || aCopiedRational->IsVRational());

  // Verify weights are copied correctly
  for (Standard_Integer i = 1; i <= aRationalSurface->NbUPoles(); ++i)
  {
    for (Standard_Integer j = 1; j <= aRationalSurface->NbVPoles(); ++j)
    {
      EXPECT_DOUBLE_EQ(aRationalSurface->Weight(i, j), aCopiedRational->Weight(i, j));
    }
  }
}

TEST_F(Geom_BezierSurface_Test, CopyIndependence)
{
  Handle(Geom_BezierSurface) aCopiedSurface = new Geom_BezierSurface(*myOriginalSurface);

  // Modify the original surface
  gp_Pnt aNewPole(10, 10, 10);
  myOriginalSurface->SetPole(2, 2, aNewPole);

  // Verify the copied surface is not affected
  gp_Pnt anOrigPole = aCopiedSurface->Pole(2, 2);
  EXPECT_FALSE(anOrigPole.IsEqual(aNewPole, 1e-10));
}