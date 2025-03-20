// Created on: 2007-05-29
// Created by: Vlad Romashko
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#include <BinMDataStd.hxx>
#include <BinMDataStd_ByteArrayDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_ByteArray.hxx>
#include <TDF_Attribute.hxx>
#include <TDocStd_FormatVersion.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDataStd_ByteArrayDriver, BinMDF_ADriver)

//=================================================================================================

BinMDataStd_ByteArrayDriver::BinMDataStd_ByteArrayDriver(
  const Handle(Message_Messenger)& theMsgDriver)
    : BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(TDataStd_ByteArray)->Name())
{
}

//=================================================================================================

Handle(TDF_Attribute) BinMDataStd_ByteArrayDriver::NewEmpty() const
{
  return new TDataStd_ByteArray();
}

//=======================================================================
// function : Paste
// purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean BinMDataStd_ByteArrayDriver::Paste(const BinObjMgt_Persistent&  theSource,
                                                    const Handle(TDF_Attribute)& theTarget,
                                                    BinObjMgt_RRelocationTable& theRelocTable) const
{
  Standard_Integer aFirstInd, aLastInd;
  if (!(theSource >> aFirstInd >> aLastInd))
    return Standard_False;
  if (aLastInd < aFirstInd)
    return Standard_False;

  TColStd_Array1OfByte aTargetArray(aFirstInd, aLastInd);
  theSource.GetByteArray(&aTargetArray(aFirstInd), aTargetArray.Length());

  const Handle(TDataStd_ByteArray) anAtt = Handle(TDataStd_ByteArray)::DownCast(theTarget);
  Handle(TColStd_HArray1OfByte)    bytes = new TColStd_HArray1OfByte(aFirstInd, aLastInd);
  for (Standard_Integer i = aFirstInd; i <= aLastInd; i++)
  {
    bytes->SetValue(i, aTargetArray.Value(i));
  }
  anAtt->ChangeArray(bytes);

  Standard_Boolean aDelta(Standard_False);
  if (theRelocTable.GetHeaderData()->StorageVersion().IntegerValue()
      >= TDocStd_FormatVersion_VERSION_3)
  {
    Standard_Byte aDeltaValue;
    if (!(theSource >> aDeltaValue))
      return Standard_False;
    else
      aDelta = (aDeltaValue != 0);
  }
  anAtt->SetDelta(aDelta);

  BinMDataStd::SetAttributeID(theSource,
                              anAtt,
                              theRelocTable.GetHeaderData()->StorageVersion().IntegerValue());
  return Standard_True;
}

//=======================================================================
// function : Paste
// purpose  : transient -> persistent (store)
//=======================================================================
void BinMDataStd_ByteArrayDriver::Paste(const Handle(TDF_Attribute)& theSource,
                                        BinObjMgt_Persistent&        theTarget,
                                        BinObjMgt_SRelocationTable&) const
{
  Handle(TDataStd_ByteArray) anAtt     = Handle(TDataStd_ByteArray)::DownCast(theSource);
  const Standard_Integer     aFirstInd = anAtt->Lower();
  const Standard_Integer     aLastInd  = anAtt->Upper();
  if (aLastInd < aFirstInd)
    return;
  theTarget << aFirstInd << aLastInd;

  const Handle(TColStd_HArray1OfByte)& bytes = anAtt->InternalArray();
  Standard_Integer                     lower = bytes->Lower(), i = lower, upper = bytes->Upper();
  TColStd_Array1OfByte                 aSourceArray(lower, upper);
  for (; i <= upper; i++)
  {
    aSourceArray.SetValue(i, bytes->Value(i));
  }
  Standard_Byte* aPtr = (Standard_Byte*)&aSourceArray(lower);
  theTarget.PutByteArray(aPtr, bytes->Length());
  theTarget << (Standard_Byte)(anAtt->GetDelta() ? 1 : 0);

  // process user defined guid
  if (anAtt->ID() != TDataStd_ByteArray::GetID())
    theTarget << anAtt->ID();
}
