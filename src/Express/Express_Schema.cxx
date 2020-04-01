// Created:	Tue Nov  2 12:29:06 1999
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

#include <Express_Schema.hxx>

#include <Express_Alias.hxx>
#include <Express_ComplexType.hxx>
#include <Express_Entity.hxx>
#include <Express_Enum.hxx>
#include <Express_HSequenceOfEntity.hxx>
#include <Express_HSequenceOfField.hxx>
#include <Express_NamedType.hxx>
#include <Express_Select.hxx>
#include <Express_Type.hxx>
#include <Message.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Express_Schema, Standard_Transient)

//=======================================================================
// function : Express_Schema
// purpose  :
//=======================================================================

Express_Schema::Express_Schema (const Standard_CString theName,
                                const Handle(Express_HSequenceOfItem)& theItems)
{
  myName = new TCollection_HAsciiString (theName);
  myItems = theItems;
  Prepare();
}

//=======================================================================
// function : Express_Schema
// purpose  :
//=======================================================================

Express_Schema::Express_Schema (const Handle(TCollection_HAsciiString)& theName,
                                const Handle(Express_HSequenceOfItem)& theItems)
{
  myName = theName;
  myItems = theItems;
  Prepare();
}

//=======================================================================
// function : Name
// purpose  :
//=======================================================================

const Handle(TCollection_HAsciiString)& Express_Schema::Name() const
{
  return myName;
}

//=======================================================================
// function : Items
// purpose  :
//=======================================================================

const Handle(Express_HSequenceOfItem)& Express_Schema::Items() const
{
  return myItems;
}

//=======================================================================
// function : NbItems
// purpose  :
//=======================================================================

Standard_Integer Express_Schema::NbItems() const
{
  return myItems->Length();
}

//=======================================================================
// function : Item
// purpose  :
//=======================================================================

Handle(Express_Item) Express_Schema::Item (const Standard_Integer theNum) const
{
  return myItems->Value (theNum);
}

//=======================================================================
// function : Item
// purpose  :
//=======================================================================

Handle(Express_Item) Express_Schema::Item (const Standard_CString theName,
                                           const Standard_Boolean theSilent) const
{
  if (!myDict.IsBound (theName))
  {
    if (!theSilent)
    {
      Message::SendFail() << "Error: attempt to access unknown item by name " << theName;
    }
    return {};
  }
  return myDict.Find (theName);
}

//=======================================================================
// function : Item
// purpose  :
//=======================================================================

Handle(Express_Item) Express_Schema::Item (const TCollection_AsciiString& theName) const
{
  return Item (theName.ToCString());
}

//=======================================================================
// function : Item
// purpose  :
//=======================================================================

Handle(Express_Item) Express_Schema::Item (const Handle(TCollection_HAsciiString)& theName) const
{
  return Item (theName->ToCString());
}

//=======================================================================
// function : nameToCasCade
// purpose  : auxilary for Prepare()
//           Convert STEP-style name (lowercase, with underscores)
//           to CASCADE-style name (each word starts with uppercase, no intervals)
//=======================================================================
static void nameToCasCade (const Handle(TCollection_HAsciiString)& theName)
{
  if (theName.IsNull())
  {
    return;
  }
  for (Standard_Integer i = 1; i <= theName->Length(); i++)
  {
    if (theName->Value (i) == '_')
    {
      theName->Remove (i);
    }
    else if (i > 1)
    {
      continue;
    }
    theName->SetValue (i, UpperCase (theName->Value (i)));
  }
}

//=======================================================================
// function : nameToCasCade
// purpose  : auxilary for Prepare()
//           Convert names for Type object
//=======================================================================
static void nameToCasCade (const Handle(Express_Type)& theType)
{
  if (theType->IsKind (STANDARD_TYPE(Express_NamedType)))
  {
    const Handle(Express_NamedType) aNamedType = Handle(Express_NamedType)::DownCast (theType);
    nameToCasCade (aNamedType->HName());
  }
  else if (theType->IsKind (STANDARD_TYPE(Express_ComplexType)))
  {
    const Handle(Express_ComplexType) aComplexType = Handle(Express_ComplexType)::DownCast (theType);
    nameToCasCade (aComplexType->Type());
  }
}

//=======================================================================
// function : Prepare
// purpose  : Prepare data: convert names to CasCade, fill dictionary of typenames
//           and set handles to items where they are referenced by names
//=======================================================================

void Express_Schema::Prepare()
{
  myDict.Clear();
  if (myItems.IsNull())
  {
    return;
  }

  Standard_Integer aNbItems = NbItems();

  // convert names and fill dictionary
  for (Standard_Integer aNum = 1; aNum <= aNbItems; aNum++)
  {
    // get item
    const Handle(Express_Item) anItem = Item (aNum);

    // change item name
    nameToCasCade (anItem->HName());

    // change names of referred types and other names
    if (anItem->IsKind (STANDARD_TYPE(Express_Alias)))
    {
      const Handle(Express_Alias) anAlias = Handle(Express_Alias)::DownCast (anItem);
      nameToCasCade (anAlias->Type());
    }
    else if (anItem->IsKind (STANDARD_TYPE(Express_Select)))
    {
      const Handle(Express_Select) aSelect = Handle(Express_Select)::DownCast (anItem);
      for (Standard_Integer i = 1; i <= aSelect->Names()->Length(); i++)
      {
        nameToCasCade (aSelect->Names()->Value (i));
      }
    }
    else if (anItem->IsKind (STANDARD_TYPE(Express_Enum)))
    {
      const Handle(Express_Enum) anEnum = Handle(Express_Enum)::DownCast (anItem);
      for (Standard_Integer i = 1; i <= anEnum->Names()->Length(); i++)
      {
        nameToCasCade (anEnum->Names()->Value (i));
      }
    }
    else if (anItem->IsKind (STANDARD_TYPE(Express_Entity)))
    {
      const Handle(Express_Entity) anEntity = Handle(Express_Entity)::DownCast (anItem);
      for (Standard_Integer i = 1; i <= anEntity->SuperTypes()->Length(); i++)
      {
        nameToCasCade (anEntity->SuperTypes()->Value (i));
      }
      const Handle(Express_HSequenceOfField) aFields = anEntity->Fields();
      for (Standard_Integer i = 1; i <= aFields->Length(); i++)
      {
        nameToCasCade (aFields->Value (i)->HName());
        nameToCasCade (aFields->Value (i)->Type());
      }
    }

    // add to dictionary
    myDict.Bind (anItem->Name(), anItem);
  }

  // set references to items from other items and types
  for (Standard_Integer aNum = 1; aNum <= aNbItems; aNum++)
  {
    const Handle(Express_Item) anItem = Item (aNum);

    if (anItem->IsKind (STANDARD_TYPE(Express_Alias)))
    {
      const Handle(Express_Alias) anAlias = Handle(Express_Alias)::DownCast (anItem);
      PrepareType (anAlias->Type());
      // for aliases, define package to avoid warnings
      anAlias->SetPackageName ("Standard");
      continue;
    }
    else if (anItem->IsKind (STANDARD_TYPE(Express_Select)))
    {
      const Handle(Express_Select) aSelect = Handle(Express_Select)::DownCast (anItem);
      Handle(TColStd_HSequenceOfHAsciiString) aNames = aSelect->Names();
      Handle(Express_HSequenceOfItem) anItems = aSelect->Items();
      for (Standard_Integer i = 1; i <= aNames->Length(); i++)
      {
        Handle(Express_Item) aSubItem = Item (aNames->Value (i));
        // if select refers to another select, expand it
        if (aSubItem->IsKind (STANDARD_TYPE(Express_Select)))
        {
          Message::SendInfo() << "Info: SELECT " << anItem->Name() << " refers to another SELECT " << aSubItem->Name() << "; expanded";
          const Handle(Express_Select) aSubSelect = Handle(Express_Select)::DownCast (aSubItem);
          Standard_Integer j = 1;
          for (; j <= aSubSelect->Names()->Length(); j++)
          {
            aNames->InsertBefore (i + j - 1, aSubSelect->Names()->Value (j));
          }
          aNames->Remove (i + j - 1);
          i--;
          continue;
        }
        anItems->Append (aSubItem);
      }
    }
    else if (anItem->IsKind (STANDARD_TYPE(Express_Entity)))
    {
      const Handle(Express_Entity) anEntity = Handle(Express_Entity)::DownCast (anItem);
      Handle(TColStd_HSequenceOfHAsciiString) aNames = anEntity->SuperTypes();
      Handle(Express_HSequenceOfEntity) anInhItems = anEntity->Inherit();
      for (Standard_Integer i = 1; i <= aNames->Length(); i++)
      {
        Handle(Express_Entity) aSubEntity = Handle(Express_Entity)::DownCast (Item (aNames->Value (i)));
        if (!aSubEntity.IsNull())
        {
          anInhItems->Append (aSubEntity);
        }
        else
        {
          Message::SendFail() << "Error in " << anItem->Name() << ": supertype " << aNames->Value (i)->String() << " is not an ENTITY; ignored";
        }
      }
      const Handle(Express_HSequenceOfField) aFields = anEntity->Fields();
      for (Standard_Integer i = 1; i <= aFields->Length(); i++)
      {
        PrepareType (aFields->Value (i)->Type());
      }
    }
  }

}

//=======================================================================
// function : PrepareType
// purpose  :
//=======================================================================

void Express_Schema::PrepareType (const Handle(Express_Type)& theType) const
{
  if (theType->IsKind (STANDARD_TYPE(Express_NamedType)))
  {
    Handle(Express_NamedType) aNamedType = Handle(Express_NamedType)::DownCast (theType);
    aNamedType->SetItem (Item (aNamedType->Name()));
  }
  else if (theType->IsKind (STANDARD_TYPE(Express_ComplexType)))
  {
    Handle(Express_ComplexType) aComplexType = Handle(Express_ComplexType)::DownCast (theType);
    PrepareType (aComplexType->Type());
  }
}
