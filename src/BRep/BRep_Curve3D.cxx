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



#include <BRep_Curve3D.ixx>

//=======================================================================
//function : BRep_Curve3D
//purpose  : 
//=======================================================================

BRep_Curve3D::BRep_Curve3D(const Handle(Geom_Curve)& C,
			   const TopLoc_Location& L) :
                           BRep_GCurve(L,
			   C.IsNull() ? RealFirst() : C->FirstParameter(),
			   C.IsNull() ? RealLast()  : C->LastParameter()),
			   myCurve(C)
{
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void BRep_Curve3D::D0(const Standard_Real U, gp_Pnt& P) const
{
  // shoud be D0 NYI
  P = myCurve->Value(U);
}

//=======================================================================
//function : IsCurve3D
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_Curve3D::IsCurve3D()const 
{
  return Standard_True;
}


//=======================================================================
//function : Curve3D
//purpose  : 
//=======================================================================

const Handle(Geom_Curve)&  BRep_Curve3D::Curve3D()const 
{
  return myCurve;
}


//=======================================================================
//function : Curve3D
//purpose  : 
//=======================================================================

void BRep_Curve3D::Curve3D(const Handle(Geom_Curve)& C)
{
  myCurve = C;
}

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(BRep_CurveRepresentation) BRep_Curve3D::Copy() const
{
  Handle(BRep_Curve3D) C = new BRep_Curve3D(myCurve,Location());

  C->SetRange(First(), Last());
  return C;
}
