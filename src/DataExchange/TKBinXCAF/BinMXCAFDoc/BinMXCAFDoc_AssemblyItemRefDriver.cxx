// Created on: 2017-02-16
// Created by: Eugeny NIKONOV
// Copyright (c) 2005-2017 OPEN CASCADE SAS
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

#include <BinObjMgt_Persistent.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <BinMXCAFDoc_AssemblyItemRefDriver.hxx>
#include <XCAFDoc_AssemblyItemRef.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMXCAFDoc_AssemblyItemRefDriver, BinMDF_ADriver)

//=================================================================================================

BinMXCAFDoc_AssemblyItemRefDriver::BinMXCAFDoc_AssemblyItemRefDriver(
  const occ::handle<Message_Messenger>& theMsgDriver)
    : BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_AssemblyItemRef)->Name())
{
}

//=================================================================================================

occ::handle<TDF_Attribute> BinMXCAFDoc_AssemblyItemRefDriver::NewEmpty() const
{
  return new XCAFDoc_AssemblyItemRef();
}

//=================================================================================================

bool BinMXCAFDoc_AssemblyItemRefDriver::Paste(
  const BinObjMgt_Persistent&  theSource,
  const occ::handle<TDF_Attribute>& theTarget,
  BinObjMgt_RRelocationTable& /*theRelocTable*/) const
{
  occ::handle<XCAFDoc_AssemblyItemRef> aThis = occ::down_cast<XCAFDoc_AssemblyItemRef>(theTarget);
  if (aThis.IsNull())
    return false;

  TCollection_AsciiString aPathStr;
  if (!(theSource >> aPathStr))
    return false;

  aThis->SetItem(aPathStr);

  int anExtraRef = 0;
  if (!(theSource >> anExtraRef))
    return false;

  if (anExtraRef == 1)
  {
    Standard_GUID aGUID;
    if (!(theSource >> aGUID))
      return false;

    aThis->SetGUID(aGUID);
  }
  else if (anExtraRef == 2)
  {
    int aSubshapeIndex;
    if (!(theSource >> aSubshapeIndex))
      return false;

    aThis->SetSubshapeIndex(aSubshapeIndex);
  }

  return true;
}

//=================================================================================================

void BinMXCAFDoc_AssemblyItemRefDriver::Paste(const occ::handle<TDF_Attribute>& theSource,
                                              BinObjMgt_Persistent&        theTarget,
                                              NCollection_IndexedMap<occ::handle<Standard_Transient>>& /*theRelocTable*/) const
{
  occ::handle<XCAFDoc_AssemblyItemRef> aThis = occ::down_cast<XCAFDoc_AssemblyItemRef>(theSource);
  if (!aThis.IsNull())
  {
    theTarget << aThis->GetItem().ToString();
    if (aThis->IsGUID())
    {
      theTarget << int(1);
      theTarget << aThis->GetGUID();
    }
    else if (aThis->IsSubshapeIndex())
    {
      theTarget << int(2);
      theTarget << aThis->GetSubshapeIndex();
    }
    else
      theTarget << int(0);
  }
}
