// Created on: 1995-12-05
// Created by: Laurent BOURESCHE
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



#include <GeomFill_DegeneratedBound.ixx>

//=======================================================================
//function : GeomFill_DegeneratedBound
//purpose  : 
//=======================================================================

GeomFill_DegeneratedBound::GeomFill_DegeneratedBound
(const gp_Pnt& Point, 
 const Standard_Real First, 
 const Standard_Real Last, 
 const Standard_Real Tol3d, 
 const Standard_Real Tolang) :
 GeomFill_Boundary(Tol3d,Tolang),
 myPoint(Point),myFirst(First),myLast(Last)
{
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

//gp_Pnt GeomFill_DegeneratedBound::Value(const Standard_Real U) const 
gp_Pnt GeomFill_DegeneratedBound::Value(const Standard_Real ) const 
{
  return myPoint;
}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

//void GeomFill_DegeneratedBound::D1(const Standard_Real U, 
void GeomFill_DegeneratedBound::D1(const Standard_Real , 
				   gp_Pnt& P, 
				   gp_Vec& V) const 
{
  P = myPoint;
  V.SetCoord(0.,0.,0.);
}


//=======================================================================
//function : Reparametrize
//purpose  : 
//=======================================================================

void GeomFill_DegeneratedBound::Reparametrize(const Standard_Real First, 
					      const Standard_Real Last, 
					      const Standard_Boolean , 
					      const Standard_Boolean , 
					      const Standard_Real , 
					      const Standard_Real , 
					      const Standard_Boolean )
{
  myFirst = First;
  myLast  = Last;
}


//=======================================================================
//function : Bounds
//purpose  : 
//=======================================================================

void GeomFill_DegeneratedBound::Bounds(Standard_Real& First, 
				       Standard_Real& Last) const 
{
  First = myFirst;
  Last  = myLast;
}


//=======================================================================
//function : IsDegenerated
//purpose  : 
//=======================================================================

Standard_Boolean GeomFill_DegeneratedBound::IsDegenerated() const 
{
  return Standard_True;
}
