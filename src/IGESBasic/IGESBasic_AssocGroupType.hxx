// Created on: 1993-01-09
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

#ifndef _IGESBasic_AssocGroupType_HeaderFile
#define _IGESBasic_AssocGroupType_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <IGESData_IGESEntity.hxx>
class TCollection_HAsciiString;

class IGESBasic_AssocGroupType;
DEFINE_STANDARD_HANDLE(IGESBasic_AssocGroupType, IGESData_IGESEntity)

//! defines AssocGroupType, Type <406> Form <23>
//! in package IGESBasic
//! Used to assign an unambiguous identification to a Group
//! Associativity.
class IGESBasic_AssocGroupType : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESBasic_AssocGroupType();

  //! This method is used to set the fields of the class
  //! AssocGroupType
  //! - nbDataFields : number of parameter data fields = 2
  //! - aType        : type of attached associativity
  //! - aName        : identifier of associativity of type AType
  Standard_EXPORT void Init(const Standard_Integer                  nbDataFields,
                            const Standard_Integer                  aType,
                            const Handle(TCollection_HAsciiString)& aName);

  //! returns the number of parameter data fields, always = 2
  Standard_EXPORT Standard_Integer NbData() const;

  //! returns the type of attached associativity
  Standard_EXPORT Standard_Integer AssocType() const;

  //! returns identifier of instance of specified associativity
  Standard_EXPORT Handle(TCollection_HAsciiString) Name() const;

  DEFINE_STANDARD_RTTIEXT(IGESBasic_AssocGroupType, IGESData_IGESEntity)

protected:
private:
  Standard_Integer                 theNbData;
  Standard_Integer                 theType;
  Handle(TCollection_HAsciiString) theName;
};

#endif // _IGESBasic_AssocGroupType_HeaderFile
