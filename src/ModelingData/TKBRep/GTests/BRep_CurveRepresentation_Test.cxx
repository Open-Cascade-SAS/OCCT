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

#include <BRep_Curve3D.hxx>
#include <BRep_CurveOn2Surfaces.hxx>
#include <BRep_CurveOnClosedSurface.hxx>
#include <BRep_CurveOnSurface.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_Polygon3D.hxx>
#include <BRep_PolygonOnClosedSurface.hxx>
#include <BRep_PolygonOnClosedTriangulation.hxx>
#include <BRep_PolygonOnSurface.hxx>
#include <BRep_PolygonOnTriangulation.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <GeomAbs_Shape.hxx>
#include <NCollection_Array1.hxx>
#include <Poly_Polygon2D.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <TopLoc_Location.hxx>
#include <gp_Pln.hxx>

// Helper objects for constructing test representations.
// All Is*() queries are called through the base class handle to avoid
// C++ name hiding by virtual overloads with parameters in derived classes.
namespace
{
Handle(Geom_Line)   TheLine3D() { return new Geom_Line(gp_Pnt(0., 0., 0.), gp_Dir(1., 0., 0.)); }
Handle(Geom2d_Line) TheLine2D() { return new Geom2d_Line(gp_Pnt2d(0., 0.), gp_Dir2d(1., 0.)); }
Handle(Geom_Plane)  ThePlane()  { return new Geom_Plane(gp_Pln()); }
TopLoc_Location     TheLoc()    { return TopLoc_Location(); }

Handle(Poly_Polygon3D) ThePoly3D()
{
  NCollection_Array1<gp_Pnt> aNodes(1, 2);
  aNodes.SetValue(1, gp_Pnt(0., 0., 0.));
  aNodes.SetValue(2, gp_Pnt(1., 0., 0.));
  return new Poly_Polygon3D(aNodes);
}

Handle(Poly_Polygon2D) ThePoly2D()
{
  NCollection_Array1<gp_Pnt2d> aNodes(1, 2);
  aNodes.SetValue(1, gp_Pnt2d(0., 0.));
  aNodes.SetValue(2, gp_Pnt2d(1., 0.));
  return new Poly_Polygon2D(aNodes);
}

Handle(Poly_Triangulation) TheTriangulation()
{
  return new Poly_Triangulation(1, 0, false);
}

Handle(Poly_PolygonOnTriangulation) ThePolyOnTri()
{
  NCollection_Array1<int> aNodes(1, 1);
  aNodes.SetValue(1, 1);
  return new Poly_PolygonOnTriangulation(aNodes);
}
} // namespace

TEST(BRep_CurveRepresentation_Test, Curve3D_EnumChecks)
{
  Handle(BRep_CurveRepresentation) aRep = new BRep_Curve3D(TheLine3D(), TheLoc());
  EXPECT_EQ(aRep->RepresentationKind(), BRep_CurveRepKind::Curve3D)
    << "RepresentationKind should be Curve3D";
  EXPECT_TRUE(aRep->IsCurve3D()) << "IsCurve3D should be true";
  EXPECT_FALSE(aRep->IsCurveOnSurface()) << "IsCurveOnSurface should be false";
  EXPECT_FALSE(aRep->IsRegularity()) << "IsRegularity should be false";
  EXPECT_FALSE(aRep->IsCurveOnClosedSurface()) << "IsCurveOnClosedSurface should be false";
  EXPECT_FALSE(aRep->IsPolygon3D()) << "IsPolygon3D should be false";
  EXPECT_FALSE(aRep->IsPolygonOnSurface()) << "IsPolygonOnSurface should be false";
  EXPECT_FALSE(aRep->IsPolygonOnClosedSurface()) << "IsPolygonOnClosedSurface should be false";
  EXPECT_FALSE(aRep->IsPolygonOnTriangulation()) << "IsPolygonOnTriangulation should be false";
  EXPECT_FALSE(aRep->IsPolygonOnClosedTriangulation())
    << "IsPolygonOnClosedTriangulation should be false";
}

TEST(BRep_CurveRepresentation_Test, CurveOnSurface_EnumChecks)
{
  Handle(BRep_CurveRepresentation) aRep =
    new BRep_CurveOnSurface(TheLine2D(), ThePlane(), TheLoc());
  EXPECT_EQ(aRep->RepresentationKind(), BRep_CurveRepKind::CurveOnSurface)
    << "RepresentationKind should be CurveOnSurface";
  EXPECT_TRUE(aRep->IsCurveOnSurface()) << "IsCurveOnSurface should be true";
  EXPECT_FALSE(aRep->IsCurve3D()) << "IsCurve3D should be false";
  EXPECT_FALSE(aRep->IsRegularity()) << "IsRegularity should be false";
  EXPECT_FALSE(aRep->IsCurveOnClosedSurface()) << "IsCurveOnClosedSurface should be false";
  EXPECT_FALSE(aRep->IsPolygon3D()) << "IsPolygon3D should be false";
  EXPECT_FALSE(aRep->IsPolygonOnSurface()) << "IsPolygonOnSurface should be false";
  EXPECT_FALSE(aRep->IsPolygonOnTriangulation()) << "IsPolygonOnTriangulation should be false";
}

TEST(BRep_CurveRepresentation_Test, CurveOnClosedSurface_EnumChecks)
{
  Handle(BRep_CurveRepresentation) aRep =
    new BRep_CurveOnClosedSurface(TheLine2D(), TheLine2D(), ThePlane(), TheLoc(), GeomAbs_C0);
  EXPECT_EQ(aRep->RepresentationKind(), BRep_CurveRepKind::CurveOnClosedSurface)
    << "RepresentationKind should be CurveOnClosedSurface";
  EXPECT_TRUE(aRep->IsCurveOnSurface()) << "IsCurveOnSurface should be true (parent check)";
  EXPECT_TRUE(aRep->IsCurveOnClosedSurface()) << "IsCurveOnClosedSurface should be true";
  EXPECT_TRUE(aRep->IsRegularity()) << "IsRegularity should be true";
  EXPECT_FALSE(aRep->IsCurve3D()) << "IsCurve3D should be false";
  EXPECT_FALSE(aRep->IsPolygon3D()) << "IsPolygon3D should be false";
  EXPECT_FALSE(aRep->IsPolygonOnSurface()) << "IsPolygonOnSurface should be false";
  EXPECT_FALSE(aRep->IsPolygonOnTriangulation()) << "IsPolygonOnTriangulation should be false";
}

TEST(BRep_CurveRepresentation_Test, CurveOn2Surfaces_EnumChecks)
{
  Handle(BRep_CurveRepresentation) aRep =
    new BRep_CurveOn2Surfaces(ThePlane(), ThePlane(), TheLoc(), TheLoc(), GeomAbs_C0);
  EXPECT_EQ(aRep->RepresentationKind(), BRep_CurveRepKind::CurveOn2Surfaces)
    << "RepresentationKind should be CurveOn2Surfaces";
  EXPECT_TRUE(aRep->IsRegularity()) << "IsRegularity should be true";
  EXPECT_FALSE(aRep->IsCurveOnSurface()) << "IsCurveOnSurface should be false";
  EXPECT_FALSE(aRep->IsCurveOnClosedSurface()) << "IsCurveOnClosedSurface should be false";
  EXPECT_FALSE(aRep->IsCurve3D()) << "IsCurve3D should be false";
  EXPECT_FALSE(aRep->IsPolygon3D()) << "IsPolygon3D should be false";
  EXPECT_FALSE(aRep->IsPolygonOnSurface()) << "IsPolygonOnSurface should be false";
  EXPECT_FALSE(aRep->IsPolygonOnTriangulation()) << "IsPolygonOnTriangulation should be false";
}

TEST(BRep_CurveRepresentation_Test, Polygon3D_EnumChecks)
{
  Handle(BRep_CurveRepresentation) aRep = new BRep_Polygon3D(ThePoly3D(), TheLoc());
  EXPECT_EQ(aRep->RepresentationKind(), BRep_CurveRepKind::Polygon3D)
    << "RepresentationKind should be Polygon3D";
  EXPECT_TRUE(aRep->IsPolygon3D()) << "IsPolygon3D should be true";
  EXPECT_FALSE(aRep->IsCurve3D()) << "IsCurve3D should be false";
  EXPECT_FALSE(aRep->IsCurveOnSurface()) << "IsCurveOnSurface should be false";
  EXPECT_FALSE(aRep->IsRegularity()) << "IsRegularity should be false";
  EXPECT_FALSE(aRep->IsPolygonOnSurface()) << "IsPolygonOnSurface should be false";
  EXPECT_FALSE(aRep->IsPolygonOnTriangulation()) << "IsPolygonOnTriangulation should be false";
}

TEST(BRep_CurveRepresentation_Test, PolygonOnSurface_EnumChecks)
{
  Handle(BRep_CurveRepresentation) aRep =
    new BRep_PolygonOnSurface(ThePoly2D(), ThePlane(), TheLoc());
  EXPECT_EQ(aRep->RepresentationKind(), BRep_CurveRepKind::PolygonOnSurface)
    << "RepresentationKind should be PolygonOnSurface";
  EXPECT_TRUE(aRep->IsPolygonOnSurface()) << "IsPolygonOnSurface should be true";
  EXPECT_FALSE(aRep->IsPolygonOnClosedSurface()) << "IsPolygonOnClosedSurface should be false";
  EXPECT_FALSE(aRep->IsCurve3D()) << "IsCurve3D should be false";
  EXPECT_FALSE(aRep->IsPolygon3D()) << "IsPolygon3D should be false";
  EXPECT_FALSE(aRep->IsPolygonOnTriangulation()) << "IsPolygonOnTriangulation should be false";
}

TEST(BRep_CurveRepresentation_Test, PolygonOnClosedSurface_EnumChecks)
{
  Handle(BRep_CurveRepresentation) aRep =
    new BRep_PolygonOnClosedSurface(ThePoly2D(), ThePoly2D(), ThePlane(), TheLoc());
  EXPECT_EQ(aRep->RepresentationKind(), BRep_CurveRepKind::PolygonOnClosedSurface)
    << "RepresentationKind should be PolygonOnClosedSurface";
  EXPECT_TRUE(aRep->IsPolygonOnSurface()) << "IsPolygonOnSurface should be true (parent check)";
  EXPECT_TRUE(aRep->IsPolygonOnClosedSurface()) << "IsPolygonOnClosedSurface should be true";
  EXPECT_FALSE(aRep->IsCurve3D()) << "IsCurve3D should be false";
  EXPECT_FALSE(aRep->IsPolygon3D()) << "IsPolygon3D should be false";
  EXPECT_FALSE(aRep->IsPolygonOnTriangulation()) << "IsPolygonOnTriangulation should be false";
}

TEST(BRep_CurveRepresentation_Test, PolygonOnTriangulation_EnumChecks)
{
  Handle(BRep_CurveRepresentation) aRep =
    new BRep_PolygonOnTriangulation(ThePolyOnTri(), TheTriangulation(), TheLoc());
  EXPECT_EQ(aRep->RepresentationKind(), BRep_CurveRepKind::PolygonOnTriangulation)
    << "RepresentationKind should be PolygonOnTriangulation";
  EXPECT_TRUE(aRep->IsPolygonOnTriangulation()) << "IsPolygonOnTriangulation should be true";
  EXPECT_FALSE(aRep->IsPolygonOnClosedTriangulation())
    << "IsPolygonOnClosedTriangulation should be false";
  EXPECT_FALSE(aRep->IsCurve3D()) << "IsCurve3D should be false";
  EXPECT_FALSE(aRep->IsPolygon3D()) << "IsPolygon3D should be false";
  EXPECT_FALSE(aRep->IsPolygonOnSurface()) << "IsPolygonOnSurface should be false";
  EXPECT_FALSE(aRep->IsCurveOnSurface()) << "IsCurveOnSurface should be false";
}

TEST(BRep_CurveRepresentation_Test, PolygonOnClosedTriangulation_EnumChecks)
{
  Handle(BRep_CurveRepresentation) aRep =
    new BRep_PolygonOnClosedTriangulation(ThePolyOnTri(), ThePolyOnTri(), TheTriangulation(), TheLoc());
  EXPECT_EQ(aRep->RepresentationKind(), BRep_CurveRepKind::PolygonOnClosedTriangulation)
    << "RepresentationKind should be PolygonOnClosedTriangulation";
  EXPECT_TRUE(aRep->IsPolygonOnTriangulation())
    << "IsPolygonOnTriangulation should be true (parent check)";
  EXPECT_TRUE(aRep->IsPolygonOnClosedTriangulation())
    << "IsPolygonOnClosedTriangulation should be true";
  EXPECT_FALSE(aRep->IsCurve3D()) << "IsCurve3D should be false";
  EXPECT_FALSE(aRep->IsPolygon3D()) << "IsPolygon3D should be false";
  EXPECT_FALSE(aRep->IsPolygonOnSurface()) << "IsPolygonOnSurface should be false";
  EXPECT_FALSE(aRep->IsCurveOnSurface()) << "IsCurveOnSurface should be false";
}
