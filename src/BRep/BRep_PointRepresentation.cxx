// Created on: 1993-08-10
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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



#include <BRep_PointRepresentation.ixx>

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
  Standard_DomainError::Raise("BRep_PointRepresentation");
    return 0;
}


//=======================================================================
//function : Parameter2
//purpose  : 
//=======================================================================

void  BRep_PointRepresentation::Parameter2(const Standard_Real )
{
  Standard_DomainError::Raise("BRep_PointRepresentation");
}


//=======================================================================
//function : Curve
//purpose  : 
//=======================================================================

const Handle(Geom_Curve)&  BRep_PointRepresentation::Curve()const 
{
  Standard_DomainError::Raise("BRep_PointRepresentation");
    return *((Handle(Geom_Curve)*) NULL);
}


//=======================================================================
//function : Curve
//purpose  : 
//=======================================================================

void  BRep_PointRepresentation::Curve(const Handle(Geom_Curve)& )
{
  Standard_DomainError::Raise("BRep_PointRepresentation");
}


//=======================================================================
//function : PCurve
//purpose  : 
//=======================================================================

const Handle(Geom2d_Curve)&  BRep_PointRepresentation::PCurve()const 
{
  Standard_DomainError::Raise("BRep_PointRepresentation");
    return *((Handle(Geom2d_Curve)*) NULL);
}


//=======================================================================
//function : PCurve
//purpose  : 
//=======================================================================

void  BRep_PointRepresentation::PCurve(const Handle(Geom2d_Curve)& )
{
  Standard_DomainError::Raise("BRep_PointRepresentation");
}


//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================

const Handle(Geom_Surface)&  BRep_PointRepresentation::Surface()const 
{
  Standard_DomainError::Raise("BRep_PointRepresentation");
    return *((Handle(Geom_Surface)*) NULL);
}


//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================

void  BRep_PointRepresentation::Surface(const Handle(Geom_Surface)& )
{
  Standard_DomainError::Raise("BRep_PointRepresentation");
}


