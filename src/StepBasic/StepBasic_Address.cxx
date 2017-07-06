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
#include <StepBasic_Address.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_Address,Standard_Transient)

StepBasic_Address::StepBasic_Address ()  {}

void StepBasic_Address::Init(
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
	// --- classe own fields ---
	hasInternalLocation = hasAinternalLocation;
	internalLocation = aInternalLocation;
	hasStreetNumber = hasAstreetNumber;
	streetNumber = aStreetNumber;
	hasStreet = hasAstreet;
	street = aStreet;
	hasPostalBox = hasApostalBox;
	postalBox = aPostalBox;
	hasTown = hasAtown;
	town = aTown;
	hasRegion = hasAregion;
	region = aRegion;
	hasPostalCode = hasApostalCode;
	postalCode = aPostalCode;
	hasCountry = hasAcountry;
	country = aCountry;
	hasFacsimileNumber = hasAfacsimileNumber;
	facsimileNumber = aFacsimileNumber;
	hasTelephoneNumber = hasAtelephoneNumber;
	telephoneNumber = aTelephoneNumber;
	hasElectronicMailAddress = hasAelectronicMailAddress;
	electronicMailAddress = aElectronicMailAddress;
	hasTelexNumber = hasAtelexNumber;
	telexNumber = aTelexNumber;
}


void StepBasic_Address::SetInternalLocation(const Handle(TCollection_HAsciiString)& aInternalLocation)
{
	internalLocation = aInternalLocation;
	hasInternalLocation = Standard_True;
}

void StepBasic_Address::UnSetInternalLocation()
{
	hasInternalLocation = Standard_False;
	internalLocation.Nullify();
}

Handle(TCollection_HAsciiString) StepBasic_Address::InternalLocation() const
{
	return internalLocation;
}

Standard_Boolean StepBasic_Address::HasInternalLocation() const
{
	return hasInternalLocation;
}

void StepBasic_Address::SetStreetNumber(const Handle(TCollection_HAsciiString)& aStreetNumber)
{
	streetNumber = aStreetNumber;
	hasStreetNumber = Standard_True;
}

void StepBasic_Address::UnSetStreetNumber()
{
	hasStreetNumber = Standard_False;
	streetNumber.Nullify();
}

Handle(TCollection_HAsciiString) StepBasic_Address::StreetNumber() const
{
	return streetNumber;
}

Standard_Boolean StepBasic_Address::HasStreetNumber() const
{
	return hasStreetNumber;
}

void StepBasic_Address::SetStreet(const Handle(TCollection_HAsciiString)& aStreet)
{
	street = aStreet;
	hasStreet = Standard_True;
}

void StepBasic_Address::UnSetStreet()
{
	hasStreet = Standard_False;
	street.Nullify();
}

Handle(TCollection_HAsciiString) StepBasic_Address::Street() const
{
	return street;
}

Standard_Boolean StepBasic_Address::HasStreet() const
{
	return hasStreet;
}

void StepBasic_Address::SetPostalBox(const Handle(TCollection_HAsciiString)& aPostalBox)
{
	postalBox = aPostalBox;
	hasPostalBox = Standard_True;
}

void StepBasic_Address::UnSetPostalBox()
{
	hasPostalBox = Standard_False;
	postalBox.Nullify();
}

Handle(TCollection_HAsciiString) StepBasic_Address::PostalBox() const
{
	return postalBox;
}

Standard_Boolean StepBasic_Address::HasPostalBox() const
{
	return hasPostalBox;
}

void StepBasic_Address::SetTown(const Handle(TCollection_HAsciiString)& aTown)
{
	town = aTown;
	hasTown = Standard_True;
}

void StepBasic_Address::UnSetTown()
{
	hasTown = Standard_False;
	town.Nullify();
}

Handle(TCollection_HAsciiString) StepBasic_Address::Town() const
{
	return town;
}

Standard_Boolean StepBasic_Address::HasTown() const
{
	return hasTown;
}

void StepBasic_Address::SetRegion(const Handle(TCollection_HAsciiString)& aRegion)
{
	region = aRegion;
	hasRegion = Standard_True;
}

void StepBasic_Address::UnSetRegion()
{
	hasRegion = Standard_False;
	region.Nullify();
}

Handle(TCollection_HAsciiString) StepBasic_Address::Region() const
{
	return region;
}

Standard_Boolean StepBasic_Address::HasRegion() const
{
	return hasRegion;
}

void StepBasic_Address::SetPostalCode(const Handle(TCollection_HAsciiString)& aPostalCode)
{
	postalCode = aPostalCode;
	hasPostalCode = Standard_True;
}

void StepBasic_Address::UnSetPostalCode()
{
	hasPostalCode = Standard_False;
	postalCode.Nullify();
}

Handle(TCollection_HAsciiString) StepBasic_Address::PostalCode() const
{
	return postalCode;
}

Standard_Boolean StepBasic_Address::HasPostalCode() const
{
	return hasPostalCode;
}

void StepBasic_Address::SetCountry(const Handle(TCollection_HAsciiString)& aCountry)
{
	country = aCountry;
	hasCountry = Standard_True;
}

void StepBasic_Address::UnSetCountry()
{
	hasCountry = Standard_False;
	country.Nullify();
}

Handle(TCollection_HAsciiString) StepBasic_Address::Country() const
{
	return country;
}

Standard_Boolean StepBasic_Address::HasCountry() const
{
	return hasCountry;
}

void StepBasic_Address::SetFacsimileNumber(const Handle(TCollection_HAsciiString)& aFacsimileNumber)
{
	facsimileNumber = aFacsimileNumber;
	hasFacsimileNumber = Standard_True;
}

void StepBasic_Address::UnSetFacsimileNumber()
{
	hasFacsimileNumber = Standard_False;
	facsimileNumber.Nullify();
}

Handle(TCollection_HAsciiString) StepBasic_Address::FacsimileNumber() const
{
	return facsimileNumber;
}

Standard_Boolean StepBasic_Address::HasFacsimileNumber() const
{
	return hasFacsimileNumber;
}

void StepBasic_Address::SetTelephoneNumber(const Handle(TCollection_HAsciiString)& aTelephoneNumber)
{
	telephoneNumber = aTelephoneNumber;
	hasTelephoneNumber = Standard_True;
}

void StepBasic_Address::UnSetTelephoneNumber()
{
	hasTelephoneNumber = Standard_False;
	telephoneNumber.Nullify();
}

Handle(TCollection_HAsciiString) StepBasic_Address::TelephoneNumber() const
{
	return telephoneNumber;
}

Standard_Boolean StepBasic_Address::HasTelephoneNumber() const
{
	return hasTelephoneNumber;
}

void StepBasic_Address::SetElectronicMailAddress(const Handle(TCollection_HAsciiString)& aElectronicMailAddress)
{
	electronicMailAddress = aElectronicMailAddress;
	hasElectronicMailAddress = Standard_True;
}

void StepBasic_Address::UnSetElectronicMailAddress()
{
	hasElectronicMailAddress = Standard_False;
	electronicMailAddress.Nullify();
}

Handle(TCollection_HAsciiString) StepBasic_Address::ElectronicMailAddress() const
{
	return electronicMailAddress;
}

Standard_Boolean StepBasic_Address::HasElectronicMailAddress() const
{
	return hasElectronicMailAddress;
}

void StepBasic_Address::SetTelexNumber(const Handle(TCollection_HAsciiString)& aTelexNumber)
{
	telexNumber = aTelexNumber;
	hasTelexNumber = Standard_True;
}

void StepBasic_Address::UnSetTelexNumber()
{
	hasTelexNumber = Standard_False;
	telexNumber.Nullify();
}

Handle(TCollection_HAsciiString) StepBasic_Address::TelexNumber() const
{
	return telexNumber;
}

Standard_Boolean StepBasic_Address::HasTelexNumber() const
{
	return hasTelexNumber;
}
