// Created on: 1993-01-09
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

#ifndef _IGESBasic_Name_HeaderFile
#define _IGESBasic_Name_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <IGESData_NameEntity.hxx>
class TCollection_HAsciiString;


class IGESBasic_Name;
DEFINE_STANDARD_HANDLE(IGESBasic_Name, IGESData_NameEntity)

//! defines Name, Type <406> Form <15>
//! in package IGESBasic
//! Used to specify a user defined name
class IGESBasic_Name : public IGESData_NameEntity
{

public:

  
  Standard_EXPORT IGESBasic_Name();
  
  //! This method is used to set the fields of the class Name
  //! - nbPropVal  : Number of property values, always = 1
  //! - aName      : Stores the Name
  Standard_EXPORT void Init (const Standard_Integer nbPropVal, const Handle(TCollection_HAsciiString)& aName);
  
  //! returns the number of property values, which should be 1
  Standard_EXPORT Standard_Integer NbPropertyValues() const;
  
  //! returns the user defined Name
  Standard_EXPORT Handle(TCollection_HAsciiString) Value() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IGESBasic_Name,IGESData_NameEntity)

protected:




private:


  Standard_Integer theNbPropertyValues;
  Handle(TCollection_HAsciiString) theName;


};







#endif // _IGESBasic_Name_HeaderFile
