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


class StepBasic_Address;
DEFINE_STANDARD_HANDLE(StepBasic_Address, Standard_Transient)


class StepBasic_Address : public Standard_Transient
{

public:

  
  //! Returns a Address
  Standard_EXPORT StepBasic_Address();
  
  Standard_EXPORT void Init (const Standard_Boolean hasAinternalLocation, const Handle(TCollection_HAsciiString)& aInternalLocation, const Standard_Boolean hasAstreetNumber, const Handle(TCollection_HAsciiString)& aStreetNumber, const Standard_Boolean hasAstreet, const Handle(TCollection_HAsciiString)& aStreet, const Standard_Boolean hasApostalBox, const Handle(TCollection_HAsciiString)& aPostalBox, const Standard_Boolean hasAtown, const Handle(TCollection_HAsciiString)& aTown, const Standard_Boolean hasAregion, const Handle(TCollection_HAsciiString)& aRegion, const Standard_Boolean hasApostalCode, const Handle(TCollection_HAsciiString)& aPostalCode, const Standard_Boolean hasAcountry, const Handle(TCollection_HAsciiString)& aCountry, const Standard_Boolean hasAfacsimileNumber, const Handle(TCollection_HAsciiString)& aFacsimileNumber, const Standard_Boolean hasAtelephoneNumber, const Handle(TCollection_HAsciiString)& aTelephoneNumber, const Standard_Boolean hasAelectronicMailAddress, const Handle(TCollection_HAsciiString)& aElectronicMailAddress, const Standard_Boolean hasAtelexNumber, const Handle(TCollection_HAsciiString)& aTelexNumber);
  
  Standard_EXPORT void SetInternalLocation (const Handle(TCollection_HAsciiString)& aInternalLocation);
  
  Standard_EXPORT void UnSetInternalLocation();
  
  Standard_EXPORT Handle(TCollection_HAsciiString) InternalLocation() const;
  
  Standard_EXPORT Standard_Boolean HasInternalLocation() const;
  
  Standard_EXPORT void SetStreetNumber (const Handle(TCollection_HAsciiString)& aStreetNumber);
  
  Standard_EXPORT void UnSetStreetNumber();
  
  Standard_EXPORT Handle(TCollection_HAsciiString) StreetNumber() const;
  
  Standard_EXPORT Standard_Boolean HasStreetNumber() const;
  
  Standard_EXPORT void SetStreet (const Handle(TCollection_HAsciiString)& aStreet);
  
  Standard_EXPORT void UnSetStreet();
  
  Standard_EXPORT Handle(TCollection_HAsciiString) Street() const;
  
  Standard_EXPORT Standard_Boolean HasStreet() const;
  
  Standard_EXPORT void SetPostalBox (const Handle(TCollection_HAsciiString)& aPostalBox);
  
  Standard_EXPORT void UnSetPostalBox();
  
  Standard_EXPORT Handle(TCollection_HAsciiString) PostalBox() const;
  
  Standard_EXPORT Standard_Boolean HasPostalBox() const;
  
  Standard_EXPORT void SetTown (const Handle(TCollection_HAsciiString)& aTown);
  
  Standard_EXPORT void UnSetTown();
  
  Standard_EXPORT Handle(TCollection_HAsciiString) Town() const;
  
  Standard_EXPORT Standard_Boolean HasTown() const;
  
  Standard_EXPORT void SetRegion (const Handle(TCollection_HAsciiString)& aRegion);
  
  Standard_EXPORT void UnSetRegion();
  
  Standard_EXPORT Handle(TCollection_HAsciiString) Region() const;
  
  Standard_EXPORT Standard_Boolean HasRegion() const;
  
  Standard_EXPORT void SetPostalCode (const Handle(TCollection_HAsciiString)& aPostalCode);
  
  Standard_EXPORT void UnSetPostalCode();
  
  Standard_EXPORT Handle(TCollection_HAsciiString) PostalCode() const;
  
  Standard_EXPORT Standard_Boolean HasPostalCode() const;
  
  Standard_EXPORT void SetCountry (const Handle(TCollection_HAsciiString)& aCountry);
  
  Standard_EXPORT void UnSetCountry();
  
  Standard_EXPORT Handle(TCollection_HAsciiString) Country() const;
  
  Standard_EXPORT Standard_Boolean HasCountry() const;
  
  Standard_EXPORT void SetFacsimileNumber (const Handle(TCollection_HAsciiString)& aFacsimileNumber);
  
  Standard_EXPORT void UnSetFacsimileNumber();
  
  Standard_EXPORT Handle(TCollection_HAsciiString) FacsimileNumber() const;
  
  Standard_EXPORT Standard_Boolean HasFacsimileNumber() const;
  
  Standard_EXPORT void SetTelephoneNumber (const Handle(TCollection_HAsciiString)& aTelephoneNumber);
  
  Standard_EXPORT void UnSetTelephoneNumber();
  
  Standard_EXPORT Handle(TCollection_HAsciiString) TelephoneNumber() const;
  
  Standard_EXPORT Standard_Boolean HasTelephoneNumber() const;
  
  Standard_EXPORT void SetElectronicMailAddress (const Handle(TCollection_HAsciiString)& aElectronicMailAddress);
  
  Standard_EXPORT void UnSetElectronicMailAddress();
  
  Standard_EXPORT Handle(TCollection_HAsciiString) ElectronicMailAddress() const;
  
  Standard_EXPORT Standard_Boolean HasElectronicMailAddress() const;
  
  Standard_EXPORT void SetTelexNumber (const Handle(TCollection_HAsciiString)& aTelexNumber);
  
  Standard_EXPORT void UnSetTelexNumber();
  
  Standard_EXPORT Handle(TCollection_HAsciiString) TelexNumber() const;
  
  Standard_EXPORT Standard_Boolean HasTelexNumber() const;




  DEFINE_STANDARD_RTTIEXT(StepBasic_Address,Standard_Transient)

protected:




private:


  Handle(TCollection_HAsciiString) internalLocation;
  Handle(TCollection_HAsciiString) streetNumber;
  Handle(TCollection_HAsciiString) street;
  Handle(TCollection_HAsciiString) postalBox;
  Handle(TCollection_HAsciiString) town;
  Handle(TCollection_HAsciiString) region;
  Handle(TCollection_HAsciiString) postalCode;
  Handle(TCollection_HAsciiString) country;
  Handle(TCollection_HAsciiString) facsimileNumber;
  Handle(TCollection_HAsciiString) telephoneNumber;
  Handle(TCollection_HAsciiString) electronicMailAddress;
  Handle(TCollection_HAsciiString) telexNumber;
  Standard_Boolean hasInternalLocation;
  Standard_Boolean hasStreetNumber;
  Standard_Boolean hasStreet;
  Standard_Boolean hasPostalBox;
  Standard_Boolean hasTown;
  Standard_Boolean hasRegion;
  Standard_Boolean hasPostalCode;
  Standard_Boolean hasCountry;
  Standard_Boolean hasFacsimileNumber;
  Standard_Boolean hasTelephoneNumber;
  Standard_Boolean hasElectronicMailAddress;
  Standard_Boolean hasTelexNumber;


};







#endif // _StepBasic_Address_HeaderFile
