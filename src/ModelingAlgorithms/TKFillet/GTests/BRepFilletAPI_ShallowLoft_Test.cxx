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
#include <BRepAdaptor_Surface.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <BRepFilletAPI_MakeChamfer.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <BRep_Tool.hxx>
#include <ChFi3d.hxx>
#include <Precision.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedMap.hxx>
#include <gtest/gtest.h>

#include <cmath>

namespace
{
// A shallow ridge between spline faces, with a curved longitudinal guide.
TopoDS_Shape makeShallowLoft(const double theAngle)
{
  BRepOffsetAPI_ThruSections aLoft(true, false);
  for (int i = 0; i < 3; ++i)
  {
    const double               z = 20.0 * i;
    const double               y = i == 1 ? 0.5 : 0.0;
    BRepBuilderAPI_MakePolygon aPolygon;
    aPolygon.Add(gp_Pnt(-20, y, z));
    aPolygon.Add(gp_Pnt(0, y, z));
    aPolygon.Add(gp_Pnt(20, y - 20 * std::tan(theAngle), z));
    aPolygon.Add(gp_Pnt(20, y - 20, z));
    aPolygon.Add(gp_Pnt(-20, y - 20, z));
    aPolygon.Close();
    aLoft.AddWire(aPolygon.Wire());
  }
  aLoft.Build();
  return aLoft.Shape();
}

TopoDS_Edge ridgeEdge(const TopoDS_Shape& theShape)
{
  for (TopExp_Explorer anEdges(theShape, TopAbs_EDGE); anEdges.More(); anEdges.Next())
  {
    const TopoDS_Edge anEdge = TopoDS::Edge(anEdges.Current());
    BRepAdaptor_Curve aCurve(anEdge);
    const gp_Pnt      aFirst = aCurve.EvalD0(aCurve.FirstParameter());
    const gp_Pnt      aLast  = aCurve.EvalD0(aCurve.LastParameter());
    if (std::abs(aFirst.X()) < Precision::Confusion()
        && std::abs(aLast.X()) < Precision::Confusion() && std::abs(aLast.Z() - aFirst.Z()) > 39.0)
    {
      return anEdge;
    }
  }
  return TopoDS_Edge();
}

void checkValidSolid(const TopoDS_Shape& theShape)
{
  EXPECT_TRUE(BRepCheck_Analyzer(theShape).IsValid());
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aSolids;
  TopExp::MapShapes(theShape, TopAbs_SOLID, aSolids);
  EXPECT_EQ(aSolids.Extent(), 1);
}

class BRepFilletAPI_ShallowLoft : public testing::TestWithParam<bool>
{
};
} // namespace

TEST_P(BRepFilletAPI_ShallowLoft, Fillet)
{
  for (double anAngle : {0.005, 0.02})
  {
    SCOPED_TRACE(anAngle);
    const bool   toReuse = GetParam();
    TopoDS_Shape aShape;
    for (double aRadius : {0.1, 0.5, 1.0, 2.0, 5.0, 10.0})
    {
      SCOPED_TRACE(aRadius);
      if (!toReuse || aShape.IsNull())
      {
        aShape = makeShallowLoft(anAngle);
      }
      ASSERT_TRUE(BRepCheck_Analyzer(aShape).IsValid());
      const TopoDS_Edge anEdge = ridgeEdge(aShape);
      ASSERT_FALSE(anEdge.IsNull());
      NCollection_IndexedDataMap<TopoDS_Shape,
                                 NCollection_List<TopoDS_Shape>,
                                 TopTools_ShapeMapHasher>
        anAncestors;
      TopExp::MapShapesAndAncestors(aShape, TopAbs_EDGE, TopAbs_FACE, anAncestors);
      const NCollection_List<TopoDS_Shape>& aFaces = anAncestors.FindFromKey(anEdge);
      ASSERT_EQ(aFaces.Size(), 2u);
      const TopoDS_Face aFirst  = TopoDS::Face(aFaces.First());
      const TopoDS_Face aSecond = TopoDS::Face(aFaces.Last());
      ASSERT_EQ(BRepAdaptor_Surface(aFirst).GetType(), GeomAbs_BSplineSurface);
      ASSERT_EQ(BRepAdaptor_Surface(aSecond).GetType(), GeomAbs_BSplineSurface);
      EXPECT_FALSE(
        ChFi3d::IsTangentFaces(anEdge, aFirst, aSecond, GeomAbs_G1, Precision::Angular()));
      EXPECT_TRUE(ChFi3d::IsTangentFaces(anEdge, aFirst, aSecond, GeomAbs_G1, 0.1));
      BRepFilletAPI_MakeFillet aFillet(aShape);
      aFillet.Add(aRadius, anEdge);
      ASSERT_EQ(aFillet.NbContours(), 1);
      aFillet.Build();
      ASSERT_TRUE(aFillet.IsDone());
      checkValidSolid(aFillet.Shape());
      EXPECT_TRUE(BRepCheck_Analyzer(aShape).IsValid());
    }
  }
}

TEST_P(BRepFilletAPI_ShallowLoft, Chamfer)
{
  for (double anAngle : {0.005, 0.02})
  {
    SCOPED_TRACE(anAngle);
    TopoDS_Shape aShape;
    for (double aDistance : {0.1, 0.5, 1.0, 2.0, 5.0, 10.0})
    {
      SCOPED_TRACE(aDistance);
      if (!GetParam() || aShape.IsNull())
      {
        aShape = makeShallowLoft(anAngle);
      }
      ASSERT_TRUE(BRepCheck_Analyzer(aShape).IsValid());
      const TopoDS_Edge anEdge = ridgeEdge(aShape);
      ASSERT_FALSE(anEdge.IsNull());
      BRepFilletAPI_MakeChamfer aChamfer(aShape);
      aChamfer.Add(aDistance, anEdge);
      ASSERT_EQ(aChamfer.NbContours(), 1);
      aChamfer.Build();
      ASSERT_TRUE(aChamfer.IsDone());
      checkValidSolid(aChamfer.Shape());
      EXPECT_TRUE(BRepCheck_Analyzer(aShape).IsValid());
    }
  }
}

INSTANTIATE_TEST_SUITE_P(Input,
                         BRepFilletAPI_ShallowLoft,
                         testing::Bool(),
                         [](const testing::TestParamInfo<bool>& theInfo) {
                           return theInfo.param ? "Reused" : "Fresh";
                         });

TEST(BRepFilletAPI_ShallowLoftTest, GeneratedSmoothBoundariesAreNotNewContours)
{
  for (double anAngle : {0.005, 0.02, 0.3})
  {
    SCOPED_TRACE(anAngle);
    const TopoDS_Shape       aShape = makeShallowLoft(anAngle);
    BRepFilletAPI_MakeFillet aFillet(aShape);
    aFillet.Add(1.0, ridgeEdge(aShape));
    aFillet.Build();
    ASSERT_TRUE(aFillet.IsDone());
    checkValidSolid(aFillet.Shape());

    NCollection_IndexedDataMap<TopoDS_Shape,
                               NCollection_List<TopoDS_Shape>,
                               TopTools_ShapeMapHasher>
      anAncestors;
    TopExp::MapShapesAndAncestors(aFillet.Shape(), TopAbs_EDGE, TopAbs_FACE, anAncestors);
    int aRegularCount      = 0;
    int anApproximateCount = 0;
    for (int i = 1; i <= anAncestors.Extent(); ++i)
    {
      const NCollection_List<TopoDS_Shape>& aFaces = anAncestors.FindFromIndex(i);
      if (aFaces.Size() != 2)
      {
        continue;
      }
      const TopoDS_Edge anEdge  = TopoDS::Edge(anAncestors.FindKey(i));
      const TopoDS_Face aFirst  = TopoDS::Face(aFaces.First());
      const TopoDS_Face aSecond = TopoDS::Face(aFaces.Last());
      if (aFirst.IsSame(aSecond) || BRep_Tool::Continuity(anEdge, aFirst, aSecond) < GeomAbs_G1)
      {
        continue;
      }
      ++aRegularCount;
      if (!ChFi3d::IsTangentFaces(anEdge, aFirst, aSecond, GeomAbs_G1, Precision::Angular()))
      {
        ++anApproximateCount;
      }
      BRepFilletAPI_MakeFillet aNextFillet(aFillet.Shape());
      aNextFillet.Add(0.1, anEdge);
      EXPECT_EQ(aNextFillet.NbContours(), 0);
      BRepFilletAPI_MakeChamfer aNextChamfer(aFillet.Shape());
      aNextChamfer.Add(0.1, anEdge);
      EXPECT_EQ(aNextChamfer.NbContours(), 0);
    }
    EXPECT_GE(aRegularCount, 2);
    EXPECT_GT(anApproximateCount, 0);
  }
}
