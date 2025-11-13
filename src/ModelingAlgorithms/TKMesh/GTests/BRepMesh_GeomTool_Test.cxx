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

#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepMesh_GeomTool.hxx>
#include <Geom_Circle.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Precision.hxx>
#include <gp_Ax2.hxx>
#include <gp_Pln.hxx>

#include <gtest/gtest.h>

TEST(BRepMesh_GeomTool_Test, OCC25547_StaticMethodsExportAndFunctionality)
{
  // Bug OCC25547: static class methods not exported in BRepMesh_GeomTool
  // This test verifies that BRepMesh_GeomTool static methods are properly exported
  // and functional

  // Test 1: Discretize an arc
  const Standard_Real       aFirstP = 0., aLastP = M_PI;
  Handle(Geom_Circle)       aCircle = new Geom_Circle(gp_Ax2(gp::Origin(), gp::DZ()), 10);
  Handle(Geom_TrimmedCurve) aHalf   = new Geom_TrimmedCurve(aCircle, aFirstP, aLastP);
  TopoDS_Edge               anEdge  = BRepBuilderAPI_MakeEdge(aHalf);
  BRepAdaptor_Curve         anAdaptor(anEdge);
  BRepMesh_GeomTool         aGeomTool(anAdaptor, aFirstP, aLastP, 0.1, 0.5);

  EXPECT_GT(aGeomTool.NbPoints(), 0) << "BRepMesh_GeomTool failed to discretize an arc";

  // Test 2: Test Normal() static method
  TopoDS_Face                 aFace = BRepBuilderAPI_MakeFace(gp_Pln(gp::Origin(), gp::DZ()));
  BRepAdaptor_Surface         aSurf(aFace);
  Handle(BRepAdaptor_Surface) aHSurf = new BRepAdaptor_Surface(aSurf);

  gp_Pnt           aPnt;
  gp_Dir           aNormal;
  Standard_Boolean isNormalComputed = BRepMesh_GeomTool::Normal(aHSurf, 10., 10., aPnt, aNormal);

  EXPECT_TRUE(isNormalComputed) << "BRepMesh_GeomTool failed to compute a normal of surface";

  // Test 3: Test IntLinLin() static method - line-line intersection
  gp_XY aRefPnts[4] = {gp_XY(-10., -10.), gp_XY(10., 10.), gp_XY(-10., 10.), gp_XY(10., -10.)};

  gp_Pnt2d                   anIntPnt;
  Standard_Real              aParams[2];
  BRepMesh_GeomTool::IntFlag anIntFlag = BRepMesh_GeomTool::IntLinLin(aRefPnts[0],
                                                                      aRefPnts[1],
                                                                      aRefPnts[2],
                                                                      aRefPnts[3],
                                                                      anIntPnt.ChangeCoord(),
                                                                      aParams);

  Standard_Real aDiff = anIntPnt.Distance(gp::Origin2d());
  EXPECT_EQ(anIntFlag, BRepMesh_GeomTool::Cross)
    << "BRepMesh_GeomTool::IntLinLin should return Cross flag";
  EXPECT_LE(aDiff, Precision::PConfusion())
    << "BRepMesh_GeomTool failed to intersect two lines at origin";

  // Test 4: Test IntSegSeg() static method - segment-segment intersection
  anIntFlag = BRepMesh_GeomTool::IntSegSeg(aRefPnts[0],
                                           aRefPnts[1],
                                           aRefPnts[2],
                                           aRefPnts[3],
                                           Standard_False,
                                           Standard_False,
                                           anIntPnt);

  aDiff = anIntPnt.Distance(gp::Origin2d());
  EXPECT_EQ(anIntFlag, BRepMesh_GeomTool::Cross)
    << "BRepMesh_GeomTool::IntSegSeg should return Cross flag";
  EXPECT_LE(aDiff, Precision::PConfusion())
    << "BRepMesh_GeomTool failed to intersect two segments at origin";
}
