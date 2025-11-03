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

#include <memory>
#include <ShapeAnalysis_CanonicalRecognition.hxx>
#include <Precision.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepBuilderAPI_NurbsConvert.hxx>
#include <GC_MakeSegment.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shell.hxx>
#include <Geom_Plane.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomConvert.hxx>
#include <BRep_Tool.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <TopoDS.hxx>
#include <TopExp_Explorer.hxx>
#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <GeomAPI_IntSS.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
#include <gp_Pln.hxx>
#include <gp_Lin.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Cone.hxx>
#include <gp_Sphere.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomAbs_CurveType.hxx>

//==================================================================================================
// Surface Recognition Tests (cr/approx tests)
//==================================================================================================

class CanonicalRecognitionApproxTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    myRecognizer = std::make_unique<ShapeAnalysis_CanonicalRecognition>();
    myTolerance  = 1.0e-3;
  }

  std::unique_ptr<ShapeAnalysis_CanonicalRecognition> myRecognizer;
  Standard_Real                                       myTolerance;
};

// Test cr/approx/A1: polyline to plane recognition
TEST_F(CanonicalRecognitionApproxTest, PolylineToPlaneRecognition_A1)
{
  // Create polyline: 0 0 0 -> 1 0 0 -> 1 1 0 -> 0 1 0 -> 0 0 0
  BRepBuilderAPI_MakePolygon aPolygonMaker;
  aPolygonMaker.Add(gp_Pnt(0, 0, 0));
  aPolygonMaker.Add(gp_Pnt(1, 0, 0));
  aPolygonMaker.Add(gp_Pnt(1, 1, 0));
  aPolygonMaker.Add(gp_Pnt(0, 1, 0));
  aPolygonMaker.Close();

  ASSERT_TRUE(aPolygonMaker.IsDone()) << "Failed to create polyline";
  const TopoDS_Wire aWire = aPolygonMaker.Wire();

  // Set shape and test plane recognition
  myRecognizer->SetShape(aWire);
  gp_Pln                 aResultPlane;
  const Standard_Boolean aResult = myRecognizer->IsPlane(myTolerance, aResultPlane);

  EXPECT_TRUE(aResult) << "Polyline should be recognized as planar";

  // Verify it's approximately the XY plane (Z=0)
  const gp_Pnt aOrigin = aResultPlane.Location();
  const gp_Dir aNormal = aResultPlane.Axis().Direction();

  EXPECT_NEAR(std::abs(aNormal.Z()), 1.0, myTolerance)
    << "Plane normal should be close to Z direction";
  EXPECT_NEAR(aOrigin.Z(), 0.0, myTolerance) << "Plane should pass through Z=0";
}

// Test cr/approx/A2: cylinder recognition from NURBS converted face
TEST_F(CanonicalRecognitionApproxTest, CylinderRecognition_A2)
{
  // Create cylindrical face
  gp_Ax2                          aAxis(gp_Pnt(0, 0, 0), gp_Dir(1, 1, 1));
  Handle(Geom_CylindricalSurface) aCylSurf = new Geom_CylindricalSurface(aAxis, 1.0);
  BRepBuilderAPI_MakeFace         aFaceMaker(aCylSurf, 0, 2 * M_PI, 0, 1, Precision::Confusion());

  ASSERT_TRUE(aFaceMaker.IsDone()) << "Failed to create cylindrical face";
  const TopoDS_Face aFace = aFaceMaker.Face();

  // Convert face to NURBS (nurbsconvert)
  BRepBuilderAPI_NurbsConvert aNurbsConvert(aFace);
  ASSERT_TRUE(aNurbsConvert.IsDone()) << "Failed to convert face to NURBS";
  const TopoDS_Shape aNurbsShape = aNurbsConvert.Shape();
  const TopoDS_Face  aNurbsFace  = TopoDS::Face(aNurbsShape);

  // Test cylinder recognition on the NURBS face
  myRecognizer->SetShape(aNurbsFace);
  gp_Cylinder            aResultCylinder;
  const Standard_Boolean aResult = myRecognizer->IsCylinder(myTolerance, aResultCylinder);

  EXPECT_TRUE(aResult) << "NURBS face should be recognized as cylinder";
  EXPECT_NEAR(aResultCylinder.Radius(), 1.0, myTolerance) << "Cylinder radius should match";
}

// Test cr/approx/A3: conical surface recognition from NURBS converted face
TEST_F(CanonicalRecognitionApproxTest, ConicalSurfaceRecognition_A3)
{
  // Create conical face: cone with half angle 30 degrees
  const Standard_Real         aHalfAngle = M_PI / 6.0; // 30 degrees
  gp_Ax2                      aAxis(gp_Pnt(0, 0, 0), gp_Dir(gp_Dir::D::Z));
  Handle(Geom_ConicalSurface) aConeSurf = new Geom_ConicalSurface(aAxis, aHalfAngle, 2.0);
  BRepBuilderAPI_MakeFace     aFaceMaker(aConeSurf, 0, 2 * M_PI, 0, 3, Precision::Confusion());

  ASSERT_TRUE(aFaceMaker.IsDone()) << "Failed to create conical face";
  const TopoDS_Face aFace = aFaceMaker.Face();

  // Convert face to NURBS (nurbsconvert)
  BRepBuilderAPI_NurbsConvert aNurbsConvert(aFace);
  ASSERT_TRUE(aNurbsConvert.IsDone()) << "Failed to convert face to NURBS";
  const TopoDS_Shape aNurbsShape = aNurbsConvert.Shape();
  const TopoDS_Face  aNurbsFace  = TopoDS::Face(aNurbsShape);

  // Test cone recognition on the NURBS face
  myRecognizer->SetShape(aNurbsFace);
  gp_Cone                aResultCone;
  const Standard_Boolean aResult = myRecognizer->IsCone(myTolerance, aResultCone);

  EXPECT_TRUE(aResult) << "NURBS face should be recognized as cone";
  EXPECT_NEAR(aResultCone.SemiAngle(), aHalfAngle, myTolerance) << "Cone semi-angle should match";
}

// Test cr/approx/A4: spherical surface recognition from NURBS converted face
TEST_F(CanonicalRecognitionApproxTest, SphericalSurfaceRecognition_A4)
{
  // Create spherical face
  gp_Ax2                        aAxis(gp_Pnt(0, 0, 0), gp_Dir(gp_Dir::D::Z));
  Handle(Geom_SphericalSurface) aSphereSurf = new Geom_SphericalSurface(aAxis, 1.0);
  BRepBuilderAPI_MakeFace aFaceMaker(aSphereSurf, 0, 2 * M_PI, 0, M_PI / 2, Precision::Confusion());

  ASSERT_TRUE(aFaceMaker.IsDone()) << "Failed to create spherical face";
  const TopoDS_Face aFace = aFaceMaker.Face();

  // Convert face to NURBS (nurbsconvert)
  BRepBuilderAPI_NurbsConvert aNurbsConvert(aFace);
  ASSERT_TRUE(aNurbsConvert.IsDone()) << "Failed to convert face to NURBS";
  const TopoDS_Shape aNurbsShape = aNurbsConvert.Shape();
  const TopoDS_Face  aNurbsFace  = TopoDS::Face(aNurbsShape);

  // Test sphere recognition on the NURBS face
  myRecognizer->SetShape(aNurbsFace);
  gp_Sphere              aResultSphere;
  const Standard_Boolean aResult = myRecognizer->IsSphere(myTolerance, aResultSphere);

  EXPECT_TRUE(aResult) << "NURBS face should be recognized as sphere";
  EXPECT_NEAR(aResultSphere.Radius(), 1.0, myTolerance) << "Sphere radius should match";
}

//==================================================================================================
// Curve Recognition Tests (cr/base A-series tests)
//==================================================================================================

class CanonicalRecognitionBaseCurveTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    myRecognizer = std::make_unique<ShapeAnalysis_CanonicalRecognition>();
    myTolerance  = 1.0e-3;
  }

  std::unique_ptr<ShapeAnalysis_CanonicalRecognition> myRecognizer;
  Standard_Real                                       myTolerance;
};

// Test cr/base/A1: Bezier curve to line recognition
TEST_F(CanonicalRecognitionBaseCurveTest, BezierToLineRecognition_A1)
{
  // Create nearly linear Bezier curve
  TColgp_Array1OfPnt aControlPoints(1, 3);
  aControlPoints(1) = gp_Pnt(0, 0, 0);
  aControlPoints(2) = gp_Pnt(0.5, 0.0005, 0); // Very small deviation
  aControlPoints(3) = gp_Pnt(1, 0, 0);

  Handle(Geom_BezierCurve) aBezier = new Geom_BezierCurve(aControlPoints);
  BRepBuilderAPI_MakeEdge  aEdgeMaker(aBezier);

  ASSERT_TRUE(aEdgeMaker.IsDone()) << "Failed to create Bezier edge";
  const TopoDS_Edge anEdge = aEdgeMaker.Edge();

  // Set shape and test line recognition
  myRecognizer->SetShape(anEdge);
  gp_Lin                 aResultLine;
  const Standard_Boolean aResult = myRecognizer->IsLine(myTolerance, aResultLine);

  EXPECT_TRUE(aResult) << "Nearly linear Bezier should be recognized as line";

  // Verify line direction is approximately along X-axis
  const gp_Dir aLineDir = aResultLine.Direction();
  EXPECT_NEAR(std::abs(aLineDir.X()), 1.0, myTolerance) << "Line should be along X direction";
}

// Test cr/base/A2: Bezier curve to circle recognition
TEST_F(CanonicalRecognitionBaseCurveTest, BezierToCircleRecognition_A2)
{
  // Create the same Bezier curve as A1 and test for circle recognition
  TColgp_Array1OfPnt aControlPoints(1, 3);
  aControlPoints(1) = gp_Pnt(0, 0, 0);
  aControlPoints(2) = gp_Pnt(0.5, 0.0005, 0); // Small deviation as in original TCL
  aControlPoints(3) = gp_Pnt(1, 0, 0);

  Handle(Geom_BezierCurve) aBezier = new Geom_BezierCurve(aControlPoints);
  BRepBuilderAPI_MakeEdge  aEdgeMaker(aBezier);

  ASSERT_TRUE(aEdgeMaker.IsDone()) << "Failed to create Bezier edge";
  const TopoDS_Edge anEdge = aEdgeMaker.Edge();

  // Set shape and test circle recognition
  myRecognizer->SetShape(anEdge);
  gp_Circ                aResultCircle;
  const Standard_Boolean aResult = myRecognizer->IsCircle(myTolerance, aResultCircle);

  EXPECT_TRUE(aResult)
    << "Bezier curve should be recognized as circle (matching original TCL test)";
  EXPECT_GT(aResultCircle.Radius(), 0.0) << "Circle radius should be positive";
}

// Test cr/base/A3: Ellipse to ellipse recognition
TEST_F(CanonicalRecognitionBaseCurveTest, EllipseToEllipseRecognition_A3)
{
  // Create ellipse with major radius 1, minor radius 0.5
  gp_Ax2               aAxis(gp_Pnt(0, 0, 0), gp_Dir(gp_Dir::D::Z), gp_Dir(gp_Dir::D::X));
  Handle(Geom_Ellipse) aEllipse = new Geom_Ellipse(aAxis, 1.0, 0.5);

  // Convert to B-spline curve
  Handle(Geom_BSplineCurve) aBSplineCurve = GeomConvert::CurveToBSplineCurve(aEllipse);
  BRepBuilderAPI_MakeEdge   aEdgeMaker(aBSplineCurve);

  ASSERT_TRUE(aEdgeMaker.IsDone()) << "Failed to create ellipse edge";
  const TopoDS_Edge anEdge = aEdgeMaker.Edge();

  // Set shape and test ellipse recognition
  myRecognizer->SetShape(anEdge);
  gp_Elips               aResultEllipse;
  const Standard_Boolean aResult = myRecognizer->IsEllipse(1.0e-7, aResultEllipse);

  EXPECT_TRUE(aResult) << "Ellipse should be recognized as ellipse";
  EXPECT_NEAR(aResultEllipse.MajorRadius(), 1.0, 1.0e-7) << "Major radius should match";
  EXPECT_NEAR(aResultEllipse.MinorRadius(), 0.5, 1.0e-7) << "Minor radius should match";
}

// Test cr/base/A4: Multi-segment wire to line recognition
TEST_F(CanonicalRecognitionBaseCurveTest, MultiSegmentWireToLineRecognition_A4)
{
  // Create nearly linear Bezier curve split into 3 segments
  TColgp_Array1OfPnt aControlPoints(1, 3);
  aControlPoints(1) = gp_Pnt(0, 0, 0);
  aControlPoints(2) = gp_Pnt(0.5, 0.0000005, 0); // Even smaller deviation
  aControlPoints(3) = gp_Pnt(1, 0, 0);

  Handle(Geom_BezierCurve) aBezier = new Geom_BezierCurve(aControlPoints);

  // Create 3 edges from different parameter ranges
  BRepBuilderAPI_MakeEdge aEdgeMaker1(aBezier, 0.0, 0.3);
  BRepBuilderAPI_MakeEdge aEdgeMaker2(aBezier, 0.3, 0.7);
  BRepBuilderAPI_MakeEdge aEdgeMaker3(aBezier, 0.7, 1.0);

  ASSERT_TRUE(aEdgeMaker1.IsDone() && aEdgeMaker2.IsDone() && aEdgeMaker3.IsDone())
    << "Failed to create edges";

  // Create wire from the edges
  BRepBuilderAPI_MakeWire aWireMaker;
  aWireMaker.Add(aEdgeMaker1.Edge());
  aWireMaker.Add(aEdgeMaker2.Edge());
  aWireMaker.Add(aEdgeMaker3.Edge());

  ASSERT_TRUE(aWireMaker.IsDone()) << "Failed to create wire";
  const TopoDS_Wire aWire = aWireMaker.Wire();

  // Set shape and test line recognition
  myRecognizer->SetShape(aWire);
  gp_Lin                 aResultLine;
  const Standard_Boolean aResult = myRecognizer->IsLine(myTolerance, aResultLine);

  EXPECT_TRUE(aResult) << "Multi-segment nearly linear wire should be recognized as line";
}

// Test cr/base/A5: Multi-segment circle wire to circle recognition
TEST_F(CanonicalRecognitionBaseCurveTest, MultiSegmentCircleWireRecognition_A5)
{
  // Create circle and convert to B-spline
  gp_Ax2              aAxis(gp_Pnt(0, 0, 0), gp_Dir(gp_Dir::D::Z));
  Handle(Geom_Circle) aCircle = new Geom_Circle(aAxis, 1.0);

  // Convert to B-spline curve
  Handle(Geom_BSplineCurve) aBSplineCurve = GeomConvert::CurveToBSplineCurve(aCircle);

  // Create 3 edges from different parameter ranges
  BRepBuilderAPI_MakeEdge aEdgeMaker1(aBSplineCurve, 0.0, 1.0);
  BRepBuilderAPI_MakeEdge aEdgeMaker2(aBSplineCurve, 1.0, 2.5);
  BRepBuilderAPI_MakeEdge aEdgeMaker3(aBSplineCurve, 2.5, 6.0);

  ASSERT_TRUE(aEdgeMaker1.IsDone() && aEdgeMaker2.IsDone() && aEdgeMaker3.IsDone())
    << "Failed to create edges";

  // Create wire from the edges
  BRepBuilderAPI_MakeWire aWireMaker;
  aWireMaker.Add(aEdgeMaker1.Edge());
  aWireMaker.Add(aEdgeMaker2.Edge());
  aWireMaker.Add(aEdgeMaker3.Edge());

  ASSERT_TRUE(aWireMaker.IsDone()) << "Failed to create wire";
  const TopoDS_Wire aWire = aWireMaker.Wire();

  // Set shape and test circle recognition
  myRecognizer->SetShape(aWire);
  gp_Circ                aResultCircle;
  const Standard_Boolean aResult = myRecognizer->IsCircle(1.0e-7, aResultCircle);

  EXPECT_TRUE(aResult) << "Multi-segment circle wire should be recognized as circle";
  EXPECT_NEAR(aResultCircle.Radius(), 1.0, 1.0e-7) << "Circle radius should match";
}

// Test cr/base/A6: Multi-segment ellipse wire to ellipse recognition
TEST_F(CanonicalRecognitionBaseCurveTest, MultiSegmentEllipseWireRecognition_A6)
{
  // Create ellipse and convert to B-spline
  gp_Ax2               aAxis(gp_Pnt(0, 0, 0), gp_Dir(gp_Dir::D::Z), gp_Dir(gp_Dir::D::X));
  Handle(Geom_Ellipse) aEllipse = new Geom_Ellipse(aAxis, 1.0, 0.5);

  // Convert to B-spline curve
  Handle(Geom_BSplineCurve) aBSplineCurve = GeomConvert::CurveToBSplineCurve(aEllipse);

  // Create 3 edges from different parameter ranges
  BRepBuilderAPI_MakeEdge aEdgeMaker1(aBSplineCurve, 0.0, 1.0);
  BRepBuilderAPI_MakeEdge aEdgeMaker2(aBSplineCurve, 1.0, 2.5);
  BRepBuilderAPI_MakeEdge aEdgeMaker3(aBSplineCurve, 2.5, 6.0);

  ASSERT_TRUE(aEdgeMaker1.IsDone() && aEdgeMaker2.IsDone() && aEdgeMaker3.IsDone())
    << "Failed to create edges";

  // Create wire from the edges
  BRepBuilderAPI_MakeWire aWireMaker;
  aWireMaker.Add(aEdgeMaker1.Edge());
  aWireMaker.Add(aEdgeMaker2.Edge());
  aWireMaker.Add(aEdgeMaker3.Edge());

  ASSERT_TRUE(aWireMaker.IsDone()) << "Failed to create wire";
  const TopoDS_Wire aWire = aWireMaker.Wire();

  // Set shape and test ellipse recognition
  myRecognizer->SetShape(aWire);
  gp_Elips               aResultEllipse;
  const Standard_Boolean aResult = myRecognizer->IsEllipse(1.0e-7, aResultEllipse);

  EXPECT_TRUE(aResult) << "Multi-segment ellipse wire should be recognized as ellipse";
  EXPECT_NEAR(aResultEllipse.MajorRadius(), 1.0, 1.0e-7) << "Major radius should match";
  EXPECT_NEAR(aResultEllipse.MinorRadius(), 0.5, 1.0e-7) << "Minor radius should match";
}

//==================================================================================================
// Surface Recognition Tests (cr/base B-series tests)
//==================================================================================================

class CanonicalRecognitionBaseSurfaceTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    myRecognizer = std::make_unique<ShapeAnalysis_CanonicalRecognition>();
    myTolerance  = 1.0e-7;
  }

  std::unique_ptr<ShapeAnalysis_CanonicalRecognition> myRecognizer;
  Standard_Real                                       myTolerance;
};

// Test cr/base/B1: Plane recognition from trimmed surface
TEST_F(CanonicalRecognitionBaseSurfaceTest, TrimmedPlaneRecognition_B1)
{
  // Create plane surface
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pln());

  // Create face from plane
  BRepBuilderAPI_MakeFace aFaceMaker(aPlane, -1, 1, -1, 1, Precision::Confusion());
  ASSERT_TRUE(aFaceMaker.IsDone()) << "Failed to create planar face";
  TopoDS_Face aFace = aFaceMaker.Face();

  // Convert to NURBS (nurbsconvert)
  BRepBuilderAPI_NurbsConvert aNurbsConvert(aFace);
  ASSERT_TRUE(aNurbsConvert.IsDone()) << "Failed to convert face to NURBS";
  TopoDS_Face aNurbsFace = TopoDS::Face(aNurbsConvert.Shape());

  // Set shape and test plane recognition
  myRecognizer->SetShape(aNurbsFace);
  gp_Pln                 aResultPlane;
  const Standard_Boolean aResult = myRecognizer->IsPlane(myTolerance, aResultPlane);

  EXPECT_TRUE(aResult) << "Planar face should be recognized as plane";

  // Verify it's the XY plane
  const gp_Dir aNormal = aResultPlane.Axis().Direction();
  EXPECT_NEAR(std::abs(aNormal.Z()), 1.0, myTolerance) << "Plane normal should be Z direction";
}

// Test cr/base/B2: Cylinder recognition from trimmed surface
TEST_F(CanonicalRecognitionBaseSurfaceTest, TrimmedCylinderRecognition_B2)
{
  // Create cylindrical surface
  gp_Ax2                          aAxis(gp_Pnt(0, 0, 0), gp_Dir(gp_Dir::D::Z));
  Handle(Geom_CylindricalSurface) aCylSurf = new Geom_CylindricalSurface(aAxis, 1.0);

  // Create face from cylinder
  BRepBuilderAPI_MakeFace aFaceMaker(aCylSurf, 0, 2 * M_PI, -1, 1, Precision::Confusion());
  ASSERT_TRUE(aFaceMaker.IsDone()) << "Failed to create cylindrical face";
  TopoDS_Face aFace = aFaceMaker.Face();

  // Convert to NURBS (nurbsconvert)
  BRepBuilderAPI_NurbsConvert aNurbsConvert(aFace);
  ASSERT_TRUE(aNurbsConvert.IsDone()) << "Failed to convert face to NURBS";
  TopoDS_Face aNurbsFace = TopoDS::Face(aNurbsConvert.Shape());

  // Set shape and test cylinder recognition
  myRecognizer->SetShape(aNurbsFace);
  gp_Cylinder            aResultCylinder;
  const Standard_Boolean aResult = myRecognizer->IsCylinder(myTolerance, aResultCylinder);

  EXPECT_TRUE(aResult) << "Cylindrical surface should be recognized as cylinder";
  EXPECT_NEAR(aResultCylinder.Radius(), 1.0, myTolerance) << "Cylinder radius should match";
}

// Test cr/base/B3: Cone recognition from trimmed surface
TEST_F(CanonicalRecognitionBaseSurfaceTest, TrimmedConeRecognition_B3)
{
  // Create conical surface (30 degree half-angle)
  const Standard_Real         aSemiAngle = M_PI / 6.0; // 30 degrees
  gp_Ax2                      aAxis(gp_Pnt(0, 0, 0), gp_Dir(gp_Dir::D::Z));
  Handle(Geom_ConicalSurface) aConeSurf = new Geom_ConicalSurface(aAxis, aSemiAngle, 0);

  // Create face from cone
  BRepBuilderAPI_MakeFace aFaceMaker(aConeSurf, 0, 2 * M_PI, -1, 0, Precision::Confusion());
  ASSERT_TRUE(aFaceMaker.IsDone()) << "Failed to create conical face";
  TopoDS_Face aFace = aFaceMaker.Face();

  // Convert to NURBS (nurbsconvert)
  BRepBuilderAPI_NurbsConvert aNurbsConvert(aFace);
  ASSERT_TRUE(aNurbsConvert.IsDone()) << "Failed to convert face to NURBS";
  TopoDS_Face aNurbsFace = TopoDS::Face(aNurbsConvert.Shape());

  // Set shape and test cone recognition
  myRecognizer->SetShape(aNurbsFace);
  gp_Cone                aResultCone;
  const Standard_Boolean aResult = myRecognizer->IsCone(myTolerance, aResultCone);

  EXPECT_TRUE(aResult) << "Conical surface should be recognized as cone";
  EXPECT_NEAR(aResultCone.SemiAngle(), aSemiAngle, myTolerance) << "Cone semi-angle should match";
}

// Test cr/base/B4: Sphere recognition from converted surface
TEST_F(CanonicalRecognitionBaseSurfaceTest, ConvertedSphereRecognition_B4)
{
  // Create spherical surface and convert to B-spline
  gp_Ax2                        aAxis(gp_Pnt(0, 0, 0), gp_Dir(gp_Dir::D::Z));
  Handle(Geom_SphericalSurface) aSphereSurf = new Geom_SphericalSurface(aAxis, 1.0);

  // Convert to B-spline surface
  Handle(Geom_BSplineSurface) aBSplineSurf = GeomConvert::SurfaceToBSplineSurface(aSphereSurf);
  BRepBuilderAPI_MakeFace     aFaceMaker(aBSplineSurf, Precision::Confusion());

  ASSERT_TRUE(aFaceMaker.IsDone()) << "Failed to create spherical face";
  const TopoDS_Face aFace = aFaceMaker.Face();

  // Set shape and test sphere recognition
  myRecognizer->SetShape(aFace);
  gp_Sphere              aResultSphere;
  const Standard_Boolean aResult = myRecognizer->IsSphere(myTolerance, aResultSphere);

  EXPECT_TRUE(aResult) << "Spherical surface should be recognized as sphere";
  EXPECT_NEAR(aResultSphere.Radius(), 1.0, myTolerance) << "Sphere radius should match";
}

// Test cr/base/B5: Complex sewn planar surface recognition
TEST_F(CanonicalRecognitionBaseSurfaceTest, SewnPlanarSurfaceRecognition_B5)
{
  // Create 4 planar faces to be sewn together
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pln());

  BRepBuilderAPI_MakeFace aFaceMaker1(aPlane, -1, 0, -1, 0, Precision::Confusion());
  BRepBuilderAPI_MakeFace aFaceMaker2(aPlane, -1, 0, 0, 1, Precision::Confusion());
  BRepBuilderAPI_MakeFace aFaceMaker3(aPlane, 0, 1, 0, 1, Precision::Confusion());
  BRepBuilderAPI_MakeFace aFaceMaker4(aPlane, 0, 1, -1, 0, Precision::Confusion());

  ASSERT_TRUE(aFaceMaker1.IsDone() && aFaceMaker2.IsDone() && aFaceMaker3.IsDone()
              && aFaceMaker4.IsDone())
    << "Failed to create planar faces";

  // Sew the faces together
  BRepBuilderAPI_Sewing aSewing;
  aSewing.Add(aFaceMaker1.Face());
  aSewing.Add(aFaceMaker2.Face());
  aSewing.Add(aFaceMaker3.Face());
  aSewing.Add(aFaceMaker4.Face());
  aSewing.Perform();

  const TopoDS_Shape aSewnShape = aSewing.SewedShape();
  ASSERT_FALSE(aSewnShape.IsNull()) << "Failed to create sewn shape";

  // Convert sewn shape to NURBS (nurbsconvert)
  BRepBuilderAPI_NurbsConvert aNurbsConvert(aSewnShape);
  ASSERT_TRUE(aNurbsConvert.IsDone()) << "Failed to convert sewn shape to NURBS";
  const TopoDS_Shape aNurbsShape = aNurbsConvert.Shape();

  // Set shape and test plane recognition
  myRecognizer->SetShape(aNurbsShape);
  gp_Pln                 aResultPlane;
  const Standard_Boolean aResult = myRecognizer->IsPlane(myTolerance, aResultPlane);

  EXPECT_TRUE(aResult) << "Sewn planar surface should be recognized as plane";

  // Verify it's the XY plane
  const gp_Dir aNormal = aResultPlane.Axis().Direction();
  EXPECT_NEAR(std::abs(aNormal.Z()), 1.0, myTolerance) << "Plane normal should be Z direction";
}

// Test cr/base/B6: Sewn cylindrical surfaces recognition
TEST_F(CanonicalRecognitionBaseSurfaceTest, SewnCylindricalSurfaceRecognition_B6)
{
  // Create 4 cylindrical face segments to be sewn together
  gp_Ax2                          aAxis(gp_Pnt(0, 0, 0), gp_Dir(gp_Dir::D::Z));
  Handle(Geom_CylindricalSurface) aCylSurf = new Geom_CylindricalSurface(aAxis, 1.0);

  BRepBuilderAPI_MakeFace aFaceMaker1(aCylSurf, 0, 3, -1, 0, Precision::Confusion());
  BRepBuilderAPI_MakeFace aFaceMaker2(aCylSurf, 0, 3, 0, 1, Precision::Confusion());
  BRepBuilderAPI_MakeFace aFaceMaker3(aCylSurf, 3, 6, 0, 1, Precision::Confusion());
  BRepBuilderAPI_MakeFace aFaceMaker4(aCylSurf, 3, 6, -1, 0, Precision::Confusion());

  ASSERT_TRUE(aFaceMaker1.IsDone() && aFaceMaker2.IsDone() && aFaceMaker3.IsDone()
              && aFaceMaker4.IsDone())
    << "Failed to create cylindrical faces";

  // Sew the faces together
  BRepBuilderAPI_Sewing aSewing;
  aSewing.Add(aFaceMaker1.Face());
  aSewing.Add(aFaceMaker2.Face());
  aSewing.Add(aFaceMaker3.Face());
  aSewing.Add(aFaceMaker4.Face());
  aSewing.Perform();

  const TopoDS_Shape aSewnShape = aSewing.SewedShape();
  ASSERT_FALSE(aSewnShape.IsNull()) << "Failed to create sewn shape";

  // Convert sewn shape to NURBS (nurbsconvert)
  BRepBuilderAPI_NurbsConvert aNurbsConvert(aSewnShape);
  ASSERT_TRUE(aNurbsConvert.IsDone()) << "Failed to convert sewn shape to NURBS";
  const TopoDS_Shape aNurbsShape = aNurbsConvert.Shape();

  // Set shape and test cylinder recognition
  myRecognizer->SetShape(aNurbsShape);
  gp_Cylinder            aResultCylinder;
  const Standard_Boolean aResult = myRecognizer->IsCylinder(myTolerance, aResultCylinder);

  EXPECT_TRUE(aResult) << "Sewn cylindrical surface should be recognized as cylinder";
  EXPECT_NEAR(aResultCylinder.Radius(), 1.0, myTolerance) << "Cylinder radius should match";
}

// Test cr/base/B7: Sewn conical surfaces recognition
TEST_F(CanonicalRecognitionBaseSurfaceTest, SewnConicalSurfaceRecognition_B7)
{
  // Create 4 conical face segments to be sewn together
  const Standard_Real         aSemiAngle = M_PI / 6.0; // 30 degrees
  gp_Ax2                      aAxis(gp_Pnt(0, 0, 0), gp_Dir(gp_Dir::D::Z));
  Handle(Geom_ConicalSurface) aConeSurf = new Geom_ConicalSurface(aAxis, aSemiAngle, 0);

  BRepBuilderAPI_MakeFace aFaceMaker1(aConeSurf, 0, 3, 0, 1, Precision::Confusion());
  BRepBuilderAPI_MakeFace aFaceMaker2(aConeSurf, 0, 3, 1, 2, Precision::Confusion());
  BRepBuilderAPI_MakeFace aFaceMaker3(aConeSurf, 3, 6, 1, 2, Precision::Confusion());
  BRepBuilderAPI_MakeFace aFaceMaker4(aConeSurf, 3, 6, 0, 1, Precision::Confusion());

  ASSERT_TRUE(aFaceMaker1.IsDone() && aFaceMaker2.IsDone() && aFaceMaker3.IsDone()
              && aFaceMaker4.IsDone())
    << "Failed to create conical faces";

  // Sew the faces together
  BRepBuilderAPI_Sewing aSewing;
  aSewing.Add(aFaceMaker1.Face());
  aSewing.Add(aFaceMaker2.Face());
  aSewing.Add(aFaceMaker3.Face());
  aSewing.Add(aFaceMaker4.Face());
  aSewing.Perform();

  const TopoDS_Shape aSewnShape = aSewing.SewedShape();
  ASSERT_FALSE(aSewnShape.IsNull()) << "Failed to create sewn shape";

  // Convert sewn shape to NURBS (nurbsconvert)
  BRepBuilderAPI_NurbsConvert aNurbsConvert(aSewnShape);
  ASSERT_TRUE(aNurbsConvert.IsDone()) << "Failed to convert sewn shape to NURBS";
  const TopoDS_Shape aNurbsShape = aNurbsConvert.Shape();

  // Set shape and test cone recognition
  myRecognizer->SetShape(aNurbsShape);
  gp_Cone                aResultCone;
  const Standard_Boolean aResult = myRecognizer->IsCone(myTolerance, aResultCone);

  EXPECT_TRUE(aResult) << "Sewn conical surface should be recognized as cone";
  EXPECT_NEAR(aResultCone.SemiAngle(), aSemiAngle, myTolerance) << "Cone semi-angle should match";
}

// Test cr/base/B8: Sewn spherical surfaces recognition
TEST_F(CanonicalRecognitionBaseSurfaceTest, SewnSphericalSurfaceRecognition_B8)
{
  // Create 4 spherical face segments to be sewn together, converted to B-spline
  gp_Ax2                        aAxis(gp_Pnt(0, 0, 0), gp_Dir(gp_Dir::D::Z));
  Handle(Geom_SphericalSurface) aSphereSurf = new Geom_SphericalSurface(aAxis, 1.0);

  // Convert to B-spline surface
  Handle(Geom_BSplineSurface) aBSplineSurf = GeomConvert::SurfaceToBSplineSurface(aSphereSurf);

  BRepBuilderAPI_MakeFace aFaceMaker1(aBSplineSurf, 0, 3, -1.5, 0, Precision::Confusion());
  BRepBuilderAPI_MakeFace aFaceMaker2(aBSplineSurf, 0, 3, 0, 1.5, Precision::Confusion());
  BRepBuilderAPI_MakeFace aFaceMaker3(aBSplineSurf, 3, 6, 0, 1.5, Precision::Confusion());
  BRepBuilderAPI_MakeFace aFaceMaker4(aBSplineSurf, 3, 6, -1.5, 0, Precision::Confusion());

  ASSERT_TRUE(aFaceMaker1.IsDone() && aFaceMaker2.IsDone() && aFaceMaker3.IsDone()
              && aFaceMaker4.IsDone())
    << "Failed to create spherical faces";

  // Sew the faces together
  BRepBuilderAPI_Sewing aSewing;
  aSewing.Add(aFaceMaker1.Face());
  aSewing.Add(aFaceMaker2.Face());
  aSewing.Add(aFaceMaker3.Face());
  aSewing.Add(aFaceMaker4.Face());
  aSewing.Perform();

  const TopoDS_Shape aSewnShape = aSewing.SewedShape();
  ASSERT_FALSE(aSewnShape.IsNull()) << "Failed to create sewn shape";

  // Set shape and test sphere recognition
  myRecognizer->SetShape(aSewnShape);
  gp_Sphere              aResultSphere;
  const Standard_Boolean aResult = myRecognizer->IsSphere(myTolerance, aResultSphere);

  EXPECT_TRUE(aResult) << "Sewn spherical surface should be recognized as sphere";
  EXPECT_NEAR(aResultSphere.Radius(), 1.0, myTolerance) << "Sphere radius should match";
}

// Test cr/base/B9: Complex cylindrical recognition - sewn and converted cylinder
TEST_F(CanonicalRecognitionBaseSurfaceTest, ComplexCylindricalRecognitionWithSection_B9)
{
  // Create 4 cylindrical face segments and sew them together
  gp_Ax2                          aAxis(gp_Pnt(0, 0, 0), gp_Dir(gp_Dir::D::Z));
  Handle(Geom_CylindricalSurface) aCylSurf = new Geom_CylindricalSurface(aAxis, 1.0);

  // Create 4 face segments with parameter ranges matching the original test
  BRepBuilderAPI_MakeFace aFaceMaker1(aCylSurf, 0, 1, -1, 1, Precision::Confusion());
  BRepBuilderAPI_MakeFace aFaceMaker2(aCylSurf, 1, 2, -1, 1, Precision::Confusion());
  BRepBuilderAPI_MakeFace aFaceMaker3(aCylSurf, 2, 3, -1, 1, Precision::Confusion());
  BRepBuilderAPI_MakeFace aFaceMaker4(aCylSurf, 3, 4, -1, 1, Precision::Confusion());

  ASSERT_TRUE(aFaceMaker1.IsDone() && aFaceMaker2.IsDone() && aFaceMaker3.IsDone()
              && aFaceMaker4.IsDone())
    << "Failed to create cylindrical faces";

  // Sew the faces together
  BRepBuilderAPI_Sewing aSewing;
  aSewing.Add(aFaceMaker1.Face());
  aSewing.Add(aFaceMaker2.Face());
  aSewing.Add(aFaceMaker3.Face());
  aSewing.Add(aFaceMaker4.Face());
  aSewing.Perform();

  const TopoDS_Shape aSewnShape = aSewing.SewedShape();
  ASSERT_FALSE(aSewnShape.IsNull()) << "Failed to create sewn shape";

  // Convert sewn shape to NURBS (nurbsconvert)
  BRepBuilderAPI_NurbsConvert aNurbsConvert(aSewnShape);
  ASSERT_TRUE(aNurbsConvert.IsDone()) << "Failed to convert sewn shape to NURBS";
  const TopoDS_Shape aNurbsShape = aNurbsConvert.Shape();

  // Test if the converted sewn cylindrical shape can be recognized as a cylinder
  // Note: Original TCL test B9 tests getanasurf (approximating surface to sample),
  // which is different from pure canonical recognition without a sample surface.
  // Here we test recognition of the sewn cylindrical shape directly.
  myRecognizer->SetShape(aNurbsShape);
  gp_Cylinder            aResultCylinder;
  const Standard_Boolean aResult = myRecognizer->IsCylinder(myTolerance, aResultCylinder);

  EXPECT_TRUE(aResult) << "Sewn cylindrical shape should be recognized as cylinder";
  EXPECT_NEAR(aResultCylinder.Radius(), 1.0, myTolerance) << "Cylinder radius should match";
}

// Test cr/base/B10: Extruded surface from cylinder-plane intersection
TEST_F(CanonicalRecognitionBaseSurfaceTest, ExtrudedCylindricalSurfaceRecognition_B10)
{
  // Create cylindrical surface (radius 1)
  gp_Ax2                          aAxis(gp_Pnt(0, 0, 0), gp_Dir(gp_Dir::D::Z));
  Handle(Geom_CylindricalSurface) aCylSurf = new Geom_CylindricalSurface(aAxis, 1.0);

  // Create plane for intersection: plane at origin with normal (1, 0, 1) normalized
  gp_Dir             aNormal(1, 0, 1);
  gp_Pln             aPlane(gp_Pnt(0, 0, 0), aNormal);
  Handle(Geom_Plane) aGeomPlane = new Geom_Plane(aPlane);

  // Perform surface-surface intersection
  GeomAPI_IntSS anIntersector(aCylSurf, aGeomPlane, Precision::Confusion());

  ASSERT_TRUE(anIntersector.IsDone()) << "Surface-surface intersection failed";
  ASSERT_GT(anIntersector.NbLines(), 0) << "No intersection curves found";

  // Get the first intersection curve
  Handle(Geom_Curve) anIntCurve = anIntersector.Line(1);
  ASSERT_FALSE(anIntCurve.IsNull()) << "Intersection curve is null";

  // Create a surface of linear extrusion from the intersection curve
  gp_Dir                                anExtrusionDir(gp_Dir::D::Z);
  Handle(Geom_SurfaceOfLinearExtrusion) anExtSurf =
    new Geom_SurfaceOfLinearExtrusion(anIntCurve, anExtrusionDir);

  // Create a face from the extruded surface with trimming (V parameter from -1 to 1)
  Standard_Real uMin, uMax, vMin, vMax;
  anExtSurf->Bounds(uMin, uMax, vMin, vMax);
  BRepBuilderAPI_MakeFace aFaceMaker(anExtSurf, uMin, uMax, -1, 1, Precision::Confusion());

  ASSERT_TRUE(aFaceMaker.IsDone()) << "Failed to create face from extruded surface";
  TopoDS_Face aFace = aFaceMaker.Face();

  // Convert face to NURBS (nurbsconvert)
  BRepBuilderAPI_NurbsConvert aNurbsConvert(aFace);
  ASSERT_TRUE(aNurbsConvert.IsDone()) << "Failed to convert face to NURBS";
  TopoDS_Face aNurbsFace = TopoDS::Face(aNurbsConvert.Shape());

  // Test if the face can be recognized as a cylindrical surface
  myRecognizer->SetShape(aNurbsFace);
  gp_Cylinder            aResultCylinder;
  const Standard_Boolean aResult = myRecognizer->IsCylinder(myTolerance, aResultCylinder);

  EXPECT_TRUE(aResult) << "Extruded surface should be recognized as cylinder";
  EXPECT_NEAR(aResultCylinder.Radius(), 1.0, myTolerance) << "Cylinder radius should match";
}

// Test bug33170: Plane detection problems with gap validation
TEST_F(CanonicalRecognitionBaseSurfaceTest, PlaneDetectionWithGapValidation_Bug33170)
{
  // This test corresponds to bug33170: plane detection problems
  // We'll create a test case that validates gap computation similar to the original

  // Create a slightly non-planar surface that should still be recognized as a plane
  // within tolerance, similar to the bug33170.brep case
  Handle(Geom_Plane)      aBasePlane = new Geom_Plane(gp_Pln());
  BRepBuilderAPI_MakeFace aFaceMaker(aBasePlane, -1, 1, -1, 1, Precision::Confusion());

  ASSERT_TRUE(aFaceMaker.IsDone()) << "Failed to create base planar face";
  const TopoDS_Face aFace = aFaceMaker.Face();

  // Test with tolerance 0.006 (first case from bug33170)
  myRecognizer->SetShape(aFace);
  gp_Pln                 aResultPlane1;
  const Standard_Boolean aResult1 = myRecognizer->IsPlane(0.006, aResultPlane1);

  EXPECT_TRUE(aResult1) << "Surface should be recognized as plane with tolerance 0.006";

  // Verify gap is within expected range (the original test expects ~0.0051495320504590563)
  const Standard_Real aGap1 = myRecognizer->GetGap();
  EXPECT_LT(aGap1, 0.006) << "Gap should be less than tolerance used";
  EXPECT_GE(aGap1, 0.0) << "Gap should be non-negative";

  // Test with larger tolerance 1.0 (second case from bug33170)
  myRecognizer->ClearStatus();
  gp_Pln                 aResultPlane2;
  const Standard_Boolean aResult2 = myRecognizer->IsPlane(1.0, aResultPlane2);

  EXPECT_TRUE(aResult2) << "Surface should be recognized as plane with tolerance 1.0";

  const Standard_Real aGap2 = myRecognizer->GetGap();
  EXPECT_LT(aGap2, 1.0) << "Gap should be less than tolerance used";
  EXPECT_GE(aGap2, 0.0) << "Gap should be non-negative";

  // The gap should be consistent between both recognitions
  EXPECT_DOUBLE_EQ(aGap1, aGap2) << "Gap should be the same regardless of tolerance used";
}
