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

#ifndef _StepBasic_OrganizationalAddress_HeaderFile
#define _StepBasic_OrganizationalAddress_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepBasic_HArray1OfOrganization.hxx>
#include <StepBasic_Address.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class StepBasic_Organization;

class StepBasic_OrganizationalAddress;
DEFINE_STANDARD_HANDLE(StepBasic_OrganizationalAddress, StepBasic_Address)

class StepBasic_OrganizationalAddress : public StepBasic_Address
{

public:
  //! Returns a OrganizationalAddress
  Standard_EXPORT StepBasic_OrganizationalAddress();

  Standard_EXPORT void Init(const Standard_Boolean                  hasAinternalLocation,
                            const Handle(TCollection_HAsciiString)& aInternalLocation,
                            const Standard_Boolean                  hasAstreetNumber,
                            const Handle(TCollection_HAsciiString)& aStreetNumber,
                            const Standard_Boolean                  hasAstreet,
                            const Handle(TCollection_HAsciiString)& aStreet,
                            const Standard_Boolean                  hasApostalBox,
                            const Handle(TCollection_HAsciiString)& aPostalBox,
                            const Standard_Boolean                  hasAtown,
                            const Handle(TCollection_HAsciiString)& aTown,
                            const Standard_Boolean                  hasAregion,
                            const Handle(TCollection_HAsciiString)& aRegion,
                            const Standard_Boolean                  hasApostalCode,
                            const Handle(TCollection_HAsciiString)& aPostalCode,
                            const Standard_Boolean                  hasAcountry,
                            const Handle(TCollection_HAsciiString)& aCountry,
                            const Standard_Boolean                  hasAfacsimileNumber,
                            const Handle(TCollection_HAsciiString)& aFacsimileNumber,
                            const Standard_Boolean                  hasAtelephoneNumber,
                            const Handle(TCollection_HAsciiString)& aTelephoneNumber,
                            const Standard_Boolean                  hasAelectronicMailAddress,
                            const Handle(TCollection_HAsciiString)& aElectronicMailAddress,
                            const Standard_Boolean                  hasAtelexNumber,
                            const Handle(TCollection_HAsciiString)& aTelexNumber,
                            const Handle(StepBasic_HArray1OfOrganization)& aOrganizations,
                            const Handle(TCollection_HAsciiString)&        aDescription);

  Standard_EXPORT void SetOrganizations(
    const Handle(StepBasic_HArray1OfOrganization)& aOrganizations);

  Standard_EXPORT Handle(StepBasic_HArray1OfOrganization) Organizations() const;

  Standard_EXPORT Handle(StepBasic_Organization) OrganizationsValue(
    const Standard_Integer num) const;

  Standard_EXPORT Standard_Integer NbOrganizations() const;

  Standard_EXPORT void SetDescription(const Handle(TCollection_HAsciiString)& aDescription);

  Standard_EXPORT Handle(TCollection_HAsciiString) Description() const;

  DEFINE_STANDARD_RTTIEXT(StepBasic_OrganizationalAddress, StepBasic_Address)

protected:
private:
  Handle(StepBasic_HArray1OfOrganization) organizations;
  Handle(TCollection_HAsciiString)        description;
};

#endif // _StepBasic_OrganizationalAddress_HeaderFile
