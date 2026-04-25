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
#include <gp_Circ.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_List.hxx>
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
