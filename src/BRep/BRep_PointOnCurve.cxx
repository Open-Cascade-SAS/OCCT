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


#include <BRep_PointOnCurve.hxx>
#include <Geom_Curve.hxx>
#include <Standard_Type.hxx>
#include <TopLoc_Location.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRep_PointOnCurve,BRep_PointRepresentation)

//=======================================================================
//function : BRep_PointOnCurve
//purpose  : 
//=======================================================================
BRep_PointOnCurve::BRep_PointOnCurve(const Standard_Real P,
				     const Handle(Geom_Curve)& C, 
				     const TopLoc_Location& L) :
       BRep_PointRepresentation(P,L),
       myCurve(C)
{
}


//=======================================================================
//function : IsPointOnCurve
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_PointOnCurve::IsPointOnCurve()const 
{
  return Standard_True;
}


//=======================================================================
//function : IsPointOnCurve
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_PointOnCurve::IsPointOnCurve
  (const Handle(Geom_Curve)& C,
   const TopLoc_Location& L)const 
{
  return (myCurve == C) && (Location() == L);
}


//=======================================================================
//function : Curve
//purpose  : 
//=======================================================================

const Handle(Geom_Curve)&  BRep_PointOnCurve::Curve()const 
{
  return myCurve;
}


//=======================================================================
//function : Curve
//purpose  : 
//=======================================================================

void  BRep_PointOnCurve::Curve(const Handle(Geom_Curve)& C)
{
  myCurve = C;
}


