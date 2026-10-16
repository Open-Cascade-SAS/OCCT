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

#include <BRepAdaptor_Curve.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <ChFi3d_Builder_0.hxx>
#include <GeomConvert.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <TopoDS.hxx>

#include <gtest/gtest.h>
#include <algorithm>
#include <tuple>

namespace
{
occ::handle<Geom_Curve> makeRecoilCurve(const int theKind)
{
  if (theKind == 2)
  {
    NCollection_Array1<gp_Pnt> aPoles(1, 4);
    aPoles(1) = gp_Pnt(0, 0, 0);
    aPoles(2) = gp_Pnt(10, 0, 0);
    aPoles(3) = gp_Pnt(10, 0.5, 0);
    aPoles(4) = gp_Pnt(0, 0.5, 0);
    return GeomConvert::CurveToBSplineCurve(new Geom_BezierCurve(aPoles));
  }
  // The interval crosses the circle's natural seam without closing the edge.
  occ::handle<Geom_Curve> aCurve =
    new Geom_TrimmedCurve(new Geom_Circle(gp_Ax2(gp_Pnt(), gp_Dir(0, 0, 1)), 10), 5.9, 12.1);
  return theKind == 0 ? aCurve : GeomConvert::CurveToBSplineCurve(aCurve);
}
} // namespace

class ChFi3d_CornerRecoil : public testing::TestWithParam<std::tuple<int, bool, bool, double>>
{
};

TEST_P(ChFi3d_CornerRecoil, ChooseProjectedRecoil_NearClosedAndFoldedEdges_PreservesLocalBranch)
{
  const auto [aKind, isReversed, isLast, aScale] = GetParam();
  const occ::handle<Geom_Curve> aCurve           = makeRecoilCurve(aKind);
  if (isReversed)
  {
    aCurve->Reverse();
  }
  gp_Trsf aScaleTransform;
  aScaleTransform.SetScale(gp_Pnt(), aScale);
  aCurve->Transform(aScaleTransform);
  TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(aCurve);
  gp_Trsf     aPlacement;
  aPlacement.SetRotation(gp_Ax1(gp_Pnt(), gp_Dir(1, 2, 3)), 0.731);
  aPlacement.SetTranslationPart(gp_Vec(137, -51, 23));
  anEdge.Move(TopLoc_Location(aPlacement));
  ASSERT_TRUE(BRepCheck_Analyzer(anEdge, true, false, true).IsValid());

  const BRepAdaptor_Curve anAdaptor(anEdge);
  const double            aFirst       = anAdaptor.FirstParameter();
  const double            aLast        = anAdaptor.LastParameter();
  const double            aCorner      = isLast ? aLast : aFirst;
  const double            aSpan        = (isLast ? -1 : 1) * (aLast - aFirst);
  const double            aRecoil      = aCorner + 0.08 * aSpan;
  const double            aRemote      = aCorner + 0.99 * aSpan;
  const gp_Pnt            aCornerPoint = anAdaptor.Value(aCorner);
  double                  aParameter   = aRecoil;
  ChFi3d_ChooseProjectedRecoil(anAdaptor,
                               anAdaptor.Value(aRemote),
                               aCornerPoint,
                               aCorner,
                               aRecoil,
                               aParameter);
  EXPECT_GE(aParameter, std::min(aCorner, aRecoil));
  EXPECT_LE(aParameter, std::max(aCorner, aRecoil));

  // A genuinely better local projection must still be accepted.
  const double aLocal = aCorner + 0.04 * aSpan;
  ASSERT_TRUE(ChFi3d_ChooseProjectedRecoil(anAdaptor,
                                           anAdaptor.Value(aLocal),
                                           aCornerPoint,
                                           aCorner,
                                           aRecoil,
                                           aParameter));
  EXPECT_LE(anAdaptor.Value(aParameter).Distance(anAdaptor.Value(aLocal)), Precision::Confusion());

  // A collapsed interval or projection back onto the corner must retain the recoil.
  EXPECT_FALSE(ChFi3d_ChooseProjectedRecoil(anAdaptor,
                                            aCornerPoint,
                                            aCornerPoint,
                                            aCorner,
                                            aRecoil,
                                            aParameter));
  EXPECT_DOUBLE_EQ(aParameter, aRecoil);
  EXPECT_FALSE(ChFi3d_ChooseProjectedRecoil(anAdaptor,
                                            anAdaptor.Value(aLocal),
                                            aCornerPoint,
                                            aCorner,
                                            aCorner,
                                            aParameter));
  EXPECT_DOUBLE_EQ(aParameter, aCorner);
}

INSTANTIATE_TEST_SUITE_P(NearClosedAndFoldedEdges,
                         ChFi3d_CornerRecoil,
                         testing::Combine(testing::Values(0, 1, 2),
                                          testing::Bool(),
                                          testing::Bool(),
                                          testing::Values(0.1, 1.0, 10.0)));
