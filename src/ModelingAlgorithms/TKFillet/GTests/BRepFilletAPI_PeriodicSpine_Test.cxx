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

#include <algorithm>
#include <array>
#include <cmath>
#include <vector>

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <BRep_Tool.hxx>
#include <ChFi3d_Builder_0.hxx>
#include <ChFi3d_FilletShape.hxx>
#include <ChFiDS_ElSpine.hxx>
#include <ChFiDS_Spine.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <GeomAbs_Shape.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <NCollection_IndexedMap.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

namespace
{

using ShapeMap = NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>;

TopoDS_Wire makeRectangle(const double theWidth, const double theDepth, const double theZ)
{
  BRepBuilderAPI_MakePolygon aPolygon;
  aPolygon.Add(gp_Pnt(-theWidth / 2.0, -theDepth / 2.0, theZ));
  aPolygon.Add(gp_Pnt(theWidth / 2.0, -theDepth / 2.0, theZ));
  aPolygon.Add(gp_Pnt(theWidth / 2.0, theDepth / 2.0, theZ));
  aPolygon.Add(gp_Pnt(-theWidth / 2.0, theDepth / 2.0, theZ));
  aPolygon.Close();
  return aPolygon.Wire();
}

std::array<double, 6> edgeBounds(const TopoDS_Edge& theEdge)
{
  std::array<double, 6> aBounds = {1.0e100, 1.0e100, 1.0e100, -1.0e100, -1.0e100, -1.0e100};
  ShapeMap              aVertices;
  TopExp::MapShapes(theEdge, TopAbs_VERTEX, aVertices);
  for (int anIndex = 1; anIndex <= aVertices.Extent(); ++anIndex)
  {
    const gp_Pnt aPoint = BRep_Tool::Pnt(TopoDS::Vertex(aVertices(anIndex)));
    aBounds[0]          = std::min(aBounds[0], aPoint.X());
    aBounds[1]          = std::min(aBounds[1], aPoint.Y());
    aBounds[2]          = std::min(aBounds[2], aPoint.Z());
    aBounds[3]          = std::max(aBounds[3], aPoint.X());
    aBounds[4]          = std::max(aBounds[4], aPoint.Y());
    aBounds[5]          = std::max(aBounds[5], aPoint.Z());
  }
  return aBounds;
}

bool spansHeight(const TopoDS_Edge& theEdge, const double theHeight)
{
  const std::array<double, 6> aBounds = edgeBounds(theEdge);
  return aBounds[2] < 1.0e-7 && aBounds[5] > theHeight - 1.0e-7;
}

bool liesOnBottom(const TopoDS_Edge& theEdge)
{
  const std::array<double, 6> aBounds = edgeBounds(theEdge);
  return std::abs(aBounds[2]) < 1.0e-7 && std::abs(aBounds[5]) < 1.0e-7;
}

int countC0BSplineFaces(const TopoDS_Shape& theShape)
{
  int      aC0FaceCount = 0;
  ShapeMap aFaces;
  TopExp::MapShapes(theShape, TopAbs_FACE, aFaces);
  for (int aFaceIndex = 1; aFaceIndex <= aFaces.Extent(); ++aFaceIndex)
  {
    const occ::handle<Geom_BSplineSurface> aSurface =
      occ::down_cast<Geom_BSplineSurface>(BRep_Tool::Surface(TopoDS::Face(aFaces(aFaceIndex))));
    if (aSurface.IsNull())
    {
      continue;
    }

    bool hasC0Knot = false;
    for (int aKnotIndex = 2; aKnotIndex < aSurface->NbUKnots(); ++aKnotIndex)
    {
      hasC0Knot = hasC0Knot || aSurface->UMultiplicity(aKnotIndex) >= aSurface->UDegree();
    }
    for (int aKnotIndex = 2; aKnotIndex < aSurface->NbVKnots(); ++aKnotIndex)
    {
      hasC0Knot = hasC0Knot || aSurface->VMultiplicity(aKnotIndex) >= aSurface->VDegree();
    }
    if (hasC0Knot)
    {
      ++aC0FaceCount;
    }
  }
  return aC0FaceCount;
}

occ::handle<Geom_BSplineCurve> performPeriodicSpine(const std::vector<TopoDS_Edge>& theEdges,
                                                    const double                    theTolerance)
{
  occ::handle<ChFiDS_Spine> aSpine = new ChFiDS_Spine(theTolerance);
  for (const TopoDS_Edge& anEdge : theEdges)
  {
    aSpine->SetEdges(anEdge);
  }
  aSpine->SetFirstStatus(ChFiDS_Closed);
  aSpine->SetLastStatus(ChFiDS_Closed);
  aSpine->Load();

  occ::handle<ChFiDS_ElSpine> anElSpine = new ChFiDS_ElSpine();
  anElSpine->FirstParameter(aSpine->FirstParameter());
  anElSpine->LastParameter(aSpine->LastParameter());
  anElSpine->SetPeriodic(true);
  ChFi3d_PerformElSpine(anElSpine, aSpine, GeomAbs_C1, theTolerance);
  return anElSpine->BSpline();
}

std::vector<TopoDS_Edge> makeStadiumEdges(const double theScale)
{
  const double aRadius = 10.0 * theScale;
  const double aLength = 50.0 * theScale;
  const gp_Pnt aP1(0.0, aRadius, 0.0);
  const gp_Pnt aP2(aLength, aRadius, 0.0);
  const gp_Pnt aP3(aLength, -aRadius, 0.0);
  const gp_Pnt aP4(0.0, -aRadius, 0.0);

  return {
    BRepBuilderAPI_MakeEdge(aP1, aP2),
    BRepBuilderAPI_MakeEdge(
      GC_MakeArcOfCircle(aP2, gp_Pnt(aLength + aRadius, 0.0, 0.0), aP3).Value()),
    BRepBuilderAPI_MakeEdge(aP3, aP4),
    BRepBuilderAPI_MakeEdge(GC_MakeArcOfCircle(aP4, gp_Pnt(-aRadius, 0.0, 0.0), aP1).Value())};
}

std::vector<TopoDS_Edge> makeSplitCircleEdges()
{
  const gp_Circ aCircle(gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 20.0);
  return {BRepBuilderAPI_MakeEdge(aCircle, 0.0, 0.5 * M_PI),
          BRepBuilderAPI_MakeEdge(aCircle, 0.5 * M_PI, M_PI),
          BRepBuilderAPI_MakeEdge(aCircle, M_PI, 1.5 * M_PI),
          BRepBuilderAPI_MakeEdge(aCircle, 1.5 * M_PI, 2.0 * M_PI)};
}

} // namespace

// OCC22163 guarded closure-knot removal so an exact periodic guide is not approximated. This test
// exercises ChFi3d_PerformElSpine directly and fails if removal is made unconditional again.
TEST(ChFi3d_PerformElSpineTest, ExactMultiEdgeCircle_NoApproximation_KeepsClosureMultiplicity)
{
  constexpr double                     aTolerance = 1.0e-4;
  const occ::handle<Geom_BSplineCurve> aCurve =
    performPeriodicSpine(makeSplitCircleEdges(), aTolerance);

  ASSERT_FALSE(aCurve.IsNull());
  ASSERT_TRUE(aCurve->IsPeriodic());
  EXPECT_EQ(aCurve->Multiplicity(1), aCurve->Degree())
    << "An exact periodic guide must retain the OCC22163 guarded behavior";
}

TEST(ChFi3d_PerformElSpineTest, TangentCurvatureJump_ScaledGuide_ReducesClosureMultiplicity)
{
  constexpr double aTolerance = 1.0e-4;
  for (const double aScale : {1.0, 1.0e4})
  {
    SCOPED_TRACE(::testing::Message() << "scale " << aScale);
    const occ::handle<Geom_BSplineCurve> aCurve =
      performPeriodicSpine(makeStadiumEdges(aScale), aTolerance);

    ASSERT_FALSE(aCurve.IsNull());
    ASSERT_TRUE(aCurve->IsPeriodic());
    EXPECT_LE(aCurve->Multiplicity(1), aCurve->Degree() - 2)
      << "A tangent-continuous curvature jump must be smoothed at the periodic closure";
  }
}

// Regression test for a periodic elementary spine whose closure lies at a tangent transition.
// The closure knot must be smoothed to the same requested continuity as all internal junctions;
// otherwise the final overlapping fillet stripe acquires an internal C0 surface knot.
TEST(BRepFilletAPI_MakeFilletTest, PeriodicSpine_TangentCurvatureJump_DoesNotCreateC0Surface)
{
  constexpr double aHeight       = 30.0;
  constexpr double aTaperDegrees = 10.0;
  const double     aGrowth       = 2.0 * aHeight * std::tan(aTaperDegrees * M_PI / 180.0);

  BRepOffsetAPI_ThruSections aLoft(true, true);
  aLoft.AddWire(makeRectangle(50.0, 70.0, 0.0));
  aLoft.AddWire(makeRectangle(50.0 + aGrowth, 70.0 + aGrowth, aHeight));
  aLoft.Build();
  ASSERT_TRUE(aLoft.IsDone()) << "Tapered loft should be built";

  BRepFilletAPI_MakeFillet aVerticalFillet(aLoft.Shape());
  ShapeMap                 aLoftEdges;
  TopExp::MapShapes(aLoft.Shape(), TopAbs_EDGE, aLoftEdges);
  int aVerticalEdgeCount = 0;
  for (int anEdgeIndex = 1; anEdgeIndex <= aLoftEdges.Extent(); ++anEdgeIndex)
  {
    const TopoDS_Edge anEdge = TopoDS::Edge(aLoftEdges(anEdgeIndex));
    if (spansHeight(anEdge, aHeight))
    {
      aVerticalFillet.Add(10.0, anEdge);
      ++aVerticalEdgeCount;
    }
  }
  ASSERT_EQ(aVerticalEdgeCount, 4);
  aVerticalFillet.Build();
  ASSERT_TRUE(aVerticalFillet.IsDone()) << "Vertical fillet should be built";

  std::vector<TopoDS_Edge> aBottomEdges;
  ShapeMap                 aRoundedEdges;
  TopExp::MapShapes(aVerticalFillet.Shape(), TopAbs_EDGE, aRoundedEdges);
  for (int anEdgeIndex = 1; anEdgeIndex <= aRoundedEdges.Extent(); ++anEdgeIndex)
  {
    const TopoDS_Edge anEdge = TopoDS::Edge(aRoundedEdges(anEdgeIndex));
    if (liesOnBottom(anEdge))
    {
      aBottomEdges.push_back(anEdge);
    }
  }
  ASSERT_EQ(aBottomEdges.size(), 8U);

  for (std::size_t aSeedIndex = 0; aSeedIndex < aBottomEdges.size(); ++aSeedIndex)
  {
    SCOPED_TRACE(::testing::Message() << "bottom seed edge " << aSeedIndex + 1);
    BRepFilletAPI_MakeFillet aBottomFillet(aVerticalFillet.Shape(), ChFi3d_Rational);
    aBottomFillet.SetParams(1.0e-2, 1.0e-4, 1.0e-5, 1.0e-4, 1.0e-5, 1.0e-3);
    aBottomFillet.SetContinuity(GeomAbs_C1, 1.0e-2);
    aBottomFillet.Add(5.0, aBottomEdges[aSeedIndex]);
    aBottomFillet.Build();
    ASSERT_TRUE(aBottomFillet.IsDone()) << "Bottom fillet should be built";

    const TopoDS_Shape& aResult = aBottomFillet.Shape();
    ASSERT_FALSE(aResult.IsNull());
    EXPECT_TRUE(BRepCheck_Analyzer(aResult).IsValid());
    ShapeMap aResultFaces;
    ShapeMap aResultEdges;
    TopExp::MapShapes(aResult, TopAbs_FACE, aResultFaces);
    TopExp::MapShapes(aResult, TopAbs_EDGE, aResultEdges);
    EXPECT_EQ(aResultFaces.Extent(), 18);
    EXPECT_EQ(aResultEdges.Extent(), 40);
    EXPECT_EQ(countC0BSplineFaces(aResult), 0)
      << "Periodic closure must not introduce a C0 supporting surface";
  }
}
