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

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepCheck_Result.hxx>
#include <BRepCheck_Wire.hxx>
#include <BRepLib.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <NCollection_List.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <gp.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Ax3.hxx>
#include <gp_Circ.hxx>
#include <gp_Pln.hxx>

#include <gtest/gtest.h>

#include <cmath>
#include <vector>

namespace
{

std::vector<BRepCheck_Status> toVector(const NCollection_List<BRepCheck_Status>& theList)
{
  std::vector<BRepCheck_Status> aResult;
  for (NCollection_List<BRepCheck_Status>::Iterator anIt(theList); anIt.More(); anIt.Next())
  {
    aResult.push_back(anIt.Value());
  }
  return aResult;
}

// A cylindrical face with its seam edge and theHoles inner wires, each a
// circle drawn in the (U, V) space of the cylinder.
TopoDS_Face makeHoledCylinder(const int theHoles)
{
  const occ::handle<Geom_CylindricalSurface> aSurface =
    new Geom_CylindricalSurface(gp_Ax3(gp::XOY()), 10.0);
  BRepBuilderAPI_MakeFace aMaker(aSurface, 0.0, 2.0 * M_PI, 0.0, 20.0, Precision::Confusion());
  for (int i = 0; i < theHoles; ++i)
  {
    const gp_Pnt2d                   aCentre(0.5 + i * 0.6, 10.0);
    const occ::handle<Geom2d_Circle> aCircle =
      new Geom2d_Circle(gp_Ax2d(aCentre, gp_Dir2d(1.0, 0.0)), 0.2);
    TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(aCircle, aSurface).Edge();
    BRepLib::BuildCurves3d(anEdge);
    const TopoDS_Wire aHole = BRepBuilderAPI_MakeWire(anEdge).Wire();
    aMaker.Add(TopoDS::Wire(aHole.Reversed()));
  }
  return aMaker.Face();
}

TopoDS_Face makeHoledDisc(const int theHoles)
{
  const TopoDS_Wire anOuter =
    BRepBuilderAPI_MakeWire(BRepBuilderAPI_MakeEdge(gp_Circ(gp::XOY(), 100.0)).Edge()).Wire();
  BRepBuilderAPI_MakeFace aMaker(gp_Pln(gp::XOY()), anOuter);
  for (int i = 0; i < theHoles; ++i)
  {
    const gp_Pnt      aCentre(-60.0 + (i % 40) * 3.0, -60.0 + (i / 40) * 3.0, 0.0);
    const gp_Circ     aCircle(gp_Ax2(aCentre, gp::DZ()), 1.0);
    const TopoDS_Wire aHole =
      BRepBuilderAPI_MakeWire(BRepBuilderAPI_MakeEdge(aCircle).Edge()).Wire();
    aMaker.Add(TopoDS::Wire(aHole.Reversed()));
  }
  return aMaker.Face();
}

// Every wire's status list in the context of the face must be the same from
// the analyzer, which shares the face's seam edges between its wires, and
// from a stand-alone InContext(face), which lists them itself.
void checkAnalyzerMatchesInContext(const TopoDS_Face& theFace, const bool theIsParallel)
{
  BRepCheck_Analyzer anAnalyzer(theFace, true, theIsParallel);
  int                aWires = 0;
  for (TopExp_Explorer anExp(theFace, TopAbs_WIRE); anExp.More(); anExp.Next(), ++aWires)
  {
    const TopoDS_Wire&                   aWire     = TopoDS::Wire(anExp.Current());
    const occ::handle<BRepCheck_Result>& aAnalyzed = anAnalyzer.Result(aWire);
    ASSERT_FALSE(aAnalyzed.IsNull());
    ASSERT_TRUE(aAnalyzed->IsStatusOnShape(theFace));

    occ::handle<BRepCheck_Wire> aAlone = new BRepCheck_Wire(aWire);
    aAlone->InContext(theFace);
    ASSERT_TRUE(aAlone->IsStatusOnShape(theFace));

    EXPECT_EQ(toVector(aAnalyzed->StatusOnShape(theFace)),
              toVector(aAlone->StatusOnShape(theFace)));
  }
  EXPECT_GT(aWires, 1);
}

} // namespace

TEST(BRepCheck_WireTest, AnalyzerMatchesInContext_SeamFaceWithHoles)
{
  const TopoDS_Face aFace = makeHoledCylinder(8);
  ASSERT_FALSE(aFace.IsNull());
  checkAnalyzerMatchesInContext(aFace, false);
  checkAnalyzerMatchesInContext(aFace, true);
}

TEST(BRepCheck_WireTest, AnalyzerMatchesInContext_HoledDisc)
{
  const TopoDS_Face aFace = makeHoledDisc(200);
  ASSERT_FALSE(aFace.IsNull());
  checkAnalyzerMatchesInContext(aFace, false);
  checkAnalyzerMatchesInContext(aFace, true);
}

TEST(BRepCheck_WireTest, SharedSeamEdgesMatchClosed2d)
{
  const TopoDS_Face aCylinder = makeHoledCylinder(4);
  const TopoDS_Face aDisc     = makeHoledDisc(4);
  ASSERT_FALSE(aCylinder.IsNull());
  ASSERT_FALSE(aDisc.IsNull());

  // One instance crosses two faces to show it is refilled when the face changes.
  BRepCheck_Wire::FaceSeamEdges aSeamEdges;
  for (const TopoDS_Face& aFace : {aCylinder, aDisc, aCylinder})
  {
    for (TopExp_Explorer anExp(aFace, TopAbs_WIRE); anExp.More(); anExp.Next())
    {
      const TopoDS_Wire&          aWire   = TopoDS::Wire(anExp.Current());
      occ::handle<BRepCheck_Wire> aShared = new BRepCheck_Wire(aWire);
      occ::handle<BRepCheck_Wire> aAlone  = new BRepCheck_Wire(aWire);
      EXPECT_EQ(aShared->Closed2d(aFace, aSeamEdges), aAlone->Closed2d(aFace));
      EXPECT_TRUE(aSeamEdges.IsDone);
      EXPECT_TRUE(aSeamEdges.Face.IsEqual(aFace));
    }
  }
  BRepCheck_Wire::FaceSeamEdges aFresh;
  occ::handle<BRepCheck_Wire>   anOuter =
    new BRepCheck_Wire(TopoDS::Wire(TopExp_Explorer(aCylinder, TopAbs_WIRE).Current()));
  anOuter->Closed2d(aCylinder, aFresh);
  EXPECT_EQ(aFresh.Edges.Extent(), 1);
}
