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

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepGProp.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <GProp_GProps.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>

#include <gtest/gtest.h>

TEST(BRepBuilderAPI_MakeFaceTest, FaceFromPlane)
{
  gp_Pln                  aPln(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  BRepBuilderAPI_MakeFace aMakeFace(aPln);
  ASSERT_TRUE(aMakeFace.IsDone()) << "Face from plane creation failed";

  TopoDS_Face aFace = aMakeFace.Face();
  ASSERT_FALSE(aFace.IsNull()) << "Resulting face is null";
}

TEST(BRepBuilderAPI_MakeFaceTest, FaceFromWire)
{
  // Create a rectangular wire in XY plane
  gp_Pnt aP1(0.0, 0.0, 0.0);
  gp_Pnt aP2(10.0, 0.0, 0.0);
  gp_Pnt aP3(10.0, 5.0, 0.0);
  gp_Pnt aP4(0.0, 5.0, 0.0);

  TopoDS_Edge anE1 = BRepBuilderAPI_MakeEdge(aP1, aP2).Edge();
  TopoDS_Edge anE2 = BRepBuilderAPI_MakeEdge(aP2, aP3).Edge();
  TopoDS_Edge anE3 = BRepBuilderAPI_MakeEdge(aP3, aP4).Edge();
  TopoDS_Edge anE4 = BRepBuilderAPI_MakeEdge(aP4, aP1).Edge();

  BRepBuilderAPI_MakeWire aMakeWire;
  aMakeWire.Add(anE1);
  aMakeWire.Add(anE2);
  aMakeWire.Add(anE3);
  aMakeWire.Add(anE4);
  ASSERT_TRUE(aMakeWire.IsDone()) << "Wire creation failed";

  BRepBuilderAPI_MakeFace aMakeFace(aMakeWire.Wire());
  ASSERT_TRUE(aMakeFace.IsDone()) << "Face from wire creation failed";

  TopoDS_Face aFace = aMakeFace.Face();
  ASSERT_FALSE(aFace.IsNull());

  BRepCheck_Analyzer anAnalyzer(aFace);
  EXPECT_TRUE(anAnalyzer.IsValid()) << "Face from wire is not valid";

  // Area should be 10 * 5 = 50
  GProp_GProps aProps;
  BRepGProp::SurfaceProperties(aFace, aProps);
  EXPECT_NEAR(aProps.Mass(), 50.0, Precision::Confusion());
}

TEST(BRepBuilderAPI_MakeFaceTest, FaceFromGeomPlane_WithBounds)
{
  occ::handle<Geom_Plane> aGeomPlane = new Geom_Plane(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  BRepBuilderAPI_MakeFace aMakeFace(aGeomPlane, 0.0, 10.0, 0.0, 5.0, Precision::Confusion());
  ASSERT_TRUE(aMakeFace.IsDone()) << "Face from Geom_Plane with bounds failed";

  TopoDS_Face aFace = aMakeFace.Face();
  ASSERT_FALSE(aFace.IsNull());

  BRepCheck_Analyzer anAnalyzer(aFace);
  EXPECT_TRUE(anAnalyzer.IsValid()) << "Bounded face is not valid";

  GProp_GProps aProps;
  BRepGProp::SurfaceProperties(aFace, aProps);
  EXPECT_NEAR(aProps.Mass(), 50.0, Precision::Confusion());
}

TEST(BRepBuilderAPI_MakeFaceTest, FaceFromCylindricalSurface)
{
  gp_Ax3                               anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  occ::handle<Geom_CylindricalSurface> aCylSurf = new Geom_CylindricalSurface(anAxis, 5.0);

  // Create a bounded face: U from 0 to 2*PI, V from 0 to 10
  BRepBuilderAPI_MakeFace aMakeFace(aCylSurf, 0.0, 2.0 * M_PI, 0.0, 10.0, Precision::Confusion());
  ASSERT_TRUE(aMakeFace.IsDone()) << "Face from cylindrical surface failed";

  TopoDS_Face aFace = aMakeFace.Face();
  ASSERT_FALSE(aFace.IsNull());

  BRepCheck_Analyzer anAnalyzer(aFace);
  EXPECT_TRUE(anAnalyzer.IsValid()) << "Cylindrical face is not valid";

  // Area of cylinder: 2*PI*R*H = 2*PI*5*10 = 100*PI
  GProp_GProps aProps;
  BRepGProp::SurfaceProperties(aFace, aProps);
  EXPECT_NEAR(aProps.Mass(), 2.0 * M_PI * 5.0 * 10.0, 0.01);
}
