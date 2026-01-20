// Created on: 1996-01-12
// Created by: Denis PASCAL
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <BRep_Tool.hxx>
#include <DBRep_DrawableShape.hxx>
#include <Draw.hxx>
#include <Draw_Display.hxx>
#include <DrawDim.hxx>
#include <DrawDim_PlanarAngle.hxx>
#include <ElCLib.hxx>
#include <Geom2d_CartesianPoint.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAPI_InterCurveCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <GeomAPI.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <Standard_Type.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DrawDim_PlanarAngle, DrawDim_PlanarDimension)

//=================================================================================================

DrawDim_PlanarAngle::DrawDim_PlanarAngle(const TopoDS_Face&  face,
                                         const TopoDS_Shape& line1,
                                         const TopoDS_Shape& line2)
{
  myPlane    = face;
  myLine1    = line1;
  myLine2    = line2;
  myPosition = 100;
}

//=================================================================================================

DrawDim_PlanarAngle::DrawDim_PlanarAngle(const TopoDS_Shape& line1, const TopoDS_Shape& line2)
{
  myLine1    = line1;
  myLine2    = line2;
  myPosition = 100;
}

//=================================================================================================

void DrawDim_PlanarAngle::Sector(const bool reversed, const bool inverted)
{
  myIsReversed = reversed;
  myIsInverted = inverted;
}

//=================================================================================================

void DrawDim_PlanarAngle::Position(const double value)
{
  myPosition = value;
}

//=======================================================================
// function : DrawOn
// purpose  : line1^line2 suppose positifs
//=======================================================================

void DrawDim_PlanarAngle::DrawOn(Draw_Display& dis) const
{
  bool clockwise = myIsReversed;
  bool parallel  = !myIsInverted;
  // geometrie
  gp_Pln plane = occ::down_cast<Geom_Plane>(BRep_Tool::Surface(myPlane))->Pln();
  // if (plane.IsNull()) return;
  if (!(myLine1.ShapeType() == TopAbs_EDGE))
    return;
  if (!(myLine2.ShapeType() == TopAbs_EDGE))
    return;
  double      s1, e1, s2, e2;
  occ::handle<Geom_Curve> curve1 = BRep_Tool::Curve(TopoDS::Edge(myLine1), s1, e1);
  occ::handle<Geom_Curve> curve2 = BRep_Tool::Curve(TopoDS::Edge(myLine2), s2, e2);
  if (!curve1->IsKind(STANDARD_TYPE(Geom_Line)) || !curve2->IsKind(STANDARD_TYPE(Geom_Line)))
    return;
  occ::handle<Geom2d_Geometry> L1 = GeomAPI::To2d(curve1, plane);
  if (L1->IsInstance(STANDARD_TYPE(Geom2d_TrimmedCurve)))
  {
    L1 = occ::down_cast<Geom2d_TrimmedCurve>(L1)->BasisCurve();
  }
  gp_Lin2d                l1 = occ::down_cast<Geom2d_Line>(L1)->Lin2d();
  occ::handle<Geom2d_Geometry> L2 = GeomAPI::To2d(curve2, plane);
  if (L2->IsInstance(STANDARD_TYPE(Geom2d_TrimmedCurve)))
  {
    L2 = occ::down_cast<Geom2d_TrimmedCurve>(L2)->BasisCurve();
  }
  gp_Lin2d l2 = occ::down_cast<Geom2d_Line>(L2)->Lin2d();
  //
  IntAna2d_AnaIntersection inter;
  inter.Perform(l1, l2);
  if (!inter.IsDone() || !inter.NbPoints())
    return;
  gp_Pnt2d pinter = inter.Point(1).Value();
  //
  double angle;
  angle = std::abs(l1.Direction().Angle(l2.Direction()));
  gp_Circ2d c(gp_Ax2d(pinter, l1.Direction()), myPosition);

  // retour au plan
  occ::handle<Geom_Curve> C      = GeomAPI::To3d(new Geom2d_Circle(c), plane);
  gp_Circ            circle = occ::down_cast<Geom_Circle>(C)->Circ();
  //
  double p1 = 0., p2 = 0.;
  angle = std::abs(angle);
  if (parallel && !clockwise)
  {
    p1 = 0.0;
    p2 = angle;
    dis.Draw(circle, 0.0, angle);
  }
  if (!parallel && !clockwise)
  {
    p1 = angle;
    p2 = M_PI;
  }
  if (parallel && clockwise)
  {
    p1 = M_PI;
    p2 = M_PI + angle;
  }
  if (!parallel && clockwise)
  {
    p1 = M_PI + angle;
    p2 = 2 * M_PI;
  }
  // affichage
  dis.Draw(circle, p1, p2);
  double ptext   = (p1 + p2) / 2;
  gp_Pnt        pnttext = ElCLib::Value(ptext, circle);
  //
  DrawText(pnttext, dis);
}
