// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Precision.hxx>
#include <Select3D_SensitivePrimitiveArray.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <TopLoc_Location.hxx>

#include <gtest/gtest.h>

TEST(Select3D_SensitivePrimitiveArrayTest, GetVertex_ReturnsTriangleVertices)
{
  occ::handle<Graphic3d_ArrayOfTriangles> aTris = new Graphic3d_ArrayOfTriangles(3);
  aTris->AddVertex(0.0, 0.0, 0.0);
  aTris->AddVertex(1.0, 0.0, 0.0);
  aTris->AddVertex(0.0, 1.0, 0.0);

  occ::handle<SelectMgr_EntityOwner>            anOwner = new SelectMgr_EntityOwner();
  occ::handle<Select3D_SensitivePrimitiveArray> aSens = new Select3D_SensitivePrimitiveArray(anOwner);
  ASSERT_TRUE(aSens->InitTriangulation(aTris->Attributes(), aTris->Indices(), TopLoc_Location()));

  const std::array<NCollection_Vec3<float>, 3> aVertices = aSens->GetVertex(0);
  EXPECT_NEAR(aVertices[0].x(), 0.0f, float(Precision::Confusion()));
  EXPECT_NEAR(aVertices[0].y(), 0.0f, float(Precision::Confusion()));
  EXPECT_NEAR(aVertices[0].z(), 0.0f, float(Precision::Confusion()));
  EXPECT_NEAR(aVertices[1].x(), 1.0f, float(Precision::Confusion()));
  EXPECT_NEAR(aVertices[1].y(), 0.0f, float(Precision::Confusion()));
  EXPECT_NEAR(aVertices[1].z(), 0.0f, float(Precision::Confusion()));
  EXPECT_NEAR(aVertices[2].x(), 0.0f, float(Precision::Confusion()));
  EXPECT_NEAR(aVertices[2].y(), 1.0f, float(Precision::Confusion()));
  EXPECT_NEAR(aVertices[2].z(), 0.0f, float(Precision::Confusion()));
}
