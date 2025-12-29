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

// gka 05.03.99 S4134 upgrade from CD to DIS

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include "RWStepBasic_RWPersonalAddress.pxx"
#include <StepBasic_Person.hxx>
#include <StepBasic_PersonalAddress.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

RWStepBasic_RWPersonalAddress::RWStepBasic_RWPersonalAddress() = default;

void RWStepBasic_RWPersonalAddress::ReadStep(
  const occ::handle<StepData_StepReaderData>&   data,
  const int                                     num,
  occ::handle<Interface_Check>&                 ach,
  const occ::handle<StepBasic_PersonalAddress>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 14, ach, "personal_address"))
    return;

  // --- inherited field : internalLocation ---

  occ::handle<TCollection_HAsciiString> aInternalLocation;
  bool                                  hasAinternalLocation = true;
  if (data->IsParamDefined(num, 1))
  {
    // szv#4:S4163:12Mar99 `bool stat1 =` not needed
    data->ReadString(num, 1, "internal_location", ach, aInternalLocation);
  }
  else
  {
    hasAinternalLocation = false;
    aInternalLocation.Nullify();
  }

  // --- inherited field : streetNumber ---

  occ::handle<TCollection_HAsciiString> aStreetNumber;
  bool                                  hasAstreetNumber = true;
  if (data->IsParamDefined(num, 2))
  {
    // szv#4:S4163:12Mar99 `bool stat2 =` not needed
    data->ReadString(num, 2, "street_number", ach, aStreetNumber);
  }
  else
  {
    hasAstreetNumber = false;
    aStreetNumber.Nullify();
  }

  // --- inherited field : street ---

  occ::handle<TCollection_HAsciiString> aStreet;
  bool                                  hasAstreet = true;
  if (data->IsParamDefined(num, 3))
  {
    // szv#4:S4163:12Mar99 `bool stat3 =` not needed
    data->ReadString(num, 3, "street", ach, aStreet);
  }
  else
  {
    hasAstreet = false;
    aStreet.Nullify();
  }

  // --- inherited field : postalBox ---

  occ::handle<TCollection_HAsciiString> aPostalBox;
  bool                                  hasApostalBox = true;
  if (data->IsParamDefined(num, 4))
  {
    // szv#4:S4163:12Mar99 `bool stat4 =` not needed
    data->ReadString(num, 4, "postal_box", ach, aPostalBox);
  }
  else
  {
    hasApostalBox = false;
    aPostalBox.Nullify();
  }

  // --- inherited field : town ---

  occ::handle<TCollection_HAsciiString> aTown;
  bool                                  hasAtown = true;
  if (data->IsParamDefined(num, 5))
  {
    // szv#4:S4163:12Mar99 `bool stat5 =` not needed
    data->ReadString(num, 5, "town", ach, aTown);
  }
  else
  {
    hasAtown = false;
    aTown.Nullify();
  }

  // --- inherited field : region ---

  occ::handle<TCollection_HAsciiString> aRegion;
  bool                                  hasAregion = true;
  if (data->IsParamDefined(num, 6))
  {
    // szv#4:S4163:12Mar99 `bool stat6 =` not needed
    data->ReadString(num, 6, "region", ach, aRegion);
  }
  else
  {
    hasAregion = false;
    aRegion.Nullify();
  }

  // --- inherited field : postalCode ---

  occ::handle<TCollection_HAsciiString> aPostalCode;
  bool                                  hasApostalCode = true;
  if (data->IsParamDefined(num, 7))
  {
    // szv#4:S4163:12Mar99 `bool stat7 =` not needed
    data->ReadString(num, 7, "postal_code", ach, aPostalCode);
  }
  else
  {
    hasApostalCode = false;
    aPostalCode.Nullify();
  }

  // --- inherited field : country ---

  occ::handle<TCollection_HAsciiString> aCountry;
  bool                                  hasAcountry = true;
  if (data->IsParamDefined(num, 8))
  {
    // szv#4:S4163:12Mar99 `bool stat8 =` not needed
    data->ReadString(num, 8, "country", ach, aCountry);
  }
  else
  {
    hasAcountry = false;
    aCountry.Nullify();
  }

  // --- inherited field : facsimileNumber ---

  occ::handle<TCollection_HAsciiString> aFacsimileNumber;
  bool                                  hasAfacsimileNumber = true;
  if (data->IsParamDefined(num, 9))
  {
    // szv#4:S4163:12Mar99 `bool stat9 =` not needed
    data->ReadString(num, 9, "facsimile_number", ach, aFacsimileNumber);
  }
  else
  {
    hasAfacsimileNumber = false;
    aFacsimileNumber.Nullify();
  }

  // --- inherited field : telephoneNumber ---

  occ::handle<TCollection_HAsciiString> aTelephoneNumber;
  bool                                  hasAtelephoneNumber = true;
  if (data->IsParamDefined(num, 10))
  {
    // szv#4:S4163:12Mar99 `bool stat10 =` not needed
    data->ReadString(num, 10, "telephone_number", ach, aTelephoneNumber);
  }
  else
  {
    hasAtelephoneNumber = false;
    aTelephoneNumber.Nullify();
  }

  // --- inherited field : electronicMailAddress ---

  occ::handle<TCollection_HAsciiString> aElectronicMailAddress;
  bool                                  hasAelectronicMailAddress = true;
  if (data->IsParamDefined(num, 11))
  {
    // szv#4:S4163:12Mar99 `bool stat11 =` not needed
    data->ReadString(num, 11, "electronic_mail_address", ach, aElectronicMailAddress);
  }
  else
  {
    hasAelectronicMailAddress = false;
    aElectronicMailAddress.Nullify();
  }

  // --- inherited field : telexNumber ---

  occ::handle<TCollection_HAsciiString> aTelexNumber;
  bool                                  hasAtelexNumber = true;
  if (data->IsParamDefined(num, 12))
  {
    // szv#4:S4163:12Mar99 `bool stat12 =` not needed
    data->ReadString(num, 12, "telex_number", ach, aTelexNumber);
  }
  else
  {
    hasAtelexNumber = false;
    aTelexNumber.Nullify();
  }

  // --- own field : people ---

  occ::handle<NCollection_HArray1<occ::handle<StepBasic_Person>>> aPeople;
  occ::handle<StepBasic_Person>                                   anent13;
  int                                                             nsub13;
  if (data->ReadSubList(num, 13, "people", ach, nsub13))
  {
    int nb13 = data->NbParams(nsub13);
    aPeople  = new NCollection_HArray1<occ::handle<StepBasic_Person>>(1, nb13);
    for (int i13 = 1; i13 <= nb13; i13++)
    {
      // szv#4:S4163:12Mar99 `bool stat13 =` not needed
      if (data->ReadEntity(nsub13, i13, "person", ach, STANDARD_TYPE(StepBasic_Person), anent13))
        aPeople->SetValue(i13, anent13);
    }
  }

  // --- own field : description ---

  occ::handle<TCollection_HAsciiString> aDescription;
  if (data->IsParamDefined(num, 14))
  { // gka 05.03.99 S4134 upgrade from CD to DIS
    // szv#4:S4163:12Mar99 `bool stat14 =` not needed
    data->ReadString(num, 14, "description", ach, aDescription);
  }
  //--- Initialisation of the read entity ---

  ent->Init(hasAinternalLocation,
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
            aTelexNumber,
            aPeople,
            aDescription);
}

void RWStepBasic_RWPersonalAddress::WriteStep(
  StepData_StepWriter&                          SW,
  const occ::handle<StepBasic_PersonalAddress>& ent) const
{

  // --- inherited field internalLocation ---

  bool hasAinternalLocation = ent->HasInternalLocation();
  if (hasAinternalLocation)
  {
    SW.Send(ent->InternalLocation());
  }
  else
  {
    SW.SendUndef();
  }

  // --- inherited field streetNumber ---

  bool hasAstreetNumber = ent->HasStreetNumber();
  if (hasAstreetNumber)
  {
    SW.Send(ent->StreetNumber());
  }
  else
  {
    SW.SendUndef();
  }

  // --- inherited field street ---

  bool hasAstreet = ent->HasStreet();
  if (hasAstreet)
  {
    SW.Send(ent->Street());
  }
  else
  {
    SW.SendUndef();
  }

  // --- inherited field postalBox ---

  bool hasApostalBox = ent->HasPostalBox();
  if (hasApostalBox)
  {
    SW.Send(ent->PostalBox());
  }
  else
  {
    SW.SendUndef();
  }

  // --- inherited field town ---

  bool hasAtown = ent->HasTown();
  if (hasAtown)
  {
    SW.Send(ent->Town());
  }
  else
  {
    SW.SendUndef();
  }

  // --- inherited field region ---

  bool hasAregion = ent->HasRegion();
  if (hasAregion)
  {
    SW.Send(ent->Region());
  }
  else
  {
    SW.SendUndef();
  }

  // --- inherited field postalCode ---

  bool hasApostalCode = ent->HasPostalCode();
  if (hasApostalCode)
  {
    SW.Send(ent->PostalCode());
  }
  else
  {
    SW.SendUndef();
  }

  // --- inherited field country ---

  bool hasAcountry = ent->HasCountry();
  if (hasAcountry)
  {
    SW.Send(ent->Country());
  }
  else
  {
    SW.SendUndef();
  }

  // --- inherited field facsimileNumber ---

  bool hasAfacsimileNumber = ent->HasFacsimileNumber();
  if (hasAfacsimileNumber)
  {
    SW.Send(ent->FacsimileNumber());
  }
  else
  {
    SW.SendUndef();
  }

  // --- inherited field telephoneNumber ---

  bool hasAtelephoneNumber = ent->HasTelephoneNumber();
  if (hasAtelephoneNumber)
  {
    SW.Send(ent->TelephoneNumber());
  }
  else
  {
    SW.SendUndef();
  }

  // --- inherited field electronicMailAddress ---

  bool hasAelectronicMailAddress = ent->HasElectronicMailAddress();
  if (hasAelectronicMailAddress)
  {
    SW.Send(ent->ElectronicMailAddress());
  }
  else
  {
    SW.SendUndef();
  }

  // --- inherited field telexNumber ---

  bool hasAtelexNumber = ent->HasTelexNumber();
  if (hasAtelexNumber)
  {
    SW.Send(ent->TelexNumber());
  }
  else
  {
    SW.SendUndef();
  }

  // --- own field : people ---

  SW.OpenSub();
  for (int i13 = 1; i13 <= ent->NbPeople(); i13++)
  {
    SW.Send(ent->PeopleValue(i13));
  }
  SW.CloseSub();

  // --- own field : description ---

  SW.Send(ent->Description());
}

void RWStepBasic_RWPersonalAddress::Share(const occ::handle<StepBasic_PersonalAddress>& ent,
                                          Interface_EntityIterator&                     iter) const
{

  int nbElem1 = ent->NbPeople();
  for (int is1 = 1; is1 <= nbElem1; is1++)
  {
    iter.GetOneItem(ent->PeopleValue(is1));
  }
}
