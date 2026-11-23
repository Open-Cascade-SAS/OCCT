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

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Circle.hxx>
#include <Precision.hxx>
#include <TopOpeBRepDS_BuildTool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopExp_Explorer.hxx>

TEST(TopOpeBRepDS_BuildToolTest, CopyReversedPeriodicEdgePreservesRange)
{
  const occ::handle<Geom_Circle> aCircle = new Geom_Circle(gp_Ax2(), 1.0);
  TopoDS_Edge                    aSource = BRepBuilderAPI_MakeEdge(aCircle);
  aSource.Reverse();

  TopOpeBRepDS_BuildTool aBuildTool;
  TopoDS_Shape           aCopy;
  aBuildTool.CopyEdge(aSource, aCopy);
  for (TopExp_Explorer aVertexIt(aSource, TopAbs_VERTEX); aVertexIt.More(); aVertexIt.Next())
  {
    aBuildTool.AddEdgeVertex(aSource, aCopy, aVertexIt.Current());
  }

  double aFirst, aLast;
  BRep_Tool::Range(TopoDS::Edge(aCopy), aFirst, aLast);
  EXPECT_NEAR(aFirst, aCircle->FirstParameter(), Precision::PConfusion());
  EXPECT_NEAR(aLast, aCircle->LastParameter(), Precision::PConfusion());
}
