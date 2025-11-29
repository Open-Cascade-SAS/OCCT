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
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <gp_Pnt.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

#include <gtest/gtest.h>

//==================================================================================================
// Basic empty/null shape tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, OCC30708_1_InitializeWithNullShape)
{
  TopoDS_Iterator it;
  TopoDS_Shape    empty;

  // Should not throw exception when initializing with null shape
  EXPECT_NO_THROW(it.Initialize(empty));

  // More() should return false on null shape
  EXPECT_FALSE(it.More());
}

TEST(TopoDS_Iterator_Test, DefaultConstructor_MoreReturnsFalse)
{
  TopoDS_Iterator anIter;
  EXPECT_FALSE(anIter.More());
}

TEST(TopoDS_Iterator_Test, EmptyVertex_NoChildren)
{
  TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));

  TopoDS_Iterator anIter(aVertex);
  EXPECT_FALSE(anIter.More());
}

TEST(TopoDS_Iterator_Test, EmptyEdge_NoChildren)
{
  BRep_Builder aBuilder;
  TopoDS_Edge  anEdge;
  aBuilder.MakeEdge(anEdge);

  TopoDS_Iterator anIter(anEdge);
  EXPECT_FALSE(anIter.More());
}

TEST(TopoDS_Iterator_Test, EmptyWire_NoChildren)
{
  BRep_Builder aBuilder;
  TopoDS_Wire  aWire;
  aBuilder.MakeWire(aWire);

  TopoDS_Iterator anIter(aWire);
  EXPECT_FALSE(anIter.More());
}

TEST(TopoDS_Iterator_Test, EmptyFace_NoChildren)
{
  BRep_Builder aBuilder;
  TopoDS_Face  aFace;
  aBuilder.MakeFace(aFace);

  TopoDS_Iterator anIter(aFace);
  EXPECT_FALSE(anIter.More());
}

TEST(TopoDS_Iterator_Test, EmptyShell_NoChildren)
{
  BRep_Builder aBuilder;
  TopoDS_Shell aShell;
  aBuilder.MakeShell(aShell);

  TopoDS_Iterator anIter(aShell);
  EXPECT_FALSE(anIter.More());
}

TEST(TopoDS_Iterator_Test, EmptySolid_NoChildren)
{
  BRep_Builder aBuilder;
  TopoDS_Solid aSolid;
  aBuilder.MakeSolid(aSolid);

  TopoDS_Iterator anIter(aSolid);
  EXPECT_FALSE(anIter.More());
}

TEST(TopoDS_Iterator_Test, EmptyCompSolid_NoChildren)
{
  BRep_Builder     aBuilder;
  TopoDS_CompSolid aCompSolid;
  aBuilder.MakeCompSolid(aCompSolid);

  TopoDS_Iterator anIter(aCompSolid);
  EXPECT_FALSE(anIter.More());
}

TEST(TopoDS_Iterator_Test, EmptyCompound_NoChildren)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  TopoDS_Iterator anIter(aCompound);
  EXPECT_FALSE(anIter.More());
}

//==================================================================================================
// Value access tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, ValueOnEmpty_ThrowsException)
{
  TopoDS_Iterator anIter;
  EXPECT_THROW(anIter.Value(), Standard_NoSuchObject);
}

TEST(TopoDS_Iterator_Test, ValueAfterEnd_ThrowsException)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0)));

  TopoDS_Iterator anIter(aCompound);
  EXPECT_TRUE(anIter.More());
  anIter.Next();
  EXPECT_FALSE(anIter.More());
  EXPECT_THROW(anIter.Value(), Standard_NoSuchObject);
}

//==================================================================================================
// Re-initialization tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, Reinitialize_WithDifferentShape)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound1, aCompound2;
  aBuilder.MakeCompound(aCompound1);
  aBuilder.MakeCompound(aCompound2);

  aBuilder.Add(aCompound1, BRepBuilderAPI_MakeVertex(gp_Pnt(1, 0, 0)));
  aBuilder.Add(aCompound2, BRepBuilderAPI_MakeVertex(gp_Pnt(2, 0, 0)));
  aBuilder.Add(aCompound2, BRepBuilderAPI_MakeVertex(gp_Pnt(3, 0, 0)));

  TopoDS_Iterator anIter(aCompound1);
  EXPECT_TRUE(anIter.More());

  // Count children of first compound
  int aCount1 = 0;
  for (; anIter.More(); anIter.Next())
  {
    ++aCount1;
  }
  EXPECT_EQ(aCount1, 1);

  // Re-initialize with second compound
  anIter.Initialize(aCompound2);
  int aCount2 = 0;
  for (; anIter.More(); anIter.Next())
  {
    ++aCount2;
  }
  EXPECT_EQ(aCount2, 2);
}

TEST(TopoDS_Iterator_Test, Reinitialize_WithNullShape)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0)));

  TopoDS_Iterator anIter(aCompound);
  EXPECT_TRUE(anIter.More());

  // Re-initialize with null shape
  TopoDS_Shape aNullShape;
  anIter.Initialize(aNullShape);
  EXPECT_FALSE(anIter.More());
}

//==================================================================================================
// Cumulative orientation tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, CumulativeOrientation_Enabled)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0)));

  TopoDS_Shape aReversed = aCompound.Reversed();

  TopoDS_Iterator anIter(aReversed, true, false);
  EXPECT_TRUE(anIter.More());
  EXPECT_EQ(anIter.Value().Orientation(), TopAbs_REVERSED);
}

TEST(TopoDS_Iterator_Test, CumulativeOrientation_Disabled)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0)));

  TopoDS_Shape aReversed = aCompound.Reversed();

  TopoDS_Iterator anIter(aReversed, false, false);
  EXPECT_TRUE(anIter.More());
  EXPECT_EQ(anIter.Value().Orientation(), TopAbs_FORWARD);
}

//==================================================================================================
// Multiple iteration passes
//==================================================================================================

TEST(TopoDS_Iterator_Test, MultipleIterationPasses_SameResult)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  const int aNbVertices = 5;
  for (int i = 0; i < aNbVertices; ++i)
  {
    aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(i, 0, 0)));
  }

  // First pass
  int aCount1 = 0;
  for (TopoDS_Iterator anIter1(aCompound); anIter1.More(); anIter1.Next())
  {
    ++aCount1;
  }

  // Second pass - should give same result
  int aCount2 = 0;
  for (TopoDS_Iterator anIter2(aCompound); anIter2.More(); anIter2.Next())
  {
    ++aCount2;
  }

  EXPECT_EQ(aCount1, aNbVertices);
  EXPECT_EQ(aCount2, aNbVertices);
}

//==================================================================================================
// All shape types iteration
//==================================================================================================

TEST(TopoDS_Iterator_Test, IterateAllShapeTypes)
{
  BRep_Builder aBuilder;

  // Edge with vertices
  TopoDS_Edge anEdge;
  aBuilder.MakeEdge(anEdge);
  aBuilder.Add(anEdge, BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0)));
  aBuilder.Add(anEdge, BRepBuilderAPI_MakeVertex(gp_Pnt(1, 0, 0)));

  int anEdgeChildren = 0;
  for (TopoDS_Iterator anIter(anEdge); anIter.More(); anIter.Next())
  {
    EXPECT_EQ(anIter.Value().ShapeType(), TopAbs_VERTEX);
    ++anEdgeChildren;
  }
  EXPECT_EQ(anEdgeChildren, 2);

  // Wire with edge
  TopoDS_Wire aWire;
  aBuilder.MakeWire(aWire);
  TopoDS_Edge anEdge2;
  aBuilder.MakeEdge(anEdge2);
  aBuilder.Add(aWire, anEdge2);

  int aWireChildren = 0;
  for (TopoDS_Iterator anIter(aWire); anIter.More(); anIter.Next())
  {
    EXPECT_EQ(anIter.Value().ShapeType(), TopAbs_EDGE);
    ++aWireChildren;
  }
  EXPECT_EQ(aWireChildren, 1);

  // Face with wire
  TopoDS_Face aFace;
  aBuilder.MakeFace(aFace);
  TopoDS_Wire aWire2;
  aBuilder.MakeWire(aWire2);
  aBuilder.Add(aFace, aWire2);

  int aFaceChildren = 0;
  for (TopoDS_Iterator anIter(aFace); anIter.More(); anIter.Next())
  {
    EXPECT_EQ(anIter.Value().ShapeType(), TopAbs_WIRE);
    ++aFaceChildren;
  }
  EXPECT_EQ(aFaceChildren, 1);

  // Shell with face
  TopoDS_Shell aShell;
  aBuilder.MakeShell(aShell);
  TopoDS_Face aFace2;
  aBuilder.MakeFace(aFace2);
  aBuilder.Add(aShell, aFace2);

  int aShellChildren = 0;
  for (TopoDS_Iterator anIter(aShell); anIter.More(); anIter.Next())
  {
    EXPECT_EQ(anIter.Value().ShapeType(), TopAbs_FACE);
    ++aShellChildren;
  }
  EXPECT_EQ(aShellChildren, 1);

  // Solid with shell
  TopoDS_Solid aSolid;
  aBuilder.MakeSolid(aSolid);
  TopoDS_Shell aShell2;
  aBuilder.MakeShell(aShell2);
  aBuilder.Add(aSolid, aShell2);

  int aSolidChildren = 0;
  for (TopoDS_Iterator anIter(aSolid); anIter.More(); anIter.Next())
  {
    EXPECT_EQ(anIter.Value().ShapeType(), TopAbs_SHELL);
    ++aSolidChildren;
  }
  EXPECT_EQ(aSolidChildren, 1);

  // CompSolid with solid
  TopoDS_CompSolid aCompSolid;
  aBuilder.MakeCompSolid(aCompSolid);
  TopoDS_Solid aSolid2;
  aBuilder.MakeSolid(aSolid2);
  aBuilder.Add(aCompSolid, aSolid2);

  int aCompSolidChildren = 0;
  for (TopoDS_Iterator anIter(aCompSolid); anIter.More(); anIter.Next())
  {
    EXPECT_EQ(anIter.Value().ShapeType(), TopAbs_SOLID);
    ++aCompSolidChildren;
  }
  EXPECT_EQ(aCompSolidChildren, 1);

  // Compound with mixed types
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0)));

  int aCompoundChildren = 0;
  for (TopoDS_Iterator anIter(aCompound); anIter.More(); anIter.Next())
  {
    ++aCompoundChildren;
  }
  EXPECT_EQ(aCompoundChildren, 1);
}
