// Created on: 1994-03-03
// Created by: Yves FRICAUD
// Copyright (c) 1994-1999 Matra Datavision
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



#include <Bisector.ixx>
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

