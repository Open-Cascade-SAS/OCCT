// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <StdLPersistent_NamedData.hxx>

#include <TCollection_ExtendedString.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

static const TCollection_ExtendedString& String(occ::handle<StdObjMgt_Persistent> theValue)
{
  if (theValue)
    return theValue->ExtString()->String();

  return TCollection_ExtendedString::EmptyString();
}

template <class HArray>
static typename HArray::ArrayHandle Array(occ::handle<StdObjMgt_Persistent> theValue)
{
  occ::handle<HArray> anArray = occ::down_cast<HArray>(theValue);
  return anArray ? anArray->Array() : NULL;
}

//=======================================================================
// function : Import
// purpose  : Import transient attribute from the persistent data
//=======================================================================
void StdLPersistent_NamedData::Import(const occ::handle<TDataStd_NamedData>& theAttribute) const
{
  if (myDimensions.IsNull())
    return;

  if (myInts)
  {
    NCollection_DataMap<TCollection_ExtendedString, int> aMap;
    for (int i = lower(0); i <= upper(0); i++)
      aMap.Bind(myInts.Key(i), myInts.Value(i));

    theAttribute->ChangeIntegers(aMap);
  }

  if (myReals)
  {
    NCollection_DataMap<TCollection_ExtendedString, double> aMap;
    for (int i = lower(1); i <= upper(1); i++)
      aMap.Bind(myReals.Key(i), myReals.Value(i));

    theAttribute->ChangeReals(aMap);
  }

  if (myStrings)
  {
    NCollection_DataMap<TCollection_ExtendedString, TCollection_ExtendedString> aMap;
    for (int i = lower(2); i <= upper(2); i++)
      aMap.Bind(myStrings.Key(i), String(myStrings.Value(i)));

    theAttribute->ChangeStrings(aMap);
  }

  if (myBytes)
  {
    NCollection_DataMap<TCollection_ExtendedString, uint8_t> aMap;
    for (int i = lower(3); i <= upper(3); i++)
      aMap.Bind(myBytes.Key(i), myBytes.Value(i));

    theAttribute->ChangeBytes(aMap);
  }

  if (myIntArrays)
  {
    NCollection_DataMap<TCollection_ExtendedString, occ::handle<NCollection_HArray1<int>>> aMap;
    for (int i = lower(4); i <= upper(4); i++)
      aMap.Bind(myIntArrays.Key(i), Array<StdLPersistent_HArray1::Integer>(myIntArrays.Value(i)));

    theAttribute->ChangeArraysOfIntegers(aMap);
  }

  if (myRealArrays)
  {
    NCollection_DataMap<TCollection_ExtendedString, occ::handle<NCollection_HArray1<double>>> aMap;
    for (int i = lower(5); i <= upper(5); i++)
      aMap.Bind(myRealArrays.Key(i), Array<StdLPersistent_HArray1::Real>(myRealArrays.Value(i)));

    theAttribute->ChangeArraysOfReals(aMap);
  }
}

int StdLPersistent_NamedData::lower(int theIndex) const
{
  const occ::handle<NCollection_HArray2<int>>& aDimensions = myDimensions->Array();
  return aDimensions->Value(aDimensions->LowerRow() + theIndex, aDimensions->LowerCol());
}

int StdLPersistent_NamedData::upper(int theIndex) const
{
  const occ::handle<NCollection_HArray2<int>>& aDimensions = myDimensions->Array();
  return aDimensions->Value(aDimensions->LowerRow() + theIndex, aDimensions->UpperCol());
}
