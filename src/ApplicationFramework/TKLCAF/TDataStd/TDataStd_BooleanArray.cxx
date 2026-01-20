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

#include <TDataStd_BooleanArray.hxx>

#include <Standard_Dump.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_BooleanArray, TDF_Attribute)

static int DegreeOf2(const int degree)
{
  switch (degree)
  {
    case 0:
      return 1;
    case 1:
      return 2;
    case 2:
      return 4;
    case 3:
      return 8;
    case 4:
      return 16;
    case 5:
      return 32;
    case 6:
      return 64;
    case 7:
      return 128;
    case 8:
      return 256;
  }
  return -1;
}

//=================================================================================================

const Standard_GUID& TDataStd_BooleanArray::GetID()
{
  static Standard_GUID TDataStd_BooleanArrayID("C7E98E54-B5EA-4aa9-AC99-9164EBD07F10");
  return TDataStd_BooleanArrayID;
}

//=======================================================================
// function : SetAttr
// purpose  : Implements Set functionality
//=======================================================================
static occ::handle<TDataStd_BooleanArray> SetAttr(const TDF_Label&       label,
                                             const int lower,
                                             const int upper,
                                             const Standard_GUID&   theGuid)
{
  occ::handle<TDataStd_BooleanArray> A;
  if (!label.FindAttribute(theGuid, A))
  {
    A = new TDataStd_BooleanArray;
    A->SetID(theGuid);
    A->Init(lower, upper);
    label.AddAttribute(A);
  }
  else if (lower != A->Lower() || upper != A->Upper())
  {
    A->Init(lower, upper);
  }
  return A;
}

//=================================================================================================

TDataStd_BooleanArray::TDataStd_BooleanArray()
    : myID(GetID())
{
}

//=================================================================================================

void TDataStd_BooleanArray::Init(const int lower, const int upper)
{
  Standard_RangeError_Raise_if(upper < lower, "TDataStd_BooleanArray::Init");
  Backup();
  myLower  = lower;
  myUpper  = upper;
  myValues = new NCollection_HArray1<uint8_t>(0, Length() >> 3, 0 /*initialize to FALSE*/);
}

//=================================================================================================

occ::handle<TDataStd_BooleanArray> TDataStd_BooleanArray::Set(const TDF_Label&       label,
                                                         const int lower,
                                                         const int upper)
{
  return SetAttr(label, lower, upper, GetID());
}

//=======================================================================
// function : Set
// purpose  : Set user defined attribute with specific ID
//=======================================================================
occ::handle<TDataStd_BooleanArray> TDataStd_BooleanArray::Set(const TDF_Label&       label,
                                                         const Standard_GUID&   theGuid,
                                                         const int lower,
                                                         const int upper)
{
  return SetAttr(label, lower, upper, theGuid);
}

//=================================================================================================

void TDataStd_BooleanArray::SetValue(const int index, const bool value)
{

  if (myValues.IsNull())
    return;
  int byte_index = (index - myLower) >> 3;
  int degree     = index - (byte_index << 3) - myLower;
  int byte_value = DegreeOf2(degree);

  if ((value != 0) == ((myValues->Value(byte_index) & byte_value) > 0))
    return;

  Backup();

  if (value)
  {
    myValues->ChangeValue(byte_index) |= byte_value;
  }
  else
  {
    myValues->ChangeValue(byte_index) ^= byte_value;
  }
}

//=================================================================================================

bool TDataStd_BooleanArray::Value(const int index) const
{
  if (myValues.IsNull())
    return false;
  if (index < myLower || index > myUpper)
    return false;

  int byte_index = (index - myLower) >> 3;
  int degree     = index - (byte_index << 3) - myLower;
  int byte_value = DegreeOf2(degree);

  return (myValues->Value(byte_index) & byte_value) > 0;
}

//=================================================================================================

int TDataStd_BooleanArray::Lower(void) const
{
  return myLower;
}

//=================================================================================================

int TDataStd_BooleanArray::Upper(void) const
{
  return myUpper;
}

//=================================================================================================

int TDataStd_BooleanArray::Length(void) const
{
  return myUpper - myLower + 1;
}

//=================================================================================================

const occ::handle<NCollection_HArray1<uint8_t>>& TDataStd_BooleanArray::InternalArray() const
{
  return myValues;
}

//=================================================================================================

void TDataStd_BooleanArray::SetInternalArray(const occ::handle<NCollection_HArray1<uint8_t>>& values)
{
  myValues = values;
}

//=================================================================================================

const Standard_GUID& TDataStd_BooleanArray::ID() const
{
  return myID;
}

//=================================================================================================

void TDataStd_BooleanArray::SetID(const Standard_GUID& theGuid)
{
  if (myID == theGuid)
    return;
  Backup();
  myID = theGuid;
}

//=================================================================================================

void TDataStd_BooleanArray::SetID()
{
  Backup();
  myID = GetID();
}

//=================================================================================================

occ::handle<TDF_Attribute> TDataStd_BooleanArray::NewEmpty() const
{
  return new TDataStd_BooleanArray();
}

//=================================================================================================

void TDataStd_BooleanArray::Restore(const occ::handle<TDF_Attribute>& With)
{
  occ::handle<TDataStd_BooleanArray> anArray = occ::down_cast<TDataStd_BooleanArray>(With);
  if (!anArray->myValues.IsNull())
  {
    myLower                     = anArray->Lower();
    myUpper                     = anArray->Upper();
    int byte_upper = Length() >> 3;
    myValues = new NCollection_HArray1<uint8_t>(0, byte_upper, 0 /*initialize to FALSE*/);
    const NCollection_Array1<uint8_t>& with_array = anArray->myValues->Array1();
    for (int i = 0; i <= byte_upper; i++)
    {
      myValues->SetValue(i, with_array.Value(i));
    }
    myID = anArray->ID();
  }
  else
  {
    myValues.Nullify();
  }
}

//=================================================================================================

void TDataStd_BooleanArray::Paste(const occ::handle<TDF_Attribute>& Into,
                                  const occ::handle<TDF_RelocationTable>&) const
{
  if (!myValues.IsNull())
  {
    occ::handle<TDataStd_BooleanArray> anArray = occ::down_cast<TDataStd_BooleanArray>(Into);
    if (!anArray.IsNull())
    {
      anArray->Init(myLower, myUpper);
      for (int i = myLower; i <= myUpper; i++)
      {
        anArray->SetValue(i, Value(i));
      }
    }
    anArray->SetID(myID);
  }
}

//=================================================================================================

Standard_OStream& TDataStd_BooleanArray::Dump(Standard_OStream& anOS) const
{
  anOS << "\nBooleanArray: ";
  char sguid[Standard_GUID_SIZE_ALLOC];
  myID.ToCString(sguid);
  anOS << sguid;
  anOS << std::endl;
  return anOS;
}

//=================================================================================================

void TDataStd_BooleanArray::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)

  if (!myValues.IsNull())
  {
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myValues->Lower())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myValues->Upper())

    for (NCollection_Array1<uint8_t>::Iterator aValueIt(myValues->Array1()); aValueIt.More();
         aValueIt.Next())
    {
      const uint8_t& aValue = aValueIt.Value();
      OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, aValue)
    }
  }
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myLower)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myUpper)
}
