// Created on: 1997-03-28
// Created by: Philippe MANGIN
// Copyright (c) 1997-1999 Matra Datavision
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



#include <ChFi3d_SearchSing.ixx>

#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

ChFi3d_SearchSing::ChFi3d_SearchSing(const Handle(Geom_Curve)& C1,
				     const Handle(Geom_Curve)& C2)
{
  myC1 = C1;
  myC2 = C2;
}



Standard_Boolean ChFi3d_SearchSing::Value(const Standard_Real X,
					  Standard_Real& F) 
{
  gp_Pnt P1, P2;
  gp_Vec V1, V2;
  myC1->D1(X, P1, V1);
  myC2->D1(X, P2, V2);
  gp_Vec V(P1,P2);
  F = V * (V2-V1);
  return Standard_True;
}

Standard_Boolean ChFi3d_SearchSing::Derivative(const Standard_Real X,
					       Standard_Real& D ) 
{
  gp_Pnt P1, P2;
  gp_Vec V1, V2, W1, W2;
  myC1->D2(X, P1, V1, W1);
  myC2->D2(X, P2, V2, W2);
  gp_Vec V(P1,P2), VPrim;
  VPrim = V2 -V1;
  D = VPrim.SquareMagnitude() + (V * (W2-W1));
  return Standard_True;
}

Standard_Boolean ChFi3d_SearchSing::Values(const Standard_Real X,
					   Standard_Real& F,
					   Standard_Real& D ) 
{
  gp_Pnt P1, P2;
  gp_Vec V1, V2, W1, W2;
  myC1->D2(X, P1, V1, W1);
  myC2->D2(X, P2, V2, W2);
  gp_Vec V(P1,P2), VPrim;
  VPrim = V2 -V1;
  F = V * VPrim;
  D = VPrim.SquareMagnitude() + (V * (W2-W1));
  return Standard_True;
}
