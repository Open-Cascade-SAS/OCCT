// Created:	Tue Nov  2 15:11:36 1999
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

#include <Express_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Express_Type, Standard_Transient)

//=================================================================================================

Express_Type::Express_Type() {}

//=================================================================================================

Standard_Boolean Express_Type::IsStandard() const
{
  return Standard_False;
}

//=================================================================================================

Standard_Boolean Express_Type::IsSimple() const
{
  return IsStandard();
}

//=================================================================================================

Standard_Boolean Express_Type::IsHandle() const
{
  return !IsSimple();
}

//=================================================================================================

Standard_Boolean Express_Type::Use() const
{
  return Standard_False;
}

//=================================================================================================

void Express_Type::Use2(const TCollection_AsciiString&, const TCollection_AsciiString&) const {}
