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

#include <BOPTools_AlgoTools3D.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <gp_Pnt.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

namespace
{
//! Find the lateral face, whose periodic surface has a seam edge.
TopoDS_Face CylindricalFace(const double theRadius, const double theHeight)
{
  BRepPrimAPI_MakeCylinder aCylinderMaker(theRadius, theHeight);
  const TopoDS_Shape&      aCylinder = aCylinderMaker.Shape();
  if (!aCylinderMaker.IsDone())
  {
    return TopoDS_Face();
  }
  for (TopExp_Explorer anExp(aCylinder, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    const TopoDS_Face& aFace = TopoDS::Face(anExp.Current());
    if (!occ::down_cast<Geom_CylindricalSurface>(BRep_Tool::Surface(aFace)).IsNull())
    {
      return aFace;
    }
  }
  return TopoDS_Face();
}

//! Find the edge represented by two pcurves on the face.
TopoDS_Edge SeamEdge(const TopoDS_Face& theFace)
{
  for (TopExp_Explorer anExp(theFace, TopAbs_EDGE); anExp.More(); anExp.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(anExp.Current());
    if (BRep_Tool::IsClosed(anEdge, theFace))
    {
      return anEdge;
    }
  }
  return TopoDS_Edge();
}

//! Build a lower-half seam fragment with a 3D curve but no face pcurve.
TopoDS_Edge SeamFragmentWithoutPCurve(const double theRadius, const double theHeight)
{
  BRepBuilderAPI_MakeEdge anEdgeMaker(gp_Pnt(theRadius, 0., 0.),
                                      gp_Pnt(theRadius, 0., 0.5 * theHeight));
  if (!anEdgeMaker.IsDone())
  {
    return TopoDS_Edge();
  }
  return anEdgeMaker.Edge();
}
} // namespace

TEST(BOPTools_AlgoTools3DTest, DoSplitSEAMOnFaceWithoutPCurve)
{
  constexpr double  aRadius = 10.;
  constexpr double  aHeight = 20.;
  const TopoDS_Face aFace   = CylindricalFace(aRadius, aHeight);
  ASSERT_FALSE(aFace.IsNull());

  const TopoDS_Edge aSplit = SeamFragmentWithoutPCurve(aRadius, aHeight);
  ASSERT_FALSE(aSplit.IsNull());

  double aT1 = 0., aT2 = 0.;
  ASSERT_TRUE(BRep_Tool::CurveOnSurface(aSplit, aFace, aT1, aT2).IsNull())
    << "the split must enter the function without a pcurve on the face";

  EXPECT_FALSE(BOPTools_AlgoTools3D::DoSplitSEAMOnFace(aSplit, aFace));

  EXPECT_TRUE(BRep_Tool::CurveOnSurface(aSplit, aFace, aT1, aT2).IsNull());
  EXPECT_FALSE(BRep_Tool::IsClosed(aSplit, aFace));
}

TEST(BOPTools_AlgoTools3DTest, DoSplitSEAMOnFaceWithOriginWithoutPCurve)
{
  constexpr double  aRadius = 10.;
  constexpr double  aHeight = 20.;
  const TopoDS_Face aFace   = CylindricalFace(aRadius, aHeight);
  ASSERT_FALSE(aFace.IsNull());

  const TopoDS_Edge aSeam = SeamEdge(aFace);
  ASSERT_FALSE(aSeam.IsNull());

  const TopoDS_Edge aSplit = SeamFragmentWithoutPCurve(aRadius, aHeight);
  ASSERT_FALSE(aSplit.IsNull());

  EXPECT_FALSE(BOPTools_AlgoTools3D::DoSplitSEAMOnFace(aSeam, aSplit, aFace));
  EXPECT_FALSE(BRep_Tool::IsClosed(aSplit, aFace));
}

TEST(BOPTools_AlgoTools3DTest, DoSplitSEAMOnFaceWithPCurve)
{
  const TopoDS_Face aFace = CylindricalFace(10., 20.);
  ASSERT_FALSE(aFace.IsNull());

  const TopoDS_Edge aSeam = SeamEdge(aFace);
  ASSERT_FALSE(aSeam.IsNull());

  double                  aF = 0., aL = 0.;
  occ::handle<Geom_Curve> aC3D = BRep_Tool::Curve(aSeam, aF, aL);
  ASSERT_FALSE(aC3D.IsNull());

  BRepBuilderAPI_MakeEdge aSplitMaker(aC3D, aF, 0.5 * (aF + aL));
  ASSERT_TRUE(aSplitMaker.IsDone());
  const TopoDS_Edge         aSplit = aSplitMaker.Edge();
  occ::handle<Geom2d_Curve> aPCurve =
    BRep_Tool::CurveOnSurface(TopoDS::Edge(aSeam.Oriented(TopAbs_FORWARD)), aFace, aF, aL);
  ASSERT_FALSE(aPCurve.IsNull());
  BRep_Builder().UpdateEdge(aSplit, aPCurve, aFace, BRep_Tool::Tolerance(aSplit));
  ASSERT_FALSE(BRep_Tool::IsClosed(aSplit, aFace));

  EXPECT_TRUE(BOPTools_AlgoTools3D::DoSplitSEAMOnFace(aSplit, aFace));
  EXPECT_TRUE(BRep_Tool::IsClosed(aSplit, aFace))
    << "the split must have become a seam edge of the face";
}
