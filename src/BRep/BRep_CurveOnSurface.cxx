// Created on: 1993-07-06
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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


#include <BRep_CurveOnSurface.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Surface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>
#include <TopLoc_Location.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRep_CurveOnSurface,BRep_GCurve)

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


