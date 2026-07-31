// Created on: 1993-01-13
// Created by: CKY / Contract Toubro-Larsen ( Deepak PRABHU )
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

#ifndef _IGESDefs_MacroDef_HeaderFile
#define _IGESDefs_MacroDef_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <TCollection_HAsciiString.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESData_IGESEntity.hxx>
class TCollection_HAsciiString;

//! defines IGES Macro Definition Entity, Type <306> Form <0>
//! in package IGESDefs
//! This Class specifies the action of a specific MACRO.
//! After specification MACRO can be used as necessary
//! by means of MACRO class instance entity.
class IGESDefs_MacroDef : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESDefs_MacroDef();

  //! This method is used to set the fields of the class
  //! MacroDef
  //! - macro          : MACRO
  //! - entityTypeID   : Entity Type ID
  //! - langStatements : Language Statements
  //! - endMacro       : END MACRO
  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&                                   macro,
    const int                                                                      entityTypeID,
    const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& langStatements,
    const occ::handle<TCollection_HAsciiString>&                                   endMacro);

  //! returns the number of language statements
  Standard_EXPORT int NbStatements() const;

  //! returns the MACRO(Literal)
  Standard_EXPORT occ::handle<TCollection_HAsciiString> MACRO() const;

  //! returns the Entity Type ID
  Standard_EXPORT int EntityTypeID() const;

  Standard_EXPORT occ::handle<TCollection_HAsciiString> LanguageStatement(const int StatNum) const;

  //! returns the ENDM(Literal)
  Standard_EXPORT occ::handle<TCollection_HAsciiString> ENDMACRO() const;

  DEFINE_STANDARD_RTTIEXT(IGESDefs_MacroDef, IGESData_IGESEntity)

private:
  occ::handle<TCollection_HAsciiString>                                   theMACRO;
  int                                                                     theEntityTypeID;
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> theLangStatements;
  occ::handle<TCollection_HAsciiString>                                   theENDMACRO;
};

#endif // _IGESDefs_MacroDef_HeaderFile
