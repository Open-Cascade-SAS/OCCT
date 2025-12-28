// Created on: 1995-12-01
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

#ifndef _StepBasic_PersonalAddress_HeaderFile
#define _StepBasic_PersonalAddress_HeaderFile

#include <Standard.hxx>

#include <StepBasic_Person.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepBasic_Address.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class StepBasic_Person;

class StepBasic_PersonalAddress : public StepBasic_Address
{

public:
  //! Returns a PersonalAddress
  Standard_EXPORT StepBasic_PersonalAddress();

  Standard_EXPORT void Init(const bool                   hasAinternalLocation,
                            const occ::handle<TCollection_HAsciiString>&  aInternalLocation,
                            const bool                   hasAstreetNumber,
                            const occ::handle<TCollection_HAsciiString>&  aStreetNumber,
                            const bool                   hasAstreet,
                            const occ::handle<TCollection_HAsciiString>&  aStreet,
                            const bool                   hasApostalBox,
                            const occ::handle<TCollection_HAsciiString>&  aPostalBox,
                            const bool                   hasAtown,
                            const occ::handle<TCollection_HAsciiString>&  aTown,
                            const bool                   hasAregion,
                            const occ::handle<TCollection_HAsciiString>&  aRegion,
                            const bool                   hasApostalCode,
                            const occ::handle<TCollection_HAsciiString>&  aPostalCode,
                            const bool                   hasAcountry,
                            const occ::handle<TCollection_HAsciiString>&  aCountry,
                            const bool                   hasAfacsimileNumber,
                            const occ::handle<TCollection_HAsciiString>&  aFacsimileNumber,
                            const bool                   hasAtelephoneNumber,
                            const occ::handle<TCollection_HAsciiString>&  aTelephoneNumber,
                            const bool                   hasAelectronicMailAddress,
                            const occ::handle<TCollection_HAsciiString>&  aElectronicMailAddress,
                            const bool                   hasAtelexNumber,
                            const occ::handle<TCollection_HAsciiString>&  aTelexNumber,
                            const occ::handle<NCollection_HArray1<occ::handle<StepBasic_Person>>>& aPeople,
                            const occ::handle<TCollection_HAsciiString>&  aDescription);

  Standard_EXPORT void SetPeople(const occ::handle<NCollection_HArray1<occ::handle<StepBasic_Person>>>& aPeople);

  Standard_EXPORT occ::handle<NCollection_HArray1<occ::handle<StepBasic_Person>>> People() const;

  Standard_EXPORT occ::handle<StepBasic_Person> PeopleValue(const int num) const;

  Standard_EXPORT int NbPeople() const;

  Standard_EXPORT void SetDescription(const occ::handle<TCollection_HAsciiString>& aDescription);

  Standard_EXPORT occ::handle<TCollection_HAsciiString> Description() const;

  DEFINE_STANDARD_RTTIEXT(StepBasic_PersonalAddress, StepBasic_Address)

private:
  occ::handle<NCollection_HArray1<occ::handle<StepBasic_Person>>> people;
  occ::handle<TCollection_HAsciiString>  description;
};

#endif // _StepBasic_PersonalAddress_HeaderFile
