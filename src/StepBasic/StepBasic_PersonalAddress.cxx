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
#include <StepBasic_PersonalAddress.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_PersonalAddress,StepBasic_Address)

StepBasic_PersonalAddress::StepBasic_PersonalAddress ()  {}

void StepBasic_PersonalAddress::Init(
	const Standard_Boolean hasAinternalLocation,
	const Handle(TCollection_HAsciiString)& aInternalLocation,
	const Standard_Boolean hasAstreetNumber,
	const Handle(TCollection_HAsciiString)& aStreetNumber,
	const Standard_Boolean hasAstreet,
	const Handle(TCollection_HAsciiString)& aStreet,
	const Standard_Boolean hasApostalBox,
	const Handle(TCollection_HAsciiString)& aPostalBox,
	const Standard_Boolean hasAtown,
	const Handle(TCollection_HAsciiString)& aTown,
	const Standard_Boolean hasAregion,
	const Handle(TCollection_HAsciiString)& aRegion,
	const Standard_Boolean hasApostalCode,
	const Handle(TCollection_HAsciiString)& aPostalCode,
	const Standard_Boolean hasAcountry,
	const Handle(TCollection_HAsciiString)& aCountry,
	const Standard_Boolean hasAfacsimileNumber,
	const Handle(TCollection_HAsciiString)& aFacsimileNumber,
	const Standard_Boolean hasAtelephoneNumber,
	const Handle(TCollection_HAsciiString)& aTelephoneNumber,
	const Standard_Boolean hasAelectronicMailAddress,
	const Handle(TCollection_HAsciiString)& aElectronicMailAddress,
	const Standard_Boolean hasAtelexNumber,
	const Handle(TCollection_HAsciiString)& aTelexNumber,
	const Handle(StepBasic_HArray1OfPerson)& aPeople,
	const Handle(TCollection_HAsciiString)& aDescription)
{
	// --- classe own fields ---
	people = aPeople;
	description = aDescription;
	// --- classe inherited fields ---
	StepBasic_Address::Init(hasAinternalLocation, aInternalLocation, hasAstreetNumber, aStreetNumber, hasAstreet, aStreet, hasApostalBox, aPostalBox, hasAtown, aTown, hasAregion, aRegion, hasApostalCode, aPostalCode, hasAcountry, aCountry, hasAfacsimileNumber, aFacsimileNumber, hasAtelephoneNumber, aTelephoneNumber, hasAelectronicMailAddress, aElectronicMailAddress, hasAtelexNumber, aTelexNumber);
}


void StepBasic_PersonalAddress::SetPeople(const Handle(StepBasic_HArray1OfPerson)& aPeople)
{
	people = aPeople;
}

Handle(StepBasic_HArray1OfPerson) StepBasic_PersonalAddress::People() const
{
	return people;
}

Handle(StepBasic_Person) StepBasic_PersonalAddress::PeopleValue(const Standard_Integer num) const
{
	return people->Value(num);
}

Standard_Integer StepBasic_PersonalAddress::NbPeople () const
{
	if (people.IsNull()) return 0;
	return people->Length();
}

void StepBasic_PersonalAddress::SetDescription(const Handle(TCollection_HAsciiString)& aDescription)
{
	description = aDescription;
}

Handle(TCollection_HAsciiString) StepBasic_PersonalAddress::Description() const
{
	return description;
}
