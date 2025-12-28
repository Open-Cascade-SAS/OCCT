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

GeomAPI_IntCS::GeomAPI_IntCS(const occ::handle<Geom_Curve>& C, const occ::handle<Geom_Surface>& S)
{
  Perform(C, S);
}

//=================================================================================================

void GeomAPI_IntCS::Perform(const occ::handle<Geom_Curve>& C, const occ::handle<Geom_Surface>& S)
{
  myCurve = C;

  occ::handle<GeomAdaptor_Curve>   HC = new GeomAdaptor_Curve(C);
  occ::handle<GeomAdaptor_Surface> HS = new GeomAdaptor_Surface(S);

  myIntCS.Perform(HC, HS);
}

//=================================================================================================

bool GeomAPI_IntCS::IsDone() const
{
  return myIntCS.IsDone();
}

//=================================================================================================

int GeomAPI_IntCS::NbPoints() const
{
  return myIntCS.NbPoints();
}

//=======================================================================
// function : gp_Pnt&
// purpose  :
//=======================================================================

const gp_Pnt& GeomAPI_IntCS::Point(const int Index) const
{
  return myIntCS.Point(Index).Pnt();
}

//=================================================================================================

void GeomAPI_IntCS::Parameters(const int Index, double& U, double& V, double& W) const
{
  const IntCurveSurface_IntersectionPoint& ThePoint = myIntCS.Point(Index);

  U = ThePoint.U();
  V = ThePoint.V();
  W = ThePoint.W();
}

//=================================================================================================

int GeomAPI_IntCS::NbSegments() const
{
  return myIntCS.NbSegments();
}

//=================================================================================================

occ::handle<Geom_Curve> GeomAPI_IntCS::Segment(const int Index) const
{
  const IntCurveSurface_IntersectionPoint& FirstPoint = myIntCS.Segment(Index).FirstPoint();

  const IntCurveSurface_IntersectionPoint& LastPoint = myIntCS.Segment(Index).SecondPoint();

  occ::handle<Geom_TrimmedCurve> TheCurve =
    new Geom_TrimmedCurve(myCurve, FirstPoint.W(), LastPoint.W());

  return TheCurve;
}

//=================================================================================================

void GeomAPI_IntCS::Parameters(const int Index,
                               double&   U1,
                               double&   V1,
                               double&   U2,
                               double&   V2) const
{
  const IntCurveSurface_IntersectionPoint& FirstPoint = myIntCS.Segment(Index).FirstPoint();

  const IntCurveSurface_IntersectionPoint& LastPoint = myIntCS.Segment(Index).SecondPoint();

  U1 = FirstPoint.U();
  V1 = FirstPoint.V();
  U2 = LastPoint.U();
  V2 = LastPoint.V();
}
