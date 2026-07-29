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
#include <BRep_Tool.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <NCollection_HArray2.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeExtend_CompositeSurface.hxx>
#include <ShapeFix_ComposeShell.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Face.hxx>
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
} // namespace

// Test for issue #1409: ShapeFix_ComposeShell crashes with null context.
TEST(ShapeFix_ComposeShellTest, Perform_WithoutContext_DoesNotCrash)
{
  const TopoDS_Face aFace = squareFace();
  ASSERT_FALSE(aFace.IsNull());

  occ::handle<Geom_Surface>                                   aSurf = BRep_Tool::Surface(aFace);
  occ::handle<NCollection_HArray2<occ::handle<Geom_Surface>>> aGrid =
    new NCollection_HArray2<occ::handle<Geom_Surface>>(1, 1, 1, 1);
  aGrid->SetValue(1, 1, aSurf);
  occ::handle<ShapeExtend_CompositeSurface> aCompSurf = new ShapeExtend_CompositeSurface(aGrid);

  occ::handle<ShapeFix_ComposeShell> aShell = new ShapeFix_ComposeShell();
  aShell->Init(aCompSurf, TopLoc_Location(), aFace, 1.e-7);
  EXPECT_NO_THROW(aShell->Perform());
  EXPECT_FALSE(aShell->Result().IsNull());
}

// Test for issue #1409: ShapeFix_ComposeShell::SplitEdges() crashes with null context.
TEST(ShapeFix_ComposeShellTest, SplitEdges_WithoutContext_DoesNotCrash)
{
  const TopoDS_Face aFace = squareFace();
  ASSERT_FALSE(aFace.IsNull());

  occ::handle<Geom_Surface>                                   aSurf = BRep_Tool::Surface(aFace);
  occ::handle<NCollection_HArray2<occ::handle<Geom_Surface>>> aGrid =
    new NCollection_HArray2<occ::handle<Geom_Surface>>(1, 1, 1, 1);
  aGrid->SetValue(1, 1, aSurf);
  occ::handle<ShapeExtend_CompositeSurface> aCompSurf = new ShapeExtend_CompositeSurface(aGrid);

  occ::handle<ShapeFix_ComposeShell> aShell = new ShapeFix_ComposeShell();
  aShell->Init(aCompSurf, TopLoc_Location(), aFace, 1.e-7);
  EXPECT_NO_THROW(aShell->SplitEdges());
}

// A live context, set before Perform(), must produce the same result as the self-created one.
TEST(ShapeFix_ComposeShellTest, Perform_WithAndWithoutContext_SameResult)
{
  const TopoDS_Face aFace = squareFace();
  ASSERT_FALSE(aFace.IsNull());

  occ::handle<Geom_Surface> aSurf = BRep_Tool::Surface(aFace);

  occ::handle<NCollection_HArray2<occ::handle<Geom_Surface>>> aGridA =
    new NCollection_HArray2<occ::handle<Geom_Surface>>(1, 1, 1, 1);
  aGridA->SetValue(1, 1, aSurf);
  occ::handle<ShapeFix_ComposeShell> aShellNoContext = new ShapeFix_ComposeShell();
  aShellNoContext->Init(new ShapeExtend_CompositeSurface(aGridA), TopLoc_Location(), aFace, 1.e-7);
  const bool aDoneNoContext = aShellNoContext->Perform();

  occ::handle<NCollection_HArray2<occ::handle<Geom_Surface>>> aGridB =
    new NCollection_HArray2<occ::handle<Geom_Surface>>(1, 1, 1, 1);
  aGridB->SetValue(1, 1, aSurf);
  occ::handle<ShapeFix_ComposeShell> aShellWithContext = new ShapeFix_ComposeShell();
  aShellWithContext->SetContext(new ShapeBuild_ReShape);
  aShellWithContext->Init(new ShapeExtend_CompositeSurface(aGridB),
                          TopLoc_Location(),
                          aFace,
                          1.e-7);
  const bool aDoneWithContext = aShellWithContext->Perform();

  EXPECT_EQ(aDoneNoContext, aDoneWithContext);
  EXPECT_EQ(aShellNoContext->Result().IsNull(), aShellWithContext->Result().IsNull());
}
