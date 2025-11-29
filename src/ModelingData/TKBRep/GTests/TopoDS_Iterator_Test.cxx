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
#include <BRepPrimAPI_MakeBox.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TopLoc_Location.hxx>
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

//==================================================================================================
// Cumulative location tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, CumulativeLocation_Enabled)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0)));

  // Apply location to compound
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(100, 0, 0));
  TopLoc_Location aLoc(aTrsf);
  TopoDS_Shape    aLocatedCompound = aCompound.Located(aLoc);

  // With cumulative location (default)
  TopoDS_Iterator anIterCum(aLocatedCompound, true, true);
  EXPECT_TRUE(anIterCum.More());
  EXPECT_FALSE(anIterCum.Value().Location().IsIdentity());
}

TEST(TopoDS_Iterator_Test, CumulativeLocation_Disabled)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0)));

  // Apply location to compound
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(100, 0, 0));
  TopLoc_Location aLoc(aTrsf);
  TopoDS_Shape    aLocatedCompound = aCompound.Located(aLoc);

  // Without cumulative location
  TopoDS_Iterator anIterNoCum(aLocatedCompound, true, false);
  EXPECT_TRUE(anIterNoCum.More());
  EXPECT_TRUE(anIterNoCum.Value().Location().IsIdentity());
}

//==================================================================================================
// Nested shape iteration tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, NestedCompounds_IterateFirstLevel)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aOuter, aInner1, aInner2;
  aBuilder.MakeCompound(aOuter);
  aBuilder.MakeCompound(aInner1);
  aBuilder.MakeCompound(aInner2);

  aBuilder.Add(aInner1, BRepBuilderAPI_MakeVertex(gp_Pnt(1, 0, 0)));
  aBuilder.Add(aInner2, BRepBuilderAPI_MakeVertex(gp_Pnt(2, 0, 0)));

  aBuilder.Add(aOuter, aInner1);
  aBuilder.Add(aOuter, aInner2);

  // Iterator only iterates direct children (first level)
  int aCount = 0;
  for (TopoDS_Iterator anIter(aOuter); anIter.More(); anIter.Next())
  {
    EXPECT_EQ(anIter.Value().ShapeType(), TopAbs_COMPOUND);
    ++aCount;
  }
  EXPECT_EQ(aCount, 2);
}

TEST(TopoDS_Iterator_Test, DeepHierarchy_ManualRecursion)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aLevel1, aLevel2, aLevel3;
  aBuilder.MakeCompound(aLevel1);
  aBuilder.MakeCompound(aLevel2);
  aBuilder.MakeCompound(aLevel3);

  TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));
  aBuilder.Add(aLevel3, aVertex);
  aBuilder.Add(aLevel2, aLevel3);
  aBuilder.Add(aLevel1, aLevel2);

  // Manual recursion to find the vertex
  TopoDS_Iterator anIter1(aLevel1);
  EXPECT_TRUE(anIter1.More());
  EXPECT_EQ(anIter1.Value().ShapeType(), TopAbs_COMPOUND);

  TopoDS_Iterator anIter2(anIter1.Value());
  EXPECT_TRUE(anIter2.More());
  EXPECT_EQ(anIter2.Value().ShapeType(), TopAbs_COMPOUND);

  TopoDS_Iterator anIter3(anIter2.Value());
  EXPECT_TRUE(anIter3.More());
  EXPECT_EQ(anIter3.Value().ShapeType(), TopAbs_VERTEX);
}

//==================================================================================================
// Large compound iteration tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, LargeCompound_IterateThousandChildren)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  const int aNbChildren = 1000;
  for (int i = 0; i < aNbChildren; ++i)
  {
    aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(i, 0, 0)));
  }

  int aCount = 0;
  for (TopoDS_Iterator anIter(aCompound); anIter.More(); anIter.Next())
  {
    ++aCount;
  }
  EXPECT_EQ(aCount, aNbChildren);
}

TEST(TopoDS_Iterator_Test, LargeCompound_MultipleIterators)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  const int aNbChildren = 100;
  for (int i = 0; i < aNbChildren; ++i)
  {
    aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(i, 0, 0)));
  }

  // Multiple iterators on the same shape should work independently
  TopoDS_Iterator anIter1(aCompound);
  TopoDS_Iterator anIter2(aCompound);
  TopoDS_Iterator anIter3(aCompound);

  int aCount1 = 0, aCount2 = 0, aCount3 = 0;
  for (; anIter1.More(); anIter1.Next())
    ++aCount1;
  for (; anIter2.More(); anIter2.Next())
    ++aCount2;
  for (; anIter3.More(); anIter3.Next())
    ++aCount3;

  EXPECT_EQ(aCount1, aNbChildren);
  EXPECT_EQ(aCount2, aNbChildren);
  EXPECT_EQ(aCount3, aNbChildren);
}

//==================================================================================================
// Orientation composition tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, OrientationComposition_ForwardForward)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));
  aBuilder.Add(aCompound, aVertex); // Forward vertex to forward compound

  TopoDS_Iterator anIter(aCompound, true, false);
  EXPECT_TRUE(anIter.More());
  EXPECT_EQ(anIter.Value().Orientation(), TopAbs_FORWARD);
}

TEST(TopoDS_Iterator_Test, OrientationComposition_ReversedForward)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));
  aBuilder.Add(aCompound, aVertex);

  TopoDS_Shape aReversedCompound = aCompound.Reversed();

  // With cumulative orientation: REVERSED * FORWARD = REVERSED
  TopoDS_Iterator anIter(aReversedCompound, true, false);
  EXPECT_TRUE(anIter.More());
  EXPECT_EQ(anIter.Value().Orientation(), TopAbs_REVERSED);
}

TEST(TopoDS_Iterator_Test, OrientationComposition_ForwardReversed)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));
  aBuilder.Add(aCompound, aVertex.Reversed());

  // Forward compound with reversed child
  TopoDS_Iterator anIter(aCompound, true, false);
  EXPECT_TRUE(anIter.More());
  EXPECT_EQ(anIter.Value().Orientation(), TopAbs_REVERSED);
}

TEST(TopoDS_Iterator_Test, OrientationComposition_ReversedReversed)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));
  aBuilder.Add(aCompound, aVertex.Reversed());

  TopoDS_Shape aReversedCompound = aCompound.Reversed();

  // REVERSED * REVERSED = FORWARD
  TopoDS_Iterator anIter(aReversedCompound, true, false);
  EXPECT_TRUE(anIter.More());
  EXPECT_EQ(anIter.Value().Orientation(), TopAbs_FORWARD);
}

TEST(TopoDS_Iterator_Test, OrientationComposition_InternalUnchanged)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));
  aBuilder.Add(aCompound, aVertex.Oriented(TopAbs_INTERNAL));

  TopoDS_Shape aReversedCompound = aCompound.Reversed();

  // INTERNAL orientation should remain unchanged
  TopoDS_Iterator anIter(aReversedCompound, true, false);
  EXPECT_TRUE(anIter.More());
  EXPECT_EQ(anIter.Value().Orientation(), TopAbs_INTERNAL);
}

TEST(TopoDS_Iterator_Test, OrientationComposition_ExternalUnchanged)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));
  aBuilder.Add(aCompound, aVertex.Oriented(TopAbs_EXTERNAL));

  TopoDS_Shape aReversedCompound = aCompound.Reversed();

  // EXTERNAL orientation should remain unchanged
  TopoDS_Iterator anIter(aReversedCompound, true, false);
  EXPECT_TRUE(anIter.More());
  EXPECT_EQ(anIter.Value().Orientation(), TopAbs_EXTERNAL);
}

//==================================================================================================
// Iterator with different flag combinations
//==================================================================================================

TEST(TopoDS_Iterator_Test, FlagCombinations_CumOriTrue_CumLocTrue)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0)));

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(10, 0, 0));
  TopLoc_Location aLoc(aTrsf);
  TopoDS_Shape    aLocatedReversed = aCompound.Located(aLoc).Reversed();

  TopoDS_Iterator anIter(aLocatedReversed, true, true);
  EXPECT_TRUE(anIter.More());
  EXPECT_EQ(anIter.Value().Orientation(), TopAbs_REVERSED);
  EXPECT_FALSE(anIter.Value().Location().IsIdentity());
}

TEST(TopoDS_Iterator_Test, FlagCombinations_CumOriTrue_CumLocFalse)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0)));

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(10, 0, 0));
  TopLoc_Location aLoc(aTrsf);
  TopoDS_Shape    aLocatedReversed = aCompound.Located(aLoc).Reversed();

  TopoDS_Iterator anIter(aLocatedReversed, true, false);
  EXPECT_TRUE(anIter.More());
  EXPECT_EQ(anIter.Value().Orientation(), TopAbs_REVERSED);
  EXPECT_TRUE(anIter.Value().Location().IsIdentity());
}

TEST(TopoDS_Iterator_Test, FlagCombinations_CumOriFalse_CumLocTrue)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0)));

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(10, 0, 0));
  TopLoc_Location aLoc(aTrsf);
  TopoDS_Shape    aLocatedReversed = aCompound.Located(aLoc).Reversed();

  TopoDS_Iterator anIter(aLocatedReversed, false, true);
  EXPECT_TRUE(anIter.More());
  EXPECT_EQ(anIter.Value().Orientation(), TopAbs_FORWARD);
  EXPECT_FALSE(anIter.Value().Location().IsIdentity());
}

TEST(TopoDS_Iterator_Test, FlagCombinations_CumOriFalse_CumLocFalse)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0)));

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(10, 0, 0));
  TopLoc_Location aLoc(aTrsf);
  TopoDS_Shape    aLocatedReversed = aCompound.Located(aLoc).Reversed();

  TopoDS_Iterator anIter(aLocatedReversed, false, false);
  EXPECT_TRUE(anIter.More());
  EXPECT_EQ(anIter.Value().Orientation(), TopAbs_FORWARD);
  EXPECT_TRUE(anIter.Value().Location().IsIdentity());
}

//==================================================================================================
// Mixed orientation children tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, MixedOrientations_CountEach)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  // Add vertices with all four orientations
  aBuilder.Add(aCompound,
               BRepBuilderAPI_MakeVertex(gp_Pnt(1, 0, 0)).Vertex().Oriented(TopAbs_FORWARD));
  aBuilder.Add(aCompound,
               BRepBuilderAPI_MakeVertex(gp_Pnt(2, 0, 0)).Vertex().Oriented(TopAbs_REVERSED));
  aBuilder.Add(aCompound,
               BRepBuilderAPI_MakeVertex(gp_Pnt(3, 0, 0)).Vertex().Oriented(TopAbs_INTERNAL));
  aBuilder.Add(aCompound,
               BRepBuilderAPI_MakeVertex(gp_Pnt(4, 0, 0)).Vertex().Oriented(TopAbs_EXTERNAL));

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

//==================================================================================================
// Iterator consistency tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, Consistency_SameChildMultipleTimes)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));

  // Add same vertex multiple times
  aBuilder.Add(aCompound, aVertex);
  aBuilder.Add(aCompound, aVertex);
  aBuilder.Add(aCompound, aVertex);

  int aCount = 0;
  for (TopoDS_Iterator anIter(aCompound); anIter.More(); anIter.Next())
  {
    ++aCount;
  }
  EXPECT_EQ(aCount, 3);
}

TEST(TopoDS_Iterator_Test, Consistency_SameChildDifferentOrientations)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));

  aBuilder.Add(aCompound, aVertex);
  aBuilder.Add(aCompound, aVertex.Reversed());

  int aCount = 0;
  for (TopoDS_Iterator anIter(aCompound); anIter.More(); anIter.Next())
  {
    ++aCount;
  }
  EXPECT_EQ(aCount, 2);
}

//==================================================================================================
// Iterator after shape modification tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, AfterAdd_NewIteratorSeesNewChild)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(1, 0, 0)));

  TopoDS_Iterator anIter1(aCompound);
  int             aCount1 = 0;
  for (; anIter1.More(); anIter1.Next())
    ++aCount1;
  EXPECT_EQ(aCount1, 1);

  // Add more children
  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(2, 0, 0)));
  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(3, 0, 0)));

  // New iterator should see all children
  TopoDS_Iterator anIter2(aCompound);
  int             aCount2 = 0;
  for (; anIter2.More(); anIter2.Next())
    ++aCount2;
  EXPECT_EQ(aCount2, 3);
}

TEST(TopoDS_Iterator_Test, AfterRemove_NewIteratorSeesFewerChildren)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aV1 = BRepBuilderAPI_MakeVertex(gp_Pnt(1, 0, 0));
  TopoDS_Vertex aV2 = BRepBuilderAPI_MakeVertex(gp_Pnt(2, 0, 0));
  TopoDS_Vertex aV3 = BRepBuilderAPI_MakeVertex(gp_Pnt(3, 0, 0));

  aBuilder.Add(aCompound, aV1);
  aBuilder.Add(aCompound, aV2);
  aBuilder.Add(aCompound, aV3);

  TopoDS_Iterator anIter1(aCompound);
  int             aCount1 = 0;
  for (; anIter1.More(); anIter1.Next())
    ++aCount1;
  EXPECT_EQ(aCount1, 3);

  // Remove middle child
  aBuilder.Remove(aCompound, aV2);

  // New iterator should see fewer children
  TopoDS_Iterator anIter2(aCompound);
  int             aCount2 = 0;
  for (; anIter2.More(); anIter2.Next())
    ++aCount2;
  EXPECT_EQ(aCount2, 2);
}

//==================================================================================================
// Edge iteration tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, Edge_IterateVertices)
{
  BRep_Builder aBuilder;
  TopoDS_Edge  anEdge;
  aBuilder.MakeEdge(anEdge);

  TopoDS_Vertex aV1 = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));
  TopoDS_Vertex aV2 = BRepBuilderAPI_MakeVertex(gp_Pnt(1, 0, 0));

  aBuilder.Add(anEdge, aV1.Oriented(TopAbs_FORWARD));
  aBuilder.Add(anEdge, aV2.Oriented(TopAbs_REVERSED));

  int aCount = 0;
  for (TopoDS_Iterator anIter(anEdge); anIter.More(); anIter.Next())
  {
    EXPECT_EQ(anIter.Value().ShapeType(), TopAbs_VERTEX);
    ++aCount;
  }
  EXPECT_EQ(aCount, 2);
}

TEST(TopoDS_Iterator_Test, Edge_ClosedEdge_SameVertexTwice)
{
  BRep_Builder aBuilder;
  TopoDS_Edge  anEdge;
  aBuilder.MakeEdge(anEdge);

  TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));

  aBuilder.Add(anEdge, aVertex.Oriented(TopAbs_FORWARD));
  aBuilder.Add(anEdge, aVertex.Oriented(TopAbs_REVERSED));

  int aCount = 0;
  for (TopoDS_Iterator anIter(anEdge); anIter.More(); anIter.Next())
  {
    EXPECT_EQ(anIter.Value().ShapeType(), TopAbs_VERTEX);
    ++aCount;
  }
  EXPECT_EQ(aCount, 2);
}

//==================================================================================================
// Wire iteration tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, Wire_IterateEdges)
{
  BRep_Builder aBuilder;
  TopoDS_Wire  aWire;
  aBuilder.MakeWire(aWire);

  const int aNbEdges = 5;
  for (int i = 0; i < aNbEdges; ++i)
  {
    TopoDS_Edge anEdge;
    aBuilder.MakeEdge(anEdge);
    aBuilder.Add(aWire, anEdge);
  }

  int aCount = 0;
  for (TopoDS_Iterator anIter(aWire); anIter.More(); anIter.Next())
  {
    EXPECT_EQ(anIter.Value().ShapeType(), TopAbs_EDGE);
    ++aCount;
  }
  EXPECT_EQ(aCount, aNbEdges);
}

//==================================================================================================
// Face iteration tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, Face_IterateWires)
{
  BRep_Builder aBuilder;
  TopoDS_Face  aFace;
  aBuilder.MakeFace(aFace);

  TopoDS_Wire aOuterWire, anInnerWire;
  aBuilder.MakeWire(aOuterWire);
  aBuilder.MakeWire(anInnerWire);

  aBuilder.Add(aFace, aOuterWire);
  aBuilder.Add(aFace, anInnerWire);

  int aCount = 0;
  for (TopoDS_Iterator anIter(aFace); anIter.More(); anIter.Next())
  {
    EXPECT_EQ(anIter.Value().ShapeType(), TopAbs_WIRE);
    ++aCount;
  }
  EXPECT_EQ(aCount, 2);
}

TEST(TopoDS_Iterator_Test, Face_WiresAndVertices)
{
  BRep_Builder aBuilder;
  TopoDS_Face  aFace;
  aBuilder.MakeFace(aFace);

  TopoDS_Wire   aWire;
  TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));

  aBuilder.MakeWire(aWire);
  aBuilder.Add(aFace, aWire);
  aBuilder.Add(aFace, aVertex);

  int aWireCount = 0, aVertexCount = 0;
  for (TopoDS_Iterator anIter(aFace); anIter.More(); anIter.Next())
  {
    if (anIter.Value().ShapeType() == TopAbs_WIRE)
      ++aWireCount;
    else if (anIter.Value().ShapeType() == TopAbs_VERTEX)
      ++aVertexCount;
  }

  EXPECT_EQ(aWireCount, 1);
  EXPECT_EQ(aVertexCount, 1);
}

//==================================================================================================
// Shell iteration tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, Shell_IterateFaces)
{
  BRep_Builder aBuilder;
  TopoDS_Shell aShell;
  aBuilder.MakeShell(aShell);

  const int aNbFaces = 6;
  for (int i = 0; i < aNbFaces; ++i)
  {
    TopoDS_Face aFace;
    aBuilder.MakeFace(aFace);
    aBuilder.Add(aShell, aFace);
  }

  int aCount = 0;
  for (TopoDS_Iterator anIter(aShell); anIter.More(); anIter.Next())
  {
    EXPECT_EQ(anIter.Value().ShapeType(), TopAbs_FACE);
    ++aCount;
  }
  EXPECT_EQ(aCount, aNbFaces);
}

//==================================================================================================
// Solid iteration tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, Solid_IterateShells)
{
  BRep_Builder aBuilder;
  TopoDS_Solid aSolid;
  aBuilder.MakeSolid(aSolid);

  TopoDS_Shell aShell;
  aBuilder.MakeShell(aShell);
  aBuilder.Add(aSolid, aShell);

  int aCount = 0;
  for (TopoDS_Iterator anIter(aSolid); anIter.More(); anIter.Next())
  {
    EXPECT_EQ(anIter.Value().ShapeType(), TopAbs_SHELL);
    ++aCount;
  }
  EXPECT_EQ(aCount, 1);
}

TEST(TopoDS_Iterator_Test, Solid_MultipleShells)
{
  BRep_Builder aBuilder;
  TopoDS_Solid aSolid;
  aBuilder.MakeSolid(aSolid);

  const int aNbShells = 3;
  for (int i = 0; i < aNbShells; ++i)
  {
    TopoDS_Shell aShell;
    aBuilder.MakeShell(aShell);
    aBuilder.Add(aSolid, aShell);
  }

  int aCount = 0;
  for (TopoDS_Iterator anIter(aSolid); anIter.More(); anIter.Next())
  {
    EXPECT_EQ(anIter.Value().ShapeType(), TopAbs_SHELL);
    ++aCount;
  }
  EXPECT_EQ(aCount, aNbShells);
}

//==================================================================================================
// CompSolid iteration tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, CompSolid_IterateSolids)
{
  BRep_Builder     aBuilder;
  TopoDS_CompSolid aCompSolid;
  aBuilder.MakeCompSolid(aCompSolid);

  const int aNbSolids = 4;
  for (int i = 0; i < aNbSolids; ++i)
  {
    TopoDS_Solid aSolid;
    aBuilder.MakeSolid(aSolid);
    aBuilder.Add(aCompSolid, aSolid);
  }

  int aCount = 0;
  for (TopoDS_Iterator anIter(aCompSolid); anIter.More(); anIter.Next())
  {
    EXPECT_EQ(anIter.Value().ShapeType(), TopAbs_SOLID);
    ++aCount;
  }
  EXPECT_EQ(aCount, aNbSolids);
}

//==================================================================================================
// Compound iteration with mixed types tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, Compound_MixedTypes_AllShapeTypes)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  TopoDS_Vertex    aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));
  TopoDS_Edge      anEdge;
  TopoDS_Wire      aWire;
  TopoDS_Face      aFace;
  TopoDS_Shell     aShell;
  TopoDS_Solid     aSolid;
  TopoDS_CompSolid aCompSolid;

  aBuilder.MakeEdge(anEdge);
  aBuilder.MakeWire(aWire);
  aBuilder.MakeFace(aFace);
  aBuilder.MakeShell(aShell);
  aBuilder.MakeSolid(aSolid);
  aBuilder.MakeCompSolid(aCompSolid);

  aBuilder.Add(aCompound, aVertex);
  aBuilder.Add(aCompound, anEdge);
  aBuilder.Add(aCompound, aWire);
  aBuilder.Add(aCompound, aFace);
  aBuilder.Add(aCompound, aShell);
  aBuilder.Add(aCompound, aSolid);
  aBuilder.Add(aCompound, aCompSolid);

  int aTypeCount[8] = {0};
  for (TopoDS_Iterator anIter(aCompound); anIter.More(); anIter.Next())
  {
    TopAbs_ShapeEnum aType = anIter.Value().ShapeType();
    if (aType >= TopAbs_COMPOUND && aType <= TopAbs_SHAPE)
    {
      aTypeCount[aType]++;
    }
  }

  EXPECT_EQ(aTypeCount[TopAbs_VERTEX], 1);
  EXPECT_EQ(aTypeCount[TopAbs_EDGE], 1);
  EXPECT_EQ(aTypeCount[TopAbs_WIRE], 1);
  EXPECT_EQ(aTypeCount[TopAbs_FACE], 1);
  EXPECT_EQ(aTypeCount[TopAbs_SHELL], 1);
  EXPECT_EQ(aTypeCount[TopAbs_SOLID], 1);
  EXPECT_EQ(aTypeCount[TopAbs_COMPSOLID], 1);
}

//==================================================================================================
// Location chaining tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, LocationChaining_NestedLocations)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aInner, aOuter;
  aBuilder.MakeCompound(aInner);
  aBuilder.MakeCompound(aOuter);

  TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));
  aBuilder.Add(aInner, aVertex);

  // Apply location to inner compound
  gp_Trsf aTrsf1;
  aTrsf1.SetTranslation(gp_Vec(10, 0, 0));
  TopLoc_Location aLoc1(aTrsf1);
  TopoDS_Shape    aLocatedInner = aInner.Located(aLoc1);

  aBuilder.Add(aOuter, aLocatedInner);

  // Apply location to outer compound
  gp_Trsf aTrsf2;
  aTrsf2.SetTranslation(gp_Vec(0, 20, 0));
  TopLoc_Location aLoc2(aTrsf2);
  TopoDS_Shape    aLocatedOuter = aOuter.Located(aLoc2);

  // With cumulative location, should have both locations combined
  TopoDS_Iterator anIterOuter(aLocatedOuter, true, true);
  EXPECT_TRUE(anIterOuter.More());
  EXPECT_FALSE(anIterOuter.Value().Location().IsIdentity());

  TopoDS_Iterator anIterInner(anIterOuter.Value(), true, true);
  EXPECT_TRUE(anIterInner.More());
  EXPECT_FALSE(anIterInner.Value().Location().IsIdentity());
}

//==================================================================================================
// Iterator state tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, State_DefaultConstructorIsInvalid)
{
  TopoDS_Iterator anIter;
  EXPECT_FALSE(anIter.More());
}

TEST(TopoDS_Iterator_Test, State_AfterNext_MovesToNextChild)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(1, 0, 0)));
  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(2, 0, 0)));
  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(3, 0, 0)));

  TopoDS_Iterator anIter(aCompound);

  EXPECT_TRUE(anIter.More());
  anIter.Next();
  EXPECT_TRUE(anIter.More());
  anIter.Next();
  EXPECT_TRUE(anIter.More());
  anIter.Next();
  EXPECT_FALSE(anIter.More());
}

TEST(TopoDS_Iterator_Test, State_InitializeResetsState)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(1, 0, 0)));
  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(2, 0, 0)));

  TopoDS_Iterator anIter(aCompound);

  // Exhaust the iterator
  while (anIter.More())
    anIter.Next();
  EXPECT_FALSE(anIter.More());

  // Re-initialize
  anIter.Initialize(aCompound);
  EXPECT_TRUE(anIter.More());

  int aCount = 0;
  for (; anIter.More(); anIter.Next())
    ++aCount;
  EXPECT_EQ(aCount, 2);
}

//==================================================================================================
// TShape sharing via iterator tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, TShapeSharing_SameChildInMultipleParents)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aParent1, aParent2;
  aBuilder.MakeCompound(aParent1);
  aBuilder.MakeCompound(aParent2);

  TopoDS_Vertex aSharedVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));

  aBuilder.Add(aParent1, aSharedVertex);
  aBuilder.Add(aParent2, aSharedVertex);

  TopoDS_Iterator anIter1(aParent1);
  TopoDS_Iterator anIter2(aParent2);

  EXPECT_TRUE(anIter1.More());
  EXPECT_TRUE(anIter2.More());

  // Both should reference the same TShape
  EXPECT_TRUE(anIter1.Value().IsSame(anIter2.Value()));
}

//==================================================================================================
// Iterator with empty nested compound tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, EmptyNestedCompound_StillVisible)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aOuter, aInnerEmpty;
  aBuilder.MakeCompound(aOuter);
  aBuilder.MakeCompound(aInnerEmpty);

  // Add empty compound to outer
  aBuilder.Add(aOuter, aInnerEmpty);

  TopoDS_Iterator anOuterIter(aOuter);
  EXPECT_TRUE(anOuterIter.More());
  EXPECT_EQ(anOuterIter.Value().ShapeType(), TopAbs_COMPOUND);

  // The empty inner compound should have no children
  TopoDS_Iterator anInnerIter(anOuterIter.Value());
  EXPECT_FALSE(anInnerIter.More());
}

//==================================================================================================
// Stress tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, Stress_DeepHierarchy_10Levels)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aLevels[10];

  for (int i = 0; i < 10; ++i)
  {
    aBuilder.MakeCompound(aLevels[i]);
  }

  // Build hierarchy from bottom up
  TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));
  aBuilder.Add(aLevels[9], aVertex);

  for (int i = 8; i >= 0; --i)
  {
    aBuilder.Add(aLevels[i], aLevels[i + 1]);
  }

  // Traverse down to find the vertex
  TopoDS_Shape aCurrent = aLevels[0];
  for (int i = 0; i < 9; ++i)
  {
    TopoDS_Iterator anIter(aCurrent);
    EXPECT_TRUE(anIter.More());
    EXPECT_EQ(anIter.Value().ShapeType(), TopAbs_COMPOUND);
    aCurrent = anIter.Value();
  }

  // Final level should have the vertex
  TopoDS_Iterator aFinalIter(aCurrent);
  EXPECT_TRUE(aFinalIter.More());
  EXPECT_EQ(aFinalIter.Value().ShapeType(), TopAbs_VERTEX);
}

TEST(TopoDS_Iterator_Test, Stress_WideHierarchy_100ChildrenPerLevel)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  // Add 100 sub-compounds, each with 10 vertices
  for (int i = 0; i < 100; ++i)
  {
    TopoDS_Compound aSubCompound;
    aBuilder.MakeCompound(aSubCompound);

    for (int j = 0; j < 10; ++j)
    {
      aBuilder.Add(aSubCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(i * 10 + j, 0, 0)));
    }

    aBuilder.Add(aCompound, aSubCompound);
  }

  // Verify top level
  int aTopCount = 0;
  for (TopoDS_Iterator anIter(aCompound); anIter.More(); anIter.Next())
  {
    EXPECT_EQ(anIter.Value().ShapeType(), TopAbs_COMPOUND);
    ++aTopCount;
  }
  EXPECT_EQ(aTopCount, 100);

  // Verify each sub-level has 10 children
  for (TopoDS_Iterator anIter(aCompound); anIter.More(); anIter.Next())
  {
    int aSubCount = 0;
    for (TopoDS_Iterator aSubIter(anIter.Value()); aSubIter.More(); aSubIter.Next())
    {
      EXPECT_EQ(aSubIter.Value().ShapeType(), TopAbs_VERTEX);
      ++aSubCount;
    }
    EXPECT_EQ(aSubCount, 10);
  }
}

//==================================================================================================
// Value consistency tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, Value_ConsistentAcrossCalls)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0)));

  TopoDS_Iterator anIter(aCompound);
  EXPECT_TRUE(anIter.More());

  // Multiple calls to Value() should return the same shape
  TopoDS_Shape aValue1 = anIter.Value();
  TopoDS_Shape aValue2 = anIter.Value();
  TopoDS_Shape aValue3 = anIter.Value();

  EXPECT_TRUE(aValue1.IsEqual(aValue2));
  EXPECT_TRUE(aValue2.IsEqual(aValue3));
}

//==================================================================================================
// Initialize with different parameters tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, Initialize_DifferentFlags)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0)));

  TopoDS_Shape aReversed = aCompound.Reversed();

  TopoDS_Iterator anIter;

  // Initialize with cumOri = true
  anIter.Initialize(aReversed, true, false);
  EXPECT_TRUE(anIter.More());
  EXPECT_EQ(anIter.Value().Orientation(), TopAbs_REVERSED);

  // Re-initialize with cumOri = false
  anIter.Initialize(aReversed, false, false);
  EXPECT_TRUE(anIter.More());
  EXPECT_EQ(anIter.Value().Orientation(), TopAbs_FORWARD);
}

//==================================================================================================
// Partial iteration tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, PartialIteration_StopEarly)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  for (int i = 0; i < 10; ++i)
  {
    aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(i, 0, 0)));
  }

  // Iterate only first 5 elements
  TopoDS_Iterator anIter(aCompound);
  int             aCount = 0;
  for (; anIter.More() && aCount < 5; anIter.Next())
  {
    ++aCount;
  }

  EXPECT_EQ(aCount, 5);
  EXPECT_TRUE(anIter.More()); // Iterator should still have more elements
}

TEST(TopoDS_Iterator_Test, PartialIteration_ResumeFromMiddle)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  for (int i = 0; i < 10; ++i)
  {
    aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(i, 0, 0)));
  }

  TopoDS_Iterator anIter(aCompound);

  // Skip first 3
  for (int i = 0; i < 3 && anIter.More(); ++i)
  {
    anIter.Next();
  }

  // Count remaining
  int aRemaining = 0;
  for (; anIter.More(); anIter.Next())
  {
    ++aRemaining;
  }

  EXPECT_EQ(aRemaining, 7);
}

//==================================================================================================
// Interleaved iteration tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, InterleavedIteration_TwoShapes)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound1, aCompound2;
  aBuilder.MakeCompound(aCompound1);
  aBuilder.MakeCompound(aCompound2);

  for (int i = 0; i < 5; ++i)
  {
    aBuilder.Add(aCompound1, BRepBuilderAPI_MakeVertex(gp_Pnt(i, 0, 0)));
    aBuilder.Add(aCompound2, BRepBuilderAPI_MakeVertex(gp_Pnt(i + 100, 0, 0)));
  }

  TopoDS_Iterator anIter1(aCompound1);
  TopoDS_Iterator anIter2(aCompound2);

  // Interleave iteration
  int aCount1 = 0, aCount2 = 0;
  while (anIter1.More() || anIter2.More())
  {
    if (anIter1.More())
    {
      ++aCount1;
      anIter1.Next();
    }
    if (anIter2.More())
    {
      ++aCount2;
      anIter2.Next();
    }
  }

  EXPECT_EQ(aCount1, 5);
  EXPECT_EQ(aCount2, 5);
}

//==================================================================================================
// Iterator copy tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, IteratorCopy_IndependentState)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  for (int i = 0; i < 5; ++i)
  {
    aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(i, 0, 0)));
  }

  TopoDS_Iterator anIter1(aCompound);
  anIter1.Next(); // Move to second element
  anIter1.Next(); // Move to third element

  // Copy iterator
  TopoDS_Iterator anIter2 = anIter1;

  // Both should be at same position initially
  EXPECT_TRUE(anIter1.Value().IsEqual(anIter2.Value()));

  // Advance original
  anIter1.Next();

  // Copy should still be at previous position
  EXPECT_FALSE(anIter1.Value().IsEqual(anIter2.Value()));

  // Count remaining for each
  int aRemaining1 = 0, aRemaining2 = 0;
  for (; anIter1.More(); anIter1.Next())
    ++aRemaining1;
  for (; anIter2.More(); anIter2.Next())
    ++aRemaining2;

  EXPECT_EQ(aRemaining1, 2); // 4th and 5th elements
  EXPECT_EQ(aRemaining2, 3); // 3rd, 4th, and 5th elements
}

//==================================================================================================
// Iteration order tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, IterationOrder_Consistent)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  // Add vertices in specific order
  for (int i = 0; i < 10; ++i)
  {
    aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(i, 0, 0)));
  }

  // First iteration - collect order
  NCollection_Vector<TopoDS_Shape> aFirstPass;
  for (TopoDS_Iterator anIter(aCompound); anIter.More(); anIter.Next())
  {
    aFirstPass.Append(anIter.Value());
  }

  // Second iteration - verify same order
  int anIndex = 0;
  for (TopoDS_Iterator anIter(aCompound); anIter.More(); anIter.Next(), ++anIndex)
  {
    EXPECT_TRUE(anIter.Value().IsEqual(aFirstPass.Value(anIndex)));
  }
}

//==================================================================================================
// Box full traversal tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, BoxTraversal_CountAllElements)
{
  BRepPrimAPI_MakeBox aBoxMaker(10, 20, 30);
  TopoDS_Shape        aBox = aBoxMaker.Shape();

  // Count shells in solid
  int aShellCount = 0;
  for (TopoDS_Iterator aSolidIter(aBox); aSolidIter.More(); aSolidIter.Next())
  {
    ++aShellCount;
  }
  EXPECT_EQ(aShellCount, 1);

  // Get the shell
  TopoDS_Iterator aSolidIter(aBox);
  TopoDS_Shape    aShell = aSolidIter.Value();

  // Count faces in shell
  int aFaceCount = 0;
  for (TopoDS_Iterator aShellIter(aShell); aShellIter.More(); aShellIter.Next())
  {
    ++aFaceCount;
  }
  EXPECT_EQ(aFaceCount, 6);
}

//==================================================================================================
// Nested iterator tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, NestedIterators_TwoLevels)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aOuter;
  aBuilder.MakeCompound(aOuter);

  const int aNbInner    = 3;
  const int aNbPerInner = 4;

  for (int i = 0; i < aNbInner; ++i)
  {
    TopoDS_Compound aInner;
    aBuilder.MakeCompound(aInner);

    for (int j = 0; j < aNbPerInner; ++j)
    {
      aBuilder.Add(aInner, BRepBuilderAPI_MakeVertex(gp_Pnt(i * 10 + j, 0, 0)));
    }

    aBuilder.Add(aOuter, aInner);
  }

  // Iterate outer
  int anOuterCount   = 0;
  int aTotalVertices = 0;
  for (TopoDS_Iterator anOuterIter(aOuter); anOuterIter.More(); anOuterIter.Next())
  {
    ++anOuterCount;
    EXPECT_EQ(anOuterIter.Value().ShapeType(), TopAbs_COMPOUND);

    // Iterate inner
    for (TopoDS_Iterator anInnerIter(anOuterIter.Value()); anInnerIter.More(); anInnerIter.Next())
    {
      EXPECT_EQ(anInnerIter.Value().ShapeType(), TopAbs_VERTEX);
      ++aTotalVertices;
    }
  }

  EXPECT_EQ(anOuterCount, aNbInner);
  EXPECT_EQ(aTotalVertices, aNbInner * aNbPerInner);
}

//==================================================================================================
// Location propagation tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, LocationPropagation_NestedCompounds)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aInner, aOuter;
  aBuilder.MakeCompound(aInner);
  aBuilder.MakeCompound(aOuter);

  aBuilder.Add(aInner, BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0)));

  // Add inner with location
  gp_Trsf aTrsf1;
  aTrsf1.SetTranslation(gp_Vec(10, 0, 0));
  TopLoc_Location aLoc1(aTrsf1);
  aBuilder.Add(aOuter, aInner.Located(aLoc1));

  // Add location to outer
  gp_Trsf aTrsf2;
  aTrsf2.SetTranslation(gp_Vec(0, 20, 0));
  TopLoc_Location aLoc2(aTrsf2);
  TopoDS_Shape    aLocatedOuter = aOuter.Located(aLoc2);

  // With cumulative location, iterate to vertex
  TopoDS_Iterator anOuterIter(aLocatedOuter, true, true);
  EXPECT_TRUE(anOuterIter.More());

  TopoDS_Iterator anInnerIter(anOuterIter.Value(), true, true);
  EXPECT_TRUE(anInnerIter.More());

  // Vertex should have combined location
  EXPECT_FALSE(anInnerIter.Value().Location().IsIdentity());
}

//==================================================================================================
// Edge with seam vertex tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, Edge_SeamVertexIteration)
{
  BRep_Builder aBuilder;
  TopoDS_Edge  anEdge;
  aBuilder.MakeEdge(anEdge);

  // Create a seam vertex (same vertex at both ends with different orientations)
  TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));

  aBuilder.Add(anEdge, aVertex.Oriented(TopAbs_FORWARD));
  aBuilder.Add(anEdge, aVertex.Oriented(TopAbs_REVERSED));

  // Should have 2 children
  int aCount = 0;
  for (TopoDS_Iterator anIter(anEdge); anIter.More(); anIter.Next())
  {
    ++aCount;
  }
  EXPECT_EQ(aCount, 2);

  // Both children should be partners (same TShape)
  TopoDS_Iterator anIter(anEdge);
  TopoDS_Shape    aFirst = anIter.Value();
  anIter.Next();
  TopoDS_Shape aSecond = anIter.Value();

  EXPECT_TRUE(aFirst.IsPartner(aSecond));
  EXPECT_NE(aFirst.Orientation(), aSecond.Orientation());
}

//==================================================================================================
// Orientation inheritance tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, OrientationInheritance_ThreeLevels)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aLevel1, aLevel2, aLevel3;
  aBuilder.MakeCompound(aLevel1);
  aBuilder.MakeCompound(aLevel2);
  aBuilder.MakeCompound(aLevel3);

  aBuilder.Add(aLevel3, BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0)));
  aBuilder.Add(aLevel2, aLevel3);
  aBuilder.Add(aLevel1, aLevel2);

  // All forward
  TopoDS_Iterator anIter1(aLevel1, true, false);
  EXPECT_EQ(anIter1.Value().Orientation(), TopAbs_FORWARD);

  TopoDS_Iterator anIter2(anIter1.Value(), true, false);
  EXPECT_EQ(anIter2.Value().Orientation(), TopAbs_FORWARD);

  TopoDS_Iterator anIter3(anIter2.Value(), true, false);
  EXPECT_EQ(anIter3.Value().Orientation(), TopAbs_FORWARD);

  // Reverse top level
  TopoDS_Shape aReversedLevel1 = aLevel1.Reversed();

  TopoDS_Iterator anIterR1(aReversedLevel1, true, false);
  EXPECT_EQ(anIterR1.Value().Orientation(), TopAbs_REVERSED);

  TopoDS_Iterator anIterR2(anIterR1.Value(), true, false);
  EXPECT_EQ(anIterR2.Value().Orientation(), TopAbs_REVERSED);

  TopoDS_Iterator anIterR3(anIterR2.Value(), true, false);
  EXPECT_EQ(anIterR3.Value().Orientation(), TopAbs_REVERSED);
}

//==================================================================================================
// Shape comparison via iterator tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, ShapeComparison_SameChildrenDifferentParents)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aParent1, aParent2;
  aBuilder.MakeCompound(aParent1);
  aBuilder.MakeCompound(aParent2);

  TopoDS_Vertex aShared = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));

  aBuilder.Add(aParent1, aShared);
  aBuilder.Add(aParent2, aShared);

  TopoDS_Iterator anIter1(aParent1);
  TopoDS_Iterator anIter2(aParent2);

  // Should be equal (same TShape, location, orientation)
  EXPECT_TRUE(anIter1.Value().IsEqual(anIter2.Value()));
}

//==================================================================================================
// Empty shape special cases
//==================================================================================================

TEST(TopoDS_Iterator_Test, EmptyShape_AllTypes)
{
  BRep_Builder aBuilder;

  TopoDS_Edge anEdge;
  aBuilder.MakeEdge(anEdge);

  TopoDS_Wire aWire;
  aBuilder.MakeWire(aWire);

  TopoDS_Face aFace;
  aBuilder.MakeFace(aFace);

  TopoDS_Shell aShell;
  aBuilder.MakeShell(aShell);

  TopoDS_Solid aSolid;
  aBuilder.MakeSolid(aSolid);

  TopoDS_CompSolid aCompSolid;
  aBuilder.MakeCompSolid(aCompSolid);

  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  // All empty shapes should have no children via iterator
  EXPECT_FALSE(TopoDS_Iterator(anEdge).More());
  EXPECT_FALSE(TopoDS_Iterator(aWire).More());
  EXPECT_FALSE(TopoDS_Iterator(aFace).More());
  EXPECT_FALSE(TopoDS_Iterator(aShell).More());
  EXPECT_FALSE(TopoDS_Iterator(aSolid).More());
  EXPECT_FALSE(TopoDS_Iterator(aCompSolid).More());
  EXPECT_FALSE(TopoDS_Iterator(aCompound).More());
}

//==================================================================================================
// Iteration with modifications between iterations
//==================================================================================================

TEST(TopoDS_Iterator_Test, ModificationBetweenIterations)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(1, 0, 0)));
  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(2, 0, 0)));

  // First iteration
  int aCount1 = 0;
  for (TopoDS_Iterator anIter(aCompound); anIter.More(); anIter.Next())
    ++aCount1;
  EXPECT_EQ(aCount1, 2);

  // Modify
  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(3, 0, 0)));

  // Second iteration
  int aCount2 = 0;
  for (TopoDS_Iterator anIter(aCompound); anIter.More(); anIter.Next())
    ++aCount2;
  EXPECT_EQ(aCount2, 3);

  // Modify again
  TopoDS_Vertex aV4 = BRepBuilderAPI_MakeVertex(gp_Pnt(4, 0, 0));
  aBuilder.Add(aCompound, aV4);

  // Third iteration
  int aCount3 = 0;
  for (TopoDS_Iterator anIter(aCompound); anIter.More(); anIter.Next())
    ++aCount3;
  EXPECT_EQ(aCount3, 4);

  // Remove and iterate
  aBuilder.Remove(aCompound, aV4);
  int aCount4 = 0;
  for (TopoDS_Iterator anIter(aCompound); anIter.More(); anIter.Next())
    ++aCount4;
  EXPECT_EQ(aCount4, 3);
}

//==================================================================================================
// Shape type filtering simulation tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, FilterByType_OnlyEdges)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  // Add mixed types
  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0)));

  TopoDS_Edge anEdge1, anEdge2;
  aBuilder.MakeEdge(anEdge1);
  aBuilder.MakeEdge(anEdge2);
  aBuilder.Add(aCompound, anEdge1);
  aBuilder.Add(aCompound, anEdge2);

  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(1, 0, 0)));

  TopoDS_Wire aWire;
  aBuilder.MakeWire(aWire);
  aBuilder.Add(aCompound, aWire);

  // Filter to count only edges
  int anEdgeCount = 0;
  for (TopoDS_Iterator anIter(aCompound); anIter.More(); anIter.Next())
  {
    if (anIter.Value().ShapeType() == TopAbs_EDGE)
    {
      ++anEdgeCount;
    }
  }

  EXPECT_EQ(anEdgeCount, 2);
}

TEST(TopoDS_Iterator_Test, FilterByType_OnlyVertices)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  // Add mixed types
  TopoDS_Edge anEdge;
  aBuilder.MakeEdge(anEdge);
  aBuilder.Add(aCompound, anEdge);

  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0)));
  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(1, 0, 0)));
  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(2, 0, 0)));

  TopoDS_Wire aWire;
  aBuilder.MakeWire(aWire);
  aBuilder.Add(aCompound, aWire);

  // Filter to count only vertices
  int aVertexCount = 0;
  for (TopoDS_Iterator anIter(aCompound); anIter.More(); anIter.Next())
  {
    if (anIter.Value().ShapeType() == TopAbs_VERTEX)
    {
      ++aVertexCount;
    }
  }

  EXPECT_EQ(aVertexCount, 3);
}

//==================================================================================================
// Complex location chain tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, LocationChain_ThreeLevels)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aLevel1, aLevel2, aLevel3;
  aBuilder.MakeCompound(aLevel1);
  aBuilder.MakeCompound(aLevel2);
  aBuilder.MakeCompound(aLevel3);

  aBuilder.Add(aLevel3, BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0)));

  // Apply locations at each level
  gp_Trsf aTrsf1, aTrsf2, aTrsf3;
  aTrsf1.SetTranslation(gp_Vec(10, 0, 0));
  aTrsf2.SetTranslation(gp_Vec(0, 20, 0));
  aTrsf3.SetTranslation(gp_Vec(0, 0, 30));

  TopoDS_Shape aLocated3 = aLevel3.Located(TopLoc_Location(aTrsf3));
  aBuilder.Add(aLevel2, aLocated3);

  TopoDS_Shape aLocated2 = aLevel2.Located(TopLoc_Location(aTrsf2));
  aBuilder.Add(aLevel1, aLocated2);

  TopoDS_Shape aLocated1 = aLevel1.Located(TopLoc_Location(aTrsf1));

  // With cumulative location, all locations should be combined
  TopoDS_Iterator anIter1(aLocated1, true, true);
  EXPECT_TRUE(anIter1.More());
  EXPECT_FALSE(anIter1.Value().Location().IsIdentity());

  TopoDS_Iterator anIter2(anIter1.Value(), true, true);
  EXPECT_TRUE(anIter2.More());
  EXPECT_FALSE(anIter2.Value().Location().IsIdentity());

  TopoDS_Iterator anIter3(anIter2.Value(), true, true);
  EXPECT_TRUE(anIter3.More());
  EXPECT_FALSE(anIter3.Value().Location().IsIdentity());
}

//==================================================================================================
// Default constructor and later initialization
//==================================================================================================

TEST(TopoDS_Iterator_Test, DefaultConstruction_ThenInitialize)
{
  TopoDS_Iterator anIter;
  EXPECT_FALSE(anIter.More());

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0)));

  anIter.Initialize(aCompound);
  EXPECT_TRUE(anIter.More());

  int aCount = 0;
  for (; anIter.More(); anIter.Next())
    ++aCount;
  EXPECT_EQ(aCount, 1);
}

//==================================================================================================
// Iterator assignment tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, Assignment_CopiesState)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  for (int i = 0; i < 5; ++i)
  {
    aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(i, 0, 0)));
  }

  TopoDS_Iterator anIter1(aCompound);
  anIter1.Next();
  anIter1.Next(); // Position at 3rd element

  TopoDS_Iterator anIter2;
  anIter2 = anIter1;

  // Both should be at same position
  EXPECT_TRUE(anIter1.More());
  EXPECT_TRUE(anIter2.More());
  EXPECT_TRUE(anIter1.Value().IsEqual(anIter2.Value()));

  // Count remaining from each
  int aRemaining1 = 0;
  for (; anIter1.More(); anIter1.Next())
    ++aRemaining1;

  int aRemaining2 = 0;
  for (; anIter2.More(); anIter2.Next())
    ++aRemaining2;

  EXPECT_EQ(aRemaining1, aRemaining2);
}

//==================================================================================================
// Real geometry iteration tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, BoxSolid_IterateShell)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Solid& aSolid = aBoxMaker.Solid();

  // A solid should have exactly one shell
  int aShellCount = 0;
  for (TopoDS_Iterator anIter(aSolid); anIter.More(); anIter.Next())
  {
    EXPECT_EQ(anIter.Value().ShapeType(), TopAbs_SHELL);
    ++aShellCount;
  }
  EXPECT_EQ(aShellCount, 1);
}

TEST(TopoDS_Iterator_Test, BoxShell_IterateFaces)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aShape = aBoxMaker.Shape();

  // Get shell from solid
  TopoDS_Iterator aSolidIter(aShape);
  ASSERT_TRUE(aSolidIter.More());
  const TopoDS_Shape& aShell = aSolidIter.Value();

  // Shell should have 6 faces
  int aFaceCount = 0;
  for (TopoDS_Iterator aShellIter(aShell); aShellIter.More(); aShellIter.Next())
  {
    EXPECT_EQ(aShellIter.Value().ShapeType(), TopAbs_FACE);
    ++aFaceCount;
  }
  EXPECT_EQ(aFaceCount, 6);
}

TEST(TopoDS_Iterator_Test, BoxFace_IterateWire)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aShape = aBoxMaker.Shape();

  // Get shell -> face
  TopoDS_Iterator     aSolidIter(aShape);
  TopoDS_Iterator     aShellIter(aSolidIter.Value());
  const TopoDS_Shape& aFace = aShellIter.Value();

  // Each face should have exactly one outer wire
  int aWireCount = 0;
  for (TopoDS_Iterator aFaceIter(aFace); aFaceIter.More(); aFaceIter.Next())
  {
    EXPECT_EQ(aFaceIter.Value().ShapeType(), TopAbs_WIRE);
    ++aWireCount;
  }
  EXPECT_EQ(aWireCount, 1);
}

TEST(TopoDS_Iterator_Test, BoxWire_IterateEdges)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aShape = aBoxMaker.Shape();

  // Get shell -> face -> wire
  TopoDS_Iterator     aSolidIter(aShape);
  TopoDS_Iterator     aShellIter(aSolidIter.Value());
  TopoDS_Iterator     aFaceIter(aShellIter.Value());
  const TopoDS_Shape& aWire = aFaceIter.Value();

  // Each wire on a box face should have 4 edges
  int anEdgeCount = 0;
  for (TopoDS_Iterator aWireIter(aWire); aWireIter.More(); aWireIter.Next())
  {
    EXPECT_EQ(aWireIter.Value().ShapeType(), TopAbs_EDGE);
    ++anEdgeCount;
  }
  EXPECT_EQ(anEdgeCount, 4);
}

TEST(TopoDS_Iterator_Test, BoxEdge_IterateVertices)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aShape = aBoxMaker.Shape();

  // Get shell -> face -> wire -> edge
  TopoDS_Iterator     aSolidIter(aShape);
  TopoDS_Iterator     aShellIter(aSolidIter.Value());
  TopoDS_Iterator     aFaceIter(aShellIter.Value());
  TopoDS_Iterator     aWireIter(aFaceIter.Value());
  const TopoDS_Shape& anEdge = aWireIter.Value();

  // Each edge should have 2 vertices
  int aVertexCount = 0;
  for (TopoDS_Iterator anEdgeIter(anEdge); anEdgeIter.More(); anEdgeIter.Next())
  {
    EXPECT_EQ(anEdgeIter.Value().ShapeType(), TopAbs_VERTEX);
    ++aVertexCount;
  }
  EXPECT_EQ(aVertexCount, 2);
}

//==================================================================================================
// Iterator with transformation verification
//==================================================================================================

TEST(TopoDS_Iterator_Test, TransformedShape_LocationPreserved)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(1, 2, 3));

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(100, 200, 300));
  TopoDS_Shape aLocatedVertex = aVertex.Located(TopLoc_Location(aTrsf));

  aBuilder.Add(aCompound, aLocatedVertex);

  // Without cumulative location
  TopoDS_Iterator anIterNoCum(aCompound, true, false);
  EXPECT_TRUE(anIterNoCum.More());
  EXPECT_FALSE(anIterNoCum.Value().Location().IsIdentity());

  // The location should match what we set
  gp_Trsf aResultTrsf = anIterNoCum.Value().Location().Transformation();
  EXPECT_NEAR(aResultTrsf.TranslationPart().X(), 100.0, 1e-10);
  EXPECT_NEAR(aResultTrsf.TranslationPart().Y(), 200.0, 1e-10);
  EXPECT_NEAR(aResultTrsf.TranslationPart().Z(), 300.0, 1e-10);
}

TEST(TopoDS_Iterator_Test, NestedTransformations_Cumulative)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aOuter, aInner;
  aBuilder.MakeCompound(aOuter);
  aBuilder.MakeCompound(aInner);

  TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));
  aBuilder.Add(aInner, aVertex);

  gp_Trsf aTrsfInner, aTrsfOuter;
  aTrsfInner.SetTranslation(gp_Vec(10, 0, 0));
  aTrsfOuter.SetTranslation(gp_Vec(0, 20, 0));

  TopoDS_Shape aLocatedInner = aInner.Located(TopLoc_Location(aTrsfInner));
  aBuilder.Add(aOuter, aLocatedInner);

  TopoDS_Shape aLocatedOuter = aOuter.Located(TopLoc_Location(aTrsfOuter));

  // With cumulative location enabled
  TopoDS_Iterator anOuterIter(aLocatedOuter, true, true);
  EXPECT_TRUE(anOuterIter.More());

  TopoDS_Iterator anInnerIter(anOuterIter.Value(), true, true);
  EXPECT_TRUE(anInnerIter.More());

  // The inner vertex should have combined transformation
  gp_Trsf aFinalTrsf = anInnerIter.Value().Location().Transformation();
  // Should be (10, 20, 0) = inner + outer
  EXPECT_NEAR(aFinalTrsf.TranslationPart().X(), 10.0, 1e-10);
  EXPECT_NEAR(aFinalTrsf.TranslationPart().Y(), 20.0, 1e-10);
  EXPECT_NEAR(aFinalTrsf.TranslationPart().Z(), 0.0, 1e-10);
}

//==================================================================================================
// Complex compound structure tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, MixedHierarchy_DifferentLevels)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aRoot;
  aBuilder.MakeCompound(aRoot);

  // Add direct vertex
  aBuilder.Add(aRoot, BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0)));

  // Add compound with vertices
  TopoDS_Compound aSub1;
  aBuilder.MakeCompound(aSub1);
  aBuilder.Add(aSub1, BRepBuilderAPI_MakeVertex(gp_Pnt(1, 0, 0)));
  aBuilder.Add(aSub1, BRepBuilderAPI_MakeVertex(gp_Pnt(2, 0, 0)));
  aBuilder.Add(aRoot, aSub1);

  // Add nested compound
  TopoDS_Compound aSub2, aSubSub;
  aBuilder.MakeCompound(aSub2);
  aBuilder.MakeCompound(aSubSub);
  aBuilder.Add(aSubSub, BRepBuilderAPI_MakeVertex(gp_Pnt(3, 0, 0)));
  aBuilder.Add(aSub2, aSubSub);
  aBuilder.Add(aRoot, aSub2);

  // Root should have 3 direct children
  int aRootChildCount = 0;
  for (TopoDS_Iterator anIter(aRoot); anIter.More(); anIter.Next())
  {
    ++aRootChildCount;
  }
  EXPECT_EQ(aRootChildCount, 3);
}

TEST(TopoDS_Iterator_Test, DeeplyNested_TenLevels)
{
  BRep_Builder aBuilder;

  // Create 10-level deep hierarchy
  TopoDS_Compound aLevels[10];
  for (int i = 0; i < 10; ++i)
  {
    aBuilder.MakeCompound(aLevels[i]);
  }

  // Add vertex at deepest level
  aBuilder.Add(aLevels[9], BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0)));

  // Link levels
  for (int i = 8; i >= 0; --i)
  {
    aBuilder.Add(aLevels[i], aLevels[i + 1]);
  }

  // Traverse all levels
  TopoDS_Shape aCurrent = aLevels[0];
  for (int i = 0; i < 10; ++i)
  {
    TopoDS_Iterator anIter(aCurrent);
    EXPECT_TRUE(anIter.More());
    aCurrent = anIter.Value();
  }

  // Final shape should be a vertex
  EXPECT_EQ(aCurrent.ShapeType(), TopAbs_VERTEX);
}

//==================================================================================================
// Shared sub-shapes iteration
//==================================================================================================

TEST(TopoDS_Iterator_Test, SharedVertex_InMultipleEdges)
{
  BRep_Builder aBuilder;

  TopoDS_Vertex aSharedVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));
  TopoDS_Vertex aVertex1      = BRepBuilderAPI_MakeVertex(gp_Pnt(1, 0, 0));
  TopoDS_Vertex aVertex2      = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 1, 0));

  TopoDS_Edge anEdge1, anEdge2;
  aBuilder.MakeEdge(anEdge1);
  aBuilder.MakeEdge(anEdge2);

  aBuilder.Add(anEdge1, aSharedVertex);
  aBuilder.Add(anEdge1, aVertex1);
  aBuilder.Add(anEdge2, aSharedVertex);
  aBuilder.Add(anEdge2, aVertex2);

  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, anEdge1);
  aBuilder.Add(aCompound, anEdge2);

  // Iterate edges and collect vertices
  NCollection_List<TopoDS_Shape> aAllVertices;
  for (TopoDS_Iterator aCompIter(aCompound); aCompIter.More(); aCompIter.Next())
  {
    for (TopoDS_Iterator anEdgeIter(aCompIter.Value()); anEdgeIter.More(); anEdgeIter.Next())
    {
      aAllVertices.Append(anEdgeIter.Value());
    }
  }

  // Should have 4 vertices total (2 per edge)
  EXPECT_EQ(aAllVertices.Size(), 4);

  // Check that the shared vertex appears twice
  int aSharedCount = 0;
  for (const TopoDS_Shape& aV : aAllVertices)
  {
    if (aV.IsPartner(aSharedVertex))
    {
      ++aSharedCount;
    }
  }
  EXPECT_EQ(aSharedCount, 2);
}

//==================================================================================================
// Iteration order and consistency tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, OrderPreserved_FIFO)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  // Add vertices at specific coordinates
  const int aNb = 10;
  for (int i = 0; i < aNb; ++i)
  {
    aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(i, 0, 0)));
  }

  // Verify order is preserved (FIFO)
  int anExpected = 0;
  for (TopoDS_Iterator anIter(aCompound); anIter.More(); anIter.Next())
  {
    gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(anIter.Value()));
    EXPECT_NEAR(aPnt.X(), anExpected, 1e-10);
    ++anExpected;
  }
  EXPECT_EQ(anExpected, aNb);
}

TEST(TopoDS_Iterator_Test, MultipleFullPasses_SameOrder)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  for (int i = 0; i < 5; ++i)
  {
    aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(i * 10, i * 20, i * 30)));
  }

  // Collect shapes from first pass
  NCollection_List<TopoDS_Shape> aFirstPass;
  for (TopoDS_Iterator anIter(aCompound); anIter.More(); anIter.Next())
  {
    aFirstPass.Append(anIter.Value());
  }

  // Verify subsequent passes match
  for (int aPassNum = 0; aPassNum < 3; ++aPassNum)
  {
    auto anExpectedIt = aFirstPass.begin();
    for (TopoDS_Iterator anIter(aCompound); anIter.More(); anIter.Next())
    {
      ASSERT_NE(anExpectedIt, aFirstPass.end());
      EXPECT_TRUE(anIter.Value().IsEqual(*anExpectedIt));
      ++anExpectedIt;
    }
  }
}

//==================================================================================================
// Reinitialize with different flags
//==================================================================================================

TEST(TopoDS_Iterator_Test, Reinitialize_DifferentFlags)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));
  aBuilder.Add(aCompound, aVertex.Reversed());

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(10, 20, 30));
  TopoDS_Shape aLocatedCompound = aCompound.Located(TopLoc_Location(aTrsf));

  TopoDS_Iterator anIter;

  // With cumulative orientation
  anIter.Initialize(aLocatedCompound, true, false);
  EXPECT_TRUE(anIter.More());
  EXPECT_EQ(anIter.Value().Orientation(), TopAbs_REVERSED);

  // Without cumulative orientation
  anIter.Initialize(aLocatedCompound, false, false);
  EXPECT_TRUE(anIter.More());
  EXPECT_EQ(anIter.Value().Orientation(), TopAbs_REVERSED);

  // With cumulative location
  anIter.Initialize(aLocatedCompound, true, true);
  EXPECT_TRUE(anIter.More());
  EXPECT_FALSE(anIter.Value().Location().IsIdentity());

  // Without cumulative location
  anIter.Initialize(aLocatedCompound, true, false);
  EXPECT_TRUE(anIter.More());
  EXPECT_TRUE(anIter.Value().Location().IsIdentity());
}

//==================================================================================================
// Single child tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, SingleChild_Compound)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(5, 5, 5));
  aBuilder.Add(aCompound, aVertex);

  TopoDS_Iterator anIter(aCompound);
  EXPECT_TRUE(anIter.More());
  EXPECT_TRUE(anIter.Value().IsPartner(aVertex));

  anIter.Next();
  EXPECT_FALSE(anIter.More());
}

TEST(TopoDS_Iterator_Test, SingleChild_Wire)
{
  BRep_Builder aBuilder;

  TopoDS_Wire aWire;
  aBuilder.MakeWire(aWire);

  TopoDS_Edge anEdge;
  aBuilder.MakeEdge(anEdge);
  aBuilder.Add(aWire, anEdge);

  TopoDS_Iterator anIter(aWire);
  EXPECT_TRUE(anIter.More());
  EXPECT_EQ(anIter.Value().ShapeType(), TopAbs_EDGE);

  anIter.Next();
  EXPECT_FALSE(anIter.More());
}

//==================================================================================================
// Stress tests
//==================================================================================================

TEST(TopoDS_Iterator_Test, Stress_WideAndDeep)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aRoot;
  aBuilder.MakeCompound(aRoot);

  const int aNbBranches        = 5;
  const int aNbLeavesPerBranch = 10;

  for (int iBranch = 0; iBranch < aNbBranches; ++iBranch)
  {
    TopoDS_Compound aBranch;
    aBuilder.MakeCompound(aBranch);

    // Add leaves directly to each branch
    for (int iLeaf = 0; iLeaf < aNbLeavesPerBranch; ++iLeaf)
    {
      aBuilder.Add(aBranch, BRepBuilderAPI_MakeVertex(gp_Pnt(iBranch, iLeaf, 0)));
    }

    aBuilder.Add(aRoot, aBranch);
  }

  // Count root children
  int aRootCount = 0;
  for (TopoDS_Iterator anIter(aRoot); anIter.More(); anIter.Next())
  {
    ++aRootCount;
    // Each branch should have aNbLeavesPerBranch children
    int aLeafCount = 0;
    for (TopoDS_Iterator aBranchIter(anIter.Value()); aBranchIter.More(); aBranchIter.Next())
    {
      ++aLeafCount;
    }
    EXPECT_EQ(aLeafCount, aNbLeavesPerBranch);
  }
  EXPECT_EQ(aRootCount, aNbBranches);
}

TEST(TopoDS_Iterator_Test, Stress_RapidReinitialization)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  for (int i = 0; i < 10; ++i)
  {
    aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(i, 0, 0)));
  }

  TopoDS_Iterator anIter;

  // Rapid reinitialize 1000 times
  for (int i = 0; i < 1000; ++i)
  {
    anIter.Initialize(aCompound);
    EXPECT_TRUE(anIter.More());

    int aCount = 0;
    for (; anIter.More(); anIter.Next())
    {
      ++aCount;
    }
    EXPECT_EQ(aCount, 10);
  }
}

//==================================================================================================
// All four orientations in one compound
//==================================================================================================

TEST(TopoDS_Iterator_Test, AllOrientations_InOneCompound)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));

  aBuilder.Add(aCompound, aVertex);                           // FORWARD
  aBuilder.Add(aCompound, aVertex.Reversed());                // REVERSED
  aBuilder.Add(aCompound, aVertex.Oriented(TopAbs_INTERNAL)); // INTERNAL
  aBuilder.Add(aCompound, aVertex.Oriented(TopAbs_EXTERNAL)); // EXTERNAL

  int aOrientCounts[4] = {0, 0, 0, 0};
  for (TopoDS_Iterator anIter(aCompound, true); anIter.More(); anIter.Next())
  {
    aOrientCounts[anIter.Value().Orientation()]++;
  }

  EXPECT_EQ(aOrientCounts[TopAbs_FORWARD], 1);
  EXPECT_EQ(aOrientCounts[TopAbs_REVERSED], 1);
  EXPECT_EQ(aOrientCounts[TopAbs_INTERNAL], 1);
  EXPECT_EQ(aOrientCounts[TopAbs_EXTERNAL], 1);
}

//==================================================================================================
// Iterator on reversed parent with all child orientations
//==================================================================================================

TEST(TopoDS_Iterator_Test, ReversedParent_AllChildOrientations)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));

  aBuilder.Add(aCompound, aVertex);                           // FORWARD
  aBuilder.Add(aCompound, aVertex.Reversed());                // REVERSED
  aBuilder.Add(aCompound, aVertex.Oriented(TopAbs_INTERNAL)); // INTERNAL
  aBuilder.Add(aCompound, aVertex.Oriented(TopAbs_EXTERNAL)); // EXTERNAL

  // Iterate on reversed compound with cumulative orientation
  TopoDS_Shape aReversedCompound = aCompound.Reversed();

  int aOrientCounts[4] = {0, 0, 0, 0};
  for (TopoDS_Iterator anIter(aReversedCompound, true); anIter.More(); anIter.Next())
  {
    aOrientCounts[anIter.Value().Orientation()]++;
  }

  // FORWARD <-> REVERSED, INTERNAL/EXTERNAL unchanged
  EXPECT_EQ(aOrientCounts[TopAbs_FORWARD], 1);  // was REVERSED
  EXPECT_EQ(aOrientCounts[TopAbs_REVERSED], 1); // was FORWARD
  EXPECT_EQ(aOrientCounts[TopAbs_INTERNAL], 1);
  EXPECT_EQ(aOrientCounts[TopAbs_EXTERNAL], 1);
}

//==================================================================================================
// Empty iteration edge cases
//==================================================================================================

TEST(TopoDS_Iterator_Test, EmptyCompound_MoreReturnsFalse)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  TopoDS_Iterator anIter(aCompound);
  EXPECT_FALSE(anIter.More());
}

TEST(TopoDS_Iterator_Test, SingleElement_AfterNextMoreFalse)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0)));

  TopoDS_Iterator anIter(aCompound);
  EXPECT_TRUE(anIter.More());

  anIter.Next();
  EXPECT_FALSE(anIter.More());
}

//==================================================================================================
// Iterator with compound containing real solid
//==================================================================================================

TEST(TopoDS_Iterator_Test, CompoundWithBox_IterateSolid)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  aBuilder.Add(aCompound, aBoxMaker.Shape());

  TopoDS_Iterator anIter(aCompound);
  EXPECT_TRUE(anIter.More());
  EXPECT_EQ(anIter.Value().ShapeType(), TopAbs_SOLID);

  anIter.Next();
  EXPECT_FALSE(anIter.More());
}

TEST(TopoDS_Iterator_Test, CompoundWithMultipleBoxes)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  const int aNbBoxes = 5;
  for (int i = 0; i < aNbBoxes; ++i)
  {
    BRepPrimAPI_MakeBox aBoxMaker(10.0 + i, 20.0 + i, 30.0 + i);
    aBuilder.Add(aCompound, aBoxMaker.Shape());
  }

  int aSolidCount = 0;
  for (TopoDS_Iterator anIter(aCompound); anIter.More(); anIter.Next())
  {
    EXPECT_EQ(anIter.Value().ShapeType(), TopAbs_SOLID);
    ++aSolidCount;
  }
  EXPECT_EQ(aSolidCount, aNbBoxes);
}

//==================================================================================================
// Compound with located boxes
//==================================================================================================

TEST(TopoDS_Iterator_Test, CompoundWithLocatedBoxes_CumulativeLocation)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  BRepPrimAPI_MakeBox aBoxMaker(10.0, 10.0, 10.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  // Add box at different locations
  for (int i = 0; i < 3; ++i)
  {
    gp_Trsf aTrsf;
    aTrsf.SetTranslation(gp_Vec(i * 20, 0, 0));
    aBuilder.Add(aCompound, aBox.Located(TopLoc_Location(aTrsf)));
  }

  // Verify each box has different location
  double aLastX = -100;
  for (TopoDS_Iterator anIter(aCompound, true, false); anIter.More(); anIter.Next())
  {
    gp_Trsf aTrsf = anIter.Value().Location().Transformation();
    double  aX    = aTrsf.TranslationPart().X();
    EXPECT_GT(aX, aLastX);
    aLastX = aX;
  }
}

//==================================================================================================
// TShape partner tests through iteration
//==================================================================================================

TEST(TopoDS_Iterator_Test, SameTShape_DifferentOrientationsInIteration)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(1, 2, 3));

  // Add same TShape with different orientations
  aBuilder.Add(aCompound, aVertex);
  aBuilder.Add(aCompound, aVertex.Reversed());

  TopoDS_Iterator anIter(aCompound);
  EXPECT_TRUE(anIter.More());
  TopoDS_Shape aFirst = anIter.Value();

  anIter.Next();
  EXPECT_TRUE(anIter.More());
  TopoDS_Shape aSecond = anIter.Value();

  // Same TShape (IsPartner), but different orientation
  EXPECT_TRUE(aFirst.IsPartner(aSecond));
  EXPECT_NE(aFirst.Orientation(), aSecond.Orientation());
}

//==================================================================================================
// Verify iteration doesn't modify shapes
//==================================================================================================

TEST(TopoDS_Iterator_Test, IterationDoesNotModifyShapes)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  NCollection_List<TopoDS_Shape> aOriginals;
  for (int i = 0; i < 5; ++i)
  {
    TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(i, i * 2, i * 3));
    aOriginals.Append(aVertex);
    aBuilder.Add(aCompound, aVertex);
  }

  // Iterate multiple times
  for (int aPass = 0; aPass < 10; ++aPass)
  {
    for (TopoDS_Iterator anIter(aCompound); anIter.More(); anIter.Next())
    {
      // Just iterate
      (void)anIter.Value();
    }
  }

  // Verify shapes unchanged
  auto anOrigIt = aOriginals.begin();
  for (TopoDS_Iterator anIter(aCompound); anIter.More(); anIter.Next())
  {
    ASSERT_NE(anOrigIt, aOriginals.end());
    EXPECT_TRUE(anIter.Value().IsEqual(*anOrigIt));
    ++anOrigIt;
  }
}
