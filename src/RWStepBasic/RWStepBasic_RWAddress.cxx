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


#include <Interface_Check.hxx>
#include <RWStepBasic_RWAddress.hxx>
#include <StepBasic_Address.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

RWStepBasic_RWAddress::RWStepBasic_RWAddress () {}

void RWStepBasic_RWAddress::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_Address)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,12,ach,"address")) return;
	// --- own field : internalLocation ---

	Handle(TCollection_HAsciiString) aInternalLocation;
	Standard_Boolean hasAinternalLocation = Standard_True;
	if (data->IsParamDefined(num,1)) {
	  //szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	  data->ReadString (num,1,"internal_location",ach,aInternalLocation);
	}
	else {
	  hasAinternalLocation = Standard_False;
	  aInternalLocation.Nullify();
	}

	// --- own field : streetNumber ---

	Handle(TCollection_HAsciiString) aStreetNumber;
	Standard_Boolean hasAstreetNumber = Standard_True;
	if (data->IsParamDefined(num,2)) {
	  //szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	  data->ReadString (num,2,"street_number",ach,aStreetNumber);
	}
	else {
	  hasAstreetNumber = Standard_False;
	  aStreetNumber.Nullify();
	}

	// --- own field : street ---

	Handle(TCollection_HAsciiString) aStreet;
	Standard_Boolean hasAstreet = Standard_True;
	if (data->IsParamDefined(num,3)) {
	  //szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	  data->ReadString (num,3,"street",ach,aStreet);
	}
	else {
	  hasAstreet = Standard_False;
	  aStreet.Nullify();
	}

	// --- own field : postalBox ---

	Handle(TCollection_HAsciiString) aPostalBox;
	Standard_Boolean hasApostalBox = Standard_True;
	if (data->IsParamDefined(num,4)) {
	  //szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	  data->ReadString (num,4,"postal_box",ach,aPostalBox);
	}
	else {
	  hasApostalBox = Standard_False;
	  aPostalBox.Nullify();
	}

	// --- own field : town ---

	Handle(TCollection_HAsciiString) aTown;
	Standard_Boolean hasAtown = Standard_True;
	if (data->IsParamDefined(num,5)) {
	  //szv#4:S4163:12Mar99 `Standard_Boolean stat5 =` not needed
	  data->ReadString (num,5,"town",ach,aTown);
	}
	else {
	  hasAtown = Standard_False;
	  aTown.Nullify();
	}

	// --- own field : region ---

	Handle(TCollection_HAsciiString) aRegion;
	Standard_Boolean hasAregion = Standard_True;
	if (data->IsParamDefined(num,6)) {
	  //szv#4:S4163:12Mar99 `Standard_Boolean stat6 =` not needed
	  data->ReadString (num,6,"region",ach,aRegion);
	}
	else {
	  hasAregion = Standard_False;
	  aRegion.Nullify();
	}

	// --- own field : postalCode ---

	Handle(TCollection_HAsciiString) aPostalCode;
	Standard_Boolean hasApostalCode = Standard_True;
	if (data->IsParamDefined(num,7)) {
	  //szv#4:S4163:12Mar99 `Standard_Boolean stat7 =` not needed
	  data->ReadString (num,7,"postal_code",ach,aPostalCode);
	}
	else {
	  hasApostalCode = Standard_False;
	  aPostalCode.Nullify();
	}

	// --- own field : country ---

	Handle(TCollection_HAsciiString) aCountry;
	Standard_Boolean hasAcountry = Standard_True;
	if (data->IsParamDefined(num,8)) {
	  //szv#4:S4163:12Mar99 `Standard_Boolean stat8 =` not needed
	  data->ReadString (num,8,"country",ach,aCountry);
	}
	else {
	  hasAcountry = Standard_False;
	  aCountry.Nullify();
	}

	// --- own field : facsimileNumber ---

	Handle(TCollection_HAsciiString) aFacsimileNumber;
	Standard_Boolean hasAfacsimileNumber = Standard_True;
	if (data->IsParamDefined(num,9)) {
	  //szv#4:S4163:12Mar99 `Standard_Boolean stat9 =` not needed
	  data->ReadString (num,9,"facsimile_number",ach,aFacsimileNumber);
	}
	else {
	  hasAfacsimileNumber = Standard_False;
	  aFacsimileNumber.Nullify();
	}

	// --- own field : telephoneNumber ---

	Handle(TCollection_HAsciiString) aTelephoneNumber;
	Standard_Boolean hasAtelephoneNumber = Standard_True;
	if (data->IsParamDefined(num,10)) {
	  //szv#4:S4163:12Mar99 `Standard_Boolean stat10 =` not needed
	  data->ReadString (num,10,"telephone_number",ach,aTelephoneNumber);
	}
	else {
	  hasAtelephoneNumber = Standard_False;
	  aTelephoneNumber.Nullify();
	}

	// --- own field : electronicMailAddress ---

	Handle(TCollection_HAsciiString) aElectronicMailAddress;
	Standard_Boolean hasAelectronicMailAddress = Standard_True;
	if (data->IsParamDefined(num,11)) {
	  //szv#4:S4163:12Mar99 `Standard_Boolean stat11 =` not needed
	  data->ReadString (num,11,"electronic_mail_address",ach,aElectronicMailAddress);
	}
	else {
	  hasAelectronicMailAddress = Standard_False;
	  aElectronicMailAddress.Nullify();
	}

	// --- own field : telexNumber ---

	Handle(TCollection_HAsciiString) aTelexNumber;
	Standard_Boolean hasAtelexNumber = Standard_True;
	if (data->IsParamDefined(num,12)) {
	  //szv#4:S4163:12Mar99 `Standard_Boolean stat12 =` not needed
	  data->ReadString (num,12,"telex_number",ach,aTelexNumber);
	}
	else {
	  hasAtelexNumber = Standard_False;
	  aTelexNumber.Nullify();
	}

	//--- Initialisation of the read entity ---


	ent->Init(hasAinternalLocation, aInternalLocation, hasAstreetNumber, aStreetNumber, hasAstreet, aStreet, hasApostalBox, aPostalBox, hasAtown, aTown, hasAregion, aRegion, hasApostalCode, aPostalCode, hasAcountry, aCountry, hasAfacsimileNumber, aFacsimileNumber, hasAtelephoneNumber, aTelephoneNumber, hasAelectronicMailAddress, aElectronicMailAddress, hasAtelexNumber, aTelexNumber);
}


void RWStepBasic_RWAddress::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_Address)& ent) const
{

	// --- own field : internalLocation ---

	Standard_Boolean hasAinternalLocation = ent->HasInternalLocation();
	if (hasAinternalLocation) {
	  SW.Send(ent->InternalLocation());
	}
	else {
	  SW.SendUndef();
	}

	// --- own field : streetNumber ---

	Standard_Boolean hasAstreetNumber = ent->HasStreetNumber();
	if (hasAstreetNumber) {
	  SW.Send(ent->StreetNumber());
	}
	else {
	  SW.SendUndef();
	}

	// --- own field : street ---

	Standard_Boolean hasAstreet = ent->HasStreet();
	if (hasAstreet) {
	  SW.Send(ent->Street());
	}
	else {
	  SW.SendUndef();
	}

	// --- own field : postalBox ---

	Standard_Boolean hasApostalBox = ent->HasPostalBox();
	if (hasApostalBox) {
	  SW.Send(ent->PostalBox());
	}
	else {
	  SW.SendUndef();
	}

	// --- own field : town ---

	Standard_Boolean hasAtown = ent->HasTown();
	if (hasAtown) {
	  SW.Send(ent->Town());
	}
	else {
	  SW.SendUndef();
	}

	// --- own field : region ---

	Standard_Boolean hasAregion = ent->HasRegion();
	if (hasAregion) {
	  SW.Send(ent->Region());
	}
	else {
	  SW.SendUndef();
	}

	// --- own field : postalCode ---

	Standard_Boolean hasApostalCode = ent->HasPostalCode();
	if (hasApostalCode) {
	  SW.Send(ent->PostalCode());
	}
	else {
	  SW.SendUndef();
	}

	// --- own field : country ---

	Standard_Boolean hasAcountry = ent->HasCountry();
	if (hasAcountry) {
	  SW.Send(ent->Country());
	}
	else {
	  SW.SendUndef();
	}

	// --- own field : facsimileNumber ---

	Standard_Boolean hasAfacsimileNumber = ent->HasFacsimileNumber();
	if (hasAfacsimileNumber) {
	  SW.Send(ent->FacsimileNumber());
	}
	else {
	  SW.SendUndef();
	}

	// --- own field : telephoneNumber ---

	Standard_Boolean hasAtelephoneNumber = ent->HasTelephoneNumber();
	if (hasAtelephoneNumber) {
	  SW.Send(ent->TelephoneNumber());
	}
	else {
	  SW.SendUndef();
	}

	// --- own field : electronicMailAddress ---

	Standard_Boolean hasAelectronicMailAddress = ent->HasElectronicMailAddress();
	if (hasAelectronicMailAddress) {
	  SW.Send(ent->ElectronicMailAddress());
	}
	else {
	  SW.SendUndef();
	}

	// --- own field : telexNumber ---

	Standard_Boolean hasAtelexNumber = ent->HasTelexNumber();
	if (hasAtelexNumber) {
	  SW.Send(ent->TelexNumber());
	}
	else {
	  SW.SendUndef();
	}
}
