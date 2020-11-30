// Copyright (c) 2020 OPEN CASCADE SAS
//
// This file is part of the examples of the Open CASCADE Technology software library.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE

#include "TopologySamples.h"

#include "AdaptorVec_AIS.h"

#include <gp_Circ.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>

#include <Geom_Axis1Placement.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <Geom2dAPI_PointsToBSpline.hxx>
#include <GeomAPI_PointsToBSplineSurface.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <TColgp_Array2OfPnt.hxx>

#include <BRep_Builder.hxx>
#include <BRepGProp.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepTools_ReShape.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_CompCurve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRepAlgoAPI_Splitter.hxx>
#include <BRepAlgoAPI_Defeaturing.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeEdge2d.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeShell.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_NurbsConvert.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRepFilletAPI_MakeChamfer.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <BRepOffsetAPI_MakeOffset.hxx>
#include <BRepOffsetAPI_MakeEvolved.hxx>
#include <Extrema_ExtCS.hxx>
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <GProp_GProps.hxx>
#include <GProp_PrincipalProps.hxx>

#include <AIS_Axis.hxx>
#include <AIS_ColoredShape.hxx>
#include <AIS_Plane.hxx>
#include <AIS_Point.hxx>
#include <AIS_TextLabel.hxx>

void TopologySamples::ExecuteSample (const TCollection_AsciiString& theSampleName)
{
  Standard_Boolean anIsSamplePresent = Standard_True;
  FindSourceCode(theSampleName);
  if (theSampleName == "Vertex3dSample")
    Vertex3dSample();
  else if (theSampleName == "Edge3dSample")
    Edge3dSample();
  else if (theSampleName == "Face3dSample")
    Face3dSample();
  else if (theSampleName == "Wire3dSample")
    Wire3dSample();
  else if (theSampleName == "Shell3dSample")
    Shell3dSample();
  else if (theSampleName == "Solid3dSample")
    Solid3dSample();
  else if (theSampleName == "Edge2dSample")
    Edge2dSample();
  else if (theSampleName == "Box3dSample")
    Box3dSample();
  else if (theSampleName == "Cylinder3dSample")
    Cylinder3dSample();
  else if (theSampleName == "Revolution3dSample")
    Revolution3dSample();
  else if (theSampleName == "TopologyIterator3dSample")
    TopologyIterator3dSample();
  else if (theSampleName == "TopologyExplorer3dSample")
    TopologyExplorer3dSample();
  else if (theSampleName == "AssessToCurve3dSample")
    AssessToCurve3dSample();
  else if (theSampleName == "AssessToCompositeCurve3dSample")
    AssessToCompositeCurve3dSample();
  else if (theSampleName == "AssessToSurface3dSample")
    AssessToSurface3dSample();
  else if (theSampleName == "Common3dSample")
    Common3dSample();
  else if (theSampleName == "Cut3dSample")
    Cut3dSample();
  else if (theSampleName == "Cut3dSample")
    Cut3dSample();
  else if (theSampleName == "Fuse3dSample")
    Fuse3dSample();
  else if (theSampleName == "Section3dSample")
    Section3dSample();
  else if (theSampleName == "Splitter3dSample")
    Splitter3dSample();
  else if (theSampleName == "Defeaturing3dSample")
    Defeaturing3dSample();
  else if (theSampleName == "Fillet3dSample")
    Fillet3dSample();
  else if (theSampleName == "Chamfer3dSample")
    Chamfer3dSample();
  else if (theSampleName == "Offset3dSample")
    Offset3dSample();
  else if (theSampleName == "Evolved3dSample")
    Evolved3dSample();
  else if (theSampleName == "Copy3dSample")
    Copy3dSample();
  else if (theSampleName == "Transform3dSample")
    Transform3dSample();
  else if (theSampleName == "ConvertToNurbs3dSample")
    ConvertToNurbs3dSample();
  else if (theSampleName == "SewContiguousFaces3dSample")
    SewContiguousFaces3dSample();
  else if (theSampleName == "CheckValidity3dSample")
    CheckValidity3dSample();
  else if (theSampleName == "ComputeLinearProperties3dSample")
    ComputeLinearProperties3dSample();
  else if (theSampleName == "ComputeSurfaceProperties3dSample")
    ComputeSurfaceProperties3dSample();
  else if (theSampleName == "ComputeVolumeProperties3dSample")
    ComputeVolumeProperties3dSample();
  else
  {
    myResult << "No function found: " << theSampleName;
    myCode += TCollection_AsciiString("No function found: ") + theSampleName;
    anIsSamplePresent = Standard_False;
  }
  myIsProcessed = anIsSamplePresent;
}

void TopologySamples::Vertex3dSample()
{
  // Make a vertex from a 3D point.
  gp_Pnt aPnt(0.0, 0.0, 10.0);
  TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(aPnt);
  myResult << "TopoDS_Vertex was created at [ "
           << aPnt.X() << ", " << aPnt.Y() << ", " << aPnt.Z()
           << " ]" << std::endl;

  Handle(AIS_Shape) aAisVertex = new AIS_Shape(aVertex);
  Handle(AIS_TextLabel) anAisLabel = new AIS_TextLabel();
  Standard_SStream aSS;
  aSS << "TopoDS_Vertex [" << aPnt.X() << ", " << aPnt.Y() << ", " << aPnt.Z() << "]" << std::endl;
  anAisLabel->SetText(aSS.str().c_str());
  anAisLabel->SetPosition(aPnt);
  myObject3d.Append(aAisVertex);
  myObject3d.Append(anAisLabel);
}

void TopologySamples::Edge3dSample()
{
  // Make an edge from two 3D points.
  gp_Pnt aPnt1(0.0, 10.0, 0.0);
  gp_Pnt aPnt2(10.0, 10.0, 0.0);
  TopoDS_Edge anEdgeP12 = BRepBuilderAPI_MakeEdge(aPnt1, aPnt2);
  myResult << "TopoDS_Edge between [ "
           << aPnt1.X() << ", " << aPnt1.Y() << ", " << aPnt1.Z()
           << " ] and [ "
           << aPnt2.X() << ", " << aPnt2.Y() << ", " << aPnt2.Z()
           << " ] was created in yellow" << std::endl
           << std::endl;

  // Make an edge from a circular segment.
  // Create a circle in XY plane of the radius 5.0.
  gp_Circ aCirc(gp::XOY(), 5.0);
  // Make a circular edge from the 1st quoter in the parametric space.
  TopoDS_Edge anEdgeCirc = BRepBuilderAPI_MakeEdge(aCirc, 0.0, M_PI_2);
  myResult << "TopoDS_Edge on the circle's 1st quoter" << std::endl
           << "with the center at [ "
           << aCirc.Location().X() << ", " << aCirc.Location().Y() << ", " << aCirc.Location().Z()
           << " ] and R = " << aCirc.Radius() << " was created in red" << std::endl
           << std::endl;

  // Make an edge from a 3D curve (BSpline).
  // Define points.
  gp_Pnt aPole1(0.0, 0.0, 10.0);
  gp_Pnt aPole2(5.0, 5.0, 5.0);
  gp_Pnt aPole3(10.0, 10.0, 15.0);
  gp_Pnt aPole4(15.0, 5.0, 20.0);
  // Add points to the curve poles array.
  TColgp_Array1OfPnt aPoles(1, 4);
  aPoles.SetValue(1, aPole1);
  aPoles.SetValue(2, aPole2);
  aPoles.SetValue(3, aPole3);
  aPoles.SetValue(4, aPole4);
  // Make a BSpline curve from the points array
  Handle(Geom_BSplineCurve) aBSplineCurve = GeomAPI_PointsToBSpline(aPoles).Curve();
  // Make an edge between two point on the BSpline curve.
  gp_Pnt aPntOnCurve1, aPntOnCurve2;
  aBSplineCurve->D0 (0.75 * aBSplineCurve->FirstParameter()
                   + 0.25 * aBSplineCurve->LastParameter(),
                     aPntOnCurve1);
  aBSplineCurve->D0 (0.25 * aBSplineCurve->FirstParameter()
                   + 0.75 * aBSplineCurve->LastParameter(),
                     aPntOnCurve2);
  TopoDS_Edge anEdgeBSpline = BRepBuilderAPI_MakeEdge(aBSplineCurve, aPntOnCurve1, aPntOnCurve2);
  myResult << "TopoDS_Edge on the BSpline curve" << std::endl
           << "between [ "
           << aPntOnCurve1.X() << ", " << aPntOnCurve1.Y() << ", " << aPntOnCurve1.Z()
           << " ] and [ "
           << aPntOnCurve2.X() << ", " << aPntOnCurve2.Y() << ", " << aPntOnCurve2.Z()
           << " ]" << std::endl
           << "was created in green" << std::endl;

  Handle(AIS_ColoredShape) anAisEdgeP12 = new AIS_ColoredShape(anEdgeP12);
  Handle(AIS_ColoredShape) anAisEdgeCirc = new AIS_ColoredShape(anEdgeCirc);
  Handle(AIS_ColoredShape) anAisEdgeBSpline = new AIS_ColoredShape(anEdgeBSpline);
  anAisEdgeP12->SetColor(Quantity_Color(Quantity_NOC_YELLOW));
  anAisEdgeCirc->SetColor(Quantity_Color(Quantity_NOC_RED));
  anAisEdgeBSpline->SetColor(Quantity_Color(Quantity_NOC_GREEN));
  myObject3d.Append(anAisEdgeP12);
  myObject3d.Append(anAisEdgeCirc);
  myObject3d.Append(anAisEdgeBSpline);
  Handle(AIS_TextLabel) anAisEdgeP12Label = new AIS_TextLabel();
  anAisEdgeP12Label->SetText("Edge between two points");
  anAisEdgeP12Label->SetPosition(0.5 * (aPnt1.XYZ() + aPnt2.XYZ()));
  anAisEdgeP12Label->SetColor(Quantity_Color(Quantity_NOC_YELLOW));
  myObject3d.Append(anAisEdgeP12Label);
  Handle(AIS_TextLabel) anAisEdgeCircLabel = new AIS_TextLabel();
  anAisEdgeCircLabel->SetText("Circular edge");
  anAisEdgeCircLabel->SetPosition(aCirc.Location());
  anAisEdgeCircLabel->SetColor(Quantity_Color(Quantity_NOC_RED));
  myObject3d.Append(anAisEdgeCircLabel);
  Handle(AIS_TextLabel) anAisEdgeBSplineLabel = new AIS_TextLabel();
  anAisEdgeBSplineLabel->SetText("BSpline edge");
  anAisEdgeBSplineLabel->SetPosition(aPole3);
  anAisEdgeBSplineLabel->SetColor(Quantity_Color(Quantity_NOC_GREEN));
  myObject3d.Append(anAisEdgeBSplineLabel);
  TopoDS_Vertex anEdgeP12_V1, anEdgeP12_V2;
  TopExp::Vertices(anEdgeP12, anEdgeP12_V1, anEdgeP12_V2);
  myObject3d.Append(new AIS_Shape(anEdgeP12_V1));
  myObject3d.Append(new AIS_Shape(anEdgeP12_V2));
  TopoDS_Vertex anEdgeCirc_V1, anEdgeCirc_V2;
  TopExp::Vertices(anEdgeCirc, anEdgeCirc_V1, anEdgeCirc_V2);
  myObject3d.Append(new AIS_Shape(anEdgeCirc_V1));
  myObject3d.Append(new AIS_Shape(anEdgeCirc_V2));
  TopoDS_Vertex anEdgeBSpline_V1, anEdgeBSpline_V2;
  TopExp::Vertices(anEdgeBSpline, anEdgeBSpline_V1, anEdgeBSpline_V2);
  myObject3d.Append(new AIS_Shape(anEdgeBSpline_V1));
  myObject3d.Append(new AIS_Shape(anEdgeBSpline_V2));
}

void TopologySamples::Face3dSample()
{
  // Make a face from a sphere with the center
  // at [0.0, 0.0, 10.0] and R = 5.
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0.0, 0.0, 10.0), gp::DZ()), 5.0);
  TopoDS_Face aFaceSphere = BRepBuilderAPI_MakeFace(aSphere);
  myResult << "TopoDS_Face on the sphere with" << std::endl
           << "the center at [ "
           << aSphere.Location().X() << ", " << aSphere.Location().Y() << ", " << aSphere.Location().Z()
           << " ] and R = " << aSphere.Radius() << " was created in yellow" << std::endl
           << std::endl;

  // Make a flat rectangular face on XY plane.
  gp_Pln aPln(gp::XOY());
  TopoDS_Face aFaceRect = BRepBuilderAPI_MakeFace(aPln, -10.0, +10.0, -20.0, +20.0);
  myResult << "TopoDS_Face on the rectangle was created in red" << std::endl
           << std::endl;

  // Make a face from a BSpline surface.
  // Define a 4x4 grid of points for BSpline surface.
  TColgp_Array2OfPnt aPoints(1, 4, 1, 4);
  for (Standard_Integer i = 1; i <= 4; ++i)
  {
    gp_Pnt aPnt;
    aPnt.SetX(5.0 * i);
    for (Standard_Integer j = 1; j <= 4; ++j)
    {
      aPnt.SetY(5.0 * j);
      if (1 < i && i < 4 && 1 < j && j < 4)
      {
        aPnt.SetZ(15.0);
      }
      else
      {
        aPnt.SetZ(10.0);
      }
      aPoints.SetValue(i, j, aPnt);
    }
  }
  // Make a BSpline surface from the points array.
  Handle(Geom_BSplineSurface) aBSplineSurf = GeomAPI_PointsToBSplineSurface(aPoints).Surface();
  Standard_Real aU1, aU2, aV1, aV2;
  aBSplineSurf->Bounds(aU1, aU2, aV1, aV2);
  TopoDS_Face aFaceBSpline = BRepBuilderAPI_MakeFace(aBSplineSurf, aU1, aU2, aV1, aV2, Precision::Confusion());
  myResult << "TopoDS_Face on the BSpline surface was created in green" << std::endl << std::endl;

  Handle(AIS_ColoredShape) anAisFaceSphere = new AIS_ColoredShape(aFaceSphere);
  Handle(AIS_ColoredShape) anAisFaceRect = new AIS_ColoredShape(aFaceRect);
  Handle(AIS_ColoredShape) anAisFaceBSpline = new AIS_ColoredShape(aFaceBSpline);
  anAisFaceSphere->SetColor(Quantity_Color(Quantity_NOC_YELLOW));
  anAisFaceRect->SetColor(Quantity_Color(Quantity_NOC_RED));
  anAisFaceBSpline->SetColor(Quantity_Color(Quantity_NOC_GREEN));
  myObject3d.Append(anAisFaceSphere);
  myObject3d.Append(anAisFaceRect);
  myObject3d.Append(anAisFaceBSpline);
  Handle(AIS_TextLabel) anAisFaceSphereLabel = new AIS_TextLabel();
  anAisFaceSphereLabel->SetText("Spherical face");
  anAisFaceSphereLabel->SetPosition(aSphere.Location().XYZ() + aSphere.Radius() * gp::DZ().XYZ());
  anAisFaceSphereLabel->SetColor(Quantity_Color(Quantity_NOC_YELLOW));
  myObject3d.Append(anAisFaceSphereLabel);
  Handle(AIS_TextLabel) anAisFaceRectLabel = new AIS_TextLabel();
  anAisFaceRectLabel->SetText("Flat rectangular face");
  anAisFaceRectLabel->SetPosition(aPln.Location().XYZ() + 2.5 * gp::DZ().XYZ());
  anAisFaceRectLabel->SetColor(Quantity_Color(Quantity_NOC_RED));
  myObject3d.Append(anAisFaceRectLabel);
  Handle(AIS_TextLabel) anAisFaceBSplineLabel = new AIS_TextLabel();
  anAisFaceBSplineLabel->SetText("BSpline face");
  anAisFaceBSplineLabel->SetPosition(aPoints(4, 4));
  anAisFaceBSplineLabel->SetColor(Quantity_Color(Quantity_NOC_GREEN));
  myObject3d.Append(anAisFaceBSplineLabel);
}

void TopologySamples::Wire3dSample()
{
  // Make a wire from edges created on a set of points.
  // Add points to the curve poles array.
  TColgp_Array1OfPnt aPoints(1, 4);
  aPoints.SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  aPoints.SetValue(2, gp_Pnt(20.0, 0.0, 0.0));
  aPoints.SetValue(3, gp_Pnt(20.0, 10.0, 0.0));
  aPoints.SetValue(4, gp_Pnt(0.0, 10.0, 0.0));
  // A wire maker contains an empty wire.
  BRepBuilderAPI_MakeWire aMakeWire;
  for (Standard_Integer i = 1; i <= 4; ++i)
  {
    Standard_Integer i1 = i;
    Standard_Integer i2 = i1 < 4 ? i1 + 1 : 1;
    const gp_Pnt& aPnt1 = aPoints.Value(i1);
    const gp_Pnt& aPnt2 = aPoints.Value(i2);
    TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(aPnt1, aPnt2);
    // Add an edge to the wire under construction.
    // The edge must be connectible to the wire under construction, and,
    // unless it is the first edge of the wire, must satisfy the following
    // condition: one of its vertices must be geometrically coincident
    // with one of the vertices of the wire (provided that the highest
    // tolerance factor is assigned to the two vertices).
    // It could also be the same vertex.
    // Warning
    // If the edge is not connectible to the wire under construction it is not added.
    // The function IsDone will return false and the function
    // Wire will raise an error, until a new connectible edge is added.
    aMakeWire.Add(anEdge);
    Standard_ASSERT_VOID(aMakeWire.IsDone(), "Added edge isn't connectible!");
  }
  // Retrieve a constructed wire.
  TopoDS_Wire aWire = aMakeWire.Wire();
  myResult << "TopoDS_Wire was created. Vertices :" << std::endl;
  // Retrieve wire vertices. 4 vertices are expected, because of
  // edges connecting during wire constructing.
  TopTools_IndexedMapOfShape aVertices;
  TopExp::MapShapes(aWire, TopAbs_VERTEX, aVertices);
  for (TopTools_IndexedMapOfShape::Iterator anIt(aVertices); anIt.More(); anIt.Next())
  {
    TopoDS_Vertex aVertex = TopoDS::Vertex(anIt.Value());
    gp_Pnt aPnt = BRep_Tool::Pnt(aVertex);
    myResult << "[ " << aPnt.X() << ", " << aPnt.Y() << ", " << aPnt.Z() << " ]" << std::endl;
    Handle(AIS_Shape) anAisVertex = new AIS_Shape(aVertex);
    myObject3d.Append(anAisVertex);
  }

  Handle(AIS_Shape) anAisWire = new AIS_Shape(aWire);
  myObject3d.Append(anAisWire);
}

void TopologySamples::Shell3dSample()
{
  // Make a shell from a cylinder with R = 5 and directed along Z axis
  gp_Cylinder aCyl(gp::XOY(), 5.0);
  Handle(Geom_Surface) aCylSurf = new Geom_CylindricalSurface(aCyl);
  TopoDS_Shell aCylShell = BRepBuilderAPI_MakeShell(aCylSurf, 0.0, 2.0 * M_PI, -10.0, +10.0);
  myResult << "TopoDS_Shell on the cylinder R = " << aCyl.Radius() << std::endl
           << "with axis [ "
           << aCyl.Position().Direction().X() << ", "
           << aCyl.Position().Direction().Y() << ", "
           << aCyl.Position().Direction().Z() << " ]" << std::endl
           << "limited in length [-10 ... +10] was created" << std::endl;

  Handle(AIS_Shape) anAisShell = new AIS_Shape(aCylShell);
  myObject3d.Append(anAisShell);
}

void TopologySamples::Solid3dSample()
{
  // Make a torus from a shell.
  gp_Torus aTorus(gp::XOY(), 20.0, 7.5);
  Handle(Geom_Surface) aTorusSurf = new Geom_ToroidalSurface(aTorus);
  TopoDS_Shell aTorusShell = BRepBuilderAPI_MakeShell(aTorusSurf, 0.0, 2.0 * M_PI, 0.0, 2.0 * M_PI);
  // Make a solid on the torus shell.
  TopoDS_Solid aTorusSolid = BRepBuilderAPI_MakeSolid(aTorusShell);
  myResult << "TopoDS_Solid on the torus with" << std::endl
           << "R major = " << aTorus.MajorRadius() << std::endl
           << "R minor = " << aTorus.MinorRadius() << std::endl
           << "was created" << std::endl;

  Handle(AIS_Shape) anAisSolid = new AIS_Shape(aTorusSolid);
  myObject3d.Append(anAisSolid);
}

void TopologySamples::Edge2dSample()
{
  // Make an edge from two 2D points.
  gp_Pnt2d aPnt1(0.0, 10.0);
  gp_Pnt2d aPnt2(10.0, 10.0);
  TopoDS_Edge anEdgeP12 = BRepBuilderAPI_MakeEdge2d(aPnt1, aPnt2);
  myResult << "TopoDS_Edge between [ "
           << aPnt1.X() << ", " << aPnt1.Y() << " ] and [ "
           << aPnt2.X() << ", " << aPnt2.Y() << " ] was created in yellow" << std::endl
           << std::endl;

  // Make an edge from a circular segment.
  // Create a circle of the radius 5.0.
  gp_Circ2d aCirc(gp::OX2d(), 5.0);
  // Make a circular edge from the 1st quoter in the parametric space.
  TopoDS_Edge anEdgeCirc = BRepBuilderAPI_MakeEdge2d(aCirc, 0.0, M_PI_2);
  myResult << "TopoDS_Edge on the 2D circle's 1st quoter" << std::endl
           << "with the center at [ " << aCirc.Location().X() << ", " << aCirc.Location().Y()
           << " ] and R = " << aCirc.Radius() << " was created in red" << std::endl
           << std::endl;

  // Make an edge from a 2D curve (BSpline).
  // Define points.
  gp_Pnt2d aPole1(0.0, 0.0);
  gp_Pnt2d aPole2(5.0, 5.0);
  gp_Pnt2d aPole3(10.0, 10.0);
  gp_Pnt2d aPole4(15.0, 5.0);
  // Add points to the curve poles array.
  TColgp_Array1OfPnt2d aPoles(1, 4);
  aPoles.SetValue(1, aPole1);
  aPoles.SetValue(2, aPole2);
  aPoles.SetValue(3, aPole3);
  aPoles.SetValue(4, aPole4);
  // Make a BSpline curve from the points array
  Handle(Geom2d_BSplineCurve) aBSplineCurve = Geom2dAPI_PointsToBSpline(aPoles).Curve();
  // Make an edge between two point on the BSpline curve.
  gp_Pnt2d aPntOnCurve1, aPntOnCurve2;
  aBSplineCurve->D0 (0.75 * aBSplineCurve->FirstParameter()
                   + 0.25 * aBSplineCurve->LastParameter(),
                     aPntOnCurve1);
  aBSplineCurve->D0 (0.25 * aBSplineCurve->FirstParameter()
                   + 0.75 * aBSplineCurve->LastParameter(),
                     aPntOnCurve2);
  TopoDS_Edge anEdgeBSpline = BRepBuilderAPI_MakeEdge2d(aBSplineCurve, aPntOnCurve1, aPntOnCurve2);
  myResult << "TopoDS_Edge on the 2D BSpline curve" << std::endl
           << "between [ " << aPntOnCurve1.X() << ", " << aPntOnCurve1.Y()
           << " ] and [ " << aPntOnCurve2.X() << ", " << aPntOnCurve2.Y() << " ]" << std::endl
           << "was created in green" << std::endl;

  Handle(AIS_ColoredShape) anAisEdgeP12 = new AIS_ColoredShape(anEdgeP12);
  Handle(AIS_ColoredShape) anAisEdgeCirc = new AIS_ColoredShape(anEdgeCirc);
  Handle(AIS_ColoredShape) anAisEdgeBSpline = new AIS_ColoredShape(anEdgeBSpline);
  anAisEdgeP12->SetColor(Quantity_Color(Quantity_NOC_YELLOW));
  anAisEdgeCirc->SetColor(Quantity_Color(Quantity_NOC_RED));
  anAisEdgeBSpline->SetColor(Quantity_Color(Quantity_NOC_GREEN));
  myObject2d.Append(anAisEdgeP12);
  myObject2d.Append(anAisEdgeCirc);
  myObject2d.Append(anAisEdgeBSpline);
  Handle(AIS_TextLabel) anAisEdgeP12Label = new AIS_TextLabel();
  anAisEdgeP12Label->SetText("Edge between two 2d points");
  anAisEdgeP12Label->SetPosition(0.5 * (gp_XYZ(aPnt1.X(), aPnt1.Y() + 0.5, 0.0) + gp_XYZ(aPnt2.X(), aPnt2.Y() + 0.5, 0.0)));
  anAisEdgeP12Label->SetColor(Quantity_Color(Quantity_NOC_YELLOW));
  myObject2d.Append(anAisEdgeP12Label);
  Handle(AIS_TextLabel) anAisEdgeCircLabel = new AIS_TextLabel();
  anAisEdgeCircLabel->SetText("Circular edge");
  anAisEdgeCircLabel->SetPosition(gp_XYZ(aCirc.Location().X(), aCirc.Location().Y() + 0.5, 0.0));
  anAisEdgeCircLabel->SetColor(Quantity_Color(Quantity_NOC_RED));
  myObject2d.Append(anAisEdgeCircLabel);
  Handle(AIS_TextLabel) anAisEdgeBSplineLabel = new AIS_TextLabel();
  anAisEdgeBSplineLabel->SetText("BSpline edge");
  anAisEdgeBSplineLabel->SetPosition(gp_XYZ(aPole3.X(), aPole3.Y() + 0.5, 0.0));
  anAisEdgeBSplineLabel->SetColor(Quantity_Color(Quantity_NOC_GREEN));
  myObject2d.Append(anAisEdgeBSplineLabel);
  TopoDS_Vertex anEdgeP12_V1, anEdgeP12_V2;
  TopExp::Vertices(anEdgeP12, anEdgeP12_V1, anEdgeP12_V2);
  myObject2d.Append(new AIS_Shape(anEdgeP12_V1));
  myObject2d.Append(new AIS_Shape(anEdgeP12_V2));
  TopoDS_Vertex anEdgeCirc_V1, anEdgeCirc_V2;
  TopExp::Vertices(anEdgeCirc, anEdgeCirc_V1, anEdgeCirc_V2);
  myObject2d.Append(new AIS_Shape(anEdgeCirc_V1));
  myObject2d.Append(new AIS_Shape(anEdgeCirc_V2));
  TopoDS_Vertex anEdgeBSpline_V1, anEdgeBSpline_V2;
  TopExp::Vertices(anEdgeBSpline, anEdgeBSpline_V1, anEdgeBSpline_V2);
  myObject2d.Append(new AIS_Shape(anEdgeBSpline_V1));
  myObject2d.Append(new AIS_Shape(anEdgeBSpline_V2));
}

void TopologySamples::Box3dSample()
{
  // Make a box with a corner at [0, 0, 0] and the specified sizes.
  Standard_Real aSizeX = 5.0;
  Standard_Real aSizeY = 10.0;
  Standard_Real aSizeZ = 15.0;
  TopoDS_Shape aBox1 = BRepPrimAPI_MakeBox(aSizeX, aSizeY, aSizeZ);
  myResult << "Box at corner [0, 0, 0] and sizes ["
           << aSizeX << ", " << aSizeY << ", " << aSizeZ
           << "] was created in yellow" << std::endl;

  // Make a box by two points.
  gp_Pnt aPnt1(10.0, 0.0, 0.0);
  gp_Pnt aPnt2(20.0, 10.0, 15.0);
  TopoDS_Shape aBox2 = BRepPrimAPI_MakeBox(aPnt1, aPnt2);
  myResult << "Box with corners ["
           << aPnt1.X() << ", " << aPnt1.Y() << ", " << aPnt1.Z()
           << "] and ["
           << aPnt2.X() << ", " << aPnt2.Y() << ", " << aPnt2.Z()
           << "] was created in red" << std::endl;

  Handle(AIS_ColoredShape) anAisBox1 = new AIS_ColoredShape(aBox1);
  Handle(AIS_ColoredShape) anAisBox2 = new AIS_ColoredShape(aBox2);
  anAisBox1->SetColor(Quantity_Color(Quantity_NOC_YELLOW));
  anAisBox2->SetColor(Quantity_Color(Quantity_NOC_RED));
  myObject3d.Append(anAisBox1);
  myObject3d.Append(anAisBox2);
}

void TopologySamples::Cylinder3dSample()
{
  // Make a cylinder of the specified radius and length.
  Standard_Real aRadius1 = 5.0;
  Standard_Real aLength1 = 15.0;
  TopoDS_Shape aCyl1 = BRepPrimAPI_MakeCylinder(aRadius1, aLength1);
  myResult << "Cylinder with Radius = " << aRadius1
           << " and Length = " << aLength1
           << " was created in yellow" << std::endl;

  // Make a cylinder of the specified radius, length and sector angle.
  Standard_Real aRadius2 = 8.0;
  Standard_Real aLength2 = 25.0;
  Standard_Real anAngle = M_PI_2;
  TopoDS_Shape aCyl2 = BRepPrimAPI_MakeCylinder(aRadius2, aLength2, anAngle);
  myResult << "Cylinder with Radius = " << aRadius2
           << " , Length = " << aLength2
           << " and Angle = " << anAngle
           << " was created in red" << std::endl;

  Handle(AIS_ColoredShape) anAisCyl1 = new AIS_ColoredShape(aCyl1);
  Handle(AIS_ColoredShape) anAisCyl2 = new AIS_ColoredShape(aCyl2);
  anAisCyl1->SetColor(Quantity_Color(Quantity_NOC_YELLOW));
  anAisCyl2->SetColor(Quantity_Color(Quantity_NOC_RED));
  myObject3d.Append(anAisCyl1);
  myObject3d.Append(anAisCyl2);
}

void TopologySamples::Revolution3dSample()
{
  // Make a toroidal face by a series of shape revolves.
  // Make a starting vertex at [-1.0, 0, 0].
  TopoDS_Shape aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(-1.0, 0.0, 0.0));

  // Make a circular edge by revolting aVertex around
  // an axis Y positioned at [-1.5, 0.0, 0.0] on 2*Pi angle
  gp_Ax1 anAxis1(gp_Pnt(-1.5, 0.0, 0.0), gp::DY());
  TopoDS_Shape anEdge = BRepPrimAPI_MakeRevol(aVertex, anAxis1);
  myResult << "Circular edge was created in yellow" << std::endl;

  // Make a toroidal face by revolting anEdge around
  // Z axis on 2*Pi angle.
  TopoDS_Shape aFace = BRepPrimAPI_MakeRevol(anEdge, gp::OZ());
  myResult << "Toroidal face was created in red" << std::endl;

  Handle(AIS_Axis) anAisAxis1 = new AIS_Axis(new Geom_Axis1Placement(anAxis1));
  Handle(AIS_Axis) anAisAxis2 = new AIS_Axis(new Geom_Axis1Placement(gp::OZ()));
  Handle(AIS_Shape) anAisVertex = new AIS_Shape(aVertex);
  Handle(AIS_ColoredShape) anAisEdge = new AIS_ColoredShape(anEdge);
  Handle(AIS_ColoredShape) anAisFace = new AIS_ColoredShape(aFace);
  anAisEdge->SetColor(Quantity_Color(Quantity_NOC_GREEN));
  anAisEdge->SetWidth(1.5);
  anAisAxis1->SetColor(Quantity_NOC_GREEN);
  anAisFace->SetColor(Quantity_Color(Quantity_NOC_RED));
  anAisAxis2->SetColor(Quantity_NOC_RED);
  myObject3d.Append(anAisVertex);
  myObject3d.Append(anAisEdge);
  myObject3d.Append(anAisFace);
}

void TopologySamples::TopologyIterator3dSample()
{
  // Make a compound shape.
  TopoDS_Compound aComp;
  BRep_Builder aBuilder;
  aBuilder.MakeCompound(aComp);
  // Add shapes to the compound.
  aBuilder.Add(aComp, BRepBuilderAPI_MakeVertex(gp::Origin()));
  aBuilder.Add(aComp, BRepBuilderAPI_MakeEdge(gp_Pnt(5.0, 0.0, 0.0), gp_Pnt(10.0, 0.0, 0.0)));
  aBuilder.Add(aComp, BRepBuilderAPI_MakeFace(gp_Sphere(gp::XOY(), 10.0)));
  aBuilder.Add(aComp, BRepBuilderAPI_MakeWire(
    BRepBuilderAPI_MakeEdge(gp_Pnt(15.0, 0.0, 0.0), gp_Pnt(20.0, 0.0, 0.0)),
    BRepBuilderAPI_MakeEdge(gp_Pnt(20.0, 0.0, 0.0), gp_Pnt(25.0, 10.0, 5.0))
  ));
  aBuilder.Add(aComp, BRepPrimAPI_MakeBox(5.0, 6.0, 7.0).Shell());
  aBuilder.Add(aComp, BRepPrimAPI_MakeBox(5.0, 6.0, 7.0).Solid());
  TopoDS_Compound aComp1;
  aBuilder.MakeCompound(aComp1);
  aBuilder.Add(aComp, aComp1);

  // Iterate over compound components.
  myResult << "Compound components:" << std::endl;
  Standard_Integer anI = 1;
  for (TopoDS_Iterator anIt(aComp); anIt.More(); anIt.Next(), ++anI)
  {
    const TopoDS_Shape& aShape = anIt.Value();
    myResult << "#" << anI << " : ";
    Handle(AIS_ColoredShape) anAisShape;
    switch (aShape.ShapeType())
    {
      case TopAbs_VERTEX:
        myResult << "TopAbs_VERTEX";
        anAisShape = new AIS_ColoredShape(aShape);
        anAisShape->SetColor(Quantity_Color(Quantity_NOC_YELLOW));
        break;
      case TopAbs_EDGE:
        anAisShape = new AIS_ColoredShape(aShape);
        anAisShape->SetColor(Quantity_Color(Quantity_NOC_GREEN));
        myResult << "TopAbs_EDGE";
        break;
      case TopAbs_WIRE:
        myResult << "TopAbs_WIRE";
        break;
      case TopAbs_FACE:
        anAisShape = new AIS_ColoredShape(aShape);
        anAisShape->SetColor(Quantity_Color(Quantity_NOC_RED));
        myResult << "TopAbs_FACE";
        break;
      case TopAbs_SHELL:
        myResult << "TopAbs_SHELL";
        break;
      case TopAbs_SOLID:
        myResult << "TopAbs_SOLID";
        break;
      case TopAbs_COMPOUND:
        myResult << "TopAbs_COMPOUND";
        break;
      case TopAbs_COMPSOLID:
        myResult << "TopAbs_COMPSOLID";
        break;
      case TopAbs_SHAPE:
        myResult << "TopAbs_SHAPE";
        break;
    }
    myResult << std::endl;
    if (anAisShape)
    {
      myObject3d.Append(anAisShape);
    }
  }
}

void TopologySamples::TopologyExplorer3dSample()
{
  // Make a box with a corner at [0, 0, 0] and the specified sizes.
  Standard_Real aSizeX = 5.0;
  Standard_Real aSizeY = 10.0;
  Standard_Real aSizeZ = 15.0;
  TopoDS_Shape aBox = BRepPrimAPI_MakeBox(aSizeX, aSizeY, aSizeZ);

  // Explore vertex references.
  myResult << "Vertex refs. : ";
  Standard_Integer nbVertices = 0;
  for (TopExp_Explorer anExp(aBox, TopAbs_VERTEX); anExp.More(); anExp.Next(), ++nbVertices)
  {
    const TopoDS_Shape& aShape = anExp.Current();
    Handle(AIS_ColoredShape) anAisShape = new AIS_ColoredShape(aShape);
    anAisShape->SetColor(Quantity_Color(Quantity_NOC_YELLOW));
    myObject3d.Append(anAisShape);
  }
  myResult << nbVertices << std::endl;

  // Explore edge references.
  myResult << "Edge refs.   : ";
  Standard_Integer nbEdges = 0;
  for (TopExp_Explorer anExp(aBox, TopAbs_EDGE); anExp.More(); anExp.Next(), ++nbEdges)
  {
    const TopoDS_Shape& aShape = anExp.Current();
    Handle(AIS_ColoredShape) anAisShape = new AIS_ColoredShape(aShape);
    anAisShape->SetColor(Quantity_Color(Quantity_NOC_GREEN));
    anAisShape->SetWidth(2.5);
    myObject3d.Append(anAisShape);
  }
  myResult << nbEdges << std::endl;

  // Explore face references.
  myResult << "Face refs.   : ";
  Standard_Integer nbFaces = 0;
  for (TopExp_Explorer anExp(aBox, TopAbs_FACE); anExp.More(); anExp.Next(), ++nbFaces)
  {
    const TopoDS_Shape& aShape = anExp.Current();
    Handle(AIS_ColoredShape) anAisShape = new AIS_ColoredShape(aShape);
    anAisShape->SetColor(Quantity_Color(Quantity_NOC_GREEN));
    anAisShape->SetWidth(2.5);
    myObject3d.Append(anAisShape);
  }
  myResult << nbFaces << std::endl;

  // Explore shell references.
  myResult << "Wire refs.   : ";
  Standard_Integer nbWires = 0;
  for (TopExp_Explorer anExp(aBox, TopAbs_WIRE); anExp.More(); anExp.Next(), ++nbWires)
  {
  }
  myResult << nbWires << std::endl;

  // Explore shell references.
  myResult << "Shell refs.  : ";
  Standard_Integer nbShells = 0;
  for (TopExp_Explorer anExp(aBox, TopAbs_SHELL); anExp.More(); anExp.Next(), ++nbShells)
  {
  }
  myResult << nbShells << std::endl;

  // Explore solid references.
  myResult << "Solid refs.  : ";
  Standard_Integer nbSolids = 0;
  for (TopExp_Explorer anExp(aBox, TopAbs_SOLID); anExp.More(); anExp.Next(), ++nbSolids)
  {
  }
  myResult << nbSolids << std::endl;
}

void TopologySamples::AssessToCurve3dSample()
{
  // Make a face from a sphere.
  gp_Sphere aSphere(gp::XOY(), 1.0);
  TopoDS_Face aFace = BRepBuilderAPI_MakeFace(aSphere);
  myResult << "TopoDS_Face on the sphere with" << std::endl
           << "the center at [ "
           << aSphere.Location().X() << ", " << aSphere.Location().Y() << ", " << aSphere.Location().Z()
           << " ] and R = " << aSphere.Radius() << " was created in yellow" << std::endl
           << std::endl;

  Handle(AIS_ColoredShape) anAisFace = new AIS_ColoredShape(aFace);
  anAisFace->SetColor(Quantity_Color(Quantity_NOC_YELLOW));
  myObject3d.Append(anAisFace);

  // Retrieve the first not degenerated edge.
  TopoDS_Edge aCurveEdge;
  for (TopExp_Explorer anExp(aFace, TopAbs_EDGE); anExp.More(); anExp.Next())
  {
    TopoDS_Edge anEdge = TopoDS::Edge(anExp.Current());
    if (!BRep_Tool::Degenerated(anEdge))
    {
      aCurveEdge = anEdge;
      break;
    }
  }
  if (!aCurveEdge.IsNull())
  {
    // Make a curve on edge adaptor.
    BRepAdaptor_Curve aCurveAdaptor(aCurveEdge);
    myResult << "Curve adaptor for edge was built in red" << std::endl;

    Handle(AIS_ColoredShape) anAisCurveEdge = new AIS_ColoredShape(aCurveEdge);
    anAisCurveEdge->SetColor(Quantity_Color(Quantity_NOC_RED));
    anAisCurveEdge->SetWidth(1.5);
    myObject3d.Append(anAisCurveEdge);

    // Use the curve adaptor for some calculations, e.g. compute
    // a set of points using GCPnts_QuasiUniformDeflection algo.
    Standard_Real aDeflection = 0.1;
    GCPnts_QuasiUniformDeflection anAlgo(aCurveAdaptor, aDeflection);
    Standard_ASSERT_VOID(anAlgo.IsDone(), "Success is expected!");
    myResult << "Distribution of point on the curve with " << aDeflection
             << " deflection was performed:" << std::endl;
    for (Standard_Integer i = 1; i <= anAlgo.NbPoints(); ++i)
    {
      gp_Pnt aPnt = anAlgo.Value(i);
      myResult << "Point #" << i << " : [ "
               << aPnt.X() << ", " << aPnt.Y() << ", " << aPnt.Z() << " ]" << std::endl;
      Handle(AIS_Point) anAisPnt = new AIS_Point(new Geom_CartesianPoint(aPnt));
      myObject3d.Append(anAisPnt);
    }
  }
}

void TopologySamples::AssessToCompositeCurve3dSample()
{
  // Make a wire containing two BSpline curves.
  // Define points.
  gp_Pnt aPole1(0.0, 0.0, 10.0);
  gp_Pnt aPole2(5.0, 5.0, 5.0);
  gp_Pnt aPole3(10.0, 10.0, 15.0);
  gp_Pnt aPole4(15.0, 5.0, 20.0);
  gp_Pnt aPole5(25.0, 15.0, 20.0);
  gp_Pnt aPole6(35.0, 15.0, 15.0);
  gp_Pnt aPole7(45.0, 25.0, 10.0);
  // Add points to the curve poles array.
  TColgp_Array1OfPnt aPoles1(1, 4), aPoles2(1, 4);
  aPoles1.SetValue(1, aPole1);
  aPoles1.SetValue(2, aPole2);
  aPoles1.SetValue(3, aPole3);
  aPoles1.SetValue(4, aPole4);
  aPoles2.SetValue(1, aPole4);
  aPoles2.SetValue(2, aPole5);
  aPoles2.SetValue(3, aPole6);
  aPoles2.SetValue(4, aPole7);
  // Make a BSpline curves from the point arrays
  Handle(Geom_BSplineCurve) aBSplineCurve1 = GeomAPI_PointsToBSpline(aPoles1).Curve();
  Handle(Geom_BSplineCurve) aBSplineCurve2 = GeomAPI_PointsToBSpline(aPoles2).Curve();
  // Make edges
  TopoDS_Edge anEdge1 = BRepBuilderAPI_MakeEdge(aBSplineCurve1);
  TopoDS_Edge anEdge2 = BRepBuilderAPI_MakeEdge(aBSplineCurve2);
  // Make a wire
  BRepBuilderAPI_MakeWire aMakeWire;
  aMakeWire.Add(anEdge1);
  aMakeWire.Add(anEdge2);
  Standard_ASSERT_VOID(aMakeWire.IsDone(), "Added edge isn't connectible!");
  TopoDS_Wire aWire = aMakeWire.Wire();
  myResult << "Wire of two BSpline curves was created" << std::endl;

  Handle(AIS_ColoredShape) anAisWire = new AIS_ColoredShape(aWire);
  myObject3d.Append(anAisWire);

  // Make an adaptor.
  BRepAdaptor_CompCurve aCurveAdaptor(aWire);

  // Use the curve adaptor for some calculations, e.g. compute
  // a set of points using GCPnts_QuasiUniformDeflection algo.
  Standard_Real aDeflection = 0.5;
  GCPnts_QuasiUniformDeflection anAlgo(aCurveAdaptor, aDeflection);
  Standard_ASSERT_VOID(anAlgo.IsDone(), "Success is expected!");
  myResult << "Distribution of point on the curve with " << aDeflection
           << " deflection was performed:" << std::endl;
  for (Standard_Integer i = 1; i <= anAlgo.NbPoints(); ++i)
  {
    gp_Pnt aPnt = anAlgo.Value(i);
    myResult << "Point #" << i << " : [ "
             << aPnt.X() << ", " << aPnt.Y() << ", " << aPnt.Z() << " ]" << std::endl;
    Handle(AIS_Point) anAisPnt = new AIS_Point(new Geom_CartesianPoint(aPnt));
    myObject3d.Append(anAisPnt);
  }
}

void TopologySamples::AssessToSurface3dSample()
{
  // Make a face from a sphere.
  gp_Sphere aSphere(gp::XOY(), 4.0);
  TopoDS_Face aFace = BRepBuilderAPI_MakeFace(aSphere);
  myResult << "TopoDS_Face on the sphere with" << std::endl
           << "the center at [ "
           << aSphere.Location().X() << ", " << aSphere.Location().Y() << ", " << aSphere.Location().Z()
           << " ] and R = " << aSphere.Radius() << " was created in yellow" << std::endl
           << std::endl;

  // Make a surface adaptor.
  BRepAdaptor_Surface aSurfAdaptor(aFace);

  // Use the surface adaptor for some calculations, e.g. compute
  // a normal vector at a surface point.
  Standard_Real anU = 0.0, aV = 0.0;
  gp_Pnt aPnt;
  gp_Vec aDU, aDV;
  aSurfAdaptor.D1(anU, aV, aPnt, aDU, aDV);
  gp_Vec aNorm = aDU.Crossed(aDV);
  Standard_ASSERT_VOID(aNorm.Magnitude() > Precision::Confusion(), "Non zero vector is expected!");
  aNorm.Normalize();
  myResult << "Normal vector at ( " << anU << ", " << aV << " )" << std::endl
           << " = " << aNorm.X() << ", " << aNorm.Y() << ", " << aNorm.Z() << " ] is in red" << std::endl;

  Handle(AIS_ColoredShape) anAisFace = new AIS_ColoredShape(aFace);
  anAisFace->SetColor(Quantity_Color(Quantity_NOC_YELLOW));
  Handle(AIS_Point) anAisPnt = new AIS_Point(new Geom_CartesianPoint(aPnt));
  Handle(AIS_ColoredShape) anAisNorm = new AIS_ColoredShape(
    BRepBuilderAPI_MakeEdge(aPnt, aPnt.XYZ() + aNorm.XYZ()));
  myObject3d.Append(anAisFace);
  myObject3d.Append(anAisNorm);
  myObject3d.Append(anAisPnt);
}

void TopologySamples::Common3dSample()
{
  // Make a box #1 with a corner at [0, 0, 0] and the specified sizes.
  Standard_Real aSizeX = 10.0;
  Standard_Real aSizeY = 15.0;
  Standard_Real aSizeZ = 20.0;
  TopoDS_Shape aShape1 = BRepPrimAPI_MakeBox(aSizeX, aSizeY, aSizeZ);
  myResult << "Box at corner [0, 0, 0] and sizes ["
           << aSizeX << ", " << aSizeY << ", " << aSizeZ
           << "] was created in yellow wireframe" << std::endl;

  // Make a box #2 by two points.
  gp_Pnt aPnt1(5.0, 7.5, 10.0);
  gp_Pnt aPnt2(20.0, 25.0, 30.0);
  TopoDS_Shape aShape2 = BRepPrimAPI_MakeBox(aPnt1, aPnt2);
  myResult << "Box with corners ["
           << aPnt1.X() << ", " << aPnt1.Y() << ", " << aPnt1.Z()
           << "] and ["
           << aPnt2.X() << ", " << aPnt2.Y() << ", " << aPnt2.Z()
           << "] was created in green wirefreme" << std::endl;

  // Create a boolean algo.
  BRepAlgoAPI_Common anAlgo(aShape1, aShape2);

  // Make operation.
  anAlgo.Build();

  if (!anAlgo.IsDone()) // Process errors
  {
    myResult << "Errors : " << std::endl;
    anAlgo.DumpErrors(myResult);
  }
  if (anAlgo.HasWarnings()) // Process warnings
  {
    myResult << "Warnings : " << std::endl;
    anAlgo.DumpErrors(myResult);
  }

  if (anAlgo.IsDone())
  {
    // Get result.
    TopoDS_Shape aResultShape = anAlgo.Shape();
    myResult << "Result shape was created in red shading" << std::endl;

    Handle(AIS_ColoredShape) anAisShape1 = new AIS_ColoredShape(aShape1);
    Handle(AIS_ColoredShape) anAisShape2 = new AIS_ColoredShape(aShape2);
    anAisShape1->SetColor(Quantity_Color(Quantity_NOC_YELLOW));
    anAisShape2->SetColor(Quantity_Color(Quantity_NOC_GREEN));
    Handle(AIS_ColoredShape) anAisResult = new AIS_ColoredShape(aResultShape);
    anAisResult->SetColor(Quantity_Color(Quantity_NOC_RED));
    myObject3d.Append(anAisShape1);
    myObject3d.Append(anAisShape2);
    myObject3d.Append(anAisResult);
    myContext->SetDisplayMode(anAisShape1, 0, Standard_True);
    myContext->SetDisplayMode(anAisShape2, 0, Standard_True);
    myContext->SetDisplayMode(anAisResult, 1, Standard_True);
  }
}

void TopologySamples::Cut3dSample()
{
  // Make a box #1 with a corner at [0, 0, 0] and the specified sizes.
  Standard_Real aSizeX = 10.0;
  Standard_Real aSizeY = 15.0;
  Standard_Real aSizeZ = 20.0;
  TopoDS_Shape aShape1 = BRepPrimAPI_MakeBox(aSizeX, aSizeY, aSizeZ);
  myResult << "Box at corner [0, 0, 0] and sizes ["
           << aSizeX << ", " << aSizeY << ", " << aSizeZ
           << "] was created in yellow wireframe" << std::endl;

  // Make a box #2 by two points as a cutting tool.
  gp_Pnt aPnt1(5.0, 7.5, 10.0);
  gp_Pnt aPnt2(20.0, 25.0, 30.0);
  TopoDS_Shape aShape2 = BRepPrimAPI_MakeBox(aPnt1, aPnt2);
  myResult << "Box with corners ["
           << aPnt1.X() << ", " << aPnt1.Y() << ", " << aPnt1.Z()
           << "] and ["
           << aPnt2.X() << ", " << aPnt2.Y() << ", " << aPnt2.Z()
           << "] was created in green wireframe" << std::endl;

  // Create a boolean algo.
  BRepAlgoAPI_Cut anAlgo(aShape1, aShape2);

  // Make operation.
  anAlgo.Build();

  if (!anAlgo.IsDone()) // Process errors
  {
    myResult << "Errors : " << std::endl;
    anAlgo.DumpErrors(myResult);
  }
  if (anAlgo.HasWarnings()) // Process warnings
  {
    myResult << "Warnings : " << std::endl;
    anAlgo.DumpErrors(myResult);
  }

  if (anAlgo.IsDone())
  {
    // Get result.
    TopoDS_Shape aResultShape = anAlgo.Shape();
    myResult << "Result shape was created in red shading" << std::endl;
    Handle(AIS_ColoredShape) anAisShape1 = new AIS_ColoredShape(aShape1);
    Handle(AIS_ColoredShape) anAisShape2 = new AIS_ColoredShape(aShape2);
    anAisShape1->SetColor(Quantity_Color(Quantity_NOC_YELLOW));
    anAisShape2->SetColor(Quantity_Color(Quantity_NOC_GREEN));
    Handle(AIS_ColoredShape) anAisResult = new AIS_ColoredShape(aResultShape);
    anAisResult->SetColor(Quantity_Color(Quantity_NOC_RED));
    myObject3d.Append(anAisShape1);
    myObject3d.Append(anAisShape2);
    myObject3d.Append(anAisResult);
    myContext->SetDisplayMode(anAisShape1, 0, Standard_True);
    myContext->SetDisplayMode(anAisShape2, 0, Standard_True);
    myContext->SetDisplayMode(anAisResult, 1, Standard_True);
  }
}

void TopologySamples::Fuse3dSample()
{
  // Make a box #1 with a corner at [0, 0, 0] and the specified sizes.
  Standard_Real aSizeX = 10.0;
  Standard_Real aSizeY = 15.0;
  Standard_Real aSizeZ = 20.0;
  TopoDS_Shape aShape1 = BRepPrimAPI_MakeBox(aSizeX, aSizeY, aSizeZ);
  myResult << "Box at corner [0, 0, 0] and sizes ["
           << aSizeX << ", " << aSizeY << ", " << aSizeZ
           << "] was created in yellow wireframe" << std::endl;

  // Make a box #2 by two points.
  gp_Pnt aPnt1(5.0, 7.5, 10.0);
  gp_Pnt aPnt2(20.0, 25.0, 30.0);
  TopoDS_Shape aShape2 = BRepPrimAPI_MakeBox(aPnt1, aPnt2);
  myResult << "Box with corners ["
           << aPnt1.X() << ", " << aPnt1.Y() << ", " << aPnt1.Z()
           << "] and ["
           << aPnt2.X() << ", " << aPnt2.Y() << ", " << aPnt2.Z()
           << "] was created in green wireframe" << std::endl;

  // Create a boolean algo.
  BRepAlgoAPI_Fuse anAlgo(aShape1, aShape2);

  // Make operation.
  anAlgo.Build();

  if (!anAlgo.IsDone()) // Process errors
  {
    myResult << "Errors : " << std::endl;
    anAlgo.DumpErrors(myResult);
  }
  if (anAlgo.HasWarnings()) // Process warnings
  {
    myResult << "Warnings : " << std::endl;
    anAlgo.DumpErrors(myResult);
  }

  if (anAlgo.IsDone())
  {
    // Get result.
    TopoDS_Shape aResultShape = anAlgo.Shape();
    myResult << "Result shape was created in red shading" << std::endl;
    Handle(AIS_ColoredShape) anAisShape1 = new AIS_ColoredShape(aShape1);
    Handle(AIS_ColoredShape) anAisShape2 = new AIS_ColoredShape(aShape2);
    anAisShape1->SetColor(Quantity_Color(Quantity_NOC_YELLOW));
    anAisShape2->SetColor(Quantity_Color(Quantity_NOC_GREEN));
    Handle(AIS_ColoredShape) anAisResult = new AIS_ColoredShape(aResultShape);
    anAisResult->SetColor(Quantity_Color(Quantity_NOC_RED));
    myObject3d.Append(anAisShape1);
    myObject3d.Append(anAisShape2);
    myObject3d.Append(anAisResult);
    myContext->SetDisplayMode(anAisShape1, 0, Standard_True);
    myContext->SetDisplayMode(anAisShape2, 0, Standard_True);
    myContext->SetDisplayMode(anAisResult, 1, Standard_True);
  }
}

void TopologySamples::Section3dSample()
{
  // Make a box #1 with a corner at [0, 0, 0] and the specified sizes.
  Standard_Real aSizeX = 10.0;
  Standard_Real aSizeY = 15.0;
  Standard_Real aSizeZ = 20.0;
  TopoDS_Shape aShape = BRepPrimAPI_MakeBox(aSizeX, aSizeY, aSizeZ);
  myResult << "Box at corner [0, 0, 0] and sizes ["
           << aSizeX << ", " << aSizeY << ", " << aSizeZ
           << "] was created in yellow wireframe" << std::endl;

  // Create a boolean algo.
  // Make a section by a plane.
  gp_Pln aPln(gp_Pnt(aSizeX / 2.0, aSizeY / 2.0, aSizeZ / 2.0), gp::DZ());
  BRepAlgoAPI_Section anAlgo(aShape, aPln, Standard_False);

  // Make operation.
  anAlgo.Build();

  if (!anAlgo.IsDone()) // Process errors
  {
    myResult << "Errors : " << std::endl;
    anAlgo.DumpErrors(myResult);
  }
  if (anAlgo.HasWarnings()) // Process warnings
  {
    myResult << "Warnings : " << std::endl;
    anAlgo.DumpErrors(myResult);
  }

  if (anAlgo.IsDone())
  {
    // Get result.
    TopoDS_Shape aResultShape = anAlgo.Shape();
    myResult << "Result shape was created in red" << std::endl;
    Handle(AIS_ColoredShape) anAisShape = new AIS_ColoredShape(aShape);
    Handle(AIS_Plane) anAisPlane = new AIS_Plane(new Geom_Plane(aPln));
    anAisShape->SetColor(Quantity_Color(Quantity_NOC_YELLOW));
    Handle(AIS_ColoredShape) anAisResult = new AIS_ColoredShape(aResultShape);
    anAisResult->SetColor(Quantity_Color(Quantity_NOC_RED));
    myObject3d.Append(anAisShape);
    myObject3d.Append(anAisPlane);
    myObject3d.Append(anAisResult);
    myContext->SetDisplayMode(anAisShape, 0, Standard_True);
  }
}

void TopologySamples::Splitter3dSample()
{
  // Make a box by two points.
  gp_Pnt aPnt1(-5.0, -7.5, -10.0);
  gp_Pnt aPnt2(10.0, 15.0, 10.0);
  TopoDS_Shape aBox = BRepPrimAPI_MakeBox(aPnt1, aPnt2);
  myResult << "Box with corners ["
           << aPnt1.X() << ", " << aPnt1.Y() << ", " << aPnt1.Z()
           << "] and ["
           << aPnt2.X() << ", " << aPnt2.Y() << ", " << aPnt2.Z()
           << "] was created in yellow" << std::endl;

  // Make a splitting tool as XY plane.
  TopoDS_Shape aTool = BRepBuilderAPI_MakeFace(gp_Pln(gp::XOY()));

  // Create a splitter algo.
  BRepAlgoAPI_Splitter aSplitter;

  // Add shapes to be split.
  TopTools_ListOfShape anArguments;
  anArguments.Append(aBox);
  aSplitter.SetArguments(anArguments);

  // Add tool shapes.
  TopTools_ListOfShape aTools;
  aTools.Append(aTool);
  aSplitter.SetTools(aTools);

  // Perform splitting.
  aSplitter.Build();

  if (!aSplitter.IsDone()) // Process errors
  {
    myResult << "Errors : " << std::endl;
    aSplitter.DumpErrors(myResult);
  }
  if (aSplitter.HasWarnings()) // Process warnings
  {
    myResult << "Warnings : " << std::endl;
    aSplitter.DumpErrors(myResult);
  }

  Handle(AIS_ColoredShape) anAisBox = new AIS_ColoredShape(aBox);
  anAisBox->SetColor(Quantity_Color(Quantity_NOC_YELLOW));
  myObject3d.Append(anAisBox);

  if (aSplitter.IsDone()) // Process results
  {
    // Simplification of the result shape is performed by the means of
    // ShapeUpgrade_UnifySameDomain algorithm. The result of the operation will
    // be overwritten with the simplified result.
    // The simplification is performed without creation of the Internal shapes,
    // i.e. shapes connections will never be broken.
    // Simplification is performed on the whole result shape. Thus, if the input
    // shapes contained connected tangent edges or faces unmodified during the operation
    // they will also be unified.
    aSplitter.SimplifyResult();

    // Get result of splitting.
    TopoDS_Shape aResult = aSplitter.Shape();
    myResult << "Splitting result (shapes are moved apart for illustativeness) is in red" << std::endl;

    // In this particular sample two shapes in the result are expected.
    // Lets move apart them for illustrative purposes.
    TopoDS_Iterator anIt(aResult);
    Standard_ASSERT_VOID(anIt.More(), "Not empty result is expected!");
    TopoDS_Shape aBox1 = anIt.Value(); anIt.Next();
    Standard_ASSERT_VOID(anIt.More(), "Two shapes in the result are expected!");
    TopoDS_Shape aBox2 = anIt.Value();
    gp_Trsf aTrsf1; aTrsf1.SetTranslation(gp_Vec(0.0, 0.0, -15.0));
    aBox1.Move(aTrsf1);
    gp_Trsf aTrsf2; aTrsf2.SetTranslation(gp_Vec(0.0, 0.0, +15.0));
    aBox2.Move(aTrsf2);

    Handle(AIS_ColoredShape) anAisBox1 = new AIS_ColoredShape(aBox1);
    Handle(AIS_ColoredShape) anAisBox2 = new AIS_ColoredShape(aBox2);
    anAisBox1->SetColor(Quantity_Color(Quantity_NOC_RED));
    anAisBox2->SetColor(Quantity_Color(Quantity_NOC_RED));
    myObject3d.Append(anAisBox1);
    myObject3d.Append(anAisBox2);
  }
}

void TopologySamples::Defeaturing3dSample()
{
  // Prepare a box with a chamfer.
  // Make a box with a corner at [0, 0, 0] and the specified sizes.
  Standard_Real aSizeX = 8.0;
  Standard_Real aSizeY = 10.0;
  Standard_Real aSizeZ = 15.0;
  TopoDS_Shape aBox = BRepPrimAPI_MakeBox(aSizeX, aSizeY, aSizeZ);
  // Initialize chamfer algo.
  BRepFilletAPI_MakeChamfer anAlgo(aBox);
  // Set edge to apply a chamfer with specified distance from it.
  // Select the 5th edge in the map returned by TopExp::MapShapes method.
  TopTools_IndexedMapOfShape anEdges;
  TopExp::MapShapes(aBox, TopAbs_EDGE, anEdges);
  TopoDS_Edge anEdge = TopoDS::Edge(anEdges.FindKey(5));
  Standard_Real aDist = 4.0;
  anAlgo.Add(aDist, anEdge);
  // Make a chamfer.
  anAlgo.Build();
  Standard_ASSERT_VOID(anAlgo.IsDone(), "Couldn't prepare a box with a chamfer!");
  // Get a box with a chamfer.
  TopoDS_Shape aBoxWithChamfer = anAlgo.Shape();
  myResult << "Box with a chamfer is in yellow shading" << std::endl;

  Handle(AIS_ColoredShape) anAisBoxWithChamfer = new AIS_ColoredShape(aBoxWithChamfer);
  anAisBoxWithChamfer->SetColor(Quantity_Color(Quantity_NOC_YELLOW));
  myObject3d.Append(anAisBoxWithChamfer);
  myContext->SetDisplayMode(anAisBoxWithChamfer, 1, Standard_True);

  // Retrieve chamfer faces generated from the edge
  const TopTools_ListOfShape& aGenShapes = anAlgo.Generated(anEdge);
  Standard_ASSERT_VOID(!aGenShapes.IsEmpty(), "Chamfer face is expected!");
  for (TopTools_ListOfShape::Iterator anIt(aGenShapes); anIt.More(); anIt.Next())
  {
    Handle(AIS_ColoredShape) anAisShape = new AIS_ColoredShape(anIt.Value());
    anAisShape->SetColor(Quantity_Color(Quantity_NOC_GREEN));
    anAisShape->SetWidth(2.5);
    myObject3d.Append(anAisShape);
    myContext->SetDisplayMode(anAisBoxWithChamfer, 1, Standard_True);
  }
  myResult << "Chamfer faces : " << aGenShapes.Size() << std::endl;
  myResult << "The first one is using to remove" << std::endl;
  myResult << "The removed face is in green" << std::endl;

  // Initialize defeaturing algo.
  BRepAlgoAPI_Defeaturing aDefeatAlgo;
  aDefeatAlgo.SetShape(aBoxWithChamfer);
  aDefeatAlgo.AddFaceToRemove(aGenShapes.First());

  // Remove the chamfer.
  aDefeatAlgo.Build();

  if (aDefeatAlgo.IsDone())
  {
    // Get result.
    TopoDS_Shape aResult = aDefeatAlgo.Shape();
    myResult << "Defeatured box is in red wireframe" << std::endl;

    Handle(AIS_ColoredShape) anAisResult = new AIS_ColoredShape(aResult);
    anAisResult->SetColor(Quantity_Color(Quantity_NOC_RED));
    myObject3d.Append(anAisResult);
    myContext->SetDisplayMode(anAisResult, 0, Standard_True);
  }
}

void TopologySamples::Fillet3dSample()
{
  // Make a box with a corner at [0, 0, 0] and the specified sizes.
  Standard_Real aSizeX = 8.0;
  Standard_Real aSizeY = 10.0;
  Standard_Real aSizeZ = 15.0;
  TopoDS_Shape aBox = BRepPrimAPI_MakeBox(aSizeX, aSizeY, aSizeZ);
  myResult << "Box at corner [0, 0, 0] and sizes ["
           << aSizeX << ", " << aSizeY << ", " << aSizeZ
           << "] was created in yellow wireframe" << std::endl;

  // Initialize fillet algo.
  BRepFilletAPI_MakeFillet anAlgo(aBox);

  // Set edge to apply a fillet with specified radius.
  // Select the first edge in the map returned by TopExp::MapShapes method.
  TopTools_IndexedMapOfShape anEdges;
  TopExp::MapShapes(aBox, TopAbs_EDGE, anEdges);
  TopoDS_Edge anEdge = TopoDS::Edge(anEdges.FindKey(1));
  Standard_Real aRadius = 3.0;
  anAlgo.Add(aRadius, anEdge);
  myResult << "Make a fillet of " << aRadius << " radius on an edge in green" << std::endl;

  Handle(AIS_ColoredShape) anAisBox = new AIS_ColoredShape(aBox);
  Handle(AIS_ColoredShape) anAisEdge = new AIS_ColoredShape(anEdge);
  anAisBox->SetColor(Quantity_Color(Quantity_NOC_YELLOW));
  anAisEdge->SetColor(Quantity_Color(Quantity_NOC_GREEN));
  anAisEdge->SetWidth(2.5);
  myObject3d.Append(anAisBox);
  myObject3d.Append(anAisEdge);
  myContext->SetDisplayMode(anAisBox, 0, Standard_True);

  // Make a fillet.
  anAlgo.Build();

  if (anAlgo.IsDone())
  {
    // Get result.
    TopoDS_Shape aResult = anAlgo.Shape();
    myResult << "Fillet was built. Result shape is in red shading" << std::endl;

    Handle(AIS_ColoredShape) anAisResult = new AIS_ColoredShape(aResult);
    anAisResult->SetColor(Quantity_Color(Quantity_NOC_RED));
    myObject3d.Append(anAisResult);
    myContext->SetDisplayMode(anAisResult, 1, Standard_True);
  }
}

void TopologySamples::Chamfer3dSample()
{
  // Make a box with a corner at [0, 0, 0] and the specified sizes.
  Standard_Real aSizeX = 8.0;
  Standard_Real aSizeY = 10.0;
  Standard_Real aSizeZ = 15.0;
  TopoDS_Shape aBox = BRepPrimAPI_MakeBox(aSizeX, aSizeY, aSizeZ);
  myResult << "Box at corner [0, 0, 0] and sizes ["
           << aSizeX << ", " << aSizeY << ", " << aSizeZ
           << "] was created in yellow wirewrame" << std::endl;

  // Initialize chamfer algo.
  BRepFilletAPI_MakeChamfer anAlgo(aBox);

  // Set edge to apply a chamfer with specified distance from it.
  // Select the 5th edge in the map returned by TopExp::MapShapes method.
  TopTools_IndexedMapOfShape anEdges;
  TopExp::MapShapes(aBox, TopAbs_EDGE, anEdges);
  TopoDS_Edge anEdge = TopoDS::Edge(anEdges.FindKey(5));
  Standard_Real aDist = 4.0;
  anAlgo.Add(aDist, anEdge);
  myResult << "Make a chamfer of " << aDist << " size on an edge in green" << std::endl;

  Handle(AIS_ColoredShape) anAisBox = new AIS_ColoredShape(aBox);
  Handle(AIS_ColoredShape) anAisEdge = new AIS_ColoredShape(anEdge);
  anAisBox->SetColor(Quantity_Color(Quantity_NOC_YELLOW));
  anAisEdge->SetColor(Quantity_Color(Quantity_NOC_GREEN));
  anAisEdge->SetWidth(2.5);
  myObject3d.Append(anAisBox);
  myObject3d.Append(anAisEdge);
  myContext->SetDisplayMode(anAisBox, 0, Standard_True);

  // Make a chamfer.
  anAlgo.Build();
  if (anAlgo.IsDone())
  {
    // Get result.
    TopoDS_Shape aResult = anAlgo.Shape();
    myResult << "Fillet was built. Result shape is in red shading" << std::endl;

    Handle(AIS_ColoredShape) anAisResult = new AIS_ColoredShape(aResult);
    anAisResult->SetColor(Quantity_Color(Quantity_NOC_RED));
    myObject3d.Append(anAisResult);
    myContext->SetDisplayMode(anAisResult, 1, Standard_True);

    const TopTools_ListOfShape& aGenShapes = anAlgo.Generated(anEdge);
    for (TopTools_ListOfShape::Iterator anIt(aGenShapes); anIt.More(); anIt.Next())
    {
      Handle(AIS_ColoredShape) anAisShape = new AIS_ColoredShape(anIt.Value());
      anAisShape->SetColor(Quantity_Color(Quantity_NOC_RED));
      anAisShape->SetWidth(2.5);
      myObject3d.Append(anAisShape);
    }
  }
}

void TopologySamples::Offset3dSample()
{
  // Make a triangle wire.
  BRepBuilderAPI_MakePolygon aTria;
  TopoDS_Vertex aVertA = BRepBuilderAPI_MakeVertex(gp_Pnt(-0.5, 0.0, 0.0));
  TopoDS_Vertex aVertB = BRepBuilderAPI_MakeVertex(gp_Pnt(0.0, 0.0, +1.0));
  TopoDS_Vertex aVertC = BRepBuilderAPI_MakeVertex(gp_Pnt(+0.5, 0.0, 0.0));
  aTria.Add(aVertA);
  aTria.Add(aVertB);
  aTria.Add(aVertC);
  aTria.Close();
  TopoDS_Wire aWire = aTria.Wire();
  myResult << "Triangular wire was created in yellow" << std::endl;

  Handle(AIS_ColoredShape) anAisWire = new AIS_ColoredShape(aWire);
  anAisWire->SetColor(Quantity_Color(Quantity_NOC_YELLOW));
  myObject3d.Append(anAisWire);

  // Initialize offset algo.
  BRepOffsetAPI_MakeOffset anAlgo(aWire);

  // Perform a series of offsets with linearly increasing value and altitude.
  Standard_Real anOffsetStep = 0.2;
  Standard_Real anAltitudeStep = 0.1;
  for (Standard_Integer i = 1; i <= 4; ++i)
  {
    Standard_Real anOffset = anOffsetStep * i;
    Standard_Real anAltitude = anAltitudeStep * i;
    anAlgo.Perform(anOffset, anAltitude);
    if (anAlgo.IsDone())
    {
      // Get result.
      TopoDS_Shape aResult = anAlgo.Shape();
      myResult << "#" << i << " : Offset = " << anOffset << " Altitude = " << anAltitude
               << ". Result is in red." << std::endl;

      Handle(AIS_ColoredShape) anAisResult = new AIS_ColoredShape(aResult);
      anAisResult->SetColor(Quantity_Color(Quantity_NOC_RED));
      myObject3d.Append(anAisResult);
    }
  }
}

void TopologySamples::Evolved3dSample()
{
  // Make a triangle wire as a spine.
  BRepBuilderAPI_MakePolygon aTria;
  TopoDS_Vertex aVertA = BRepBuilderAPI_MakeVertex(gp_Pnt(-0.5, 0.0, 0.0));
  TopoDS_Vertex aVertB = BRepBuilderAPI_MakeVertex(gp_Pnt(0.0, +1.0, 0.0));
  TopoDS_Vertex aVertC = BRepBuilderAPI_MakeVertex(gp_Pnt(+0.5, 0.0, 0.0));
  aTria.Add(aVertA);
  aTria.Add(aVertB);
  aTria.Add(aVertC);
  aTria.Close();
  TopoDS_Wire aSpine = aTria.Wire();
  myResult << "Profile wire was created in yellow" << std::endl;

  // Make a wire as a profile.
  BRepBuilderAPI_MakePolygon aPoly;
  TopoDS_Vertex aVert1 = BRepBuilderAPI_MakeVertex(gp_Pnt(-0.5, 0.0, 0.0));
  TopoDS_Vertex aVert2 = BRepBuilderAPI_MakeVertex(gp_Pnt(-0.5, -0.1, 0.5));
  TopoDS_Vertex aVert3 = BRepBuilderAPI_MakeVertex(gp_Pnt(-0.5, -0.2, 1.0));
  aPoly.Add(aVert1);
  aPoly.Add(aVert2);
  aPoly.Add(aVert3);
  TopoDS_Wire aProfile = aPoly.Wire();
  myResult << "Spine wire was created in greed" << std::endl;

  Handle(AIS_ColoredShape) anAisSpine = new AIS_ColoredShape(aSpine);
  Handle(AIS_ColoredShape) anAisProfile = new AIS_ColoredShape(aProfile);
  anAisSpine->SetColor(Quantity_Color(Quantity_NOC_YELLOW));
  anAisSpine->SetWidth(2.5);
  anAisProfile->SetColor(Quantity_Color(Quantity_NOC_GREEN));
  anAisProfile->SetWidth(2.5);
  myObject3d.Append(anAisSpine);
  myObject3d.Append(anAisProfile);

  // Initialize evolving algo.
  GeomAbs_JoinType aJoinType = GeomAbs_Arc;
  Standard_Boolean aIsGlobalCS = Standard_False;
  Standard_Boolean aIsSolid = Standard_True;
  BRepOffsetAPI_MakeEvolved anAlgo(aSpine, aProfile, aJoinType, aIsGlobalCS, aIsSolid);

  // Perform evolving.
  anAlgo.Build();

  if (anAlgo.IsDone())
  {
    // Get result.
    TopoDS_Shape aResult = anAlgo.Shape();
    myResult << "Evolving result is in red" << std::endl;

    Handle(AIS_ColoredShape) anAisResult = new AIS_ColoredShape(aResult);
    anAisResult->SetColor(Quantity_Color(Quantity_NOC_RED));
    myObject3d.Append(anAisResult);
  }
}

void TopologySamples::Copy3dSample()
{
  // Make a box with a corner at [0, 0, 0] and the specified sizes.
  Standard_Real aSizeX = 10.0;
  Standard_Real aSizeY = 15.0;
  Standard_Real aSizeZ = 20.0;
  BRepPrimAPI_MakeBox aBoxMake(aSizeX, aSizeY, aSizeZ);
  TopoDS_Shape aBox = aBoxMake.Shape();
  myResult << "Box at corner [0, 0, 0] and sizes ["
           << aSizeX << ", " << aSizeY << ", " << aSizeZ
           << "] was created in yellow" << std::endl;

  // Make a box copy.
  TopoDS_Shape aBoxCopy = BRepBuilderAPI_Copy(aBox);
  myResult << "Box copy was created in red" << std::endl;

  gp_Trsf aTrsf1; aTrsf1.SetTranslation(gp_Vec(15.0, 0.0, 0.0));
  aBoxCopy.Move(aTrsf1);
  myResult << "Box copy shape is moved apart for illustativeness" << std::endl;

  Handle(AIS_ColoredShape) anAisBox = new AIS_ColoredShape(aBox);
  anAisBox->SetColor(Quantity_Color(Quantity_NOC_YELLOW));
  anAisBox->SetWidth(2.5);
  Handle(AIS_ColoredShape) anAisBoxCopy = new AIS_ColoredShape(aBoxCopy);
  anAisBoxCopy->SetColor(Quantity_Color(Quantity_NOC_RED));
  myObject3d.Append(anAisBox);
  myObject3d.Append(anAisBoxCopy);
}

void TopologySamples::Transform3dSample()
{
  // Make a box with a corner at [0, 0, 0] and the specified sizes.
  Standard_Real aSizeX = 10.0;
  Standard_Real aSizeY = 15.0;
  Standard_Real aSizeZ = 20.0;
  BRepPrimAPI_MakeBox aBoxMake(aSizeX, aSizeY, aSizeZ);
  TopoDS_Shape aBox = aBoxMake.Shape();
  myResult << "Box at corner [0, 0, 0] and sizes ["
           << aSizeX << ", " << aSizeY << ", " << aSizeZ
           << "] was created in yellow" << std::endl;

  // Move the box.
  gp_Trsf aTrMove; aTrMove.SetTranslation(gp_Vec(15.0, 20.0, 25.0));
  TopoDS_Shape aMovedBox = BRepBuilderAPI_Transform(aBox, aTrMove, Standard_True);
  myResult << "Moved box in green" << std::endl;

  // Rotate the moved box
  gp_Trsf aTrRot; aTrRot.SetRotation(gp_Ax1(gp_Pnt(15.0, 20.0, 25.0), gp::DZ()), 3.0*M_PI_4);
  TopoDS_Shape aRotatedBox = BRepBuilderAPI_Transform(aBox, aTrRot, Standard_True);
  myResult << "Rotated box in red" << std::endl;

  Handle(AIS_ColoredShape) anAisBox = new AIS_ColoredShape(aBox);
  Handle(AIS_ColoredShape) anAisMovedBox = new AIS_ColoredShape(aMovedBox);
  Handle(AIS_ColoredShape) anAisRotatedBox = new AIS_ColoredShape(aRotatedBox);
  anAisBox->SetColor(Quantity_Color(Quantity_NOC_YELLOW));
  anAisMovedBox->SetColor(Quantity_Color(Quantity_NOC_GREEN));
  anAisRotatedBox->SetColor(Quantity_Color(Quantity_NOC_RED));
  myObject3d.Append(anAisBox);
  myObject3d.Append(anAisMovedBox);
  myObject3d.Append(anAisRotatedBox);
}

void TopologySamples::ConvertToNurbs3dSample()
{
  // Make a torus face.
  gp_Torus aTorus(gp::XOY(), 20.0, 7.5);
  TopoDS_Shape aTorusFace = BRepBuilderAPI_MakeFace(aTorus);
  myResult << "TopoDS_Solid on the torus with" << std::endl
           << "R major = " << aTorus.MajorRadius() << std::endl
           << "R minor = " << aTorus.MinorRadius() << std::endl
           << "was created in yellow" << std::endl;

  // Convert faces/edges from analytic to NURBS geometry.
  TopoDS_Shape aNurbsFace = BRepBuilderAPI_NurbsConvert(aTorusFace);
  myResult << "Converted torus in red" << std::endl;
  gp_Trsf aTrsf1; aTrsf1.SetTranslation(gp_Vec(60.0, 0.0, 0.0));
  aNurbsFace.Move(aTrsf1);
  myResult << "Converted torus is moved apart for illustativeness" << std::endl;

  Handle(AIS_ColoredShape) anAisTorus = new AIS_ColoredShape(aTorusFace);
  Handle(AIS_ColoredShape) anAisNurbs = new AIS_ColoredShape(aNurbsFace);
  anAisTorus->SetColor(Quantity_Color(Quantity_NOC_YELLOW));
  anAisTorus->SetWidth(2.5);
  anAisNurbs->SetColor(Quantity_Color(Quantity_NOC_RED));
  myObject3d.Append(anAisTorus);
  myObject3d.Append(anAisNurbs);
}

void TopologySamples::SewContiguousFaces3dSample()
{
  // Make a sphere.
  gp_Sphere aSphere(gp::XOY(), 1.0);
  // South hemisphere.
  TopoDS_Face aFace1 = BRepBuilderAPI_MakeFace(aSphere, 0.0, 2.0 * M_PI, -M_PI_2, 0.0);
  // North hemisphere.
  TopoDS_Face aFace2 = BRepBuilderAPI_MakeFace(aSphere, 0.0, 2.0 * M_PI, 0.0, +M_PI_2);

  // Make a default tailor.
  BRepBuilderAPI_Sewing aTailor;

  // Add hemisphere faces.
  aTailor.Add(aFace1);
  aTailor.Add(aFace2);

  // Perform sewing.
  aTailor.Perform();

  // Get result.
  const TopoDS_Shape& aSewedSphere = aTailor.SewedShape();
  myResult << "Two hemispheres were sewed : " << aTailor.NbFreeEdges() << " free edges" << std::endl;

  Handle(AIS_ColoredShape) anAisSewedSphere = new AIS_ColoredShape(aSewedSphere);
  anAisSewedSphere->SetColor(Quantity_Color(Quantity_NOC_RED));
  myObject3d.Append(anAisSewedSphere);
}

void TopologySamples::CheckValidity3dSample()
{
  // Make a box with a corner at [0, 0, 0] and the specified sizes.
  Standard_Real aSizeX = 10.0;
  Standard_Real aSizeY = 15.0;
  Standard_Real aSizeZ = 20.0;
  BRepPrimAPI_MakeBox aBoxMake(aSizeX, aSizeY, aSizeZ);
  TopoDS_Shape aBox = aBoxMake.Shape();
  myResult << "Box at corner [0, 0, 0] and sizes ["
           << aSizeX << ", " << aSizeY << ", " << aSizeZ
           << "] was created in yellow" << std::endl;

  // Analyze the box.
  BRepCheck_Analyzer anAnalyzer(aBox);
  myResult << "Box is " << (anAnalyzer.IsValid() ? "valid" : "invalid") << std::endl;

  // Make the box invalid manually.
  Handle(BRepTools_ReShape) aReShape = new BRepTools_ReShape();
  myResult << "Remove the top face from the box (red)" << std::endl;
  aReShape->Remove(aBoxMake.TopFace());
  TopoDS_Shape aBox1 = aReShape->Apply(aBox);
  myResult << "The top face was removed" << std::endl;

  // Analyze the modified box.
  BRepCheck_Analyzer anAnalyzer1(aBox1);
  myResult << "Modified box is " << (anAnalyzer1.IsValid() ? "valid" : "invalid") << std::endl;

  Handle(AIS_ColoredShape) anAisBox = new AIS_ColoredShape(aBox);
  Handle(AIS_ColoredShape) anAisTopFace = new AIS_ColoredShape(aBoxMake.TopFace());
  anAisBox->SetColor(Quantity_Color(Quantity_NOC_YELLOW));
  anAisTopFace->SetColor(Quantity_Color(Quantity_NOC_RED));
  myObject3d.Append(anAisBox);
  myObject3d.Append(anAisTopFace);
}

void TopologySamples::ComputeLinearProperties3dSample()
{
  // Make an edge from a circular segment.
  // Create a circle in XY plane of the radius 1.0.
  gp_Circ aCirc(gp::XOY(), 1.0);
  // Make a circular edge from the 1st quoter in the parametric space.
  TopoDS_Edge aShape = BRepBuilderAPI_MakeEdge(aCirc, 0.0, M_PI);
  myResult << "TopoDS_Edge on the circle's 1st quoter" << std::endl
           << "with the center at [ "
           << aCirc.Location().X() << ", " << aCirc.Location().Y() << ", " << aCirc.Location().Z()
           << " ] and R = " << aCirc.Radius() << " was created in red" << std::endl
           << std::endl;

  // Retrieve linear properties from the edge.
  GProp_GProps aGProps;
  BRepGProp::LinearProperties(aShape, aGProps);
  Standard_Real aLength = aGProps.Mass();
  gp_Pnt aCOM = aGProps.CentreOfMass();
  Standard_Real anIx, anIy, anIz;
  aGProps.StaticMoments(anIx, anIy, anIz);
  gp_Mat aMOI = aGProps.MatrixOfInertia();
  myResult << "Linear properties:" << std::endl
           << "  Length            = " << aLength << std::endl
           << "  Center of mass    = [ " << aCOM.X() << ", " << aCOM.Y() << ", " << aCOM.Z() << " ]" << std::endl
           << "  Static moments    = [ " << anIx << ", " << anIy << ", " << anIz << " ]" << std::endl
           << "  Matrix of inertia = [ "
           << aMOI(1, 1) << ", " << aMOI(1, 2) << ", " << aMOI(1, 3) << std::endl
           << std::setw(33) << aMOI(2, 1) << ", " << aMOI(2, 2) << ", " << aMOI(2, 3) << std::endl
           << std::setw(33) << aMOI(3, 1) << ", " << aMOI(3, 2) << ", " << aMOI(3, 3) << " ]" << std::endl;
  GProp_PrincipalProps aPProps = aGProps.PrincipalProperties();
  Standard_Real anIxx, anIyy, anIzz;
  aPProps.Moments(anIxx, anIyy, anIzz);
  Standard_Real aRxx, aRyy, aRzz;
  aPProps.RadiusOfGyration(aRxx, aRyy, aRzz);
  myResult << "Principal properties:" << std::endl
           << "  Has symmetric axis  : " << (aPProps.HasSymmetryAxis() ? "YES" : "NO") << std::endl
           << "  Has symmetric point : " << (aPProps.HasSymmetryPoint() ? "YES" : "NO") << std::endl
           << "  Moments of inertia  = [ " << anIxx << ", " << anIyy << ", " << anIzz << " ]" << std::endl
           << "  Radius of gyration  = [ " << aRxx << ", " << aRyy << ", " << aRzz << " ]" << std::endl;
  if (!aPProps.HasSymmetryPoint())
  {
    const gp_Vec& anAxis1 = aPProps.FirstAxisOfInertia();
    myResult << "  1st axis of inertia = [ " << anAxis1.X() << ", " << anAxis1.Y() << ", " << anAxis1.Z() << " ]" << std::endl;
    Handle(AIS_ColoredShape) anAisAxis1 = new AIS_ColoredShape(
      BRepBuilderAPI_MakeEdge(aCOM, aCOM.XYZ() + anAxis1.XYZ()));
    anAisAxis1->SetColor(Quantity_Color(Quantity_NOC_GREEN));
    myObject3d.Append(anAisAxis1);
    if (!aPProps.HasSymmetryPoint())
    {
      const gp_Vec& anAxis2 = aPProps.SecondAxisOfInertia();
      myResult << "  2nd axis of inertia = [ " << anAxis2.X() << ", " << anAxis2.Y() << ", " << anAxis2.Z() << " ]" << std::endl;
      Handle(AIS_ColoredShape) anAisAxis2 = new AIS_ColoredShape(
        BRepBuilderAPI_MakeEdge(aCOM, aCOM.XYZ() + anAxis2.XYZ()));
      anAisAxis2->SetColor(Quantity_Color(Quantity_NOC_GREEN));
      myObject3d.Append(anAisAxis2);
      const gp_Vec& anAxis3 = aPProps.ThirdAxisOfInertia();
      myResult << "  3rd axis of inertia = [ " << anAxis3.X() << ", " << anAxis3.Y() << ", " << anAxis3.Z() << " ]" << std::endl;
      Handle(AIS_ColoredShape) anAisAxis3 = new AIS_ColoredShape(
        BRepBuilderAPI_MakeEdge(aCOM, aCOM.XYZ() + anAxis3.XYZ()));
      anAisAxis3->SetColor(Quantity_Color(Quantity_NOC_GREEN));
      myObject3d.Append(anAisAxis3);
    }
  }

  Handle(AIS_ColoredShape) anAisShape = new AIS_ColoredShape(aShape);
  Handle(AIS_Point) anAisCOM = new AIS_Point(new Geom_CartesianPoint(aCOM));
  anAisShape->SetColor(Quantity_Color(Quantity_NOC_RED));
  Handle(AIS_TextLabel) aCOMLabel = new AIS_TextLabel();
  aCOMLabel->SetText("Center of mass");
  aCOMLabel->SetPosition(aCOM);
  Handle(AIS_Axis) anAisAxisX = new AIS_Axis(new Geom_Axis2Placement(gp::YOZ()), AIS_TOAX_XAxis);
  Handle(AIS_Axis) anAisAxisY = new AIS_Axis(new Geom_Axis2Placement(gp::ZOX()), AIS_TOAX_YAxis);
  Handle(AIS_Axis) anAisAxisZ = new AIS_Axis(new Geom_Axis2Placement(gp::XOY()), AIS_TOAX_ZAxis);
  myObject3d.Append(anAisAxisX);
  myObject3d.Append(anAisAxisY);
  myObject3d.Append(anAisAxisZ);
  myObject3d.Append(anAisShape);
  myObject3d.Append(anAisCOM);
  myObject3d.Append(aCOMLabel);
}

void TopologySamples::ComputeSurfaceProperties3dSample()
{
  // Make a face from a cylinder with R = 1
  // and directed along Z axis
  gp_Cylinder aCyl(gp::XOY(), 1.0);
  TopoDS_Face aShape = BRepBuilderAPI_MakeFace(aCyl, 0.0, M_PI, -1.0, +1.0).Face();
  myResult << "TopoDS_Face on the cylinder R = " << aCyl.Radius() << std::endl
           << "with axis [ " << aCyl.Position().Direction().X() << ", " << aCyl.Position().Direction().Y() << ", " << aCyl.Position().Direction().Z() << " ]" << std::endl
           << "limited in length [-1 ... +1] was created in red" << std::endl;

  // Retrieve surface properties from the face.
  GProp_GProps aGProps;
  BRepGProp::SurfaceProperties(aShape, aGProps);
  Standard_Real aArea = aGProps.Mass();
  gp_Pnt aCOM = aGProps.CentreOfMass();
  Standard_Real anIx, anIy, anIz;
  aGProps.StaticMoments(anIx, anIy, anIz);
  gp_Mat aMOI = aGProps.MatrixOfInertia();
  myResult << "Linear properties:" << std::endl
           << "  Area              = " << aArea << std::endl
           << "  Center of mass    = [ " << aCOM.X() << ", " << aCOM.Y() << ", " << aCOM.Z() << " ]" << std::endl
           << "  Static moments    = [ " << anIx << ", " << anIy << ", " << anIz << " ]" << std::endl
           << "  Matrix of inertia = [ "
           << aMOI(1, 1) << ", " << aMOI(1, 2) << ", " << aMOI(1, 3) << std::endl
           << std::setw(33) << aMOI(2, 1) << ", " << aMOI(2, 2) << ", " << aMOI(2, 3) << std::endl
           << std::setw(33) << aMOI(3, 1) << ", " << aMOI(3, 2) << ", " << aMOI(3, 3) << " ]" << std::endl;
  GProp_PrincipalProps aPProps = aGProps.PrincipalProperties();
  Standard_Real anIxx, anIyy, anIzz;
  aPProps.Moments(anIxx, anIyy, anIzz);
  Standard_Real aRxx, aRyy, aRzz;
  aPProps.RadiusOfGyration(aRxx, aRyy, aRzz);
  myResult << "Principal properties:" << std::endl
           << "  Has symmetric axis  : " << (aPProps.HasSymmetryAxis() ? "YES" : "NO") << std::endl
           << "  Has symmetric point : " << (aPProps.HasSymmetryPoint() ? "YES" : "NO") << std::endl
           << "  Moments of inertia  = [ " << anIxx << ", " << anIyy << ", " << anIzz << " ]" << std::endl
           << "  Radius of gyration  = [ " << aRxx << ", " << aRyy << ", " << aRzz << " ]" << std::endl;
  if (!aPProps.HasSymmetryPoint())
  {
    const gp_Vec& anAxis1 = aPProps.FirstAxisOfInertia();
    myResult << "  1st axis of inertia = [ " << anAxis1.X() << ", " << anAxis1.Y() << ", " << anAxis1.Z() << " ]" << std::endl;
    Handle(AIS_ColoredShape) anAisAxis1 = new AIS_ColoredShape(
      BRepBuilderAPI_MakeEdge(aCOM, aCOM.XYZ() + anAxis1.XYZ()));
    anAisAxis1->SetColor(Quantity_Color(Quantity_NOC_GREEN));
    myObject3d.Append(anAisAxis1);
    if (!aPProps.HasSymmetryPoint())
    {
      const gp_Vec& anAxis2 = aPProps.SecondAxisOfInertia();
      myResult << "  2nd axis of inertia = [ " << anAxis2.X() << ", " << anAxis2.Y() << ", " << anAxis2.Z() << " ]" << std::endl;
      Handle(AIS_ColoredShape) anAisAxis2 = new AIS_ColoredShape(
        BRepBuilderAPI_MakeEdge(aCOM, aCOM.XYZ() + anAxis2.XYZ()));
      anAisAxis2->SetColor(Quantity_Color(Quantity_NOC_GREEN));
      myObject3d.Append(anAisAxis2);
      const gp_Vec& anAxis3 = aPProps.ThirdAxisOfInertia();
      myResult << "  3rd axis of inertia = [ " << anAxis3.X() << ", " << anAxis3.Y() << ", " << anAxis3.Z() << " ]" << std::endl;
      Handle(AIS_ColoredShape) anAisAxis3 = new AIS_ColoredShape(
        BRepBuilderAPI_MakeEdge(aCOM, aCOM.XYZ() + anAxis3.XYZ()));
      anAisAxis3->SetColor(Quantity_Color(Quantity_NOC_GREEN));
      myObject3d.Append(anAisAxis3);
    }
  }

  Handle(AIS_ColoredShape) anAisShape = new AIS_ColoredShape(aShape);
  Handle(AIS_Point) anAisCOM = new AIS_Point(new Geom_CartesianPoint(aCOM));
  anAisShape->SetColor(Quantity_Color(Quantity_NOC_RED));
  Handle(AIS_TextLabel) aCOMLabel = new AIS_TextLabel();
  aCOMLabel->SetText("Center of mass");
  aCOMLabel->SetPosition(aCOM);
  Handle(AIS_Axis) anAisAxisX = new AIS_Axis(new Geom_Axis2Placement(gp::YOZ()), AIS_TOAX_XAxis);
  Handle(AIS_Axis) anAisAxisY = new AIS_Axis(new Geom_Axis2Placement(gp::ZOX()), AIS_TOAX_YAxis);
  Handle(AIS_Axis) anAisAxisZ = new AIS_Axis(new Geom_Axis2Placement(gp::XOY()), AIS_TOAX_ZAxis);
  myObject3d.Append(anAisAxisX);
  myObject3d.Append(anAisAxisY);
  myObject3d.Append(anAisAxisZ);
  myObject3d.Append(anAisShape);
  myObject3d.Append(anAisCOM);
  myObject3d.Append(aCOMLabel);
}

void TopologySamples::ComputeVolumeProperties3dSample()
{
  // Make a box by two points.
  gp_Pnt aPnt1(-0.5, -0.6, -0.7);
  gp_Pnt aPnt2(+0.8, +0.9, +1.0);
  TopoDS_Shape aShape = BRepPrimAPI_MakeBox(aPnt1, aPnt2);
  myResult << "Box with corners [" << aPnt1.X() << ", " << aPnt1.Y() << ", " << aPnt1.Z()
           << "] and [" << aPnt2.X() << ", " << aPnt2.Y() << ", " << aPnt2.Z()
           << "] was created in red" << std::endl;

  // Retrieve volume properties from the face.
  GProp_GProps aGProps;
  BRepGProp::VolumeProperties(aShape, aGProps);
  Standard_Real aVolume = aGProps.Mass();
  gp_Pnt aCOM = aGProps.CentreOfMass();
  Standard_Real anIx, anIy, anIz;
  aGProps.StaticMoments(anIx, anIy, anIz);
  gp_Mat aMOI = aGProps.MatrixOfInertia();
  myResult << "Linear properties:" << std::endl
           << "  Volume            = " << aVolume << std::endl
           << "  Center of mass    = [ " << aCOM.X() << ", " << aCOM.Y() << ", " << aCOM.Z() << " ]" << std::endl
           << "  Static moments    = [ " << anIx << ", " << anIy << ", " << anIz << " ]" << std::endl
           << "  Matrix of inertia = [ "
           << aMOI(1, 1) << ", " << aMOI(1, 2) << ", " << aMOI(1, 3) << std::endl
           << std::setw(33) << aMOI(2, 1) << ", " << aMOI(2, 2) << ", " << aMOI(2, 3) << std::endl
           << std::setw(33) << aMOI(3, 1) << ", " << aMOI(3, 2) << ", " << aMOI(3, 3) << " ]" << std::endl;
  GProp_PrincipalProps aPProps = aGProps.PrincipalProperties();
  Standard_Real anIxx, anIyy, anIzz;
  aPProps.Moments(anIxx, anIyy, anIzz);
  Standard_Real aRxx, aRyy, aRzz;
  aPProps.RadiusOfGyration(aRxx, aRyy, aRzz);
  myResult << "Principal properties:" << std::endl
           << "  Has symmetric axis  : " << (aPProps.HasSymmetryAxis() ? "YES" : "NO") << std::endl
           << "  Has symmetric point : " << (aPProps.HasSymmetryPoint() ? "YES" : "NO") << std::endl
           << "  Moments of inertia  = [ " << anIxx << ", " << anIyy << ", " << anIzz << " ]" << std::endl
           << "  Radius of gyration  = [ " << aRxx << ", " << aRyy << ", " << aRzz << " ]" << std::endl;
  if (!aPProps.HasSymmetryPoint())
  {
    const gp_Vec& anAxis1 = aPProps.FirstAxisOfInertia();
    myResult << "  1st axis of inertia = [ " << anAxis1.X() << ", " << anAxis1.Y() << ", " << anAxis1.Z() << " ]" << std::endl;
    Handle(AIS_ColoredShape) anAisAxis1 = new AIS_ColoredShape(
      BRepBuilderAPI_MakeEdge(aCOM, aCOM.XYZ() + anAxis1.XYZ()));
    anAisAxis1->SetColor(Quantity_Color(Quantity_NOC_GREEN));
    myObject3d.Append(anAisAxis1);
    if (!aPProps.HasSymmetryPoint())
    {
      const gp_Vec& anAxis2 = aPProps.SecondAxisOfInertia();
      myResult << "  2nd axis of inertia = [ " << anAxis2.X() << ", " << anAxis2.Y() << ", " << anAxis2.Z() << " ]" << std::endl;
      Handle(AIS_ColoredShape) anAisAxis2 = new AIS_ColoredShape(
        BRepBuilderAPI_MakeEdge(aCOM, aCOM.XYZ() + anAxis2.XYZ()));
      anAisAxis2->SetColor(Quantity_Color(Quantity_NOC_GREEN));
      myObject3d.Append(anAisAxis2);
      const gp_Vec& anAxis3 = aPProps.ThirdAxisOfInertia();
      myResult << "  3rd axis of inertia = [ " << anAxis3.X() << ", " << anAxis3.Y() << ", " << anAxis3.Z() << " ]" << std::endl;
      Handle(AIS_ColoredShape) anAisAxis3 = new AIS_ColoredShape(
        BRepBuilderAPI_MakeEdge(aCOM, aCOM.XYZ() + anAxis3.XYZ()));
      anAisAxis3->SetColor(Quantity_Color(Quantity_NOC_GREEN));
      myObject3d.Append(anAisAxis3);
    }
  }

  Handle(AIS_ColoredShape) anAisShape = new AIS_ColoredShape(aShape);
  Handle(AIS_Point) anAisCOM = new AIS_Point(new Geom_CartesianPoint(aCOM));
  anAisShape->SetColor(Quantity_Color(Quantity_NOC_RED));
  Handle(AIS_TextLabel) aCOMLabel = new AIS_TextLabel();
  aCOMLabel->SetText("Center of mass");
  aCOMLabel->SetPosition(aCOM);
  Handle(AIS_Axis) anAisAxisX = new AIS_Axis(new Geom_Axis2Placement(gp::YOZ()), AIS_TOAX_XAxis);
  Handle(AIS_Axis) anAisAxisY = new AIS_Axis(new Geom_Axis2Placement(gp::ZOX()), AIS_TOAX_YAxis);
  Handle(AIS_Axis) anAisAxisZ = new AIS_Axis(new Geom_Axis2Placement(gp::XOY()), AIS_TOAX_ZAxis);
  myObject3d.Append(anAisAxisX);
  myObject3d.Append(anAisAxisY);
  myObject3d.Append(anAisAxisZ);
  myObject3d.Append(anAisShape);
  myObject3d.Append(anAisCOM);
  myObject3d.Append(aCOMLabel);
}
