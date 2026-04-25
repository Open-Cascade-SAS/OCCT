// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <BRep_Tool.hxx>
#include <BRepAdaptor_CompCurve.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <Geom_Circle.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <NCollection_List.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopExp_Explorer.hxx>

// Test OCC5696: BRepAdaptor_CompCurve::Edge() method
// Migrated from QABugs_5.cxx
TEST(BRepAdaptor_CompCurve_Test, OCC5696_EdgeMethod)
{
  // Create a simple edge from two points
  TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(gp_Pnt(0, 0, 0), gp_Pnt(2, 0, 0));

  // Create a wire from the edge
  TopoDS_Wire aWire = BRepBuilderAPI_MakeWire(anEdge);

  // Create a composite curve adaptor
  BRepAdaptor_CompCurve aCurve(aWire);

  // Get curve parameters
  double aFirst = aCurve.FirstParameter();
  double aLast  = aCurve.LastParameter();
  double aPar   = (aFirst + aLast) / 2.0;

  // Test the Edge() method
  double      aParEdge = 0.0;
  TopoDS_Edge anEdgeFound;

  // The original test was checking that this method doesn't throw an exception
  // and returns valid parameter
  EXPECT_NO_THROW({ aCurve.Edge(aPar, anEdgeFound, aParEdge); })
    << "Edge() method should not throw an exception";

  // Verify that the returned edge is valid
  EXPECT_FALSE(anEdgeFound.IsNull()) << "Returned edge should not be null";

  // Verify that the parameter is within valid range [0, edge length]
  EXPECT_GE(aParEdge, 0.0) << "Edge parameter should be non-negative";
  EXPECT_LE(aParEdge, 2.0) << "Edge parameter should not exceed edge length";

  // The parameter should be approximately half of the edge length
  EXPECT_NEAR(1.0, aParEdge, 0.01) << "Edge parameter should be approximately 1.0";
}

// Test OCC29430: BRepAdaptor_CompCurve::Value() at boundary parameters matches wire vertices.
// The bug was that evaluating a composite curve at its First/LastParameter
// did not return the correct endpoint.
TEST(BRepAdaptor_CompCurve_Test, OCC29430_ArcBoundaryPoints)
{
  const double r45 = M_PI / 4.0, r225 = 3.0 * M_PI / 4.0;

  GC_MakeArcOfCircle arcMaker(
    gp_Circ(gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(gp_Dir::D::Z), gp_Dir(gp_Dir::D::X)), 1.0),
    r45,
    r225,
    true);
  BRepBuilderAPI_MakeEdge edgeMaker(arcMaker.Value());
  BRepBuilderAPI_MakeWire wireMaker(edgeMaker.Edge());
  const TopoDS_Wire aWire = wireMaker.Wire();

  BRepAdaptor_CompCurve aCurve(aWire);
  const gp_Pnt aStartPt = aCurve.Value(aCurve.FirstParameter());
  const gp_Pnt anEndPt  = aCurve.Value(aCurve.LastParameter());

  // Collect wire vertices
  NCollection_List<gp_Pnt> aVertices;
  for (TopExp_Explorer anExp(aWire, TopAbs_VERTEX); anExp.More(); anExp.Next())
  {
    aVertices.Append(BRep_Tool::Pnt(TopoDS::Vertex(anExp.Current())));
  }
  ASSERT_GE(aVertices.Size(), 1);

  // Start point should match one of the wire vertices (within 1e-7 tolerance)
  bool aStartMatchesAnyVertex = false;
  bool anEndMatchesAnyVertex  = false;
  for (const gp_Pnt& aV : aVertices)
  {
    if (aStartPt.Distance(aV) < 1.0e-7)
      aStartMatchesAnyVertex = true;
    if (anEndPt.Distance(aV) < 1.0e-7)
      anEndMatchesAnyVertex = true;
  }
  EXPECT_TRUE(aStartMatchesAnyVertex) << "Start point does not match any wire vertex";
  EXPECT_TRUE(anEndMatchesAnyVertex) << "End point does not match any wire vertex";
  EXPECT_GT(aStartPt.Distance(anEndPt), 1.0e-7) << "Start and end points should be different";
}

// Test OCC30869: BRepAdaptor_CompCurve D1 at boundary parameters of a wire with reversed edge.
// The bug was that a wire with a single reversed-orientation trimmed-circle edge returned
// incorrect boundary point coordinates and tangent directions.
// Migrated from QABugs_20.cxx OCC30869
TEST(BRepAdaptor_CompCurve_Test, OCC30869_ReversedEdgeBoundaryPoints)
{
  // Build a circle: center(1,0,0), Z-axis(0,-1,0), X-axis(0,0,-1), radius=1
  const gp_Ax2 anAx2(gp_Pnt(1., 0., 0.), gp_Dir(0., -1., 0.), gp_Dir(0., 0., -1.));
  Handle(Geom_Circle) aCircle = new Geom_Circle(anAx2, 1.0);

  const double t1 = M_PI / 2.0;         // 1.5707963267949
  const double t2 = 3.0 * M_PI / 2.0;   // 4.71238898038469

  Handle(Geom_TrimmedCurve) aTrimmed = new Geom_TrimmedCurve(aCircle, t1, t2);
  TopoDS_Edge               anEdge   = BRepBuilderAPI_MakeEdge(aTrimmed).Edge();

  // Reverse the edge, then wrap it in a wire
  anEdge.Orientation(TopAbs_REVERSED);
  TopoDS_Wire aWire = BRepBuilderAPI_MakeWire(anEdge).Wire();

  BRepAdaptor_CompCurve aBACC(aWire);
  const double          aFirst = aBACC.FirstParameter();
  const double          aLast  = aBACC.LastParameter();

  gp_Pnt aPFirst, aPLast;
  gp_Vec aVFirst, aVLast;
  aBACC.D1(aFirst, aPFirst, aVFirst);
  aBACC.D1(aLast, aPLast, aVLast);

  if (aVFirst.SquareMagnitude() > gp::Resolution())
    aVFirst.Normalize();
  if (aVLast.SquareMagnitude() > gp::Resolution())
    aVLast.Normalize();

  // Reference: inverse circle (normal = (0,1,0)), evaluated at the same parameters
  const gp_Ax2 anAx2Ref(gp_Pnt(1., 0., 0.), gp_Dir(0., 1., 0.), gp_Dir(0., 0., -1.));
  Handle(Geom_Circle) aCircleRef = new Geom_Circle(anAx2Ref, 1.0);

  gp_Pnt aRefP1, aRefP2;
  gp_Vec aRefV1, aRefV2;
  aCircleRef->D1(t1, aRefP1, aRefV1);
  aCircleRef->D1(t2, aRefP2, aRefV2);
  if (aRefV1.SquareMagnitude() > gp::Resolution())
    aRefV1.Normalize();
  if (aRefV2.SquareMagnitude() > gp::Resolution())
    aRefV2.Normalize();

  const double aTol = 1.e-7;
  EXPECT_NEAR(aPFirst.X(), aRefP1.X(), aTol) << "First point X";
  EXPECT_NEAR(aPFirst.Y(), aRefP1.Y(), aTol) << "First point Y";
  EXPECT_NEAR(aPFirst.Z(), aRefP1.Z(), aTol) << "First point Z";

  EXPECT_NEAR(aVFirst.X(), aRefV1.X(), aTol) << "First tangent X";
  EXPECT_NEAR(aVFirst.Y(), aRefV1.Y(), aTol) << "First tangent Y";
  EXPECT_NEAR(aVFirst.Z(), aRefV1.Z(), aTol) << "First tangent Z";

  EXPECT_NEAR(aPLast.X(), aRefP2.X(), aTol) << "Last point X";
  EXPECT_NEAR(aPLast.Y(), aRefP2.Y(), aTol) << "Last point Y";
  EXPECT_NEAR(aPLast.Z(), aRefP2.Z(), aTol) << "Last point Z";

  EXPECT_NEAR(aVLast.X(), aRefV2.X(), aTol) << "Last tangent X";
  EXPECT_NEAR(aVLast.Y(), aRefV2.Y(), aTol) << "Last tangent Y";
  EXPECT_NEAR(aVLast.Z(), aRefV2.Z(), aTol) << "Last tangent Z";
}
