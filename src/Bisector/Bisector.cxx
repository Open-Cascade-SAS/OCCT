// Created on: 1994-03-03
// Created by: Yves FRICAUD
// Copyright (c) 1994-1999 Matra Datavision
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


#include <Bisector.hxx>
#include <Geom2d_Curve.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>

//======================================================================
// function : IsConvex
// Purpose  :
//======================================================================
Standard_Boolean Bisector::IsConvex(const Handle(Geom2d_Curve)& Cu,
				    const Standard_Real         Sign)
{
  
  Standard_Real U1 = (Cu->LastParameter() + Cu->FirstParameter())/2.;
  gp_Pnt2d      P1;
  gp_Vec2d      V1,V2;
  Cu->D2(U1,P1,V1,V2);
  Standard_Real Tol = 1.e-5;
  if (Sign*(V1^V2) < Tol) return Standard_True; // <= 0.
  else                    return Standard_False;
}      

