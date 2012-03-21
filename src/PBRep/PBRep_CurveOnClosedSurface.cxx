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



#include <PBRep_CurveOnClosedSurface.ixx>


//=======================================================================
//function : PBRep_CurveOnClosedSurface
//purpose  : 
//=======================================================================

PBRep_CurveOnClosedSurface::PBRep_CurveOnClosedSurface
  (const Handle(PGeom2d_Curve)& PC1,
   const Handle(PGeom2d_Curve)& PC2,
   const Standard_Real          CF,
   const Standard_Real          CL,
   const Handle(PGeom_Surface)& S,
   const PTopLoc_Location& L,
   const GeomAbs_Shape C) :
  PBRep_CurveOnSurface(PC1, CF, CL, S, L),
  myPCurve2(PC2),
  myContinuity(C)
{
}


//=======================================================================
//function : PCurve2
//purpose  : 
//=======================================================================

Handle(PGeom2d_Curve)  PBRep_CurveOnClosedSurface::PCurve2()const 
{
  return myPCurve2;
}


//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape  PBRep_CurveOnClosedSurface::Continuity()const 
{
  return myContinuity;
}

//=======================================================================
//function : IsCurveOnClosedSurface
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_CurveOnClosedSurface::IsCurveOnClosedSurface()const 
{
  return Standard_True;
}

//=======================================================================
//function : IsRegularity
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_CurveOnClosedSurface::IsRegularity()const 
{
  return Standard_True;
}

//=======================================================================
//function : SetUVPoints2
//purpose  : 
//=======================================================================

void PBRep_CurveOnClosedSurface::SetUVPoints2(const gp_Pnt2d& Pnt1,
					      const gp_Pnt2d& Pnt2) 
{
  myUV21 = Pnt1;
  myUV22 = Pnt2;
}

//=======================================================================
//function : FirstUV2
//purpose  : 
//=======================================================================

gp_Pnt2d PBRep_CurveOnClosedSurface::FirstUV2() const 
{
  return myUV21;
}

//=======================================================================
//function : LastUV2
//purpose  : 
//=======================================================================

gp_Pnt2d PBRep_CurveOnClosedSurface::LastUV2() const 
{
  return myUV22;
}

