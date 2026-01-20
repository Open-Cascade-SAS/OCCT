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

#include <TDataStd_ByteArray.hxx>

#include <Standard_Dump.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_DeltaOnModificationOfByteArray.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_DefaultDeltaOnModification.hxx>
#include <TDF_DeltaOnModification.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_ByteArray, TDF_Attribute)

//=================================================================================================

const Standard_GUID& TDataStd_ByteArray::GetID()
{
  static Standard_GUID TDataStd_ByteArrayID("FD9B918F-2980-4c66-85E0-D71965475290");
  return TDataStd_ByteArrayID;
}

//=================================================================================================

TDataStd_ByteArray::TDataStd_ByteArray()
    : myIsDelta(false),
      myID(GetID())
{
}

//=======================================================================
// function : SetAttr
// purpose  : Implements Set functionality
//=======================================================================
static occ::handle<TDataStd_ByteArray> SetAttr(const TDF_Label&       label,
                                          const int lower,
                                          const int upper,
                                          const bool isDelta,
                                          const Standard_GUID&   theGuid)
{
  occ::handle<TDataStd_ByteArray> A;
  if (!label.FindAttribute(theGuid, A))
  {
    A = new TDataStd_ByteArray;
    A->Init(lower, upper);
    A->SetDelta(isDelta);
    A->SetID(theGuid);
    label.AddAttribute(A);
  }
  else if (lower != A->Lower() || upper != A->Upper())
  {
    A->Init(lower, upper);
  }
  return A;
}

//=================================================================================================

void TDataStd_ByteArray::Init(const int lower, const int upper)
{
  Standard_RangeError_Raise_if(upper < lower, "TDataStd_ByteArray::Init");
  Backup();
  myValue = new NCollection_HArray1<uint8_t>(lower, upper, 0x00);
}

//=================================================================================================

occ::handle<TDataStd_ByteArray> TDataStd_ByteArray::Set(const TDF_Label&       label,
                                                   const int lower,
                                                   const int upper,
                                                   const bool isDelta)
{
  return SetAttr(label, lower, upper, isDelta, GetID());
}

//=======================================================================
// function : Set
// purpose  : Set user defined attribute with specific ID
//=======================================================================
occ::handle<TDataStd_ByteArray> TDataStd_ByteArray::Set(const TDF_Label&       label,
                                                   const Standard_GUID&   theGuid,
                                                   const int lower,
                                                   const int upper,
                                                   const bool isDelta)
{
  return SetAttr(label, lower, upper, isDelta, theGuid);
}

//=================================================================================================

void TDataStd_ByteArray::SetValue(const int index, const uint8_t value)
{
  if (myValue.IsNull())
    return;
  if (value == myValue->Value(index))
    return;
  Backup();

  myValue->SetValue(index, value);
}

//=================================================================================================

uint8_t TDataStd_ByteArray::Value(const int index) const
{
  return myValue->Value(index);
}

//=================================================================================================

int TDataStd_ByteArray::Lower(void) const
{
  if (myValue.IsNull())
    return 0;
  return myValue->Lower();
}

//=================================================================================================

int TDataStd_ByteArray::Upper(void) const
{
  if (myValue.IsNull())
    return -1;
  return myValue->Upper();
}

//=================================================================================================

int TDataStd_ByteArray::Length(void) const
{
  if (myValue.IsNull())
    return 0;
  return myValue->Length();
}

//=======================================================================
// function : ChangeArray
// purpose  : If value of <newArray> differs from <myValue>, Backup
//         : performed and myValue refers to new instance of HArray1OfByte
//         : that holds <newArray>
//=======================================================================
void TDataStd_ByteArray::ChangeArray(const occ::handle<NCollection_HArray1<uint8_t>>& newArray,
                                     const bool               isCheckItems)
{

  int aLower    = newArray->Lower();
  int anUpper   = newArray->Upper();
  bool aDimEqual = false;
  int i;

  if (Lower() == aLower && Upper() == anUpper)
  {
    aDimEqual = true;
    if (isCheckItems)
    {
      bool isEqual = true;
      for (i = aLower; i <= anUpper; i++)
      {
        if (myValue->Value(i) != newArray->Value(i))
        {
          isEqual = false;
          break;
        }
      }
      if (isEqual)
        return;
    }
  }

  Backup();
  // Handles of myValue of current and backuped attributes will be different!
  if (myValue.IsNull() || !aDimEqual)
    myValue = new NCollection_HArray1<uint8_t>(aLower, anUpper);

  for (i = aLower; i <= anUpper; i++)
    myValue->SetValue(i, newArray->Value(i));
}

//=================================================================================================

const Standard_GUID& TDataStd_ByteArray::ID() const
{
  return myID;
}

//=================================================================================================

void TDataStd_ByteArray::SetID(const Standard_GUID& theGuid)
{
  if (myID == theGuid)
    return;
  Backup();
  myID = theGuid;
}

//=================================================================================================

void TDataStd_ByteArray::SetID()
{
  Backup();
  myID = GetID();
}

//=================================================================================================

occ::handle<TDF_Attribute> TDataStd_ByteArray::NewEmpty() const
{
  return new TDataStd_ByteArray();
}

//=================================================================================================

void TDataStd_ByteArray::Restore(const occ::handle<TDF_Attribute>& With)
{
  occ::handle<TDataStd_ByteArray> anArray = occ::down_cast<TDataStd_ByteArray>(With);
  if (!anArray->myValue.IsNull())
  {
    const NCollection_Array1<uint8_t>& with_array = anArray->myValue->Array1();
    int            lower = with_array.Lower(), i = lower, upper = with_array.Upper();
    myValue = new NCollection_HArray1<uint8_t>(lower, upper);
    for (; i <= upper; i++)
      myValue->SetValue(i, with_array.Value(i));
    myIsDelta = anArray->myIsDelta;
    myID      = anArray->ID();
  }
  else
    myValue.Nullify();
}

//=================================================================================================

void TDataStd_ByteArray::Paste(const occ::handle<TDF_Attribute>& Into,
                               const occ::handle<TDF_RelocationTable>&) const
{
  if (!myValue.IsNull())
  {
    occ::handle<TDataStd_ByteArray> anAtt = occ::down_cast<TDataStd_ByteArray>(Into);
    if (!anAtt.IsNull())
    {
      anAtt->ChangeArray(myValue, false);
      anAtt->SetDelta(myIsDelta);
      anAtt->SetID(myID);
    }
  }
}

//=================================================================================================

Standard_OStream& TDataStd_ByteArray::Dump(Standard_OStream& anOS) const
{
  anOS << "\nByteArray: ";
  char sguid[Standard_GUID_SIZE_ALLOC];
  myID.ToCString(sguid);
  anOS << sguid << std::endl;
  return anOS;
}

//=================================================================================================

occ::handle<TDF_DeltaOnModification> TDataStd_ByteArray::DeltaOnModification(
  const occ::handle<TDF_Attribute>& OldAttribute) const
{
  if (myIsDelta)
    return new TDataStd_DeltaOnModificationOfByteArray(
      occ::down_cast<TDataStd_ByteArray>(OldAttribute));
  else
    return new TDF_DefaultDeltaOnModification(OldAttribute);
}

//=================================================================================================

void TDataStd_ByteArray::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)

  if (!myValue.IsNull())
  {
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myValue->Lower())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myValue->Upper())

    for (NCollection_Array1<uint8_t>::Iterator aValueIt(myValue->Array1()); aValueIt.More();
         aValueIt.Next())
    {
      const uint8_t& aValue = aValueIt.Value();
      OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, aValue)
    }
  }
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIsDelta)
}
