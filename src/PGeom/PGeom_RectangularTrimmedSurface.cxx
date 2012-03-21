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



#include <PGeom_RectangularTrimmedSurface.ixx>

//=======================================================================
//function : PGeom_RectangularTrimmedSurface
//purpose  : 
//=======================================================================

PGeom_RectangularTrimmedSurface::PGeom_RectangularTrimmedSurface()
{}


//=======================================================================
//function : PGeom_RectangularTrimmedSurface
//purpose  : 
//=======================================================================

PGeom_RectangularTrimmedSurface::PGeom_RectangularTrimmedSurface
  (const Handle(PGeom_Surface)& aBasisSurface,
   const Standard_Real aFirstU,
   const Standard_Real aLastU,
   const Standard_Real aFirstV,
   const Standard_Real aLastV) :
  basisSurface(aBasisSurface),
  firstU(aFirstU),
  lastU(aLastU),
  firstV(aFirstV),
  lastV(aLastV)
{}


//=======================================================================
//function : BasisSurface
//purpose  : 
//=======================================================================

void  PGeom_RectangularTrimmedSurface::BasisSurface
  (const Handle(PGeom_Surface)& aBasisSurface)
{ basisSurface = aBasisSurface;}


//=======================================================================
//function : BasisSurface
//purpose  : 
//=======================================================================

Handle(PGeom_Surface)  PGeom_RectangularTrimmedSurface::BasisSurface() const 
{ return basisSurface; }


//=======================================================================
//function : FirstU
//purpose  : 
//=======================================================================

void  PGeom_RectangularTrimmedSurface::FirstU(const Standard_Real aFirstU)
{ firstU = aFirstU; }


//=======================================================================
//function : FirstU
//purpose  : 
//=======================================================================

Standard_Real  PGeom_RectangularTrimmedSurface::FirstU() const 
{ return firstU; }


//=======================================================================
//function : LastU
//purpose  : 
//=======================================================================

void  PGeom_RectangularTrimmedSurface::LastU(const Standard_Real aLastU)
{ lastU = aLastU; }


//=======================================================================
//function : LastU
//purpose  : 
//=======================================================================

Standard_Real  PGeom_RectangularTrimmedSurface::LastU() const 
{ return lastU; }


//=======================================================================
//function : FirstV
//purpose  : 
//=======================================================================

void  PGeom_RectangularTrimmedSurface::FirstV(const Standard_Real aFirstV)
{ firstV = aFirstV; }


//=======================================================================
//function : FirstV
//purpose  : 
//=======================================================================

Standard_Real  PGeom_RectangularTrimmedSurface::FirstV() const 
{ return firstV; }


//=======================================================================
//function : LastV
//purpose  : 
//=======================================================================

void  PGeom_RectangularTrimmedSurface::LastV(const Standard_Real aLastV)
{ lastV = aLastV; }


//=======================================================================
//function : LastV
//purpose  : 
//=======================================================================

Standard_Real  PGeom_RectangularTrimmedSurface::LastV() const 
{ return lastV; }
