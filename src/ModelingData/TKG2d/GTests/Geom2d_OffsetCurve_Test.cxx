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

#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>

class Geom2d_OffsetCurve_Test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Create a circle as basis curve
    gp_Circ2d             aCircle(gp_Ax2d(gp_Pnt2d(0, 0), gp_Dir2d(gp_Dir2d::D::X)), 5.0);
    Handle(Geom2d_Circle) aBasisCurve = new Geom2d_Circle(aCircle);

    // Create offset curve
    Standard_Real anOffsetValue = 2.0;

    myOriginalCurve = new Geom2d_OffsetCurve(aBasisCurve, anOffsetValue);
  }

  Handle(Geom2d_OffsetCurve) myOriginalCurve;
};

TEST_F(Geom2d_OffsetCurve_Test, CopyConstructorBasicProperties)
{
  // Test copy constructor
  Handle(Geom2d_OffsetCurve) aCopiedCurve = new Geom2d_OffsetCurve(*myOriginalCurve);

  // Verify basic properties are identical
  EXPECT_DOUBLE_EQ(myOriginalCurve->Offset(), aCopiedCurve->Offset());
  EXPECT_EQ(myOriginalCurve->IsPeriodic(), aCopiedCurve->IsPeriodic());
  EXPECT_EQ(myOriginalCurve->IsClosed(), aCopiedCurve->IsClosed());
}

TEST_F(Geom2d_OffsetCurve_Test, CopyConstructorBasisCurve)
{
  Handle(Geom2d_OffsetCurve) aCopiedCurve = new Geom2d_OffsetCurve(*myOriginalCurve);

  // Verify basis curves are equivalent but independent
  Handle(Geom2d_Curve) anOrigBasis = myOriginalCurve->BasisCurve();
  Handle(Geom2d_Curve) aCopyBasis  = aCopiedCurve->BasisCurve();

  // They should be different objects
  EXPECT_NE(anOrigBasis.get(), aCopyBasis.get());

  // But functionally equivalent
  EXPECT_DOUBLE_EQ(anOrigBasis->FirstParameter(), aCopyBasis->FirstParameter());
  EXPECT_DOUBLE_EQ(anOrigBasis->LastParameter(), aCopyBasis->LastParameter());
}

TEST_F(Geom2d_OffsetCurve_Test, CopyMethodUsesOptimizedConstructor)
{
  // Test that Copy() method uses the optimized copy constructor
  Handle(Geom2d_Geometry)    aCopiedGeom  = myOriginalCurve->Copy();
  Handle(Geom2d_OffsetCurve) aCopiedCurve = Handle(Geom2d_OffsetCurve)::DownCast(aCopiedGeom);

  EXPECT_FALSE(aCopiedCurve.IsNull());

  // Verify the copy is functionally identical
  EXPECT_DOUBLE_EQ(myOriginalCurve->Offset(), aCopiedCurve->Offset());

  // Test evaluation at several points
  Standard_Real anUFirst = myOriginalCurve->FirstParameter();
  Standard_Real anULast  = myOriginalCurve->LastParameter();
  Standard_Real aStep    = (anULast - anUFirst) / 4.0;

  for (Standard_Real u = anUFirst; u <= anULast; u += aStep)
  {
    gp_Pnt2d anOrigPnt = myOriginalCurve->Value(u);
    gp_Pnt2d aCopyPnt  = aCopiedCurve->Value(u);
    EXPECT_TRUE(anOrigPnt.IsEqual(aCopyPnt, 1e-10));
  }
}

TEST_F(Geom2d_OffsetCurve_Test, CopyIndependence)
{
  Handle(Geom2d_OffsetCurve) aCopiedCurve = new Geom2d_OffsetCurve(*myOriginalCurve);

  // Store original offset value
  Standard_Real anOrigOffset = aCopiedCurve->Offset();

  // Modify the original curve
  myOriginalCurve->SetOffsetValue(10.0);

  // Verify the copied curve is not affected
  EXPECT_DOUBLE_EQ(aCopiedCurve->Offset(), anOrigOffset);
  EXPECT_NE(aCopiedCurve->Offset(), myOriginalCurve->Offset());
}