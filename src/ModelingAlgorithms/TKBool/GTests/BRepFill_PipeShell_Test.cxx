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

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepFill_PipeShell.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

namespace
{
TopoDS_Wire MakeSquareProfile(const gp_Pnt& theCenter, const double theHalfSide)
{
  BRepBuilderAPI_MakePolygon aPolygon;
  aPolygon.Add(gp_Pnt(theCenter.X() - theHalfSide, theCenter.Y() - theHalfSide, theCenter.Z()));
  aPolygon.Add(gp_Pnt(theCenter.X() + theHalfSide, theCenter.Y() - theHalfSide, theCenter.Z()));
  aPolygon.Add(gp_Pnt(theCenter.X() + theHalfSide, theCenter.Y() + theHalfSide, theCenter.Z()));
  aPolygon.Add(gp_Pnt(theCenter.X() - theHalfSide, theCenter.Y() + theHalfSide, theCenter.Z()));
  aPolygon.Close();
  return aPolygon.Wire();
}
} // namespace

TEST(BRepFill_PipeShellTest, MakeSolid_StraightSpineClosedProfile_ProducesClosedSolid)
{
  const TopoDS_Edge aSpineEdge =
    BRepBuilderAPI_MakeEdge(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(0.0, 0.0, 10.0)).Edge();
  const TopoDS_Wire aSpine = BRepBuilderAPI_MakeWire(aSpineEdge).Wire();

  const TopoDS_Wire aProfile = MakeSquareProfile(gp_Pnt(0.0, 0.0, 0.0), 1.0);

  BRepFill_PipeShell aPipe(aSpine);
  aPipe.Add(aProfile);
  ASSERT_TRUE(aPipe.IsReady());
  ASSERT_TRUE(aPipe.Build());
  ASSERT_TRUE(aPipe.MakeSolid());

  const TopoDS_Shape& aResult = aPipe.Shape();
  EXPECT_FALSE(aResult.IsNull());
  EXPECT_TRUE(aResult.Closed());

  BRepCheck_Analyzer anAnalyzer(aResult);
  EXPECT_TRUE(anAnalyzer.IsValid());
}

TEST(BRepFill_PipeShellTest, MakeSolid_PolylineSpineClosedProfile_ProducesClosedSolid)
{
  BRepBuilderAPI_MakePolygon aSpinePolygon;
  aSpinePolygon.Add(gp_Pnt(0.0, 0.0, 0.0));
  aSpinePolygon.Add(gp_Pnt(0.0, 0.0, 5.0));
  aSpinePolygon.Add(gp_Pnt(0.0, 5.0, 10.0));
  const TopoDS_Wire aSpine = aSpinePolygon.Wire();

  const TopoDS_Wire aProfile = MakeSquareProfile(gp_Pnt(0.0, 0.0, 0.0), 0.5);

  BRepFill_PipeShell aPipe(aSpine);
  aPipe.SetTransition(BRepFill_Right);
  aPipe.Add(aProfile);
  ASSERT_TRUE(aPipe.IsReady());
  ASSERT_TRUE(aPipe.Build());
  ASSERT_TRUE(aPipe.MakeSolid());

  const TopoDS_Shape& aResult = aPipe.Shape();
  EXPECT_FALSE(aResult.IsNull());
  EXPECT_TRUE(aResult.Closed());
}
