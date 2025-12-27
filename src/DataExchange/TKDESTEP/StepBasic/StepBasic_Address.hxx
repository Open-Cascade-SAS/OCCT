// Created on: 1995-12-04
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _StepBasic_Address_HeaderFile
#define _StepBasic_Address_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Transient.hxx>
class TCollection_HAsciiString;

class StepBasic_Address : public Standard_Transient
{

public:
  //! Returns a Address
  Standard_EXPORT StepBasic_Address();

  Standard_EXPORT void Init(const bool                  hasAinternalLocation,
                            const occ::handle<TCollection_HAsciiString>& aInternalLocation,
                            const bool                  hasAstreetNumber,
                            const occ::handle<TCollection_HAsciiString>& aStreetNumber,
                            const bool                  hasAstreet,
                            const occ::handle<TCollection_HAsciiString>& aStreet,
                            const bool                  hasApostalBox,
                            const occ::handle<TCollection_HAsciiString>& aPostalBox,
                            const bool                  hasAtown,
                            const occ::handle<TCollection_HAsciiString>& aTown,
                            const bool                  hasAregion,
                            const occ::handle<TCollection_HAsciiString>& aRegion,
                            const bool                  hasApostalCode,
                            const occ::handle<TCollection_HAsciiString>& aPostalCode,
                            const bool                  hasAcountry,
                            const occ::handle<TCollection_HAsciiString>& aCountry,
                            const bool                  hasAfacsimileNumber,
                            const occ::handle<TCollection_HAsciiString>& aFacsimileNumber,
                            const bool                  hasAtelephoneNumber,
                            const occ::handle<TCollection_HAsciiString>& aTelephoneNumber,
                            const bool                  hasAelectronicMailAddress,
                            const occ::handle<TCollection_HAsciiString>& aElectronicMailAddress,
                            const bool                  hasAtelexNumber,
                            const occ::handle<TCollection_HAsciiString>& aTelexNumber);

  Standard_EXPORT void SetInternalLocation(
    const occ::handle<TCollection_HAsciiString>& aInternalLocation);

  Standard_EXPORT void UnSetInternalLocation();

  Standard_EXPORT occ::handle<TCollection_HAsciiString> InternalLocation() const;

  Standard_EXPORT bool HasInternalLocation() const;

  Standard_EXPORT void SetStreetNumber(const occ::handle<TCollection_HAsciiString>& aStreetNumber);

  Standard_EXPORT void UnSetStreetNumber();

  Standard_EXPORT occ::handle<TCollection_HAsciiString> StreetNumber() const;

  Standard_EXPORT bool HasStreetNumber() const;

  Standard_EXPORT void SetStreet(const occ::handle<TCollection_HAsciiString>& aStreet);

  Standard_EXPORT void UnSetStreet();

  Standard_EXPORT occ::handle<TCollection_HAsciiString> Street() const;

  Standard_EXPORT bool HasStreet() const;

  Standard_EXPORT void SetPostalBox(const occ::handle<TCollection_HAsciiString>& aPostalBox);

  Standard_EXPORT void UnSetPostalBox();

  Standard_EXPORT occ::handle<TCollection_HAsciiString> PostalBox() const;

  Standard_EXPORT bool HasPostalBox() const;

  Standard_EXPORT void SetTown(const occ::handle<TCollection_HAsciiString>& aTown);

  Standard_EXPORT void UnSetTown();

  Standard_EXPORT occ::handle<TCollection_HAsciiString> Town() const;

  Standard_EXPORT bool HasTown() const;

  Standard_EXPORT void SetRegion(const occ::handle<TCollection_HAsciiString>& aRegion);

  Standard_EXPORT void UnSetRegion();

  Standard_EXPORT occ::handle<TCollection_HAsciiString> Region() const;

  Standard_EXPORT bool HasRegion() const;

  Standard_EXPORT void SetPostalCode(const occ::handle<TCollection_HAsciiString>& aPostalCode);

  Standard_EXPORT void UnSetPostalCode();

  Standard_EXPORT occ::handle<TCollection_HAsciiString> PostalCode() const;

  Standard_EXPORT bool HasPostalCode() const;

  Standard_EXPORT void SetCountry(const occ::handle<TCollection_HAsciiString>& aCountry);

  Standard_EXPORT void UnSetCountry();

  Standard_EXPORT occ::handle<TCollection_HAsciiString> Country() const;

  Standard_EXPORT bool HasCountry() const;

  Standard_EXPORT void SetFacsimileNumber(const occ::handle<TCollection_HAsciiString>& aFacsimileNumber);

  Standard_EXPORT void UnSetFacsimileNumber();

  Standard_EXPORT occ::handle<TCollection_HAsciiString> FacsimileNumber() const;

  Standard_EXPORT bool HasFacsimileNumber() const;

  Standard_EXPORT void SetTelephoneNumber(const occ::handle<TCollection_HAsciiString>& aTelephoneNumber);

  Standard_EXPORT void UnSetTelephoneNumber();

  Standard_EXPORT occ::handle<TCollection_HAsciiString> TelephoneNumber() const;

  Standard_EXPORT bool HasTelephoneNumber() const;

  Standard_EXPORT void SetElectronicMailAddress(
    const occ::handle<TCollection_HAsciiString>& aElectronicMailAddress);

  Standard_EXPORT void UnSetElectronicMailAddress();

  Standard_EXPORT occ::handle<TCollection_HAsciiString> ElectronicMailAddress() const;

  Standard_EXPORT bool HasElectronicMailAddress() const;

  Standard_EXPORT void SetTelexNumber(const occ::handle<TCollection_HAsciiString>& aTelexNumber);

  Standard_EXPORT void UnSetTelexNumber();

  Standard_EXPORT occ::handle<TCollection_HAsciiString> TelexNumber() const;

  Standard_EXPORT bool HasTelexNumber() const;

  DEFINE_STANDARD_RTTIEXT(StepBasic_Address, Standard_Transient)

private:
  occ::handle<TCollection_HAsciiString> internalLocation;
  occ::handle<TCollection_HAsciiString> streetNumber;
  occ::handle<TCollection_HAsciiString> street;
  occ::handle<TCollection_HAsciiString> postalBox;
  occ::handle<TCollection_HAsciiString> town;
  occ::handle<TCollection_HAsciiString> region;
  occ::handle<TCollection_HAsciiString> postalCode;
  occ::handle<TCollection_HAsciiString> country;
  occ::handle<TCollection_HAsciiString> facsimileNumber;
  occ::handle<TCollection_HAsciiString> telephoneNumber;
  occ::handle<TCollection_HAsciiString> electronicMailAddress;
  occ::handle<TCollection_HAsciiString> telexNumber;
  bool                 hasInternalLocation;
  bool                 hasStreetNumber;
  bool                 hasStreet;
  bool                 hasPostalBox;
  bool                 hasTown;
  bool                 hasRegion;
  bool                 hasPostalCode;
  bool                 hasCountry;
  bool                 hasFacsimileNumber;
  bool                 hasTelephoneNumber;
  bool                 hasElectronicMailAddress;
  bool                 hasTelexNumber;
};

#endif // _StepBasic_Address_HeaderFile
