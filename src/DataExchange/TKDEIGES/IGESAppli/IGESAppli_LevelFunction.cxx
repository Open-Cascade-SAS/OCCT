// Created by: CKY / Contract Toubro-Larsen
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESAppli_LevelFunction.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_LevelFunction, IGESData_IGESEntity)

IGESAppli_LevelFunction::IGESAppli_LevelFunction() = default;

void IGESAppli_LevelFunction::Init(const int                                    nbPropVal,
                                   const int                                    aCode,
                                   const occ::handle<TCollection_HAsciiString>& aFuncDescrip)
{
  theNbPropertyValues = nbPropVal;
  theFuncDescripCode  = aCode;
  theFuncDescrip      = aFuncDescrip;
  InitTypeAndForm(406, 3);
}

int IGESAppli_LevelFunction::NbPropertyValues() const
{
  return theNbPropertyValues;
}

int IGESAppli_LevelFunction::FuncDescriptionCode() const
{
  return theFuncDescripCode;
}

occ::handle<TCollection_HAsciiString> IGESAppli_LevelFunction::FuncDescription() const
{
  return theFuncDescrip;
}
