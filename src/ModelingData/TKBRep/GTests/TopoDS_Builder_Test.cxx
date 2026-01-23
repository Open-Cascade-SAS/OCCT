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

#include <gtest/gtest.h>

#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Builder.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <gp_Pnt.hxx>

//==================================================================================================
// Test TopoDS_Builder::MakeWire
//==================================================================================================

TEST(TopoDS_Builder_Test, MakeWire)
{
  TopoDS_Builder aBuilder;
  TopoDS_Wire    aWire;

  aBuilder.MakeWire(aWire);

  EXPECT_FALSE(aWire.IsNull()) << "Wire should not be null after MakeWire";
  EXPECT_EQ(aWire.ShapeType(), TopAbs_WIRE) << "Shape type should be WIRE";
  EXPECT_TRUE(aWire.Free()) << "Newly created wire should be free";
}

//==================================================================================================
// Test TopoDS_Builder::MakeShell
//==================================================================================================

TEST(TopoDS_Builder_Test, MakeShell)
{
  TopoDS_Builder aBuilder;
  TopoDS_Shell   aShell;

  aBuilder.MakeShell(aShell);

  EXPECT_FALSE(aShell.IsNull()) << "Shell should not be null";
  EXPECT_EQ(aShell.ShapeType(), TopAbs_SHELL);
}

//==================================================================================================
// Test TopoDS_Builder::MakeSolid
//==================================================================================================

TEST(TopoDS_Builder_Test, MakeSolid)
{
  TopoDS_Builder aBuilder;
  TopoDS_Solid   aSolid;

  aBuilder.MakeSolid(aSolid);

  EXPECT_FALSE(aSolid.IsNull()) << "Solid should not be null";
  EXPECT_EQ(aSolid.ShapeType(), TopAbs_SOLID);
}

//==================================================================================================
// Test TopoDS_Builder::MakeCompSolid
//==================================================================================================

TEST(TopoDS_Builder_Test, MakeCompSolid)
{
  TopoDS_Builder   aBuilder;
  TopoDS_CompSolid aCompSolid;

  aBuilder.MakeCompSolid(aCompSolid);

  EXPECT_FALSE(aCompSolid.IsNull()) << "CompSolid should not be null";
  EXPECT_EQ(aCompSolid.ShapeType(), TopAbs_COMPSOLID);
}

//==================================================================================================
// Test TopoDS_Builder::MakeCompound
//==================================================================================================

TEST(TopoDS_Builder_Test, MakeCompound)
{
  TopoDS_Builder  aBuilder;
  TopoDS_Compound aCompound;

  aBuilder.MakeCompound(aCompound);

  EXPECT_FALSE(aCompound.IsNull()) << "Compound should not be null";
  EXPECT_EQ(aCompound.ShapeType(), TopAbs_COMPOUND);
}

//==================================================================================================
// Test TopoDS_Builder::Add - add vertices to compound
//==================================================================================================

TEST(TopoDS_Builder_Test, Add_VerticesToCompound)
{
  TopoDS_Builder  aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  // Create and add vertices
  TopoDS_Vertex aV1 = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));
  TopoDS_Vertex aV2 = BRepBuilderAPI_MakeVertex(gp_Pnt(1, 0, 0));
  TopoDS_Vertex aV3 = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 1, 0));

  aBuilder.Add(aCompound, aV1);
  aBuilder.Add(aCompound, aV2);
  aBuilder.Add(aCompound, aV3);

  // Count children using iterator
  int aCount = 0;
  for (TopoDS_Iterator anIt(aCompound); anIt.More(); anIt.Next())
  {
    ++aCount;
  }

  EXPECT_EQ(aCount, 3) << "Compound should have 3 vertices";
}

//==================================================================================================
// Test TopoDS_Builder::Add - add edges to wire
//==================================================================================================

TEST(TopoDS_Builder_Test, Add_EdgesToWire)
{
  TopoDS_Builder aBuilder;
  TopoDS_Wire    aWire;
  aBuilder.MakeWire(aWire);

  // Create edges forming a triangle
  TopoDS_Edge aE1 = BRepBuilderAPI_MakeEdge(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0));
  TopoDS_Edge aE2 = BRepBuilderAPI_MakeEdge(gp_Pnt(1, 0, 0), gp_Pnt(0.5, 1, 0));
  TopoDS_Edge aE3 = BRepBuilderAPI_MakeEdge(gp_Pnt(0.5, 1, 0), gp_Pnt(0, 0, 0));

  aBuilder.Add(aWire, aE1);
  aBuilder.Add(aWire, aE2);
  aBuilder.Add(aWire, aE3);

  // Count children
  int aCount = 0;
  for (TopoDS_Iterator anIt(aWire); anIt.More(); anIt.Next())
  {
    EXPECT_EQ(anIt.Value().ShapeType(), TopAbs_EDGE);
    ++aCount;
  }

  EXPECT_EQ(aCount, 3) << "Wire should have 3 edges";
}

//==================================================================================================
// Test TopoDS_Builder::Remove
//==================================================================================================

TEST(TopoDS_Builder_Test, Remove_FromCompound)
{
  TopoDS_Builder  aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  // Add vertices
  TopoDS_Vertex aV1 = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));
  TopoDS_Vertex aV2 = BRepBuilderAPI_MakeVertex(gp_Pnt(1, 0, 0));
  TopoDS_Vertex aV3 = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 1, 0));

  aBuilder.Add(aCompound, aV1);
  aBuilder.Add(aCompound, aV2);
  aBuilder.Add(aCompound, aV3);

  // Remove middle vertex
  aBuilder.Remove(aCompound, aV2);

  // Count remaining children
  int aCount = 0;
  for (TopoDS_Iterator anIt(aCompound); anIt.More(); anIt.Next())
  {
    ++aCount;
  }

  EXPECT_EQ(aCount, 2) << "Compound should have 2 vertices after removal";
}

//==================================================================================================
// Test adding many shapes to compound
//==================================================================================================

TEST(TopoDS_Builder_Test, Add_ManyShapes)
{
  TopoDS_Builder  aBuilder;
  TopoDS_Compound aCompound;

  aBuilder.MakeCompound(aCompound);

  // Add 500 vertices
  for (int i = 0; i < 500; ++i)
  {
    TopoDS_Vertex aV = BRepBuilderAPI_MakeVertex(gp_Pnt(i, 0, 0));
    aBuilder.Add(aCompound, aV);
  }

  // Verify count
  int aCount = 0;
  for (TopoDS_Iterator anIt(aCompound); anIt.More(); anIt.Next())
  {
    ++aCount;
  }

  EXPECT_EQ(aCount, 500) << "Compound should have 500 vertices";
}

//==================================================================================================
// Test TShape flags through shape interface
//==================================================================================================

TEST(TopoDS_Builder_Test, TShapeFlags)
{
  TopoDS_Builder  aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  // Test initial flags
  EXPECT_TRUE(aCompound.Free()) << "New shape should be free";
  EXPECT_TRUE(aCompound.Modified()) << "New shape should be modified";

  // Test setting flags
  aCompound.Checked(true);
  EXPECT_TRUE(aCompound.Checked());

  aCompound.Closed(true);
  EXPECT_TRUE(aCompound.Closed());

  aCompound.Infinite(true);
  EXPECT_TRUE(aCompound.Infinite());

  aCompound.Convex(true);
  EXPECT_TRUE(aCompound.Convex());
}

//==================================================================================================
// Test TopoDS_Builder::Remove first child
//==================================================================================================

TEST(TopoDS_Builder_Test, Remove_FirstChild)
{
  TopoDS_Builder  aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aV1 = BRepBuilderAPI_MakeVertex(gp_Pnt(1, 0, 0));
  TopoDS_Vertex aV2 = BRepBuilderAPI_MakeVertex(gp_Pnt(2, 0, 0));
  TopoDS_Vertex aV3 = BRepBuilderAPI_MakeVertex(gp_Pnt(3, 0, 0));

  aBuilder.Add(aCompound, aV1);
  aBuilder.Add(aCompound, aV2);
  aBuilder.Add(aCompound, aV3);

  // Remove first vertex
  aBuilder.Remove(aCompound, aV1);

  // Should have 2 remaining
  int aCount = 0;
  for (TopoDS_Iterator anIt(aCompound); anIt.More(); anIt.Next())
  {
    ++aCount;
  }
  EXPECT_EQ(aCount, 2);
}

//==================================================================================================
// Test TopoDS_Builder::Remove last child
//==================================================================================================

TEST(TopoDS_Builder_Test, Remove_LastChild)
{
  TopoDS_Builder  aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aV1 = BRepBuilderAPI_MakeVertex(gp_Pnt(1, 0, 0));
  TopoDS_Vertex aV2 = BRepBuilderAPI_MakeVertex(gp_Pnt(2, 0, 0));
  TopoDS_Vertex aV3 = BRepBuilderAPI_MakeVertex(gp_Pnt(3, 0, 0));

  aBuilder.Add(aCompound, aV1);
  aBuilder.Add(aCompound, aV2);
  aBuilder.Add(aCompound, aV3);

  // Remove last vertex
  aBuilder.Remove(aCompound, aV3);

  // Should have 2 remaining
  int aCount = 0;
  for (TopoDS_Iterator anIt(aCompound); anIt.More(); anIt.Next())
  {
    ++aCount;
  }
  EXPECT_EQ(aCount, 2);
}

//==================================================================================================
// Test TopoDS_Builder::Remove all children one by one
//==================================================================================================

TEST(TopoDS_Builder_Test, Remove_AllChildren)
{
  TopoDS_Builder  aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aV1 = BRepBuilderAPI_MakeVertex(gp_Pnt(1, 0, 0));
  TopoDS_Vertex aV2 = BRepBuilderAPI_MakeVertex(gp_Pnt(2, 0, 0));
  TopoDS_Vertex aV3 = BRepBuilderAPI_MakeVertex(gp_Pnt(3, 0, 0));

  aBuilder.Add(aCompound, aV1);
  aBuilder.Add(aCompound, aV2);
  aBuilder.Add(aCompound, aV3);

  // Remove all
  aBuilder.Remove(aCompound, aV1);
  aBuilder.Remove(aCompound, aV2);
  aBuilder.Remove(aCompound, aV3);

  // Should be empty
  TopoDS_Iterator anIt(aCompound);
  EXPECT_FALSE(anIt.More()) << "Compound should be empty after removing all children";
}

//==================================================================================================
// Test TopoDS_Builder::Remove from wire
//==================================================================================================

TEST(TopoDS_Builder_Test, Remove_FromWire)
{
  TopoDS_Builder aBuilder;
  TopoDS_Wire    aWire;
  aBuilder.MakeWire(aWire);

  TopoDS_Edge aE1 = BRepBuilderAPI_MakeEdge(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0));
  TopoDS_Edge aE2 = BRepBuilderAPI_MakeEdge(gp_Pnt(1, 0, 0), gp_Pnt(1, 1, 0));
  TopoDS_Edge aE3 = BRepBuilderAPI_MakeEdge(gp_Pnt(1, 1, 0), gp_Pnt(0, 0, 0));

  aBuilder.Add(aWire, aE1);
  aBuilder.Add(aWire, aE2);
  aBuilder.Add(aWire, aE3);

  // Remove middle edge
  aBuilder.Remove(aWire, aE2);

  // Should have 2 edges
  int aCount = 0;
  for (TopoDS_Iterator anIt(aWire); anIt.More(); anIt.Next())
  {
    ++aCount;
  }
  EXPECT_EQ(aCount, 2);
}

//==================================================================================================
// Test TopoDS_Builder with nested compounds
//==================================================================================================

TEST(TopoDS_Builder_Test, NestedCompounds)
{
  TopoDS_Builder  aBuilder;
  TopoDS_Compound aOuter, aInner;
  aBuilder.MakeCompound(aOuter);
  aBuilder.MakeCompound(aInner);

  // Add vertices to inner
  aBuilder.Add(aInner, BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0)));
  aBuilder.Add(aInner, BRepBuilderAPI_MakeVertex(gp_Pnt(1, 0, 0)));

  // Add inner to outer
  aBuilder.Add(aOuter, aInner);

  // Outer should have 1 child (the inner compound)
  int aOuterCount = 0;
  for (TopoDS_Iterator anIt(aOuter); anIt.More(); anIt.Next())
  {
    EXPECT_EQ(anIt.Value().ShapeType(), TopAbs_COMPOUND);
    ++aOuterCount;
  }
  EXPECT_EQ(aOuterCount, 1);

  // Inner should still have 2 vertices
  EXPECT_EQ(aInner.TShape()->NbChildren(), 2);
}

//==================================================================================================
// Test TopoDS_Builder::Add sets Modified flag
//==================================================================================================

TEST(TopoDS_Builder_Test, Add_SetsModifiedFlag)
{
  TopoDS_Builder  aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  // Clear modified flag
  aCompound.TShape()->Modified(false);
  EXPECT_FALSE(aCompound.Modified());

  // Add should set modified
  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0)));
  EXPECT_TRUE(aCompound.Modified()) << "Add should set Modified flag";
}

//==================================================================================================
// Test TopoDS_Builder::Remove sets Modified flag
//==================================================================================================

TEST(TopoDS_Builder_Test, Remove_SetsModifiedFlag)
{
  TopoDS_Builder  aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aV = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));
  aBuilder.Add(aCompound, aV);

  // Clear modified flag
  aCompound.TShape()->Modified(false);
  EXPECT_FALSE(aCompound.Modified());

  // Remove should set modified
  aBuilder.Remove(aCompound, aV);
  EXPECT_TRUE(aCompound.Modified()) << "Remove should set Modified flag";
}

//==================================================================================================
// Test all shape types
//==================================================================================================

TEST(TopoDS_Builder_Test, AllTypes)
{
  TopoDS_Builder aBuilder;

  // Wire
  TopoDS_Wire aWire;
  aBuilder.MakeWire(aWire);
  EXPECT_FALSE(aWire.IsNull());
  EXPECT_EQ(aWire.ShapeType(), TopAbs_WIRE);
  EXPECT_TRUE(aWire.Free());

  // Shell
  TopoDS_Shell aShell;
  aBuilder.MakeShell(aShell);
  EXPECT_FALSE(aShell.IsNull());
  EXPECT_EQ(aShell.ShapeType(), TopAbs_SHELL);
  EXPECT_TRUE(aShell.Free());

  // Solid
  TopoDS_Solid aSolid;
  aBuilder.MakeSolid(aSolid);
  EXPECT_FALSE(aSolid.IsNull());
  EXPECT_EQ(aSolid.ShapeType(), TopAbs_SOLID);
  EXPECT_TRUE(aSolid.Free());

  // CompSolid
  TopoDS_CompSolid aCompSolid;
  aBuilder.MakeCompSolid(aCompSolid);
  EXPECT_FALSE(aCompSolid.IsNull());
  EXPECT_EQ(aCompSolid.ShapeType(), TopAbs_COMPSOLID);
  EXPECT_TRUE(aCompSolid.Free());

  // Compound
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  EXPECT_FALSE(aCompound.IsNull());
  EXPECT_EQ(aCompound.ShapeType(), TopAbs_COMPOUND);
  EXPECT_TRUE(aCompound.Free());
}

//==================================================================================================
// Test mixed shapes in compound
//==================================================================================================

TEST(TopoDS_Builder_Test, MixedShapesInCompound)
{
  TopoDS_Builder  aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  // Add different shape types
  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0)));
  aBuilder.Add(aCompound, BRepBuilderAPI_MakeEdge(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0)));

  TopoDS_Wire aWire;
  aBuilder.MakeWire(aWire);
  aBuilder.Add(aCompound, aWire);

  TopoDS_Shell aShell;
  aBuilder.MakeShell(aShell);
  aBuilder.Add(aCompound, aShell);

  TopoDS_Compound aInner;
  aBuilder.MakeCompound(aInner);
  aBuilder.Add(aCompound, aInner);

  EXPECT_EQ(aCompound.TShape()->NbChildren(), 5);

  // Verify types via iteration
  int aVertexCount = 0, aEdgeCount = 0, aWireCount = 0, aShellCount = 0, aCompoundCount = 0;
  for (TopoDS_Iterator anIt(aCompound); anIt.More(); anIt.Next())
  {
    switch (anIt.Value().ShapeType())
    {
      case TopAbs_VERTEX:
        ++aVertexCount;
        break;
      case TopAbs_EDGE:
        ++aEdgeCount;
        break;
      case TopAbs_WIRE:
        ++aWireCount;
        break;
      case TopAbs_SHELL:
        ++aShellCount;
        break;
      case TopAbs_COMPOUND:
        ++aCompoundCount;
        break;
      default:
        break;
    }
  }

  EXPECT_EQ(aVertexCount, 1);
  EXPECT_EQ(aEdgeCount, 1);
  EXPECT_EQ(aWireCount, 1);
  EXPECT_EQ(aShellCount, 1);
  EXPECT_EQ(aCompoundCount, 1);
}
