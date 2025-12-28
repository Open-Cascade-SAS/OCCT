// Created on: 2007-07-02
// Created by: Sergey ZARITCHNY
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

#include <BinMDataStd_NamedDataDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDataStd_NamedData.hxx>
#include <TDF_Attribute.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDataStd_NamedDataDriver, BinMDF_ADriver)

//=================================================================================================

BinMDataStd_NamedDataDriver::BinMDataStd_NamedDataDriver(
  const occ::handle<Message_Messenger>& theMsgDriver)
    : BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(TDataStd_NamedData)->Name())
{
}

//=================================================================================================

occ::handle<TDF_Attribute> BinMDataStd_NamedDataDriver::NewEmpty() const
{
  return new TDataStd_NamedData();
}

//=======================================================================
// function : Paste
// purpose  : persistent -> transient (retrieve)
//=======================================================================
bool BinMDataStd_NamedDataDriver::Paste(const BinObjMgt_Persistent&       theSource,
                                        const occ::handle<TDF_Attribute>& theTarget,
                                        BinObjMgt_RRelocationTable&) const
{

  occ::handle<TDataStd_NamedData> T = occ::down_cast<TDataStd_NamedData>(theTarget);
  if (T.IsNull())
    return false;
  int aLower, anUpper, i;
  if (!(theSource >> aLower >> anUpper))
    return false;
  //  const int aLength = anUpper - aLower + 1;
  if (anUpper < aLower)
    return false;
  if (anUpper | aLower)
  {
    NCollection_DataMap<TCollection_ExtendedString, int> anIntegers;
    for (i = aLower; i <= anUpper; i++)
    {
      TCollection_ExtendedString aKey;
      int                        aValue;
      if (!(theSource >> aKey >> aValue))
        return false;
      anIntegers.Bind(aKey, aValue);
    }
    T->ChangeIntegers(anIntegers);
  }

  if (!(theSource >> aLower >> anUpper))
    return false;
  if (anUpper < aLower)
    return false;
  if (anUpper | aLower)
  {
    NCollection_DataMap<TCollection_ExtendedString, double> aReals;
    for (i = aLower; i <= anUpper; i++)
    {
      TCollection_ExtendedString aKey;
      double                     aValue;
      if (!(theSource >> aKey >> aValue))
        return false;
      aReals.Bind(aKey, aValue);
    }
    T->ChangeReals(aReals);
  }

  // strings
  if (!(theSource >> aLower >> anUpper))
    return false;
  if (anUpper < aLower)
    return false;
  if (anUpper | aLower)
  {
    NCollection_DataMap<TCollection_ExtendedString, TCollection_ExtendedString> aStrings;
    for (i = aLower; i <= anUpper; i++)
    {
      TCollection_ExtendedString aKey;
      TCollection_ExtendedString aValue;
      if (!(theSource >> aKey >> aValue))
        return false;
      aStrings.Bind(aKey, aValue);
    }
    T->ChangeStrings(aStrings);
  }

  // Bytes
  if (!(theSource >> aLower >> anUpper))
    return false;
  if (anUpper < aLower)
    return false;
  if (anUpper | aLower)
  {
    NCollection_DataMap<TCollection_ExtendedString, uint8_t> aBytes;
    for (i = aLower; i <= anUpper; i++)
    {
      TCollection_ExtendedString aKey;
      uint8_t                    aValue;
      if (!(theSource >> aKey >> aValue))
        return false;
      aBytes.Bind(aKey, (uint8_t)aValue);
    }
    T->ChangeBytes(aBytes);
  }

  // arrays of integers
  if (!(theSource >> aLower >> anUpper))
    return false;
  if (anUpper < aLower)
    return false;
  bool aResult = false;
  if (anUpper | aLower)
  {
    NCollection_DataMap<TCollection_ExtendedString, occ::handle<NCollection_HArray1<int>>>
      anIntArrays;
    for (i = aLower; i <= anUpper; i++)
    {
      TCollection_ExtendedString aKey;
      if (!(theSource >> aKey))
        return false;
      int low, up;
      if (!(theSource >> low >> up))
        return false;
      if (up < low)
        return false;
      if (up | low)
      {
        occ::handle<NCollection_HArray1<int>> aTargetArray = new NCollection_HArray1<int>(low, up);
        if (!theSource.GetIntArray(&(aTargetArray->ChangeArray1())(low), up - low + 1))
          return false;

        bool Ok = anIntArrays.Bind(aKey, aTargetArray);
        aResult |= Ok;
      }
    }
    if (aResult)
      T->ChangeArraysOfIntegers(anIntArrays);
  }

  // arrays of reals
  if (!(theSource >> aLower >> anUpper))
    return false;
  if (anUpper < aLower)
    return false;
  aResult = false;
  if (anUpper | aLower)
  {
    NCollection_DataMap<TCollection_ExtendedString, occ::handle<NCollection_HArray1<double>>>
      aRealArrays;
    for (i = aLower; i <= anUpper; i++)
    {
      TCollection_ExtendedString aKey;
      if (!(theSource >> aKey))
        return false;
      int low, up;
      if (!(theSource >> low >> up))
        return false;
      if (up < low)
        return false;
      if (low | up)
      {
        occ::handle<NCollection_HArray1<double>> aTargetArray =
          new NCollection_HArray1<double>(low, up);
        if (!theSource.GetRealArray(&(aTargetArray->ChangeArray1())(low), up - low + 1))
          return false;
        bool Ok = aRealArrays.Bind(aKey, aTargetArray);
        aResult |= Ok;
      }
    }
    if (aResult)
      T->ChangeArraysOfReals(aRealArrays);
  }
  return true;
}

//=======================================================================
// function : Paste
// purpose  : transient -> persistent (store)
//=======================================================================
void BinMDataStd_NamedDataDriver::Paste(
  const occ::handle<TDF_Attribute>& theSource,
  BinObjMgt_Persistent&             theTarget,
  NCollection_IndexedMap<occ::handle<Standard_Transient>>&) const
{
  occ::handle<TDataStd_NamedData> S = occ::down_cast<TDataStd_NamedData>(theSource);
  if (S.IsNull())
    return;
  //  int i=0;

  S->LoadDeferredData();
  if (S->HasIntegers() && !S->GetIntegersContainer().IsEmpty())
  {
    theTarget.PutInteger(1) << S->GetIntegersContainer().Extent(); // dim
    NCollection_DataMap<TCollection_ExtendedString, int>::Iterator itr(S->GetIntegersContainer());
    for (; itr.More(); itr.Next())
    {
      theTarget << itr.Key() << itr.Value(); // key - value;
    }
  }
  else
  {
    theTarget.PutInteger(0).PutInteger(0);
  }

  if (S->HasReals() && !S->GetRealsContainer().IsEmpty())
  {
    theTarget.PutInteger(1) << S->GetRealsContainer().Extent();
    NCollection_DataMap<TCollection_ExtendedString, double>::Iterator itr(S->GetRealsContainer());
    for (; itr.More(); itr.Next())
    {
      theTarget << itr.Key() << itr.Value();
    }
  }
  else
  {
    theTarget.PutInteger(0).PutInteger(0);
  }

  if (S->HasStrings() && !S->GetStringsContainer().IsEmpty())
  {
    theTarget.PutInteger(1) << S->GetStringsContainer().Extent();
    NCollection_DataMap<TCollection_ExtendedString, TCollection_ExtendedString>::Iterator itr(
      S->GetStringsContainer());
    for (; itr.More(); itr.Next())
    {
      theTarget << itr.Key() << itr.Value();
    }
  }
  else
  {
    theTarget.PutInteger(0).PutInteger(0);
  }

  if (S->HasBytes() && !S->GetBytesContainer().IsEmpty())
  {
    theTarget.PutInteger(1) << S->GetBytesContainer().Extent();
    NCollection_DataMap<TCollection_ExtendedString, uint8_t>::Iterator itr(S->GetBytesContainer());
    for (; itr.More(); itr.Next())
    {
      theTarget << itr.Key() << (uint8_t)itr.Value();
    }
  }
  else
  {
    theTarget.PutInteger(0).PutInteger(0);
  }

  if (S->HasArraysOfIntegers() && !S->GetArraysOfIntegersContainer().IsEmpty())
  {
    theTarget.PutInteger(1) << S->GetArraysOfIntegersContainer().Extent();
    NCollection_DataMap<TCollection_ExtendedString, occ::handle<NCollection_HArray1<int>>>::Iterator
      itr(S->GetArraysOfIntegersContainer());
    for (; itr.More(); itr.Next())
    {
      theTarget << itr.Key(); // key
      const NCollection_Array1<int>& anArr1 = itr.Value()->Array1();
      theTarget << anArr1.Lower() << anArr1.Upper(); // value Arr1 dimensions
      int* aPtr = (int*)&anArr1(anArr1.Lower());
      theTarget.PutIntArray(aPtr, anArr1.Length());
    }
  }
  else
  {
    theTarget.PutInteger(0).PutInteger(0);
  }

  if (S->HasArraysOfReals() && !S->GetArraysOfRealsContainer().IsEmpty())
  {
    theTarget.PutInteger(1) << S->GetArraysOfRealsContainer().Extent(); // dim
    NCollection_DataMap<TCollection_ExtendedString,
                        occ::handle<NCollection_HArray1<double>>>::Iterator
      itr(S->GetArraysOfRealsContainer());
    for (; itr.More(); itr.Next())
    {
      theTarget << itr.Key(); // key
      const NCollection_Array1<double>& anArr1 = itr.Value()->Array1();
      theTarget << anArr1.Lower() << anArr1.Upper(); // value Arr1 dimensions
      double* aPtr = (double*)&anArr1(anArr1.Lower());
      theTarget.PutRealArray(aPtr, anArr1.Length());
    }
  }
  else
  {
    theTarget.PutInteger(0).PutInteger(0);
  }
}
