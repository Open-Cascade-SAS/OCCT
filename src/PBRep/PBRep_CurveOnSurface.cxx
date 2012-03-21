// Created on: 1993-07-26
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



#include <PBRep_CurveOnSurface.ixx>


//=======================================================================
//function : PBRep_CurveOnSurface
//purpose  : 
//=======================================================================

PBRep_CurveOnSurface::PBRep_CurveOnSurface(const Handle(PGeom2d_Curve)& PC,
					   const Standard_Real          CF,
					   const Standard_Real          CL,
					   const Handle(PGeom_Surface)& S,
					   const PTopLoc_Location&      L) :
       PBRep_GCurve(L, CF, CL),
       myPCurve(PC),
       mySurface(S)
{
}


//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================

Handle(PGeom_Surface)  PBRep_CurveOnSurface::Surface()const 
{
  return mySurface;
}


//=======================================================================
//function : PCurve
//purpose  : 
//=======================================================================

Handle(PGeom2d_Curve)  PBRep_CurveOnSurface::PCurve()const 
{
  return myPCurve;
}



//=======================================================================
//function : IsCurveOnSurface
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_CurveOnSurface::IsCurveOnSurface()const 
{
  return Standard_True;
}


//=======================================================================
//function : SetUVPoints
//purpose  : 
//=======================================================================

void PBRep_CurveOnSurface::SetUVPoints(const gp_Pnt2d& Pnt1,
				       const gp_Pnt2d& Pnt2) 
{
  myUV1 = Pnt1;
  myUV2 = Pnt2;
}

//=======================================================================
//function : FirstUV
//purpose  : 
//=======================================================================

gp_Pnt2d PBRep_CurveOnSurface::FirstUV() const 
{
  return myUV1;
}

//=======================================================================
//function : LastUV
//purpose  : 
//=======================================================================

gp_Pnt2d PBRep_CurveOnSurface::LastUV() const 
{
  return myUV2;
}

