// Created on: 1995-09-12
// Created by: Bruno DUMORTIER
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <GeomAPI_IntCS.ixx>

#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <IntCurveSurface_IntersectionPoint.hxx>
#include <IntCurveSurface_IntersectionSegment.hxx>

//=======================================================================
//function : GeomAPI_IntCS
//purpose  : 
//=======================================================================

GeomAPI_IntCS::GeomAPI_IntCS()
{
}


//=======================================================================
//function : GeomAPI_IntCS
//purpose  : 
//=======================================================================

GeomAPI_IntCS::GeomAPI_IntCS(const Handle(Geom_Curve)&   C, 
			     const Handle(Geom_Surface)& S)
{
  Perform(C, S);
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void GeomAPI_IntCS::Perform(const Handle(Geom_Curve)&   C,
			    const Handle(Geom_Surface)& S)
{
  myCurve = C;

  Handle(GeomAdaptor_HCurve) HC = 
    new GeomAdaptor_HCurve(C);
  Handle(GeomAdaptor_HSurface) HS = 
    new GeomAdaptor_HSurface(S);

  myIntCS.Perform(HC, HS);
}


//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean GeomAPI_IntCS::IsDone() const 
{
  return myIntCS.IsDone();
}


//=======================================================================
//function : NbPoints
//purpose  : 
//=======================================================================

Standard_Integer GeomAPI_IntCS::NbPoints() const 
{
  return myIntCS.NbPoints();
}


//=======================================================================
//function : gp_Pnt&
//purpose  : 
//=======================================================================

const gp_Pnt& GeomAPI_IntCS::Point(const Standard_Integer Index) const 
{
  return myIntCS.Point(Index).Pnt();
}


//=======================================================================
//function : Parameters
//purpose  : 
//=======================================================================

void GeomAPI_IntCS::Parameters(const Standard_Integer Index,
			       Quantity_Parameter& U, 
			       Quantity_Parameter& V, 
			       Quantity_Parameter& W) const 
{
  const IntCurveSurface_IntersectionPoint& ThePoint = 
    myIntCS.Point(Index);

  U = ThePoint.U();
  V = ThePoint.V();
  W = ThePoint.W();
}


//=======================================================================
//function : NbSegments
//purpose  : 
//=======================================================================

Standard_Integer GeomAPI_IntCS::NbSegments() const 
{
  return myIntCS.NbSegments();
}


//=======================================================================
//function : Segment
//purpose  : 
//=======================================================================

Handle(Geom_Curve) GeomAPI_IntCS::Segment(const Standard_Integer Index) const 
{
  const IntCurveSurface_IntersectionPoint& FirstPoint = 
    myIntCS.Segment(Index).FirstPoint();

  const IntCurveSurface_IntersectionPoint& LastPoint = 
    myIntCS.Segment(Index).SecondPoint();

  Handle(Geom_TrimmedCurve) TheCurve  = 
    new Geom_TrimmedCurve( myCurve, FirstPoint.W(), LastPoint.W());
  
  return TheCurve;
}


//=======================================================================
//function : Parameters
//purpose  : 
//=======================================================================

void GeomAPI_IntCS::Parameters(const Standard_Integer Index,
			       Quantity_Parameter& U1,
			       Quantity_Parameter& V1,
			       Quantity_Parameter& U2,
			       Quantity_Parameter& V2) const 
{
  const IntCurveSurface_IntersectionPoint& FirstPoint = 
    myIntCS.Segment(Index).FirstPoint();

  const IntCurveSurface_IntersectionPoint& LastPoint = 
    myIntCS.Segment(Index).SecondPoint();

  U1 = FirstPoint.U();
  V1 = FirstPoint.V();
  U2 =  LastPoint.U();
  V2 =  LastPoint.V();
}
