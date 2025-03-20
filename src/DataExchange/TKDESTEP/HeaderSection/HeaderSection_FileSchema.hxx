// Created on: 1994-06-16
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _HeaderSection_FileSchema_HeaderFile
#define _HeaderSection_FileSchema_HeaderFile

#include <Standard.hxx>

#include <Interface_HArray1OfHAsciiString.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;

class HeaderSection_FileSchema;
DEFINE_STANDARD_HANDLE(HeaderSection_FileSchema, Standard_Transient)

class HeaderSection_FileSchema : public Standard_Transient
{

public:
  //! Returns a FileSchema
  Standard_EXPORT HeaderSection_FileSchema();

  Standard_EXPORT void Init(const Handle(Interface_HArray1OfHAsciiString)& aSchemaIdentifiers);

  Standard_EXPORT void SetSchemaIdentifiers(
    const Handle(Interface_HArray1OfHAsciiString)& aSchemaIdentifiers);

  Standard_EXPORT Handle(Interface_HArray1OfHAsciiString) SchemaIdentifiers() const;

  Standard_EXPORT Handle(TCollection_HAsciiString) SchemaIdentifiersValue(
    const Standard_Integer num) const;

  Standard_EXPORT Standard_Integer NbSchemaIdentifiers() const;

  DEFINE_STANDARD_RTTIEXT(HeaderSection_FileSchema, Standard_Transient)

protected:
private:
  Handle(Interface_HArray1OfHAsciiString) schemaIdentifiers;
};

#endif // _HeaderSection_FileSchema_HeaderFile
