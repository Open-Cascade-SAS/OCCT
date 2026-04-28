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

#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <NCollection_IndexedMap.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeFix_Shape.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

namespace
{
int CountSubShapes(const TopoDS_Shape& theShape, const TopAbs_ShapeEnum theType)
{
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMap;
  TopExp::MapShapes(theShape, theType, aMap);
  return aMap.Extent();
}
} // namespace

// Basic Apply() via ShapeBuild_ReShape (the subclass used throughout shape
// healing) must produce the expected vertex substitution on a compound.
TEST(ShapeBuild_ReShapeTest, Apply_PerformsVertexReplacementOnCompound)
{
  const TopoDS_Vertex aV1     = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));
  const TopoDS_Vertex aV2     = BRepBuilderAPI_MakeVertex(gp_Pnt(1, 0, 0));
  const TopoDS_Vertex aV2Repl = BRepBuilderAPI_MakeVertex(gp_Pnt(1.5, 0, 0));

  TopoDS_Compound aParent;
  BRep_Builder    aBuilder;
  aBuilder.MakeCompound(aParent);
  aBuilder.Add(aParent, aV1);
  aBuilder.Add(aParent, aV2);

  occ::handle<ShapeBuild_ReShape> aReShape = new ShapeBuild_ReShape;
  aReShape->Replace(aV2, aV2Repl);

  const TopoDS_Shape aResult = aReShape->Apply(aParent);
  ASSERT_FALSE(aResult.IsNull());

  bool aFoundReplacement = false;
  for (TopoDS_Iterator anIt(aResult); anIt.More(); anIt.Next())
  {
    if (anIt.Value().TShape() == aV2Repl.TShape())
      aFoundReplacement = true;
    EXPECT_FALSE(anIt.Value().TShape() == aV2.TShape());
  }
  EXPECT_TRUE(aFoundReplacement);
}

// Structural containment through ShapeBuild_ReShape's Apply override.
// The base class has its own DFS guard, the subclass has its own applyImpl;
// both paths must survive cyclic containment without a stack overflow.
TEST(ShapeBuild_ReShapeTest, Apply_StructuralContainmentWithoutCrash)
{
  const TopoDS_Vertex aV1 = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));
  const TopoDS_Vertex aV2 = BRepBuilderAPI_MakeVertex(gp_Pnt(1, 0, 0));
  const TopoDS_Vertex aV3 = BRepBuilderAPI_MakeVertex(gp_Pnt(2, 0, 0));
  const TopoDS_Edge   anE = BRepBuilderAPI_MakeEdge(aV1, aV2);

  TopoDS_Compound aContainer;
  BRep_Builder    aBuilder;
  aBuilder.MakeCompound(aContainer);
  aBuilder.Add(aContainer, anE);
  aBuilder.Add(aContainer, aV3);

  occ::handle<ShapeBuild_ReShape> aReShape = new ShapeBuild_ReShape;
  aReShape->Replace(anE, aContainer);

  TopoDS_Shape aResult;
  ASSERT_NO_FATAL_FAILURE(aResult = aReShape->Apply(anE));
  EXPECT_FALSE(aResult.IsNull());
}

// Regression test for the originally reported symptom: running shape healing
// repeatedly on the same shape must not cause the edge count to grow.
// Before the UpdateWire/Apply fixes, repeated ShapeFix_Shape::Perform on
// shapes with shared sub-shapes produced geometrically growing edge counts
// from cascading replacement chains.
TEST(ShapeFix_ShapeStabilityTest, RepeatedPerformDoesNotMultiplyEdges)
{
  BRepPrimAPI_MakeBox aMakeBox(10.0, 10.0, 10.0);
  const TopoDS_Shape  aBox = aMakeBox.Shape();
  ASSERT_TRUE(aMakeBox.IsDone());

  const int aNbEdgesInitial    = CountSubShapes(aBox, TopAbs_EDGE);
  const int aNbVerticesInitial = CountSubShapes(aBox, TopAbs_VERTEX);
  const int aNbFacesInitial    = CountSubShapes(aBox, TopAbs_FACE);
  ASSERT_EQ(aNbEdgesInitial, 12);

  TopoDS_Shape aCurrent = aBox;
  for (int anIter = 0; anIter < 5; ++anIter)
  {
    occ::handle<ShapeFix_Shape> aFixer = new ShapeFix_Shape(aCurrent);
    aFixer->Perform();
    aCurrent = aFixer->Shape();
    ASSERT_FALSE(aCurrent.IsNull()) << "iteration " << anIter;

    // Counts must remain identical across every iteration - no multiplication,
    // no loss of shapes on a valid input.
    EXPECT_EQ(CountSubShapes(aCurrent, TopAbs_FACE), aNbFacesInitial) << "iteration " << anIter;
    EXPECT_EQ(CountSubShapes(aCurrent, TopAbs_EDGE), aNbEdgesInitial) << "iteration " << anIter;
    EXPECT_EQ(CountSubShapes(aCurrent, TopAbs_VERTEX), aNbVerticesInitial)
      << "iteration " << anIter;
  }
}

// Diamond sharing through ShapeBuild_ReShape: two compounds share a vertex
// whose replacement is registered. Both parents must pick up the replacement
// and the DFS guard must not mistake the shared reference for a cycle.
TEST(ShapeBuild_ReShapeTest, Apply_DiamondSharedVertexInTwoCompounds)
{
  const TopoDS_Vertex aShared    = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));
  const TopoDS_Vertex aSharedNew = BRepBuilderAPI_MakeVertex(gp_Pnt(0.1, 0, 0));
  const TopoDS_Vertex anOther1   = BRepBuilderAPI_MakeVertex(gp_Pnt(1, 0, 0));
  const TopoDS_Vertex anOther2   = BRepBuilderAPI_MakeVertex(gp_Pnt(2, 0, 0));

  TopoDS_Compound aParentA, aParentB, aGrand;
  BRep_Builder    aBuilder;
  aBuilder.MakeCompound(aParentA);
  aBuilder.Add(aParentA, aShared);
  aBuilder.Add(aParentA, anOther1);
  aBuilder.MakeCompound(aParentB);
  aBuilder.Add(aParentB, aShared);
  aBuilder.Add(aParentB, anOther2);
  aBuilder.MakeCompound(aGrand);
  aBuilder.Add(aGrand, aParentA);
  aBuilder.Add(aGrand, aParentB);

  occ::handle<ShapeBuild_ReShape> aReShape = new ShapeBuild_ReShape;
  aReShape->Replace(aShared, aSharedNew);

  const TopoDS_Shape aResult = aReShape->Apply(aGrand);
  ASSERT_FALSE(aResult.IsNull());

  int aNbReplVerts = 0;
  int aNbOriginals = 0;
  for (TopExp_Explorer anExp(aResult, TopAbs_VERTEX); anExp.More(); anExp.Next())
  {
    if (anExp.Current().TShape() == aSharedNew.TShape())
      ++aNbReplVerts;
    if (anExp.Current().TShape() == aShared.TShape())
      ++aNbOriginals;
  }
  EXPECT_EQ(aNbOriginals, 0) << "Original vertex must be substituted everywhere";
  EXPECT_EQ(aNbReplVerts, 2) << "Replacement must appear in both parentA and parentB";
}
