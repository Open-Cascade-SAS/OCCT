// Created on: 1995-11-03
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



#include <GeomFill_SimpleBound.ixx>
#include <Law_BSpFunc.hxx>
#include <Law.hxx>

//=======================================================================
//function : GeomFill_SimpleBound
//purpose  : 
//=======================================================================

GeomFill_SimpleBound::GeomFill_SimpleBound
(const Handle(Adaptor3d_HCurve)& Curve,
 const Standard_Real           Tol3d,
 const Standard_Real           Tolang) :
 GeomFill_Boundary(Tol3d,Tolang), myC3d(Curve)
{
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

gp_Pnt GeomFill_SimpleBound::Value(const Standard_Real U) const 
{
  Standard_Real x = U;
  if(!myPar.IsNull()) x = myPar->Value(U);
  return myC3d->Value(x);
}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void GeomFill_SimpleBound::D1(const Standard_Real U, 
			      gp_Pnt& P, 
			      gp_Vec& V) const 
{
  Standard_Real x = U, dx = 1.;
  if(!myPar.IsNull()) myPar->D1(U,x,dx);
  myC3d->D1(x, P, V);
  V.Multiply(dx);
}

//=======================================================================
//function : Reparametrize
//purpose  : 
//=======================================================================

void GeomFill_SimpleBound::Reparametrize(const Standard_Real First, 
					 const Standard_Real Last,
					 const Standard_Boolean HasDF, 
					 const Standard_Boolean HasDL, 
					 const Standard_Real DF, 
					 const Standard_Real DL,
					 const Standard_Boolean Rev)
{
  Handle(Law_BSpline) curve = Law::Reparametrize(myC3d->Curve(),
						 First,Last,
						 HasDF,HasDL,DF,DL,
						 Rev,30);
  myPar = new Law_BSpFunc();
  (*((Handle_Law_BSpFunc*) &myPar))->SetCurve(curve);
}

//=======================================================================
//function : Bounds
//purpose  : 
//=======================================================================

void GeomFill_SimpleBound::Bounds(Standard_Real& First, 
				  Standard_Real& Last) const 
{
  if(!myPar.IsNull()) myPar->Bounds(First,Last);
  else {
    First = myC3d->FirstParameter();
    Last = myC3d->LastParameter();
  }
}


//=======================================================================
//function : IsDegenerated
//purpose  : 
//=======================================================================

Standard_Boolean GeomFill_SimpleBound::IsDegenerated() const 
{
  return Standard_False;
}
