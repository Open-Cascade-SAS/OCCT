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

#include <Geom_OffsetCurve.hxx>
#include <Geom_Circle.hxx>
#include <gp_Circ.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

class Geom_OffsetCurve_Test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Create a circle as basis curve
    gp_Circ             aCircle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(gp_Dir::D::Z)), 5.0);
    Handle(Geom_Circle) aBasisCurve = new Geom_Circle(aCircle);

    // Create offset curve
    gp_Dir        anOffsetDir(gp_Dir::D::Z);
    Standard_Real anOffsetValue = 2.0;

    myOriginalCurve = new Geom_OffsetCurve(aBasisCurve, anOffsetValue, anOffsetDir);
  }

  Handle(Geom_OffsetCurve) myOriginalCurve;
};

TEST_F(Geom_OffsetCurve_Test, CopyConstructorBasicProperties)
{
  // Test copy constructor
  Handle(Geom_OffsetCurve) aCopiedCurve = new Geom_OffsetCurve(*myOriginalCurve);

  // Verify basic properties are identical
  EXPECT_DOUBLE_EQ(myOriginalCurve->Offset(), aCopiedCurve->Offset());
  EXPECT_TRUE(myOriginalCurve->Direction().IsEqual(aCopiedCurve->Direction(), 1e-10));
  EXPECT_EQ(myOriginalCurve->IsPeriodic(), aCopiedCurve->IsPeriodic());
  EXPECT_EQ(myOriginalCurve->IsClosed(), aCopiedCurve->IsClosed());
}

TEST_F(Geom_OffsetCurve_Test, CopyConstructorBasisCurve)
{
  Handle(Geom_OffsetCurve) aCopiedCurve = new Geom_OffsetCurve(*myOriginalCurve);

  // Verify basis curves are equivalent but independent
  Handle(Geom_Curve) anOrigBasis = myOriginalCurve->BasisCurve();
  Handle(Geom_Curve) aCopyBasis  = aCopiedCurve->BasisCurve();

  // They should be different objects
  EXPECT_NE(anOrigBasis.get(), aCopyBasis.get());

  // But functionally equivalent
  EXPECT_DOUBLE_EQ(anOrigBasis->FirstParameter(), aCopyBasis->FirstParameter());
  EXPECT_DOUBLE_EQ(anOrigBasis->LastParameter(), aCopyBasis->LastParameter());
}

TEST_F(Geom_OffsetCurve_Test, CopyMethodUsesOptimizedConstructor)
{
  // Test that Copy() method uses the optimized copy constructor
  Handle(Geom_Geometry)    aCopiedGeom  = myOriginalCurve->Copy();
  Handle(Geom_OffsetCurve) aCopiedCurve = Handle(Geom_OffsetCurve)::DownCast(aCopiedGeom);

  EXPECT_FALSE(aCopiedCurve.IsNull());

  // Verify the copy is functionally identical
  EXPECT_DOUBLE_EQ(myOriginalCurve->Offset(), aCopiedCurve->Offset());

  // Test evaluation at several points
  Standard_Real anUFirst = myOriginalCurve->FirstParameter();
  Standard_Real anULast  = myOriginalCurve->LastParameter();
  Standard_Real aStep    = (anULast - anUFirst) / 4.0;

  for (Standard_Real u = anUFirst; u <= anULast; u += aStep)
  {
    gp_Pnt anOrigPnt = myOriginalCurve->Value(u);
    gp_Pnt aCopyPnt  = aCopiedCurve->Value(u);
    EXPECT_TRUE(anOrigPnt.IsEqual(aCopyPnt, 1e-10));
  }
}

TEST_F(Geom_OffsetCurve_Test, CopyIndependence)
{
  Handle(Geom_OffsetCurve) aCopiedCurve = new Geom_OffsetCurve(*myOriginalCurve);

  // Store original offset value
  Standard_Real anOrigOffset = aCopiedCurve->Offset();

  // Modify the original curve
  myOriginalCurve->SetOffsetValue(10.0);

  // Verify the copied curve is not affected
  EXPECT_DOUBLE_EQ(aCopiedCurve->Offset(), anOrigOffset);
  EXPECT_NE(aCopiedCurve->Offset(), myOriginalCurve->Offset());
}