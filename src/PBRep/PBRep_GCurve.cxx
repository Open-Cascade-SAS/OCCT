// Copyright (c) 1998-1999 Matra Datavision
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

#include <PBRep_GCurve.ixx>


//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

PBRep_GCurve::PBRep_GCurve(const PTopLoc_Location& L, 
			   const Standard_Real First, 
			   const Standard_Real Last) :
			   PBRep_CurveRepresentation(L),
			   myFirst(First),
			   myLast(Last)
{
}

//=======================================================================
//function : First
//purpose  : 
//=======================================================================

Standard_Real  PBRep_GCurve::First()const 
{
  return myFirst;
}


//=======================================================================
//function : First
//purpose  : 
//=======================================================================

void  PBRep_GCurve::First(const Standard_Real F)
{
  myFirst = F;
}


//=======================================================================
//function : Last
//purpose  : 
//=======================================================================

Standard_Real  PBRep_GCurve::Last()const 
{
  return myLast;
}


//=======================================================================
//function : Last
//purpose  : 
//=======================================================================

void  PBRep_GCurve::Last(const Standard_Real L)
{
  myLast = L;
}


//=======================================================================
//function : IsGCurve
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_GCurve::IsGCurve() const 
{
  return Standard_True;
}

