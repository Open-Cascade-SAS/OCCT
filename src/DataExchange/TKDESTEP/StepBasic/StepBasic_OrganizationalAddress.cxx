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

#include <StepBasic_OrganizationalAddress.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_OrganizationalAddress, StepBasic_Address)

StepBasic_OrganizationalAddress::StepBasic_OrganizationalAddress() {}

void StepBasic_OrganizationalAddress::Init(
  const bool                         hasAinternalLocation,
  const occ::handle<TCollection_HAsciiString>&        aInternalLocation,
  const bool                         hasAstreetNumber,
  const occ::handle<TCollection_HAsciiString>&        aStreetNumber,
  const bool                         hasAstreet,
  const occ::handle<TCollection_HAsciiString>&        aStreet,
  const bool                         hasApostalBox,
  const occ::handle<TCollection_HAsciiString>&        aPostalBox,
  const bool                         hasAtown,
  const occ::handle<TCollection_HAsciiString>&        aTown,
  const bool                         hasAregion,
  const occ::handle<TCollection_HAsciiString>&        aRegion,
  const bool                         hasApostalCode,
  const occ::handle<TCollection_HAsciiString>&        aPostalCode,
  const bool                         hasAcountry,
  const occ::handle<TCollection_HAsciiString>&        aCountry,
  const bool                         hasAfacsimileNumber,
  const occ::handle<TCollection_HAsciiString>&        aFacsimileNumber,
  const bool                         hasAtelephoneNumber,
  const occ::handle<TCollection_HAsciiString>&        aTelephoneNumber,
  const bool                         hasAelectronicMailAddress,
  const occ::handle<TCollection_HAsciiString>&        aElectronicMailAddress,
  const bool                         hasAtelexNumber,
  const occ::handle<TCollection_HAsciiString>&        aTelexNumber,
  const occ::handle<NCollection_HArray1<occ::handle<StepBasic_Organization>>>& aOrganizations,
  const occ::handle<TCollection_HAsciiString>&        aDescription)
{
  // --- classe own fields ---
  organizations = aOrganizations;
  description   = aDescription;
  // --- classe inherited fields ---
  StepBasic_Address::Init(hasAinternalLocation,
                          aInternalLocation,
                          hasAstreetNumber,
                          aStreetNumber,
                          hasAstreet,
                          aStreet,
                          hasApostalBox,
                          aPostalBox,
                          hasAtown,
                          aTown,
                          hasAregion,
                          aRegion,
                          hasApostalCode,
                          aPostalCode,
                          hasAcountry,
                          aCountry,
                          hasAfacsimileNumber,
                          aFacsimileNumber,
                          hasAtelephoneNumber,
                          aTelephoneNumber,
                          hasAelectronicMailAddress,
                          aElectronicMailAddress,
                          hasAtelexNumber,
                          aTelexNumber);
}

void StepBasic_OrganizationalAddress::SetOrganizations(
  const occ::handle<NCollection_HArray1<occ::handle<StepBasic_Organization>>>& aOrganizations)
{
  organizations = aOrganizations;
}

occ::handle<NCollection_HArray1<occ::handle<StepBasic_Organization>>> StepBasic_OrganizationalAddress::Organizations() const
{
  return organizations;
}

occ::handle<StepBasic_Organization> StepBasic_OrganizationalAddress::OrganizationsValue(
  const int num) const
{
  return organizations->Value(num);
}

int StepBasic_OrganizationalAddress::NbOrganizations() const
{
  if (organizations.IsNull())
    return 0;
  return organizations->Length();
}

void StepBasic_OrganizationalAddress::SetDescription(
  const occ::handle<TCollection_HAsciiString>& aDescription)
{
  description = aDescription;
}

occ::handle<TCollection_HAsciiString> StepBasic_OrganizationalAddress::Description() const
{
  return description;
}
