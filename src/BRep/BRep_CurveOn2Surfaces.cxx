// Created on: 1993-07-06
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


#include <BRep_CurveOn2Surfaces.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <Geom_Surface.hxx>
#include <gp_Pnt.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_Type.hxx>
#include <TopLoc_Location.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRep_CurveOn2Surfaces,BRep_CurveRepresentation)

//=======================================================================
//function : BRep_CurveOn2Surfaces
//purpose  : 
//=======================================================================
BRep_CurveOn2Surfaces::BRep_CurveOn2Surfaces(const Handle(Geom_Surface)& S1,
					     const Handle(Geom_Surface)& S2,
					     const TopLoc_Location& L1, 
					     const TopLoc_Location& L2,
					     const GeomAbs_Shape C) :
       BRep_CurveRepresentation(L1),
       mySurface(S1),
       mySurface2(S2),
       myLocation2(L2),
       myContinuity(C)
{
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void  BRep_CurveOn2Surfaces::D0(const Standard_Real , gp_Pnt& )const 
{
  throw Standard_NullObject("BRep_CurveOn2Surfaces::D0");
}


//=======================================================================
//function : IsRegularity
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_CurveOn2Surfaces::IsRegularity()const 
{
  return Standard_True;
}


//=======================================================================
//function : IsRegularity
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_CurveOn2Surfaces::IsRegularity
  (const Handle(Geom_Surface)& S1, 
   const Handle(Geom_Surface)& S2, 
   const TopLoc_Location& L1, 
   const TopLoc_Location& L2)const 
{
  return ((mySurface  == S1 && mySurface2  == S2 &&
	   myLocation == L1 && myLocation2 == L2) ||
	  (mySurface  == S2 && mySurface2  == S1 &&
	   myLocation == L2 && myLocation2 == L1));
}


//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================

const Handle(Geom_Surface)&  BRep_CurveOn2Surfaces::Surface()const 
{
  return mySurface;
}

//=======================================================================
//function : Surface2
//purpose  : 
//=======================================================================

const Handle(Geom_Surface)&  BRep_CurveOn2Surfaces::Surface2()const 
{
  return mySurface2;
}


//=======================================================================
//function : Location2
//purpose  : 
//=======================================================================

const TopLoc_Location&  BRep_CurveOn2Surfaces::Location2()const 
{
  return myLocation2;
}

//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

const GeomAbs_Shape&  BRep_CurveOn2Surfaces::Continuity()const 
{
  return myContinuity;
}

//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

void BRep_CurveOn2Surfaces::Continuity(const GeomAbs_Shape C)
{
  myContinuity = C;
}


//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(BRep_CurveRepresentation) BRep_CurveOn2Surfaces::Copy() const 
{
  Handle(BRep_CurveOn2Surfaces) C = new
    BRep_CurveOn2Surfaces(Surface(),Surface2(),
			  Location(),Location2(),
			  myContinuity);
  return C;
}
