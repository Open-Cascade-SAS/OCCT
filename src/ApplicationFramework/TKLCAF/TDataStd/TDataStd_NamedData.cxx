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

#include <TDataStd_NamedData.hxx>

#include <Standard_Dump.hxx>
#include <Standard_GUID.hxx>
#include <TCollection_ExtendedString.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <TDataStd_HDataMapOfStringByte.hxx>
#include <TDataStd_HDataMapOfStringHArray1OfInteger.hxx>
#include <TDataStd_HDataMapOfStringHArray1OfReal.hxx>
#include <TDataStd_HDataMapOfStringInteger.hxx>
#include <TDataStd_HDataMapOfStringReal.hxx>
#include <TDataStd_HDataMapOfStringString.hxx>

#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_NamedData, TDF_Attribute)

//=================================================================================================

const Standard_GUID& TDataStd_NamedData::GetID()
{
  static Standard_GUID TDataStd_NamedDataID("F170FD21-CBAE-4e7d-A4B4-0560A4DA2D16");
  return TDataStd_NamedDataID;
}

//=================================================================================================

TDataStd_NamedData::TDataStd_NamedData() = default;

//=================================================================================================

occ::handle<TDataStd_NamedData> TDataStd_NamedData::Set(const TDF_Label& label)
{
  occ::handle<TDataStd_NamedData> A;
  if (!label.FindAttribute(TDataStd_NamedData::GetID(), A))
  {
    A = new TDataStd_NamedData;
    label.AddAttribute(A);
  }
  return A;
}

//=================================================================================================

void TDataStd_NamedData::clear()
{
  myIntegers.Nullify();
  myReals.Nullify();
  myStrings.Nullify();
  myBytes.Nullify();
  myArraysOfIntegers.Nullify();
  myArraysOfReals.Nullify();
}

// Category: Integers

//=================================================================================================

bool TDataStd_NamedData::HasIntegers() const
{
  return !myIntegers.IsNull() && !myIntegers->Map().IsEmpty();
}

//=================================================================================================

bool TDataStd_NamedData::HasInteger(const TCollection_ExtendedString& theName) const
{
  if (!HasIntegers())
  {
    return false;
  }
  return myIntegers->Map().IsBound(theName);
}

//=================================================================================================

int TDataStd_NamedData::GetInteger(const TCollection_ExtendedString& theName)
{
  if (!HasIntegers())
  {
    NCollection_DataMap<TCollection_ExtendedString, int> aMap;
    myIntegers = new TDataStd_HDataMapOfStringInteger(aMap);
  }
  return myIntegers->Map()(theName);
}

//=================================================================================================

void TDataStd_NamedData::setInteger(const TCollection_ExtendedString& theName, const int theInteger)
{
  if (!HasIntegers())
  {
    NCollection_DataMap<TCollection_ExtendedString, int> aMap;
    myIntegers = new TDataStd_HDataMapOfStringInteger(aMap);
  }
  myIntegers->ChangeMap().Bind(theName, theInteger);
}

//=================================================================================================

void TDataStd_NamedData::SetInteger(const TCollection_ExtendedString& theName, const int theInteger)
{
  if (!HasIntegers())
  {
    NCollection_DataMap<TCollection_ExtendedString, int> aMap;
    myIntegers = new TDataStd_HDataMapOfStringInteger(aMap);
  }
  if (int* aValuePtr = myIntegers->ChangeMap().ChangeSeek(theName))
  {
    if (*aValuePtr != theInteger)
    {
      Backup();
      *aValuePtr = theInteger;
    }
  }
  else
  {
    Backup();
    myIntegers->ChangeMap().Bind(theName, theInteger);
  }
}

//=================================================================================================

const NCollection_DataMap<TCollection_ExtendedString, int>& TDataStd_NamedData::
  GetIntegersContainer()
{
  if (!HasIntegers())
  {
    NCollection_DataMap<TCollection_ExtendedString, int> aMap;
    myIntegers = new TDataStd_HDataMapOfStringInteger(aMap);
  }
  return myIntegers->Map();
}

//=================================================================================================

void TDataStd_NamedData::ChangeIntegers(
  const NCollection_DataMap<TCollection_ExtendedString, int>& theIntegers)
{
  if (!HasIntegers())
  {
    NCollection_DataMap<TCollection_ExtendedString, int> aMap;
    myIntegers = new TDataStd_HDataMapOfStringInteger(aMap);
  };
  if (&myIntegers->Map() == &theIntegers)
  {
    return;
  }
  Backup();
  myIntegers->ChangeMap().Assign(theIntegers);
}

// Category: Reals
//           =====

//=================================================================================================

bool TDataStd_NamedData::HasReals() const
{
  return !myReals.IsNull() && !myReals->Map().IsEmpty();
}

//=================================================================================================

bool TDataStd_NamedData::HasReal(const TCollection_ExtendedString& theName) const
{
  if (!HasReals())
  {
    return false;
  }
  return myReals->Map().IsBound(theName);
}

//=================================================================================================

double TDataStd_NamedData::GetReal(const TCollection_ExtendedString& theName)
{
  if (!HasReals())
  {
    NCollection_DataMap<TCollection_ExtendedString, double> aMap;
    myReals = new TDataStd_HDataMapOfStringReal(aMap);
  }
  return myReals->Map()(theName);
}

//=================================================================================================

void TDataStd_NamedData::setReal(const TCollection_ExtendedString& theName, const double theReal)
{
  if (!HasReals())
  {
    NCollection_DataMap<TCollection_ExtendedString, double> aMap;
    myReals = new TDataStd_HDataMapOfStringReal(aMap);
  }
  myReals->ChangeMap().Bind(theName, theReal);
}

//=================================================================================================

void TDataStd_NamedData::SetReal(const TCollection_ExtendedString& theName, const double theReal)
{
  if (!HasReals())
  {
    NCollection_DataMap<TCollection_ExtendedString, double> aMap;
    myReals = new TDataStd_HDataMapOfStringReal(aMap);
  }
  if (double* aValuePtr = myReals->ChangeMap().ChangeSeek(theName))
  {
    if (*aValuePtr != theReal)
    {
      Backup();
      *aValuePtr = theReal;
    }
  }
  else
  {
    myReals->ChangeMap().Bind(theName, theReal);
  }
}

//=================================================================================================

const NCollection_DataMap<TCollection_ExtendedString, double>& TDataStd_NamedData::
  GetRealsContainer()
{
  if (!HasReals())
  {
    NCollection_DataMap<TCollection_ExtendedString, double> aMap;
    myReals = new TDataStd_HDataMapOfStringReal(aMap);
  }
  return myReals->Map();
}

//=================================================================================================

void TDataStd_NamedData::ChangeReals(
  const NCollection_DataMap<TCollection_ExtendedString, double>& theReals)
{
  if (!HasReals())
  {
    NCollection_DataMap<TCollection_ExtendedString, double> aMap;
    myReals = new TDataStd_HDataMapOfStringReal(aMap);
  }
  if (&myReals->Map() == &theReals)
  {
    return;
  }
  Backup();
  myReals->ChangeMap().Assign(theReals);
}

// Category: Strings
//           =======

//=================================================================================================

bool TDataStd_NamedData::HasStrings() const
{
  return !myStrings.IsNull() && !myStrings->Map().IsEmpty();
}

//=================================================================================================

bool TDataStd_NamedData::HasString(const TCollection_ExtendedString& theName) const
{
  if (!HasStrings())
  {
    return false;
  }
  return myStrings->Map().IsBound(theName);
}

//=================================================================================================

const TCollection_ExtendedString& TDataStd_NamedData::GetString(
  const TCollection_ExtendedString& theName)
{
  if (!HasStrings())
  {
    NCollection_DataMap<TCollection_ExtendedString, TCollection_ExtendedString> aMap;
    myStrings = new TDataStd_HDataMapOfStringString(aMap);
  }
  return myStrings->Map()(theName);
}

//=================================================================================================

void TDataStd_NamedData::setString(const TCollection_ExtendedString& theName,
                                   const TCollection_ExtendedString& theString)
{
  if (!HasStrings())
  {
    NCollection_DataMap<TCollection_ExtendedString, TCollection_ExtendedString> aMap;
    myStrings = new TDataStd_HDataMapOfStringString(aMap);
  }

  myStrings->ChangeMap().Bind(theName, theString);
}

//=================================================================================================

void TDataStd_NamedData::SetString(const TCollection_ExtendedString& theName,
                                   const TCollection_ExtendedString& theString)
{
  if (!HasStrings())
  {
    NCollection_DataMap<TCollection_ExtendedString, TCollection_ExtendedString> aMap;
    myStrings = new TDataStd_HDataMapOfStringString(aMap);
  }

  if (TCollection_ExtendedString* aValuePtr = myStrings->ChangeMap().ChangeSeek(theName))
  {
    if (*aValuePtr != theString)
    {
      Backup();
      *aValuePtr = theString;
    }
  }
  else
  {
    Backup();
    myStrings->ChangeMap().Bind(theName, theString);
  }
}

//=================================================================================================

const NCollection_DataMap<TCollection_ExtendedString, TCollection_ExtendedString>&
  TDataStd_NamedData::GetStringsContainer()
{
  if (!HasStrings())
  {
    NCollection_DataMap<TCollection_ExtendedString, TCollection_ExtendedString> aMap;
    myStrings = new TDataStd_HDataMapOfStringString(aMap);
  }
  return myStrings->Map();
}

//=================================================================================================

void TDataStd_NamedData::ChangeStrings(
  const NCollection_DataMap<TCollection_ExtendedString, TCollection_ExtendedString>& theStrings)
{
  if (!HasStrings())
  {
    NCollection_DataMap<TCollection_ExtendedString, TCollection_ExtendedString> aMap;
    myStrings = new TDataStd_HDataMapOfStringString(aMap);
  }
  if (&myStrings->Map() == &theStrings)
  {
    return;
  }
  Backup();
  myStrings->ChangeMap().Assign(theStrings);
}

// Category: Bytes
//           =====

//=================================================================================================

bool TDataStd_NamedData::HasBytes() const
{
  return !myBytes.IsNull() && !myBytes->Map().IsEmpty();
}

//=================================================================================================

bool TDataStd_NamedData::HasByte(const TCollection_ExtendedString& theName) const
{
  if (!HasBytes())
  {
    return false;
  }
  return myBytes->Map().IsBound(theName);
}

//=================================================================================================

uint8_t TDataStd_NamedData::GetByte(const TCollection_ExtendedString& theName)
{
  if (!HasBytes())
  {
    NCollection_DataMap<TCollection_ExtendedString, uint8_t> aMap;
    myBytes = new TDataStd_HDataMapOfStringByte(aMap);
  }
  return myBytes->Map()(theName);
}

//=================================================================================================

void TDataStd_NamedData::setByte(const TCollection_ExtendedString& theName, const uint8_t theByte)
{
  if (!HasBytes())
  {
    NCollection_DataMap<TCollection_ExtendedString, uint8_t> aMap;
    myBytes = new TDataStd_HDataMapOfStringByte(aMap);
  }
  myBytes->ChangeMap().Bind(theName, theByte);
}

//=================================================================================================

void TDataStd_NamedData::SetByte(const TCollection_ExtendedString& theName, const uint8_t theByte)
{
  if (!HasBytes())
  {
    NCollection_DataMap<TCollection_ExtendedString, uint8_t> aMap;
    myBytes = new TDataStd_HDataMapOfStringByte(aMap);
  }

  if (uint8_t* aValuePtr = myBytes->ChangeMap().ChangeSeek(theName))
  {
    if (*aValuePtr != theByte)
    {
      Backup();
      *aValuePtr = theByte;
    }
  }
  else
  {
    Backup();
    myBytes->ChangeMap().Bind(theName, theByte);
  }
}

//=================================================================================================

const NCollection_DataMap<TCollection_ExtendedString, uint8_t>& TDataStd_NamedData::
  GetBytesContainer()
{
  if (!HasBytes())
  {
    NCollection_DataMap<TCollection_ExtendedString, uint8_t> aMap;
    myBytes = new TDataStd_HDataMapOfStringByte(aMap);
  }
  return myBytes->Map();
}

//=================================================================================================

void TDataStd_NamedData::ChangeBytes(
  const NCollection_DataMap<TCollection_ExtendedString, uint8_t>& theBytes)
{
  if (!HasBytes())
  {
    NCollection_DataMap<TCollection_ExtendedString, uint8_t> aMap;
    myBytes = new TDataStd_HDataMapOfStringByte(aMap);
  }
  if (&myBytes->Map() == &theBytes)
  {
    return;
  }
  Backup();
  myBytes->ChangeMap().Assign(theBytes);
}

// Category: Arrays of integers
//           ==================

//=================================================================================================

bool TDataStd_NamedData::HasArraysOfIntegers() const
{
  return !myArraysOfIntegers.IsNull() && !myArraysOfIntegers->Map().IsEmpty();
}

//=================================================================================================

bool TDataStd_NamedData::HasArrayOfIntegers(const TCollection_ExtendedString& theName) const
{
  if (!HasArraysOfIntegers())
  {
    return false;
  }
  return myArraysOfIntegers->Map().IsBound(theName);
}

//=================================================================================================

const occ::handle<NCollection_HArray1<int>>& TDataStd_NamedData::GetArrayOfIntegers(
  const TCollection_ExtendedString& theName)
{
  if (!HasArraysOfIntegers())
  {
    NCollection_DataMap<TCollection_ExtendedString, occ::handle<NCollection_HArray1<int>>> aMap;
    myArraysOfIntegers = new TDataStd_HDataMapOfStringHArray1OfInteger(aMap);
  }
  return myArraysOfIntegers->Map().Find(theName);
}

//=================================================================================================

void TDataStd_NamedData::setArrayOfIntegers(
  const TCollection_ExtendedString&            theName,
  const occ::handle<NCollection_HArray1<int>>& theArrayOfIntegers)
{
  if (!HasArraysOfIntegers())
  {
    NCollection_DataMap<TCollection_ExtendedString, occ::handle<NCollection_HArray1<int>>> aMap;
    myArraysOfIntegers = new TDataStd_HDataMapOfStringHArray1OfInteger(aMap);
  }

  occ::handle<NCollection_HArray1<int>> anArray;
  if (!theArrayOfIntegers.IsNull())
  {
    // deep copy of the array
    const int aLower = theArrayOfIntegers->Lower(), anUpper = theArrayOfIntegers->Upper();
    anArray = new NCollection_HArray1<int>(aLower, anUpper);
    for (int anIter = aLower; anIter <= anUpper; ++anIter)
    {
      anArray->SetValue(anIter, theArrayOfIntegers->Value(anIter));
    }
  }
  myArraysOfIntegers->ChangeMap().Bind(theName, anArray);
}

//=================================================================================================

const NCollection_DataMap<TCollection_ExtendedString, occ::handle<NCollection_HArray1<int>>>&
  TDataStd_NamedData::GetArraysOfIntegersContainer()
{
  if (!HasArraysOfIntegers())
  {
    NCollection_DataMap<TCollection_ExtendedString, occ::handle<NCollection_HArray1<int>>> aMap;
    myArraysOfIntegers = new TDataStd_HDataMapOfStringHArray1OfInteger(aMap);
  }
  return myArraysOfIntegers->Map();
}

//=================================================================================================

void TDataStd_NamedData::ChangeArraysOfIntegers(
  const NCollection_DataMap<TCollection_ExtendedString, occ::handle<NCollection_HArray1<int>>>&
    theIntegers)
{
  if (!HasArraysOfIntegers())
  {
    NCollection_DataMap<TCollection_ExtendedString, occ::handle<NCollection_HArray1<int>>> aMap;
    myArraysOfIntegers = new TDataStd_HDataMapOfStringHArray1OfInteger(aMap);
  }
  if (&myArraysOfIntegers->Map() == &theIntegers)
  {
    return;
  }
  Backup();
  myArraysOfIntegers->ChangeMap().Assign(theIntegers);
}

// Category: Arrays of reals
//           ===============

//=================================================================================================

bool TDataStd_NamedData::HasArraysOfReals() const
{
  return !myArraysOfReals.IsNull() && !myArraysOfReals->Map().IsEmpty();
}

//=================================================================================================

bool TDataStd_NamedData::HasArrayOfReals(const TCollection_ExtendedString& theName) const
{
  if (!HasArraysOfReals())
  {
    return false;
  }
  return myArraysOfReals->Map().IsBound(theName);
}

//=================================================================================================

const occ::handle<NCollection_HArray1<double>>& TDataStd_NamedData::GetArrayOfReals(
  const TCollection_ExtendedString& theName)
{
  if (!HasArraysOfReals())
  {
    NCollection_DataMap<TCollection_ExtendedString, occ::handle<NCollection_HArray1<double>>> aMap;
    myArraysOfReals = new TDataStd_HDataMapOfStringHArray1OfReal(aMap);
  }
  return myArraysOfReals->Map().Find(theName);
}

//=================================================================================================

void TDataStd_NamedData::setArrayOfReals(
  const TCollection_ExtendedString&               theName,
  const occ::handle<NCollection_HArray1<double>>& theArrayOfReals)
{
  if (!HasArraysOfReals())
  {
    NCollection_DataMap<TCollection_ExtendedString, occ::handle<NCollection_HArray1<double>>> aMap;
    myArraysOfReals = new TDataStd_HDataMapOfStringHArray1OfReal(aMap);
  }

  occ::handle<NCollection_HArray1<double>> anArray;
  if (!theArrayOfReals.IsNull())
  {
    // deep copy of the array
    const int aLower = theArrayOfReals->Lower(), anUpper = theArrayOfReals->Upper();
    anArray = new NCollection_HArray1<double>(aLower, anUpper);
    for (int anIter = aLower; anIter <= anUpper; ++anIter)
    {
      anArray->SetValue(anIter, theArrayOfReals->Value(anIter));
    }
  }
  myArraysOfReals->ChangeMap().Bind(theName, anArray);
}

//=================================================================================================

const NCollection_DataMap<TCollection_ExtendedString, occ::handle<NCollection_HArray1<double>>>&
  TDataStd_NamedData::GetArraysOfRealsContainer()
{
  if (!HasArraysOfReals())
  {
    NCollection_DataMap<TCollection_ExtendedString, occ::handle<NCollection_HArray1<double>>> aMap;
    myArraysOfReals = new TDataStd_HDataMapOfStringHArray1OfReal(aMap);
  }
  return myArraysOfReals->Map();
}

//=================================================================================================

void TDataStd_NamedData::ChangeArraysOfReals(
  const NCollection_DataMap<TCollection_ExtendedString, occ::handle<NCollection_HArray1<double>>>&
    theReals)
{
  if (!HasArraysOfReals())
  {
    NCollection_DataMap<TCollection_ExtendedString, occ::handle<NCollection_HArray1<double>>> aMap;
    myArraysOfReals = new TDataStd_HDataMapOfStringHArray1OfReal(aMap);
  }
  if (&myArraysOfReals->Map() == &theReals)
  {
    return;
  }
  Backup();
  myArraysOfReals->ChangeMap().Assign(theReals);
}

//=================================================================================================

const Standard_GUID& TDataStd_NamedData::ID() const
{
  return GetID();
}

//=================================================================================================

occ::handle<TDF_Attribute> TDataStd_NamedData::NewEmpty() const
{
  return new TDataStd_NamedData();
}

//=================================================================================================

void TDataStd_NamedData::Restore(const occ::handle<TDF_Attribute>& With)
{

  occ::handle<TDataStd_NamedData> ND = occ::down_cast<TDataStd_NamedData>(With);
  if (ND.IsNull())
  {
    return;
  }
  // Integers
  if (!ND->GetIntegersContainer().IsEmpty())
  {
    if (!HasIntegers())
    {
      NCollection_DataMap<TCollection_ExtendedString, int> aMap;
      myIntegers = new TDataStd_HDataMapOfStringInteger(aMap);
    }
    myIntegers->ChangeMap().Assign(ND->GetIntegersContainer());
  }

  // Reals
  if (!ND->GetRealsContainer().IsEmpty())
  {
    if (!HasReals())
    {
      NCollection_DataMap<TCollection_ExtendedString, double> aMap;
      myReals = new TDataStd_HDataMapOfStringReal(aMap);
    }
    myReals->ChangeMap().Assign(ND->GetRealsContainer());
  }

  // Strings
  if (!ND->GetStringsContainer().IsEmpty())
  {
    if (!HasStrings())
    {
      NCollection_DataMap<TCollection_ExtendedString, TCollection_ExtendedString> aMap;
      myStrings = new TDataStd_HDataMapOfStringString(aMap);
    }
    myStrings->ChangeMap().Assign(ND->GetStringsContainer());
  }

  // Bytes
  if (!ND->GetBytesContainer().IsEmpty())
  {
    if (!HasBytes())
    {
      NCollection_DataMap<TCollection_ExtendedString, uint8_t> aMap;
      myBytes = new TDataStd_HDataMapOfStringByte(aMap);
    }
    myBytes->ChangeMap().Assign(ND->GetBytesContainer());
  }

  // Arrays of integers
  if (!ND->GetArraysOfIntegersContainer().IsEmpty())
  {
    if (!HasArraysOfIntegers())
    {
      NCollection_DataMap<TCollection_ExtendedString, occ::handle<NCollection_HArray1<int>>> aMap;
      myArraysOfIntegers = new TDataStd_HDataMapOfStringHArray1OfInteger(aMap);
    }
    NCollection_DataMap<TCollection_ExtendedString, occ::handle<NCollection_HArray1<int>>>::Iterator
      itr(ND->GetArraysOfIntegersContainer());
    for (; itr.More(); itr.Next())
    {
      // Deep copy of the arrays
      const occ::handle<NCollection_HArray1<int>>& ints = itr.Value();
      occ::handle<NCollection_HArray1<int>>        copied_ints;
      if (!ints.IsNull())
      {
        int lower = ints->Lower(), i = lower, upper = ints->Upper();
        copied_ints = new NCollection_HArray1<int>(lower, upper);
        for (; i <= upper; i++)
        {
          copied_ints->SetValue(i, ints->Value(i));
        }
      }
      myArraysOfIntegers->ChangeMap().Bind(itr.Key(), copied_ints);
    }
  }

  // Arrays of realss
  if (!ND->GetArraysOfRealsContainer().IsEmpty())
  {
    if (!HasArraysOfReals())
    {
      NCollection_DataMap<TCollection_ExtendedString, occ::handle<NCollection_HArray1<double>>>
        aMap;
      myArraysOfReals = new TDataStd_HDataMapOfStringHArray1OfReal(aMap);
    }
    NCollection_DataMap<TCollection_ExtendedString,
                        occ::handle<NCollection_HArray1<double>>>::Iterator
      itr(ND->GetArraysOfRealsContainer());
    for (; itr.More(); itr.Next())
    {
      // Deep copy of the arrays
      const occ::handle<NCollection_HArray1<double>>& dbls = itr.Value();
      occ::handle<NCollection_HArray1<double>>        copied_dbls;
      if (!dbls.IsNull())
      {
        int lower = dbls->Lower(), i = lower, upper = dbls->Upper();
        copied_dbls = new NCollection_HArray1<double>(lower, upper);
        for (; i <= upper; i++)
        {
          copied_dbls->SetValue(i, dbls->Value(i));
        }
      }
      myArraysOfReals->ChangeMap().Bind(itr.Key(), copied_dbls);
    }
  }
}

//=================================================================================================

void TDataStd_NamedData::Paste(const occ::handle<TDF_Attribute>& Into,
                               const occ::handle<TDF_RelocationTable>&) const
{
  occ::handle<TDataStd_NamedData> ND = occ::down_cast<TDataStd_NamedData>(Into);
  if (ND.IsNull())
  {
    return;
  }

  // Integers
  if (HasIntegers() && !myIntegers->Map().IsEmpty())
  {
    if (!ND->HasIntegers())
    {
      NCollection_DataMap<TCollection_ExtendedString, int> aMap;
      ND->myIntegers = new TDataStd_HDataMapOfStringInteger(aMap);
    };
    ND->myIntegers->ChangeMap().Assign(myIntegers->Map());
  }

  // Reals
  if (HasReals() && !myReals->Map().IsEmpty())
  {
    if (!ND->HasReals())
    {
      NCollection_DataMap<TCollection_ExtendedString, double> aMap;
      ND->myReals = new TDataStd_HDataMapOfStringReal(aMap);
    };
    ND->myReals->ChangeMap().Assign(myReals->Map());
  }

  // Strings
  if (HasStrings() && !myStrings->Map().IsEmpty())
  {
    if (!ND->HasStrings())
    {
      NCollection_DataMap<TCollection_ExtendedString, TCollection_ExtendedString> aMap;
      ND->myStrings = new TDataStd_HDataMapOfStringString(aMap);
    };
    ND->myStrings->ChangeMap().Assign(myStrings->Map());
  }

  // Bytes
  if (HasBytes() && !myBytes->Map().IsEmpty())
  {
    if (!ND->HasBytes())
    {
      NCollection_DataMap<TCollection_ExtendedString, uint8_t> aMap;
      ND->myBytes = new TDataStd_HDataMapOfStringByte(aMap);
    };
    ND->myBytes->ChangeMap().Assign(myBytes->Map());
  }

  // Arrays of integers
  if (HasArraysOfIntegers() && !myArraysOfIntegers->Map().IsEmpty())
  {
    if (!ND->HasArraysOfIntegers())
    {
      NCollection_DataMap<TCollection_ExtendedString, occ::handle<NCollection_HArray1<int>>> aMap;
      ND->myArraysOfIntegers = new TDataStd_HDataMapOfStringHArray1OfInteger(aMap);
    }

    NCollection_DataMap<TCollection_ExtendedString, occ::handle<NCollection_HArray1<int>>>::Iterator
      itr(myArraysOfIntegers->Map());
    for (; itr.More(); itr.Next())
    {
      const occ::handle<NCollection_HArray1<int>>& ints = itr.Value();
      occ::handle<NCollection_HArray1<int>>        copied_ints;
      if (!ints.IsNull())
      {
        int lower = ints->Lower(), i = lower, upper = ints->Upper();
        copied_ints = new NCollection_HArray1<int>(lower, upper);
        for (; i <= upper; i++)
        {
          copied_ints->SetValue(i, ints->Value(i));
        }
      }
      ND->myArraysOfIntegers->ChangeMap().Bind(itr.Key(), copied_ints);
    }
  }

  // Arrays of reals
  if (HasArraysOfReals() && !myArraysOfReals->Map().IsEmpty())
  {
    if (!ND->HasArraysOfReals())
    {
      NCollection_DataMap<TCollection_ExtendedString, occ::handle<NCollection_HArray1<double>>>
        aMap;
      ND->myArraysOfReals = new TDataStd_HDataMapOfStringHArray1OfReal(aMap);
    }
    NCollection_DataMap<TCollection_ExtendedString,
                        occ::handle<NCollection_HArray1<double>>>::Iterator itr(myArraysOfReals
                                                                                  ->Map());
    for (; itr.More(); itr.Next())
    {
      const occ::handle<NCollection_HArray1<double>>& dbls = itr.Value();
      occ::handle<NCollection_HArray1<double>>        copied_dbls;
      if (!dbls.IsNull())
      {
        int lower = dbls->Lower(), i = lower, upper = dbls->Upper();
        copied_dbls = new NCollection_HArray1<double>(lower, upper);
        for (; i <= upper; i++)
        {
          copied_dbls->SetValue(i, dbls->Value(i));
        }
      }
      ND->myArraysOfReals->ChangeMap().Bind(itr.Key(), copied_dbls);
    }
  }
}

//=================================================================================================

Standard_OStream& TDataStd_NamedData::Dump(Standard_OStream& anOS) const
{
  anOS << "NamedData: ";
  anOS << "\tIntegers = " << (HasIntegers() ? myIntegers->Map().Extent() : 0);
  anOS << "\tReals = " << (HasReals() ? myReals->Map().Extent() : 0);
  anOS << "\tStrings = " << (HasStrings() ? myStrings->Map().Extent() : 0);
  anOS << "\tBytes = " << (HasBytes() ? myBytes->Map().Extent() : 0);
  anOS << "\tArraysOfIntegers = "
       << (HasArraysOfIntegers() ? myArraysOfIntegers->Map().Extent() : 0);
  anOS << "\tArraysOfReals = " << (HasArraysOfReals() ? myArraysOfReals->Map().Extent() : 0);
  return anOS;
}

//=================================================================================================

void TDataStd_NamedData::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)

  if (!myIntegers.IsNull())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIntegers->Map().Size())
  if (!myReals.IsNull())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myReals->Map().Size())
  if (!myStrings.IsNull())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myStrings->Map().Size())
  if (!myBytes.IsNull())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myBytes->Map().Size())
  if (!myArraysOfIntegers.IsNull())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myArraysOfIntegers->Map().Size())
  if (!myArraysOfReals.IsNull())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myArraysOfReals->Map().Size())
}
