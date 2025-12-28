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
#include "XmlTObjDrivers_ObjectDriver.hxx"
#include <TObj_TObject.hxx>
#include <TObj_Persistence.hxx>
#include <XmlObjMgt_Persistent.hxx>
#include <XmlObjMgt_RRelocationTable.hxx>
#include <XmlObjMgt.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlTObjDrivers_ObjectDriver, XmlMDF_ADriver)

//=================================================================================================

XmlTObjDrivers_ObjectDriver::XmlTObjDrivers_ObjectDriver(
  const occ::handle<Message_Messenger>& theMessageDriver)
    : XmlMDF_ADriver(theMessageDriver, nullptr)
{
}

//=======================================================================
// function : NewEmpty
// purpose  : Creates a new attribute
//=======================================================================

occ::handle<TDF_Attribute> XmlTObjDrivers_ObjectDriver::NewEmpty() const
{
  return new TObj_TObject;
}

//=======================================================================
// function : Paste
// purpose  : Translate the contents of <aSource> and put it
//           into <aTarget>, using the relocation table
//           <aRelocTable> to keep the sharings.
//=======================================================================

bool XmlTObjDrivers_ObjectDriver::Paste(const XmlObjMgt_Persistent&       Source,
                                        const occ::handle<TDF_Attribute>& Target,
                                        XmlObjMgt_RRelocationTable& /*RelocTable*/) const
{
  TCollection_ExtendedString aString;
  if (XmlObjMgt::GetExtendedString(Source, aString))
  {
    TCollection_AsciiString  anAscii(aString);
    occ::handle<TObj_Object> anObject =
      TObj_Persistence::CreateNewObject(anAscii.ToCString(), Target->Label());
    occ::down_cast<TObj_TObject>(Target)->Set(anObject);
    return true;
  }
  myMessageDriver->Send("error retrieving ExtendedString for type TObj_TModel", Message_Fail);
  return false;
}

//=======================================================================
// function : Paste
// purpose  : Translate the contents of <aSource> and put it
//           into <aTarget>, using the relocation table
//           <aRelocTable> to keep the sharings.
//           anObject is stored as a Name of class derived from TObj_Object
//=======================================================================

void XmlTObjDrivers_ObjectDriver::Paste(const occ::handle<TDF_Attribute>& Source,
                                        XmlObjMgt_Persistent&             Target,
                                        XmlObjMgt_SRelocationTable& /*RelocTable*/) const
{
  occ::handle<TObj_TObject> aTObj     = occ::down_cast<TObj_TObject>(Source);
  occ::handle<TObj_Object>  anIObject = aTObj->Get();

  XmlObjMgt::SetExtendedString(Target, anIObject->DynamicType()->Name());
}
