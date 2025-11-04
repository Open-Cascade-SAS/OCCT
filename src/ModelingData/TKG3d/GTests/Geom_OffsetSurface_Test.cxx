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

#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pln.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>

class Geom_OffsetSurface_Test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Create a plane as basis surface
    gp_Pln             aPlane(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(gp_Dir::D::Z)));
    Handle(Geom_Plane) aBasisSurface = new Geom_Plane(aPlane);

    // Create offset surface
    Standard_Real anOffsetValue = 3.0;

    myOriginalSurface = new Geom_OffsetSurface(aBasisSurface, anOffsetValue);
  }

  Handle(Geom_OffsetSurface) myOriginalSurface;
};

TEST_F(Geom_OffsetSurface_Test, CopyConstructorBasicProperties)
{
  // Test copy constructor
  Handle(Geom_OffsetSurface) aCopiedSurface = new Geom_OffsetSurface(*myOriginalSurface);

  // Verify basic properties are identical
  EXPECT_DOUBLE_EQ(myOriginalSurface->Offset(), aCopiedSurface->Offset());
  EXPECT_EQ(myOriginalSurface->IsUPeriodic(), aCopiedSurface->IsUPeriodic());
  EXPECT_EQ(myOriginalSurface->IsVPeriodic(), aCopiedSurface->IsVPeriodic());
  EXPECT_EQ(myOriginalSurface->IsUClosed(), aCopiedSurface->IsUClosed());
  EXPECT_EQ(myOriginalSurface->IsVClosed(), aCopiedSurface->IsVClosed());
}

TEST_F(Geom_OffsetSurface_Test, CopyConstructorBasisSurface)
{
  Handle(Geom_OffsetSurface) aCopiedSurface = new Geom_OffsetSurface(*myOriginalSurface);

  // Verify basis surfaces are equivalent but independent
  Handle(Geom_Surface) anOrigBasis = myOriginalSurface->BasisSurface();
  Handle(Geom_Surface) aCopyBasis  = aCopiedSurface->BasisSurface();

  // They should be different objects
  EXPECT_NE(anOrigBasis.get(), aCopyBasis.get());

  // But functionally equivalent
  Standard_Real anUFirst, anULast, aVFirst, aVLast;
  anOrigBasis->Bounds(anUFirst, anULast, aVFirst, aVLast);

  Standard_Real anUFirstCopy, anULastCopy, aVFirstCopy, aVLastCopy;
  aCopyBasis->Bounds(anUFirstCopy, anULastCopy, aVFirstCopy, aVLastCopy);

  EXPECT_DOUBLE_EQ(anUFirst, anUFirstCopy);
  EXPECT_DOUBLE_EQ(anULast, anULastCopy);
  EXPECT_DOUBLE_EQ(aVFirst, aVFirstCopy);
  EXPECT_DOUBLE_EQ(aVLast, aVLastCopy);
}

TEST_F(Geom_OffsetSurface_Test, CopyMethodUsesOptimizedConstructor)
{
  // Test that Copy() method uses the optimized copy constructor
  Handle(Geom_Geometry)      aCopiedGeom    = myOriginalSurface->Copy();
  Handle(Geom_OffsetSurface) aCopiedSurface = Handle(Geom_OffsetSurface)::DownCast(aCopiedGeom);

  EXPECT_FALSE(aCopiedSurface.IsNull());

  // Verify the copy is functionally identical
  EXPECT_DOUBLE_EQ(myOriginalSurface->Offset(), aCopiedSurface->Offset());

  // Test evaluation at several points
  for (Standard_Real u = -5.0; u <= 5.0; u += 2.5)
  {
    for (Standard_Real v = -5.0; v <= 5.0; v += 2.5)
    {
      gp_Pnt anOrigPnt = myOriginalSurface->Value(u, v);
      gp_Pnt aCopyPnt  = aCopiedSurface->Value(u, v);
      EXPECT_TRUE(anOrigPnt.IsEqual(aCopyPnt, 1e-10));
    }
  }
}

TEST_F(Geom_OffsetSurface_Test, CopyIndependence)
{
  Handle(Geom_OffsetSurface) aCopiedSurface = new Geom_OffsetSurface(*myOriginalSurface);

  // Store original offset value
  Standard_Real anOrigOffset = aCopiedSurface->Offset();

  // Modify the original surface
  myOriginalSurface->SetOffsetValue(15.0);

  // Verify the copied surface is not affected
  EXPECT_DOUBLE_EQ(aCopiedSurface->Offset(), anOrigOffset);
  EXPECT_NE(aCopiedSurface->Offset(), myOriginalSurface->Offset());
}