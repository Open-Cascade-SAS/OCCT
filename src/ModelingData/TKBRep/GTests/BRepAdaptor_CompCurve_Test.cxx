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

#include <BRepAdaptor_CompCurve.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <gp_Pnt.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>

// Test OCC5696: BRepAdaptor_CompCurve::Edge() method
// Migrated from QABugs_5.cxx
TEST(BRepAdaptor_CompCurve_Test, OCC5696_EdgeMethod)
{
  // Create a simple edge from two points
  TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(gp_Pnt(0, 0, 0), gp_Pnt(2, 0, 0));

  // Create a wire from the edge
  TopoDS_Wire wire = BRepBuilderAPI_MakeWire(edge);

  // Create a composite curve adaptor
  BRepAdaptor_CompCurve curve(wire);

  // Get curve parameters
  Standard_Real first = curve.FirstParameter();
  Standard_Real last  = curve.LastParameter();
  Standard_Real par   = (first + last) / 2.0;

  // Test the Edge() method
  Standard_Real par_edge;
  TopoDS_Edge   edge_found;

  // The original test was checking that this method doesn't throw an exception
  // and returns valid parameter
  EXPECT_NO_THROW({
    curve.Edge(par, edge_found, par_edge);
  }) << "Edge() method should not throw an exception";

  // Verify that the returned edge is valid
  EXPECT_FALSE(edge_found.IsNull()) << "Returned edge should not be null";

  // Verify that the parameter is within valid range [0, edge length]
  EXPECT_GE(par_edge, 0.0) << "Edge parameter should be non-negative";
  EXPECT_LE(par_edge, 2.0) << "Edge parameter should not exceed edge length";

  // The parameter should be approximately half of the edge length
  EXPECT_NEAR(1.0, par_edge, 0.01) << "Edge parameter should be approximately 1.0";
}
