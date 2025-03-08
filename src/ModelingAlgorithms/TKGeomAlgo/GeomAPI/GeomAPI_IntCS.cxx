// Created on: 1995-09-12
// Created by: Bruno DUMORTIER
// Copyright (c) 1995-1999 Matra Datavision
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

#include <Geom_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAPI_IntCS.hxx>
#include <gp_Pnt.hxx>
#include <IntCurveSurface_IntersectionPoint.hxx>
#include <IntCurveSurface_IntersectionSegment.hxx>

//=================================================================================================

GeomAPI_IntCS::GeomAPI_IntCS() {}

//=================================================================================================

GeomAPI_IntCS::GeomAPI_IntCS(const Handle(Geom_Curve)& C, const Handle(Geom_Surface)& S)
{
  Perform(C, S);
}

//=================================================================================================

void GeomAPI_IntCS::Perform(const Handle(Geom_Curve)& C, const Handle(Geom_Surface)& S)
{
  myCurve = C;

  Handle(GeomAdaptor_Curve)   HC = new GeomAdaptor_Curve(C);
  Handle(GeomAdaptor_Surface) HS = new GeomAdaptor_Surface(S);

  myIntCS.Perform(HC, HS);
}

//=================================================================================================

Standard_Boolean GeomAPI_IntCS::IsDone() const
{
  return myIntCS.IsDone();
}

//=================================================================================================

Standard_Integer GeomAPI_IntCS::NbPoints() const
{
  return myIntCS.NbPoints();
}

//=======================================================================
// function : gp_Pnt&
// purpose  :
//=======================================================================

const gp_Pnt& GeomAPI_IntCS::Point(const Standard_Integer Index) const
{
  return myIntCS.Point(Index).Pnt();
}

//=================================================================================================

void GeomAPI_IntCS::Parameters(const Standard_Integer Index,
                               Standard_Real&         U,
                               Standard_Real&         V,
                               Standard_Real&         W) const
{
  const IntCurveSurface_IntersectionPoint& ThePoint = myIntCS.Point(Index);

  U = ThePoint.U();
  V = ThePoint.V();
  W = ThePoint.W();
}

//=================================================================================================

Standard_Integer GeomAPI_IntCS::NbSegments() const
{
  return myIntCS.NbSegments();
}

//=================================================================================================

Handle(Geom_Curve) GeomAPI_IntCS::Segment(const Standard_Integer Index) const
{
  const IntCurveSurface_IntersectionPoint& FirstPoint = myIntCS.Segment(Index).FirstPoint();

  const IntCurveSurface_IntersectionPoint& LastPoint = myIntCS.Segment(Index).SecondPoint();

  Handle(Geom_TrimmedCurve) TheCurve =
    new Geom_TrimmedCurve(myCurve, FirstPoint.W(), LastPoint.W());

  return TheCurve;
}

//=================================================================================================

void GeomAPI_IntCS::Parameters(const Standard_Integer Index,
                               Standard_Real&         U1,
                               Standard_Real&         V1,
                               Standard_Real&         U2,
                               Standard_Real&         V2) const
{
  const IntCurveSurface_IntersectionPoint& FirstPoint = myIntCS.Segment(Index).FirstPoint();

  const IntCurveSurface_IntersectionPoint& LastPoint = myIntCS.Segment(Index).SecondPoint();

  U1 = FirstPoint.U();
  V1 = FirstPoint.V();
  U2 = LastPoint.U();
  V2 = LastPoint.V();
}
