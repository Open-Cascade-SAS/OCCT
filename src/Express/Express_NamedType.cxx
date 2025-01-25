// Created:	Tue Nov  2 15:13:31 1999
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

#include <Express_NamedType.hxx>

#include <Express_Alias.hxx>
#include <Express_Entity.hxx>
#include <Express_Enum.hxx>
#include <Express_Item.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Express_NamedType, Express_Type)

//=================================================================================================

Express_NamedType::Express_NamedType(const Standard_CString theName)
{
  myName = new TCollection_HAsciiString(theName);
}

//=================================================================================================

Express_NamedType::Express_NamedType(const Handle(TCollection_HAsciiString)& theName)
{
  myName = theName;
}

//=================================================================================================

const TCollection_AsciiString& Express_NamedType::Name() const
{
  return myName->String();
}

//=================================================================================================

Handle(TCollection_HAsciiString) Express_NamedType::HName() const
{
  return myName;
}

//=================================================================================================

const Handle(Express_Item)& Express_NamedType::Item() const
{
  return myItem;
}

//=================================================================================================

void Express_NamedType::SetItem(const Handle(Express_Item)& theItem)
{
  myItem = theItem;
}

//=================================================================================================

const TCollection_AsciiString Express_NamedType::CPPName() const
{
  return myItem->CPPName();
}

//=================================================================================================

Standard_Boolean Express_NamedType::IsStandard() const
{
  if (myItem->IsKind(STANDARD_TYPE(Express_Alias)))
  {
    Handle(Express_Alias) anAlias = Handle(Express_Alias)::DownCast(myItem);
    return anAlias->Type()->IsStandard();
  }
  return Standard_False;
}

//=================================================================================================

Standard_Boolean Express_NamedType::IsSimple() const
{
  if (myItem->IsKind(STANDARD_TYPE(Express_Alias)))
  {
    Handle(Express_Alias) anAlias = Handle(Express_Alias)::DownCast(myItem);
    return anAlias->Type()->IsSimple();
  }
  if (myItem->IsKind(STANDARD_TYPE(Express_Enum)))
  {
    return Standard_True;
  }
  return Standard_False; // SELECT & ENTITY
}

//=================================================================================================

Standard_Boolean Express_NamedType::IsHandle() const
{
  if (myItem->IsKind(STANDARD_TYPE(Express_Alias)))
  {
    Handle(Express_Alias) alias = Handle(Express_Alias)::DownCast(myItem);
    return alias->Type()->IsHandle();
  }
  if (myItem->IsKind(STANDARD_TYPE(Express_Entity)))
  {
    return Standard_True;
  }
  return Standard_False; // SELECT & ENUMERATION
}

//=================================================================================================

Standard_Boolean Express_NamedType::Use() const
{
  return myItem->Use();
}

//=================================================================================================

void Express_NamedType::Use2(const TCollection_AsciiString& theRefName,
                             const TCollection_AsciiString& theRefPack) const
{
  myItem->Use2(theRefName, theRefPack);
}
