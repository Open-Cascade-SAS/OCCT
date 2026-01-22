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
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Builder.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>

//=================================================================================================
// Test OCC30708_1: Initialize with null shape
//=================================================================================================

TEST(TopoDS_Iterator_Test, OCC30708_1_InitializeWithNullShape)
{
  TopoDS_Iterator it;
  TopoDS_Shape    empty;

  // Should not throw exception when initializing with null shape
  EXPECT_NO_THROW(it.Initialize(empty));

  // More() should return false on null shape
  EXPECT_FALSE(it.More());
}

//=================================================================================================
// Test iterator on empty compound
//=================================================================================================

TEST(TopoDS_Iterator_Test, EmptyCompound)
{
  TopoDS_Builder  aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  TopoDS_Iterator anIt(aCompound);
  EXPECT_FALSE(anIt.More()) << "Empty compound should have no children";
}

//=================================================================================================
// Test iterator on compound with vertices
//=================================================================================================

TEST(TopoDS_Iterator_Test, CompoundWithVertices)
{
  TopoDS_Builder  aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  // Add 5 vertices
  for (int i = 0; i < 5; ++i)
  {
    TopoDS_Vertex aV = BRepBuilderAPI_MakeVertex(gp_Pnt(i, 0, 0));
    aBuilder.Add(aCompound, aV);
  }

  // Iterate and count
  int aCount = 0;
  for (TopoDS_Iterator anIt(aCompound); anIt.More(); anIt.Next())
  {
    EXPECT_EQ(anIt.Value().ShapeType(), TopAbs_VERTEX);
    ++aCount;
  }

  EXPECT_EQ(aCount, 5) << "Should iterate over 5 vertices";
}

//=================================================================================================
// Test iterator on wire with edges
//=================================================================================================

TEST(TopoDS_Iterator_Test, WireWithEdges)
{
  TopoDS_Builder aBuilder;
  TopoDS_Wire    aWire;
  aBuilder.MakeWire(aWire);

  // Create a rectangular wire (4 edges)
  TopoDS_Edge aE1 = BRepBuilderAPI_MakeEdge(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0));
  TopoDS_Edge aE2 = BRepBuilderAPI_MakeEdge(gp_Pnt(1, 0, 0), gp_Pnt(1, 1, 0));
  TopoDS_Edge aE3 = BRepBuilderAPI_MakeEdge(gp_Pnt(1, 1, 0), gp_Pnt(0, 1, 0));
  TopoDS_Edge aE4 = BRepBuilderAPI_MakeEdge(gp_Pnt(0, 1, 0), gp_Pnt(0, 0, 0));

  aBuilder.Add(aWire, aE1);
  aBuilder.Add(aWire, aE2);
  aBuilder.Add(aWire, aE3);
  aBuilder.Add(aWire, aE4);

  // Iterate and verify
  int aCount = 0;
  for (TopoDS_Iterator anIt(aWire); anIt.More(); anIt.Next())
  {
    EXPECT_EQ(anIt.Value().ShapeType(), TopAbs_EDGE);
    ++aCount;
  }

  EXPECT_EQ(aCount, 4) << "Wire should have 4 edges";
}

//=================================================================================================
// Test iterator on edge with vertices
//=================================================================================================

TEST(TopoDS_Iterator_Test, EdgeWithVertices)
{
  TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(gp_Pnt(0, 0, 0), gp_Pnt(1, 1, 1));

  int aCount = 0;
  for (TopoDS_Iterator anIt(anEdge); anIt.More(); anIt.Next())
  {
    EXPECT_EQ(anIt.Value().ShapeType(), TopAbs_VERTEX);
    ++aCount;
  }

  EXPECT_EQ(aCount, 2) << "Edge should have 2 vertices";
}

//=================================================================================================
// Test iterator on vertex (no children)
//=================================================================================================

TEST(TopoDS_Iterator_Test, VertexNoChildren)
{
  TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));

  TopoDS_Iterator anIt(aVertex);
  EXPECT_FALSE(anIt.More()) << "Vertex should have no children";
}

//=================================================================================================
// Test iterator cumulative orientation
//=================================================================================================

TEST(TopoDS_Iterator_Test, CumulativeOrientation)
{
  TopoDS_Builder  aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aV = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));
  aBuilder.Add(aCompound, aV);

  // Reverse the compound
  aCompound.Reverse();

  // With cumOri=true (default), child orientation should be composed
  TopoDS_Iterator anIt1(aCompound, true, true);
  EXPECT_TRUE(anIt1.More());
  TopAbs_Orientation anOri1 = anIt1.Value().Orientation();

  // With cumOri=false, child orientation should be original
  TopoDS_Iterator anIt2(aCompound, false, true);
  EXPECT_TRUE(anIt2.More());
  TopAbs_Orientation anOri2 = anIt2.Value().Orientation();

  // Orientations should differ when compound is reversed
  EXPECT_NE(anOri1, anOri2) << "Cumulative orientation should affect result";
}

//=================================================================================================
// Test iterator re-initialization
//=================================================================================================

TEST(TopoDS_Iterator_Test, ReInitialization)
{
  TopoDS_Builder  aBuilder;
  TopoDS_Compound aCompound1, aCompound2;
  aBuilder.MakeCompound(aCompound1);
  aBuilder.MakeCompound(aCompound2);

  // Add different number of vertices
  for (int i = 0; i < 3; ++i)
  {
    aBuilder.Add(aCompound1, BRepBuilderAPI_MakeVertex(gp_Pnt(i, 0, 0)));
  }
  for (int i = 0; i < 5; ++i)
  {
    aBuilder.Add(aCompound2, BRepBuilderAPI_MakeVertex(gp_Pnt(i, 1, 0)));
  }

  TopoDS_Iterator anIt(aCompound1);

  // Count first compound
  int aCount1 = 0;
  for (; anIt.More(); anIt.Next())
    ++aCount1;
  EXPECT_EQ(aCount1, 3);

  // Re-initialize with second compound
  anIt.Initialize(aCompound2);

  // Count second compound
  int aCount2 = 0;
  for (; anIt.More(); anIt.Next())
    ++aCount2;
  EXPECT_EQ(aCount2, 5);
}

//=================================================================================================
// Test iterator on box solid (shell -> faces)
//=================================================================================================

TEST(TopoDS_Iterator_Test, BoxSolidIteration)
{
  // Create a box
  TopoDS_Shape aBox = BRepPrimAPI_MakeBox(1.0, 2.0, 3.0).Shape();

  // Iterate over solid's children (should be 1 shell)
  int aShellCount = 0;
  for (TopoDS_Iterator anIt(aBox); anIt.More(); anIt.Next())
  {
    EXPECT_EQ(anIt.Value().ShapeType(), TopAbs_SHELL);
    ++aShellCount;

    // Iterate over shell's children (should be 6 faces)
    int aFaceCount = 0;
    for (TopoDS_Iterator anIt2(anIt.Value()); anIt2.More(); anIt2.Next())
    {
      EXPECT_EQ(anIt2.Value().ShapeType(), TopAbs_FACE);
      ++aFaceCount;
    }
    EXPECT_EQ(aFaceCount, 6) << "Box shell should have 6 faces";
  }

  EXPECT_EQ(aShellCount, 1) << "Box solid should have 1 shell";
}

//=================================================================================================
// Test iterator with many children (performance/stress test)
//=================================================================================================

TEST(TopoDS_Iterator_Test, ManyChildren)
{
  TopoDS_Builder  aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound, 1000); // Large bucket

  const int aNbVertices = 1000;

  // Add many vertices
  for (int i = 0; i < aNbVertices; ++i)
  {
    aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(i, 0, 0)));
  }

  // Iterate and count
  int aCount = 0;
  for (TopoDS_Iterator anIt(aCompound); anIt.More(); anIt.Next())
  {
    ++aCount;
  }

  EXPECT_EQ(aCount, aNbVertices) << "Should iterate over all vertices";
}

//=================================================================================================
// Test default constructor followed by Initialize
//=================================================================================================

TEST(TopoDS_Iterator_Test, DefaultConstructorThenInitialize)
{
  TopoDS_Builder  aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0)));

  // Default construct, then initialize
  TopoDS_Iterator anIt;
  EXPECT_FALSE(anIt.More()) << "Default constructed iterator should have More()=false";

  anIt.Initialize(aCompound);
  EXPECT_TRUE(anIt.More()) << "After Initialize, More() should be true";

  EXPECT_EQ(anIt.Value().ShapeType(), TopAbs_VERTEX);
}

//=================================================================================================
// Test Value() throws when not More() (only in debug builds with exceptions enabled)
//=================================================================================================

#ifndef No_Exception
TEST(TopoDS_Iterator_Test, ValueThrowsWhenNotMore)
{
  TopoDS_Builder  aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  TopoDS_Iterator anIt(aCompound);
  EXPECT_FALSE(anIt.More());

  // Value() should throw Standard_NoSuchObject when !More()
  EXPECT_THROW(anIt.Value(), Standard_NoSuchObject);
}
#endif

//=================================================================================================
// Test iterator with cumulative location
//=================================================================================================

TEST(TopoDS_Iterator_Test, CumulativeLocation)
{
  TopoDS_Builder  aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aV = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));
  aBuilder.Add(aCompound, aV);

  // Apply a translation to the compound
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(10, 20, 30));
  TopLoc_Location aLoc(aTrsf);
  aCompound.Location(aLoc);

  // With cumLoc=true (default), child should have composed location
  TopoDS_Iterator anIt1(aCompound, true, true);
  EXPECT_TRUE(anIt1.More());
  TopLoc_Location aChildLoc1 = anIt1.Value().Location();

  // With cumLoc=false, child should have original location
  TopoDS_Iterator anIt2(aCompound, true, false);
  EXPECT_TRUE(anIt2.More());
  TopLoc_Location aChildLoc2 = anIt2.Value().Location();

  // Locations should differ
  EXPECT_NE(aChildLoc1.IsIdentity(), aChildLoc2.IsIdentity())
    << "Cumulative location should affect child location";
}

//=================================================================================================
// Test iterator on nested compounds
//=================================================================================================

TEST(TopoDS_Iterator_Test, NestedCompounds)
{
  TopoDS_Builder  aBuilder;
  TopoDS_Compound aOuter, aInner1, aInner2;
  aBuilder.MakeCompound(aOuter);
  aBuilder.MakeCompound(aInner1);
  aBuilder.MakeCompound(aInner2);

  // Add vertices to inner compounds
  aBuilder.Add(aInner1, BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0)));
  aBuilder.Add(aInner1, BRepBuilderAPI_MakeVertex(gp_Pnt(1, 0, 0)));
  aBuilder.Add(aInner2, BRepBuilderAPI_MakeVertex(gp_Pnt(2, 0, 0)));

  // Add inner compounds to outer
  aBuilder.Add(aOuter, aInner1);
  aBuilder.Add(aOuter, aInner2);

  // Outer should have 2 children (both compounds)
  int aOuterCount = 0;
  for (TopoDS_Iterator anIt(aOuter); anIt.More(); anIt.Next())
  {
    EXPECT_EQ(anIt.Value().ShapeType(), TopAbs_COMPOUND);
    ++aOuterCount;
  }
  EXPECT_EQ(aOuterCount, 2);

  // First inner should have 2 children
  TopoDS_Iterator anIt(aOuter);
  int             aInner1Count = 0;
  for (TopoDS_Iterator anIt2(anIt.Value()); anIt2.More(); anIt2.Next())
  {
    ++aInner1Count;
  }
  EXPECT_EQ(aInner1Count, 2);
}

//=================================================================================================
// Test iterator on face with wires (from box)
//=================================================================================================

TEST(TopoDS_Iterator_Test, FaceWithWires)
{
  TopoDS_Shape aBox = BRepPrimAPI_MakeBox(1.0, 1.0, 1.0).Shape();

  // Get first face
  TopExp_Explorer anExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());
  const TopoDS_Face& aFace = TopoDS::Face(anExp.Current());

  // Iterate over face's children (should be wires)
  int aWireCount = 0;
  for (TopoDS_Iterator anIt(aFace); anIt.More(); anIt.Next())
  {
    EXPECT_EQ(anIt.Value().ShapeType(), TopAbs_WIRE);
    ++aWireCount;
  }

  EXPECT_GE(aWireCount, 1) << "Face should have at least 1 wire";
}

//=================================================================================================
// Test iterator preserves shape identity
//=================================================================================================

TEST(TopoDS_Iterator_Test, ShapeIdentity)
{
  TopoDS_Builder  aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aV1 = BRepBuilderAPI_MakeVertex(gp_Pnt(1, 0, 0));
  TopoDS_Vertex aV2 = BRepBuilderAPI_MakeVertex(gp_Pnt(2, 0, 0));

  aBuilder.Add(aCompound, aV1);
  aBuilder.Add(aCompound, aV2);

  // Iterate and check that TShape pointers match
  TopoDS_Iterator anIt(aCompound);
  EXPECT_TRUE(anIt.Value().TShape() == aV1.TShape() || anIt.Value().TShape() == aV2.TShape());
  anIt.Next();
  EXPECT_TRUE(anIt.Value().TShape() == aV1.TShape() || anIt.Value().TShape() == aV2.TShape());
}

//=================================================================================================
// Test iterator on shell with faces
//=================================================================================================

TEST(TopoDS_Iterator_Test, ShellWithFaces)
{
  TopoDS_Shape aBox = BRepPrimAPI_MakeBox(1.0, 1.0, 1.0).Shape();

  // Get shell from box
  TopExp_Explorer anExp(aBox, TopAbs_SHELL);
  ASSERT_TRUE(anExp.More());
  const TopoDS_Shell& aShell = TopoDS::Shell(anExp.Current());

  // Iterate over shell's faces
  int aFaceCount = 0;
  for (TopoDS_Iterator anIt(aShell); anIt.More(); anIt.Next())
  {
    EXPECT_EQ(anIt.Value().ShapeType(), TopAbs_FACE);
    ++aFaceCount;
  }

  EXPECT_EQ(aFaceCount, 6) << "Box shell should have 6 faces";
}
