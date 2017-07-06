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
#include <StepBasic_Person.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_Person,Standard_Transient)

StepBasic_Person::StepBasic_Person ()  {}

void StepBasic_Person::Init(
	const Handle(TCollection_HAsciiString)& aId,
	const Standard_Boolean hasAlastName,
	const Handle(TCollection_HAsciiString)& aLastName,
	const Standard_Boolean hasAfirstName,
	const Handle(TCollection_HAsciiString)& aFirstName,
	const Standard_Boolean hasAmiddleNames,
	const Handle(Interface_HArray1OfHAsciiString)& aMiddleNames,
	const Standard_Boolean hasAprefixTitles,
	const Handle(Interface_HArray1OfHAsciiString)& aPrefixTitles,
	const Standard_Boolean hasAsuffixTitles,
	const Handle(Interface_HArray1OfHAsciiString)& aSuffixTitles)
{
	// --- classe own fields ---
	id = aId;
	hasLastName = hasAlastName;
	lastName = aLastName;
	hasFirstName = hasAfirstName;
	firstName = aFirstName;
	hasMiddleNames = hasAmiddleNames;
	middleNames = aMiddleNames;
	hasPrefixTitles = hasAprefixTitles;
	prefixTitles = aPrefixTitles;
	hasSuffixTitles = hasAsuffixTitles;
	suffixTitles = aSuffixTitles;
}


void StepBasic_Person::SetId(const Handle(TCollection_HAsciiString)& aId)
{
	id = aId;
}

Handle(TCollection_HAsciiString) StepBasic_Person::Id() const
{
	return id;
}

void StepBasic_Person::SetLastName(const Handle(TCollection_HAsciiString)& aLastName)
{
	lastName = aLastName;
	hasLastName = Standard_True;
}

void StepBasic_Person::UnSetLastName()
{
	hasLastName = Standard_False;
	lastName.Nullify();
}

Handle(TCollection_HAsciiString) StepBasic_Person::LastName() const
{
	return lastName;
}

Standard_Boolean StepBasic_Person::HasLastName() const
{
	return hasLastName;
}

void StepBasic_Person::SetFirstName(const Handle(TCollection_HAsciiString)& aFirstName)
{
	firstName = aFirstName;
	hasFirstName = Standard_True;
}

void StepBasic_Person::UnSetFirstName()
{
	hasFirstName = Standard_False;
	firstName.Nullify();
}

Handle(TCollection_HAsciiString) StepBasic_Person::FirstName() const
{
	return firstName;
}

Standard_Boolean StepBasic_Person::HasFirstName() const
{
	return hasFirstName;
}

void StepBasic_Person::SetMiddleNames(const Handle(Interface_HArray1OfHAsciiString)& aMiddleNames)
{
	middleNames = aMiddleNames;
	hasMiddleNames = Standard_True;
}

void StepBasic_Person::UnSetMiddleNames()
{
	hasMiddleNames = Standard_False;
	middleNames.Nullify();
}

Handle(Interface_HArray1OfHAsciiString) StepBasic_Person::MiddleNames() const
{
	return middleNames;
}

Standard_Boolean StepBasic_Person::HasMiddleNames() const
{
	return hasMiddleNames;
}

Handle(TCollection_HAsciiString) StepBasic_Person::MiddleNamesValue(const Standard_Integer num) const
{
	return middleNames->Value(num);
}

Standard_Integer StepBasic_Person::NbMiddleNames () const
{
	if (middleNames.IsNull()) return 0;
	return middleNames->Length();
}

void StepBasic_Person::SetPrefixTitles(const Handle(Interface_HArray1OfHAsciiString)& aPrefixTitles)
{
	prefixTitles = aPrefixTitles;
	hasPrefixTitles = Standard_True;
}

void StepBasic_Person::UnSetPrefixTitles()
{
	hasPrefixTitles = Standard_False;
	prefixTitles.Nullify();
}

Handle(Interface_HArray1OfHAsciiString) StepBasic_Person::PrefixTitles() const
{
	return prefixTitles;
}

Standard_Boolean StepBasic_Person::HasPrefixTitles() const
{
	return hasPrefixTitles;
}

Handle(TCollection_HAsciiString) StepBasic_Person::PrefixTitlesValue(const Standard_Integer num) const
{
	return prefixTitles->Value(num);
}

Standard_Integer StepBasic_Person::NbPrefixTitles () const
{
	if (prefixTitles.IsNull()) return 0;
	return prefixTitles->Length();
}

void StepBasic_Person::SetSuffixTitles(const Handle(Interface_HArray1OfHAsciiString)& aSuffixTitles)
{
	suffixTitles = aSuffixTitles;
	hasSuffixTitles = Standard_True;
}

void StepBasic_Person::UnSetSuffixTitles()
{
	hasSuffixTitles = Standard_False;
	suffixTitles.Nullify();
}

Handle(Interface_HArray1OfHAsciiString) StepBasic_Person::SuffixTitles() const
{
	return suffixTitles;
}

Standard_Boolean StepBasic_Person::HasSuffixTitles() const
{
	return hasSuffixTitles;
}

Handle(TCollection_HAsciiString) StepBasic_Person::SuffixTitlesValue(const Standard_Integer num) const
{
	return suffixTitles->Value(num);
}

Standard_Integer StepBasic_Person::NbSuffixTitles () const
{
	if (suffixTitles.IsNull()) return 0;
	return suffixTitles->Length();
}
