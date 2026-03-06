// Copyright (c) 2026 OPEN CASCADE SAS
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
#include <Poly_Polygon2D.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <TopLoc_Location.hxx>

#include <gtest/gtest.h>

using TypeEnum = BRep_CurveRepresentation::TypeEnum;

namespace
{
class BRep_CurveRepresentation_CustomOther : public BRep_CurveRepresentation
{
public:
  BRep_CurveRepresentation_CustomOther()
      : BRep_CurveRepresentation(Type_Other, TopLoc_Location())
  {
  }

  occ::handle<BRep_CurveRepresentation> Copy() const override
  {
    return new BRep_CurveRepresentation_CustomOther();
  }

  DEFINE_STANDARD_RTTIEXT(BRep_CurveRepresentation_CustomOther, BRep_CurveRepresentation)
};
} // namespace

IMPLEMENT_STANDARD_RTTIEXT(BRep_CurveRepresentation_CustomOther, BRep_CurveRepresentation)

TEST(BRep_CurveRepresentationTest, Curve3D_Type)
{
  occ::handle<Geom_Line>    aLine  = new Geom_Line(gp_Pnt(), gp_Dir(1, 0, 0));
  occ::handle<BRep_Curve3D> aCurve = new BRep_Curve3D(aLine, TopLoc_Location());
  EXPECT_EQ(aCurve->Type(), TypeEnum::Type_Curve3D);
  EXPECT_TRUE(aCurve->IsCurve3D());
  EXPECT_FALSE(aCurve->IsCurveOnSurface());
  EXPECT_FALSE(aCurve->IsRegularity());
}

TEST(BRep_CurveRepresentationTest, CurveOnSurface_Type)
{
  occ::handle<Geom2d_Line>         aPC    = new Geom2d_Line(gp_Pnt2d(), gp_Dir2d(1, 0));
  occ::handle<Geom_Plane>          aPlane = new Geom_Plane(gp::XOY());
  occ::handle<BRep_CurveOnSurface> aCOS   = new BRep_CurveOnSurface(aPC, aPlane, TopLoc_Location());
  EXPECT_EQ(aCOS->Type(), TypeEnum::Type_CurveOnSurface);
  EXPECT_TRUE(aCOS->IsCurveOnSurface());
  EXPECT_FALSE(aCOS->IsCurveOnClosedSurface());
  EXPECT_FALSE(aCOS->IsRegularity());
}

TEST(BRep_CurveRepresentationTest, CurveOnClosedSurface_Type)
{
  occ::handle<Geom2d_Line>               aPC1   = new Geom2d_Line(gp_Pnt2d(), gp_Dir2d(1, 0));
  occ::handle<Geom2d_Line>               aPC2   = new Geom2d_Line(gp_Pnt2d(), gp_Dir2d(0, 1));
  occ::handle<Geom_Plane>                aPlane = new Geom_Plane(gp::XOY());
  occ::handle<BRep_CurveOnClosedSurface> aCOCS =
    new BRep_CurveOnClosedSurface(aPC1, aPC2, aPlane, TopLoc_Location(), GeomAbs_C0);
  EXPECT_EQ(aCOCS->Type(), TypeEnum::Type_CurveOnClosedSurface);
  EXPECT_TRUE(aCOCS->IsCurveOnSurface());
  EXPECT_TRUE(aCOCS->IsCurveOnClosedSurface());
  EXPECT_TRUE(aCOCS->IsRegularity());
}

TEST(BRep_CurveRepresentationTest, CurveOn2Surfaces_Type)
{
  occ::handle<Geom_Plane>            aP1 = new Geom_Plane(gp::XOY());
  occ::handle<Geom_Plane>            aP2 = new Geom_Plane(gp::ZOX());
  occ::handle<BRep_CurveOn2Surfaces> aC2S =
    new BRep_CurveOn2Surfaces(aP1, aP2, TopLoc_Location(), TopLoc_Location(), GeomAbs_C0);
  EXPECT_EQ(aC2S->Type(), TypeEnum::Type_CurveOn2Surfaces);
  EXPECT_FALSE(aC2S->IsCurveOnSurface());
  EXPECT_TRUE(aC2S->IsRegularity());
}

TEST(BRep_CurveRepresentationTest, Polygon3D_Type)
{
  NCollection_Array1<gp_Pnt> aNodes(1, 2);
  aNodes(1)                         = gp_Pnt(0, 0, 0);
  aNodes(2)                         = gp_Pnt(1, 0, 0);
  occ::handle<Poly_Polygon3D> aPoly = new Poly_Polygon3D(aNodes);
  occ::handle<BRep_Polygon3D> aRep  = new BRep_Polygon3D(aPoly, TopLoc_Location());
  EXPECT_EQ(aRep->Type(), TypeEnum::Type_Polygon3D);
  EXPECT_TRUE(aRep->IsPolygon3D());
  EXPECT_FALSE(aRep->IsPolygonOnSurface());
  EXPECT_FALSE(aRep->IsPolygonOnTriangulation());
}

TEST(BRep_CurveRepresentationTest, PolygonOnSurface_Type)
{
  NCollection_Array1<gp_Pnt2d> aNodes(1, 2);
  aNodes(1)                                 = gp_Pnt2d(0, 0);
  aNodes(2)                                 = gp_Pnt2d(1, 0);
  occ::handle<Poly_Polygon2D>        aPoly  = new Poly_Polygon2D(aNodes);
  occ::handle<Geom_Plane>            aPlane = new Geom_Plane(gp::XOY());
  occ::handle<BRep_PolygonOnSurface> aRep =
    new BRep_PolygonOnSurface(aPoly, aPlane, TopLoc_Location());
  EXPECT_EQ(aRep->Type(), TypeEnum::Type_PolygonOnSurface);
  EXPECT_TRUE(aRep->IsPolygonOnSurface());
  EXPECT_FALSE(aRep->IsPolygonOnClosedSurface());
  EXPECT_FALSE(aRep->IsPolygonOnTriangulation());
}

TEST(BRep_CurveRepresentationTest, PolygonOnClosedSurface_Type)
{
  NCollection_Array1<gp_Pnt2d> aNodes(1, 2);
  aNodes(1)                                       = gp_Pnt2d(0, 0);
  aNodes(2)                                       = gp_Pnt2d(1, 0);
  occ::handle<Poly_Polygon2D>              aP1    = new Poly_Polygon2D(aNodes);
  occ::handle<Poly_Polygon2D>              aP2    = new Poly_Polygon2D(aNodes);
  occ::handle<Geom_Plane>                  aPlane = new Geom_Plane(gp::XOY());
  occ::handle<BRep_PolygonOnClosedSurface> aRep =
    new BRep_PolygonOnClosedSurface(aP1, aP2, aPlane, TopLoc_Location());
  EXPECT_EQ(aRep->Type(), TypeEnum::Type_PolygonOnClosedSurface);
  EXPECT_TRUE(aRep->IsPolygonOnSurface());
  EXPECT_TRUE(aRep->IsPolygonOnClosedSurface());
  EXPECT_FALSE(aRep->IsPolygonOnTriangulation());
}

TEST(BRep_CurveRepresentationTest, PolygonOnTriangulation_Type)
{
  NCollection_Array1<int> aNodes(1, 2);
  aNodes(1)                                      = 1;
  aNodes(2)                                      = 2;
  occ::handle<Poly_PolygonOnTriangulation> aPoly = new Poly_PolygonOnTriangulation(aNodes);
  occ::handle<Poly_Triangulation>          aTri  = new Poly_Triangulation(2, 1, false);
  occ::handle<BRep_PolygonOnTriangulation> aRep =
    new BRep_PolygonOnTriangulation(aPoly, aTri, TopLoc_Location());
  EXPECT_EQ(aRep->Type(), TypeEnum::Type_PolygonOnTriangulation);
  EXPECT_TRUE(aRep->IsPolygonOnTriangulation());
  EXPECT_FALSE(aRep->IsPolygonOnClosedTriangulation());
  EXPECT_FALSE(aRep->IsPolygonOnSurface());
}

TEST(BRep_CurveRepresentationTest, PolygonOnClosedTriangulation_Type)
{
  NCollection_Array1<int> aNodes(1, 2);
  aNodes(1)                                           = 1;
  aNodes(2)                                           = 2;
  occ::handle<Poly_PolygonOnTriangulation>       aP1  = new Poly_PolygonOnTriangulation(aNodes);
  occ::handle<Poly_PolygonOnTriangulation>       aP2  = new Poly_PolygonOnTriangulation(aNodes);
  occ::handle<Poly_Triangulation>                aTri = new Poly_Triangulation(2, 1, false);
  occ::handle<BRep_PolygonOnClosedTriangulation> aRep =
    new BRep_PolygonOnClosedTriangulation(aP1, aP2, aTri, TopLoc_Location());
  EXPECT_EQ(aRep->Type(), TypeEnum::Type_PolygonOnClosedTriangulation);
  EXPECT_TRUE(aRep->IsPolygonOnTriangulation());
  EXPECT_TRUE(aRep->IsPolygonOnClosedTriangulation());
  EXPECT_FALSE(aRep->IsPolygonOnSurface());
}

TEST(BRep_CurveRepresentationTest, OtherType_DefaultPredicates)
{
  occ::handle<BRep_CurveRepresentation> aRep = new BRep_CurveRepresentation_CustomOther();
  EXPECT_EQ(aRep->Type(), TypeEnum::Type_Other);
  EXPECT_FALSE(aRep->IsCurve3D());
  EXPECT_FALSE(aRep->IsCurveOnSurface());
  EXPECT_FALSE(aRep->IsCurveOnClosedSurface());
  EXPECT_FALSE(aRep->IsRegularity());
  EXPECT_FALSE(aRep->IsPolygon3D());
  EXPECT_FALSE(aRep->IsPolygonOnSurface());
  EXPECT_FALSE(aRep->IsPolygonOnClosedSurface());
  EXPECT_FALSE(aRep->IsPolygonOnTriangulation());
  EXPECT_FALSE(aRep->IsPolygonOnClosedTriangulation());
}
