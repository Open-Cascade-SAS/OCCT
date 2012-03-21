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

#include <StepBasic_PersonalAddress.ixx>


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
	const Handle(TCollection_HAsciiString)& aTelexNumber)
{

	StepBasic_Address::Init(hasAinternalLocation, aInternalLocation, hasAstreetNumber, aStreetNumber, hasAstreet, aStreet, hasApostalBox, aPostalBox, hasAtown, aTown, hasAregion, aRegion, hasApostalCode, aPostalCode, hasAcountry, aCountry, hasAfacsimileNumber, aFacsimileNumber, hasAtelephoneNumber, aTelephoneNumber, hasAelectronicMailAddress, aElectronicMailAddress, hasAtelexNumber, aTelexNumber);
}

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
