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
#include <StepGeom_Direction.hxx>
#include <StepShape_ExtrudedFaceSolid.hxx>
#include <StepShape_FaceSurface.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_ExtrudedFaceSolid,StepShape_SweptFaceSolid)

StepShape_ExtrudedFaceSolid::StepShape_ExtrudedFaceSolid ()  {}

void StepShape_ExtrudedFaceSolid::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_FaceSurface)& aSweptArea,
	const Handle(StepGeom_Direction)& aExtrudedDirection,
	const Standard_Real aDepth)
{
  // --- classe own fields ---
  extrudedDirection = aExtrudedDirection;
  depth = aDepth;
  // --- classe inherited fields ---
  StepShape_SweptFaceSolid::Init(aName, aSweptArea);
}


void StepShape_ExtrudedFaceSolid::SetExtrudedDirection(const Handle(StepGeom_Direction)& aExtrudedDirection)
{
  extrudedDirection = aExtrudedDirection;
}

Handle(StepGeom_Direction) StepShape_ExtrudedFaceSolid::ExtrudedDirection() const
{
  return extrudedDirection;
}

void StepShape_ExtrudedFaceSolid::SetDepth(const Standard_Real aDepth)
{
  depth = aDepth;
}

Standard_Real StepShape_ExtrudedFaceSolid::Depth() const
{
  return depth;
}
