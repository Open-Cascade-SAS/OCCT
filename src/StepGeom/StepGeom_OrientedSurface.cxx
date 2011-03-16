// File:	StepGeom_OrientedSurface.cxx
// Created:	Fri Jan  4 17:42:44 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <StepGeom_OrientedSurface.ixx>

//=======================================================================
//function : StepGeom_OrientedSurface
//purpose  : 
//=======================================================================

StepGeom_OrientedSurface::StepGeom_OrientedSurface ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepGeom_OrientedSurface::Init (const Handle(TCollection_HAsciiString) &aRepresentationItem_Name,
                                     const Standard_Boolean aOrientation)
{
  StepGeom_Surface::Init(aRepresentationItem_Name);

  theOrientation = aOrientation;
}

//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================

Standard_Boolean StepGeom_OrientedSurface::Orientation () const
{
  return theOrientation;
}

//=======================================================================
//function : SetOrientation
//purpose  : 
//=======================================================================

void StepGeom_OrientedSurface::SetOrientation (const Standard_Boolean aOrientation)
{
  theOrientation = aOrientation;
}
