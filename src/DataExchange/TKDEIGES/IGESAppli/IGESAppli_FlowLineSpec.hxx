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

#ifndef _IGESAppli_FlowLineSpec_HeaderFile
#define _IGESAppli_FlowLineSpec_HeaderFile

#include <Standard.hxx>

#include <TCollection_HAsciiString.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;

//! defines FlowLineSpec, Type <406> Form <14>
//! in package IGESAppli
//! Attaches one or more text strings to entities being
//! used to represent a flow line
class IGESAppli_FlowLineSpec : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESAppli_FlowLineSpec();

  //! This method is used to set the fields of the class
  //! FlowLineSpec
  //! - allProperties : primary flow line specification and modifiers
  Standard_EXPORT void Init(const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& allProperties);

  //! returns the number of property values
  Standard_EXPORT int NbPropertyValues() const;

  //! returns primary flow line specification name
  Standard_EXPORT occ::handle<TCollection_HAsciiString> FlowLineName() const;

  //! returns specified modifier element
  //! raises exception if Index <= 1 or Index > NbPropertyValues
  Standard_EXPORT occ::handle<TCollection_HAsciiString> Modifier(const int Index) const;

  DEFINE_STANDARD_RTTIEXT(IGESAppli_FlowLineSpec, IGESData_IGESEntity)

private:
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> theNameAndModifiers;
};

#endif // _IGESAppli_FlowLineSpec_HeaderFile
