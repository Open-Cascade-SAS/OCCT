// Created on: 2004-11-24
// Created by: Edward AGAPOV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

// The original implementation Copyright: (C) RINA S.p.A

#include <Message_Messenger.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <TDF_Tool.hxx>

#include <BinTObjDrivers_ObjectDriver.hxx>
#include <TObj_Assistant.hxx>
#include <TObj_TObject.hxx>
#include <TObj_Persistence.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinTObjDrivers_ObjectDriver, BinMDF_ADriver)

//=================================================================================================

BinTObjDrivers_ObjectDriver::BinTObjDrivers_ObjectDriver(
  const occ::handle<Message_Messenger>& theMessageDriver)
    : BinMDF_ADriver(theMessageDriver, nullptr)
{
}

//=======================================================================
// function : NewEmpty
// purpose  : Creates a new attribute
//=======================================================================

occ::handle<TDF_Attribute> BinTObjDrivers_ObjectDriver::NewEmpty() const
{
  return new TObj_TObject;
}

//=======================================================================
// function : Paste
// purpose  : Translate the contents of <theSource> and put it
//           into <theTarget>.
//=======================================================================

bool BinTObjDrivers_ObjectDriver::Paste(const BinObjMgt_Persistent&       theSource,
                                        const occ::handle<TDF_Attribute>& theTarget,
                                        BinObjMgt_RRelocationTable&) const
{
  int aSavedPos = theSource.Position();

  // first try to get the type as an integer ID
  int anID;
  if (!(theSource >> anID))
    return false;
  occ::handle<TObj_Object> anObject;
  if ((unsigned)anID > 0xffff)
  {
    // if we are here it means that the type was stored as an ascii string,
    // so rewind theSource and reget
    theSource.SetPosition(aSavedPos);
    TCollection_AsciiString aName;
    if (!(theSource >> aName))
      return false;
    anObject = TObj_Persistence::CreateNewObject(aName.ToCString(), theTarget->Label());
    if (anObject.IsNull())
    {
      TCollection_AsciiString anEntry;
      TDF_Tool::Entry(theTarget->Label(), anEntry);
      myMessageDriver->Send(
        TCollection_ExtendedString("TObj_TObject retrieval: wrong object type name ") + aName
          + ", entry " + anEntry,
        Message_Fail);
      TObj_Assistant::BindType(nullptr);
      return false;
    }
    // register the type
    TObj_Assistant::BindType(anObject->DynamicType());
  }
  else
  {
    // use anID to get the type from earlier registered ones
    occ::handle<Standard_Type> aType = TObj_Assistant::FindType(anID);
    if (!aType.IsNull())
      anObject = TObj_Persistence::CreateNewObject(aType->Name(), theTarget->Label());
    else
    {
      return false;
    }
  }
  occ::down_cast<TObj_TObject>(theTarget)->Set(anObject);
  return true;
}

//=======================================================================
// function : Paste
// purpose  : Translate the contents of <theSource> and put it
//           into <theTarget>.
//           anObject is stored as a Name of class derived from TObj_Object
//=======================================================================

void BinTObjDrivers_ObjectDriver::Paste(
  const occ::handle<TDF_Attribute>& theSource,
  BinObjMgt_Persistent&             theTarget,
  NCollection_IndexedMap<occ::handle<Standard_Transient>>&) const
{
  occ::handle<TObj_TObject> aTObj     = occ::down_cast<TObj_TObject>(theSource);
  occ::handle<TObj_Object>  anIObject = aTObj->Get();
  if (anIObject.IsNull())
    return;

  occ::handle<Standard_Type> aType = anIObject->DynamicType();

  int anID = TObj_Assistant::FindTypeIndex(anIObject->DynamicType());

  if (anID == 0)
  {
    // we first meet this type;
    // register a type and store a type name as a string
    TObj_Assistant::BindType(aType);
    TCollection_AsciiString aName = aType->Name();
    theTarget << aName;
  }
  else
  {
    // store the integer type ID
    theTarget << anID;
  }
}
