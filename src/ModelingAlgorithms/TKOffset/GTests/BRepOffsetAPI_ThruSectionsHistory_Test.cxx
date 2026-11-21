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
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <NCollection_List.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <gp.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

namespace
{
//==================================================================================================

TopoDS_Wire makeCapsuleWire(const double theX)
{
  const gp_Circ aLeft(gp_Ax2(gp_Pnt(theX, 0.0, 0.0), gp::DZ(), gp::DX()), 5.0);
  const gp_Circ aRight(gp_Ax2(gp_Pnt(theX + 10.0, 0.0, 0.0), gp::DZ(), gp::DX()), 5.0);
  BRepBuilderAPI_MakeWire aWire;
  aWire.Add(BRepBuilderAPI_MakeEdge(aLeft, 0.5 * M_PI, 1.5 * M_PI).Edge());
  aWire.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(theX, -5.0, 0.0), gp_Pnt(theX + 10.0, -5.0, 0.0)).Edge());
  aWire.Add(BRepBuilderAPI_MakeEdge(aRight, 1.5 * M_PI, 2.5 * M_PI).Edge());
  aWire.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(theX + 10.0, 5.0, 0.0), gp_Pnt(theX, 5.0, 0.0)).Edge());
  return aWire.Wire();
}
} // namespace

TEST(BRepOffsetAPI_ThruSectionsTest, RepeatedClosedSectionPreservesGeneratedFaces)
{
  const TopoDS_Wire aFirst = makeCapsuleWire(0.0);
  const gp_Circ aCircle(gp_Ax2(gp_Pnt(50.0, 0.0, 15.0), gp::DX(), gp_Dir(0.0, 0.0, -1.0)), 3.0);
  const TopoDS_Wire aSecond = BRepBuilderAPI_MakeWire(BRepBuilderAPI_MakeEdge(aCircle).Edge()).Wire();

  BRepOffsetAPI_ThruSections aLoft;
  aLoft.SetMaxDegree(5);
  aLoft.AddWire(aFirst);
  aLoft.AddWire(aSecond);
  aLoft.AddWire(makeCapsuleWire(90.0));
  aLoft.AddWire(aFirst);
  aLoft.Build();

  ASSERT_TRUE(aLoft.IsDone());
  ASSERT_FALSE(aLoft.Shape().IsNull());
  EXPECT_TRUE(BRepCheck_Analyzer(aLoft.Shape()).IsValid());
  for (TopExp_Explorer anExp(aFirst, TopAbs_EDGE); anExp.More(); anExp.Next())
  {
    const NCollection_List<TopoDS_Shape>& aGenerated = aLoft.Generated(anExp.Current());
    ASSERT_FALSE(aGenerated.IsEmpty());
    for (const TopoDS_Shape& aFace : aGenerated)
    {
      ASSERT_FALSE(aFace.IsNull());
      EXPECT_EQ(aFace.ShapeType(), TopAbs_FACE);
    }
  }
}
