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

#include <HeaderSection_FileName.ixx>


HeaderSection_FileName::HeaderSection_FileName ()  {}

void HeaderSection_FileName::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(TCollection_HAsciiString)& aTimeStamp,
	const Handle(Interface_HArray1OfHAsciiString)& aAuthor,
	const Handle(Interface_HArray1OfHAsciiString)& aOrganization,
	const Handle(TCollection_HAsciiString)& aPreprocessorVersion,
	const Handle(TCollection_HAsciiString)& aOriginatingSystem,
	const Handle(TCollection_HAsciiString)& aAuthorisation)
{
	// --- class own fields ---
	name = aName;
	timeStamp = aTimeStamp;
	author = aAuthor;
	organization = aOrganization;
	preprocessorVersion = aPreprocessorVersion;
	originatingSystem = aOriginatingSystem;
	authorisation = aAuthorisation;
}


void HeaderSection_FileName::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) HeaderSection_FileName::Name() const
{
	return name;
}

void HeaderSection_FileName::SetTimeStamp(const Handle(TCollection_HAsciiString)& aTimeStamp)
{
	timeStamp = aTimeStamp;
}

Handle(TCollection_HAsciiString) HeaderSection_FileName::TimeStamp() const
{
	return timeStamp;
}

void HeaderSection_FileName::SetAuthor(const Handle(Interface_HArray1OfHAsciiString)& aAuthor)
{
	author = aAuthor;
}

Handle(Interface_HArray1OfHAsciiString) HeaderSection_FileName::Author() const
{
	return author;
}

Handle(TCollection_HAsciiString) HeaderSection_FileName::AuthorValue(const Standard_Integer num) const
{
	return author->Value(num);
}

Standard_Integer HeaderSection_FileName::NbAuthor () const
{
	if (author.IsNull()) return 0;
	return author->Length();
}

void HeaderSection_FileName::SetOrganization(const Handle(Interface_HArray1OfHAsciiString)& aOrganization)
{
	organization = aOrganization;
}

Handle(Interface_HArray1OfHAsciiString) HeaderSection_FileName::Organization() const
{
	return organization;
}

Handle(TCollection_HAsciiString) HeaderSection_FileName::OrganizationValue(const Standard_Integer num) const
{
	return organization->Value(num);
}

Standard_Integer HeaderSection_FileName::NbOrganization () const
{
	if (organization.IsNull()) return 0;
	return organization->Length();
}

void HeaderSection_FileName::SetPreprocessorVersion(const Handle(TCollection_HAsciiString)& aPreprocessorVersion)
{
	preprocessorVersion = aPreprocessorVersion;
}

Handle(TCollection_HAsciiString) HeaderSection_FileName::PreprocessorVersion() const
{
	return preprocessorVersion;
}

void HeaderSection_FileName::SetOriginatingSystem(const Handle(TCollection_HAsciiString)& aOriginatingSystem)
{
	originatingSystem = aOriginatingSystem;
}

Handle(TCollection_HAsciiString) HeaderSection_FileName::OriginatingSystem() const
{
	return originatingSystem;
}

void HeaderSection_FileName::SetAuthorisation(const Handle(TCollection_HAsciiString)& aAuthorisation)
{
	authorisation = aAuthorisation;
}

Handle(TCollection_HAsciiString) HeaderSection_FileName::Authorisation() const
{
	return authorisation;
}
