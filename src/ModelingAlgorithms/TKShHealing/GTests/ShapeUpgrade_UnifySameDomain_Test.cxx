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

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <ShapeUpgrade_UnifySameDomain.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Wire.hxx>

//==================================================================================================
// ShapeUpgrade_UnifySameDomain Tests
//==================================================================================================

// Test for issue #925: Infinite loop when processing internal edges.
// This test verifies that UnifySameDomain properly handles internal edges
// without entering an infinite loop.
TEST(ShapeUpgrade_UnifySameDomainTest, InternalEdgesTermination_Issue925)
{
  // Create a planar face with an internal wire (internal edges).
  // This configuration previously caused an infinite loop in IntUnifyFaces
  // because edges removed from InternalEdges were not removed from IntVEmap.

  BRep_Builder aBuilder;

  // Create a rectangular outer wire
  gp_Pnt aP1(0, 0, 0);
  gp_Pnt aP2(10, 0, 0);
  gp_Pnt aP3(10, 10, 0);
  gp_Pnt aP4(0, 10, 0);

  BRepBuilderAPI_MakeEdge aME1(aP1, aP2);
  BRepBuilderAPI_MakeEdge aME2(aP2, aP3);
  BRepBuilderAPI_MakeEdge aME3(aP3, aP4);
  BRepBuilderAPI_MakeEdge aME4(aP4, aP1);

  BRepBuilderAPI_MakeWire aMW;
  aMW.Add(aME1.Edge());
  aMW.Add(aME2.Edge());
  aMW.Add(aME3.Edge());
  aMW.Add(aME4.Edge());

  ASSERT_TRUE(aMW.IsDone()) << "Failed to create outer wire";
  TopoDS_Wire anOuterWire = aMW.Wire();

  // Create a face from the outer wire
  gp_Pln                  aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  BRepBuilderAPI_MakeFace aMF(aPlane, anOuterWire);
  ASSERT_TRUE(aMF.IsDone()) << "Failed to create face";

  TopoDS_Face aFace = aMF.Face();

  // Create internal edges forming an open path inside the face.
  // These edges share a common vertex (aIP2) which is key to triggering
  // the infinite loop bug in IntVEmap processing.
  gp_Pnt aIP1(2, 2, 0);
  gp_Pnt aIP2(5, 5, 0);
  gp_Pnt aIP3(8, 2, 0);

  BRepBuilderAPI_MakeEdge aMIE1(aIP1, aIP2);
  BRepBuilderAPI_MakeEdge aMIE2(aIP2, aIP3);

  ASSERT_TRUE(aMIE1.IsDone()) << "Failed to create internal edge 1";
  ASSERT_TRUE(aMIE2.IsDone()) << "Failed to create internal edge 2";

  // Create an internal wire from the edges
  BRepBuilderAPI_MakeWire aMIW;
  aMIW.Add(aMIE1.Edge());
  aMIW.Add(aMIE2.Edge());
  ASSERT_TRUE(aMIW.IsDone()) << "Failed to create internal wire";

  TopoDS_Wire anInternalWire = aMIW.Wire();

  // Set wire to INTERNAL orientation and add to face
  anInternalWire.Orientation(TopAbs_INTERNAL);
  aBuilder.Add(aFace, anInternalWire);

  // Create a shell with this face
  TopoDS_Shell aShell;
  aBuilder.MakeShell(aShell);
  aBuilder.Add(aShell, aFace);

  // Run UnifySameDomain - before the fix, this would enter an infinite loop
  // because internal edges removed from InternalEdges were not removed from IntVEmap.
  ShapeUpgrade_UnifySameDomain aUnifier(aShell);
  aUnifier.SetAngularTolerance(1e-6);
  aUnifier.SetLinearTolerance(1e-5);
  aUnifier.AllowInternalEdges(true);

  // This should complete without hanging
  aUnifier.Build();

  // Verify the result
  const TopoDS_Shape& aResult = aUnifier.Shape();
  EXPECT_FALSE(aResult.IsNull()) << "UnifySameDomain result should not be null";

  // Count faces in result - should have at least one face
  int aFaceCount = 0;
  for (TopExp_Explorer anExp(aResult, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    aFaceCount++;
  }
  EXPECT_GE(aFaceCount, 1) << "Result should contain at least one face";
}

// Test UnifySameDomain with multiple coplanar faces having internal wires.
// This creates a more complex scenario where multiple faces need to be merged.
TEST(ShapeUpgrade_UnifySameDomainTest, MultipleCoplanarFacesWithInternalWires)
{
  BRep_Builder aBuilder;

  // Create two adjacent coplanar rectangular faces
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));

  // First face: 0-5 x 0-10
  gp_Pnt aP1(0, 0, 0);
  gp_Pnt aP2(5, 0, 0);
  gp_Pnt aP3(5, 10, 0);
  gp_Pnt aP4(0, 10, 0);

  BRepBuilderAPI_MakeWire aMW1;
  aMW1.Add(BRepBuilderAPI_MakeEdge(aP1, aP2).Edge());
  aMW1.Add(BRepBuilderAPI_MakeEdge(aP2, aP3).Edge());
  aMW1.Add(BRepBuilderAPI_MakeEdge(aP3, aP4).Edge());
  aMW1.Add(BRepBuilderAPI_MakeEdge(aP4, aP1).Edge());

  ASSERT_TRUE(aMW1.IsDone()) << "Failed to create wire 1";

  BRepBuilderAPI_MakeFace aMF1(aPlane, aMW1.Wire());
  ASSERT_TRUE(aMF1.IsDone()) << "Failed to create face 1";
  TopoDS_Face aFace1 = aMF1.Face();

  // Second face: 5-10 x 0-10
  gp_Pnt aP5(5, 0, 0);
  gp_Pnt aP6(10, 0, 0);
  gp_Pnt aP7(10, 10, 0);
  gp_Pnt aP8(5, 10, 0);

  BRepBuilderAPI_MakeWire aMW2;
  aMW2.Add(BRepBuilderAPI_MakeEdge(aP5, aP6).Edge());
  aMW2.Add(BRepBuilderAPI_MakeEdge(aP6, aP7).Edge());
  aMW2.Add(BRepBuilderAPI_MakeEdge(aP7, aP8).Edge());
  aMW2.Add(BRepBuilderAPI_MakeEdge(aP8, aP5).Edge());

  ASSERT_TRUE(aMW2.IsDone()) << "Failed to create wire 2";

  BRepBuilderAPI_MakeFace aMF2(aPlane, aMW2.Wire());
  ASSERT_TRUE(aMF2.IsDone()) << "Failed to create face 2";
  TopoDS_Face aFace2 = aMF2.Face();

  // Add internal wire to first face
  gp_Pnt                  aIP1(1, 5, 0);
  gp_Pnt                  aIP2(4, 5, 0);
  BRepBuilderAPI_MakeWire aMIW1;
  aMIW1.Add(BRepBuilderAPI_MakeEdge(aIP1, aIP2).Edge());
  ASSERT_TRUE(aMIW1.IsDone()) << "Failed to create internal wire 1";
  TopoDS_Wire anInternalWire1 = aMIW1.Wire();
  anInternalWire1.Orientation(TopAbs_INTERNAL);
  aBuilder.Add(aFace1, anInternalWire1);

  // Add internal wire to second face
  gp_Pnt                  aIP3(6, 5, 0);
  gp_Pnt                  aIP4(9, 5, 0);
  BRepBuilderAPI_MakeWire aMIW2;
  aMIW2.Add(BRepBuilderAPI_MakeEdge(aIP3, aIP4).Edge());
  ASSERT_TRUE(aMIW2.IsDone()) << "Failed to create internal wire 2";
  TopoDS_Wire anInternalWire2 = aMIW2.Wire();
  anInternalWire2.Orientation(TopAbs_INTERNAL);
  aBuilder.Add(aFace2, anInternalWire2);

  // Sew the faces together to share common edge
  BRepBuilderAPI_Sewing aSewer(1e-6);
  aSewer.Add(aFace1);
  aSewer.Add(aFace2);
  aSewer.Perform();

  TopoDS_Shape aSewedShape = aSewer.SewedShape();
  ASSERT_FALSE(aSewedShape.IsNull()) << "Sewing failed";

  // Run UnifySameDomain
  ShapeUpgrade_UnifySameDomain aUnifier(aSewedShape);
  aUnifier.SetAngularTolerance(1e-6);
  aUnifier.SetLinearTolerance(1e-5);
  aUnifier.AllowInternalEdges(true);

  aUnifier.Build();

  const TopoDS_Shape& aResult = aUnifier.Shape();
  EXPECT_FALSE(aResult.IsNull()) << "UnifySameDomain result should not be null";

  // After unification, we should have fewer faces (ideally 1 merged face)
  int aFaceCount = 0;
  for (TopExp_Explorer anExp(aResult, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    aFaceCount++;
  }

  // The two coplanar faces should be merged into one
  EXPECT_EQ(aFaceCount, 1) << "Coplanar faces should be merged into one";
}

// Test that UnifySameDomain handles closed internal wires correctly.
TEST(ShapeUpgrade_UnifySameDomainTest, ClosedInternalWire)
{
  BRep_Builder aBuilder;

  // Create a large rectangular face
  gp_Pnt aP1(0, 0, 0);
  gp_Pnt aP2(20, 0, 0);
  gp_Pnt aP3(20, 20, 0);
  gp_Pnt aP4(0, 20, 0);

  BRepBuilderAPI_MakeWire aMW;
  aMW.Add(BRepBuilderAPI_MakeEdge(aP1, aP2).Edge());
  aMW.Add(BRepBuilderAPI_MakeEdge(aP2, aP3).Edge());
  aMW.Add(BRepBuilderAPI_MakeEdge(aP3, aP4).Edge());
  aMW.Add(BRepBuilderAPI_MakeEdge(aP4, aP1).Edge());

  ASSERT_TRUE(aMW.IsDone()) << "Failed to create outer wire";

  gp_Pln                  aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  BRepBuilderAPI_MakeFace aMF(aPlane, aMW.Wire());
  ASSERT_TRUE(aMF.IsDone()) << "Failed to create face";
  TopoDS_Face aFace = aMF.Face();

  // Create a closed internal wire (triangle) inside the face
  gp_Pnt aIP1(5, 5, 0);
  gp_Pnt aIP2(15, 5, 0);
  gp_Pnt aIP3(10, 15, 0);

  BRepBuilderAPI_MakeWire aMIW;
  aMIW.Add(BRepBuilderAPI_MakeEdge(aIP1, aIP2).Edge());
  aMIW.Add(BRepBuilderAPI_MakeEdge(aIP2, aIP3).Edge());
  aMIW.Add(BRepBuilderAPI_MakeEdge(aIP3, aIP1).Edge());

  ASSERT_TRUE(aMIW.IsDone()) << "Failed to create internal wire";

  TopoDS_Wire anInternalWire = aMIW.Wire();
  anInternalWire.Orientation(TopAbs_INTERNAL);
  aBuilder.Add(aFace, anInternalWire);

  // Create shell
  TopoDS_Shell aShell;
  aBuilder.MakeShell(aShell);
  aBuilder.Add(aShell, aFace);

  // Run UnifySameDomain
  ShapeUpgrade_UnifySameDomain aUnifier(aShell);
  aUnifier.SetAngularTolerance(1e-6);
  aUnifier.SetLinearTolerance(1e-5);
  aUnifier.AllowInternalEdges(true);

  aUnifier.Build();

  const TopoDS_Shape& aResult = aUnifier.Shape();
  EXPECT_FALSE(aResult.IsNull()) << "UnifySameDomain result should not be null";
}

// Test basic UnifySameDomain functionality on a box (no internal edges).
// This serves as a baseline test to ensure the algorithm works correctly.
TEST(ShapeUpgrade_UnifySameDomainTest, BasicBoxUnification)
{
  // Create a simple box using two corner points
  BRepPrimAPI_MakeBox aBoxMaker(gp_Pnt(0, 0, 0), gp_Pnt(10, 10, 10));
  aBoxMaker.Build();
  ASSERT_TRUE(aBoxMaker.IsDone()) << "Failed to create box";

  TopoDS_Shape aBox = aBoxMaker.Shape();

  // Run UnifySameDomain
  ShapeUpgrade_UnifySameDomain aUnifier(aBox);
  aUnifier.Build();

  const TopoDS_Shape& aResult = aUnifier.Shape();
  EXPECT_FALSE(aResult.IsNull()) << "UnifySameDomain result should not be null";

  // A box should still have 6 faces after unification (no coplanar faces)
  int aFaceCount = 0;
  for (TopExp_Explorer anExp(aResult, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    aFaceCount++;
  }
  EXPECT_EQ(aFaceCount, 6) << "Box should have 6 faces";
}
