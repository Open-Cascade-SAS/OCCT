// File:	StepGeom_CurveBoundedSurface.cxx
// Created:	Fri Nov 26 16:26:38 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

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
