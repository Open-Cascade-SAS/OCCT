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

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <gp_Pnt.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

//=================================================================================================

// Bug BUC60870: BRepExtrema_DistShapeShape should correctly compute the minimum distance
// between a linear edge and a vertex.
// The edge runs from (0,0,0) to (0,1,0); the vertex is at (0,0.3,1).
// The theoretical minimum distance is 1.0 (perpendicular from (0,0.3,1) to (0,0.3,0)).
TEST(BRepExtrema_DistShapeShapeTest, BUC60870_EdgeToVertexMinimumDistance)
{
  const TopoDS_Edge   anEdge  = BRepBuilderAPI_MakeEdge(gp_Pnt(0, 0, 0), gp_Pnt(0, 1, 0)).Edge();
  const TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0.3, 1)).Vertex();

  BRepExtrema_DistShapeShape aDist(anEdge, aVertex, 2.0);

  ASSERT_TRUE(aDist.IsDone()) << "BRepExtrema_DistShapeShape failed to compute";
  ASSERT_GE(aDist.NbSolution(), 1) << "Expected at least one solution";

  const double aTheorDist  = 1.0;
  const double aPercentMax = 0.01; // 1% relative tolerance
  EXPECT_NEAR(aDist.Value(), aTheorDist, aTheorDist * aPercentMax)
    << "Minimum distance deviates from expected value 1.0";
}
