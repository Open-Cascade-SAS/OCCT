// Created:	Tue Nov  2 16:40:51 1999
// Author:	Andrey BETENEV
// Copyright (c) 1999-2020 OPEN CASCADE SAS
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

#include <Express_Field.hxx>

#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Express_Field, Standard_Transient)

//=======================================================================
// function : Express_Field
// purpose  :
//=======================================================================

Express_Field::Express_Field (const Standard_CString theName,
                              const Handle(Express_Type)& theType,
                              const Standard_Boolean theOpt)
{
  myName = new TCollection_HAsciiString (theName);
  myType = theType;
  myOpt = theOpt;
}

//=======================================================================
// function : Express_Field
// purpose  :
//=======================================================================

Express_Field::Express_Field (const Handle(TCollection_HAsciiString)& theName,
                              const Handle(Express_Type)& theType,
                              const Standard_Boolean theOpt)
{
  myName = theName;
  myType = theType;
  myOpt = theOpt;
}

//=======================================================================
// function : Name
// purpose  :
//=======================================================================

const TCollection_AsciiString& Express_Field::Name() const
{
  return myName->String();
}

//=======================================================================
// function : HName
// purpose  :
//=======================================================================

Handle(TCollection_HAsciiString) Express_Field::HName() const
{
  return myName;
}

//=======================================================================
// function : Type
// purpose  :
//=======================================================================

const Handle(Express_Type)& Express_Field::Type() const
{
  return myType;
}

//=======================================================================
// function : IsOptional
// purpose  :
//=======================================================================

Standard_Boolean Express_Field::IsOptional() const
{
  return myOpt;
}
