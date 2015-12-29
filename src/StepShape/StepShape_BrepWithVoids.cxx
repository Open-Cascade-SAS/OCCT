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
#include <StepShape_BrepWithVoids.hxx>
#include <StepShape_ClosedShell.hxx>
#include <StepShape_OrientedClosedShell.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_BrepWithVoids,StepShape_ManifoldSolidBrep)

StepShape_BrepWithVoids::StepShape_BrepWithVoids ()  {}

void StepShape_BrepWithVoids::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_ClosedShell)& aOuter,
	const Handle(StepShape_HArray1OfOrientedClosedShell)& aVoids)
{
	// --- classe own fields ---
	voids = aVoids;
	// --- classe inherited fields ---
	StepShape_ManifoldSolidBrep::Init(aName, aOuter);
}


void StepShape_BrepWithVoids::SetVoids(const Handle(StepShape_HArray1OfOrientedClosedShell)& aVoids)
{
	voids = aVoids;
}

Handle(StepShape_HArray1OfOrientedClosedShell) StepShape_BrepWithVoids::Voids() const
{
	return voids;
}

Handle(StepShape_OrientedClosedShell) StepShape_BrepWithVoids::VoidsValue(const Standard_Integer num) const
{
	return voids->Value(num);
}

Standard_Integer StepShape_BrepWithVoids::NbVoids () const
{
	if (voids.IsNull()) return 0;
	return voids->Length();
}
