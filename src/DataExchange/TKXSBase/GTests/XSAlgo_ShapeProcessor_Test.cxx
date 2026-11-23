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
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <NCollection_DataMap.hxx>
#include <ShapeExtend_MsgRegistrator.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TransferBRep_ShapeBinder.hxx>
#include <Transfer_TransientProcess.hxx>
#include <XSAlgo_ShapeProcessor.hxx>
#include <gp.hxx>
#include <gp_Circ.hxx>
#include <gp_Pnt.hxx>
#include <gtest/gtest.h>

//==================================================================================================

TEST(XSAlgo_ShapeProcessorTest, MergeVertexHistoryIntoUnmappedEdge)
{
  BRepBuilderAPI_MakeEdge aMakeEdge(gp_Circ(gp::XOY(), 1.0));
  ASSERT_TRUE(aMakeEdge.IsDone());
  const TopoDS_Edge   anEdge     = aMakeEdge.Edge();
  const TopoDS_Vertex aVertex    = TopExp::FirstVertex(anEdge);
  const TopoDS_Vertex aNewVertex = TopoDS::Vertex(aVertex.EmptyCopied());
  BRep_Builder        aBuilder;
  aBuilder.UpdateVertex(aNewVertex, 1.e-5);
  const occ::handle<TCollection_HAsciiString> anEntity =
    new TCollection_HAsciiString("styled edge");
  const occ::handle<TransferBRep_ShapeBinder>  aBinder  = new TransferBRep_ShapeBinder(anEdge);
  const occ::handle<Transfer_TransientProcess> aProcess = new Transfer_TransientProcess(1);
  aProcess->Bind(anEntity, aBinder);
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> aHistory;
  aHistory.Bind(aVertex, aNewVertex);

  XSAlgo_ShapeProcessor::MergeShapeTransferInfo(aProcess, aHistory, 0, nullptr);

  ASSERT_FALSE(aBinder->Result().IsNull());
  ASSERT_EQ(aBinder->Result().ShapeType(), TopAbs_EDGE);
  EXPECT_FALSE(aBinder->Result().IsSame(anEdge));
  int aNbVertices = 0;
  for (TopExp_Explorer anExplorer(aBinder->Result(), TopAbs_VERTEX); anExplorer.More();
       anExplorer.Next())
  {
    EXPECT_TRUE(anExplorer.Current().IsSame(aNewVertex));
    ++aNbVertices;
  }
  EXPECT_EQ(aNbVertices, 2);
  EXPECT_TRUE(TopExp::FirstVertex(anEdge).IsSame(aVertex));
  EXPECT_TRUE(BRepCheck_Analyzer(aBinder->Result()).IsValid());
}

//==================================================================================================

TEST(XSAlgo_ShapeProcessorTest, MergeVertexHistoryRespectsBindingRange)
{
  BRepBuilderAPI_MakeEdge aMakeEdge(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(1.0, 0.0, 0.0));
  ASSERT_TRUE(aMakeEdge.IsDone());
  const TopoDS_Edge   anEdge      = aMakeEdge.Edge();
  const TopoDS_Vertex aVertex     = TopExp::FirstVertex(anEdge);
  const TopoDS_Vertex aNewVertex  = TopoDS::Vertex(aVertex.EmptyCopied());
  const TopoDS_Vertex anUnchanged = BRepBuilderAPI_MakeVertex(gp_Pnt(0.0, 1.0, 0.0)).Vertex();
  const occ::handle<Transfer_TransientProcess> aProcess = new Transfer_TransientProcess(3);
  const occ::handle<TransferBRep_ShapeBinder>  aFirst   = new TransferBRep_ShapeBinder(anEdge);
  const occ::handle<TransferBRep_ShapeBinder>  aSecond  = new TransferBRep_ShapeBinder(anEdge);
  const occ::handle<TransferBRep_ShapeBinder>  aThird   = new TransferBRep_ShapeBinder(anUnchanged);
  aProcess->Bind(new TCollection_HAsciiString("first"), aFirst);
  aProcess->Bind(new TCollection_HAsciiString("second"), aSecond);
  aProcess->Bind(new TCollection_HAsciiString("third"), aThird);
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> aHistory;
  aHistory.Bind(aVertex, aNewVertex);

  XSAlgo_ShapeProcessor::MergeShapeTransferInfo(aProcess, aHistory, 2, nullptr);

  EXPECT_TRUE(aFirst->Result().IsSame(anEdge));
  ASSERT_FALSE(aSecond->Result().IsNull());
  ASSERT_EQ(aSecond->Result().ShapeType(), TopAbs_EDGE);
  EXPECT_TRUE(TopExp::FirstVertex(TopoDS::Edge(aSecond->Result())).IsSame(aNewVertex));
  EXPECT_TRUE(aThird->Result().IsSame(anUnchanged));
}
