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
#include <gp_Pnt.hxx>
#include <NCollection_HSequence.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>

#include <gtest/gtest.h>

namespace
{
struct TestData
{
  TopoDS_Edge Edge1;
  TopoDS_Edge Edge2;
  TopoDS_Edge Edge3;
  TopoDS_Edge InternalEdge;
  TopoDS_Edge ExternalEdge;
  TopoDS_Wire Loop;
  TopoDS_Wire InternalWire;
  TopoDS_Wire ExternalWire;
  TopoDS_Wire EmptyWire;
};

static bool makeTestData(TestData& theData)
{
  BRep_Builder  aBuilder;
  TopoDS_Vertex aVertex1, aVertex2, aVertex3, aNonManifoldVertex;
  aBuilder.MakeVertex(aVertex1, gp_Pnt(0.0, 0.0, 0.0), Precision::Confusion());
  aBuilder.MakeVertex(aVertex2, gp_Pnt(1.0, 0.0, 0.0), Precision::Confusion());
  aBuilder.MakeVertex(aVertex3, gp_Pnt(0.0, 1.0, 0.0), Precision::Confusion());
  aBuilder.MakeVertex(aNonManifoldVertex, gp_Pnt(2.0, -1.0, 0.0), Precision::Confusion());

  BRepBuilderAPI_MakeEdge anEdgeMaker1(aVertex1, aVertex2);
  BRepBuilderAPI_MakeEdge anEdgeMaker2(aVertex2, aVertex3);
  BRepBuilderAPI_MakeEdge anEdgeMaker3(aVertex3, aVertex1);
  BRepBuilderAPI_MakeEdge aNonManifoldMaker(aVertex1, aNonManifoldVertex);
  if (!anEdgeMaker1.IsDone() || !anEdgeMaker2.IsDone() || !anEdgeMaker3.IsDone()
      || !aNonManifoldMaker.IsDone())
  {
    return false;
  }

  theData.Edge1        = anEdgeMaker1.Edge();
  theData.Edge2        = anEdgeMaker2.Edge();
  theData.Edge3        = anEdgeMaker3.Edge();
  theData.InternalEdge = aNonManifoldMaker.Edge();
  theData.InternalEdge.Orientation(TopAbs_INTERNAL);
  theData.ExternalEdge = aNonManifoldMaker.Edge();
  theData.ExternalEdge.Orientation(TopAbs_EXTERNAL);

  aBuilder.MakeWire(theData.Loop);
  aBuilder.Add(theData.Loop, theData.Edge1);
  aBuilder.Add(theData.Loop, theData.Edge2);
  aBuilder.Add(theData.Loop, theData.Edge3);
  aBuilder.MakeWire(theData.InternalWire);
  aBuilder.Add(theData.InternalWire, theData.InternalEdge);
  aBuilder.MakeWire(theData.ExternalWire);
  aBuilder.Add(theData.ExternalWire, theData.ExternalEdge);
  aBuilder.MakeWire(theData.EmptyWire);
  return true;
}

static void checkResult(const occ::handle<NCollection_HSequence<TopoDS_Shape>>& theResult,
                        const int                                               theNbWires,
                        const int                                               theNbEdges,
                        const int                                               theNbInternalEdges,
                        const int                                               theNbExternalEdges,
                        const bool                                              theHasClosedLoop)
{
  ASSERT_FALSE(theResult.IsNull());
  ASSERT_EQ(theResult->Length(), theNbWires);

  int  aNbEdges         = 0;
  int  aNbInternalEdges = 0;
  int  aNbExternalEdges = 0;
  bool hasClosedLoop    = false;
  for (const TopoDS_Shape& aWire : *theResult)
  {
    int aNbWireEdges = 0;
    for (TopExp_Explorer anExp(aWire, TopAbs_EDGE); anExp.More(); anExp.Next())
    {
      ++aNbEdges;
      ++aNbWireEdges;
      if (anExp.Current().Orientation() == TopAbs_INTERNAL)
      {
        ++aNbInternalEdges;
      }
      else if (anExp.Current().Orientation() == TopAbs_EXTERNAL)
      {
        ++aNbExternalEdges;
      }
    }
    if (aNbWireEdges == 3 && aWire.Closed())
    {
      hasClosedLoop = true;
    }
  }
  EXPECT_EQ(aNbEdges, theNbEdges);
  EXPECT_EQ(aNbInternalEdges, theNbInternalEdges);
  EXPECT_EQ(aNbExternalEdges, theNbExternalEdges);
  EXPECT_EQ(hasClosedLoop, theHasClosedLoop);
}

static TopoDS_Face MakeSquareFace(const gp_Pnt& theA,
                                  const gp_Pnt& theB,
                                  const gp_Pnt& theC,
                                  const gp_Pnt& theD)
{
  BRepBuilderAPI_MakeWire aMakeWire;
  aMakeWire.Add(BRepBuilderAPI_MakeEdge(theA, theB).Edge());
  aMakeWire.Add(BRepBuilderAPI_MakeEdge(theB, theC).Edge());
  aMakeWire.Add(BRepBuilderAPI_MakeEdge(theC, theD).Edge());
  aMakeWire.Add(BRepBuilderAPI_MakeEdge(theD, theA).Edge());
  return BRepBuilderAPI_MakeFace(aMakeWire.Wire()).Face();
}
} // namespace

TEST(ShapeAnalysis_FreeBoundsTest, ConnectWires_InternalOnlyWireAfterManifoldWire)
{
  TestData aData;
  ASSERT_TRUE(makeTestData(aData));
  occ::handle<NCollection_HSequence<TopoDS_Shape>> anInput =
    new NCollection_HSequence<TopoDS_Shape>();
  anInput->Append(aData.Loop);
  anInput->Append(aData.InternalWire);

  occ::handle<NCollection_HSequence<TopoDS_Shape>> aResult =
    ShapeAnalysis_FreeBounds::ConnectWiresToWires(anInput, Precision::Confusion(), true);
  checkResult(aResult, 2, 4, 1, 0, true);
}

TEST(ShapeAnalysis_FreeBoundsTest, ConnectWires_InternalOnlyWireBeforeManifoldWire)
{
  TestData aData;
  ASSERT_TRUE(makeTestData(aData));
  occ::handle<NCollection_HSequence<TopoDS_Shape>> anInput =
    new NCollection_HSequence<TopoDS_Shape>();
  anInput->Append(aData.InternalWire);
  anInput->Append(aData.Loop);

  occ::handle<NCollection_HSequence<TopoDS_Shape>> aResult =
    ShapeAnalysis_FreeBounds::ConnectWiresToWires(anInput, Precision::Confusion(), true);
  checkResult(aResult, 2, 4, 1, 0, true);
}

TEST(ShapeAnalysis_FreeBoundsTest, ConnectWires_OnlyNonManifoldWires)
{
  TestData aData;
  ASSERT_TRUE(makeTestData(aData));
  occ::handle<NCollection_HSequence<TopoDS_Shape>> anInput =
    new NCollection_HSequence<TopoDS_Shape>();
  anInput->Append(aData.InternalWire);
  anInput->Append(aData.ExternalWire);

  occ::handle<NCollection_HSequence<TopoDS_Shape>> aResult =
    ShapeAnalysis_FreeBounds::ConnectWiresToWires(anInput, Precision::Confusion(), true);
  checkResult(aResult, 2, 2, 1, 1, false);
}

TEST(ShapeAnalysis_FreeBoundsTest, ConnectWires_EmptyWireBeforeValidWires)
{
  TestData aData;
  ASSERT_TRUE(makeTestData(aData));
  occ::handle<NCollection_HSequence<TopoDS_Shape>> anInput =
    new NCollection_HSequence<TopoDS_Shape>();
  anInput->Append(aData.EmptyWire);
  anInput->Append(aData.InternalWire);
  anInput->Append(aData.Loop);

  occ::handle<NCollection_HSequence<TopoDS_Shape>> aResult =
    ShapeAnalysis_FreeBounds::ConnectWiresToWires(anInput, Precision::Confusion(), true);
  checkResult(aResult, 2, 4, 1, 0, true);
}

TEST(ShapeAnalysis_FreeBoundsTest, ConnectWires_OnlyEmptyWire)
{
  TestData aData;
  ASSERT_TRUE(makeTestData(aData));
  occ::handle<NCollection_HSequence<TopoDS_Shape>> anInput =
    new NCollection_HSequence<TopoDS_Shape>();
  anInput->Append(aData.EmptyWire);

  occ::handle<NCollection_HSequence<TopoDS_Shape>> aResult =
    ShapeAnalysis_FreeBounds::ConnectWiresToWires(anInput, Precision::Confusion(), true);
  checkResult(aResult, 0, 0, 0, 0, false);
}

TEST(ShapeAnalysis_FreeBoundsTest, ConnectWires_MixedManifoldAndInternalEdges)
{
  TestData aData;
  ASSERT_TRUE(makeTestData(aData));
  BRep_Builder aBuilder;
  TopoDS_Wire  aMixedWire;
  aBuilder.MakeWire(aMixedWire);
  aBuilder.Add(aMixedWire, aData.Edge1);
  aBuilder.Add(aMixedWire, aData.Edge2);
  aBuilder.Add(aMixedWire, aData.Edge3);
  aBuilder.Add(aMixedWire, aData.InternalEdge);

  occ::handle<NCollection_HSequence<TopoDS_Shape>> anInput =
    new NCollection_HSequence<TopoDS_Shape>();
  anInput->Append(aMixedWire);

  occ::handle<NCollection_HSequence<TopoDS_Shape>> aResult =
    ShapeAnalysis_FreeBounds::ConnectWiresToWires(anInput, Precision::Confusion(), true);
  checkResult(aResult, 1, 4, 1, 0, false);
}

TEST(ShapeAnalysis_FreeBoundsTest, ConnectEdges_InternalEdgeAfterManifoldEdges)
{
  TestData aData;
  ASSERT_TRUE(makeTestData(aData));
  occ::handle<NCollection_HSequence<TopoDS_Shape>> anInput =
    new NCollection_HSequence<TopoDS_Shape>();
  anInput->Append(aData.Edge1);
  anInput->Append(aData.Edge2);
  anInput->Append(aData.Edge3);
  anInput->Append(aData.InternalEdge);

  occ::handle<NCollection_HSequence<TopoDS_Shape>> aResult =
    ShapeAnalysis_FreeBounds::ConnectEdgesToWires(anInput, Precision::Confusion(), true);
  checkResult(aResult, 2, 4, 1, 0, true);
}

TEST(ShapeAnalysis_FreeBoundsTest, ConnectEdges_InternalEdgeBeforeManifoldEdges)
{
  TestData aData;
  ASSERT_TRUE(makeTestData(aData));
  occ::handle<NCollection_HSequence<TopoDS_Shape>> anInput =
    new NCollection_HSequence<TopoDS_Shape>();
  anInput->Append(aData.InternalEdge);
  anInput->Append(aData.Edge1);
  anInput->Append(aData.Edge2);
  anInput->Append(aData.Edge3);

  occ::handle<NCollection_HSequence<TopoDS_Shape>> aResult =
    ShapeAnalysis_FreeBounds::ConnectEdgesToWires(anInput, Precision::Confusion(), true);
  checkResult(aResult, 2, 4, 1, 0, true);
}

// Baseline: a single closed face alone does not trigger the bug (see below).
TEST(ShapeAnalysis_FreeBoundsTest, SingleFaceNoCrash)
{
  const TopoDS_Face aFace = MakeSquareFace(gp_Pnt(0.0, 0.0, 0.0),
                                           gp_Pnt(1.0, 0.0, 0.0),
                                           gp_Pnt(1.0, 1.0, 0.0),
                                           gp_Pnt(0.0, 1.0, 0.0));

  ShapeAnalysis_FreeBounds aFreeBounds(aFace, 0.01, true, true);
  const TopoDS_Compound&   aClosed = aFreeBounds.GetClosedWires();
  const TopoDS_Compound&   anOpen  = aFreeBounds.GetOpenWires();
  (void)aClosed;
  (void)anOpen;
}

// Regression: two disjoint closed faces used to crash SplitWires (see PR description).
TEST(ShapeAnalysis_FreeBoundsTest, TwoDisjointFacesNoCrash)
{
  const TopoDS_Face aFaceA = MakeSquareFace(gp_Pnt(0.0, 0.0, 0.0),
                                            gp_Pnt(1.0, 0.0, 0.0),
                                            gp_Pnt(1.0, 1.0, 0.0),
                                            gp_Pnt(0.0, 1.0, 0.0));
  const TopoDS_Face aFaceB = MakeSquareFace(gp_Pnt(5.0, 5.0, 0.0),
                                            gp_Pnt(6.0, 5.0, 0.0),
                                            gp_Pnt(6.0, 6.0, 0.0),
                                            gp_Pnt(5.0, 6.0, 0.0));

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aFaceA);
  aBuilder.Add(aCompound, aFaceB);

  ShapeAnalysis_FreeBounds aFreeBounds(aCompound, 0.01, true, true);
  const TopoDS_Compound&   aClosed = aFreeBounds.GetClosedWires();
  const TopoDS_Compound&   anOpen  = aFreeBounds.GetOpenWires();

  int aNbClosed = 0;
  for (TopExp_Explorer anExp(aClosed, TopAbs_WIRE); anExp.More(); anExp.Next())
  {
    ++aNbClosed;
  }
  int aNbOpen = 0;
  for (TopExp_Explorer anExp(anOpen, TopAbs_WIRE); anExp.More(); anExp.Next())
  {
    ++aNbOpen;
  }

  EXPECT_EQ(aNbClosed, 2);
  EXPECT_EQ(aNbOpen, 0);
}
