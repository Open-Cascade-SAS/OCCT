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
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <NCollection_Vector.hxx>
#include <TopLoc_Location.hxx>
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

#include <cmath>
#include <functional>

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

//==================================================================================================
// Real geometry tests using BRepPrimAPI
//==================================================================================================

TEST_F(TopoDS_BuilderTest, BoxShape_IterateShells)
{
  // Create a real box and iterate its structure
  TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10, 20, 30).Shape();
  EXPECT_EQ(aBox.ShapeType(), TopAbs_SOLID);

  // Solid should contain shell(s)
  int aShellCount = 0;
  for (TopoDS_Iterator anIter(aBox); anIter.More(); anIter.Next())
  {
    EXPECT_EQ(anIter.Value().ShapeType(), TopAbs_SHELL);
    ++aShellCount;
  }
  EXPECT_EQ(aShellCount, 1);
}

TEST_F(TopoDS_BuilderTest, BoxShape_IterateFaces)
{
  TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10, 20, 30).Shape();

  // Get the shell from the solid
  TopoDS_Iterator aSolidIter(aBox);
  EXPECT_TRUE(aSolidIter.More());
  const TopoDS_Shape& aShell = aSolidIter.Value();
  EXPECT_EQ(aShell.ShapeType(), TopAbs_SHELL);

  // Shell should contain 6 faces (box has 6 faces)
  int aFaceCount = 0;
  for (TopoDS_Iterator aShellIter(aShell); aShellIter.More(); aShellIter.Next())
  {
    EXPECT_EQ(aShellIter.Value().ShapeType(), TopAbs_FACE);
    ++aFaceCount;
  }
  EXPECT_EQ(aFaceCount, 6);
}

TEST_F(TopoDS_BuilderTest, CompoundOfBoxes_MultipleShapes)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  // Add multiple boxes
  const int aNbBoxes = 5;
  for (int i = 0; i < aNbBoxes; ++i)
  {
    TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10 + i, 10 + i, 10 + i).Shape();
    myBuilder.Add(aCompound, aBox);
  }

  EXPECT_EQ(aCompound.NbChildren(), aNbBoxes);

  // All children should be solids
  for (TopoDS_Iterator anIter(aCompound); anIter.More(); anIter.Next())
  {
    EXPECT_EQ(anIter.Value().ShapeType(), TopAbs_SOLID);
  }
}

//==================================================================================================
// Wire from real edges
//==================================================================================================

TEST_F(TopoDS_BuilderTest, WireFromEdges_RealGeometry)
{
  // Create edges from line segments
  gp_Pnt aP1(0, 0, 0);
  gp_Pnt aP2(10, 0, 0);
  gp_Pnt aP3(10, 10, 0);
  gp_Pnt aP4(0, 10, 0);

  TopoDS_Edge anE1 = BRepBuilderAPI_MakeEdge(aP1, aP2);
  TopoDS_Edge anE2 = BRepBuilderAPI_MakeEdge(aP2, aP3);
  TopoDS_Edge anE3 = BRepBuilderAPI_MakeEdge(aP3, aP4);
  TopoDS_Edge anE4 = BRepBuilderAPI_MakeEdge(aP4, aP1);

  // Build wire from edges
  BRepBuilderAPI_MakeWire aWireMaker;
  aWireMaker.Add(anE1);
  aWireMaker.Add(anE2);
  aWireMaker.Add(anE3);
  aWireMaker.Add(anE4);
  EXPECT_TRUE(aWireMaker.IsDone());

  TopoDS_Wire aWire = aWireMaker.Wire();
  EXPECT_EQ(aWire.NbChildren(), 4);

  // All children should be edges
  for (TopoDS_Iterator anIter(aWire); anIter.More(); anIter.Next())
  {
    EXPECT_EQ(anIter.Value().ShapeType(), TopAbs_EDGE);
  }
}

TEST_F(TopoDS_BuilderTest, EdgeFromVertices_RealGeometry)
{
  gp_Pnt aP1(0, 0, 0);
  gp_Pnt aP2(100, 0, 0);

  TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(aP1, aP2);
  EXPECT_EQ(anEdge.NbChildren(), 2);

  // Children should be vertices
  int aVertexCount = 0;
  for (TopoDS_Iterator anIter(anEdge); anIter.More(); anIter.Next())
  {
    EXPECT_EQ(anIter.Value().ShapeType(), TopAbs_VERTEX);
    ++aVertexCount;
  }
  EXPECT_EQ(aVertexCount, 2);
}

//==================================================================================================
// Shape modification flags
//==================================================================================================

TEST_F(TopoDS_BuilderTest, ModifiedFlag_SetAndCheck)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  // Initially should be modified
  EXPECT_TRUE(aCompound.Modified());

  // Clear modified flag
  aCompound.Modified(false);
  EXPECT_FALSE(aCompound.Modified());

  // Set it back
  aCompound.Modified(true);
  EXPECT_TRUE(aCompound.Modified());
}

TEST_F(TopoDS_BuilderTest, CheckedFlag_SetAndCheck)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  // Initially should not be checked
  EXPECT_FALSE(aCompound.Checked());

  // Set checked flag
  aCompound.Checked(true);
  EXPECT_TRUE(aCompound.Checked());

  // Clear checked flag
  aCompound.Checked(false);
  EXPECT_FALSE(aCompound.Checked());
}

TEST_F(TopoDS_BuilderTest, InfiniteFlag_SetAndCheck)
{
  TopoDS_Edge anEdge;
  myBuilder.MakeEdge(anEdge);

  // Initially should not be infinite
  EXPECT_FALSE(anEdge.Infinite());

  // Set infinite flag
  anEdge.Infinite(true);
  EXPECT_TRUE(anEdge.Infinite());

  // Clear infinite flag
  anEdge.Infinite(false);
  EXPECT_FALSE(anEdge.Infinite());
}

TEST_F(TopoDS_BuilderTest, LockedFlag_SetAndCheck)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  // Initially should not be locked
  EXPECT_FALSE(aCompound.Locked());

  // Set locked flag
  aCompound.Locked(true);
  EXPECT_TRUE(aCompound.Locked());

  // Clear locked flag
  aCompound.Locked(false);
  EXPECT_FALSE(aCompound.Locked());
}

//==================================================================================================
// Shape hash code tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, ShapeHashCode_SameTShape)
{
  TopoDS_Vertex aVertex   = makeVertex(0, 0, 0);
  TopoDS_Shape  aReversed = aVertex.Reversed();

  // Same TShape should give same hash code (using std::hash)
  std::hash<TopoDS_Shape> aHasher;
  EXPECT_EQ(aHasher(aVertex), aHasher(aReversed));
}

TEST_F(TopoDS_BuilderTest, ShapeHashCode_DifferentTShape)
{
  TopoDS_Vertex aV1 = makeVertex(0, 0, 0);
  TopoDS_Vertex aV2 = makeVertex(1, 0, 0);

  // Different TShape may give different hash codes
  // (not guaranteed, but typically different)
  // Just verify hash code is computable
  std::hash<TopoDS_Shape> aHasher;
  size_t                  aHash1 = aHasher(aV1);
  size_t                  aHash2 = aHasher(aV2);
  // Just verify they are computed (no specific value check needed)
  (void)aHash1;
  (void)aHash2;
}

//==================================================================================================
// Shape equality tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, ShapeEquality_SameShape)
{
  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);
  TopoDS_Shape  aCopy   = aVertex;

  EXPECT_TRUE(aVertex.IsSame(aCopy));
  EXPECT_TRUE(aVertex.IsEqual(aCopy));
}

TEST_F(TopoDS_BuilderTest, ShapeEquality_DifferentOrientation)
{
  TopoDS_Vertex aVertex   = makeVertex(0, 0, 0);
  TopoDS_Shape  aReversed = aVertex.Reversed();

  // IsSame checks TShape only
  EXPECT_TRUE(aVertex.IsSame(aReversed));

  // IsEqual checks TShape, Location, and Orientation
  EXPECT_FALSE(aVertex.IsEqual(aReversed));
}

TEST_F(TopoDS_BuilderTest, ShapeEquality_DifferentLocation)
{
  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(10, 0, 0));
  TopLoc_Location aLoc(aTrsf);
  TopoDS_Shape    aLocated = aVertex.Located(aLoc);

  // IsSame checks TShape and Location (but not Orientation)
  // Different locations mean IsSame returns false
  EXPECT_FALSE(aVertex.IsSame(aLocated));

  // IsEqual checks TShape, Location, and Orientation
  EXPECT_FALSE(aVertex.IsEqual(aLocated));

  // IsPartner checks TShape only (ignores Location and Orientation)
  EXPECT_TRUE(aVertex.IsPartner(aLocated));
}

TEST_F(TopoDS_BuilderTest, ShapePartner_SameTShape)
{
  TopoDS_Vertex aVertex   = makeVertex(0, 0, 0);
  TopoDS_Shape  aReversed = aVertex.Reversed();

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(10, 0, 0));
  TopLoc_Location aLoc(aTrsf);
  TopoDS_Shape    aLocated = aVertex.Located(aLoc);

  // IsPartner checks if TShape is shared
  EXPECT_TRUE(aVertex.IsPartner(aReversed));
  EXPECT_TRUE(aVertex.IsPartner(aLocated));
  EXPECT_TRUE(aReversed.IsPartner(aLocated));
}

//==================================================================================================
// TShape sharing tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, TShapeSharing_ChildrenShareTShape)
{
  TopoDS_Compound aCompound1, aCompound2;
  myBuilder.MakeCompound(aCompound1);
  myBuilder.MakeCompound(aCompound2);

  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);

  myBuilder.Add(aCompound1, aVertex);
  myBuilder.Add(aCompound2, aVertex);

  // Both compounds have children that share the same TShape
  TopoDS_Iterator anIter1(aCompound1);
  TopoDS_Iterator anIter2(aCompound2);

  EXPECT_TRUE(anIter1.More());
  EXPECT_TRUE(anIter2.More());
  EXPECT_TRUE(anIter1.Value().IsSame(anIter2.Value()));
}

//==================================================================================================
// Nullify and emptiness tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, NullifyShape_BecomesNull)
{
  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);
  EXPECT_FALSE(aVertex.IsNull());

  aVertex.Nullify();
  EXPECT_TRUE(aVertex.IsNull());
}

TEST_F(TopoDS_BuilderTest, EmptyVsNull_Difference)
{
  // Null shape - no TShape at all
  TopoDS_Shape aNullShape;
  EXPECT_TRUE(aNullShape.IsNull());

  // Empty compound - has TShape but no children
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);
  EXPECT_FALSE(aCompound.IsNull());
  EXPECT_EQ(aCompound.NbChildren(), 0);
}

//==================================================================================================
// EmptyCopy tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, EmptyCopy_Compound)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);
  myBuilder.Add(aCompound, makeVertex(0, 0, 0));
  myBuilder.Add(aCompound, makeVertex(1, 0, 0));

  EXPECT_EQ(aCompound.NbChildren(), 2);

  // EmptyCopy creates a shape with same TShape type but no children
  TopoDS_Shape anEmptyCopy = aCompound.EmptyCopied();
  EXPECT_EQ(anEmptyCopy.ShapeType(), TopAbs_COMPOUND);
  EXPECT_EQ(anEmptyCopy.NbChildren(), 0);
}

TEST_F(TopoDS_BuilderTest, EmptyCopy_Wire)
{
  TopoDS_Wire aWire;
  myBuilder.MakeWire(aWire);

  TopoDS_Edge anEdge;
  myBuilder.MakeEdge(anEdge);
  myBuilder.Add(aWire, anEdge);

  EXPECT_EQ(aWire.NbChildren(), 1);

  TopoDS_Shape anEmptyCopy = aWire.EmptyCopied();
  EXPECT_EQ(anEmptyCopy.ShapeType(), TopAbs_WIRE);
  EXPECT_EQ(anEmptyCopy.NbChildren(), 0);
}

//==================================================================================================
// Complement orientation tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, ComplementOrientation_Forward)
{
  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);
  EXPECT_EQ(aVertex.Orientation(), TopAbs_FORWARD);

  TopoDS_Shape aComplemented = aVertex.Complemented();
  EXPECT_EQ(aComplemented.Orientation(), TopAbs_REVERSED);
}

TEST_F(TopoDS_BuilderTest, ComplementOrientation_Reversed)
{
  TopoDS_Vertex aVertex   = makeVertex(0, 0, 0);
  TopoDS_Shape  aReversed = aVertex.Reversed();
  EXPECT_EQ(aReversed.Orientation(), TopAbs_REVERSED);

  TopoDS_Shape aComplemented = aReversed.Complemented();
  EXPECT_EQ(aComplemented.Orientation(), TopAbs_FORWARD);
}

TEST_F(TopoDS_BuilderTest, ComplementOrientation_Internal)
{
  TopoDS_Vertex aVertex    = makeVertex(0, 0, 0);
  TopoDS_Shape  anInternal = aVertex.Oriented(TopAbs_INTERNAL);
  EXPECT_EQ(anInternal.Orientation(), TopAbs_INTERNAL);

  TopoDS_Shape aComplemented = anInternal.Complemented();
  EXPECT_EQ(aComplemented.Orientation(), TopAbs_EXTERNAL);
}

TEST_F(TopoDS_BuilderTest, ComplementOrientation_External)
{
  TopoDS_Vertex aVertex    = makeVertex(0, 0, 0);
  TopoDS_Shape  anExternal = aVertex.Oriented(TopAbs_EXTERNAL);
  EXPECT_EQ(anExternal.Orientation(), TopAbs_EXTERNAL);

  TopoDS_Shape aComplemented = anExternal.Complemented();
  EXPECT_EQ(aComplemented.Orientation(), TopAbs_INTERNAL);
}

//==================================================================================================
// Compose orientation tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, ComposeOrientation_ForwardForward)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);
  myBuilder.Add(aCompound, aVertex);

  // Forward parent + Forward child = Forward
  TopoDS_Iterator anIter(aCompound, true, false);
  EXPECT_TRUE(anIter.More());
  EXPECT_EQ(anIter.Value().Orientation(), TopAbs_FORWARD);
}

TEST_F(TopoDS_BuilderTest, ComposeOrientation_ReversedForward)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);
  myBuilder.Add(aCompound, aVertex);

  TopoDS_Shape aReversedCompound = aCompound.Reversed();

  // Reversed parent + Forward child = Reversed
  TopoDS_Iterator anIter(aReversedCompound, true, false);
  EXPECT_TRUE(anIter.More());
  EXPECT_EQ(anIter.Value().Orientation(), TopAbs_REVERSED);
}

TEST_F(TopoDS_BuilderTest, ComposeOrientation_ForwardReversed)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);
  myBuilder.Add(aCompound, aVertex.Reversed());

  // Forward parent + Reversed child = Reversed
  TopoDS_Iterator anIter(aCompound, true, false);
  EXPECT_TRUE(anIter.More());
  EXPECT_EQ(anIter.Value().Orientation(), TopAbs_REVERSED);
}

TEST_F(TopoDS_BuilderTest, ComposeOrientation_ReversedReversed)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);
  myBuilder.Add(aCompound, aVertex.Reversed());

  TopoDS_Shape aReversedCompound = aCompound.Reversed();

  // Reversed parent + Reversed child = Forward
  TopoDS_Iterator anIter(aReversedCompound, true, false);
  EXPECT_TRUE(anIter.More());
  EXPECT_EQ(anIter.Value().Orientation(), TopAbs_FORWARD);
}

//==================================================================================================
// Location operations tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, LocationMove_Translation)
{
  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(100, 200, 300));
  TopLoc_Location aLoc(aTrsf);

  TopoDS_Shape aMoved = aVertex.Moved(aLoc);

  // Check that location is applied
  EXPECT_FALSE(aMoved.Location().IsIdentity());

  // Get the actual point with location applied
  gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(aMoved));
  EXPECT_NEAR(aPnt.X(), 100.0, 1e-10);
  EXPECT_NEAR(aPnt.Y(), 200.0, 1e-10);
  EXPECT_NEAR(aPnt.Z(), 300.0, 1e-10);
}

TEST_F(TopoDS_BuilderTest, LocationLocated_ReplacesLocation)
{
  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);

  // Apply first location
  gp_Trsf aTrsf1;
  aTrsf1.SetTranslation(gp_Vec(10, 0, 0));
  TopLoc_Location aLoc1(aTrsf1);
  TopoDS_Shape    aLocated1 = aVertex.Located(aLoc1);

  // Apply second location (replaces first)
  gp_Trsf aTrsf2;
  aTrsf2.SetTranslation(gp_Vec(0, 20, 0));
  TopLoc_Location aLoc2(aTrsf2);
  TopoDS_Shape    aLocated2 = aLocated1.Located(aLoc2);

  // Located replaces the entire location
  gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(aLocated2));
  EXPECT_NEAR(aPnt.X(), 0.0, 1e-10);
  EXPECT_NEAR(aPnt.Y(), 20.0, 1e-10);
  EXPECT_NEAR(aPnt.Z(), 0.0, 1e-10);
}

TEST_F(TopoDS_BuilderTest, LocationMoved_ChainsLocation)
{
  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);

  // Apply first location
  gp_Trsf aTrsf1;
  aTrsf1.SetTranslation(gp_Vec(10, 0, 0));
  TopLoc_Location aLoc1(aTrsf1);
  TopoDS_Shape    aMoved1 = aVertex.Moved(aLoc1);

  // Apply second location (chains with first)
  gp_Trsf aTrsf2;
  aTrsf2.SetTranslation(gp_Vec(0, 20, 0));
  TopLoc_Location aLoc2(aTrsf2);
  TopoDS_Shape    aMoved2 = aMoved1.Moved(aLoc2);

  // Moved chains locations together
  gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(aMoved2));
  EXPECT_NEAR(aPnt.X(), 10.0, 1e-10);
  EXPECT_NEAR(aPnt.Y(), 20.0, 1e-10);
  EXPECT_NEAR(aPnt.Z(), 0.0, 1e-10);
}

//==================================================================================================
// Deep hierarchy tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, DeepHierarchy_FiveLevel)
{
  // Create 5-level deep hierarchy
  TopoDS_Compound aLevel1, aLevel2, aLevel3, aLevel4, aLevel5;
  myBuilder.MakeCompound(aLevel1);
  myBuilder.MakeCompound(aLevel2);
  myBuilder.MakeCompound(aLevel3);
  myBuilder.MakeCompound(aLevel4);
  myBuilder.MakeCompound(aLevel5);

  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);

  myBuilder.Add(aLevel5, aVertex);
  myBuilder.Add(aLevel4, aLevel5);
  myBuilder.Add(aLevel3, aLevel4);
  myBuilder.Add(aLevel2, aLevel3);
  myBuilder.Add(aLevel1, aLevel2);

  // Verify each level has exactly 1 child
  EXPECT_EQ(aLevel1.NbChildren(), 1);
  EXPECT_EQ(aLevel2.NbChildren(), 1);
  EXPECT_EQ(aLevel3.NbChildren(), 1);
  EXPECT_EQ(aLevel4.NbChildren(), 1);
  EXPECT_EQ(aLevel5.NbChildren(), 1);

  // Traverse down to find the vertex
  TopoDS_Iterator anIter1(aLevel1);
  EXPECT_EQ(anIter1.Value().ShapeType(), TopAbs_COMPOUND);

  TopoDS_Iterator anIter2(anIter1.Value());
  EXPECT_EQ(anIter2.Value().ShapeType(), TopAbs_COMPOUND);

  TopoDS_Iterator anIter3(anIter2.Value());
  EXPECT_EQ(anIter3.Value().ShapeType(), TopAbs_COMPOUND);

  TopoDS_Iterator anIter4(anIter3.Value());
  EXPECT_EQ(anIter4.Value().ShapeType(), TopAbs_COMPOUND);

  TopoDS_Iterator anIter5(anIter4.Value());
  EXPECT_EQ(anIter5.Value().ShapeType(), TopAbs_VERTEX);
}

TEST_F(TopoDS_BuilderTest, WideHierarchy_ManyChildren)
{
  // Compound with many children of different types
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  const int aNbEachType = 10;

  // Add vertices
  for (int i = 0; i < aNbEachType; ++i)
  {
    myBuilder.Add(aCompound, makeVertex(i, 0, 0));
  }

  // Add edges
  for (int i = 0; i < aNbEachType; ++i)
  {
    TopoDS_Edge anEdge;
    myBuilder.MakeEdge(anEdge);
    myBuilder.Add(aCompound, anEdge);
  }

  // Add wires
  for (int i = 0; i < aNbEachType; ++i)
  {
    TopoDS_Wire aWire;
    myBuilder.MakeWire(aWire);
    myBuilder.Add(aCompound, aWire);
  }

  // Add faces
  for (int i = 0; i < aNbEachType; ++i)
  {
    TopoDS_Face aFace;
    myBuilder.MakeFace(aFace);
    myBuilder.Add(aCompound, aFace);
  }

  // Add shells
  for (int i = 0; i < aNbEachType; ++i)
  {
    TopoDS_Shell aShell;
    myBuilder.MakeShell(aShell);
    myBuilder.Add(aCompound, aShell);
  }

  // Add solids
  for (int i = 0; i < aNbEachType; ++i)
  {
    TopoDS_Solid aSolid;
    myBuilder.MakeSolid(aSolid);
    myBuilder.Add(aCompound, aSolid);
  }

  EXPECT_EQ(aCompound.NbChildren(), 6 * aNbEachType);

  // Count by type
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

  EXPECT_EQ(aVertexCount, aNbEachType);
  EXPECT_EQ(anEdgeCount, aNbEachType);
  EXPECT_EQ(aWireCount, aNbEachType);
  EXPECT_EQ(aFaceCount, aNbEachType);
  EXPECT_EQ(aShellCount, aNbEachType);
  EXPECT_EQ(aSolidCount, aNbEachType);
}

//==================================================================================================
// Stress tests with many operations
//==================================================================================================

TEST_F(TopoDS_BuilderTest, StressTest_ThousandShapes)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  const int aNbShapes = 1000;

  // Add thousand vertices
  for (int i = 0; i < aNbShapes; ++i)
  {
    myBuilder.Add(aCompound, makeVertex(i, i, i));
  }

  EXPECT_EQ(aCompound.NbChildren(), aNbShapes);

  // Verify iteration is correct
  int aCount = 0;
  for (TopoDS_Iterator anIter(aCompound); anIter.More(); anIter.Next())
  {
    ++aCount;
  }
  EXPECT_EQ(aCount, aNbShapes);
}

TEST_F(TopoDS_BuilderTest, StressTest_ManyAddRemoveCycles)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  const int aNbCycles         = 50;
  const int aNbShapesPerCycle = 20;

  for (int i = 0; i < aNbCycles; ++i)
  {
    NCollection_Vector<TopoDS_Vertex> aVertices;

    // Add shapes
    for (int j = 0; j < aNbShapesPerCycle; ++j)
    {
      TopoDS_Vertex aV = makeVertex(i * 100 + j, 0, 0);
      aVertices.Append(aV);
      myBuilder.Add(aCompound, aV);
    }

    EXPECT_EQ(aCompound.NbChildren(), aNbShapesPerCycle);

    // Remove all shapes
    for (int j = 0; j < aNbShapesPerCycle; ++j)
    {
      myBuilder.Remove(aCompound, aVertices.Value(j));
    }

    EXPECT_EQ(aCompound.NbChildren(), 0);
  }
}

//==================================================================================================
// Edge with degenerate vertex tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, EdgeWithInternalVertex)
{
  TopoDS_Edge anEdge;
  myBuilder.MakeEdge(anEdge);

  TopoDS_Vertex aV1        = makeVertex(0, 0, 0);
  TopoDS_Vertex aV2        = makeVertex(1, 0, 0);
  TopoDS_Vertex aVInternal = makeVertex(0.5, 0, 0);

  myBuilder.Add(anEdge, aV1.Oriented(TopAbs_FORWARD));
  myBuilder.Add(anEdge, aV2.Oriented(TopAbs_REVERSED));
  myBuilder.Add(anEdge, aVInternal.Oriented(TopAbs_INTERNAL));

  EXPECT_EQ(anEdge.NbChildren(), 3);

  int aForward = 0, aReversed = 0, anInternal = 0;
  for (TopoDS_Iterator anIter(anEdge); anIter.More(); anIter.Next())
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
      default:
        break;
    }
  }

  EXPECT_EQ(aForward, 1);
  EXPECT_EQ(aReversed, 1);
  EXPECT_EQ(anInternal, 1);
}

//==================================================================================================
// Shape type conversion tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, TopoDS_Cast_Vertex)
{
  TopoDS_Vertex aVertex = makeVertex(1, 2, 3);
  TopoDS_Shape  aShape  = aVertex;

  // Cast back to vertex
  const TopoDS_Vertex& aCastVertex = TopoDS::Vertex(aShape);

  gp_Pnt aPnt = BRep_Tool::Pnt(aCastVertex);
  EXPECT_NEAR(aPnt.X(), 1.0, 1e-10);
  EXPECT_NEAR(aPnt.Y(), 2.0, 1e-10);
  EXPECT_NEAR(aPnt.Z(), 3.0, 1e-10);
}

TEST_F(TopoDS_BuilderTest, TopoDS_Cast_AllTypes)
{
  // Verify casting works for all types
  TopoDS_Vertex    aVertex = makeVertex(0, 0, 0);
  TopoDS_Edge      anEdge;
  TopoDS_Wire      aWire;
  TopoDS_Face      aFace;
  TopoDS_Shell     aShell;
  TopoDS_Solid     aSolid;
  TopoDS_CompSolid aCompSolid;
  TopoDS_Compound  aCompound;

  myBuilder.MakeEdge(anEdge);
  myBuilder.MakeWire(aWire);
  myBuilder.MakeFace(aFace);
  myBuilder.MakeShell(aShell);
  myBuilder.MakeSolid(aSolid);
  myBuilder.MakeCompSolid(aCompSolid);
  myBuilder.MakeCompound(aCompound);

  TopoDS_Shape aShapeVertex    = aVertex;
  TopoDS_Shape aShapeEdge      = anEdge;
  TopoDS_Shape aShapeWire      = aWire;
  TopoDS_Shape aShapeFace      = aFace;
  TopoDS_Shape aShapeShell     = aShell;
  TopoDS_Shape aShapeSolid     = aSolid;
  TopoDS_Shape aShapeCompSolid = aCompSolid;
  TopoDS_Shape aShapeCompound  = aCompound;

  EXPECT_NO_THROW(TopoDS::Vertex(aShapeVertex));
  EXPECT_NO_THROW(TopoDS::Edge(aShapeEdge));
  EXPECT_NO_THROW(TopoDS::Wire(aShapeWire));
  EXPECT_NO_THROW(TopoDS::Face(aShapeFace));
  EXPECT_NO_THROW(TopoDS::Shell(aShapeShell));
  EXPECT_NO_THROW(TopoDS::Solid(aShapeSolid));
  EXPECT_NO_THROW(TopoDS::CompSolid(aShapeCompSolid));
  EXPECT_NO_THROW(TopoDS::Compound(aShapeCompound));
}

//==================================================================================================
// Builder free flag handling tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, FreeFlag_PreservedAfterAdd)
{
  TopoDS_Compound aParent;
  myBuilder.MakeCompound(aParent);

  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);
  EXPECT_TRUE(aVertex.Free());

  myBuilder.Add(aParent, aVertex);

  // After adding, the vertex should no longer be free
  EXPECT_FALSE(aVertex.Free());
}

TEST_F(TopoDS_BuilderTest, FreeFlag_ManualSetting)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  // Should be free by default
  EXPECT_TRUE(aCompound.Free());

  // Manually set to not free
  aCompound.Free(false);
  EXPECT_FALSE(aCompound.Free());

  // Set back to free
  aCompound.Free(true);
  EXPECT_TRUE(aCompound.Free());
}

//==================================================================================================
// Reversed iteration tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, IteratorOnReversedShape_AllOrientationsComposed)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  // Add vertices with different orientations
  TopoDS_Vertex aV1 = makeVertex(1, 0, 0);
  TopoDS_Vertex aV2 = makeVertex(2, 0, 0);
  TopoDS_Vertex aV3 = makeVertex(3, 0, 0);
  TopoDS_Vertex aV4 = makeVertex(4, 0, 0);

  myBuilder.Add(aCompound, aV1.Oriented(TopAbs_FORWARD));
  myBuilder.Add(aCompound, aV2.Oriented(TopAbs_REVERSED));
  myBuilder.Add(aCompound, aV3.Oriented(TopAbs_INTERNAL));
  myBuilder.Add(aCompound, aV4.Oriented(TopAbs_EXTERNAL));

  TopoDS_Shape aReversedCompound = aCompound.Reversed();

  // With cumulative orientation
  int aForward = 0, aReversed = 0, anInternal = 0, anExternal = 0;
  for (TopoDS_Iterator anIter(aReversedCompound, true, false); anIter.More(); anIter.Next())
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

  // FORWARD -> REVERSED, REVERSED -> FORWARD
  // INTERNAL and EXTERNAL remain unchanged
  EXPECT_EQ(aReversed, 1); // was FORWARD
  EXPECT_EQ(aForward, 1);  // was REVERSED
  EXPECT_EQ(anInternal, 1);
  EXPECT_EQ(anExternal, 1);
}

//==================================================================================================
// Copy constructor and assignment tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, ShapeCopyConstructor)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);
  myBuilder.Add(aCompound, makeVertex(0, 0, 0));

  TopoDS_Shape aCopy(aCompound);

  EXPECT_TRUE(aCopy.IsEqual(aCompound));
  EXPECT_EQ(aCopy.NbChildren(), 1);
}

TEST_F(TopoDS_BuilderTest, ShapeAssignment)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);
  myBuilder.Add(aCompound, makeVertex(0, 0, 0));

  TopoDS_Shape aCopy;
  aCopy = aCompound;

  EXPECT_TRUE(aCopy.IsEqual(aCompound));
  EXPECT_EQ(aCopy.NbChildren(), 1);
}

TEST_F(TopoDS_BuilderTest, ShapeMoveConstructor)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);
  myBuilder.Add(aCompound, makeVertex(0, 0, 0));

  TopoDS_Shape aSource = aCompound;
  TopoDS_Shape aMoved(std::move(aSource));

  EXPECT_EQ(aMoved.NbChildren(), 1);
  EXPECT_TRUE(aSource.IsNull());
}

//==================================================================================================
// Complex topology iteration tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, FaceWithOuterAndInnerWires)
{
  // Create face with outer wire and hole
  TopoDS_Face aFace;
  myBuilder.MakeFace(aFace);

  TopoDS_Wire aOuterWire, anInnerWire;
  myBuilder.MakeWire(aOuterWire);
  myBuilder.MakeWire(anInnerWire);

  // Add outer wire (FORWARD orientation typically)
  myBuilder.Add(aFace, aOuterWire.Oriented(TopAbs_FORWARD));
  // Add inner wire (REVERSED orientation for hole)
  myBuilder.Add(aFace, anInnerWire.Oriented(TopAbs_REVERSED));

  EXPECT_EQ(aFace.NbChildren(), 2);

  // Check orientations
  int aForward = 0, aReversed = 0;
  for (TopoDS_Iterator anIter(aFace); anIter.More(); anIter.Next())
  {
    if (anIter.Value().Orientation() == TopAbs_FORWARD)
      ++aForward;
    else if (anIter.Value().Orientation() == TopAbs_REVERSED)
      ++aReversed;
  }

  EXPECT_EQ(aForward, 1);
  EXPECT_EQ(aReversed, 1);
}

TEST_F(TopoDS_BuilderTest, ShellWithMultipleFaces)
{
  TopoDS_Shell aShell;
  myBuilder.MakeShell(aShell);

  const int aNbFaces = 10;
  for (int i = 0; i < aNbFaces; ++i)
  {
    TopoDS_Face aFace;
    myBuilder.MakeFace(aFace);

    // Alternate orientations
    if (i % 2 == 0)
      myBuilder.Add(aShell, aFace.Oriented(TopAbs_FORWARD));
    else
      myBuilder.Add(aShell, aFace.Oriented(TopAbs_REVERSED));
  }

  EXPECT_EQ(aShell.NbChildren(), aNbFaces);

  int aForward = 0, aReversed = 0;
  for (TopoDS_Iterator anIter(aShell); anIter.More(); anIter.Next())
  {
    if (anIter.Value().Orientation() == TopAbs_FORWARD)
      ++aForward;
    else if (anIter.Value().Orientation() == TopAbs_REVERSED)
      ++aReversed;
  }

  EXPECT_EQ(aForward, aNbFaces / 2);
  EXPECT_EQ(aReversed, aNbFaces / 2);
}

//==================================================================================================
// Location with rotation tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, LocationRotation_ZAxis)
{
  TopoDS_Vertex aVertex = makeVertex(10, 0, 0);

  // Rotate 90 degrees around Z axis
  gp_Trsf aTrsf;
  aTrsf.SetRotation(gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 2);
  TopLoc_Location aLoc(aTrsf);

  TopoDS_Shape aMoved = aVertex.Moved(aLoc);

  gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(aMoved));
  EXPECT_NEAR(aPnt.X(), 0.0, 1e-10);
  EXPECT_NEAR(aPnt.Y(), 10.0, 1e-10);
  EXPECT_NEAR(aPnt.Z(), 0.0, 1e-10);
}

TEST_F(TopoDS_BuilderTest, LocationScale_Uniform)
{
  TopoDS_Vertex aVertex = makeVertex(1, 2, 3);

  // Scale by factor of 2
  gp_Trsf aTrsf;
  aTrsf.SetScale(gp_Pnt(0, 0, 0), 2.0);
  TopLoc_Location aLoc(aTrsf);

  TopoDS_Shape aMoved = aVertex.Moved(aLoc);

  gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(aMoved));
  EXPECT_NEAR(aPnt.X(), 2.0, 1e-10);
  EXPECT_NEAR(aPnt.Y(), 4.0, 1e-10);
  EXPECT_NEAR(aPnt.Z(), 6.0, 1e-10);
}

//==================================================================================================
// Combined location and orientation tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, LocationAndOrientation_Combined)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);
  myBuilder.Add(aCompound, aVertex);

  // Apply location to compound
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(100, 0, 0));
  TopLoc_Location aLoc(aTrsf);
  TopoDS_Shape    aLocatedCompound = aCompound.Located(aLoc);

  // Reverse the located compound
  TopoDS_Shape aReversedLocatedCompound = aLocatedCompound.Reversed();

  // Iterate with cumulative orientation and location
  TopoDS_Iterator anIter(aReversedLocatedCompound, true, true);
  EXPECT_TRUE(anIter.More());
  EXPECT_EQ(anIter.Value().Orientation(), TopAbs_REVERSED);
  EXPECT_FALSE(anIter.Value().Location().IsIdentity());
}

//==================================================================================================
// Convex flag tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, ConvexFlag_SetAndCheck)
{
  TopoDS_Face aFace;
  myBuilder.MakeFace(aFace);

  // Initially empty face is not convex
  EXPECT_FALSE(aFace.Convex());

  // Set convex flag
  aFace.Convex(true);
  EXPECT_TRUE(aFace.Convex());

  // Clear convex flag
  aFace.Convex(false);
  EXPECT_FALSE(aFace.Convex());
}

//==================================================================================================
// Closed flag tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, ClosedFlag_SetAndCheck)
{
  TopoDS_Wire aWire;
  myBuilder.MakeWire(aWire);

  // Initially should not be closed
  EXPECT_FALSE(aWire.Closed());

  // Set closed flag
  aWire.Closed(true);
  EXPECT_TRUE(aWire.Closed());

  // Clear closed flag
  aWire.Closed(false);
  EXPECT_FALSE(aWire.Closed());
}

//==================================================================================================
// Orientable flag tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, OrientableFlag_Edge)
{
  TopoDS_Edge anEdge;
  myBuilder.MakeEdge(anEdge);

  // Edge should be orientable by default
  EXPECT_TRUE(anEdge.Orientable());

  // Can change orientable flag
  anEdge.Orientable(false);
  EXPECT_FALSE(anEdge.Orientable());

  anEdge.Orientable(true);
  EXPECT_TRUE(anEdge.Orientable());
}

TEST_F(TopoDS_BuilderTest, OrientableFlag_Face)
{
  TopoDS_Face aFace;
  myBuilder.MakeFace(aFace);

  // Face should be orientable by default
  EXPECT_TRUE(aFace.Orientable());
}

TEST_F(TopoDS_BuilderTest, OrientableFlag_Shell)
{
  TopoDS_Shell aShell;
  myBuilder.MakeShell(aShell);

  // Shell should be orientable by default
  EXPECT_TRUE(aShell.Orientable());
}

//==================================================================================================
// Shape type tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, ShapeType_AllTypes)
{
  TopoDS_Vertex    aVertex = makeVertex(0, 0, 0);
  TopoDS_Edge      anEdge;
  TopoDS_Wire      aWire;
  TopoDS_Face      aFace;
  TopoDS_Shell     aShell;
  TopoDS_Solid     aSolid;
  TopoDS_CompSolid aCompSolid;
  TopoDS_Compound  aCompound;

  myBuilder.MakeEdge(anEdge);
  myBuilder.MakeWire(aWire);
  myBuilder.MakeFace(aFace);
  myBuilder.MakeShell(aShell);
  myBuilder.MakeSolid(aSolid);
  myBuilder.MakeCompSolid(aCompSolid);
  myBuilder.MakeCompound(aCompound);

  EXPECT_EQ(aVertex.ShapeType(), TopAbs_VERTEX);
  EXPECT_EQ(anEdge.ShapeType(), TopAbs_EDGE);
  EXPECT_EQ(aWire.ShapeType(), TopAbs_WIRE);
  EXPECT_EQ(aFace.ShapeType(), TopAbs_FACE);
  EXPECT_EQ(aShell.ShapeType(), TopAbs_SHELL);
  EXPECT_EQ(aSolid.ShapeType(), TopAbs_SOLID);
  EXPECT_EQ(aCompSolid.ShapeType(), TopAbs_COMPSOLID);
  EXPECT_EQ(aCompound.ShapeType(), TopAbs_COMPOUND);
}

//==================================================================================================
// Remove with location tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, RemoveWithLocation_SameLocation)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(10, 0, 0));
  TopLoc_Location aLoc(aTrsf);

  TopoDS_Shape aLocatedVertex = aVertex.Located(aLoc);
  myBuilder.Add(aCompound, aLocatedVertex);

  EXPECT_EQ(aCompound.NbChildren(), 1);

  // Remove with same location should work
  myBuilder.Remove(aCompound, aLocatedVertex);
  EXPECT_EQ(aCompound.NbChildren(), 0);
}

TEST_F(TopoDS_BuilderTest, RemoveWithLocation_DifferentLocation)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);

  gp_Trsf aTrsf1;
  aTrsf1.SetTranslation(gp_Vec(10, 0, 0));
  TopLoc_Location aLoc1(aTrsf1);

  gp_Trsf aTrsf2;
  aTrsf2.SetTranslation(gp_Vec(20, 0, 0));
  TopLoc_Location aLoc2(aTrsf2);

  myBuilder.Add(aCompound, aVertex.Located(aLoc1));
  EXPECT_EQ(aCompound.NbChildren(), 1);

  // Remove with different location should not match
  myBuilder.Remove(aCompound, aVertex.Located(aLoc2));
  EXPECT_EQ(aCompound.NbChildren(), 1);

  // Remove with correct location should work
  myBuilder.Remove(aCompound, aVertex.Located(aLoc1));
  EXPECT_EQ(aCompound.NbChildren(), 0);
}

//==================================================================================================
// Multiple parents sharing child tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, MultipleParents_SharedChild)
{
  TopoDS_Compound aParent1, aParent2, aParent3;
  myBuilder.MakeCompound(aParent1);
  myBuilder.MakeCompound(aParent2);
  myBuilder.MakeCompound(aParent3);

  TopoDS_Vertex aSharedVertex = makeVertex(0, 0, 0);

  myBuilder.Add(aParent1, aSharedVertex);
  myBuilder.Add(aParent2, aSharedVertex);
  myBuilder.Add(aParent3, aSharedVertex);

  // Each parent should have 1 child
  EXPECT_EQ(aParent1.NbChildren(), 1);
  EXPECT_EQ(aParent2.NbChildren(), 1);
  EXPECT_EQ(aParent3.NbChildren(), 1);

  // All children should share the same TShape
  TopoDS_Iterator anIter1(aParent1);
  TopoDS_Iterator anIter2(aParent2);
  TopoDS_Iterator anIter3(aParent3);

  EXPECT_TRUE(anIter1.Value().IsPartner(anIter2.Value()));
  EXPECT_TRUE(anIter2.Value().IsPartner(anIter3.Value()));
}

//==================================================================================================
// Reverse in place tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, Reverse_InPlace)
{
  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);
  EXPECT_EQ(aVertex.Orientation(), TopAbs_FORWARD);

  aVertex.Reverse();
  EXPECT_EQ(aVertex.Orientation(), TopAbs_REVERSED);

  aVertex.Reverse();
  EXPECT_EQ(aVertex.Orientation(), TopAbs_FORWARD);
}

TEST_F(TopoDS_BuilderTest, Complement_InPlace)
{
  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);
  EXPECT_EQ(aVertex.Orientation(), TopAbs_FORWARD);

  aVertex.Complement();
  EXPECT_EQ(aVertex.Orientation(), TopAbs_REVERSED);

  aVertex.Complement();
  EXPECT_EQ(aVertex.Orientation(), TopAbs_FORWARD);
}

//==================================================================================================
// Build realistic topology tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, BuildWireManually)
{
  // Create 4 edges forming a square
  TopoDS_Edge anE1, anE2, anE3, anE4;
  myBuilder.MakeEdge(anE1);
  myBuilder.MakeEdge(anE2);
  myBuilder.MakeEdge(anE3);
  myBuilder.MakeEdge(anE4);

  // Create vertices
  TopoDS_Vertex aV1 = makeVertex(0, 0, 0);
  TopoDS_Vertex aV2 = makeVertex(10, 0, 0);
  TopoDS_Vertex aV3 = makeVertex(10, 10, 0);
  TopoDS_Vertex aV4 = makeVertex(0, 10, 0);

  // Add vertices to edges
  myBuilder.Add(anE1, aV1.Oriented(TopAbs_FORWARD));
  myBuilder.Add(anE1, aV2.Oriented(TopAbs_REVERSED));

  myBuilder.Add(anE2, aV2.Oriented(TopAbs_FORWARD));
  myBuilder.Add(anE2, aV3.Oriented(TopAbs_REVERSED));

  myBuilder.Add(anE3, aV3.Oriented(TopAbs_FORWARD));
  myBuilder.Add(anE3, aV4.Oriented(TopAbs_REVERSED));

  myBuilder.Add(anE4, aV4.Oriented(TopAbs_FORWARD));
  myBuilder.Add(anE4, aV1.Oriented(TopAbs_REVERSED));

  // Each edge should have 2 vertices
  EXPECT_EQ(anE1.NbChildren(), 2);
  EXPECT_EQ(anE2.NbChildren(), 2);
  EXPECT_EQ(anE3.NbChildren(), 2);
  EXPECT_EQ(anE4.NbChildren(), 2);

  // Build wire
  TopoDS_Wire aWire;
  myBuilder.MakeWire(aWire);
  myBuilder.Add(aWire, anE1);
  myBuilder.Add(aWire, anE2);
  myBuilder.Add(aWire, anE3);
  myBuilder.Add(aWire, anE4);

  EXPECT_EQ(aWire.NbChildren(), 4);
}

TEST_F(TopoDS_BuilderTest, BuildShellManually)
{
  // Create faces
  TopoDS_Face aF1, aF2, aF3;
  myBuilder.MakeFace(aF1);
  myBuilder.MakeFace(aF2);
  myBuilder.MakeFace(aF3);

  // Create wires for faces
  TopoDS_Wire aW1, aW2, aW3;
  myBuilder.MakeWire(aW1);
  myBuilder.MakeWire(aW2);
  myBuilder.MakeWire(aW3);

  // Add wires to faces
  myBuilder.Add(aF1, aW1);
  myBuilder.Add(aF2, aW2);
  myBuilder.Add(aF3, aW3);

  // Build shell
  TopoDS_Shell aShell;
  myBuilder.MakeShell(aShell);
  myBuilder.Add(aShell, aF1);
  myBuilder.Add(aShell, aF2);
  myBuilder.Add(aShell, aF3);

  EXPECT_EQ(aShell.NbChildren(), 3);
}

TEST_F(TopoDS_BuilderTest, BuildSolidManually)
{
  // Create shell
  TopoDS_Shell aShell;
  myBuilder.MakeShell(aShell);

  // Add some faces to shell
  for (int i = 0; i < 6; ++i)
  {
    TopoDS_Face aFace;
    myBuilder.MakeFace(aFace);
    myBuilder.Add(aShell, aFace);
  }

  EXPECT_EQ(aShell.NbChildren(), 6);

  // Build solid
  TopoDS_Solid aSolid;
  myBuilder.MakeSolid(aSolid);
  myBuilder.Add(aSolid, aShell);

  EXPECT_EQ(aSolid.NbChildren(), 1);
}

//==================================================================================================
// Location identity tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, LocationIdentity_IsIdentity)
{
  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);

  // Default location should be identity
  EXPECT_TRUE(aVertex.Location().IsIdentity());
}

TEST_F(TopoDS_BuilderTest, LocationIdentity_AfterLocated)
{
  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(10, 0, 0));
  TopLoc_Location aLoc(aTrsf);

  TopoDS_Shape aLocated = aVertex.Located(aLoc);
  EXPECT_FALSE(aLocated.Location().IsIdentity());

  // Located with identity should give identity
  TopoDS_Shape aIdentityLocated = aVertex.Located(TopLoc_Location());
  EXPECT_TRUE(aIdentityLocated.Location().IsIdentity());
}

//==================================================================================================
// TShape modification tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, TShapeAccess)
{
  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);

  // TShape should not be null
  EXPECT_FALSE(aVertex.TShape().IsNull());

  // Reversed shape should have same TShape
  TopoDS_Shape aReversed = aVertex.Reversed();
  EXPECT_TRUE(aVertex.TShape() == aReversed.TShape());
}

//==================================================================================================
// Compound of compounds tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, CompoundOfCompounds_MultiLevel)
{
  TopoDS_Compound aRoot;
  myBuilder.MakeCompound(aRoot);

  const int aNbSubCompounds        = 5;
  const int aNbVerticesPerCompound = 3;

  for (int i = 0; i < aNbSubCompounds; ++i)
  {
    TopoDS_Compound aSubCompound;
    myBuilder.MakeCompound(aSubCompound);

    for (int j = 0; j < aNbVerticesPerCompound; ++j)
    {
      myBuilder.Add(aSubCompound, makeVertex(i * 10 + j, 0, 0));
    }

    myBuilder.Add(aRoot, aSubCompound);
  }

  EXPECT_EQ(aRoot.NbChildren(), aNbSubCompounds);

  // Each sub-compound should have the correct number of vertices
  for (TopoDS_Iterator anIter(aRoot); anIter.More(); anIter.Next())
  {
    EXPECT_EQ(anIter.Value().ShapeType(), TopAbs_COMPOUND);
    EXPECT_EQ(anIter.Value().NbChildren(), aNbVerticesPerCompound);
  }
}

//==================================================================================================
// Null shape behavior tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, NullShape_IsNull)
{
  TopoDS_Shape aNullShape;
  EXPECT_TRUE(aNullShape.IsNull());
}

TEST_F(TopoDS_BuilderTest, NullShape_OrientationDefault)
{
  TopoDS_Shape aNullShape;
  EXPECT_TRUE(aNullShape.IsNull());

  // Default-constructed shape has TopAbs_EXTERNAL orientation (value 3)
  EXPECT_EQ(aNullShape.Orientation(), TopAbs_EXTERNAL);
}

TEST_F(TopoDS_BuilderTest, NullShape_Location)
{
  TopoDS_Shape aNullShape;
  EXPECT_TRUE(aNullShape.IsNull());

  // Location is stored in the shape itself, so it's safe to call
  EXPECT_TRUE(aNullShape.Location().IsIdentity());
}

//==================================================================================================
// Edge degenerate tests
//==================================================================================================

TEST_F(TopoDS_BuilderTest, Edge_DegenerateFlag)
{
  TopoDS_Edge anEdge;
  myBuilder.MakeEdge(anEdge);

  // Check degenerate flag (stored in edge)
  EXPECT_FALSE(BRep_Tool::Degenerated(anEdge));
}

//==================================================================================================
// Same shape different orientations in parent
//==================================================================================================

TEST_F(TopoDS_BuilderTest, SameShapeFourOrientations)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex = makeVertex(0, 0, 0);

  myBuilder.Add(aCompound, aVertex.Oriented(TopAbs_FORWARD));
  myBuilder.Add(aCompound, aVertex.Oriented(TopAbs_REVERSED));
  myBuilder.Add(aCompound, aVertex.Oriented(TopAbs_INTERNAL));
  myBuilder.Add(aCompound, aVertex.Oriented(TopAbs_EXTERNAL));

  EXPECT_EQ(aCompound.NbChildren(), 4);

  // Verify all share the same TShape
  NCollection_Vector<TopoDS_Shape> aChildren;
  for (TopoDS_Iterator anIter(aCompound); anIter.More(); anIter.Next())
  {
    aChildren.Append(anIter.Value());
  }

  for (int i = 0; i < aChildren.Size(); ++i)
  {
    for (int j = i + 1; j < aChildren.Size(); ++j)
    {
      EXPECT_TRUE(aChildren.Value(i).IsPartner(aChildren.Value(j)));
    }
  }
}

//==================================================================================================
// Complex removal patterns
//==================================================================================================

TEST_F(TopoDS_BuilderTest, RemovePattern_FromMiddle)
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

  // Remove elements from the middle (indices 3, 4, 5, 6)
  for (int i = 3; i <= 6; ++i)
  {
    myBuilder.Remove(aCompound, aVertices.Value(i));
  }

  EXPECT_EQ(aCompound.NbChildren(), 6);

  // Verify remaining are correct
  int aCount = 0;
  for (TopoDS_Iterator anIter(aCompound); anIter.More(); anIter.Next())
  {
    gp_Pnt aP = BRep_Tool::Pnt(TopoDS::Vertex(anIter.Value()));
    // Should be 0, 1, 2, 7, 8, 9
    EXPECT_TRUE(aP.X() < 3.0 || aP.X() > 6.0);
    ++aCount;
  }
  EXPECT_EQ(aCount, 6);
}

TEST_F(TopoDS_BuilderTest, RemovePattern_EveryThird)
{
  TopoDS_Compound aCompound;
  myBuilder.MakeCompound(aCompound);

  NCollection_Vector<TopoDS_Vertex> aVertices;
  for (int i = 0; i < 12; ++i)
  {
    TopoDS_Vertex aV = makeVertex(i, 0, 0);
    aVertices.Append(aV);
    myBuilder.Add(aCompound, aV);
  }

  // Remove every third element (0, 3, 6, 9)
  for (int i = 0; i < 12; i += 3)
  {
    myBuilder.Remove(aCompound, aVertices.Value(i));
  }

  EXPECT_EQ(aCompound.NbChildren(), 8);
}

//==================================================================================================
// Box topology exploration
//==================================================================================================

TEST_F(TopoDS_BuilderTest, BoxTopology_FullExploration)
{
  TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10, 20, 30).Shape();

  // Solid -> Shell -> Faces
  TopoDS_Iterator aSolidIter(aBox);
  ASSERT_TRUE(aSolidIter.More());
  const TopoDS_Shape& aShell = aSolidIter.Value();
  EXPECT_EQ(aShell.ShapeType(), TopAbs_SHELL);

  // Count faces in shell
  int aFaceCount = 0;
  for (TopoDS_Iterator aShellIter(aShell); aShellIter.More(); aShellIter.Next())
  {
    EXPECT_EQ(aShellIter.Value().ShapeType(), TopAbs_FACE);
    ++aFaceCount;

    // Each face should have at least one wire
    const TopoDS_Shape& aFace      = aShellIter.Value();
    int                 aWireCount = 0;
    for (TopoDS_Iterator aFaceIter(aFace); aFaceIter.More(); aFaceIter.Next())
    {
      EXPECT_EQ(aFaceIter.Value().ShapeType(), TopAbs_WIRE);
      ++aWireCount;

      // Each wire should have edges
      const TopoDS_Shape& aWire       = aFaceIter.Value();
      int                 anEdgeCount = 0;
      for (TopoDS_Iterator aWireIter(aWire); aWireIter.More(); aWireIter.Next())
      {
        EXPECT_EQ(aWireIter.Value().ShapeType(), TopAbs_EDGE);
        ++anEdgeCount;
      }
      EXPECT_EQ(anEdgeCount, 4); // Each face of box has 4 edges
    }
    EXPECT_EQ(aWireCount, 1); // Each face has 1 wire (no holes)
  }
  EXPECT_EQ(aFaceCount, 6); // Box has 6 faces
}
