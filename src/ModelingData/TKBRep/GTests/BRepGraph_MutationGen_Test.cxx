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

#include <BRepGraph.hxx>
#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepPrimAPI_MakeBox.hxx>

#include <gtest/gtest.h>

class BRepGraph_MutationGenTest : public testing::Test
{
protected:
  void SetUp() override
  {
    BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
    const TopoDS_Shape& aBox = aBoxMaker.Shape();
    myGraph.Build(aBox);
    ASSERT_TRUE(myGraph.IsDone());
  }

  BRepGraph myGraph;
};

TEST_F(BRepGraph_MutationGenTest, MutationGen_IncrementedOnMutation)
{
  EXPECT_EQ(myGraph.Topo().Edge(BRepGraph_EdgeId(0)).MutationGen, 0u);

  myGraph.Builder().MutEdge(BRepGraph_EdgeId(0))->Tolerance = 0.5;

  EXPECT_EQ(myGraph.Topo().Edge(BRepGraph_EdgeId(0)).MutationGen, 1u);
}

TEST_F(BRepGraph_MutationGenTest, MutationGen_MultipleIncrements)
{
  myGraph.Builder().MutEdge(BRepGraph_EdgeId(0))->Tolerance = 0.1;
  myGraph.Builder().MutEdge(BRepGraph_EdgeId(0))->Tolerance = 0.2;

  EXPECT_EQ(myGraph.Topo().Edge(BRepGraph_EdgeId(0)).MutationGen, 2u);
}

TEST_F(BRepGraph_MutationGenTest, MutationGen_DeferredMode)
{
  myGraph.Builder().BeginDeferredInvalidation();
  myGraph.Builder().MutEdge(BRepGraph_EdgeId(0))->Tolerance = 0.5;

  // MutationGen is incremented even in deferred mode.
  EXPECT_EQ(myGraph.Topo().Edge(BRepGraph_EdgeId(0)).MutationGen, 1u);

  myGraph.Builder().EndDeferredInvalidation();

  // Still 1 after flush - flush doesn't re-increment.
  EXPECT_EQ(myGraph.Topo().Edge(BRepGraph_EdgeId(0)).MutationGen, 1u);
}

TEST_F(BRepGraph_MutationGenTest, MutationGen_PropagatedParent_NotIncremented)
{
  // Mutate an edge - parent wire/face/shell/solid get IsModified via propagation,
  // but their MutationGen must stay 0 (they weren't directly mutated).
  const int aNbWires  = myGraph.Topo().NbWires();
  const int aNbFaces  = myGraph.Topo().NbFaces();
  const int aNbShells = myGraph.Topo().NbShells();
  const int aNbSolids = myGraph.Topo().NbSolids();

  myGraph.Builder().MutEdge(BRepGraph_EdgeId(0))->Tolerance = 0.5;

  EXPECT_EQ(myGraph.Topo().Edge(BRepGraph_EdgeId(0)).MutationGen, 1u);

  for (int i = 0; i < aNbWires; ++i)
    EXPECT_EQ(myGraph.Topo().Wire(BRepGraph_WireId(i)).MutationGen, 0u);
  for (int i = 0; i < aNbFaces; ++i)
    EXPECT_EQ(myGraph.Topo().Face(BRepGraph_FaceId(i)).MutationGen, 0u);
  for (int i = 0; i < aNbShells; ++i)
    EXPECT_EQ(myGraph.Topo().Shell(BRepGraph_ShellId(i)).MutationGen, 0u);
  for (int i = 0; i < aNbSolids; ++i)
    EXPECT_EQ(myGraph.Topo().Solid(BRepGraph_SolidId(i)).MutationGen, 0u);
}

TEST_F(BRepGraph_MutationGenTest, MutationGen_DeferredPropagatedParent_NotIncremented)
{
  // Same as above but in deferred mode - propagation on flush must not
  // increment MutationGen on parents.
  myGraph.Builder().BeginDeferredInvalidation();
  myGraph.Builder().MutEdge(BRepGraph_EdgeId(0))->Tolerance = 0.5;
  myGraph.Builder().EndDeferredInvalidation();

  EXPECT_EQ(myGraph.Topo().Edge(BRepGraph_EdgeId(0)).MutationGen, 1u);

  for (int i = 0; i < myGraph.Topo().NbWires(); ++i)
    EXPECT_EQ(myGraph.Topo().Wire(BRepGraph_WireId(i)).MutationGen, 0u);
  for (int i = 0; i < myGraph.Topo().NbFaces(); ++i)
    EXPECT_EQ(myGraph.Topo().Face(BRepGraph_FaceId(i)).MutationGen, 0u);
  for (int i = 0; i < myGraph.Topo().NbShells(); ++i)
    EXPECT_EQ(myGraph.Topo().Shell(BRepGraph_ShellId(i)).MutationGen, 0u);
  for (int i = 0; i < myGraph.Topo().NbSolids(); ++i)
    EXPECT_EQ(myGraph.Topo().Solid(BRepGraph_SolidId(i)).MutationGen, 0u);
}
