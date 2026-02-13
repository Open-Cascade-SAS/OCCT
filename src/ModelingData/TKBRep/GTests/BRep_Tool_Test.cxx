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

#include <BRep_Builder.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_TFace.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>

TEST(BRep_Tool_Test, Curve_ReturnsValidCurve)
{
  TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(gp_Pnt(0., 0., 0.), gp_Pnt(1., 0., 0.));
  TopLoc_Location aLoc;
  double          aFirst = 0.0, aLast = 0.0;
  const Handle(Geom_Curve)& aCurve = BRep_Tool::Curve(anEdge, aLoc, aFirst, aLast);
  EXPECT_FALSE(aCurve.IsNull()) << "Edge from MakeEdge should have a 3D curve";
  EXPECT_LT(aFirst, aLast) << "First parameter should be less than Last";
}

TEST(BRep_Tool_Test, Curve_NullForDegenerateEdge)
{
  BRep_Builder aBuilder;
  TopoDS_Edge  anEdge;
  aBuilder.MakeEdge(anEdge);
  aBuilder.Degenerated(anEdge, true);

  TopLoc_Location aLoc;
  double          aFirst = 0.0, aLast = 0.0;
  const Handle(Geom_Curve)& aCurve = BRep_Tool::Curve(anEdge, aLoc, aFirst, aLast);
  EXPECT_TRUE(aCurve.IsNull()) << "Edge without 3D curve should return null";
}

TEST(BRep_Tool_Test, Curve_LocationComposition)
{
  // Create an edge
  TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(gp_Pnt(0., 0., 0.), gp_Pnt(1., 0., 0.));

  // Move it with a non-identity location
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(10., 20., 30.));
  anEdge.Move(aTrsf);

  TopLoc_Location aLoc;
  double          aFirst = 0.0, aLast = 0.0;
  const Handle(Geom_Curve)& aCurve = BRep_Tool::Curve(anEdge, aLoc, aFirst, aLast);
  EXPECT_FALSE(aCurve.IsNull()) << "Moved edge should still have a curve";
  EXPECT_FALSE(aLoc.IsIdentity()) << "Location should be non-identity after move";
}

TEST(BRep_Tool_Test, Polygon3D_ReturnsNullForNewEdge)
{
  TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(gp_Pnt(0., 0., 0.), gp_Pnt(1., 0., 0.));
  TopLoc_Location                aLoc;
  const Handle(Poly_Polygon3D)& aPoly = BRep_Tool::Polygon3D(anEdge, aLoc);
  EXPECT_TRUE(aPoly.IsNull()) << "Fresh edge should have no polygon3D";
}

TEST(BRep_Tool_Test, IsGeometric_TrueForEdgeWithCurve)
{
  TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(gp_Pnt(0., 0., 0.), gp_Pnt(1., 0., 0.));
  EXPECT_TRUE(BRep_Tool::IsGeometric(anEdge)) << "Edge with 3D curve should be geometric";
}

TEST(BRep_Tool_Test, IsGeometric_FalseForEmptyEdge)
{
  BRep_Builder aBuilder;
  TopoDS_Edge  anEdge;
  aBuilder.MakeEdge(anEdge);
  EXPECT_FALSE(BRep_Tool::IsGeometric(anEdge)) << "Edge with no representations should not be geometric";
}

TEST(BRep_Tool_Test, Range_MatchesCurveParameters)
{
  Handle(Geom_Line) aLine = new Geom_Line(gp_Pnt(0., 0., 0.), gp_Dir(1., 0., 0.));
  TopoDS_Edge       anEdge = BRepBuilderAPI_MakeEdge(aLine, 2.0, 7.0);
  double            aFirst = 0.0, aLast = 0.0;
  BRep_Tool::Range(anEdge, aFirst, aLast);
  EXPECT_NEAR(aFirst, 2.0, Precision::Confusion()) << "First parameter should match";
  EXPECT_NEAR(aLast, 7.0, Precision::Confusion()) << "Last parameter should match";
}

TEST(BRep_Tool_Test, Tolerance_Edge_DefaultValue)
{
  BRep_Builder aBuilder;
  TopoDS_Edge  anEdge;
  aBuilder.MakeEdge(anEdge);
  const double aTol = BRep_Tool::Tolerance(anEdge);
  EXPECT_GE(aTol, Precision::Confusion()) << "Default edge tolerance should be >= Precision::Confusion()";
}

TEST(BRep_Tool_Test, Tolerance_Edge_SetAndGet)
{
  BRep_Builder aBuilder;
  TopoDS_Edge  anEdge;
  aBuilder.MakeEdge(anEdge);
  aBuilder.UpdateEdge(anEdge, 0.5);
  const double aTol = BRep_Tool::Tolerance(anEdge);
  EXPECT_NEAR(aTol, 0.5, 1e-15) << "Tolerance should match the set value";
}

TEST(BRep_Tool_Test, Tolerance_Edge_ClampedToConfusion)
{
  BRep_Builder aBuilder;
  TopoDS_Edge  anEdge;
  aBuilder.MakeEdge(anEdge);
  aBuilder.UpdateEdge(anEdge, 1e-20);
  const double aTol = BRep_Tool::Tolerance(anEdge);
  EXPECT_GE(aTol, Precision::Confusion()) << "Tolerance should be clamped to Precision::Confusion()";
}

TEST(BRep_Tool_Test, Tolerance_Face_DefaultValue)
{
  BRep_Builder aBuilder;
  TopoDS_Face  aFace;
  aBuilder.MakeFace(aFace);
  const double aTol = BRep_Tool::Tolerance(aFace);
  EXPECT_GE(aTol, Precision::Confusion()) << "Default face tolerance should be >= Precision::Confusion()";
}

TEST(BRep_Tool_Test, Tolerance_Face_ClampedToConfusion)
{
  BRep_Builder aBuilder;
  TopoDS_Face  aFace;
  aBuilder.MakeFace(aFace);
  aBuilder.UpdateFace(aFace, 1e-20);
  const double aTol = BRep_Tool::Tolerance(aFace);
  EXPECT_GE(aTol, Precision::Confusion()) << "Face tolerance should be clamped to Precision::Confusion()";
}

TEST(BRep_Tool_Test, Tolerance_Vertex_DefaultValue)
{
  BRep_Builder  aBuilder;
  TopoDS_Vertex aVertex;
  aBuilder.MakeVertex(aVertex, gp_Pnt(0., 0., 0.), Precision::Confusion());
  const double aTol = BRep_Tool::Tolerance(aVertex);
  EXPECT_GE(aTol, Precision::Confusion())
    << "Default vertex tolerance should be >= Precision::Confusion()";
}

TEST(BRep_Tool_Test, Tolerance_Vertex_ClampedToConfusion)
{
  BRep_Builder  aBuilder;
  TopoDS_Vertex aVertex;
  aBuilder.MakeVertex(aVertex, gp_Pnt(0., 0., 0.), 1e-20);
  const double aTol = BRep_Tool::Tolerance(aVertex);
  EXPECT_GE(aTol, Precision::Confusion())
    << "Vertex tolerance should be clamped to Precision::Confusion()";
}

TEST(BRep_Tool_Test, IsClosed_PlaneSkipsParametricCheck)
{
  // Create a planar face with a simple edge - planes are never periodic
  TopoDS_Shape aBox  = BRepPrimAPI_MakeBox(1.0, 1.0, 1.0).Shape();
  TopoDS_Face  aFace;
  for (TopExp_Explorer anExp(aBox, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    aFace = TopoDS::Face(anExp.Current());
    break;
  }
  // Get an edge on this planar face
  TopoDS_Edge anEdge;
  for (TopExp_Explorer anExp(aFace, TopAbs_EDGE); anExp.More(); anExp.Next())
  {
    anEdge = TopoDS::Edge(anExp.Current());
    break;
  }
  // On a plane, edges are never closed (plane is not periodic)
  EXPECT_FALSE(BRep_Tool::IsClosed(anEdge, aFace))
    << "Edge on planar face should not be closed (plane is not periodic)";
}

TEST(BRep_Tool_Test, IsClosed_CylindricalFace)
{
  // Create a cylinder - the seam edge should be closed on the cylindrical face
  TopoDS_Shape aCylinder = BRepPrimAPI_MakeCylinder(1.0, 2.0).Shape();
  bool         aFoundClosed = false;
  for (TopExp_Explorer aFaceExp(aCylinder, TopAbs_FACE); aFaceExp.More(); aFaceExp.Next())
  {
    const TopoDS_Face& aFace = TopoDS::Face(aFaceExp.Current());
    TopLoc_Location    aLoc;
    const Handle(Geom_Surface)& aSurf = BRep_Tool::Surface(aFace, aLoc);
    if (aSurf.IsNull())
    {
      continue;
    }
    // Check only the cylindrical face
    if (Handle(Geom_CylindricalSurface)::DownCast(aSurf).IsNull())
    {
      continue;
    }
    for (TopExp_Explorer anEdgeExp(aFace, TopAbs_EDGE); anEdgeExp.More(); anEdgeExp.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgeExp.Current());
      if (BRep_Tool::IsClosed(anEdge, aFace))
      {
        aFoundClosed = true;
        break;
      }
    }
    if (aFoundClosed)
    {
      break;
    }
  }
  EXPECT_TRUE(aFoundClosed) << "Cylinder should have a seam edge that is closed on the cylindrical face";
}
