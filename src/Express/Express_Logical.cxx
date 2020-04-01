// Created:	Tue Nov  2 15:27:26 1999
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

#include <Express_Logical.hxx>

#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Express_Logical, Express_PredefinedType)

//=======================================================================
// function : Express_Logical
// purpose  :
//=======================================================================

Express_Logical::Express_Logical()
{
}

//=======================================================================
// function : CPPName
// purpose  :
//=======================================================================

const TCollection_AsciiString Express_Logical::CPPName() const
{
  return "StepData_Logical";
}

//=======================================================================
// function : IsStandard
// purpose  :
//=======================================================================

Standard_Boolean Express_Logical::IsStandard() const
{
  return Standard_False;
}

//=======================================================================
// function : IsSimple
// purpose  :
//=======================================================================

Standard_Boolean Express_Logical::IsSimple() const
{
  return Standard_True;
}

//=======================================================================
// function : IsHandle
// purpose  :
//=======================================================================

Standard_Boolean Express_Logical::IsHandle() const
{
  return Standard_False;
}

