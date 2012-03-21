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



#include <BRep_CurveOnSurface.ixx>
#include <Precision.hxx>


//=======================================================================
//function : BRep_CurveOnSurface
//purpose  : 
//=======================================================================

BRep_CurveOnSurface::BRep_CurveOnSurface(const Handle(Geom2d_Curve)& PC, 
					 const Handle(Geom_Surface)& S, 
					 const TopLoc_Location& L) :
       BRep_GCurve(L,PC->FirstParameter(),PC->LastParameter()),
       myPCurve(PC),
       mySurface(S)
{
}


//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void BRep_CurveOnSurface::D0(const Standard_Real U, gp_Pnt& P) const
{
  // shoud be D0 NYI
  gp_Pnt2d P2d = myPCurve->Value(U);
  P = mySurface->Value(P2d.X(),P2d.Y());
  P.Transform(myLocation.Transformation());
}

//=======================================================================
//function : IsCurveOnSurface
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_CurveOnSurface::IsCurveOnSurface()const 
{
  return Standard_True;
}

//=======================================================================
//function : IsCurveOnSurface
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_CurveOnSurface::IsCurveOnSurface
  (const Handle(Geom_Surface)& S, const TopLoc_Location& L)const 
{
  return (S == mySurface) && (L == myLocation);
}



//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================

const Handle(Geom_Surface)&  BRep_CurveOnSurface::Surface()const 
{
  return mySurface;
}


//=======================================================================
//function : PCurve
//purpose  : 
//=======================================================================

const Handle(Geom2d_Curve)&  BRep_CurveOnSurface::PCurve()const 
{
  return myPCurve;
}

//=======================================================================
//function : PCurve
//purpose  : 
//=======================================================================

void  BRep_CurveOnSurface::PCurve(const Handle(Geom2d_Curve)& C)
{
  myPCurve = C;
}


//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(BRep_CurveRepresentation) BRep_CurveOnSurface::Copy() const
{
  Handle(BRep_CurveOnSurface) C = new BRep_CurveOnSurface(myPCurve,
							  mySurface,
							  Location());
 
  C->SetRange(First(),Last());
  C->SetUVPoints(myUV1,myUV2);

  return C;
}


//=======================================================================
//function : Update
//purpose  : 
//=======================================================================

void  BRep_CurveOnSurface::Update()
{
  Standard_Real f = First();
  Standard_Real l = Last();
  Standard_Boolean isneg = Precision::IsNegativeInfinite(f);
  Standard_Boolean ispos = Precision::IsPositiveInfinite(l);
  if (!isneg) {
    myPCurve->D0(f,myUV1);
  }
  if (!ispos) {
    myPCurve->D0(l,myUV2);
  }
}


