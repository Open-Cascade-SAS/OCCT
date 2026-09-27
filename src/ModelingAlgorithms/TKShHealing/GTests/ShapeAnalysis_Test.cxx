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

#include <BRepPrimAPI_MakeBox.hxx>
#include <BRep_Builder.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NullObject.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>

// A face with no surface and no edges takes the no-edge branch of GetFaceUVBounds, which used to
// dereference the null surface: an uncatchable fault in place of the Standard_Failure the callers
// in ShapeUpgrade_FaceDivide and ShapeFix_Face are written to handle.
TEST(ShapeAnalysisTest, GetFaceUVBounds_FaceWithoutSurfaceOrEdges_Raises)
{
  BRep_Builder aBuilder;
  TopoDS_Face  aFace;
  aBuilder.MakeFace(aFace);
  ASSERT_FALSE(TopExp_Explorer(aFace, TopAbs_EDGE).More()) << "Face is expected to have no edges";

  double aUMin = 0.0, aUMax = 0.0, aVMin = 0.0, aVMax = 0.0;
  EXPECT_THROW(ShapeAnalysis::GetFaceUVBounds(aFace, aUMin, aUMax, aVMin, aVMax),
               Standard_NullObject);
}

// The neighbouring input class, which already raised: with edges but no pcurves the bounding box
// stays void and Bnd_Box2d::Get raises. Both surface-less faces now report the same way.
TEST(ShapeAnalysisTest, GetFaceUVBounds_FaceWithoutSurfaceWithWire_Raises)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  TopExp_Explorer    aFaceExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(aFaceExp.More()) << "No faces found in box";
  TopExp_Explorer aWireExp(aFaceExp.Current(), TopAbs_WIRE);
  ASSERT_TRUE(aWireExp.More()) << "No wires found in box face";

  BRep_Builder aBuilder;
  TopoDS_Face  aFace;
  aBuilder.MakeFace(aFace);
  aBuilder.Add(aFace, TopoDS::Wire(aWireExp.Current()));

  double aUMin = 0.0, aUMax = 0.0, aVMin = 0.0, aVMax = 0.0;
  EXPECT_THROW(ShapeAnalysis::GetFaceUVBounds(aFace, aUMin, aUMax, aVMin, aVMax),
               Standard_ConstructionError);
}

// The no-edge branch's own valid input: the surface's bounds are still what comes back.
TEST(ShapeAnalysisTest, GetFaceUVBounds_SurfaceWithoutEdges_ReportsSurfaceBounds)
{
  const occ::handle<Geom_SphericalSurface> aSurf = new Geom_SphericalSurface(gp_Ax3(), 5.0);
  BRep_Builder                             aBuilder;
  TopoDS_Face                              aFace;
  aBuilder.MakeFace(aFace, aSurf, Precision::Confusion());
  ASSERT_FALSE(TopExp_Explorer(aFace, TopAbs_EDGE).More()) << "Face is expected to have no edges";

  double aUMin = 0.0, aUMax = 0.0, aVMin = 0.0, aVMax = 0.0;
  ASSERT_NO_THROW(ShapeAnalysis::GetFaceUVBounds(aFace, aUMin, aUMax, aVMin, aVMax));

  double aSUMin = 0.0, aSUMax = 0.0, aSVMin = 0.0, aSVMax = 0.0;
  aSurf->Bounds(aSUMin, aSUMax, aSVMin, aSVMax);
  EXPECT_NEAR(aUMin, aSUMin, Precision::PConfusion());
  EXPECT_NEAR(aUMax, aSUMax, Precision::PConfusion());
  EXPECT_NEAR(aVMin, aSVMin, Precision::PConfusion());
  EXPECT_NEAR(aVMax, aSVMax, Precision::PConfusion());
}

// The pcurve branch, which the fix does not touch.
TEST(ShapeAnalysisTest, GetFaceUVBounds_BoxFace_ReportsFiniteBounds)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();
  TopExp_Explorer    aFaceExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(aFaceExp.More()) << "No faces found in box";

  double aUMin = 0.0, aUMax = 0.0, aVMin = 0.0, aVMax = 0.0;
  ASSERT_NO_THROW(
    ShapeAnalysis::GetFaceUVBounds(TopoDS::Face(aFaceExp.Current()), aUMin, aUMax, aVMin, aVMax));
  EXPECT_LT(aUMin, aUMax);
  EXPECT_LT(aVMin, aVMax);
}
