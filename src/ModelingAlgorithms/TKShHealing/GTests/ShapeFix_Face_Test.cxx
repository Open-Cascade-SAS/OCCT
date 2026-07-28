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
#include <BRepBuilderAPI_MakeFace.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeExtend_Status.hxx>
#include <ShapeFix_Face.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Face.hxx>

#include <gtest/gtest.h>

namespace
{
//! Creates a bounded planar face suitable for testing context replacements.
//! Returns a null face if construction fails.
TopoDS_Face makePlanarFace()
{
  BRepBuilderAPI_MakeFace aMakeFace(gp_Pln(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)),
                                    0.0,
                                    1.0,
                                    0.0,
                                    1.0);
  if (!aMakeFace.IsDone())
  {
    return TopoDS_Face();
  }
  return aMakeFace.Face();
}
} // namespace

TEST(ShapeFix_FaceTest, Perform_FaceReplacedByCompound_PreservesReplacement)
{
  const TopoDS_Face aFace = makePlanarFace();
  ASSERT_FALSE(aFace.IsNull());

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  occ::handle<ShapeBuild_ReShape> aContext = new ShapeBuild_ReShape;
  aContext->Replace(aFace, aCompound);

  occ::handle<ShapeFix_Face> aFixer = new ShapeFix_Face(aFace);
  aFixer->SetContext(aContext);

  EXPECT_FALSE(aFixer->Perform());
  EXPECT_TRUE(aFixer->Status(ShapeExtend_OK));
  EXPECT_TRUE(aFixer->Result().IsSame(aCompound));
}

TEST(ShapeFix_FaceTest, Perform_RemovedFace_PreservesNullReplacement)
{
  const TopoDS_Face aFace = makePlanarFace();
  ASSERT_FALSE(aFace.IsNull());

  occ::handle<ShapeBuild_ReShape> aContext = new ShapeBuild_ReShape;
  aContext->Remove(aFace);

  occ::handle<ShapeFix_Face> aFixer = new ShapeFix_Face(aFace);
  aFixer->SetContext(aContext);

  EXPECT_FALSE(aFixer->Perform());
  EXPECT_TRUE(aFixer->Status(ShapeExtend_OK));
  EXPECT_TRUE(aFixer->Result().IsNull());
}
