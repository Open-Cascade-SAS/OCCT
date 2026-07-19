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
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRep_Builder.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

namespace
{
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
