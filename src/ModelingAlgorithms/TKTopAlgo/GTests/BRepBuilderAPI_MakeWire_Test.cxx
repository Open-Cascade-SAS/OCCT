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

#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_ListOfShape.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

TEST(BRepBuilderAPI_MakeWire_Test, OCC27552_AddEdgesAndListOfEdges)
{
  // Bug OCC27552: Wire creation fails depending on the order of edges
  // This test verifies that BRepBuilderAPI_MakeWire can successfully add
  // edges individually and as a list

  BRep_Builder  aBB;
  TopoDS_Vertex aV1, aV2, aV3;
  TopoDS_Edge   anE1, anE2;

  // Create vertices
  aBB.MakeVertex(aV1, gp_Pnt(0, 0, 0), 0.1);
  aBB.MakeVertex(aV2, gp_Pnt(5, 0, 0), 0.1);
  aBB.MakeVertex(aV3, gp_Pnt(10, 0, 0), 0.1);

  // Create edges
  anE1 = BRepBuilderAPI_MakeEdge(aV1, aV2).Edge();
  anE2 = BRepBuilderAPI_MakeEdge(aV2, aV3).Edge();

  // Build wire with individually added edges
  BRepBuilderAPI_MakeWire aMW;
  EXPECT_NO_THROW(aMW.Add(anE1));
  EXPECT_NO_THROW(aMW.Add(anE2));

  // Create additional vertices and edges for list test
  TopoDS_Vertex aV4, aV5, aV6, aV7;
  TopoDS_Edge   anE3, anE4;
  aBB.MakeVertex(aV4, gp_Pnt(10, 0.05, 0), 0.07);
  aBB.MakeVertex(aV5, gp_Pnt(10, -0.05, 0), 0.07);
  aBB.MakeVertex(aV6, gp_Pnt(10, 2, 0), 0.07);
  aBB.MakeVertex(aV7, gp_Pnt(10, -2, 0), 0.07);

  anE3 = BRepBuilderAPI_MakeEdge(aV4, aV6).Edge();
  anE4 = BRepBuilderAPI_MakeEdge(aV5, aV7).Edge();

  // Add edges as a list
  TopTools_ListOfShape aListOfEdges;
  aListOfEdges.Append(anE3);
  aListOfEdges.Append(anE4);

  EXPECT_NO_THROW(aMW.Add(aListOfEdges));

  // Verify wire was created successfully
  EXPECT_TRUE(aMW.IsDone()) << "Wire builder should complete successfully";
  TopoDS_Wire aWire = aMW.Wire();
  EXPECT_FALSE(aWire.IsNull()) << "Resulting wire should not be null";
}
