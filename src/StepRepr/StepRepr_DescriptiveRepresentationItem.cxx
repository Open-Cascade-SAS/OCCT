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
#include <StepRepr_DescriptiveRepresentationItem.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepRepr_DescriptiveRepresentationItem,StepRepr_RepresentationItem)

StepRepr_DescriptiveRepresentationItem::StepRepr_DescriptiveRepresentationItem ()  {}

void StepRepr_DescriptiveRepresentationItem::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(TCollection_HAsciiString)& aDescription)
{
	// --- classe own fields ---
	description = aDescription;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepRepr_DescriptiveRepresentationItem::SetDescription(const Handle(TCollection_HAsciiString)& aDescription)
{
	description = aDescription;
}

Handle(TCollection_HAsciiString) StepRepr_DescriptiveRepresentationItem::Description() const
{
	return description;
}
