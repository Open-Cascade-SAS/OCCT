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

// BRepTools_WireExplorer on a wire of a face whose edges have pcurves too short to step along.
// Related report: https://github.com/Open-Cascade-SAS/OCCT/issues/1568

#include <BRep_Builder.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_Plane.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <thread>

namespace
{
//! Edge of the face whose pcurve is the line through (0, 2) along +U, restricted to the range
//! [theFirst, theLast], which is a few units of the last place of 2.0 wide.
TopoDS_Edge makeEdge(const TopoDS_Face&       theFace,
                     const TopoDS_Vertex&     theV1,
                     const TopoDS_Vertex&     theV2,
                     const bool               theDegenerated,
                     const TopAbs_Orientation theOrient)
{
  const double aFirst = 1.9999999999999993;
  const double aLast  = 2.0000000000000004;
  BRep_Builder aBuilder;
  TopoDS_Edge  anEdge;
  aBuilder.MakeEdge(anEdge);
  aBuilder.UpdateEdge(anEdge,
                      new Geom2d_Line(gp_Pnt2d(0.0, 2.0), gp_Dir2d(1.0, 0.0)),
                      theFace,
                      1.e-7);
  aBuilder.Range(anEdge, aFirst, aLast);
  aBuilder.Degenerated(anEdge, theDegenerated);
  aBuilder.Add(anEdge, theV1.Oriented(TopAbs_FORWARD));
  aBuilder.Add(anEdge, theV2.Oriented(TopAbs_REVERSED));
  anEdge.Orientation(theOrient);
  return anEdge;
}
} // namespace

// The wire is the one a fillet leaves of a face that its neighbours consumed: all four edges
// are a rounding error long. Stepping along such a pcurve by a thousandth of its length does
// not move the parameter, and the explorer used to loop on it forever.
TEST(BRepTools_WireExplorerTest, OCCT1568_PCurvesShorterThanTheParameterSpacingTerminate)
{
  BRep_Builder aBuilder;
  TopoDS_Face  aFace;
  aBuilder.MakeFace(aFace, new Geom_Plane(gp_Pnt(0.0, 0.0, 4.0), gp::DZ()), 1.e-7);

  TopoDS_Vertex aV1;
  TopoDS_Vertex aV2;
  aBuilder.MakeVertex(aV1, gp_Pnt(2.0, 2.0, 4.0), 1.e-7);
  aBuilder.MakeVertex(aV2, gp_Pnt(2.0, 2.0, 4.0), 1.e-7);

  TopoDS_Wire aWire;
  aBuilder.MakeWire(aWire);
  aBuilder.Add(aWire, makeEdge(aFace, aV1, aV1, true, TopAbs_REVERSED));
  aBuilder.Add(aWire, makeEdge(aFace, aV1, aV2, false, TopAbs_FORWARD));
  aBuilder.Add(aWire, makeEdge(aFace, aV1, aV1, true, TopAbs_FORWARD));
  aBuilder.Add(aWire, makeEdge(aFace, aV2, aV1, false, TopAbs_REVERSED));
  aBuilder.Add(aFace, aWire);

  // A hang would block the test process, so the exploration runs on a thread with a deadline.
  std::atomic<int> aCount(-1);
  std::thread      aThread([&]() {
    int aN = 0;
    for (BRepTools_WireExplorer anExp(aWire, aFace); anExp.More() && aN < 100; anExp.Next())
    {
      ++aN;
    }
    aCount = aN;
  });
  for (int i = 0; i < 200 && aCount.load() < 0; ++i)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
  if (aCount.load() < 0)
  {
    aThread.detach();
    FAIL() << "BRepTools_WireExplorer did not terminate within 10 s";
  }
  aThread.join();
  EXPECT_LE(aCount.load(), 4);
}
