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

#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <Geom_Plane.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeUpgrade_WireDivide.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <gp_Pln.hxx>

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
  occ::handle<Geom_Plane> aPln = new Geom_Plane(gp_Pln(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)));
  BRepBuilderAPI_MakeFace aMakeFace(aPln, aPoly.Wire(), true);
  return aMakeFace.IsDone() ? aMakeFace.Face() : TopoDS_Face();
}

TopoDS_Wire outerWireOf(const TopoDS_Face& theFace)
{
  TopExp_Explorer anExp(theFace, TopAbs_WIRE);
  return anExp.More() ? TopoDS::Wire(anExp.Current()) : TopoDS_Wire();
}
} // namespace

// Test for issue #1409: ShapeUpgrade_WireDivide crashes with null context.
TEST(ShapeUpgrade_WireDivideTest, Perform_WithoutContext_DoesNotCrash)
{
  const TopoDS_Face aFace = squareFace();
  ASSERT_FALSE(aFace.IsNull());
  const TopoDS_Wire aWire = outerWireOf(aFace);
  ASSERT_FALSE(aWire.IsNull());

  occ::handle<ShapeUpgrade_WireDivide> aDivider = new ShapeUpgrade_WireDivide();
  aDivider->Init(aWire, aFace);
  EXPECT_NO_THROW(aDivider->Perform());
  EXPECT_FALSE(aDivider->Wire().IsNull());
}

// A live context, set before Perform(), must produce the same result as the self-created one.
TEST(ShapeUpgrade_WireDivideTest, Perform_WithAndWithoutContext_SameResult)
{
  const TopoDS_Face aFace = squareFace();
  ASSERT_FALSE(aFace.IsNull());
  const TopoDS_Wire aWire = outerWireOf(aFace);
  ASSERT_FALSE(aWire.IsNull());

  occ::handle<ShapeUpgrade_WireDivide> aDividerNoContext = new ShapeUpgrade_WireDivide();
  aDividerNoContext->Init(aWire, aFace);
  aDividerNoContext->Perform();

  occ::handle<ShapeUpgrade_WireDivide> aDividerWithContext = new ShapeUpgrade_WireDivide();
  aDividerWithContext->SetContext(new ShapeBuild_ReShape);
  aDividerWithContext->Init(aWire, aFace);
  aDividerWithContext->Perform();

  EXPECT_EQ(aDividerNoContext->Wire().IsNull(), aDividerWithContext->Wire().IsNull());
}
