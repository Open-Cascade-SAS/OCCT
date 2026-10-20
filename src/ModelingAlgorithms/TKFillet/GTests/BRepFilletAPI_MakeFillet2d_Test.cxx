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

#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepFilletAPI_MakeFillet2d.hxx>
#include <ChFi2d_ConstructionError.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

namespace
{
TopoDS_Face squareFace()
{
  BRepBuilderAPI_MakePolygon aPoly;
  aPoly.Add(gp_Pnt(0, 0, 0));
  aPoly.Add(gp_Pnt(10, 0, 0));
  aPoly.Add(gp_Pnt(10, 10, 0));
  aPoly.Add(gp_Pnt(0, 10, 0));
  aPoly.Close();
  BRepBuilderAPI_MakeFace aMakeFace(aPoly.Wire());
  return aMakeFace.IsDone() ? aMakeFace.Face() : TopoDS_Face();
}
} // namespace

// Regression test for issue #1431: a second AddChamfer() naming the same edge pair must not
// crash. The pair's shared vertex is removed from the face's wire by the first call, so the
// second call's FindConnectedEdges lookup fails; ComputeChamfer used to dereference the two
// null edges it returned unchecked.
TEST(BRepFilletAPI_MakeFillet2dTest, AddChamfer_RepeatedPair_DoesNotCrash)
{
  const TopoDS_Face aFace = squareFace();
  ASSERT_FALSE(aFace.IsNull());

  TopoDS_Edge     anEdge1, anEdge2;
  TopExp_Explorer anExp(aFace, TopAbs_EDGE);
  ASSERT_TRUE(anExp.More());
  anEdge1 = TopoDS::Edge(anExp.Current());
  anExp.Next();
  ASSERT_TRUE(anExp.More());
  anEdge2 = TopoDS::Edge(anExp.Current());

  BRepFilletAPI_MakeFillet2d aChamfer(aFace);
  const TopoDS_Edge          aFirst = aChamfer.AddChamfer(anEdge1, anEdge2, 1.0, 2.0);
  EXPECT_FALSE(aFirst.IsNull());
  EXPECT_EQ(aChamfer.Status(), ChFi2d_IsDone);

  TopoDS_Edge aSecond;
  EXPECT_NO_THROW(aSecond = aChamfer.AddChamfer(anEdge1, anEdge2, 1.0, 2.0));
  EXPECT_TRUE(aSecond.IsNull());
  EXPECT_EQ(aChamfer.Status(), ChFi2d_ConnexionError);
}
