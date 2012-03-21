// Created on: 1999-11-26
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

#include <StepGeom_CurveBoundedSurface.ixx>

//=======================================================================
//function : StepGeom_CurveBoundedSurface
//purpose  : 
//=======================================================================

StepGeom_CurveBoundedSurface::StepGeom_CurveBoundedSurface ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepGeom_CurveBoundedSurface::Init (const Handle(TCollection_HAsciiString) &aRepresentationItem_Name,
                                         const Handle(StepGeom_Surface) &aBasisSurface,
                                         const Handle(StepGeom_HArray1OfSurfaceBoundary) &aBoundaries,
                                         const Standard_Boolean aImplicitOuter)
{
  StepGeom_BoundedSurface::Init(aRepresentationItem_Name);

  theBasisSurface = aBasisSurface;

  theBoundaries = aBoundaries;

  theImplicitOuter = aImplicitOuter;
}

//=======================================================================
//function : BasisSurface
//purpose  : 
//=======================================================================

Handle(StepGeom_Surface) StepGeom_CurveBoundedSurface::BasisSurface () const
{
  return theBasisSurface;
}

//=======================================================================
//function : SetBasisSurface
//purpose  : 
//=======================================================================

void StepGeom_CurveBoundedSurface::SetBasisSurface (const Handle(StepGeom_Surface) &aBasisSurface)
{
  theBasisSurface = aBasisSurface;
}

//=======================================================================
//function : Boundaries
//purpose  : 
//=======================================================================

Handle(StepGeom_HArray1OfSurfaceBoundary) StepGeom_CurveBoundedSurface::Boundaries () const
{
  return theBoundaries;
}

//=======================================================================
//function : SetBoundaries
//purpose  : 
//=======================================================================

void StepGeom_CurveBoundedSurface::SetBoundaries (const Handle(StepGeom_HArray1OfSurfaceBoundary) &aBoundaries)
{
  theBoundaries = aBoundaries;
}

//=======================================================================
//function : ImplicitOuter
//purpose  : 
//=======================================================================

Standard_Boolean StepGeom_CurveBoundedSurface::ImplicitOuter () const
{
  return theImplicitOuter;
}

//=======================================================================
//function : SetImplicitOuter
//purpose  : 
//=======================================================================

void StepGeom_CurveBoundedSurface::SetImplicitOuter (const Standard_Boolean aImplicitOuter)
{
  theImplicitOuter = aImplicitOuter;
}
