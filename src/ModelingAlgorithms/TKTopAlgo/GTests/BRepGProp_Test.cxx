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

#include <BRepAlgoAPI_Cut.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepGProp.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Pnt.hxx>
#include <GProp_GProps.hxx>
#include <GProp_PrincipalProps.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>
#include <Standard_Handle.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

#include <gtest/gtest.h>

TEST(BRepGPropTest, LinearProperties_EdgeLength)
{
  gp_Pnt aP1(0.0, 0.0, 0.0);
  gp_Pnt aP2(3.0, 4.0, 0.0);

  BRepBuilderAPI_MakeEdge aMakeEdge(aP1, aP2);
  ASSERT_TRUE(aMakeEdge.IsDone());

  GProp_GProps aProps;
  BRepGProp::LinearProperties(aMakeEdge.Edge(), aProps);
  EXPECT_NEAR(aProps.Mass(), 5.0, Precision::Confusion()) << "Edge length should be 5";
}

TEST(BRepGPropTest, SurfaceProperties_BoxFaceArea)
{
  BRepPrimAPI_MakeBox aBox(10.0, 20.0, 30.0);
  TopoDS_Shape        aShape = aBox.Shape();
  ASSERT_TRUE(aBox.IsDone());

  // Total surface area = 2*(10*20 + 10*30 + 20*30) = 2*(200+300+600) = 2200
  GProp_GProps aProps;
  BRepGProp::SurfaceProperties(aShape, aProps);
  EXPECT_NEAR(aProps.Mass(), 2200.0, Precision::Confusion());
}

TEST(BRepGPropTest, VolumeProperties_UnitBox)
{
  BRepPrimAPI_MakeBox aBox(1.0, 1.0, 1.0);
  TopoDS_Shape        aShape = aBox.Shape();
  ASSERT_TRUE(aBox.IsDone());

  GProp_GProps aProps;
  BRepGProp::VolumeProperties(aShape, aProps);
  EXPECT_NEAR(aProps.Mass(), 1.0, Precision::Confusion()) << "Unit box volume should be 1";
}

TEST(BRepGPropTest, VolumeProperties_Sphere)
{
  const double           aRadius = 5.0;
  BRepPrimAPI_MakeSphere aSphere(aRadius);
  TopoDS_Shape           aShape = aSphere.Shape();
  ASSERT_TRUE(aSphere.IsDone());

  GProp_GProps aProps;
  BRepGProp::VolumeProperties(aShape, aProps);
  double anExpectedVolume = (4.0 / 3.0) * M_PI * aRadius * aRadius * aRadius;
  EXPECT_NEAR(aProps.Mass(), anExpectedVolume, 0.01) << "Sphere volume mismatch";
}

TEST(BRepGPropTest, VolumeProperties_BoxCenterOfMass)
{
  // Box from (0,0,0) to (10,10,10)
  BRepPrimAPI_MakeBox aBox(10.0, 10.0, 10.0);
  TopoDS_Shape        aShape = aBox.Shape();
  ASSERT_TRUE(aBox.IsDone());

  GProp_GProps aProps;
  BRepGProp::VolumeProperties(aShape, aProps);
  gp_Pnt aCOM = aProps.CentreOfMass();

  // Center of mass should be at (5, 5, 5)
  EXPECT_NEAR(aCOM.X(), 5.0, Precision::Confusion());
  EXPECT_NEAR(aCOM.Y(), 5.0, Precision::Confusion());
  EXPECT_NEAR(aCOM.Z(), 5.0, Precision::Confusion());
}

TEST(BRepGPropTest, LinearProperties_SkipShared)
{
  BRepPrimAPI_MakeBox aBox(10.0, 10.0, 10.0);
  TopoDS_Shape        aShape = aBox.Shape();
  ASSERT_TRUE(aBox.IsDone());

  // With SkipShared = false, shared edges are counted multiple times
  GProp_GProps aPropsNotSkipped;
  BRepGProp::LinearProperties(aShape, aPropsNotSkipped, false);

  // With SkipShared = true, each edge is counted once
  GProp_GProps aPropsSkipped;
  BRepGProp::LinearProperties(aShape, aPropsSkipped, true);

  // Box has 12 edges, each of length 10 = total 120
  EXPECT_NEAR(aPropsSkipped.Mass(), 120.0, Precision::Confusion())
    << "Total edge length of 10x10x10 box with SkipShared=true should be 120";

  // Without SkipShared, each edge is counted for each face it belongs to (2 faces per edge)
  // So total = 240
  EXPECT_NEAR(aPropsNotSkipped.Mass(), 240.0, Precision::Confusion())
    << "Total edge length with SkipShared=false should be double";
}

// Test OCC49: GProp_PrincipalProps::HasSymmetryAxis - cylinder has symmetry, cut does not.
// Migrated from QABugs_16.cxx OCC49
TEST(BRepGPropTest, OCC49_CylinderHasSymmetryAxis)
{
  const TopoDS_Shape aCylinder = BRepPrimAPI_MakeCylinder(10., 20.).Shape();

  GProp_GProps aProps;
  BRepGProp::VolumeProperties(aCylinder, aProps);
  const GProp_PrincipalProps aPrincipal = aProps.PrincipalProperties();
  EXPECT_TRUE(aPrincipal.HasSymmetryAxis());
}

TEST(BRepGPropTest, OCC49_CutShapeHasNoSymmetryAxis)
{
  const TopoDS_Shape aCylinder = BRepPrimAPI_MakeCylinder(10., 20.).Shape();
  const TopoDS_Shape aBox      = BRepPrimAPI_MakeBox(10., 10., 10.).Shape();

  BRepAlgoAPI_Cut aCut(aCylinder, aBox);
  ASSERT_TRUE(aCut.IsDone());

  GProp_GProps aProps;
  BRepGProp::VolumeProperties(aCut.Shape(), aProps);
  const GProp_PrincipalProps aPrincipal = aProps.PrincipalProperties();
  EXPECT_FALSE(aPrincipal.HasSymmetryAxis());
}

// OCC8797: Verify that GCPnts_AbscissaPoint::Length and BRepGProp::LinearProperties
// produce consistent arc-length values for a degree-3 BSpline curve with 7 poles.
// Both methods must agree within a tight relative tolerance.

TEST(BRepGPropTest, OCC8797_BSplineLengthConsistencyAbscissaVsLinearProperties)
{
  NCollection_Array1<gp_Pnt> aPoles(0, 6);
  aPoles(0) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(1) = gp_Pnt(1.0, 1.0, 0.0);
  aPoles(2) = gp_Pnt(2.0, 1.0, 0.0);
  aPoles(3) = gp_Pnt(3.0, 0.0, 0.0);
  aPoles(4) = gp_Pnt(4.0, 1.0, 0.0);
  aPoles(5) = gp_Pnt(5.0, 1.0, 0.0);
  aPoles(6) = gp_Pnt(6.0, 0.0, 0.0);

  NCollection_Array1<double> aKnots(0, 2);
  aKnots(0) = 0.0;
  aKnots(1) = 0.5;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMults(0, 2);
  aMults(0) = 4;
  aMults(1) = 3;
  aMults(2) = 4;

  occ::handle<Geom_BSplineCurve> aSpline = new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);
  ASSERT_FALSE(aSpline.IsNull());
  EXPECT_EQ(aSpline->NbPoles(), 7);
  EXPECT_EQ(aSpline->NbKnots(), 3);

  // Method 1: GCPnts_AbscissaPoint::Length
  GeomAdaptor_Curve anAdaptor(aSpline);
  const double      aLengthAbscissa = GCPnts_AbscissaPoint::Length(anAdaptor);
  EXPECT_GT(aLengthAbscissa, 0.0);

  // Method 2: BRepGProp::LinearProperties on the equivalent edge
  const TopoDS_Edge aEdge = BRepBuilderAPI_MakeEdge(aSpline);
  GProp_GProps      aEdgeProps;
  BRepGProp::LinearProperties(aEdge, aEdgeProps);
  const double aLengthGProp = aEdgeProps.Mass();
  EXPECT_GT(aLengthGProp, 0.0);

  // Both methods must agree within 0.1 %
  EXPECT_NEAR(aLengthAbscissa, aLengthGProp, aLengthGProp * 1e-3);
}
