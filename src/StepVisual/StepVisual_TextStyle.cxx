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
#include <StepVisual_TextStyle.hxx>
#include <StepVisual_TextStyleForDefinedFont.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_TextStyle,Standard_Transient)

StepVisual_TextStyle::StepVisual_TextStyle ()  {}

void StepVisual_TextStyle::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepVisual_TextStyleForDefinedFont)& aCharacterAppearance)
{
	// --- classe own fields ---
	name = aName;
	characterAppearance = aCharacterAppearance;
}


void StepVisual_TextStyle::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) StepVisual_TextStyle::Name() const
{
	return name;
}

void StepVisual_TextStyle::SetCharacterAppearance(const Handle(StepVisual_TextStyleForDefinedFont)& aCharacterAppearance)
{
	characterAppearance = aCharacterAppearance;
}

Handle(StepVisual_TextStyleForDefinedFont) StepVisual_TextStyle::CharacterAppearance() const
{
	return characterAppearance;
}
