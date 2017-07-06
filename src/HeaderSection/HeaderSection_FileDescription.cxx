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


#include <HeaderSection_FileDescription.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(HeaderSection_FileDescription,Standard_Transient)

HeaderSection_FileDescription::HeaderSection_FileDescription ()  {}

void HeaderSection_FileDescription::Init(
	const Handle(Interface_HArray1OfHAsciiString)& aDescription,
	const Handle(TCollection_HAsciiString)& aImplementationLevel)
{
	// --- class own fields ---
	description = aDescription;
	implementationLevel = aImplementationLevel;
}


void HeaderSection_FileDescription::SetDescription(const Handle(Interface_HArray1OfHAsciiString)& aDescription)
{
	description = aDescription;
}

Handle(Interface_HArray1OfHAsciiString) HeaderSection_FileDescription::Description() const
{
	return description;
}

Handle(TCollection_HAsciiString) HeaderSection_FileDescription::DescriptionValue(const Standard_Integer num) const
{
	return description->Value(num);
}

Standard_Integer HeaderSection_FileDescription::NbDescription () const
{
	if (description.IsNull()) return 0;
	return description->Length();
}

void HeaderSection_FileDescription::SetImplementationLevel(const Handle(TCollection_HAsciiString)& aImplementationLevel)
{
	implementationLevel = aImplementationLevel;
}

Handle(TCollection_HAsciiString) HeaderSection_FileDescription::ImplementationLevel() const
{
	return implementationLevel;
}
