// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <HeaderSection_FileDescription.ixx>


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
