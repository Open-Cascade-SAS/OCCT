// Copyright (c) 2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.

#include <gtest/gtest.h>

#include <BOPAlgo_CheckerSI.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_Pair.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepLib.hxx>
#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom2d_Line.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <Precision.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>

#include <utility>
#include <cmath>

namespace
{
//! Creates a ruled cubic crossing at normalized V parameters 0.25 and 0.75.
occ::handle<Geom_BSplineSurface> createCrossingSurface();

//! Creates a crosscap whose two preimages lie on u*u+v*v=1 before scaling.
occ::handle<Geom_BSplineSurface> createCrosscapSurface();

//! Returns completion status and the number of self-intersecting faces.
std::pair<bool, int> checkSelfFaces(const TopoDS_Face& theFace);

//==================================================================================================

occ::handle<Geom_BSplineSurface> createCrossingSurface()
{
  constexpr double           aY[4] = {4.0, -4.0 / 3.0, -4.0 / 3.0, 4.0};
  constexpr double           aZ[4] = {-6.0, 26.0 / 3.0, -26.0 / 3.0, 6.0};
  NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 4);
  for (int u = 1; u <= 2; ++u)
  {
    for (int v = 1; v <= 4; ++v)
    {
      aPoles.SetValue(u, v, gp_Pnt((u - 1) * 1000.0, aY[v - 1] * 1000.0, aZ[v - 1] * 1000.0));
    }
  }
  NCollection_Array1<double> aKnots(1, 2);
  aKnots.SetValue(1, 0.0);
  aKnots.SetValue(2, 1.0);
  NCollection_Array1<int> aUMults(1, 2);
  aUMults.Init(2);
  NCollection_Array1<int> aVMults(1, 2);
  aVMults.Init(4);
  return new Geom_BSplineSurface(aPoles, aKnots, aKnots, aUMults, aVMults, 1, 3);
}

//==================================================================================================

occ::handle<Geom_BSplineSurface> createCrosscapSurface()
{
  constexpr double           aX[3]  = {-2.0, 0.0, 2.0};
  constexpr double           aX2[3] = {4.0, -4.0, 4.0};
  constexpr double           aV[4]  = {-2.0, -2.0 / 3.0, 2.0 / 3.0, 2.0};
  constexpr double           aY[4]  = {4.0, -4.0 / 3.0, -4.0 / 3.0, 4.0};
  constexpr double           aZ[4]  = {-6.0, 26.0 / 3.0, -26.0 / 3.0, 6.0};
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 4);
  for (int u = 1; u <= 3; ++u)
  {
    for (int v = 1; v <= 4; ++v)
    {
      aPoles.SetValue(u,
                      v,
                      gp_Pnt(aX[u - 1] * 1000.0,
                             aY[v - 1] * 1000.0,
                             (aZ[v - 1] + aX2[u - 1] * aV[v - 1]) * 1000.0));
    }
  }
  NCollection_Array1<double> aKnots(1, 2);
  aKnots.SetValue(1, 0.0);
  aKnots.SetValue(2, 1.0);
  NCollection_Array1<int> aUMults(1, 2);
  aUMults.Init(3);
  NCollection_Array1<int> aVMults(1, 2);
  aVMults.Init(4);
  return new Geom_BSplineSurface(aPoles, aKnots, aKnots, aUMults, aVMults, 2, 3);
}

//==================================================================================================

std::pair<bool, int> checkSelfFaces(const TopoDS_Face& theFace)
{
  BOPAlgo_CheckerSI aChecker;
  aChecker.AddArgument(theFace);
  aChecker.SetLevelOfCheck(5);
  aChecker.SetRunParallel(false);
  aChecker.Perform();
  if (aChecker.HasErrors() || aChecker.PDS() == nullptr)
  {
    return {false, -1};
  }
  int aCount = 0;
  for (const auto& aPair : aChecker.PDS()->Interferences())
  {
    int aFirst, aSecond;
    aPair.Indices(aFirst, aSecond);
    aCount += aFirst == aSecond && aChecker.PDS()->Shape(aFirst).ShapeType() == TopAbs_FACE;
  }
  return {true, aCount};
}
} // namespace

//==================================================================================================

TEST(BOPAlgo_CheckerSITest, TrimExcludesSecondSurfaceBranch)
{
  const occ::handle<Geom_BSplineSurface> aSurface = createCrossingSurface();
  BRepBuilderAPI_MakeFace aMaker(aSurface, 0.0, 1.0, 0.1, 0.75 - 4.0e-7, Precision::Confusion());
  ASSERT_TRUE(aMaker.IsDone());
  const auto [isDone, aCount] = checkSelfFaces(aMaker.Face());
  ASSERT_TRUE(isDone);
  EXPECT_EQ(aCount, 0);
}

//==================================================================================================

TEST(BOPAlgo_CheckerSITest, TrimIncludesSecondSurfaceBranch)
{
  const occ::handle<Geom_BSplineSurface> aSurface = createCrossingSurface();
  BRepBuilderAPI_MakeFace aMaker(aSurface, 0.0, 1.0, 0.1, 0.75 + 4.0e-7, Precision::Confusion());
  ASSERT_TRUE(aMaker.IsDone());
  const auto [isDone, aCount] = checkSelfFaces(aMaker.Face());
  ASSERT_TRUE(isDone);
  EXPECT_EQ(aCount, 1);
}

//==================================================================================================

TEST(BOPAlgo_CheckerSITest, SelfIntersectionOnTrimBoundary)
{
  const occ::handle<Geom_BSplineSurface> aSurface = createCrossingSurface();
  BRepBuilderAPI_MakeFace aMaker(aSurface, 0.0, 1.0, 0.1, 0.75, Precision::Confusion());
  ASSERT_TRUE(aMaker.IsDone());
  const auto [isDone, aCount] = checkSelfFaces(aMaker.Face());
  ASSERT_TRUE(isDone);
  EXPECT_EQ(aCount, 1);
}

//==================================================================================================

TEST(BOPAlgo_CheckerSITest, TrimFarFromSecondSurfaceBranch)
{
  const occ::handle<Geom_BSplineSurface> aSurface = createCrossingSurface();
  BRepBuilderAPI_MakeFace aMaker(aSurface, 0.0, 1.0, 0.1, 0.7, Precision::Confusion());
  ASSERT_TRUE(aMaker.IsDone());
  const auto [isDone, aCount] = checkSelfFaces(aMaker.Face());
  ASSERT_TRUE(isDone);
  EXPECT_EQ(aCount, 0);
}

//==================================================================================================

TEST(BOPAlgo_CheckerSITest, ReversedLocatedTrimExcludesSecondBranch)
{
  const occ::handle<Geom_BSplineSurface> aSurface = createCrossingSurface();
  BRepBuilderAPI_MakeFace aMaker(aSurface, 0.0, 1.0, 0.1, 0.75 - 4.0e-7, Precision::Confusion());
  ASSERT_TRUE(aMaker.IsDone());
  gp_Trsf aPlacement;
  aPlacement.SetTranslation(gp_Vec(7.0, -11.0, 13.0));
  TopoDS_Face aFace = TopoDS::Face(aMaker.Face().Moved(TopLoc_Location(aPlacement)));
  aFace.Reverse();
  const auto [isDone, aCount] = checkSelfFaces(aFace);
  ASSERT_TRUE(isDone);
  EXPECT_EQ(aCount, 0);
}

//==================================================================================================

TEST(BOPAlgo_CheckerSITest, HoleExcludesOneSurfacePreimage)
{
  const occ::handle<Geom_BSplineSurface> aSurface = createCrosscapSurface();
  BRepBuilderAPI_MakeFace aMaker(aSurface, 0.0, 1.0, 0.0, 1.0, Precision::Confusion());
  ASSERT_TRUE(aMaker.IsDone());
  const gp_Pnt2d aPoints[4] = {{0.225, 0.475}, {0.225, 0.775}, {0.775, 0.775}, {0.775, 0.475}};
  BRepBuilderAPI_MakeWire aWireMaker;
  for (int i = 0; i < 4; ++i)
  {
    const gp_Pnt2d&                aFirst = aPoints[i];
    const gp_Pnt2d&                aLast  = aPoints[(i + 1) % 4];
    const occ::handle<Geom2d_Line> aCurve =
      new Geom2d_Line(aFirst, gp_Dir2d(gp_Vec2d(aFirst, aLast)));
    BRepBuilderAPI_MakeEdge anEdge(aCurve, aSurface, 0.0, aFirst.Distance(aLast));
    ASSERT_TRUE(anEdge.IsDone());
    aWireMaker.Add(anEdge.Edge());
  }
  ASSERT_TRUE(aWireMaker.IsDone());
  const TopoDS_Wire aWire = aWireMaker.Wire();
  ASSERT_TRUE(BRepLib::BuildCurves3d(aWire));
  aMaker.Add(aWire);
  ASSERT_TRUE(aMaker.IsDone());
  const auto [isDone, aCount] = checkSelfFaces(aMaker.Face());
  ASSERT_TRUE(isDone);
  EXPECT_EQ(aCount, 0);
}

//==================================================================================================

TEST(BOPAlgo_CheckerSITest, CrosscapWithoutHoleHasSelfIntersection)
{
  const occ::handle<Geom_BSplineSurface> aSurface = createCrosscapSurface();
  BRepBuilderAPI_MakeFace aMaker(aSurface, 0.0, 1.0, 0.0, 1.0, Precision::Confusion());
  ASSERT_TRUE(aMaker.IsDone());
  const auto [isDone, aCount] = checkSelfFaces(aMaker.Face());
  ASSERT_TRUE(isDone);
  EXPECT_EQ(aCount, 1);
}

//==================================================================================================

TEST(BOPAlgo_CheckerSITest, ClosedSeamOnGloballyOpenSurface)
{
  constexpr double           aX[9] = {1, 1, 0, -1, -1, -1, 0, 1, 1};
  constexpr double           aY[9] = {0, 1, 1, 1, 0, -1, -1, -1, 0};
  NCollection_Array2<gp_Pnt> aPoles(1, 9, 1, 3);
  NCollection_Array2<double> aWeights(1, 9, 1, 3);
  for (int u = 1; u <= 9; ++u)
  {
    for (int v = 1; v <= 3; ++v)
    {
      aPoles.SetValue(u, v, gp_Pnt(10.0 * aX[u - 1], 10.0 * aY[u - 1], 5.0 * (v - 1)));
      aWeights.SetValue(u, v, u % 2 == 0 ? std::sqrt(0.5) : 1.0);
    }
  }
  NCollection_Array1<double> aUKnots(1, 5);
  NCollection_Array1<int>    aUMults(1, 5);
  aUMults.Init(2);
  for (int i = 1; i <= 5; ++i)
  {
    aUKnots.SetValue(i, (i - 1) / 4.0);
  }
  aUMults.SetValue(1, 3);
  aUMults.SetValue(5, 3);
  NCollection_Array1<double> aVKnots(1, 3);
  aVKnots.SetValue(1, 0.0);
  aVKnots.SetValue(2, 0.5);
  aVKnots.SetValue(3, 1.0);
  NCollection_Array1<int> aVMults(1, 3);
  aVMults.Init(2);
  aVMults.SetValue(2, 1);
  const occ::handle<Geom_BSplineSurface> aSurface =
    new Geom_BSplineSurface(aPoles, aWeights, aUKnots, aVKnots, aUMults, aVMults, 2, 1);
  ASSERT_TRUE(aSurface->IsUClosed());
  BRepBuilderAPI_MakeFace aMaker(aSurface, 0.0, 1.0, 0.0, 0.5, Precision::Confusion());
  ASSERT_TRUE(aMaker.IsDone());

  // Only the unused V interval opens; the face still has its cylindrical seam.
  aSurface->SetPole(9, 3, gp_Pnt(20.0, 0.0, 10.0));
  ASSERT_FALSE(aSurface->IsUClosed());
  ASSERT_TRUE(BRepCheck_Analyzer(aMaker.Face()).IsValid());
  const auto [isDone, aCount] = checkSelfFaces(aMaker.Face());
  ASSERT_TRUE(isDone);
  EXPECT_EQ(aCount, 0);
}

//==================================================================================================

TEST(BOPAlgo_CheckerSITest, ClosedSeamJoinsTwoSurfacePreimages)
{
  const occ::handle<Geom_BSplineSurface> aSurface = createCrossingSurface();
  const TopoDS_Vertex            aLeft   = BRepBuilderAPI_MakeVertex(aSurface->EvalD0(0.0, 0.25));
  const TopoDS_Vertex            aRight  = BRepBuilderAPI_MakeVertex(aSurface->EvalD0(1.0, 0.25));
  const occ::handle<Geom2d_Line> aBottom = new Geom2d_Line(gp_Pnt2d(0.0, 0.25), gp_Dir2d(1.0, 0.0));
  const occ::handle<Geom2d_Line> aTop    = new Geom2d_Line(gp_Pnt2d(0.0, 0.75), gp_Dir2d(1.0, 0.0));
  const occ::handle<Geom2d_Line> aLeftCurve =
    new Geom2d_Line(gp_Pnt2d(0.0, 0.0), gp_Dir2d(0.0, 1.0));
  const occ::handle<Geom2d_Line> aRightCurve =
    new Geom2d_Line(gp_Pnt2d(1.0, 0.0), gp_Dir2d(0.0, 1.0));
  BRepBuilderAPI_MakeEdge aSeamMaker(aBottom, aSurface, aLeft, aRight, 0.0, 1.0);
  BRepBuilderAPI_MakeEdge aLeftMaker(aLeftCurve, aSurface, aLeft, aLeft, 0.25, 0.75);
  BRepBuilderAPI_MakeEdge aRightMaker(aRightCurve, aSurface, aRight, aRight, 0.25, 0.75);
  ASSERT_TRUE(aSeamMaker.IsDone());
  ASSERT_TRUE(aLeftMaker.IsDone());
  ASSERT_TRUE(aRightMaker.IsDone());
  const TopoDS_Edge aSeam = aSeamMaker.Edge();
  BRep_Builder      aBuilder;
  TopoDS_Face       aFace;
  aBuilder.MakeFace(aFace, aSurface, Precision::Confusion());
  aBuilder.UpdateEdge(aSeam, aBottom, aTop, aFace, Precision::Confusion());
  aBuilder.Range(aSeam, aFace, 0.0, 1.0);
  TopoDS_Wire aWire;
  aBuilder.MakeWire(aWire);
  aBuilder.Add(aWire, aSeam);
  aBuilder.Add(aWire, aRightMaker.Edge());
  aBuilder.Add(aWire, aSeam.Reversed());
  aBuilder.Add(aWire, aLeftMaker.Edge().Reversed());
  aBuilder.Add(aFace, aWire);
  ASSERT_TRUE(BRepLib::BuildCurves3d(aFace));
  ASSERT_TRUE(BRep_Tool::IsClosed(aSeam, aFace));
  BRepLib::SameParameter(aFace);
  ASSERT_TRUE(BRepCheck_Analyzer(aFace).IsValid());
  const auto [isDone, aCount] = checkSelfFaces(aFace);
  ASSERT_TRUE(isDone);
  EXPECT_EQ(aCount, 0);
}
