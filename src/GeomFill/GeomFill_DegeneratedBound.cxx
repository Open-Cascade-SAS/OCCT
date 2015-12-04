// Created on: 1995-12-05
// Created by: Laurent BOURESCHE
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


#include <GeomFill_DegeneratedBound.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomFill_DegeneratedBound,GeomFill_Boundary)

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
