// Created on: 1992-06-10
// Created by: Laurent BUCHARD
// Copyright (c) 1992-1999 Matra Datavision
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

//  modified by Edward AGAPOV (eap)  Jun 7 2002 (occ 438)
//  --- limit infinite points and parameters in order to make
//  --- arithmetic operation on them safe 

#include <gp_Pnt2d.hxx>
#include <IntRes2d_Domain.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>

const Standard_Real infVal = Precision::Infinite();

//=======================================================================
//function : LimitInfinite
//purpose  : 
//=======================================================================

static inline Standard_Real LimitInfinite(const Standard_Real Val)
{
  return ( Abs(Val) > infVal ? (Val>0 ? infVal : -infVal) : Val );
}

//=======================================================================
//function : IntRes2d_Domain
//purpose  : 
//=======================================================================

IntRes2d_Domain::IntRes2d_Domain(): 
       status(0),
       first_param(0.0), last_param(0.0),
       first_tol(0.0), last_tol(0.0),
       first_point(0.0, 0.0), last_point(0.0, 0.0),
       periodfirst(0.0),
       periodlast(0.0) { } 


void IntRes2d_Domain::SetValues() {
  status=0;
  periodfirst=periodlast=0.0;
}

//=======================================================================
//function : IntRes2d_Domain
//purpose  : Creates a bounded Domain.
//=======================================================================

IntRes2d_Domain::IntRes2d_Domain(const gp_Pnt2d& Pnt1,
				 const Standard_Real Par1,
				 const Standard_Real Tol1,
				 const gp_Pnt2d& Pnt2,
				 const Standard_Real Par2,
				 const Standard_Real Tol2) {

  SetValues(Pnt1,Par1,Tol1,Pnt2,Par2,Tol2);
}

//=======================================================================
//function : SetValues
//purpose  : Sets the values for a bounded domain.
//=======================================================================

void IntRes2d_Domain::SetValues(const gp_Pnt2d& Pnt1,
				const Standard_Real Par1,
				const Standard_Real Tol1,
				const gp_Pnt2d& Pnt2,
				const Standard_Real Par2,
				const Standard_Real Tol2) {

  status = 3;
  periodfirst = periodlast = 0.0;

  first_param=LimitInfinite(Par1);
  first_point.SetCoord( LimitInfinite(Pnt1.X()), LimitInfinite(Pnt1.Y()) );
  first_tol=Tol1;

  last_param=LimitInfinite(Par2);
  last_point.SetCoord( LimitInfinite(Pnt2.X()), LimitInfinite(Pnt2.Y()) );
  last_tol=Tol2;

}



IntRes2d_Domain::IntRes2d_Domain(const gp_Pnt2d& Pnt,
				 const Standard_Real Par,
				 const Standard_Real Tol,
				 const Standard_Boolean First) :
       first_param(0.0), last_param(0.0),
       first_tol(0.0), last_tol(0.0),
       first_point(0.0, 0.0), last_point(0.0, 0.0)
{
  SetValues(Pnt,Par,Tol,First);
}

void IntRes2d_Domain::SetValues(const gp_Pnt2d& Pnt,
				const Standard_Real Par,
				const Standard_Real Tol,
				const Standard_Boolean First) {
  
  periodfirst=periodlast=0.0;
  if(First) {
    status=1;
    first_param=LimitInfinite(Par);
    first_point.SetCoord( LimitInfinite(Pnt.X()), LimitInfinite(Pnt.Y()) );
    first_tol=Tol;
  }
  else {
    status=2;
    last_param=LimitInfinite(Par);
    last_point.SetCoord( LimitInfinite(Pnt.X()), LimitInfinite(Pnt.Y()) );
    last_tol=Tol;
  }
}



