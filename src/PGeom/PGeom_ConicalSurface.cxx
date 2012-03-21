// Created on: 1993-03-03
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



#include <PGeom_ConicalSurface.ixx>

//=======================================================================
//function : PGeom_ConicalSurface
//purpose  : 
//=======================================================================

PGeom_ConicalSurface::PGeom_ConicalSurface()
{}


//=======================================================================
//function : PGeom_ConicalSurface
//purpose  : 
//=======================================================================

PGeom_ConicalSurface::PGeom_ConicalSurface
  (const gp_Ax3& aPosition,
   const Standard_Real aRadius,
   const Standard_Real aSemiAngle) :
  PGeom_ElementarySurface(aPosition),
  radius(aRadius),
  semiAngle(aSemiAngle)
{}


//=======================================================================
//function : Radius
//purpose  : 
//=======================================================================

void  PGeom_ConicalSurface::Radius(const Standard_Real aRadius)
{ radius = aRadius; }


//=======================================================================
//function : Radius
//purpose  : 
//=======================================================================

Standard_Real  PGeom_ConicalSurface::Radius() const 
{ return radius; }


//=======================================================================
//function : SemiAngle
//purpose  : 
//=======================================================================

void  PGeom_ConicalSurface::SemiAngle(const Standard_Real aSemiAngle)
{ semiAngle = aSemiAngle; }


//=======================================================================
//function : SemiAngle
//purpose  : 
//=======================================================================

Standard_Real  PGeom_ConicalSurface::SemiAngle() const 
{ return semiAngle; }
