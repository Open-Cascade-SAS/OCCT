// File:        NCollection_Handle.cxx
// Created:     Fri Jan 30 15:15:17 2009
// Author:      Andrey BETENEV (abv)
// Copyright:   Open CASCADE 2009

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
