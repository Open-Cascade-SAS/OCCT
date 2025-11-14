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

#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

#include <gtest/gtest.h>

TEST(BRepPrimAPI_MakePrism_Test, OCC31294_GeneratedListForNonBaseShape)
{
  // Bug OCC31294: Modeling Algorithms - Regression relatively 7.3.0.
  // Crash in method BRepPrimAPI_MakePrism::Generated(...) if input sub-shape
  // does not belong to the base shape
  //
  // This test verifies that calling Generated() with a shape that doesn't belong
  // to the base shape doesn't crash and returns an empty list

  BRepBuilderAPI_MakeVertex aMkVert(gp_Pnt(0., 0., 0.));
  BRepBuilderAPI_MakeVertex aMkDummy(gp_Pnt(0., 0., 0.));
  BRepPrimAPI_MakePrism     aMkPrism(aMkVert.Shape(), gp_Vec(0., 0., 1.));

  // Check that Generated() returns 1 shape for the vertex used to create the prism
  Standard_Integer aNbGen = aMkPrism.Generated(aMkVert.Shape()).Extent();
  EXPECT_EQ(aNbGen, 1);

  // Check that Generated() returns 0 shapes for a vertex not used to create the prism
  // (this should not crash)
  Standard_Integer aNbDummy = aMkPrism.Generated(aMkDummy.Shape()).Extent();
  EXPECT_EQ(aNbDummy, 0);
}
