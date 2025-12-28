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

#ifndef _IGESAppli_ReferenceDesignator_HeaderFile
#define _IGESAppli_ReferenceDesignator_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <IGESData_IGESEntity.hxx>
class TCollection_HAsciiString;

//! defines ReferenceDesignator, Type <406> Form <7>
//! in package IGESAppli
//! Used to attach a text string containing the value of
//! a component reference designator to an entity being
//! used to represent a component.
class IGESAppli_ReferenceDesignator : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESAppli_ReferenceDesignator();

  //! This method is used to set the fields of the class
  //! ReferenceDesignator
  //! - nbPropVal : Number of property values = 1
  //! - aText     : Reference designator text
  Standard_EXPORT void Init(const int                                    nbPropVal,
                            const occ::handle<TCollection_HAsciiString>& aText);

  //! returns the number of property values
  //! is always 1
  Standard_EXPORT int NbPropertyValues() const;

  //! returns the Reference designator text
  Standard_EXPORT occ::handle<TCollection_HAsciiString> RefDesignatorText() const;

  DEFINE_STANDARD_RTTIEXT(IGESAppli_ReferenceDesignator, IGESData_IGESEntity)

private:
  int                                   theNbPropertyValues;
  occ::handle<TCollection_HAsciiString> theRefDesigText;
};

#endif // _IGESAppli_ReferenceDesignator_HeaderFile
