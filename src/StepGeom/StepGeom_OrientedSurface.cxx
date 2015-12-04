// Created on: 2002-01-04
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <Standard_Type.hxx>
#include <StepGeom_OrientedSurface.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_OrientedSurface,StepGeom_Surface)

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
