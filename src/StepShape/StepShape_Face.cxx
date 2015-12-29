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
#include <StepShape_Face.hxx>
#include <StepShape_FaceBound.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_Face,StepShape_TopologicalRepresentationItem)

StepShape_Face::StepShape_Face ()  {}

void StepShape_Face::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_HArray1OfFaceBound)& aBounds)
{
	// --- classe own fields ---
	bounds = aBounds;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepShape_Face::SetBounds(const Handle(StepShape_HArray1OfFaceBound)& aBounds)
{
	bounds = aBounds;
}

Handle(StepShape_HArray1OfFaceBound) StepShape_Face::Bounds() const
{
	return bounds;
}

Handle(StepShape_FaceBound) StepShape_Face::BoundsValue(const Standard_Integer num) const
{
	return bounds->Value(num);
}

Standard_Integer StepShape_Face::NbBounds () const
{
	if (bounds.IsNull()) return 0;
	return bounds->Length();
}
