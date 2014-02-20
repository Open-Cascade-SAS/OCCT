// Created on: 2009-01-30
// Created by: Andrey BETENEV (abv)
// Copyright (c) 2009-2014 OPEN CASCADE SAS
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

#include <NCollection_Handle.hxx>

// NOTE: OCCT type information functions are defined explicitly
// instead of using macros from Standard_DefineHandle.hxx,
// since class NCollection_Handle is template and this is not supported 

static Handle(Standard_Type) aTypeNCollection_Handle = 
       STANDARD_TYPE(NCollection_Handle);

const Handle(Standard_Type)& STANDARD_TYPE(NCollection_Handle)
{
  static Handle(Standard_Transient) _Ancestors[] = 
    { STANDARD_TYPE(Standard_Transient), NULL, };
  static Handle(Standard_Type) _aType = 
    new Standard_Type("NCollection_Handle", 
                      sizeof(NCollection_Handle<Standard_Transient>),
                      1, (Standard_Address)_Ancestors, (Standard_Address)NULL);
  return _aType;
}
