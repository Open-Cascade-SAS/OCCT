// Created on: 1998-02-04
// Created by: Christian CAILLET
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _Interface_SignType_HeaderFile
#define _Interface_SignType_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <MoniTool_SignText.hxx>
class TCollection_AsciiString;
class Standard_Transient;
class Interface_InterfaceModel;

//! Provides the basic service to get a type name, according
//! to a norm
//! It can be used for other classes (general signatures ...)
class Interface_SignType : public MoniTool_SignText
{

public:
  //! Returns an identification of the Signature (a word), given at
  //! initialization time
  //! Specialised to consider context as an InterfaceModel
  Standard_EXPORT TCollection_AsciiString
    Text(const occ::handle<Standard_Transient>& ent,
         const occ::handle<Standard_Transient>& context) const override;

  //! Returns the Signature for a Transient object. It is specific
  //! of each sub-class of Signature. For a Null Handle, it should
  //! provide ""
  //! It can work with the model which contains the entity
  Standard_EXPORT virtual const char* Value(
    const occ::handle<Standard_Transient>&       ent,
    const occ::handle<Interface_InterfaceModel>& model) const = 0;

  //! From a CDL Type Name, returns the Class part (package dropped)
  //! WARNING : buffered, to be immediately copied or printed
  Standard_EXPORT static const char* ClassName(const char* typnam);

  DEFINE_STANDARD_RTTIEXT(Interface_SignType, MoniTool_SignText)

};

#endif // _Interface_SignType_HeaderFile
