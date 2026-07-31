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

#include <StepBasic_Address.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_Address, Standard_Transient)

StepBasic_Address::StepBasic_Address() = default;

void StepBasic_Address::Init(const bool                                   hasAinternalLocation,
                             const occ::handle<TCollection_HAsciiString>& aInternalLocation,
                             const bool                                   hasAstreetNumber,
                             const occ::handle<TCollection_HAsciiString>& aStreetNumber,
                             const bool                                   hasAstreet,
                             const occ::handle<TCollection_HAsciiString>& aStreet,
                             const bool                                   hasApostalBox,
                             const occ::handle<TCollection_HAsciiString>& aPostalBox,
                             const bool                                   hasAtown,
                             const occ::handle<TCollection_HAsciiString>& aTown,
                             const bool                                   hasAregion,
                             const occ::handle<TCollection_HAsciiString>& aRegion,
                             const bool                                   hasApostalCode,
                             const occ::handle<TCollection_HAsciiString>& aPostalCode,
                             const bool                                   hasAcountry,
                             const occ::handle<TCollection_HAsciiString>& aCountry,
                             const bool                                   hasAfacsimileNumber,
                             const occ::handle<TCollection_HAsciiString>& aFacsimileNumber,
                             const bool                                   hasAtelephoneNumber,
                             const occ::handle<TCollection_HAsciiString>& aTelephoneNumber,
                             const bool                                   hasAelectronicMailAddress,
                             const occ::handle<TCollection_HAsciiString>& aElectronicMailAddress,
                             const bool                                   hasAtelexNumber,
                             const occ::handle<TCollection_HAsciiString>& aTelexNumber)
{
  // --- classe own fields ---
  hasInternalLocation      = hasAinternalLocation;
  internalLocation         = aInternalLocation;
  hasStreetNumber          = hasAstreetNumber;
  streetNumber             = aStreetNumber;
  hasStreet                = hasAstreet;
  street                   = aStreet;
  hasPostalBox             = hasApostalBox;
  postalBox                = aPostalBox;
  hasTown                  = hasAtown;
  town                     = aTown;
  hasRegion                = hasAregion;
  region                   = aRegion;
  hasPostalCode            = hasApostalCode;
  postalCode               = aPostalCode;
  hasCountry               = hasAcountry;
  country                  = aCountry;
  hasFacsimileNumber       = hasAfacsimileNumber;
  facsimileNumber          = aFacsimileNumber;
  hasTelephoneNumber       = hasAtelephoneNumber;
  telephoneNumber          = aTelephoneNumber;
  hasElectronicMailAddress = hasAelectronicMailAddress;
  electronicMailAddress    = aElectronicMailAddress;
  hasTelexNumber           = hasAtelexNumber;
  telexNumber              = aTelexNumber;
}

void StepBasic_Address::SetInternalLocation(
  const occ::handle<TCollection_HAsciiString>& aInternalLocation)
{
  internalLocation    = aInternalLocation;
  hasInternalLocation = true;
}

void StepBasic_Address::UnSetInternalLocation()
{
  hasInternalLocation = false;
  internalLocation.Nullify();
}

occ::handle<TCollection_HAsciiString> StepBasic_Address::InternalLocation() const
{
  return internalLocation;
}

bool StepBasic_Address::HasInternalLocation() const
{
  return hasInternalLocation;
}

void StepBasic_Address::SetStreetNumber(const occ::handle<TCollection_HAsciiString>& aStreetNumber)
{
  streetNumber    = aStreetNumber;
  hasStreetNumber = true;
}

void StepBasic_Address::UnSetStreetNumber()
{
  hasStreetNumber = false;
  streetNumber.Nullify();
}

occ::handle<TCollection_HAsciiString> StepBasic_Address::StreetNumber() const
{
  return streetNumber;
}

bool StepBasic_Address::HasStreetNumber() const
{
  return hasStreetNumber;
}

void StepBasic_Address::SetStreet(const occ::handle<TCollection_HAsciiString>& aStreet)
{
  street    = aStreet;
  hasStreet = true;
}

void StepBasic_Address::UnSetStreet()
{
  hasStreet = false;
  street.Nullify();
}

occ::handle<TCollection_HAsciiString> StepBasic_Address::Street() const
{
  return street;
}

bool StepBasic_Address::HasStreet() const
{
  return hasStreet;
}

void StepBasic_Address::SetPostalBox(const occ::handle<TCollection_HAsciiString>& aPostalBox)
{
  postalBox    = aPostalBox;
  hasPostalBox = true;
}

void StepBasic_Address::UnSetPostalBox()
{
  hasPostalBox = false;
  postalBox.Nullify();
}

occ::handle<TCollection_HAsciiString> StepBasic_Address::PostalBox() const
{
  return postalBox;
}

bool StepBasic_Address::HasPostalBox() const
{
  return hasPostalBox;
}

void StepBasic_Address::SetTown(const occ::handle<TCollection_HAsciiString>& aTown)
{
  town    = aTown;
  hasTown = true;
}

void StepBasic_Address::UnSetTown()
{
  hasTown = false;
  town.Nullify();
}

occ::handle<TCollection_HAsciiString> StepBasic_Address::Town() const
{
  return town;
}

bool StepBasic_Address::HasTown() const
{
  return hasTown;
}

void StepBasic_Address::SetRegion(const occ::handle<TCollection_HAsciiString>& aRegion)
{
  region    = aRegion;
  hasRegion = true;
}

void StepBasic_Address::UnSetRegion()
{
  hasRegion = false;
  region.Nullify();
}

occ::handle<TCollection_HAsciiString> StepBasic_Address::Region() const
{
  return region;
}

bool StepBasic_Address::HasRegion() const
{
  return hasRegion;
}

void StepBasic_Address::SetPostalCode(const occ::handle<TCollection_HAsciiString>& aPostalCode)
{
  postalCode    = aPostalCode;
  hasPostalCode = true;
}

void StepBasic_Address::UnSetPostalCode()
{
  hasPostalCode = false;
  postalCode.Nullify();
}

occ::handle<TCollection_HAsciiString> StepBasic_Address::PostalCode() const
{
  return postalCode;
}

bool StepBasic_Address::HasPostalCode() const
{
  return hasPostalCode;
}

void StepBasic_Address::SetCountry(const occ::handle<TCollection_HAsciiString>& aCountry)
{
  country    = aCountry;
  hasCountry = true;
}

void StepBasic_Address::UnSetCountry()
{
  hasCountry = false;
  country.Nullify();
}

occ::handle<TCollection_HAsciiString> StepBasic_Address::Country() const
{
  return country;
}

bool StepBasic_Address::HasCountry() const
{
  return hasCountry;
}

void StepBasic_Address::SetFacsimileNumber(
  const occ::handle<TCollection_HAsciiString>& aFacsimileNumber)
{
  facsimileNumber    = aFacsimileNumber;
  hasFacsimileNumber = true;
}

void StepBasic_Address::UnSetFacsimileNumber()
{
  hasFacsimileNumber = false;
  facsimileNumber.Nullify();
}

occ::handle<TCollection_HAsciiString> StepBasic_Address::FacsimileNumber() const
{
  return facsimileNumber;
}

bool StepBasic_Address::HasFacsimileNumber() const
{
  return hasFacsimileNumber;
}

void StepBasic_Address::SetTelephoneNumber(
  const occ::handle<TCollection_HAsciiString>& aTelephoneNumber)
{
  telephoneNumber    = aTelephoneNumber;
  hasTelephoneNumber = true;
}

void StepBasic_Address::UnSetTelephoneNumber()
{
  hasTelephoneNumber = false;
  telephoneNumber.Nullify();
}

occ::handle<TCollection_HAsciiString> StepBasic_Address::TelephoneNumber() const
{
  return telephoneNumber;
}

bool StepBasic_Address::HasTelephoneNumber() const
{
  return hasTelephoneNumber;
}

void StepBasic_Address::SetElectronicMailAddress(
  const occ::handle<TCollection_HAsciiString>& aElectronicMailAddress)
{
  electronicMailAddress    = aElectronicMailAddress;
  hasElectronicMailAddress = true;
}

void StepBasic_Address::UnSetElectronicMailAddress()
{
  hasElectronicMailAddress = false;
  electronicMailAddress.Nullify();
}

occ::handle<TCollection_HAsciiString> StepBasic_Address::ElectronicMailAddress() const
{
  return electronicMailAddress;
}

bool StepBasic_Address::HasElectronicMailAddress() const
{
  return hasElectronicMailAddress;
}

void StepBasic_Address::SetTelexNumber(const occ::handle<TCollection_HAsciiString>& aTelexNumber)
{
  telexNumber    = aTelexNumber;
  hasTelexNumber = true;
}

void StepBasic_Address::UnSetTelexNumber()
{
  hasTelexNumber = false;
  telexNumber.Nullify();
}

occ::handle<TCollection_HAsciiString> StepBasic_Address::TelexNumber() const
{
  return telexNumber;
}

bool StepBasic_Address::HasTelexNumber() const
{
  return hasTelexNumber;
}
