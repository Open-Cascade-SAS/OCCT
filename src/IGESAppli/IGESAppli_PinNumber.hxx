// Created on: 1993-01-11
// Created by: CKY / Contract Toubro-Larsen ( Arun MENON )
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

#ifndef _IGESAppli_PinNumber_HeaderFile
#define _IGESAppli_PinNumber_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <IGESData_IGESEntity.hxx>
class TCollection_HAsciiString;


class IGESAppli_PinNumber;
DEFINE_STANDARD_HANDLE(IGESAppli_PinNumber, IGESData_IGESEntity)

//! defines PinNumber, Type <406> Form <8>
//! in package IGESAppli
//! Used to attach a text string representing a component
//! pin number to an entity being used to represent an
//! electrical component's pin
class IGESAppli_PinNumber : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESAppli_PinNumber();
  
  //! This method is used to set the fields of the class
  //! PinNumber
  //! - nbPropVal : Number of property values (always = 1)
  //! - aValue    : Pin Number value
  Standard_EXPORT void Init (const Standard_Integer nbPropVal, const Handle(TCollection_HAsciiString)& aValue);
  
  //! returns the number of property values
  //! is always 1
  Standard_EXPORT Standard_Integer NbPropertyValues() const;
  
  //! returns the pin number value
  Standard_EXPORT Handle(TCollection_HAsciiString) PinNumberVal() const;




  DEFINE_STANDARD_RTTIEXT(IGESAppli_PinNumber,IGESData_IGESEntity)

protected:




private:


  Standard_Integer theNbPropertyValues;
  Handle(TCollection_HAsciiString) thePinNumber;


};







#endif // _IGESAppli_PinNumber_HeaderFile
