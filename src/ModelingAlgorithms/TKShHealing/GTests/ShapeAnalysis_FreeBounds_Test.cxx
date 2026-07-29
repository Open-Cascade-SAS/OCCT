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
#include <gp_Pnt.hxx>
#include <NCollection_HSequence.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

#include <gtest/gtest.h>

namespace
{
class ShapeAnalysis_FreeBoundsTest : public testing::Test
{
protected:
  void SetUp() override
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

    ASSERT_TRUE(anEdgeMaker1.IsDone());
    ASSERT_TRUE(anEdgeMaker2.IsDone());
    ASSERT_TRUE(anEdgeMaker3.IsDone());
    ASSERT_TRUE(aNonManifoldMaker.IsDone());

    myEdge1        = anEdgeMaker1.Edge();
    myEdge2        = anEdgeMaker2.Edge();
    myEdge3        = anEdgeMaker3.Edge();
    myInternalEdge = aNonManifoldMaker.Edge();
    myInternalEdge.Orientation(TopAbs_INTERNAL);
    myExternalEdge = aNonManifoldMaker.Edge();
    myExternalEdge.Orientation(TopAbs_EXTERNAL);

    aBuilder.MakeWire(myLoop);
    aBuilder.Add(myLoop, myEdge1);
    aBuilder.Add(myLoop, myEdge2);
    aBuilder.Add(myLoop, myEdge3);
    aBuilder.MakeWire(myInternalWire);
    aBuilder.Add(myInternalWire, myInternalEdge);
    aBuilder.MakeWire(myExternalWire);
    aBuilder.Add(myExternalWire, myExternalEdge);
    aBuilder.MakeWire(myEmptyWire);
  }

  void checkResult(const occ::handle<NCollection_HSequence<TopoDS_Shape>>& theResult,
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

  TopoDS_Edge myEdge1;
  TopoDS_Edge myEdge2;
  TopoDS_Edge myEdge3;
  TopoDS_Edge myInternalEdge;
  TopoDS_Edge myExternalEdge;
  TopoDS_Wire myLoop;
  TopoDS_Wire myInternalWire;
  TopoDS_Wire myExternalWire;
  TopoDS_Wire myEmptyWire;
};
} // namespace

TEST_F(ShapeAnalysis_FreeBoundsTest, ConnectWires_InternalOnlyWireAfterManifoldWire)
{
  occ::handle<NCollection_HSequence<TopoDS_Shape>> anInput =
    new NCollection_HSequence<TopoDS_Shape>();
  anInput->Append(myLoop);
  anInput->Append(myInternalWire);

  occ::handle<NCollection_HSequence<TopoDS_Shape>> aResult =
    ShapeAnalysis_FreeBounds::ConnectWiresToWires(anInput, Precision::Confusion(), true);
  checkResult(aResult, 2, 4, 1, 0, true);
}

TEST_F(ShapeAnalysis_FreeBoundsTest, ConnectWires_InternalOnlyWireBeforeManifoldWire)
{
  occ::handle<NCollection_HSequence<TopoDS_Shape>> anInput =
    new NCollection_HSequence<TopoDS_Shape>();
  anInput->Append(myInternalWire);
  anInput->Append(myLoop);

  occ::handle<NCollection_HSequence<TopoDS_Shape>> aResult =
    ShapeAnalysis_FreeBounds::ConnectWiresToWires(anInput, Precision::Confusion(), true);
  checkResult(aResult, 2, 4, 1, 0, true);
}

TEST_F(ShapeAnalysis_FreeBoundsTest, ConnectWires_OnlyNonManifoldWires)
{
  occ::handle<NCollection_HSequence<TopoDS_Shape>> anInput =
    new NCollection_HSequence<TopoDS_Shape>();
  anInput->Append(myInternalWire);
  anInput->Append(myExternalWire);

  occ::handle<NCollection_HSequence<TopoDS_Shape>> aResult =
    ShapeAnalysis_FreeBounds::ConnectWiresToWires(anInput, Precision::Confusion(), true);
  checkResult(aResult, 2, 2, 1, 1, false);
}

TEST_F(ShapeAnalysis_FreeBoundsTest, ConnectWires_EmptyWireBeforeValidWires)
{
  occ::handle<NCollection_HSequence<TopoDS_Shape>> anInput =
    new NCollection_HSequence<TopoDS_Shape>();
  anInput->Append(myEmptyWire);
  anInput->Append(myInternalWire);
  anInput->Append(myLoop);

  occ::handle<NCollection_HSequence<TopoDS_Shape>> aResult =
    ShapeAnalysis_FreeBounds::ConnectWiresToWires(anInput, Precision::Confusion(), true);
  checkResult(aResult, 2, 4, 1, 0, true);
}

TEST_F(ShapeAnalysis_FreeBoundsTest, ConnectWires_OnlyEmptyWire)
{
  occ::handle<NCollection_HSequence<TopoDS_Shape>> anInput =
    new NCollection_HSequence<TopoDS_Shape>();
  anInput->Append(myEmptyWire);

  occ::handle<NCollection_HSequence<TopoDS_Shape>> aResult =
    ShapeAnalysis_FreeBounds::ConnectWiresToWires(anInput, Precision::Confusion(), true);
  checkResult(aResult, 0, 0, 0, 0, false);
}

TEST_F(ShapeAnalysis_FreeBoundsTest, ConnectWires_MixedManifoldAndInternalEdges)
{
  BRep_Builder aBuilder;
  TopoDS_Wire  aMixedWire;
  aBuilder.MakeWire(aMixedWire);
  aBuilder.Add(aMixedWire, myEdge1);
  aBuilder.Add(aMixedWire, myEdge2);
  aBuilder.Add(aMixedWire, myEdge3);
  aBuilder.Add(aMixedWire, myInternalEdge);

  occ::handle<NCollection_HSequence<TopoDS_Shape>> anInput =
    new NCollection_HSequence<TopoDS_Shape>();
  anInput->Append(aMixedWire);

  occ::handle<NCollection_HSequence<TopoDS_Shape>> aResult =
    ShapeAnalysis_FreeBounds::ConnectWiresToWires(anInput, Precision::Confusion(), true);
  checkResult(aResult, 1, 4, 1, 0, false);
}

TEST_F(ShapeAnalysis_FreeBoundsTest, ConnectEdges_InternalEdgeAfterManifoldEdges)
{
  occ::handle<NCollection_HSequence<TopoDS_Shape>> anInput =
    new NCollection_HSequence<TopoDS_Shape>();
  anInput->Append(myEdge1);
  anInput->Append(myEdge2);
  anInput->Append(myEdge3);
  anInput->Append(myInternalEdge);

  occ::handle<NCollection_HSequence<TopoDS_Shape>> aResult =
    ShapeAnalysis_FreeBounds::ConnectEdgesToWires(anInput, Precision::Confusion(), true);
  checkResult(aResult, 2, 4, 1, 0, true);
}

TEST_F(ShapeAnalysis_FreeBoundsTest, ConnectEdges_InternalEdgeBeforeManifoldEdges)
{
  occ::handle<NCollection_HSequence<TopoDS_Shape>> anInput =
    new NCollection_HSequence<TopoDS_Shape>();
  anInput->Append(myInternalEdge);
  anInput->Append(myEdge1);
  anInput->Append(myEdge2);
  anInput->Append(myEdge3);

  occ::handle<NCollection_HSequence<TopoDS_Shape>> aResult =
    ShapeAnalysis_FreeBounds::ConnectEdgesToWires(anInput, Precision::Confusion(), true);
  checkResult(aResult, 2, 4, 1, 0, true);
}
