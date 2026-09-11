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

//=================================================================================================
// BOPTools_AlgoTools3D::DoSplitSEAMOnFace - the two-argument overload takes the pcurve of the
// split edge on the face. Boolean operations may hand over a split which has none: when
// BOPAlgo_PaveFiller::MakePCurves fails to build the pcurve it only adds the warning
// BOPAlgo_AlertBuildingPCurveFailed and lets the operation continue.
//=================================================================================================

class BOPTools_AlgoTools3DTest : public ::testing::Test
{
protected:
  static constexpr double THE_RADIUS = 10.;
  static constexpr double THE_HEIGHT = 20.;

  //! Returns the lateral (cylindrical, U-closed) face of a cylinder primitive.
  static TopoDS_Face CylindricalFace()
  {
    TopoDS_Shape aCylinder = BRepPrimAPI_MakeCylinder(THE_RADIUS, THE_HEIGHT).Shape();
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

  //! Returns the seam edge of the face, i.e. the one closed on it.
  static TopoDS_Edge SeamEdge(const TopoDS_Face& theFace)
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

  //! Returns a fragment geometrically coincident with the lower half of the seam of the
  //! cylindrical face, carrying a 3D curve only and no pcurve on the face.
  static TopoDS_Edge SeamFragmentWithoutPCurve()
  {
    BRepBuilderAPI_MakeEdge anEdgeMaker(gp_Pnt(THE_RADIUS, 0., 0.),
                                        gp_Pnt(THE_RADIUS, 0., 0.5 * THE_HEIGHT));
    return anEdgeMaker.Edge();
  }
};

//! A split with no pcurve on the face must be rejected, not dereferenced.
TEST_F(BOPTools_AlgoTools3DTest, DoSplitSEAMOnFaceWithoutPCurve)
{
  const TopoDS_Face aFace = CylindricalFace();
  ASSERT_FALSE(aFace.IsNull());

  const TopoDS_Edge aSplit = SeamFragmentWithoutPCurve();

  double aT1 = 0., aT2 = 0.;
  ASSERT_TRUE(BRep_Tool::CurveOnSurface(aSplit, aFace, aT1, aT2).IsNull())
    << "the split must enter the function without a pcurve on the face";

  EXPECT_FALSE(BOPTools_AlgoTools3D::DoSplitSEAMOnFace(aSplit, aFace));

  // The edge must be left exactly as it was.
  EXPECT_TRUE(BRep_Tool::CurveOnSurface(aSplit, aFace, aT1, aT2).IsNull());
  EXPECT_FALSE(BRep_Tool::IsClosed(aSplit, aFace));
}

//! The three-argument overload already rejects the same input; the two must agree.
TEST_F(BOPTools_AlgoTools3DTest, DoSplitSEAMOnFaceWithOriginWithoutPCurve)
{
  const TopoDS_Face aFace = CylindricalFace();
  ASSERT_FALSE(aFace.IsNull());

  const TopoDS_Edge aSeam = SeamEdge(aFace);
  ASSERT_FALSE(aSeam.IsNull());

  const TopoDS_Edge aSplit = SeamFragmentWithoutPCurve();

  EXPECT_FALSE(BOPTools_AlgoTools3D::DoSplitSEAMOnFace(aSeam, aSplit, aFace));
  EXPECT_FALSE(BRep_Tool::IsClosed(aSplit, aFace));
}

//! A split which does have a pcurve must still receive its second one.
TEST_F(BOPTools_AlgoTools3DTest, DoSplitSEAMOnFaceWithPCurve)
{
  const TopoDS_Face aFace = CylindricalFace();
  ASSERT_FALSE(aFace.IsNull());

  const TopoDS_Edge aSeam = SeamEdge(aFace);
  ASSERT_FALSE(aSeam.IsNull());

  // Build the lower half of the seam as a split carrying a single pcurve, the state in which
  // the Boolean operations pass a seam fragment to the function.
  double                  aF = 0., aL = 0.;
  occ::handle<Geom_Curve> aC3D = BRep_Tool::Curve(aSeam, aF, aL);
  ASSERT_FALSE(aC3D.IsNull());

  const TopoDS_Edge         aSplit = BRepBuilderAPI_MakeEdge(aC3D, aF, 0.5 * (aF + aL)).Edge();
  occ::handle<Geom2d_Curve> aPCurve =
    BRep_Tool::CurveOnSurface(TopoDS::Edge(aSeam.Oriented(TopAbs_FORWARD)), aFace, aF, aL);
  ASSERT_FALSE(aPCurve.IsNull());
  BRep_Builder().UpdateEdge(aSplit, aPCurve, aFace, BRep_Tool::Tolerance(aSplit));
  ASSERT_FALSE(BRep_Tool::IsClosed(aSplit, aFace));

  EXPECT_TRUE(BOPTools_AlgoTools3D::DoSplitSEAMOnFace(aSplit, aFace));
  EXPECT_TRUE(BRep_Tool::IsClosed(aSplit, aFace))
    << "the split must have become a seam edge of the face";
}
