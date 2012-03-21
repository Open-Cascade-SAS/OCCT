// Created on: 1993-07-06
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



#include <BRep_CurveOn2Surfaces.ixx>


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
  Standard_NullObject::Raise("BRep_CurveOn2Surfaces::D0");
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
  (const Handle_Geom_Surface& S1, 
   const Handle_Geom_Surface& S2, 
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
