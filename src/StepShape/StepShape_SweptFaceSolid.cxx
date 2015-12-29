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


#include <Standard_Type.hxx>
#include <StepShape_FaceSurface.hxx>
#include <StepShape_SweptFaceSolid.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_SweptFaceSolid,StepShape_SolidModel)

StepShape_SweptFaceSolid::StepShape_SweptFaceSolid ()  {}

void StepShape_SweptFaceSolid::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_FaceSurface)& aSweptArea)
{
  // --- classe own fields ---
  sweptArea = aSweptArea;
  // --- classe inherited fields ---
  StepRepr_RepresentationItem::Init(aName);
}


void StepShape_SweptFaceSolid::SetSweptFace(const Handle(StepShape_FaceSurface)& aSweptArea)
{
  sweptArea = aSweptArea;
}

Handle(StepShape_FaceSurface) StepShape_SweptFaceSolid::SweptFace() const
{
  return sweptArea;
}
