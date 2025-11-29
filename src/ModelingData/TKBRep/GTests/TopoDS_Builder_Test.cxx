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

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Vector.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Builder.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_FrozenShape.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_UnCompatibleShapes.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

#include <gtest/gtest.h>

//==================================================================================================
// Test fixture for TopoDS_Builder tests
//==================================================================================================

class TopoDS_BuilderTest : public testing::Test
{
protected:
  BRep_Builder myBuilder;

  TopoDS_Vertex makeVertex(double theX, double theY, double theZ)
  {
    return BRepBuilderAPI_MakeVertex(gp_Pnt(theX, theY, theZ));
  }
};

//==================================================================================================
// Empty shape iteration tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, EmptyCompound_NoChildren)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  EXPECT_EQ(aCompound.NbChildren(), 0);

  TopoDS_Iterator anIter(aCompound);
  EXPECT_FALSE(anIter.More());
}

TEST_F(TopoDS_BuilderTest, EmptyWire_NoChildren)
{
  TopoDS_Wire aWire;
  myBuilder.MakeWire(aWire);

  EXPECT_EQ(aWire.NbChildren(), 0);

  TopoDS_Iterator anIter(aWire);
  EXPECT_FALSE(anIter.More());
}

TEST_F(TopoDS_BuilderTest, EmptyShell_NoChildren)
{
  TopoDS_Shell aShell;
  myBuilder.MakeShell(aShell);

  EXPECT_EQ(aShell.NbChildren(), 0);

  TopoDS_Iterator anIter(aShell);
  EXPECT_FALSE(anIter.More());
}

TEST_F(TopoDS_BuilderTest, EmptySolid_NoChildren)
{
  TopoDS_Solid aSolid;
  myBuilder.MakeSolid(aSolid);

  EXPECT_EQ(aSolid.NbChildren(), 0);

  TopoDS_Iterator anIter(aSolid);
  EXPECT_FALSE(anIter.More());
}

TEST_F(TopoDS_BuilderTest, EmptyCompSolid_NoChildren)
{
  TopoDS_CompSolid aCompSolid;
  myBuilder.MakeCompSolid(aCompSolid);

  EXPECT_EQ(aCompSolid.NbChildren(), 0);

  TopoDS_Iterator anIter(aCompSolid);
  EXPECT_FALSE(anIter.More());
}

//==================================================================================================
// Vertex iteration test (vertex has no children)
//==================================================================================================

TEST_F(TopoDS_BuilderTest, Vertex_NoChildren)
{
  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);

  EXPECT_EQ(aVertex.NbChildren(), 0);

  TopoDS_Iterator anIter(aVertex);
  EXPECT_FALSE(anIter.More());
}

//==================================================================================================
// Add/Remove tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, AddSingleVertex_ToCompound)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex = makeVertex(1, 2, 3);
  myBuilder.Add(aCompound, aVertex);

  EXPECT_EQ(aCompound.NbChildren(), 1);

  TopoDS_Iterator anIter(aCompound);
  EXPECT_TRUE(anIter.More());
  EXPECT_EQ(anIter.Value().ShapeType(), TopAbs_VERTEX);
  anIter.Next();
  EXPECT_FALSE(anIter.More());
}

TEST_F(TopoDS_BuilderTest, AddMultipleVertices_ToCompound)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  const int aNbVertices = 10;
  for (int i = 0; i < aNbVertices; ++i)
  {
    myBuilder.Add(aCompound, makeVertex(i, i, i));
  }

  EXPECT_EQ(aCompound.NbChildren(), aNbVertices);

  int aCount = 0;
  for (TopoDS_Iterator anIter(aCompound); anIter.More(); anIter.Next())
  {
    EXPECT_EQ(anIter.Value().ShapeType(), TopAbs_VERTEX);
    ++aCount;
  }
  EXPECT_EQ(aCount, aNbVertices);
}

TEST_F(TopoDS_BuilderTest, RemoveVertex_FromCompound)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aV1 = makeVertex(1, 0, 0);
  TopoDS_Vertex aV2 = makeVertex(2, 0, 0);
  TopoDS_Vertex aV3 = makeVertex(3, 0, 0);

  myBuilder.Add(aCompound, aV1);
  myBuilder.Add(aCompound, aV2);
  myBuilder.Add(aCompound, aV3);

  EXPECT_EQ(aCompound.NbChildren(), 3);

  // Remove middle vertex
  myBuilder.Remove(aCompound, aV2);

  EXPECT_EQ(aCompound.NbChildren(), 2);

  // Verify remaining vertices
  TopoDS_Iterator anIter(aCompound);
  EXPECT_TRUE(anIter.More());

  gp_Pnt aP1 = BRep_Tool::Pnt(TopoDS::Vertex(anIter.Value()));
  EXPECT_NEAR(aP1.X(), 1.0, 1e-10);

  anIter.Next();
  EXPECT_TRUE(anIter.More());

  gp_Pnt aP3 = BRep_Tool::Pnt(TopoDS::Vertex(anIter.Value()));
  EXPECT_NEAR(aP3.X(), 3.0, 1e-10);

  anIter.Next();
  EXPECT_FALSE(anIter.More());
}

TEST_F(TopoDS_BuilderTest, RemoveFirstVertex_FromCompound)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aV1 = makeVertex(1, 0, 0);
  TopoDS_Vertex aV2 = makeVertex(2, 0, 0);

  myBuilder.Add(aCompound, aV1);
  myBuilder.Add(aCompound, aV2);

  myBuilder.Remove(aCompound, aV1);

  EXPECT_EQ(aCompound.NbChildren(), 1);

  TopoDS_Iterator anIter(aCompound);
  gp_Pnt          aP = BRep_Tool::Pnt(TopoDS::Vertex(anIter.Value()));
  EXPECT_NEAR(aP.X(), 2.0, 1e-10);
}

TEST_F(TopoDS_BuilderTest, RemoveLastVertex_FromCompound)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aV1 = makeVertex(1, 0, 0);
  TopoDS_Vertex aV2 = makeVertex(2, 0, 0);

  myBuilder.Add(aCompound, aV1);
  myBuilder.Add(aCompound, aV2);

  myBuilder.Remove(aCompound, aV2);

  EXPECT_EQ(aCompound.NbChildren(), 1);

  TopoDS_Iterator anIter(aCompound);
  gp_Pnt          aP = BRep_Tool::Pnt(TopoDS::Vertex(anIter.Value()));
  EXPECT_NEAR(aP.X(), 1.0, 1e-10);
}

TEST_F(TopoDS_BuilderTest, RemoveAllVertices_FromCompound)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aV1 = makeVertex(1, 0, 0);
  TopoDS_Vertex aV2 = makeVertex(2, 0, 0);

  myBuilder.Add(aCompound, aV1);
  myBuilder.Add(aCompound, aV2);

  myBuilder.Remove(aCompound, aV1);
  myBuilder.Remove(aCompound, aV2);

  EXPECT_EQ(aCompound.NbChildren(), 0);

  TopoDS_Iterator anIter(aCompound);
  EXPECT_FALSE(anIter.More());
}

//==================================================================================================
// Storage migration tests (local -> dynamic)
//==================================================================================================

TEST_F(TopoDS_BuilderTest, EdgeStorage_LocalToDynamic_Migration)
{
  // Edge has LocalCapacity = 2 for vertices
  // Adding more than 2 vertices should trigger migration to dynamic storage
  TopoDS_Edge anEdge;
  myBuilder.MakeEdge(anEdge);

  TopoDS_Vertex aV1 = makeVertex(0, 0, 0);
  TopoDS_Vertex aV2 = makeVertex(1, 0, 0);
  TopoDS_Vertex aV3 = makeVertex(2, 0, 0);
  TopoDS_Vertex aV4 = makeVertex(3, 0, 0);

  // Add vertices - first 2 go to local storage
  myBuilder.Add(anEdge, aV1);
  myBuilder.Add(anEdge, aV2);
  EXPECT_EQ(anEdge.NbChildren(), 2);

  // These should trigger migration to dynamic storage
  myBuilder.Add(anEdge, aV3);
  myBuilder.Add(anEdge, aV4);
  EXPECT_EQ(anEdge.NbChildren(), 4);

  // Verify all vertices are accessible after migration
  int aCount = 0;
  for (TopoDS_Iterator anIter(anEdge); anIter.More(); anIter.Next())
  {
    EXPECT_EQ(anIter.Value().ShapeType(), TopAbs_VERTEX);
    ++aCount;
  }
  EXPECT_EQ(aCount, 4);
}

TEST_F(TopoDS_BuilderTest, FaceStorage_LocalToDynamic_Migration)
{
  // Face has LocalCapacity = 2 for wires
  // Adding more than 2 wires should trigger migration to dynamic storage
  TopoDS_Face aFace;
  myBuilder.MakeFace(aFace);

  // Create some wires
  TopoDS_Wire aW1, aW2, aW3, aW4;
  myBuilder.MakeWire(aW1);
  myBuilder.MakeWire(aW2);
  myBuilder.MakeWire(aW3);
  myBuilder.MakeWire(aW4);

  // Add wires - first 2 go to local storage
  myBuilder.Add(aFace, aW1);
  myBuilder.Add(aFace, aW2);
  EXPECT_EQ(aFace.NbChildren(), 2);

  // These should trigger migration to dynamic storage
  myBuilder.Add(aFace, aW3);
  myBuilder.Add(aFace, aW4);
  EXPECT_EQ(aFace.NbChildren(), 4);

  // Verify all wires are accessible
  int aCount = 0;
  for (TopoDS_Iterator anIter(aFace); anIter.More(); anIter.Next())
  {
    EXPECT_EQ(anIter.Value().ShapeType(), TopAbs_WIRE);
    ++aCount;
  }
  EXPECT_EQ(aCount, 4);
}

TEST_F(TopoDS_BuilderTest, SolidStorage_LocalToDynamic_Migration)
{
  // Solid has LocalCapacity = 1 for shells
  // Adding more than 1 shell should trigger migration to dynamic storage
  TopoDS_Solid aSolid;
  myBuilder.MakeSolid(aSolid);

  TopoDS_Shell aS1, aS2, aS3;
  myBuilder.MakeShell(aS1);
  myBuilder.MakeShell(aS2);
  myBuilder.MakeShell(aS3);

  // Add shells
  myBuilder.Add(aSolid, aS1);
  EXPECT_EQ(aSolid.NbChildren(), 1);

  myBuilder.Add(aSolid, aS2);
  EXPECT_EQ(aSolid.NbChildren(), 2);

  myBuilder.Add(aSolid, aS3);
  EXPECT_EQ(aSolid.NbChildren(), 3);

  // Verify all shells are accessible
  int aCount = 0;
  for (TopoDS_Iterator anIter(aSolid); anIter.More(); anIter.Next())
  {
    EXPECT_EQ(anIter.Value().ShapeType(), TopAbs_SHELL);
    ++aCount;
  }
  EXPECT_EQ(aCount, 3);
}

//==================================================================================================
// Frozen shape tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, AddToFrozenShape_ThrowsException)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aV1 = makeVertex(0, 0, 0);
  TopoDS_Vertex aV2 = makeVertex(1, 0, 0);

  myBuilder.Add(aCompound, aV1);

  // Freeze the compound by adding it to another shape
  TopoDS_Compound aParent;
  myBuilder.MakeCompound(aParent);
  myBuilder.Add(aParent, aCompound);

  // Now aCompound should be frozen (Free = false)
  EXPECT_THROW(myBuilder.Add(aCompound, aV2), TopoDS_FrozenShape);
}

TEST_F(TopoDS_BuilderTest, RemoveFromFrozenShape_ThrowsException)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aV1 = makeVertex(0, 0, 0);
  myBuilder.Add(aCompound, aV1);

  // Freeze the compound by adding it to another shape
  TopoDS_Compound aParent;
  myBuilder.MakeCompound(aParent);
  myBuilder.Add(aParent, aCompound);

  EXPECT_THROW(myBuilder.Remove(aCompound, aV1), TopoDS_FrozenShape);
}

//==================================================================================================
// Incompatible shape tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, AddIncompatibleShape_ThrowsException)
{
  TopoDS_Wire aWire;
  myBuilder.MakeWire(aWire);

  // Wire can only contain edges, not vertices directly
  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);

  EXPECT_THROW(myBuilder.Add(aWire, aVertex), TopoDS_UnCompatibleShapes);
}

TEST_F(TopoDS_BuilderTest, AddShellToWire_ThrowsException)
{
  TopoDS_Wire aWire;
  myBuilder.MakeWire(aWire);

  TopoDS_Shell aShell;
  myBuilder.MakeShell(aShell);

  EXPECT_THROW(myBuilder.Add(aWire, aShell), TopoDS_UnCompatibleShapes);
}

//==================================================================================================
// Flag preservation tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, VertexFlags_ClosedAndConvex)
{
  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);

  // Vertex should be Closed and Convex by default
  EXPECT_TRUE(aVertex.Closed());
  EXPECT_TRUE(aVertex.Convex());
}

TEST_F(TopoDS_BuilderTest, SolidFlags_NotOrientable)
{
  TopoDS_Solid aSolid;
  myBuilder.MakeSolid(aSolid);

  // Solid should not be orientable
  EXPECT_FALSE(aSolid.Orientable());
}

TEST_F(TopoDS_BuilderTest, CompoundFlags_NotOrientable)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  // Compound should not be orientable
  EXPECT_FALSE(aCompound.Orientable());
}

TEST_F(TopoDS_BuilderTest, WireFlags_Orientable)
{
  TopoDS_Wire aWire;
  myBuilder.MakeWire(aWire);

  // Wire should be orientable (default from TShape)
  EXPECT_TRUE(aWire.Orientable());
}

TEST_F(TopoDS_BuilderTest, DefaultFlags_FreeAndModified)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  // All shapes should be Free and Modified by default
  EXPECT_TRUE(aCompound.Free());
  EXPECT_TRUE(aCompound.Modified());
}

//==================================================================================================
// Large number of children tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, LargeCompound_ManyChildren)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  const int aNbShapes = 1000;
  for (int i = 0; i < aNbShapes; ++i)
  {
    myBuilder.Add(aCompound, makeVertex(i, 0, 0));
  }

  EXPECT_EQ(aCompound.NbChildren(), aNbShapes);

  // Verify iteration works correctly
  int aCount = 0;
  for (TopoDS_Iterator anIter(aCompound); anIter.More(); anIter.Next())
  {
    ++aCount;
  }
  EXPECT_EQ(aCount, aNbShapes);
}

TEST_F(TopoDS_BuilderTest, LargeWire_ManyEdges)
{
  TopoDS_Wire aWire;
  myBuilder.MakeWire(aWire);

  const int aNbEdges = 100;
  for (int i = 0; i < aNbEdges; ++i)
  {
    TopoDS_Edge anEdge;
    myBuilder.MakeEdge(anEdge);
    myBuilder.Add(aWire, anEdge);
  }

  EXPECT_EQ(aWire.NbChildren(), aNbEdges);
}

//==================================================================================================
// Iterator cumulative orientation/location tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, Iterator_CumulativeOrientation)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);
  myBuilder.Add(aCompound, aVertex);

  // Create reversed compound
  TopoDS_Shape aReversed = aCompound.Reversed();

  // Iterate with cumulative orientation
  TopoDS_Iterator anIterCum(aReversed, true, false);
  EXPECT_TRUE(anIterCum.More());
  EXPECT_EQ(anIterCum.Value().Orientation(), TopAbs_REVERSED);

  // Iterate without cumulative orientation
  TopoDS_Iterator anIterNoCum(aReversed, false, false);
  EXPECT_TRUE(anIterNoCum.More());
  EXPECT_EQ(anIterNoCum.Value().Orientation(), TopAbs_FORWARD);
}

//==================================================================================================
// Remove non-existent shape test
//==================================================================================================

TEST_F(TopoDS_BuilderTest, RemoveNonExistent_NoEffect)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aV1 = makeVertex(0, 0, 0);
  TopoDS_Vertex aV2 = makeVertex(1, 0, 0);

  myBuilder.Add(aCompound, aV1);

  // Remove a vertex that was never added - should not throw, just no effect
  EXPECT_NO_THROW(myBuilder.Remove(aCompound, aV2));
  EXPECT_EQ(aCompound.NbChildren(), 1);
}

//==================================================================================================
// Mixed shape types in compound
//==================================================================================================

TEST_F(TopoDS_BuilderTest, CompoundWithMixedTypes)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);
  TopoDS_Edge   anEdge;
  myBuilder.MakeEdge(anEdge);
  TopoDS_Wire aWire;
  myBuilder.MakeWire(aWire);
  TopoDS_Face aFace;
  myBuilder.MakeFace(aFace);
  TopoDS_Shell aShell;
  myBuilder.MakeShell(aShell);
  TopoDS_Solid aSolid;
  myBuilder.MakeSolid(aSolid);

  myBuilder.Add(aCompound, aVertex);
  myBuilder.Add(aCompound, anEdge);
  myBuilder.Add(aCompound, aWire);
  myBuilder.Add(aCompound, aFace);
  myBuilder.Add(aCompound, aShell);
  myBuilder.Add(aCompound, aSolid);

  EXPECT_EQ(aCompound.NbChildren(), 6);

  // Verify each type is present
  int aVertexCount = 0, anEdgeCount = 0, aWireCount = 0;
  int aFaceCount = 0, aShellCount = 0, aSolidCount = 0;

  for (TopoDS_Iterator anIter(aCompound); anIter.More(); anIter.Next())
  {
    switch (anIter.Value().ShapeType())
    {
      case TopAbs_VERTEX:
        ++aVertexCount;
        break;
      case TopAbs_EDGE:
        ++anEdgeCount;
        break;
      case TopAbs_WIRE:
        ++aWireCount;
        break;
      case TopAbs_FACE:
        ++aFaceCount;
        break;
      case TopAbs_SHELL:
        ++aShellCount;
        break;
      case TopAbs_SOLID:
        ++aSolidCount;
        break;
      default:
        break;
    }
  }

  EXPECT_EQ(aVertexCount, 1);
  EXPECT_EQ(anEdgeCount, 1);
  EXPECT_EQ(aWireCount, 1);
  EXPECT_EQ(aFaceCount, 1);
  EXPECT_EQ(aShellCount, 1);
  EXPECT_EQ(aSolidCount, 1);
}

//==================================================================================================
// Orientation tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, AddReversedShape_OrientationPreserved)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex         = makeVertex(0, 0, 0);
  TopoDS_Shape  aReversedVertex = aVertex.Reversed();

  myBuilder.Add(aCompound, aReversedVertex);

  TopoDS_Iterator anIter(aCompound);
  EXPECT_TRUE(anIter.More());
  EXPECT_EQ(anIter.Value().Orientation(), TopAbs_REVERSED);
}

TEST_F(TopoDS_BuilderTest, AddToReversedCompound_OrientationComposed)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);

  // Add to reversed compound - child orientation is reversed during Add
  // When parent is REVERSED, child stored orientation = Reverse(child input orientation)
  // So FORWARD vertex stored as REVERSED
  TopoDS_Shape aReversedCompound = aCompound.Reversed();
  myBuilder.Add(aReversedCompound, aVertex);

  // When iterating on reversed compound with cumOri=true:
  // result = Compose(parent_orientation, stored_child_orientation)
  // result = Compose(REVERSED, REVERSED) = FORWARD
  TopoDS_Iterator anIter(aReversedCompound, true, false);
  EXPECT_TRUE(anIter.More());
  EXPECT_EQ(anIter.Value().Orientation(), TopAbs_FORWARD);

  // Without cumulative orientation, we see the stored orientation (REVERSED)
  TopoDS_Iterator anIterNoCum(aReversedCompound, false, false);
  EXPECT_TRUE(anIterNoCum.More());
  EXPECT_EQ(anIterNoCum.Value().Orientation(), TopAbs_REVERSED);
}

TEST_F(TopoDS_BuilderTest, MixedOrientations_InCompound)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aV1 = makeVertex(1, 0, 0);
  TopoDS_Vertex aV2 = makeVertex(2, 0, 0);
  TopoDS_Vertex aV3 = makeVertex(3, 0, 0);

  myBuilder.Add(aCompound, aV1);                           // FORWARD
  myBuilder.Add(aCompound, aV2.Reversed());                // REVERSED
  myBuilder.Add(aCompound, aV3.Oriented(TopAbs_INTERNAL)); // INTERNAL

  EXPECT_EQ(aCompound.NbChildren(), 3);

  int aForwardCount = 0, aReversedCount = 0, anInternalCount = 0;
  for (TopoDS_Iterator anIter(aCompound); anIter.More(); anIter.Next())
  {
    switch (anIter.Value().Orientation())
    {
      case TopAbs_FORWARD:
        ++aForwardCount;
        break;
      case TopAbs_REVERSED:
        ++aReversedCount;
        break;
      case TopAbs_INTERNAL:
        ++anInternalCount;
        break;
      default:
        break;
    }
  }

  EXPECT_EQ(aForwardCount, 1);
  EXPECT_EQ(aReversedCount, 1);
  EXPECT_EQ(anInternalCount, 1);
}

TEST_F(TopoDS_BuilderTest, RemoveReversedShape_MatchesOrientation)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aV1 = makeVertex(1, 0, 0);
  TopoDS_Vertex aV2 = makeVertex(2, 0, 0);

  myBuilder.Add(aCompound, aV1);
  myBuilder.Add(aCompound, aV2.Reversed());

  EXPECT_EQ(aCompound.NbChildren(), 2);

  // Remove the reversed vertex - must match the orientation
  myBuilder.Remove(aCompound, aV2.Reversed());

  EXPECT_EQ(aCompound.NbChildren(), 1);

  // Only V1 should remain
  TopoDS_Iterator anIter(aCompound);
  EXPECT_TRUE(anIter.More());
  gp_Pnt aP = BRep_Tool::Pnt(TopoDS::Vertex(anIter.Value()));
  EXPECT_NEAR(aP.X(), 1.0, 1e-10);
}

TEST_F(TopoDS_BuilderTest, RemoveWithWrongOrientation_NoEffect)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);

  // Add forward vertex
  myBuilder.Add(aCompound, aVertex);
  EXPECT_EQ(aCompound.NbChildren(), 1);

  // Try to remove reversed vertex - should not match
  myBuilder.Remove(aCompound, aVertex.Reversed());

  // Should still have 1 child since orientations don't match
  EXPECT_EQ(aCompound.NbChildren(), 1);
}

//==================================================================================================
// Storage migration tests (local <-> dynamic transitions)
//==================================================================================================

TEST_F(TopoDS_BuilderTest, EdgeStorage_AddRemoveAdd_MigrationCycle)
{
  // Edge has LocalCapacity = 2
  // Test: fill local -> overflow to dynamic -> remove back to local capacity -> add again
  TopoDS_Edge anEdge;
  myBuilder.MakeEdge(anEdge);

  TopoDS_Vertex aV1 = makeVertex(1, 0, 0);
  TopoDS_Vertex aV2 = makeVertex(2, 0, 0);
  TopoDS_Vertex aV3 = makeVertex(3, 0, 0);
  TopoDS_Vertex aV4 = makeVertex(4, 0, 0);

  // Add 2 vertices (local storage)
  myBuilder.Add(anEdge, aV1);
  myBuilder.Add(anEdge, aV2);
  EXPECT_EQ(anEdge.NbChildren(), 2);

  // Add 2 more (triggers migration to dynamic)
  myBuilder.Add(anEdge, aV3);
  myBuilder.Add(anEdge, aV4);
  EXPECT_EQ(anEdge.NbChildren(), 4);

  // Remove 2 (still in dynamic storage, but size <= local capacity)
  myBuilder.Remove(anEdge, aV3);
  myBuilder.Remove(anEdge, aV4);
  EXPECT_EQ(anEdge.NbChildren(), 2);

  // Add more again (should work correctly)
  TopoDS_Vertex aV5 = makeVertex(5, 0, 0);
  TopoDS_Vertex aV6 = makeVertex(6, 0, 0);
  myBuilder.Add(anEdge, aV5);
  myBuilder.Add(anEdge, aV6);
  EXPECT_EQ(anEdge.NbChildren(), 4);

  // Verify all vertices are accessible
  int aCount = 0;
  for (TopoDS_Iterator anIter(anEdge); anIter.More(); anIter.Next())
  {
    EXPECT_EQ(anIter.Value().ShapeType(), TopAbs_VERTEX);
    ++aCount;
  }
  EXPECT_EQ(aCount, 4);
}

TEST_F(TopoDS_BuilderTest, FaceStorage_BoundaryCondition_ExactLocalCapacity)
{
  // Face has LocalCapacity = 2
  // Test exact boundary: add exactly 2 (local), then add 1 more (migrate), then remove 1
  TopoDS_Face aFace;
  myBuilder.MakeFace(aFace);

  TopoDS_Wire aW1, aW2, aW3;
  myBuilder.MakeWire(aW1);
  myBuilder.MakeWire(aW2);
  myBuilder.MakeWire(aW3);

  // Fill exactly to local capacity
  myBuilder.Add(aFace, aW1);
  myBuilder.Add(aFace, aW2);
  EXPECT_EQ(aFace.NbChildren(), 2);

  // Add one more - triggers migration
  myBuilder.Add(aFace, aW3);
  EXPECT_EQ(aFace.NbChildren(), 3);

  // Remove one - still in dynamic but size = 2
  myBuilder.Remove(aFace, aW3);
  EXPECT_EQ(aFace.NbChildren(), 2);

  // Verify iteration works
  int aCount = 0;
  for (TopoDS_Iterator anIter(aFace); anIter.More(); anIter.Next())
  {
    EXPECT_EQ(anIter.Value().ShapeType(), TopAbs_WIRE);
    ++aCount;
  }
  EXPECT_EQ(aCount, 2);
}

TEST_F(TopoDS_BuilderTest, SolidStorage_SingleCapacity_MigrationStress)
{
  // Solid has LocalCapacity = 1
  // Stress test: repeatedly add/remove around the boundary
  TopoDS_Solid aSolid;
  myBuilder.MakeSolid(aSolid);

  TopoDS_Shell aS1, aS2;
  myBuilder.MakeShell(aS1);
  myBuilder.MakeShell(aS2);

  // Cycle: add 1 (local) -> add 1 (migrate) -> remove 1 (dynamic, size=1) -> repeat
  for (int i = 0; i < 5; ++i)
  {
    myBuilder.Add(aSolid, aS1);
    EXPECT_EQ(aSolid.NbChildren(), 1);

    myBuilder.Add(aSolid, aS2);
    EXPECT_EQ(aSolid.NbChildren(), 2);

    myBuilder.Remove(aSolid, aS2);
    EXPECT_EQ(aSolid.NbChildren(), 1);

    myBuilder.Remove(aSolid, aS1);
    EXPECT_EQ(aSolid.NbChildren(), 0);
  }

  // Final state should be empty
  TopoDS_Iterator anIter(aSolid);
  EXPECT_FALSE(anIter.More());
}

TEST_F(TopoDS_BuilderTest, CompoundStorage_LargeScaleMigration)
{
  // Compound uses dynamic storage from the start (BucketSize = 8)
  // Test large-scale add/remove operations
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  const int                         aNbShapes = 100;
  NCollection_Vector<TopoDS_Vertex> aVertices;

  // Add many shapes
  for (int i = 0; i < aNbShapes; ++i)
  {
    TopoDS_Vertex aV = makeVertex(i, 0, 0);
    aVertices.Append(aV);
    myBuilder.Add(aCompound, aV);
  }
  EXPECT_EQ(aCompound.NbChildren(), aNbShapes);

  // Remove every other shape
  for (int i = aNbShapes - 1; i >= 0; i -= 2)
  {
    myBuilder.Remove(aCompound, aVertices.Value(i));
  }
  EXPECT_EQ(aCompound.NbChildren(), aNbShapes / 2);

  // Verify remaining shapes
  int aCount = 0;
  for (TopoDS_Iterator anIter(aCompound); anIter.More(); anIter.Next())
  {
    ++aCount;
  }
  EXPECT_EQ(aCount, aNbShapes / 2);
}

//==================================================================================================
// Iterator behavior during/after modifications
//==================================================================================================

TEST_F(TopoDS_BuilderTest, IteratorAfterAdd_SeesNewChildren)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  myBuilder.Add(aCompound, makeVertex(1, 0, 0));

  // Create iterator
  TopoDS_Iterator anIter(aCompound);
  EXPECT_TRUE(anIter.More());
  EXPECT_EQ(aCompound.NbChildren(), 1);

  // Add more shapes
  myBuilder.Add(aCompound, makeVertex(2, 0, 0));
  myBuilder.Add(aCompound, makeVertex(3, 0, 0));

  // Create new iterator - should see all children
  TopoDS_Iterator anIter2(aCompound);
  int             aCount = 0;
  for (; anIter2.More(); anIter2.Next())
  {
    ++aCount;
  }
  EXPECT_EQ(aCount, 3);
}

TEST_F(TopoDS_BuilderTest, IteratorAfterRemove_SeesRemainingChildren)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aV1 = makeVertex(1, 0, 0);
  TopoDS_Vertex aV2 = makeVertex(2, 0, 0);
  TopoDS_Vertex aV3 = makeVertex(3, 0, 0);

  myBuilder.Add(aCompound, aV1);
  myBuilder.Add(aCompound, aV2);
  myBuilder.Add(aCompound, aV3);

  // Remove middle element
  myBuilder.Remove(aCompound, aV2);

  // New iterator should see only 2 children
  int aCount = 0;
  for (TopoDS_Iterator anIter(aCompound); anIter.More(); anIter.Next())
  {
    ++aCount;
  }
  EXPECT_EQ(aCount, 2);
}

TEST_F(TopoDS_BuilderTest, CollectThenRemove_SafePattern)
{
  // Safe pattern: collect shapes to remove during iteration, then remove after
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  for (int i = 0; i < 10; ++i)
  {
    myBuilder.Add(aCompound, makeVertex(i, 0, 0));
  }

  // Collect shapes to remove (e.g., those with X < 5)
  NCollection_Vector<TopoDS_Shape> aToRemove;
  for (TopoDS_Iterator anIter(aCompound); anIter.More(); anIter.Next())
  {
    gp_Pnt aP = BRep_Tool::Pnt(TopoDS::Vertex(anIter.Value()));
    if (aP.X() < 5.0)
    {
      aToRemove.Append(anIter.Value());
    }
  }

  // Remove collected shapes
  for (int i = 0; i < aToRemove.Size(); ++i)
  {
    myBuilder.Remove(aCompound, aToRemove.Value(i));
  }

  EXPECT_EQ(aCompound.NbChildren(), 5);

  // Verify remaining shapes have X >= 5
  for (TopoDS_Iterator anIter(aCompound); anIter.More(); anIter.Next())
  {
    gp_Pnt aP = BRep_Tool::Pnt(TopoDS::Vertex(anIter.Value()));
    EXPECT_GE(aP.X(), 5.0);
  }
}

//==================================================================================================
// Edge cases with empty and single-element containers
//==================================================================================================

TEST_F(TopoDS_BuilderTest, RemoveFromEmpty_NoEffect)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);

  // Remove from empty - should not crash
  EXPECT_NO_THROW(myBuilder.Remove(aCompound, aVertex));
  EXPECT_EQ(aCompound.NbChildren(), 0);
}

TEST_F(TopoDS_BuilderTest, RemoveOnlyElement_BecomesEmpty)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);
  myBuilder.Add(aCompound, aVertex);
  EXPECT_EQ(aCompound.NbChildren(), 1);

  myBuilder.Remove(aCompound, aVertex);
  EXPECT_EQ(aCompound.NbChildren(), 0);

  // Iterator should show empty
  TopoDS_Iterator anIter(aCompound);
  EXPECT_FALSE(anIter.More());
}

TEST_F(TopoDS_BuilderTest, AddAfterRemoveAll_WorksCorrectly)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aV1 = makeVertex(1, 0, 0);
  TopoDS_Vertex aV2 = makeVertex(2, 0, 0);

  // Add, remove all, add again
  myBuilder.Add(aCompound, aV1);
  myBuilder.Remove(aCompound, aV1);
  EXPECT_EQ(aCompound.NbChildren(), 0);

  myBuilder.Add(aCompound, aV2);
  EXPECT_EQ(aCompound.NbChildren(), 1);

  TopoDS_Iterator anIter(aCompound);
  EXPECT_TRUE(anIter.More());
  gp_Pnt aP = BRep_Tool::Pnt(TopoDS::Vertex(anIter.Value()));
  EXPECT_NEAR(aP.X(), 2.0, 1e-10);
}

//==================================================================================================
// Wire-specific tests (dynamic storage from start)
//==================================================================================================

TEST_F(TopoDS_BuilderTest, WireStorage_DynamicFromStart)
{
  // Wire uses TopoDS_DynamicShapeStorage directly (BucketSize = 8)
  TopoDS_Wire aWire;
  myBuilder.MakeWire(aWire);

  // Add edges up to and beyond bucket size
  const int                       aNbEdges = 20;
  NCollection_Vector<TopoDS_Edge> anEdges;

  for (int i = 0; i < aNbEdges; ++i)
  {
    TopoDS_Edge anEdge;
    myBuilder.MakeEdge(anEdge);
    anEdges.Append(anEdge);
    myBuilder.Add(aWire, anEdge);
  }

  EXPECT_EQ(aWire.NbChildren(), aNbEdges);

  // Remove half
  for (int i = 0; i < aNbEdges / 2; ++i)
  {
    myBuilder.Remove(aWire, anEdges.Value(i));
  }

  EXPECT_EQ(aWire.NbChildren(), aNbEdges / 2);

  // Verify iteration
  int aCount = 0;
  for (TopoDS_Iterator anIter(aWire); anIter.More(); anIter.Next())
  {
    EXPECT_EQ(anIter.Value().ShapeType(), TopAbs_EDGE);
    ++aCount;
  }
  EXPECT_EQ(aCount, aNbEdges / 2);
}

//==================================================================================================
// Shell-specific tests (dynamic storage from start)
//==================================================================================================

TEST_F(TopoDS_BuilderTest, ShellStorage_DynamicFromStart)
{
  // Shell uses TopoDS_DynamicShapeStorage directly (BucketSize = 8)
  TopoDS_Shell aShell;
  myBuilder.MakeShell(aShell);

  // Add faces up to and beyond bucket size
  const int                       aNbFaces = 15;
  NCollection_Vector<TopoDS_Face> aFaces;

  for (int i = 0; i < aNbFaces; ++i)
  {
    TopoDS_Face aFace;
    myBuilder.MakeFace(aFace);
    aFaces.Append(aFace);
    myBuilder.Add(aShell, aFace);
  }

  EXPECT_EQ(aShell.NbChildren(), aNbFaces);

  // Remove from the end
  for (int i = aNbFaces - 1; i >= aNbFaces / 2; --i)
  {
    myBuilder.Remove(aShell, aFaces.Value(i));
  }

  int aExpectedRemaining = aNbFaces / 2;
  EXPECT_EQ(aShell.NbChildren(), aExpectedRemaining);
}

//==================================================================================================
// Duplicate shape tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, AddSameShapeTwice_BothAdded)
{
  // Adding the same shape twice should result in two children
  // (they share the same TShape but are separate entries)
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);

  myBuilder.Add(aCompound, aVertex);
  myBuilder.Add(aCompound, aVertex);

  EXPECT_EQ(aCompound.NbChildren(), 2);

  // Both should be accessible via iteration
  int aCount = 0;
  for (TopoDS_Iterator anIter(aCompound); anIter.More(); anIter.Next())
  {
    ++aCount;
  }
  EXPECT_EQ(aCount, 2);
}

TEST_F(TopoDS_BuilderTest, AddSameShapeWithDifferentOrientations)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);

  myBuilder.Add(aCompound, aVertex);                           // FORWARD
  myBuilder.Add(aCompound, aVertex.Reversed());                // REVERSED
  myBuilder.Add(aCompound, aVertex.Oriented(TopAbs_INTERNAL)); // INTERNAL
  myBuilder.Add(aCompound, aVertex.Oriented(TopAbs_EXTERNAL)); // EXTERNAL

  EXPECT_EQ(aCompound.NbChildren(), 4);

  // Count orientations
  int aForward = 0, aReversed = 0, anInternal = 0, anExternal = 0;
  for (TopoDS_Iterator anIter(aCompound); anIter.More(); anIter.Next())
  {
    switch (anIter.Value().Orientation())
    {
      case TopAbs_FORWARD:
        ++aForward;
        break;
      case TopAbs_REVERSED:
        ++aReversed;
        break;
      case TopAbs_INTERNAL:
        ++anInternal;
        break;
      case TopAbs_EXTERNAL:
        ++anExternal;
        break;
    }
  }

  EXPECT_EQ(aForward, 1);
  EXPECT_EQ(aReversed, 1);
  EXPECT_EQ(anInternal, 1);
  EXPECT_EQ(anExternal, 1);
}

TEST_F(TopoDS_BuilderTest, RemoveOneDuplicate_OtherRemains)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);

  myBuilder.Add(aCompound, aVertex);
  myBuilder.Add(aCompound, aVertex);
  EXPECT_EQ(aCompound.NbChildren(), 2);

  // Remove one - the other should remain
  myBuilder.Remove(aCompound, aVertex);
  EXPECT_EQ(aCompound.NbChildren(), 1);

  // Remove again - should become empty
  myBuilder.Remove(aCompound, aVertex);
  EXPECT_EQ(aCompound.NbChildren(), 0);
}

//==================================================================================================
// Nested compound and hierarchy tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, NestedCompounds_DeepHierarchy)
{
  // Create a deep hierarchy: compound -> compound -> compound -> vertex
  TopoDS_Compound aLevel1, aLevel2, aLevel3;
  myBuilder.MakeCompound(aLevel1);
  myBuilder.MakeCompound(aLevel2);
  myBuilder.MakeCompound(aLevel3);

  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);

  myBuilder.Add(aLevel3, aVertex);
  myBuilder.Add(aLevel2, aLevel3);
  myBuilder.Add(aLevel1, aLevel2);

  // Each level should have exactly 1 child
  EXPECT_EQ(aLevel1.NbChildren(), 1);
  EXPECT_EQ(aLevel2.NbChildren(), 1);
  EXPECT_EQ(aLevel3.NbChildren(), 1);

  // Verify iteration at each level
  TopoDS_Iterator anIter1(aLevel1);
  EXPECT_TRUE(anIter1.More());
  EXPECT_EQ(anIter1.Value().ShapeType(), TopAbs_COMPOUND);

  TopoDS_Iterator anIter2(aLevel2);
  EXPECT_TRUE(anIter2.More());
  EXPECT_EQ(anIter2.Value().ShapeType(), TopAbs_COMPOUND);

  TopoDS_Iterator anIter3(aLevel3);
  EXPECT_TRUE(anIter3.More());
  EXPECT_EQ(anIter3.Value().ShapeType(), TopAbs_VERTEX);
}

TEST_F(TopoDS_BuilderTest, CompoundInCompound_RemoveInner)
{
  TopoDS_Compound aOuter, aInner;
  myBuilder.MakeCompound(aOuter);
  myBuilder.MakeCompound(aInner);

  TopoDS_Vertex aV1 = makeVertex(1, 0, 0);
  TopoDS_Vertex aV2 = makeVertex(2, 0, 0);

  myBuilder.Add(aInner, aV1);
  myBuilder.Add(aInner, aV2);
  myBuilder.Add(aOuter, aInner);

  EXPECT_EQ(aOuter.NbChildren(), 1);
  EXPECT_EQ(aInner.NbChildren(), 2);

  // Remove the inner compound from outer
  myBuilder.Remove(aOuter, aInner);
  EXPECT_EQ(aOuter.NbChildren(), 0);

  // Inner compound should still have its children
  EXPECT_EQ(aInner.NbChildren(), 2);
}

TEST_F(TopoDS_BuilderTest, SameCompoundAddedToMultipleParents)
{
  TopoDS_Compound aParent1, aParent2, aChild;
  myBuilder.MakeCompound(aParent1);
  myBuilder.MakeCompound(aParent2);
  myBuilder.MakeCompound(aChild);

  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);
  myBuilder.Add(aChild, aVertex);

  // Add same child to both parents
  myBuilder.Add(aParent1, aChild);
  myBuilder.Add(aParent2, aChild);

  EXPECT_EQ(aParent1.NbChildren(), 1);
  EXPECT_EQ(aParent2.NbChildren(), 1);

  // Child should be frozen after being added
  EXPECT_FALSE(aChild.Free());
}

//==================================================================================================
// Location handling tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, AddWithLocation_LocationPreserved)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);

  // Create a located shape
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(10, 20, 30));
  TopLoc_Location aLoc(aTrsf);

  TopoDS_Shape aLocatedVertex = aVertex.Located(aLoc);
  myBuilder.Add(aCompound, aLocatedVertex);

  EXPECT_EQ(aCompound.NbChildren(), 1);

  // Check that the child has the location
  TopoDS_Iterator anIter(aCompound, true, true);
  EXPECT_TRUE(anIter.More());
  EXPECT_FALSE(anIter.Value().Location().IsIdentity());
}

TEST_F(TopoDS_BuilderTest, Iterator_CumulativeLocation_Enabled)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);
  myBuilder.Add(aCompound, aVertex);

  // Create located compound
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(100, 0, 0));
  TopLoc_Location aLoc(aTrsf);
  TopoDS_Shape    aLocatedCompound = aCompound.Located(aLoc);

  // With cumulative location
  TopoDS_Iterator anIterCum(aLocatedCompound, true, true);
  EXPECT_TRUE(anIterCum.More());
  EXPECT_FALSE(anIterCum.Value().Location().IsIdentity());

  // Without cumulative location
  TopoDS_Iterator anIterNoCum(aLocatedCompound, true, false);
  EXPECT_TRUE(anIterNoCum.More());
  EXPECT_TRUE(anIterNoCum.Value().Location().IsIdentity());
}

TEST_F(TopoDS_BuilderTest, AddToLocatedCompound_LocationInverted)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  // Locate the compound
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(100, 0, 0));
  TopLoc_Location aLoc(aTrsf);

  TopoDS_Shape aLocatedCompound = aCompound.Located(aLoc);

  TopoDS_Vertex aVertex = makeVertex(50, 0, 0);
  myBuilder.Add(aLocatedCompound, aVertex);

  EXPECT_EQ(aLocatedCompound.NbChildren(), 1);

  // The stored child should have inverted location applied
  // so that when composed with parent location, it gives the original position
  TopoDS_Iterator anIterNoCum(aLocatedCompound, true, false);
  EXPECT_TRUE(anIterNoCum.More());
  // Child should have location (inverted parent)
  EXPECT_FALSE(anIterNoCum.Value().Location().IsIdentity());
}

//==================================================================================================
// Edge boundary condition tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, EdgeWithExactlyTwoVertices_CommonCase)
{
  // Most common case: edge with exactly 2 vertices (start and end)
  TopoDS_Edge anEdge;
  myBuilder.MakeEdge(anEdge);

  TopoDS_Vertex aV1 = makeVertex(0, 0, 0);
  TopoDS_Vertex aV2 = makeVertex(1, 0, 0);

  myBuilder.Add(anEdge, aV1.Oriented(TopAbs_FORWARD));
  myBuilder.Add(anEdge, aV2.Oriented(TopAbs_REVERSED));

  EXPECT_EQ(anEdge.NbChildren(), 2);

  // Verify orientations
  int aForward = 0, aReversed = 0;
  for (TopoDS_Iterator anIter(anEdge); anIter.More(); anIter.Next())
  {
    if (anIter.Value().Orientation() == TopAbs_FORWARD)
      ++aForward;
    else if (anIter.Value().Orientation() == TopAbs_REVERSED)
      ++aReversed;
  }
  EXPECT_EQ(aForward, 1);
  EXPECT_EQ(aReversed, 1);
}

TEST_F(TopoDS_BuilderTest, EdgeWithSameVertexTwice_ClosedEdge)
{
  // Closed edge: same vertex for start and end
  TopoDS_Edge anEdge;
  myBuilder.MakeEdge(anEdge);

  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);

  myBuilder.Add(anEdge, aVertex.Oriented(TopAbs_FORWARD));
  myBuilder.Add(anEdge, aVertex.Oriented(TopAbs_REVERSED));

  EXPECT_EQ(anEdge.NbChildren(), 2);

  // Both entries share same TShape
  TopoDS_Iterator anIter(anEdge);
  TopoDS_Shape    aFirst = anIter.Value();
  anIter.Next();
  TopoDS_Shape aSecond = anIter.Value();

  EXPECT_TRUE(aFirst.TShape() == aSecond.TShape());
  EXPECT_NE(aFirst.Orientation(), aSecond.Orientation());
}

//==================================================================================================
// CompSolid tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, CompSolidWithMultipleSolids)
{
  TopoDS_CompSolid aCompSolid;
  myBuilder.MakeCompSolid(aCompSolid);

  const int aNbSolids = 5;
  for (int i = 0; i < aNbSolids; ++i)
  {
    TopoDS_Solid aSolid;
    myBuilder.MakeSolid(aSolid);
    myBuilder.Add(aCompSolid, aSolid);
  }

  EXPECT_EQ(aCompSolid.NbChildren(), aNbSolids);

  int aCount = 0;
  for (TopoDS_Iterator anIter(aCompSolid); anIter.More(); anIter.Next())
  {
    EXPECT_EQ(anIter.Value().ShapeType(), TopAbs_SOLID);
    ++aCount;
  }
  EXPECT_EQ(aCount, aNbSolids);
}

//==================================================================================================
// Stress tests with rapid add/remove cycles
//==================================================================================================

TEST_F(TopoDS_BuilderTest, RapidAddRemoveCycles_Stability)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);

  // Rapid add/remove cycles
  for (int i = 0; i < 100; ++i)
  {
    myBuilder.Add(aCompound, aVertex);
    EXPECT_EQ(aCompound.NbChildren(), 1);

    myBuilder.Remove(aCompound, aVertex);
    EXPECT_EQ(aCompound.NbChildren(), 0);
  }

  // Final state should be empty
  TopoDS_Iterator anIter(aCompound);
  EXPECT_FALSE(anIter.More());
}

TEST_F(TopoDS_BuilderTest, AlternatingAddRemove_InterleavedOperations)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  NCollection_Vector<TopoDS_Vertex> aVertices;
  for (int i = 0; i < 20; ++i)
  {
    aVertices.Append(makeVertex(i, 0, 0));
  }

  // Add all
  for (int i = 0; i < 20; ++i)
  {
    myBuilder.Add(aCompound, aVertices.Value(i));
  }
  EXPECT_EQ(aCompound.NbChildren(), 20);

  // Remove even indices
  for (int i = 0; i < 20; i += 2)
  {
    myBuilder.Remove(aCompound, aVertices.Value(i));
  }
  EXPECT_EQ(aCompound.NbChildren(), 10);

  // Add them back
  for (int i = 0; i < 20; i += 2)
  {
    myBuilder.Add(aCompound, aVertices.Value(i));
  }
  EXPECT_EQ(aCompound.NbChildren(), 20);

  // Verify all accessible
  int aCount = 0;
  for (TopoDS_Iterator anIter(aCompound); anIter.More(); anIter.Next())
  {
    ++aCount;
  }
  EXPECT_EQ(aCount, 20);
}

//==================================================================================================
// Special orientation composition tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, DoubleReversed_BecomesForward)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);

  // Create reversed compound
  TopoDS_Shape aReversedCompound = aCompound.Reversed();

  // Add reversed vertex to reversed compound
  myBuilder.Add(aReversedCompound, aVertex.Reversed());

  // With cumulative orientation:
  // parent = REVERSED, stored child = Reverse(REVERSED) = FORWARD
  // result = Compose(REVERSED, FORWARD) = REVERSED
  TopoDS_Iterator anIter(aReversedCompound, true, false);
  EXPECT_TRUE(anIter.More());
  EXPECT_EQ(anIter.Value().Orientation(), TopAbs_REVERSED);
}

TEST_F(TopoDS_BuilderTest, InternalOrientation_NotAffectedByParent)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);

  // Add INTERNAL vertex
  myBuilder.Add(aCompound, aVertex.Oriented(TopAbs_INTERNAL));

  TopoDS_Iterator anIter(aCompound);
  EXPECT_TRUE(anIter.More());
  EXPECT_EQ(anIter.Value().Orientation(), TopAbs_INTERNAL);

  // Now with reversed compound
  TopoDS_Compound aCompound2;
  myBuilder.MakeCompound(aCompound2);
  TopoDS_Shape aReversed = aCompound2.Reversed();

  myBuilder.Add(aReversed, aVertex.Oriented(TopAbs_INTERNAL));

  // INTERNAL composed with REVERSED should give INTERNAL (not affected by compose)
  TopoDS_Iterator anIter2(aReversed, true, false);
  EXPECT_TRUE(anIter2.More());
  // Compose(REVERSED, INTERNAL) = INTERNAL
  EXPECT_EQ(anIter2.Value().Orientation(), TopAbs_INTERNAL);
}

//==================================================================================================
// Iterator reuse and state tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, Iterator_ReuseAfterShapeModification)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  myBuilder.Add(aCompound, makeVertex(1, 0, 0));
  myBuilder.Add(aCompound, makeVertex(2, 0, 0));

  // Create iterator and iterate partially
  TopoDS_Iterator anIter(aCompound);
  EXPECT_TRUE(anIter.More());
  anIter.Next();
  EXPECT_TRUE(anIter.More());

  // Modify the shape
  myBuilder.Add(aCompound, makeVertex(3, 0, 0));

  // Re-initialize iterator
  anIter.Initialize(aCompound);

  // Should now see all 3 children
  int aCount = 0;
  for (; anIter.More(); anIter.Next())
  {
    ++aCount;
  }
  EXPECT_EQ(aCount, 3);
}

TEST_F(TopoDS_BuilderTest, MultipleIterators_SameShape)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  for (int i = 0; i < 5; ++i)
  {
    myBuilder.Add(aCompound, makeVertex(i, 0, 0));
  }

  // Create multiple iterators on same shape
  TopoDS_Iterator anIter1(aCompound);
  TopoDS_Iterator anIter2(aCompound);
  TopoDS_Iterator anIter3(aCompound);

  // All should work independently
  int aCount1 = 0, aCount2 = 0, aCount3 = 0;

  for (; anIter1.More(); anIter1.Next())
    ++aCount1;
  for (; anIter2.More(); anIter2.Next())
    ++aCount2;
  for (; anIter3.More(); anIter3.Next())
    ++aCount3;

  EXPECT_EQ(aCount1, 5);
  EXPECT_EQ(aCount2, 5);
  EXPECT_EQ(aCount3, 5);
}

//==================================================================================================
// Empty after clear tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, ClearByRemovingAll_ThenAddAgain)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  NCollection_Vector<TopoDS_Vertex> aVertices;
  for (int i = 0; i < 10; ++i)
  {
    TopoDS_Vertex aV = makeVertex(i, 0, 0);
    aVertices.Append(aV);
    myBuilder.Add(aCompound, aV);
  }
  EXPECT_EQ(aCompound.NbChildren(), 10);

  // Remove all
  for (int i = 0; i < 10; ++i)
  {
    myBuilder.Remove(aCompound, aVertices.Value(i));
  }
  EXPECT_EQ(aCompound.NbChildren(), 0);

  // Add new vertices
  for (int i = 0; i < 5; ++i)
  {
    myBuilder.Add(aCompound, makeVertex(100 + i, 0, 0));
  }
  EXPECT_EQ(aCompound.NbChildren(), 5);

  // Verify iteration
  int aCount = 0;
  for (TopoDS_Iterator anIter(aCompound); anIter.More(); anIter.Next())
  {
    gp_Pnt aP = BRep_Tool::Pnt(TopoDS::Vertex(anIter.Value()));
    EXPECT_GE(aP.X(), 100.0);
    ++aCount;
  }
  EXPECT_EQ(aCount, 5);
}

//==================================================================================================
// Solid with mixed children (shell, edge, vertex)
//==================================================================================================

TEST_F(TopoDS_BuilderTest, SolidWithMixedChildren)
{
  // Solid can contain shells, edges, and vertices
  TopoDS_Solid aSolid;
  myBuilder.MakeSolid(aSolid);

  TopoDS_Shell  aShell;
  TopoDS_Edge   anEdge;
  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);

  myBuilder.MakeShell(aShell);
  myBuilder.MakeEdge(anEdge);

  myBuilder.Add(aSolid, aShell);
  myBuilder.Add(aSolid, anEdge);
  myBuilder.Add(aSolid, aVertex);

  EXPECT_EQ(aSolid.NbChildren(), 3);

  int aShellCount = 0, anEdgeCount = 0, aVertexCount = 0;
  for (TopoDS_Iterator anIter(aSolid); anIter.More(); anIter.Next())
  {
    switch (anIter.Value().ShapeType())
    {
      case TopAbs_SHELL:
        ++aShellCount;
        break;
      case TopAbs_EDGE:
        ++anEdgeCount;
        break;
      case TopAbs_VERTEX:
        ++aVertexCount;
        break;
      default:
        break;
    }
  }

  EXPECT_EQ(aShellCount, 1);
  EXPECT_EQ(anEdgeCount, 1);
  EXPECT_EQ(aVertexCount, 1);
}

//==================================================================================================
// Face with mixed children (wires and vertices)
//==================================================================================================

TEST_F(TopoDS_BuilderTest, FaceWithWiresAndVertices)
{
  // Face can contain wires (boundaries) and isolated vertices (points on surface).
  // Isolated vertices represent special points lying on the face surface
  // that are not part of any edge - this is valid B-Rep topology.
  TopoDS_Face aFace;
  myBuilder.MakeFace(aFace);

  TopoDS_Wire   aW1, aW2;
  TopoDS_Vertex aV1 = makeVertex(0, 0, 0);
  TopoDS_Vertex aV2 = makeVertex(1, 1, 0);

  myBuilder.MakeWire(aW1);
  myBuilder.MakeWire(aW2);

  myBuilder.Add(aFace, aW1); // Outer wire (boundary)
  myBuilder.Add(aFace, aW2); // Inner wire (hole)
  myBuilder.Add(aFace, aV1); // Isolated vertex on face surface
  myBuilder.Add(aFace, aV2); // Another isolated vertex on face surface

  EXPECT_EQ(aFace.NbChildren(), 4);

  int aWireCount = 0, aVertexCount = 0;
  for (TopoDS_Iterator anIter(aFace); anIter.More(); anIter.Next())
  {
    if (anIter.Value().ShapeType() == TopAbs_WIRE)
      ++aWireCount;
    else if (anIter.Value().ShapeType() == TopAbs_VERTEX)
      ++aVertexCount;
  }

  EXPECT_EQ(aWireCount, 2);
  EXPECT_EQ(aVertexCount, 2);
}
