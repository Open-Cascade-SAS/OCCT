// Created on: 2009-01-30
// Created by: Andrey BETENEV (abv)
// Copyright (c) 2009-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
