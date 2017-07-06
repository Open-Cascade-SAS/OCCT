// Created on: 1993-08-10
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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


#include <BRep_PointRepresentation.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Type.hxx>
#include <TopLoc_Location.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRep_PointRepresentation,Standard_Transient)

//=======================================================================
//function : BRep_PointRepresentation
//purpose  : 
//=======================================================================
BRep_PointRepresentation::BRep_PointRepresentation(const Standard_Real P,
						   const TopLoc_Location& L) :
       myLocation(L),
       myParameter(P)
{
}


//=======================================================================
//function : IsPointOnCurve
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_PointRepresentation::IsPointOnCurve()const 
{
  return Standard_False;
}


//=======================================================================
//function : IsPointOnCurveOnSurface
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_PointRepresentation::IsPointOnCurveOnSurface()const 
{
  return Standard_False;
}


//=======================================================================
//function : IsPointOnSurface
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_PointRepresentation::IsPointOnSurface()const 
{
  return Standard_False;
}


//=======================================================================
//function : IsPointOnCurve
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_PointRepresentation::IsPointOnCurve
  (const Handle(Geom_Curve)& , 
   const TopLoc_Location& )const 
{
  return Standard_False;
}


//=======================================================================
//function : IsPointOnCurveOnSurface
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_PointRepresentation::IsPointOnCurveOnSurface
  (const Handle(Geom2d_Curve)& ,
   const Handle(Geom_Surface)& ,
   const TopLoc_Location& )const 
{
  return Standard_False;
}


//=======================================================================
//function : IsPointOnSurface
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_PointRepresentation::IsPointOnSurface
  (const Handle(Geom_Surface)& ,
   const TopLoc_Location& )const 
{
  return Standard_False;
}


//=======================================================================
//function : Parameter2
//purpose  : 
//=======================================================================

Standard_Real  BRep_PointRepresentation::Parameter2()const 
{
  throw Standard_DomainError("BRep_PointRepresentation");
}


//=======================================================================
//function : Parameter2
//purpose  : 
//=======================================================================

void  BRep_PointRepresentation::Parameter2(const Standard_Real )
{
  throw Standard_DomainError("BRep_PointRepresentation");
}


//=======================================================================
//function : Curve
//purpose  : 
//=======================================================================

const Handle(Geom_Curve)&  BRep_PointRepresentation::Curve()const 
{
  throw Standard_DomainError("BRep_PointRepresentation");
}


//=======================================================================
//function : Curve
//purpose  : 
//=======================================================================

void  BRep_PointRepresentation::Curve(const Handle(Geom_Curve)& )
{
  throw Standard_DomainError("BRep_PointRepresentation");
}


//=======================================================================
//function : PCurve
//purpose  : 
//=======================================================================

const Handle(Geom2d_Curve)&  BRep_PointRepresentation::PCurve()const 
{
  throw Standard_DomainError("BRep_PointRepresentation");
}


//=======================================================================
//function : PCurve
//purpose  : 
//=======================================================================

void  BRep_PointRepresentation::PCurve(const Handle(Geom2d_Curve)& )
{
  throw Standard_DomainError("BRep_PointRepresentation");
}


//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================

const Handle(Geom_Surface)&  BRep_PointRepresentation::Surface()const 
{
  throw Standard_DomainError("BRep_PointRepresentation");
}


//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================

void  BRep_PointRepresentation::Surface(const Handle(Geom_Surface)& )
{
  throw Standard_DomainError("BRep_PointRepresentation");
}


