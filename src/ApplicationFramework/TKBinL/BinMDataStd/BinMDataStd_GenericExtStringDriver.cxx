// Copyright (c) 2020 OPEN CASCADE SAS
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

#include <BinMDataStd_GenericExtStringDriver.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_Attribute.hxx>
#include <BinMDataStd.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDataStd_GenericExtStringDriver, BinMDF_ADriver)

//=================================================================================================

BinMDataStd_GenericExtStringDriver::BinMDataStd_GenericExtStringDriver(
  const occ::handle<Message_Messenger>& theMessageDriver)
    : BinMDF_ADriver(theMessageDriver, STANDARD_TYPE(TDataStd_GenericExtString)->Name())
{
}

//=================================================================================================

occ::handle<TDF_Attribute> BinMDataStd_GenericExtStringDriver::NewEmpty() const
{
  return new TDataStd_Name;
}

//=================================================================================================

const occ::handle<Standard_Type>& BinMDataStd_GenericExtStringDriver::SourceType() const
{
  return Standard_Type::Instance<TDataStd_GenericExtString>();
}

//=======================================================================
// function : Paste
// purpose  : persistent -> transient (retrieve)
//=======================================================================

bool BinMDataStd_GenericExtStringDriver::Paste(const BinObjMgt_Persistent&       Source,
                                               const occ::handle<TDF_Attribute>& Target,
                                               BinObjMgt_RRelocationTable&       RelocTable) const
{
  occ::handle<TDataStd_GenericExtString> aStrAttr =
    occ::down_cast<TDataStd_GenericExtString>(Target);
  TCollection_ExtendedString aStr;
  bool                       ok = Source >> aStr;
  if (ok)
    aStrAttr->Set(aStr);
  if (RelocTable.GetHeaderData()->StorageVersion().IntegerValue()
      >= TDocStd_FormatVersion_VERSION_9)
  { // process user defined guid
    const int&    aPos = Source.Position();
    Standard_GUID aGuid;
    ok = Source >> aGuid;
    if (!ok)
    {
      Source.SetPosition(aPos);
      ok = true;
    }
    else
    {
      aStrAttr->SetID(aGuid);
    }
  }
  return ok;
}

//=======================================================================
// function : Paste
// purpose  : transient -> persistent (store)
//=======================================================================

void BinMDataStd_GenericExtStringDriver::Paste(
  const occ::handle<TDF_Attribute>& Source,
  BinObjMgt_Persistent&             Target,
  NCollection_IndexedMap<occ::handle<Standard_Transient>>& /*RelocTable*/) const
{
  occ::handle<TDataStd_GenericExtString> aStrAttr =
    occ::down_cast<TDataStd_GenericExtString>(Source);
  Target << aStrAttr->Get();
  // process user defined guid
  Target << aStrAttr->ID();
}
