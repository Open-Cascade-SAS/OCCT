// Created on: 1993-03-04
// Created by: Philippe DAUTRY
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



#include <PGeom_SweptSurface.ixx>

//=======================================================================
//function : PGeom_SweptSurface
//purpose  : 
//=======================================================================

PGeom_SweptSurface::PGeom_SweptSurface()
{}


//=======================================================================
//function : PGeom_SweptSurface
//purpose  : 
//=======================================================================

PGeom_SweptSurface::PGeom_SweptSurface
  (const Handle(PGeom_Curve)& aBasisCurve,
   const gp_Dir& aDirection) :
  basisCurve(aBasisCurve),
  direction(aDirection)
{}


//=======================================================================
//function : BasisCurve
//purpose  : 
//=======================================================================

void  PGeom_SweptSurface::BasisCurve
  (const Handle(PGeom_Curve)& aBasisCurve)
{ basisCurve = aBasisCurve; }


//=======================================================================
//function : BasisCurve
//purpose  : 
//=======================================================================

Handle(PGeom_Curve)  PGeom_SweptSurface::BasisCurve() const 
{ return basisCurve; }


//=======================================================================
//function : Direction
//purpose  : 
//=======================================================================

void  PGeom_SweptSurface::Direction(const gp_Dir& aDirection)
{ direction = aDirection; }


//=======================================================================
//function : Direction
//purpose  : 
//=======================================================================

gp_Dir  PGeom_SweptSurface::Direction() const 
{ return direction; }
