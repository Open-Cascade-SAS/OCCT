// Created on: 1993-01-11
// Created by: CKY / Contract Toubro-Larsen ( Anand NATRAJAN )
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _IGESAppli_PartNumber_HeaderFile
#define _IGESAppli_PartNumber_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <IGESData_IGESEntity.hxx>
class TCollection_HAsciiString;


class IGESAppli_PartNumber;
DEFINE_STANDARD_HANDLE(IGESAppli_PartNumber, IGESData_IGESEntity)

//! defines PartNumber, Type <406> Form <9>
//! in package IGESAppli
//! Attaches a set of text strings that define the common
//! part numbers to an entity being used to represent a
//! physical component
class IGESAppli_PartNumber : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESAppli_PartNumber();
  
  //! This method is used to set the fields of the class
  //! PartNumber
  //! - nbPropVal : number of property values, always = 4
  //! - aGenName  : Generic part number or name
  //! - aMilName  : Military Standard (MIL-STD) part number
  //! - aVendName : Vendor part number or name
  //! - anIntName : Internal part number
  Standard_EXPORT void Init (const Standard_Integer nbPropVal, const Handle(TCollection_HAsciiString)& aGenName, const Handle(TCollection_HAsciiString)& aMilName, const Handle(TCollection_HAsciiString)& aVendName, const Handle(TCollection_HAsciiString)& anIntName);
  
  //! returns number of property values, always = 4
  Standard_EXPORT Standard_Integer NbPropertyValues() const;
  
  //! returns Generic part number or name
  Standard_EXPORT Handle(TCollection_HAsciiString) GenericNumber() const;
  
  //! returns Military Standard (MIL-STD) part number
  Standard_EXPORT Handle(TCollection_HAsciiString) MilitaryNumber() const;
  
  //! returns Vendor part number or name
  Standard_EXPORT Handle(TCollection_HAsciiString) VendorNumber() const;
  
  //! returns Internal part number
  Standard_EXPORT Handle(TCollection_HAsciiString) InternalNumber() const;




  DEFINE_STANDARD_RTTIEXT(IGESAppli_PartNumber,IGESData_IGESEntity)

protected:




private:


  Standard_Integer theNbPropertyValues;
  Handle(TCollection_HAsciiString) theGenericNumber;
  Handle(TCollection_HAsciiString) theMilitaryNumber;
  Handle(TCollection_HAsciiString) theVendorNumber;
  Handle(TCollection_HAsciiString) theInternalNumber;


};







#endif // _IGESAppli_PartNumber_HeaderFile
